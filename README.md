# Money

`io1::money` is a header-only c++ 20 class that holds money amounts in the range  -9,223,372,036,854,775,808  +9,223,372,036,854,775,807. Amounts are intended to be expressed in the lowest subdivision allowed by a currency (eg. cents for USD), hence the integer base type. Localized formatting is supported by the standard `moneypunct`facet. Arithmetic operations involving `io1::money` instances and integers are exact (yet subject to overflow). Arithmetic operations with floats are rounded towards the nearest even integer (aka banker’s rounding).

# Rationale

An amount like \$0.10 cannot be represented by a float because of the involved loss of precision that would make the following operation unbalanced: \$1.00≠\$0.10+\$0.10… (ten times). Besides, not all currencies have a 1/100 subdivision[^dinar] or even a decimal subdivision[^ougiya]. As a result, money amounts are stored as plain integer values and formatting them with the correct currency / sub-currency format is left to the user of the class. For example, the value 12550 stored in a `io1::money` instance may be formatted as $125.50, 12.550 DT or 2510 UM depending on the locale context. The `io1` namespace provides overloads to `std::put_money` and `std::get_money` into order to format `io1::money` instances with the currently imbued `std::moneypunct` facet.

To prevent any unexpected rounding issue, it is not possible to construct a `io1::money` instance from a floating-point type. Multiplication and division by a floating-point type are provided and round to the nearest even integer. They are intended to support operations like percentages or loan interest rate calculations.

Integer and `io1::money` arithmetic is exact but may overflow in the same conditions as any integers. Integer division may throw an exception if the result is not exact.

[^dinar]:  In Tunisia 1 TND equals 1000 millimes.
[^ougiya]: In Mauritania 1 MRU equals 5 khoums.

# References

`io1::money` is a trivial class (`std::is_trivial_v<io1::money> == true`) and has standard layout (`std::is_standard_layout_v<io1::money> == true`).

## Traits

```cpp
io1::money::value_type;
```

The underlying type used to store amounts.

## Constructors

```cpp
io1::money::money() noexcept; (1)
constexpr io1::money::money(io1::money const & o) noexcept; (2)
template<std::integral T> explicit constexpr io1::money(T amount) noexcept; (3)
template<char... STR> constexpr io1::money operator ""_money() noexcept; (4)
```

(1)    Create an uninitialized instance. Such instances are meant to be initialized by copy assignment. Before they are they can only be destructed. Any other use of a `void` constructed `io1::money` has undefined behavior.

(2)    Create an instance which amount is the same as `o`.

(3)    Create an instance which amount is `amount`. If `amount` overflows the capacity of `io1::money` then it has undefined behavior.

(4)    If provided with an integer number, it behaves like (3) except that overflows are diagnosed with a compilation failure.
If provided with a float number, it deduces the lowest currency subdivision amount by ignoring the decimal separator (see the example below).

### Example

```cpp
static_assert(io1::money(120) == 120_money, "Assuming lowest subdivision.");
static_assert(120_money == 1.20_money,      "Assuming 1/100th subdivision.");
static_assert(12_money == 1.2_money,        "Assuming 1/10th subdivision.");
static_assert(102_money == 1.02_money,      "Assuming 1/100th subdivision.");
static_assert(1022_money == 1.022_money,    "Assuming 1/1000th subdivision.");
```

## Observers

```cpp
[[nodiscard]] constexpr io1::money::value_type const & io1::money::data() const noexcept;
```

Returns the amount stored by the instance.

## Operator Overloads

### Increment / Decrement

```cpp
[[nodiscard]] constexpr io1::money io1::money::operator++(int ignored) noexcept; (1)
[[nodiscard]] constexpr io1::money io1::money::operator--(int ignored) noexcept; (2)
constexpr io1::money & io1::money::operator++() noexcept; (3)
constexpr io1::money & io1::money::operator--() noexcept; (4)
```

(1, 2)    Post increment/decrement operator. The `int` argument is ignored. Overflow has undefined behavior.

(3, 4)    Pre increment/decrement operator. Overflow has undefined behavior.

### Assignment Operators

```cpp
constexpr io1::money & io1::money::operator=(io1::money const & m) noexcept; (1)
constexpr io1::money & io1::money::operator+=(io1::money m) noexcept; (2)
constexpr io1::money & io1::money::operator-=(io1::money m) noexcept; (3)
template<std::integral T> constexpr io1::money & io1::money::operator*=(T i) noexcept; (4)
template<std::integral T> constexpr io1::money & io1::money::operator/=(T i); (5)
io1::money & io1::money::operator*=(long double f) noexcept; (6)
io1::money & io1::money::operator/=(long double f) noexcept; (7)
```

(1)    Copy assignment.

(2, 3)    Addition and subtraction assignment. Replace the amount with the result of addition / subtraction the between the previous amount and the amount of `m`. Overflow has undefined behavior.

(4)    Multiplication by an integer assignment. Replace the amount with the result of the multiplication of the previous amount by `i`. Overflow has undefined behavior.

(5)    Division by an integer assignment. Replace the amount with the result of the division of the previous amount by `i`. If the result is not exact, the operator throws an instance of `io1::money::InexactDivision` and provides the strong exception guarantee (see example below). Dividing by zero has undefined behavior.

(6, 7)    Multiplication and division by a float. Replace the amount with the result of the multiplication / division of the previous amount by `f`. Overflow and division by zero have undefined behavior. Inexact results are rounded to nearest even integer. These operators assert that the current floating-point rounding mode is still (or has been restored to) the default value of `FE_TONEAREST`.

Multiplication and division assignments from `io1::money` instances are not provided because they would violate dimensional analysis and would hence allow flawed uses of money arithmetic. For example, consider the result of 5 USD multiplied by 10 USD. According to dimensional analysis, the result should be homogeneous to USD _squared_, hence 50 USD² and not 50 USD. Likewise, 10 USD divided by 5 USD equals 2 (no dimension) and not 2 USD.

### Example

```cpp
io1::money m = 10_money;
m /= 2; // ok, the result is 5_money
try { m /= 2; }
catch (io1::money::InexactDivision const & e)
{
    std::cerr << "Dividing " << e.dividend << " by " << e.divisor << " is not exact.\n";
    std::cout << "m still holds: " << m << '\n'; // 5_money
}
```

### Arithmetic Operators

```cpp
[[nodiscard]] constexpr io1::money io1::money::operator-() const noexcept; (1)
```

(1)    Return an instance with an opposite amount. Overflow has undefined behavior.

The following operators are built from the assignment operators of the previous section and thus have the same restrictions.

```cpp
[[nodiscard]] constexpr io1::money io1::operator+(io1::money lhs, io1::money rhs) noexcept;
[[nodiscard]] constexpr io1::money io1::operator-(io1::money lhs, io1::money rhs) noexcept;
template<std::integral T> [[nodiscard]] constexpr io1::money io1::operator*(io1::money lhs, T rhs) noexcept;
template<std::integral T> [[nodiscard]] constexpr io1::money io1::operator*(T lhs, io1::money rhs) noexcept;
template<std::integral T> [[nodiscard]] constexpr io1::money io1::operator/(io1::money lhs, T rhs);
[[nodiscard]] io1::money io1::operator*(long double lhs, io1::money rhs) noexcept;
[[nodiscard]] io1::money io1::operator/(io1::money lhs, long double rhs) noexcept;
```

Right multiplication by `long double` is not provided because it has more rounding issues than left multiplication. Consider the following example.

```cpp
auto const m1 = 2_money * 0.1 * 10.; (1)
auto const m1 = 0.1 * 10. * 2_money; (2)
```

(1)    Left to right evaluation results in `0_money` because of the intermediate rounding that occurs.

(2)    Left to right evaluation gives the expected result of `2_money`.

```cpp
[[nodiscard]] io1::moneydiv_t io1::div(io1::money m, io1::money::value_type divisor) noexcept;
```

Compute both the quotient (`io1::moneydiv_t::quot`) and remainder (`io1::moneydiv_t::rem`) of `m` divided by `divisor`. It has undefined behavior if `0 == divisor`. Returned `quot` and `rem` are such that `divisor * quot + rem == m` and `std::abs(rem) < std::abs(m)`. Like `std::div_t`, `io1::moneydiv_t` may be implemented as

```cpp
struct moneydiv_t { io1::money quot; io1::money rem; };
```

or

```cpp
struct moneydiv_t { io1::money rem; io1::money quot; };
```

### Comparison Operators

```cpp
[[nodiscard]] constexpr std::strong_ordering io1::operator<=>(io1::money lhs, io1::money rhs) noexcept;
[[nodiscard]] constexpr bool io1::operator==(io1::money lhs, io1::money rhs) noexcept;
[[nodiscard]] constexpr bool io1::operator!=(io1::money lhs, io1::money rhs) noexcept;
[[nodiscard]] constexpr bool io1::operator<(io1::money lhs, io1::money rhs) noexcept;
[[nodiscard]] constexpr bool io1::operator<=(io1::money lhs, io1::money rhs) noexcept;
[[nodiscard]] constexpr bool io1::operator>(io1::money lhs, io1::money rhs) noexcept;
[[nodiscard]] constexpr bool io1::operator>=(io1::money lhs, io1::money rhs) noexcept;
```

### Stream Operators

```cpp
std::ostream & operator<<(std::ostream & stream, io1::money m) noexcept; (1)
std::istream & operator>>(std::istream & stream, io1::money & m); (2)
[[nodiscard]] /*unspecified*/ io1::put_money(io1::money const & m, bool intl = false) noexcept; (3)
[[nodiscard]] /*unspecified*/ io1::get_money(io1::money & m, bool intl = false); (4)
```

(1)    Write the amount held by `m` in `stream`.

(2)    Parse an instance of `io1::money` from `stream` and copy assign it to `m`. The implementation is equivalent to the following code.

```cpp
std::istream & operator>>(std::istream & stream, io1::money & m)
{
    io1::money::value_type amount;
    if (stream >> amount) m = io1::money{amount};
    return stream;
}
```

(3)    Return an object of unspecified type that can be inserted into a `std::ostream` instance to format `m` according to its current `moneypunct` facet. The `intl` argument, if true, uses the international currency string instead of the currency symbol.

(4)    Return an object of unspecified type that can extract an instance of `io1::money` from a `std::istream` instance according to its current `moneypunct` facet. The `intl` argument, if true, expects to find a required international currency string instead of an optional currency symbol.

### Standard Format Support

The syntax of format specifications is the same as the standard syntax for `integers`. Alternatively, localized formatting through the current `moneypunct` facet can be achieved with the following syntax:

>  _fill-and-align_(optional) _width_(optional) _#_(optional) _type_(m or M)

#### Fill, Align and Width

Same as standard specifications for `std::string`.

#### Show Currency \#

Display currency in a way equivalent to activating the `showbase` I/O manipulator on streams.

#### Type

The type option must be one of the following:

- **m**: format with the `std::moneypunct<CharT, false>` facet.

- **M**: format with the `std::moneypunct<CharT, true>` facet.

#### Examples

```cpp
std::cout << std::format("{}\n", 12.35_money);
std::cout << std::format("{:m}\n", 12.35_money);
std::cout << std::format("{:M)\n", 12.35_money);
std::cout << std::format(std::locale("en_US.UTF-8"), "{:m}\n", 12.35_money);
std::cout << std::format(std::locale("en_US.UTF-8"), "{:M}\n", 12.35_money);
std::cout << std::format(std::locale("en_US.UTF-8"), "{:#m}\n", 12.35_money);
std::cout << std::format(std::locale("en_US.UTF-8"), "{:#M}\n", 12.35_money);
```

Assuming the global locale is the default C++ locale, the above code prints out:

```cpp
1235
1235
1235
12.35
 12.35
$12.35
USD  12.35
```

## Exceptions

```cpp
struct [[nodiscard]] io1::money::InexactDivision
{
    io1::money::value_type dividend;
    io1::money::value_type divisor;
};
```

Exception thrown when trying to divide `dividend` by `divisor` and `dividend % divisor != 0`.

# Tutorial

```cpp
// file test/tutorial.cpp
#include "io1/money.hpp"

#include <doctest/doctest.h>
#include <numeric>
#include <sstream>
#include <vector>

class american_moneypunct_facet : public std::moneypunct<char, false>
{
private:
  char do_decimal_point() const override { return '.'; };
  char do_thousands_sep() const override { return ','; };
  std::string do_grouping() const override { return "\003"; };
  std::string do_curr_symbol() const override { return "$"; };
  std::string do_positive_sign() const override { return ""; };
  std::string do_negative_sign() const override { return "-"; };
  int do_frac_digits() const override { return 2; };
  pattern do_pos_format() const override { return {{symbol, sign, value}}; };
  pattern do_neg_format() const override { return {{symbol, sign, value}}; };
};

[[nodiscard]] auto compute_installment_plan(io1::money price, std::size_t count) noexcept
{
  assert(0 != count && "Not doing a plan for no payment.");

  auto const div_result = div(price, count);
  assert(div_result.rem >= 0_money && "Count was unsigned.");

  auto const m = static_cast<std::size_t>(div_result.rem.data());
  std::vector<io1::money> plan(count, div_result.quot);
  assert(m < plan.size() && "As per the io1::div documentation since plan.size() is count.");

  for (std::size_t i = 0; i < m; ++i) ++plan[i];

  return plan;
}

TEST_CASE("Tutorial")
{
  auto const unit_price = 12.00_money;
  auto const vat = 1.2;
  auto const discount_rate = 10. / 100.;

  // Parse How many items are bought:
  unsigned int nb = 0;
  std::stringstream("2") >> nb;

  // Parse how much tip is given:
  auto tip = 0_money;
  {
    std::stringstream cin("$1.00");
    cin.imbue(std::locale(cin.getloc(), std::make_unique<american_moneypunct_facet>().release()));
    cin >> get_money(tip);
  }

  // Parse how many installments for payment plan
  std::size_t installments_nb = 0;
  std::stringstream("5") >> installments_nb;

  auto const final_price = tip + nb * (1. - discount_rate) * vat * unit_price;
  std::stringstream cout;
  cout.imbue(std::locale(cout.getloc(), std::make_unique<american_moneypunct_facet>().release()));
  cout << "Total: " << std::showbase << put_money(final_price) << '\n';

  auto const payment_plan = compute_installment_plan(final_price, installments_nb);
  cout << installments_nb << " payments:";
  for (auto const & amount : payment_plan) cout << ' ' << put_money(amount);

  CHECK_EQ(std::accumulate(payment_plan.begin(), payment_plan.end(), 0_money), final_price);
  CHECK_EQ(cout.str(), "Total: $26.92\n5 payments: $5.39 $5.39 $5.38 $5.38 $5.38");

  return;
}
```

#pragma once

#include <cassert>
#include <cfenv>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

namespace io1
{
  class money
  {
  public:
    using value_type = std::int64_t;
    // -9223372036854775807 - 1 below, is a portable way to have the value -9223372036854775808 with no
    // overflow because of 'operator-' applied after the positive int (too big) is created.
    static_assert(
        std::numeric_limits<value_type>::max() >=
                9'223'372'036'854'775'807 && // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            std::numeric_limits<value_type>::lowest() <=
                -9'223'372'036'854'775'807 - // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                    1,
        "Type too short to hold the advertised value range.");

    money() noexcept = default;

    constexpr money(money const &) noexcept = default;
    constexpr money(money &&) noexcept = default;
    constexpr money & operator=(money const &) noexcept = default;
    constexpr money & operator=(money &&) noexcept = default;

    constexpr ~money() noexcept = default;

    template <std::integral T>
    explicit constexpr money(T amount) noexcept : amount_(static_cast<value_type>(amount))
    {
    }

    template <std::floating_point T>
    explicit constexpr money(T amount) noexcept = delete;

    [[nodiscard]] constexpr value_type const & data() const noexcept { return amount_; }

    [[nodiscard]] constexpr money operator++(int) noexcept { return money{amount_++}; }
    [[nodiscard]] constexpr money operator--(int) noexcept { return money{amount_--}; }

    constexpr money & operator++() noexcept
    {
      ++amount_;
      return *this;
    }

    constexpr money & operator--() noexcept
    {
      --amount_;
      return *this;
    }

    constexpr money & operator+=(money val) noexcept
    {
      amount_ += val.amount_;
      return *this;
    }

    constexpr money & operator-=(money val) noexcept
    {
      amount_ -= val.amount_;
      return *this;
    }

    template <std::integral T>
    constexpr money & operator*=(T val) noexcept
    {
      amount_ *= static_cast<value_type>(val);
      return *this;
    }

    template <std::floating_point T>
    money & operator*=(T val) noexcept
    {
      assert((std::fegetround() == FE_TONEAREST) &&
             "Make sure the default rounding mode is active before entering this function.");
      auto const result = std::llrint(static_cast<long double>(amount_) * static_cast<long double>(val));
      static_assert(sizeof(decltype(result)) == sizeof(decltype(amount_)),
                    "Consider changing the call to 'std::llrint'.");
      amount_ = result;
      return *this;
    }

    template <std::integral T>
    constexpr money & operator/=(T val);

    template <std::floating_point T>
    money & operator/=(T val) noexcept;

    [[nodiscard]] constexpr money operator-() const noexcept { return money{-amount_}; }
    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(money lhs, money rhs) noexcept = default;

    struct [[nodiscard]] inexact_division_error;
    struct div_t;

  private:
    struct put_manip;
    struct get_manip;
    struct string_literal_decoder;

    friend io1::money::put_manip put_money(io1::money val, bool intl) noexcept;
    friend io1::money::get_manip get_money(io1::money & val, bool intl);
    template <char... STR>
    friend consteval money operator""_money() noexcept;

    value_type amount_;
  };

  struct money::div_t
  {
    money quot;
    money rem;
  };

  struct [[nodiscard]] money::inexact_division_error : public std::runtime_error
  {
    explicit inexact_division_error(value_type dividend_val,         // NOLINT(bugprone-easily-swappable-parameters)
                                    value_type divisor_val) noexcept // NOLINT(bugprone-easily-swappable-parameters)
        : std::runtime_error("Cannot perform an inexact division!"), dividend(dividend_val), divisor(divisor_val)
    {
    }

    value_type dividend;
    value_type divisor;
  };

  struct money::get_manip
  {
    explicit get_manip(money & val, bool intl) noexcept : intl_(intl), amount_(val.amount_) {};
    get_manip(get_manip const &) = delete;
    get_manip(get_manip &&) = delete;
    get_manip & operator=(get_manip const &) = delete;
    get_manip & operator=(get_manip &&) = delete;
    ~get_manip() noexcept = default;

    friend std::istream & operator>>(std::istream & stream, get_manip const & obj)
    {
      std::string amount;
      stream >> std::get_money(amount, obj.intl_);
      if (!stream) { return stream; }

      try
      {
        auto const parsed_amount = std::stoll(amount);
        static_assert(sizeof(decltype(parsed_amount)) == sizeof(decltype(obj.amount_)),
                      "Consider changing the call to std::stoll.");
        obj.amount_ = parsed_amount;
      }
      catch (std::out_of_range const &)
      {
        stream.setstate(std::ios_base::failbit);
      }

      return stream;
    }

    bool intl_;
    value_type & amount_;
  };

  struct money::put_manip
  {
    explicit put_manip(money val, bool intl) noexcept : intl_(intl), amount_(std::to_string(val.data())) {}
    put_manip(put_manip const &) = delete;
    put_manip(put_manip &&) = delete;
    put_manip & operator=(put_manip const &) = delete;
    put_manip & operator=(put_manip &&) = delete;
    ~put_manip() noexcept = default;

    friend std::ostream & operator<<(std::ostream & stream, put_manip const & obj)
    {
      return stream << std::put_money(obj.amount_, obj.intl_);
    }

    bool intl_;
    std::string amount_;
  };

  // Helper structure to build an io1::money object from a user-defined string literal
  struct money::string_literal_decoder
  {
  public:
    template <char... STR>
    [[nodiscard]] consteval static money apply() noexcept
    {
      return money{parse_mantissa<0, STR...>()};
    }

  private:
    template <value_type CURRENT_MANTISSA, char DIGIT>
    [[nodiscard]] consteval static value_type parse_digit() noexcept
    {
      static_assert('0' <= DIGIT && '9' >= DIGIT, "Unexpected digit!");

      constexpr auto digit = static_cast<value_type>(DIGIT - '0');
      constexpr auto ten = static_cast<value_type>(10);

      static_assert(CURRENT_MANTISSA >= 0, "Parsing a raw user-defined literal.");
      static_assert((std::numeric_limits<value_type>::max() - digit) / ten >= CURRENT_MANTISSA,
                    "Number not representable by io1::money");

      return (ten * CURRENT_MANTISSA) + digit;
    }

    template <char DIGIT>
    [[nodiscard]] consteval static bool not_a_digit() noexcept
    {
      return (DIGIT == '.' || DIGIT == '\'');
    }

    template <value_type CURRENT_MANTISSA, char DIGIT, char... STR>
    consteval static value_type parse_mantissa() noexcept
    {
      constexpr auto new_mantissa = []()
      {
        if constexpr (not_a_digit<DIGIT>()) { return CURRENT_MANTISSA; }
        else { return parse_digit<CURRENT_MANTISSA, DIGIT>(); }
      }();

      if constexpr (0 < sizeof...(STR)) { return parse_mantissa<new_mantissa, STR...>(); }
      else { return new_mantissa; }
    }
  };

  [[nodiscard]] constexpr money operator+(money lhs, money rhs) noexcept
  {
    return lhs += rhs;
  }

  [[nodiscard]] constexpr money operator-(money lhs, money rhs) noexcept
  {
    return lhs -= rhs;
  }

  template <class T>
  constexpr money operator*(money lhs, T rhs) = delete;

  template <std::integral T>
  [[nodiscard]] money operator*(money lhs, T rhs) noexcept
  {
    return lhs *= rhs;
  }

  template <std::integral T>
  [[nodiscard]] money operator*(T lhs, money rhs) noexcept
  {
    return rhs *= lhs;
  }

  template <std::floating_point T>
  [[nodiscard]] money operator*(T lhs, money rhs) noexcept
  {
    return rhs *= static_cast<long double>(lhs);
  }

  template <std::integral T>
  [[nodiscard]] constexpr money operator/(money lhs, T rhs)
  {
    return lhs /= rhs;
  }

  template <std::floating_point T>
  [[nodiscard]] money operator/(money lhs, T rhs) noexcept
  {
    return lhs /= static_cast<long double>(rhs);
  }

  [[nodiscard]] inline money::div_t div(money val, money::value_type divisor) noexcept
  {
    assert(0 != divisor && "Division by zero is undefined behavior.");

    auto const result = std::div(val.data(), divisor);
    return {.quot = money(result.quot), .rem = money(result.rem)};
  }

  template <std::integral T>
  constexpr money & money::operator/=(T val)
  {
    assert(0 != val && "Dividing by zero is undefined behavior.");
    auto const divisor = static_cast<value_type>(val);
    if (amount_ % divisor) { throw money::inexact_division_error{amount_, divisor}; }

    // strong guarantee
    amount_ /= divisor;

    return *this;
  }

  template <std::floating_point T>
  inline money & money::operator/=(T val) noexcept
  {
    assert(0. != val && "Dividing by zero is undefined behavior.");
    assert((std::fegetround() == FE_TONEAREST) &&
           "Make sure the default rounding mode is active before entering this function.");
    auto const result = std::llrint(static_cast<long double>(amount_) / static_cast<long double>(val));
    static_assert(sizeof(decltype(result)) == sizeof(decltype(amount_)), "Consider changing the call to std::llrint.");
    amount_ = result;

    return *this;
  }

  inline std::ostream & operator<<(std::ostream & stream, money val)
  {
    return stream << val.data();
  }

  inline std::istream & operator>>(std::istream & stream, money & val)
  {
    money::value_type amount; // NOLINT(cppcoreguidelines-init-variables) value is used once we have confirmation
    // that it has been initialized
    stream >> amount;
    if (stream) { val = money(amount); } // strong guarantee
    return stream;
  }

  [[nodiscard]] inline money::put_manip put_money(money val, bool intl = false) noexcept
  {
    return money::put_manip(val, intl);
  }

  [[nodiscard]] inline money::get_manip get_money(money & val, bool intl = false)
  {
    return money::get_manip(val, intl);
  }

  template <char... STR>
  consteval money operator""_money() noexcept
  {
    return money::string_literal_decoder::apply<STR...>();
  }
} // namespace io1

static_assert(std::is_trivial_v<io1::money> && std::is_standard_layout_v<io1::money>,
              "You have changed io1::money in a way that removed its POD nature!");

namespace io1::literals
{
  using io1::operator""_money;
}

template <class CharT>
struct std::formatter<io1::money, CharT>
{
  template <class FormatParseContext>
  constexpr auto parse(FormatParseContext & ctx)
  {
    auto close_pos = ctx.begin();
    while (close_pos != ctx.end() && *close_pos != '}') { ++close_pos; }

    std::basic_string_view<CharT> const spec(ctx.begin(), close_pos);

    if (spec.ends_with("m")) { locale_ = true; }
    else if (spec.ends_with("M"))
    {
      locale_ = true;
      intl_ = true;
    }

    if (!locale_) { return int_.parse(ctx); }

    auto size = spec.size() - 1;

    if (spec.find('#') != spec.npos)
    {
      --size;
      showbase_ = true;
    }

    FormatParseContext subctx(spec.substr(0, size));
    string_.parse(subctx);

    return close_pos;
  }

  template <class FormatContext>
  auto format(io1::money const & val, FormatContext & ctx) const
  {
    if (!locale_) { return int_.format(val.data(), ctx); }
    else
    {
      std::basic_stringstream<CharT> stream;
      stream.imbue(ctx.locale());
      stream << (showbase_ ? std::showbase : std::noshowbase) << std::put_money(std::to_string(val.data()), intl_);
      return string_.format(stream.str(), ctx);
    }
  }

  bool locale_ : 1 {false};
  bool showbase_ : 1 {false};
  bool intl_ : 1 {false};
  std::formatter<io1::money::value_type, CharT> int_;
  std::formatter<std::basic_string<CharT>, CharT> string_;
};

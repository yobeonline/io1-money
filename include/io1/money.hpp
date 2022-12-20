#pragma once

#include <cassert>
#include <cfenv>
#include <cmath>
#include <compare>
#include <concepts>
#include <iomanip>
#include <iosfwd>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace io1
{
  class Money;
}

template <char... STR>
constexpr io1::Money operator""_money(void) noexcept;

namespace io1
{
  class Money
  {
  public:
    using value_type = std::int64_t;
    // -9'223'372'036'854'775'807-1 below, is a portable way to have the value -9'223'372'036'854'775'808 with no
    // overflow because of operator- applied after the positive int (too big) is created.
    static_assert(std::numeric_limits<value_type>::max() >= 9'223'372'036'854'775'807 &&
                      std::numeric_limits<value_type>::lowest() <= -9'223'372'036'854'775'807 - 1,
                  "Type too short to hold the advertised value range.");

  public:
    Money(void) noexcept = default;

    constexpr Money(Money const &) noexcept = default;
    constexpr Money & operator=(Money const &) noexcept = default;

    template <std::integral T>
    explicit constexpr Money(T amount) noexcept : amount_(static_cast<value_type>(amount))
    {
    }

    template <std::floating_point T>
    explicit constexpr Money(T amount) noexcept = delete;

    template <char... STR>
    friend constexpr Money(::operator""_money)(void) noexcept;

    [[nodiscard]] constexpr value_type const & data(void) const noexcept { return amount_; }

    [[nodiscard]] constexpr Money operator++(int) noexcept { return Money{amount_++}; }
    [[nodiscard]] constexpr Money operator--(int) noexcept { return Money{amount_--}; }

    constexpr Money & operator++(void) noexcept
    {
      ++amount_;
      return *this;
    }
    constexpr Money & operator--(void) noexcept
    {
      --amount_;
      return *this;
    }

    constexpr Money & operator+=(Money m) noexcept
    {
      amount_ += m.amount_;
      return *this;
    }
    constexpr Money & operator-=(Money m) noexcept
    {
      amount_ -= m.amount_;
      return *this;
    }

    template <std::integral T>
    constexpr Money & operator*=(T i) noexcept
    {
      amount_ *= static_cast<value_type>(i);
      return *this;
    }

    template <std::floating_point T>
    Money & operator*=(T v) noexcept
    {
      assert((std::fegetround() == FE_TONEAREST) &&
             "Make sure the default rounding mode is active before entering this function.");
      auto const result = std::llrint(amount_ * static_cast<long double>(v));
      static_assert(sizeof(decltype(result)) == sizeof(decltype(amount_)),
                    "Consider changing the call to std::llrint.");
      amount_ = result;
      return *this;
    }

    template <std::integral T>
    constexpr Money & operator/=(T i);

    template <std::floating_point T>
    Money & operator/=(T v) noexcept;

    [[nodiscard]] constexpr Money operator-(void) const noexcept { return Money{-amount_}; }
    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(Money lhs, Money rhs) noexcept = default;

  public:
    struct [[nodiscard]] InexactDivision : private std::runtime_error
    {
      explicit InexactDivision(value_type dend, value_type dsor) noexcept
          : std::runtime_error("Cannot perform an inexact division!"), dividend(dend), divisor(dsor)
      {
      }

      value_type dividend;
      value_type divisor;
    };

  private:
    struct StringLitteralDecoder;
    struct PutMoney;
    struct GetMoney;

    friend io1::Money::PutMoney put_money(io1::Money, bool) noexcept;
    friend io1::Money::GetMoney get_money(io1::Money &, bool);

  private:
    value_type amount_;
  };

  static_assert(std::is_trivial_v<io1::Money> && std::is_standard_layout_v<io1::Money>,
                "You have changed io1::Money in a way that removed its POD nature!");

  template <std::integral T>
  constexpr Money & Money::operator/=(T i)
  {
    assert(0 != i && "Dividing by zero is undefined behavior.");
    auto const divisor = static_cast<value_type>(i);
    if (amount_ % divisor) throw io1::Money::InexactDivision{amount_, divisor};

    // strong guarantee
    amount_ /= divisor;

    return *this;
  }

  template <std::floating_point T>
  inline Money & Money::operator/=(T v) noexcept
  {
    assert(0. != v && "Dividing by zero is undefined behavior.");
    assert((std::fegetround() == FE_TONEAREST) &&
           "Make sure the default rounding mode is active before entering this function.");
    auto const result = std::llrint(amount_ / static_cast<long double>(v));
    static_assert(sizeof(decltype(result)) == sizeof(decltype(amount_)), "Consider changing the call to std::llrint.");
    amount_ = result;

    return *this;
  }

  [[nodiscard]] inline constexpr Money operator+(Money lhs, Money rhs) noexcept
  {
    return lhs += rhs;
  }
  [[nodiscard]] inline constexpr Money operator-(Money lhs, Money rhs) noexcept
  {
    return lhs -= rhs;
  }

  template <class T>
  constexpr Money operator*(Money lhs, T rhs) = delete;

  template <std::integral T>
  [[nodiscard]] constexpr Money operator*(Money lhs, T rhs) noexcept
  {
    return lhs *= rhs;
  }

  template <std::integral T>
  [[nodiscard]] constexpr Money operator*(T lhs, Money rhs) noexcept
  {
    return rhs *= lhs;
  }

  template <std::floating_point T>
  [[nodiscard]] inline Money operator*(T lhs, Money rhs) noexcept
  {
    return rhs *= static_cast<long double>(lhs);
  }

  template <std::integral T>
  [[nodiscard]] constexpr Money operator/(Money lhs, T rhs)
  {
    return lhs /= rhs;
  }

  template <std::floating_point T>
  [[nodiscard]] inline Money operator/(Money lhs, T rhs) noexcept
  {
    return lhs /= static_cast<long double>(rhs);
  }

  // Helper structure to build a io1::Money object from a user-defined string litteral
  struct Money::StringLitteralDecoder
  {
  public:
    template <char... STR>
    [[nodiscard]] constexpr static Money apply(void) noexcept
    {
      return Money{parse_mantissa<0, STR...>()};
    }

  private:
    template <value_type CURRENT_MANTISSA, char DIGIT>
    [[nodiscard]] constexpr static value_type parse_digit(void) noexcept
    {
      static_assert('0' <= DIGIT && '9' >= DIGIT, "Unexpected digit!");

      constexpr auto d = static_cast<value_type>(DIGIT - '0');
      constexpr auto ten = static_cast<value_type>(10);

      static_assert(CURRENT_MANTISSA >= 0, "Parsing a raw user-defined litteral.");
      static_assert((std::numeric_limits<value_type>::max() - d) / ten >= CURRENT_MANTISSA,
                    "Number not representable by io1::Money");

      return ten * CURRENT_MANTISSA + d;
    }

    template <char DIGIT>
    [[nodiscard]] constexpr static bool not_a_digit(void) noexcept
    {
      return (DIGIT == '.' || DIGIT == '\'');
    }

    template <value_type CURRENT_MANTISSA, char DIGIT, char... STR>
    constexpr static value_type parse_mantissa(void) noexcept
    {
      constexpr auto new_mantissa = []()
      {
        if constexpr (not_a_digit<DIGIT>()) return CURRENT_MANTISSA;
        else
          return parse_digit<CURRENT_MANTISSA, DIGIT>();
      }();

      if constexpr (0 < sizeof...(STR)) return parse_mantissa<new_mantissa, STR...>();
      else
        return new_mantissa;
    }
  };

  inline std::ostream & operator<<(std::ostream & stream, io1::Money m) noexcept
  {
    return stream << m.data();
  }
  inline std::istream & operator>>(std::istream & stream, io1::Money & m)
  {
    io1::Money::value_type amount;
    stream >> amount;
    if (stream) m = io1::Money(amount); // strong guarantee
    return stream;
  }

  struct Money::GetMoney
  {
    explicit GetMoney(Money & m, bool intl) noexcept : intl_(intl), amount_(m.amount_){};
    GetMoney(GetMoney const &) = delete;
    GetMoney & operator=(GetMoney const &) = delete;

    friend inline std::istream & operator>>(std::istream & stream, GetMoney && o)
    {
      std::string amount;
      stream >> std::get_money(amount, o.intl_);
      if (!stream) return stream;

      try
      {
        auto const parsed_amount = std::stoll(amount);
        static_assert(sizeof(decltype(parsed_amount)) == sizeof(decltype(o.amount_)),
                      "Consider changing the call to std::stoll.");
        o.amount_ = parsed_amount;
      }
      catch (std::out_of_range const &)
      {
        stream.setstate(std::ios_base::failbit);
      }

      return stream;
    }

    bool intl_;
    Money::value_type & amount_;
  };

  struct Money::PutMoney
  {
    explicit PutMoney(Money m, bool intl) noexcept : intl_(intl), amount_(std::to_string(m.data())) {}
    PutMoney(PutMoney const &) = delete;
    PutMoney & operator=(PutMoney const &) = delete;

    friend inline std::ostream & operator<<(std::ostream & stream, PutMoney const & o)
    {
      return stream << std::put_money(o.amount_, o.intl_);
    }

    bool intl_;
    std::string amount_;
  };

  [[nodiscard]] inline io1::Money::PutMoney put_money(io1::Money m, bool intl = false) noexcept
  {
    return io1::Money::PutMoney(m, intl);
  }

  [[nodiscard]] inline io1::Money::GetMoney get_money(io1::Money & m, bool intl = false)
  {
    return io1::Money::GetMoney(m, intl);
  }

  namespace detail
  {
    struct moneydiv_quotrem_t
    {
      Money quot;
      Money rem;
    };
    struct moneydiv_remquot_t
    {
      Money rem;
      Money quot;
    };
    constexpr bool is_quot_rem_v = 10 == std::div_t{10, 5}.quot;
  } // namespace detail

  using moneydiv_t = std::conditional_t<detail::is_quot_rem_v, detail::moneydiv_quotrem_t, detail::moneydiv_remquot_t>;

  [[nodiscard]] inline moneydiv_t div(Money m, Money::value_type divisor) noexcept
  {
    assert(0 != divisor && "Division by zero is undefined behavior.");

    auto const result = std::div(m.data(), divisor);

    return {.quot = Money(result.quot), .rem = Money(result.rem)};
  }

} // namespace io1

template <char... STR>
constexpr io1::Money operator""_money(void) noexcept
{
  return io1::Money::StringLitteralDecoder::apply<STR...>();
}

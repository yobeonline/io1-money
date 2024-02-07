#pragma once

#include <cassert>
#include <cfenv>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstdint>
#include <format>
#include <iomanip>
#include <iosfwd>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace io1
{
  class money
  {
  public:
    using value_type = std::int64_t;
    // -9'223'372'036'854'775'807-1 below, is a portable way to have the value -9'223'372'036'854'775'808 with no
    // overflow because of operator- applied after the positive int (too big) is created.
    static_assert(std::numeric_limits<value_type>::max() >= 9'223'372'036'854'775'807 && // NOLINT(cppcoreguidelines-avoid-magic-numbers)
                      std::numeric_limits<value_type>::lowest() <= -9'223'372'036'854'775'807 - 1, // NOLINT(cppcoreguidelines-avoid-magic-numbers)
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

    [[nodiscard]] constexpr money const operator++(int) noexcept { return money{amount_++}; }
    [[nodiscard]] constexpr money const operator--(int) noexcept { return money{amount_--}; }

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
    constexpr money & operator*=(T ival) noexcept
    {
      amount_ *= static_cast<value_type>(ival);
      return *this;
    }

    template <std::floating_point T>
    money & operator*=(T fval) noexcept
    {
      assert((std::fegetround() == FE_TONEAREST) &&
             "Make sure the default rounding mode is active before entering this function.");
      auto const result = std::llrint(amount_ * static_cast<long double>(fval));
      static_assert(sizeof(decltype(result)) == sizeof(decltype(amount_)),
                    "Consider changing the call to std::llrint.");
      amount_ = result;
      return *this;
    }

    template <std::integral T>
    constexpr money & operator/=(T ival);

    template <std::floating_point T>
    money & operator/=(T fval) noexcept;

    [[nodiscard]] constexpr money operator-() const noexcept { return money{-amount_}; }
    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(money lhs, money rhs) noexcept = default;

    struct [[nodiscard]] InexactDivision : public std::runtime_error
    {
      explicit InexactDivision(value_type dividend, value_type divisor) noexcept // NOLINT(bugprone-easily-swappable-parameters)
          : std::runtime_error("Cannot perform an inexact division!"), dividend(dividend), divisor(divisor)
      {
      }

      value_type dividend;
      value_type divisor;
    };

  private:
    struct PutMoney;
    struct GetMoney;

    friend io1::money::PutMoney put_money(io1::money val, bool intl) noexcept;
    friend io1::money::GetMoney get_money(io1::money & val, bool intl);

    value_type amount_;
  };

  static_assert(std::is_trivial_v<io1::money> && std::is_standard_layout_v<io1::money>,
                "You have changed io1::Money in a way that removed its POD nature!");

  template <std::integral T>
  constexpr money & money::operator/=(T ival)
  {
    assert(0 != ival && "Dividing by zero is undefined behavior.");
    auto const divisor = static_cast<value_type>(ival);
    if (amount_ % divisor) { throw money::InexactDivision{amount_, divisor}; }

    // strong guarantee
    amount_ /= divisor;

    return *this;
  }

  template <std::floating_point T>
  inline money & money::operator/=(T fval) noexcept
  {
    assert(0. != fval && "Dividing by zero is undefined behavior.");
    assert((std::fegetround() == FE_TONEAREST) &&
           "Make sure the default rounding mode is active before entering this function.");
    auto const result = std::llrint(amount_ / static_cast<long double>(fval));
    static_assert(sizeof(decltype(result)) == sizeof(decltype(amount_)), "Consider changing the call to std::llrint.");
    amount_ = result;

    return *this;
  }

  [[nodiscard]] inline constexpr money operator+(money lhs, money rhs) noexcept
  {
    return lhs += rhs;
  }
  [[nodiscard]] inline constexpr money operator-(money lhs, money rhs) noexcept
  {
    return lhs -= rhs;
  }

  template <class T>
  constexpr money operator*(money lhs, T rhs) = delete;

  template <std::integral T>
  [[nodiscard]] constexpr money operator*(money lhs, T rhs) noexcept
  {
    return lhs *= rhs;
  }

  template <std::integral T>
  [[nodiscard]] constexpr money operator*(T lhs, money rhs) noexcept
  {
    return rhs *= lhs;
  }

  template <std::floating_point T>
  [[nodiscard]] inline money operator*(T lhs, money rhs) noexcept
  {
    return rhs *= static_cast<long double>(lhs);
  }

  template <std::integral T>
  [[nodiscard]] constexpr money operator/(money lhs, T rhs)
  {
    return lhs /= rhs;
  }

  template <std::floating_point T>
  [[nodiscard]] inline money operator/(money lhs, T rhs) noexcept
  {
    return lhs /= static_cast<long double>(rhs);
  }

  // Helper structure to build a io1::Money object from a user-defined string litteral
  namespace detail
  {
    struct StringLitteralDecoder
    {
    public:
      template <char... STR>
      [[nodiscard]] constexpr static money apply() noexcept
      {
        return money{parse_mantissa<0, STR...>()};
      }

    private:
      template <money::value_type CURRENT_MANTISSA, char DIGIT>
      [[nodiscard]] constexpr static money::value_type parse_digit() noexcept
      {
        static_assert('0' <= DIGIT && '9' >= DIGIT, "Unexpected digit!");

        constexpr auto digit = static_cast<money::value_type>(DIGIT - '0');
        constexpr auto ten = static_cast<money::value_type>(10);

        static_assert(CURRENT_MANTISSA >= 0, "Parsing a raw user-defined litteral.");
        static_assert((std::numeric_limits<money::value_type>::max() - digit) / ten >= CURRENT_MANTISSA,
                      "Number not representable by io1::Money");

        return ten * CURRENT_MANTISSA + digit;
      }

      template <char DIGIT>
      [[nodiscard]] constexpr static bool not_a_digit() noexcept
      {
        return (DIGIT == '.' || DIGIT == '\'');
      }

      template <money::value_type CURRENT_MANTISSA, char DIGIT, char... STR>
      constexpr static money::value_type parse_mantissa() noexcept
      {
        constexpr auto new_mantissa = []()
        {
          if constexpr (not_a_digit<DIGIT>()) { return CURRENT_MANTISSA; }
          else
          {
            return parse_digit<CURRENT_MANTISSA, DIGIT>();
          }
        }();

        if constexpr (0 < sizeof...(STR)) { return parse_mantissa<new_mantissa, STR...>(); }
        else
        { return new_mantissa; }
      }
    };
  } // namespace detail

  inline std::ostream & operator<<(std::ostream & stream, io1::money val) noexcept
  {
    return stream << val.data();
  }
  inline std::istream & operator>>(std::istream & stream, io1::money & val)
  {
    io1::money::value_type amount;
    stream >> amount;
    if (stream) { val = io1::money(amount); } // strong guarantee
    return stream;
  }

  struct money::GetMoney
  {
    explicit GetMoney(money & val, bool intl) noexcept : intl_(intl), amount_(val.amount_){};
    GetMoney(GetMoney const &) = delete;
    GetMoney(GetMoney &&) = delete;
    GetMoney & operator=(GetMoney const &) = delete;
    GetMoney & operator=(GetMoney &&) = delete;
    ~GetMoney() noexcept = default;

    friend inline std::istream & operator>>(std::istream & stream, GetMoney && obj)
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
    money::value_type & amount_;
  };

  struct money::PutMoney
  {
    explicit PutMoney(money val, bool intl) noexcept : intl_(intl), amount_(std::to_string(val.data())) {}
    PutMoney(PutMoney const &) = delete;
    PutMoney(PutMoney &&) = delete;
    PutMoney & operator=(PutMoney const &) = delete;
    PutMoney & operator=(PutMoney &&) = delete;
    ~PutMoney() noexcept = default;

    friend inline std::ostream & operator<<(std::ostream & stream, PutMoney const & obj)
    {
      return stream << std::put_money(obj.amount_, obj.intl_);
    }

    bool intl_;
    std::string amount_;
  };

  [[nodiscard]] inline io1::money::PutMoney put_money(io1::money val, bool intl = false) noexcept
  {
    return io1::money::PutMoney(val, intl);
  }

  [[nodiscard]] inline io1::money::GetMoney get_money(io1::money & val, bool intl = false)
  {
    return io1::money::GetMoney(val, intl);
  }

  namespace detail
  {
    struct moneydiv_quotrem_t
    {
      money quot;
      money rem;
    };
    struct moneydiv_remquot_t
    {
      money rem;
      money quot;
    };
    constexpr bool is_quot_rem_v = 10 == std::div_t{10, 5}.quot;
  } // namespace detail

  using moneydiv_t = std::conditional_t<detail::is_quot_rem_v, detail::moneydiv_quotrem_t, detail::moneydiv_remquot_t>;

  [[nodiscard]] inline moneydiv_t div(money val, money::value_type divisor) noexcept
  {
    assert(0 != divisor && "Division by zero is undefined behavior.");

    auto const result = std::div(val.data(), divisor);

    return {.quot = money(result.quot), .rem = money(result.rem)};
  }

  inline namespace literals
  {
    template <char... STR>
    constexpr io1::money operator""_money() noexcept
    {
      return io1::detail::StringLitteralDecoder::apply<STR...>();
    }
  } // namespace literals
} // namespace io1

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

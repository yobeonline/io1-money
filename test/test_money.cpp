#include "io1/money.hpp"

#include <locale>
#include <memory>
#include <sstream>
#include <string_view>
#include <utility>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("Value bounds")
{
  [[maybe_unused]] constexpr auto max = 9'223'372'036'854'775'807_money;
  [[maybe_unused]] constexpr auto min = -9'223'372'036'854'775'807_money - 1_money;
}

TEST_CASE("Constexpr functions")
{
  {
    [[maybe_unused]] constexpr io1::Money m(1_money);
  }
  {
    [[maybe_unused]] constexpr io1::Money m = 1_money;
  }
  {
    [[maybe_unused]] constexpr auto m = 1_money ++;
  }
  {
    [[maybe_unused]] constexpr auto m = ++1_money;
  }
  {
    [[maybe_unused]] constexpr auto m = 1_money --;
  }
  {
    [[maybe_unused]] constexpr auto m = --1_money;
  }
  {
    [[maybe_unused]] constexpr auto m = 1_money *= 2;
  }
  {
    [[maybe_unused]] constexpr auto m = 10_money /= 2;
  }
  {
    [[maybe_unused]] constexpr auto m = 1_money += 2_money;
  }
  {
    [[maybe_unused]] constexpr auto m = 1_money -= 2_money;
  }
  {
    [[maybe_unused]] constexpr auto m = -1_money;
  }
  {
    [[maybe_unused]] constexpr auto m = -1_money;
  }
  {
    [[maybe_unused]] constexpr auto m = 1_money + 2_money;
  }
  {
    [[maybe_unused]] constexpr auto m = -1_money + 2_money;
  }
  {
    [[maybe_unused]] constexpr auto m = 1_money - 2_money;
  }
  {
    [[maybe_unused]] constexpr auto m = 1_money * 2;
  }
  {
    [[maybe_unused]] constexpr auto m = 10_money / 2;
  }
  {
    [[maybe_unused]] constexpr auto m = -10_money / 2;
  }
  {
    [[maybe_unused]] constexpr auto m = 2 * 1_money;
  }
  {
    [[maybe_unused]] constexpr auto m = -1_money * 2;
  }
  {
    [[maybe_unused]] constexpr auto m = 2 * -1_money;
  }
  {
    [[maybe_unused]] constexpr auto r = -1_money == -1_money;
  }
  {
    [[maybe_unused]] constexpr auto r = -1_money != -1_money;
  }
  {
    [[maybe_unused]] constexpr auto r = -1_money <= -1_money;
  }
  {
    [[maybe_unused]] constexpr auto r = -1_money >= -1_money;
  }
  {
    [[maybe_unused]] constexpr auto r = -1_money < -1_money;
  }
  {
    [[maybe_unused]] constexpr auto r = -1_money > -1_money;
  }
}

TEST_CASE("Constructors")
{
  CHECK_EQ(0, io1::Money(0).data());
  CHECK_EQ(20, io1::Money(20).data());
  CHECK_EQ(-20, io1::Money(-20).data());
}

TEST_CASE("Literal constructions")
{
  CHECK_EQ(0, (0_money).data());
  CHECK_EQ(0, (-0_money).data());
  CHECK_EQ(1, (1_money).data());
  CHECK_EQ(-1, (-1_money).data());
  CHECK_EQ(10, (10_money).data());
  CHECK_EQ(-15, (-15_money).data());
  CHECK_EQ(9'000'000'000'000'000'000, (9'000'000'000'000'000'000_money).data());
  CHECK_EQ(-9'000'000'000'000'000'000, (-9'000'000'000'000'000'000_money).data());

  CHECK_EQ(0, (0.00_money).data());
  CHECK_EQ(0, (-0.00_money).data());
  CHECK_EQ(1, (0.1_money).data());
  CHECK_EQ(-1, (-0.1_money).data());
  CHECK_EQ(1, (1._money).data());
  CHECK_EQ(-1, (-1._money).data());
  CHECK_EQ(112, (1.12_money).data());
  CHECK_EQ(-112, (-1.12_money).data());
  CHECK_EQ(101234, (10.1234_money).data());
  CHECK_EQ(-151, (-15.1_money).data());
  CHECK_EQ(9'000'000'000'000'000'000, (9.000'000'000'000'000'000_money).data());
  CHECK_EQ(-9'000'000'000'000'000'000, (-9.000'000'000'000'000'000_money).data());

  return;
}

TEST_CASE("Arithmetic")
{
  CHECK_EQ(25_money, 10_money += 15_money);
  CHECK_EQ(-5_money, 10_money -= 15_money);
  CHECK_EQ(5_money, 15_money -= 10_money);
  CHECK_EQ(25_money, 15_money += 10_money);
  CHECK_EQ(-5_money, -15_money += 10_money);
  CHECK_EQ(5_money, -10_money += 15_money);

  CHECK_EQ(25_money, 10_money + 15_money);
  CHECK_EQ(-5_money, 10_money - 15_money);
  CHECK_EQ(5_money, 15_money - 10_money);
  CHECK_EQ(25_money, 15_money + 10_money);
  CHECK_EQ(-5_money, -15_money + 10_money);
  CHECK_EQ(5_money, -10_money + 15_money);

  CHECK_EQ(5_money, ++4_money);
  CHECK_EQ(4_money, 4_money ++);
  CHECK_EQ(4_money, --5_money);
  CHECK_EQ(5_money, 5_money --);

  {
    auto m = 1_money;
    auto tmp = m++;
    CHECK_EQ(2_money, m);
    tmp = m--;
    CHECK_EQ(1_money, m);
    tmp = -m;
    CHECK_EQ(1_money, m);
  }

  CHECK_EQ(5_money, 1_money * 5);
  CHECK_EQ(5.010_money, 2.505_money * 2);
  CHECK_EQ(5_money, 5 * 1_money);
  CHECK_EQ(5.010_money, 2 * 2.505_money);
  CHECK_EQ(-5_money, 1_money * -5);
  CHECK_EQ(-5.010_money, 2.505_money * -2);
  CHECK_EQ(-5_money, -5 * 1_money);
  CHECK_EQ(-5.010_money, -2 * 2.505_money);

  CHECK_EQ(5_money, 1_money *= 5);
  CHECK_EQ(5.010_money, 2.505_money *= 2);
  CHECK_EQ(-5_money, 1_money *= -5);
  CHECK_EQ(-5.010_money, 2.505_money *= -2);

  CHECK_EQ(2_money, 4_money / 2);
  CHECK_EQ(-2_money, -4_money / 2);
  CHECK_EQ(2_money, -4_money / -2);
  CHECK_EQ(-2_money, 4_money / -2);
  CHECK_EQ(0_money, 0_money / -2);

  CHECK_EQ(2_money, 4_money /= 2);
  CHECK_EQ(-2_money, -4_money /= 2);
  CHECK_EQ(2_money, -4_money /= -2);
  CHECK_EQ(-2_money, 4_money /= -2);
  CHECK_EQ(0_money, 0_money /= -2);

  CHECK_THROWS_AS(4_money /= 3, io1::Money::InexactDivision);

  {
    auto m = 4_money;
    try
    {
      m /= 3;
    }
    catch (io1::Money::InexactDivision const & e)
    {
      CHECK_EQ(4, e.dividend);
      CHECK_EQ(3, e.divisor);
      CHECK_EQ(4_money, m);
    }
  }

  CHECK_THROWS_AS([[maybe_unused]] auto const m = 4_money / 3, io1::Money::InexactDivision);

  {
    auto m = 4_money;
    try
    {
      [[maybe_unused]] auto const m2 = m / 3;
    }
    catch (io1::Money::InexactDivision const & e)
    {
      CHECK_EQ(4, e.dividend);
      CHECK_EQ(3, e.divisor);
      CHECK_EQ(4_money, m);
    }
  }

  {
    auto const result = div(10_money, 2);
    CHECK_EQ(5_money, result.quot);
    CHECK_EQ(0_money, result.rem);
  }

  {
    auto const result = div(-10_money, 2);
    CHECK_EQ(-5_money, result.quot);
    CHECK_EQ(0_money, result.rem);
  }

  {
    auto const result = div(10_money, -2);
    CHECK_EQ(-5_money, result.quot);
    CHECK_EQ(0_money, result.rem);
  }

  {
    auto const result = div(-10_money, -2);
    CHECK_EQ(5_money, result.quot);
    CHECK_EQ(0_money, result.rem);
  }

  {
    auto const result = div(10_money, 3);
    CHECK_EQ(3_money, result.quot);
    CHECK_EQ(1_money, result.rem);
  }

  {
    auto const result = div(-10_money, 3);
    CHECK_EQ(-3_money, result.quot);
    CHECK_EQ(-1_money, result.rem);
  }

  {
    auto const result = div(10_money, -3);
    CHECK_EQ(-3_money, result.quot);
    CHECK_EQ(1_money, result.rem);
  }

  {
    auto const result = div(-10_money, -3);
    CHECK_EQ(3_money, result.quot);
    CHECK_EQ(-1_money, result.rem);
  }

  return;
}

TEST_CASE("Float arithmetic")
{
  CHECK_EQ(10_money, 2.f * 5_money);
  CHECK_EQ(-10_money, -2.f * 5_money);
  CHECK_EQ(-10_money, 2.f * -5_money);
  CHECK_EQ(10_money, -2.f * -5_money);
  CHECK_EQ(0_money, 0.f * 5_money);
  CHECK_EQ(0_money, 0.f * -5_money);

  CHECK_EQ(10_money, 5_money *= 2.f);
  CHECK_EQ(-10_money, 5_money *= -2.f);
  CHECK_EQ(-10_money, -5_money *= 2.f);
  CHECK_EQ(10_money, -5_money *= -2.f);
  CHECK_EQ(0_money, 5_money *= 0.f);
  CHECK_EQ(0_money, -5_money *= 0.f);

  CHECK_EQ(5_money, 10_money / 2.f);
  CHECK_EQ(-5_money, 10_money / -2.f);
  CHECK_EQ(-5_money, -10_money / 2.f);
  CHECK_EQ(5_money, -10_money / -2.f);
  CHECK_EQ(0_money, 0_money / 5.f);
  CHECK_EQ(0_money, 0_money / -5.f);

  CHECK_EQ(5_money, 10_money /= 2.f);
  CHECK_EQ(-5_money, 10_money /= -2.f);
  CHECK_EQ(-5_money, -10_money /= 2.f);
  CHECK_EQ(5_money, -10_money /= -2.f);
  CHECK_EQ(0_money, 0_money /= 5.f);
  CHECK_EQ(0_money, 0_money /= -5.f);

  return;
}

TEST_CASE("Float arithmetic order of evaluation")
{
  CHECK_EQ(2_money, 0.1 * 10. * 2_money);
  CHECK_EQ(2_money, 2_money *= 0.1 * 10.);

  return;
}

TEST_CASE("Rounding float arithmetic")
{
  CHECK_EQ(0_money, 0.1f * 1_money);
  CHECK_EQ(0_money, 0.5f * 1_money);
  CHECK_EQ(1_money, 0.9f * 1_money);
  CHECK_EQ(2_money, 2.1f * 1_money);
  CHECK_EQ(2_money, 2.5f * 1_money);
  CHECK_EQ(3_money, 2.9f * 1_money);
  CHECK_EQ(3_money, 3.1f * 1_money);
  CHECK_EQ(4_money, 3.5f * 1_money);
  CHECK_EQ(4_money, 3.9f * 1_money);

  CHECK_EQ(0_money, 1_money *= 0.1f);
  CHECK_EQ(0_money, 1_money *= 0.5f);
  CHECK_EQ(1_money, 1_money *= 0.9f);
  CHECK_EQ(2_money, 1_money *= 2.1f);
  CHECK_EQ(2_money, 1_money *= 2.5f);
  CHECK_EQ(3_money, 1_money *= 2.9f);
  CHECK_EQ(3_money, 1_money *= 3.1f);
  CHECK_EQ(4_money, 1_money *= 3.5f);
  CHECK_EQ(4_money, 1_money *= 3.9f);

  CHECK_EQ(0_money, -0.1f * 1_money);
  CHECK_EQ(0_money, -0.5f * 1_money);
  CHECK_EQ(-1_money, -0.9f * 1_money);
  CHECK_EQ(-2_money, -2.1f * 1_money);
  CHECK_EQ(-2_money, -2.5f * 1_money);
  CHECK_EQ(-3_money, -2.9f * 1_money);
  CHECK_EQ(-3_money, -3.1f * 1_money);
  CHECK_EQ(-4_money, -3.5f * 1_money);
  CHECK_EQ(-4_money, -3.9f * 1_money);

  CHECK_EQ(0_money, 1_money *= -0.1f);
  CHECK_EQ(0_money, 1_money *= -0.5f);
  CHECK_EQ(-1_money, 1_money *= -0.9f);
  CHECK_EQ(-2_money, 1_money *= -2.1f);
  CHECK_EQ(-2_money, 1_money *= -2.5f);
  CHECK_EQ(-3_money, 1_money *= -2.9f);
  CHECK_EQ(-3_money, 1_money *= -3.1f);
  CHECK_EQ(-4_money, 1_money *= -3.5f);
  CHECK_EQ(-4_money, 1_money *= -3.9f);

  CHECK_EQ(0_money, 0.1f * -1_money);
  CHECK_EQ(0_money, 0.5f * -1_money);
  CHECK_EQ(-1_money, 0.9f * -1_money);
  CHECK_EQ(-2_money, 2.1f * -1_money);
  CHECK_EQ(-2_money, 2.5f * -1_money);
  CHECK_EQ(-3_money, 2.9f * -1_money);
  CHECK_EQ(-3_money, 3.1f * -1_money);
  CHECK_EQ(-4_money, 3.5f * -1_money);
  CHECK_EQ(-4_money, 3.9f * -1_money);

  CHECK_EQ(0_money, -1_money *= 0.1f);
  CHECK_EQ(0_money, -1_money *= 0.5f);
  CHECK_EQ(-1_money, -1_money *= 0.9f);
  CHECK_EQ(-2_money, -1_money *= 2.1f);
  CHECK_EQ(-2_money, -1_money *= 2.5f);
  CHECK_EQ(-3_money, -1_money *= 2.9f);
  CHECK_EQ(-3_money, -1_money *= 3.1f);
  CHECK_EQ(-4_money, -1_money *= 3.5f);
  CHECK_EQ(-4_money, -1_money *= 3.9f);

  CHECK_EQ(0_money, -0.1f * -1_money);
  CHECK_EQ(0_money, -0.5f * -1_money);
  CHECK_EQ(1_money, -0.9f * -1_money);
  CHECK_EQ(2_money, -2.1f * -1_money);
  CHECK_EQ(2_money, -2.5f * -1_money);
  CHECK_EQ(3_money, -2.9f * -1_money);
  CHECK_EQ(3_money, -3.1f * -1_money);
  CHECK_EQ(4_money, -3.5f * -1_money);
  CHECK_EQ(4_money, -3.9f * -1_money);

  CHECK_EQ(0_money, -1_money *= -0.1f);
  CHECK_EQ(0_money, -1_money *= -0.5f);
  CHECK_EQ(1_money, -1_money *= -0.9f);
  CHECK_EQ(2_money, -1_money *= -2.1f);
  CHECK_EQ(2_money, -1_money *= -2.5f);
  CHECK_EQ(3_money, -1_money *= -2.9f);
  CHECK_EQ(3_money, -1_money *= -3.1f);
  CHECK_EQ(4_money, -1_money *= -3.5f);
  CHECK_EQ(4_money, -1_money *= -3.9f);

  CHECK_EQ(1_money, 1_money / 1.9f);
  CHECK_EQ(0_money, 1_money / 2.f);
  CHECK_EQ(0_money, 1_money / 2.1f);
  CHECK_EQ(2_money, 3_money / 1.9f);
  CHECK_EQ(2_money, 3_money / 2.f);
  CHECK_EQ(1_money, 3_money / 2.1f);
  CHECK_EQ(3_money, 5_money / 1.9f);
  CHECK_EQ(2_money, 5_money / 2.f);
  CHECK_EQ(2_money, 5_money / 2.1f);

  CHECK_EQ(1_money, 1_money /= 1.9f);
  CHECK_EQ(0_money, 1_money /= 2.f);
  CHECK_EQ(0_money, 1_money /= 2.1f);
  CHECK_EQ(2_money, 3_money /= 1.9f);
  CHECK_EQ(2_money, 3_money /= 2.f);
  CHECK_EQ(1_money, 3_money /= 2.1f);
  CHECK_EQ(3_money, 5_money /= 1.9f);
  CHECK_EQ(2_money, 5_money /= 2.f);
  CHECK_EQ(2_money, 5_money /= 2.1f);

  CHECK_EQ(-1_money, -1_money / 1.9f);
  CHECK_EQ(0_money, -1_money / 2.f);
  CHECK_EQ(0_money, -1_money / 2.1f);
  CHECK_EQ(-2_money, -3_money / 1.9f);
  CHECK_EQ(-2_money, -3_money / 2.f);
  CHECK_EQ(-1_money, -3_money / 2.1f);
  CHECK_EQ(-3_money, -5_money / 1.9f);
  CHECK_EQ(-2_money, -5_money / 2.f);
  CHECK_EQ(-2_money, -5_money / 2.1f);

  CHECK_EQ(-1_money, -1_money /= 1.9f);
  CHECK_EQ(0_money, -1_money /= 2.f);
  CHECK_EQ(0_money, -1_money /= 2.1f);
  CHECK_EQ(-2_money, -3_money /= 1.9f);
  CHECK_EQ(-2_money, -3_money /= 2.f);
  CHECK_EQ(-1_money, -3_money /= 2.1f);
  CHECK_EQ(-3_money, -5_money /= 1.9f);
  CHECK_EQ(-2_money, -5_money /= 2.f);
  CHECK_EQ(-2_money, -5_money /= 2.1f);

  CHECK_EQ(-1_money, 1_money / -1.9f);
  CHECK_EQ(0_money, 1_money / -2.f);
  CHECK_EQ(0_money, 1_money / -2.1f);
  CHECK_EQ(-2_money, 3_money / -1.9f);
  CHECK_EQ(-2_money, 3_money / -2.f);
  CHECK_EQ(-1_money, 3_money / -2.1f);
  CHECK_EQ(-3_money, 5_money / -1.9f);
  CHECK_EQ(-2_money, 5_money / -2.f);
  CHECK_EQ(-2_money, 5_money / -2.1f);

  CHECK_EQ(-1_money, 1_money /= -1.9f);
  CHECK_EQ(0_money, 1_money /= -2.f);
  CHECK_EQ(0_money, 1_money /= -2.1f);
  CHECK_EQ(-2_money, 3_money /= -1.9f);
  CHECK_EQ(-2_money, 3_money /= -2.f);
  CHECK_EQ(-1_money, 3_money /= -2.1f);
  CHECK_EQ(-3_money, 5_money /= -1.9f);
  CHECK_EQ(-2_money, 5_money /= -2.f);
  CHECK_EQ(-2_money, 5_money /= -2.1f);

  CHECK_EQ(1_money, -1_money / -1.9f);
  CHECK_EQ(0_money, -1_money / -2.f);
  CHECK_EQ(0_money, -1_money / -2.1f);
  CHECK_EQ(2_money, -3_money / -1.9f);
  CHECK_EQ(2_money, -3_money / -2.f);
  CHECK_EQ(1_money, -3_money / -2.1f);
  CHECK_EQ(3_money, -5_money / -1.9f);
  CHECK_EQ(2_money, -5_money / -2.f);
  CHECK_EQ(2_money, -5_money / -2.1f);

  CHECK_EQ(1_money, -1_money /= -1.9f);
  CHECK_EQ(0_money, -1_money /= -2.f);
  CHECK_EQ(0_money, -1_money /= -2.1f);
  CHECK_EQ(2_money, -3_money /= -1.9f);
  CHECK_EQ(2_money, -3_money /= -2.f);
  CHECK_EQ(1_money, -3_money /= -2.1f);
  CHECK_EQ(3_money, -5_money /= -1.9f);
  CHECK_EQ(2_money, -5_money /= -2.f);
  CHECK_EQ(2_money, -5_money /= -2.1f);

  return;
}

namespace
{
  // a test facet for monetary input / outputs
  class moneypunct_facet : public std::moneypunct<char, false>
  {
  private:
    char do_decimal_point() const override { return '_'; };
    char do_thousands_sep() const override { return '-'; };
    std::string do_grouping() const override { return "\002"; };
    std::string do_curr_symbol() const override { return "$$"; };
    std::string do_positive_sign() const override { return "++"; };
    std::string do_negative_sign() const override { return "--"; };
    int do_frac_digits() const override { return 1; };
    pattern do_pos_format() const override { return {sign, space, value, symbol}; };
    pattern do_neg_format() const override { return {sign, space, value, symbol}; };
  };

  class intl_moneypunct_facet : public std::moneypunct<char, true>
  {
  private:
    char do_decimal_point() const override { return '_'; };
    char do_thousands_sep() const override { return '-'; };
    std::string do_grouping() const override { return "\002"; };
    std::string do_curr_symbol() const override { return "CUR "; };
    std::string do_positive_sign() const override { return "++"; };
    std::string do_negative_sign() const override { return "--"; };
    int do_frac_digits() const override { return 1; };
    pattern do_pos_format() const override { return {sign, space, value, symbol}; };
    pattern do_neg_format() const override { return {sign, space, value, symbol}; };
  };
} // namespace

TEST_CASE("Classic format")
{
  std::stringstream stream;

  stream << 0_money << ' ' << -0_money << ' ' << 1_money << ' ' << -1_money << ' ' << 1.5_money << ' ' << -1.5_money
         << ' ' << -0.1_money << ' ' << 0.1_money << ' ' << -123456_money << ' ' << 123456_money << ' '
         << -123.456_money << ' ' << 123.456_money;

  CHECK_EQ("0 0 1 -1 15 -15 -1 1 -123456 123456 -123456 123456", stream.str());

  return;
}

TEST_CASE("Format")
{
  std::stringstream stream;
  stream.imbue(std::locale(stream.getloc(), std::make_unique<moneypunct_facet>().release()));

  CHECK_NOTHROW(stream << put_money(1.5_money) << ' ' << put_money(-1.5_money) << ' ' << put_money(-123456_money) << ' '
                       << put_money(123456_money) << ' ' << put_money(-123.456_money) << ' '
                       << put_money(123.456_money));
  CHECK_EQ("+ 1_5+ - 1_5- - 1-23-45_6- + 1-23-45_6+ - 1-23-45_6- + 1-23-45_6+", stream.str());

  return;
}

TEST_CASE("Format leading zero")
{
  std::stringstream stream;
  stream.imbue(std::locale(stream.getloc(), std::make_unique<moneypunct_facet>().release()));

  CHECK_NOTHROW(stream << put_money(0_money) << ' ' << put_money(-0_money) << ' ' << put_money(1_money) << ' '
                       << put_money(-1_money) << ' ' << put_money(-0.1_money) << ' ' << put_money(0.1_money));

#ifdef __GNUC__
  {
    CHECK("+ _0+ + _0+ + _1+ - _1- - _1- + _1+" == stream.str());
  }
#else
  {
    CHECK("+ 0_0+ + 0_0+ + 0_1+ - 0_1- - 0_1- + 0_1+" == stream.str());
  }
#endif

  return;
}

TEST_CASE("Formatter")
{
  // without the m or M type specifier, money behaves as an int
  CHECK("1235" == std::format("{}", 12.35_money));

  CHECK("1235" == std::format("{:<}", 12.35_money));
  CHECK("1235" == std::format("{:>}", 12.35_money));
  CHECK("1235" == std::format("{:^}", 12.35_money));
  CHECK("1235" == std::format("{:*<}", 12.35_money));
  CHECK("1235" == std::format("{:*>}", 12.35_money));
  CHECK("1235" == std::format("{:*^}", 12.35_money));

  CHECK("  1235" == std::format("{:6}", 12.35_money));
  CHECK("  1235" == std::format("{:{}}", 12.35_money, 6));
  CHECK("1235  " == std::format("{:<6}", 12.35_money));
  CHECK("  1235" == std::format("{:>6}", 12.35_money));
  CHECK(" 1235 " == std::format("{:^6}", 12.35_money));
  CHECK(" 1235  " == std::format("{:^7}", 12.35_money));

  CHECK("1235**" == std::format("{:*<6}", 12.35_money));
  CHECK("**1235" == std::format("{:*>6}", 12.35_money));
  CHECK("*1235*" == std::format("{:*^6}", 12.35_money));
  CHECK("*1235**" == std::format("{:*^7}", 12.35_money));

  CHECK("01235" == std::format("{:05}", 12.35_money));
  CHECK("1235" == std::format("{:04}", 12.35_money));

  CHECK("+1235" == std::format("{:+5}", 12.35_money));
  CHECK(" 1235" == std::format("{: 4}", 12.35_money));

  // with m or M
  std::stringstream str;
  CHECK("+ 1-23-45_6+" ==
        std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()), "{:m}", 123456_money));
  CHECK("+ 1-23-45_6$$+" ==
        std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()), "{:#m}", 123456_money));
  CHECK("+ 1-23-45_6+" == std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()),
                                      "{:M}", 123456_money));
  CHECK("+ 1-23-45_6CUR +" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:#M}",
                    123456_money));

  CHECK("+ 1-23-45_6+" ==
        std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()), "{:<m}", 123456_money));
  CHECK("+ 1-23-45_6$$+" ==
        std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()), "{:<#m}", 123456_money));
  CHECK("+ 1-23-45_6+" == std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()),
                                      "{:<M}", 123456_money));
  CHECK("+ 1-23-45_6CUR +" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:<#M}",
                    123456_money));

  CHECK("+ 1-23-45_6+" ==
        std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()), "{:>m}", 123456_money));
  CHECK("+ 1-23-45_6$$+" ==
        std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()), "{:>#m}", 123456_money));
  CHECK("+ 1-23-45_6+" == std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()),
                                      "{:>M}", 123456_money));
  CHECK("+ 1-23-45_6CUR +" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:>#M}",
                    123456_money));

  CHECK("+ 1-23-45_6+" ==
        std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()), "{:^m}", 123456_money));
  CHECK("+ 1-23-45_6$$+" ==
        std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()), "{:^#m}", 123456_money));
  CHECK("+ 1-23-45_6+" == std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()),
                                      "{:^M}", 123456_money));
  CHECK("+ 1-23-45_6CUR +" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:^#M}",
                    123456_money));

  CHECK("+ 1-23-45_6+   " == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:<15m}", 123456_money));
  CHECK("+ 1-23-45_6$$+ " == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:<15#m}", 123456_money));
  CHECK("+ 1-23-45_6+        " ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:<20M}",
                    123456_money));
  CHECK("+ 1-23-45_6CUR +    " ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:<20#M}",
                    123456_money));

  CHECK("+ 1-23-45_6$$+  foo" == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                             "{:<15#m} foo", 123456_money));

  CHECK("   + 1-23-45_6+" == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:>15m}", 123456_money));
  CHECK(" + 1-23-45_6$$+" == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:>15#m}", 123456_money));
  CHECK("        + 1-23-45_6+" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:>20M}",
                    123456_money));
  CHECK("    + 1-23-45_6CUR +" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:>20#M}",
                    123456_money));

  CHECK(" + 1-23-45_6+  " == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:^15m}", 123456_money));
  CHECK("+ 1-23-45_6$$+ " == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:^15#m}", 123456_money));
  CHECK("    + 1-23-45_6+    " ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:^20M}",
                    123456_money));
  CHECK("  + 1-23-45_6CUR +  " ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:^20#M}",
                    123456_money));

  CHECK("+ 1-23-45_6+***" == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:*<15m}", 123456_money));
  CHECK("+ 1-23-45_6$$+*" == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:*<15#m}", 123456_money));
  CHECK("+ 1-23-45_6+********" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:*<20M}",
                    123456_money));
  CHECK("+ 1-23-45_6CUR +****" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:*<20#M}",
                    123456_money));

  CHECK("***+ 1-23-45_6+" == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:*>15m}", 123456_money));
  CHECK("*+ 1-23-45_6$$+" == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:*>15#m}", 123456_money));
  CHECK("********+ 1-23-45_6+" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:*>20M}",
                    123456_money));
  CHECK("****+ 1-23-45_6CUR +" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:*>20#M}",
                    123456_money));

  CHECK("*+ 1-23-45_6+**" == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:*^15m}", 123456_money));
  CHECK("+ 1-23-45_6$$+*" == std::format(std::locale(str.getloc(), std::make_unique<moneypunct_facet>().release()),
                                         "{:*^15#m}", 123456_money));
  CHECK("****+ 1-23-45_6+****" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:*^20M}",
                    123456_money));
  CHECK("**+ 1-23-45_6CUR +**" ==
        std::format(std::locale(str.getloc(), std::make_unique<intl_moneypunct_facet>().release()), "{:*^20#M}",
                    123456_money));

  return;
}

TEST_CASE("Parse classic")
{
  std::stringstream stream;
  stream << "0 1 -1 15 -15 20745 -20745 90000000000000000000 $9";

  io1::Money m;
  CHECK_NOTHROW(stream >> m);
  CHECK_EQ(0_money, m);
  CHECK_NOTHROW(stream >> m);
  CHECK_EQ(1_money, m);
  CHECK_NOTHROW(stream >> m);
  CHECK_EQ(-1_money, m);
  CHECK_NOTHROW(stream >> m);
  CHECK_EQ(1.5_money, m);
  CHECK_NOTHROW(stream >> m);
  CHECK_EQ(-1.5_money, m);
  CHECK_NOTHROW(stream >> m);
  CHECK_EQ(2074.5_money, m);
  CHECK_NOTHROW(stream >> m);
  CHECK_EQ(-2074.5_money, m);
  CHECK_NOTHROW(stream >> m);
  CHECK_EQ(false, static_cast<bool>(stream));
  stream.clear();
  CHECK_NOTHROW(stream >> m);
  CHECK_EQ(false, static_cast<bool>(stream));
  stream.clear();

  return;
}

TEST_CASE("Parse")
{
  std::stringstream stream;
  stream << "+ 0_0+ + 0_1+ - 0_1- + _0+ + _1+ - _1- + 1_5+ - 1_5- + 20-74_5+ - 20-74_5- + "
            "9-00-00-00-00-00-00-00-00-00_0+ -9_0-";

  stream.imbue(std::locale(stream.getloc(), new moneypunct_facet));

  io1::Money m;
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(0_money, m);
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(1_money, m);
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(-1_money, m);
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(0_money, m);
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(1_money, m);
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(-1_money, m);
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(1.5_money, m);
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(-1.5_money, m);
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(2074.5_money, m);
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(-2074.5_money, m);
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(false, static_cast<bool>(stream));
  stream.clear();
  CHECK_NOTHROW(stream >> get_money(m));
  CHECK_EQ(false, static_cast<bool>(stream));
  stream.clear();

  return;
}

TEST_CASE("Comparisons")
{
  CHECK(1_money == 1_money);
  CHECK(!(2_money == 1_money));
  CHECK(1_money == -(-1_money));
  CHECK(-(-1_money) == 1_money);
  CHECK(-1_money == -1_money);

  CHECK(-1_money != 1_money);
  CHECK(1_money != -1_money);
  CHECK(!(1_money != 1_money));
  CHECK(-1_money != 1_money);

  CHECK(-1_money <= 1_money);
  CHECK(-1_money <= -1_money);
  CHECK(-1_money < 1_money);
  CHECK(!(1_money <= -1_money));
  CHECK(!(1_money < 1_money));

  CHECK(1_money >= -1_money);
  CHECK(1_money >= 1_money);
  CHECK(1_money > -1_money);
  CHECK(!(-1_money >= 1_money));
  CHECK(!(1_money > 1_money));

  CHECK(1_money <= 2_money);
  CHECK(1_money < 2_money);
  CHECK(!(2_money < 2_money));
  CHECK(!(2_money <= 1_money));

  CHECK(2_money >= 1_money);
  CHECK(2_money > 1_money);
  CHECK(!(2_money > 2_money));
  CHECK(!(1_money >= 2_money));

  return;
}
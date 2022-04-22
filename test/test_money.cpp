#include "money.hpp"

#include <locale>
#include <sstream>
#include <string_view>
#include <utility>

#define BOOST_TEST_MODULE io1::Money test
#include <boost/test/unit_test.hpp>
#include <boost/type_traits/is_detected.hpp>

BOOST_AUTO_TEST_CASE(Bounds)
{
  constexpr auto max = 9'223'372'036'854'775'807_money;
  constexpr auto min = -9'223'372'036'854'775'807_money - 1_money;
}

BOOST_AUTO_TEST_CASE(Constexpr)
{
  {
    constexpr io1::Money m(1_money);
  }
  {
    constexpr io1::Money m = 1_money;
  }
  {
    constexpr auto m = 1_money ++;
  }
  {
    constexpr auto m = ++1_money;
  }
  {
    constexpr auto m = 1_money --;
  }
  {
    constexpr auto m = --1_money;
  }
  {
    constexpr auto m = 1_money *= 2;
  }
  {
    constexpr auto m = 10_money /= 2;
  }
  {
    constexpr auto m = 1_money += 2_money;
  }
  {
    constexpr auto m = 1_money -= 2_money;
  }
  {
    constexpr auto m = -1_money;
  }
  {
    constexpr auto m = -1_money;
  }
  {
    constexpr auto m = 1_money + 2_money;
  }
  {
    constexpr auto m = -1_money + 2_money;
  }
  {
    constexpr auto m = 1_money - 2_money;
  }
  {
    constexpr auto m = 1_money * 2;
  }
  {
    constexpr auto m = 10_money / 2;
  }
  {
    constexpr auto m = -10_money / 2;
  }
  {
    constexpr auto m = 2 * 1_money;
  }
  {
    constexpr auto m = -1_money * 2;
  }
  {
    constexpr auto m = 2 * -1_money;
  }
  {
    constexpr auto r = -1_money == -1_money;
  }
  {
    constexpr auto r = -1_money != -1_money;
  }
  {
    constexpr auto r = -1_money <= -1_money;
  }
  {
    constexpr auto r = -1_money >= -1_money;
  }
  {
    constexpr auto r = -1_money < -1_money;
  }
  {
    constexpr auto r = -1_money > -1_money;
  }
}

template <typename T>
using floating_point_constructor = decltype(io1::Money(std::declval<T>()));
template <typename T>
using floating_point_multiplication_assignment = decltype(std::declval<io1::Money>().operator*=<T>(std::declval<T>()));
template <typename T>
using floating_point_division_assignment = decltype(std::declval<io1::Money>().operator/=<T>(std::declval<T>()));
template <typename T>
using floating_point_division = decltype(io1::operator/<T>(std::declval<io1::Money>(), std::declval<T>()));
template <typename T>
using floating_point_left_multiplication = decltype(io1::operator*<T>(std::declval<T>(), std::declval<io1::Money>()));
template <typename T>
using floating_point_right_multiplication = decltype(io1::operator*<T>(std::declval<io1::Money>(), std::declval<T>()));
template <typename T = long double>
using floating_point_right_multiplication_rounding =
    decltype(io1::operator*(std::declval<io1::Money>(), std::declval<T>()));

BOOST_AUTO_TEST_CASE(DeletedFunctions) // template instances that must not compile
{
  BOOST_TEST((boost::is_detected_v<floating_point_constructor, int>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_constructor, float>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_constructor, double>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_constructor, long double>));

  BOOST_TEST((boost::is_detected_v<floating_point_multiplication_assignment, int>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_multiplication_assignment, float>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_multiplication_assignment, double>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_multiplication_assignment, long double>));

  BOOST_TEST((boost::is_detected_v<floating_point_division_assignment, int>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_division_assignment, float>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_division_assignment, double>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_division_assignment, long double>));

  BOOST_TEST((boost::is_detected_v<floating_point_division, int>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_division, float>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_division, double>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_division, long double>));

  BOOST_TEST((boost::is_detected_v<floating_point_left_multiplication, int>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_left_multiplication, float>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_left_multiplication, double>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_left_multiplication, long double>));

  BOOST_TEST((boost::is_detected_v<floating_point_right_multiplication, int>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_right_multiplication, float>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_right_multiplication, double>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_right_multiplication, long double>));

  BOOST_TEST((boost::is_detected_v<floating_point_right_multiplication_rounding, int>));
  BOOST_TEST(!(boost::is_detected_v<floating_point_right_multiplication_rounding>));
}

BOOST_AUTO_TEST_CASE(Constructors)
{
  BOOST_REQUIRE_EQUAL(0, io1::Money(0).data());
  BOOST_REQUIRE_EQUAL(20, io1::Money(20).data());
  BOOST_REQUIRE_EQUAL(-20, io1::Money(-20).data());
}

BOOST_AUTO_TEST_CASE(LiteralConstructions)
{
  BOOST_REQUIRE_EQUAL(0, (0_money).data());
  BOOST_REQUIRE_EQUAL(0, (-0_money).data());
  BOOST_REQUIRE_EQUAL(1, (1_money).data());
  BOOST_REQUIRE_EQUAL(-1, (-1_money).data());
  BOOST_REQUIRE_EQUAL(10, (10_money).data());
  BOOST_REQUIRE_EQUAL(-15, (-15_money).data());
  BOOST_REQUIRE_EQUAL(9'000'000'000'000'000'000, (9'000'000'000'000'000'000_money).data());
  BOOST_REQUIRE_EQUAL(-9'000'000'000'000'000'000, (-9'000'000'000'000'000'000_money).data());

  BOOST_REQUIRE_EQUAL(0, (0.00_money).data());
  BOOST_REQUIRE_EQUAL(0, (-0.00_money).data());
  BOOST_REQUIRE_EQUAL(1, (0.1_money).data());
  BOOST_REQUIRE_EQUAL(-1, (-0.1_money).data());
  BOOST_REQUIRE_EQUAL(1, (1._money).data());
  BOOST_REQUIRE_EQUAL(-1, (-1._money).data());
  BOOST_REQUIRE_EQUAL(112, (1.12_money).data());
  BOOST_REQUIRE_EQUAL(-112, (-1.12_money).data());
  BOOST_REQUIRE_EQUAL(101234, (10.1234_money).data());
  BOOST_REQUIRE_EQUAL(-151, (-15.1_money).data());
  BOOST_REQUIRE_EQUAL(9'000'000'000'000'000'000, (9.000'000'000'000'000'000_money).data());
  BOOST_REQUIRE_EQUAL(-9'000'000'000'000'000'000, (-9.000'000'000'000'000'000_money).data());

  return;
}

BOOST_AUTO_TEST_CASE(Arithmetic)
{
  BOOST_REQUIRE_EQUAL(25_money, 10_money += 15_money);
  BOOST_REQUIRE_EQUAL(-5_money, 10_money -= 15_money);
  BOOST_REQUIRE_EQUAL(5_money, 15_money -= 10_money);
  BOOST_REQUIRE_EQUAL(25_money, 15_money += 10_money);
  BOOST_REQUIRE_EQUAL(-5_money, -15_money += 10_money);
  BOOST_REQUIRE_EQUAL(5_money, -10_money += 15_money);

  BOOST_REQUIRE_EQUAL(25_money, 10_money + 15_money);
  BOOST_REQUIRE_EQUAL(-5_money, 10_money - 15_money);
  BOOST_REQUIRE_EQUAL(5_money, 15_money - 10_money);
  BOOST_REQUIRE_EQUAL(25_money, 15_money + 10_money);
  BOOST_REQUIRE_EQUAL(-5_money, -15_money + 10_money);
  BOOST_REQUIRE_EQUAL(5_money, -10_money + 15_money);

  BOOST_REQUIRE_EQUAL(5_money, ++4_money);
  BOOST_REQUIRE_EQUAL(4_money, 4_money ++);
  BOOST_REQUIRE_EQUAL(4_money, --5_money);
  BOOST_REQUIRE_EQUAL(5_money, 5_money --);

  {
    auto m = 1_money;
    auto tmp = m++;
    BOOST_REQUIRE_EQUAL(2_money, m);
    tmp = m--;
    BOOST_REQUIRE_EQUAL(1_money, m);
    tmp = -m;
    BOOST_REQUIRE_EQUAL(1_money, m);
  }

  BOOST_REQUIRE_EQUAL(5_money, 1_money * 5);
  BOOST_REQUIRE_EQUAL(5.010_money, 2.505_money * 2);
  BOOST_REQUIRE_EQUAL(5_money, 5 * 1_money);
  BOOST_REQUIRE_EQUAL(5.010_money, 2 * 2.505_money);
  BOOST_REQUIRE_EQUAL(-5_money, 1_money * -5);
  BOOST_REQUIRE_EQUAL(-5.010_money, 2.505_money * -2);
  BOOST_REQUIRE_EQUAL(-5_money, -5 * 1_money);
  BOOST_REQUIRE_EQUAL(-5.010_money, -2 * 2.505_money);

  BOOST_REQUIRE_EQUAL(5_money, 1_money *= 5);
  BOOST_REQUIRE_EQUAL(5.010_money, 2.505_money *= 2);
  BOOST_REQUIRE_EQUAL(-5_money, 1_money *= -5);
  BOOST_REQUIRE_EQUAL(-5.010_money, 2.505_money *= -2);

  BOOST_REQUIRE_EQUAL(2_money, 4_money / 2);
  BOOST_REQUIRE_EQUAL(-2_money, -4_money / 2);
  BOOST_REQUIRE_EQUAL(2_money, -4_money / -2);
  BOOST_REQUIRE_EQUAL(-2_money, 4_money / -2);
  BOOST_REQUIRE_EQUAL(0_money, 0_money / -2);

  BOOST_REQUIRE_EQUAL(2_money, 4_money /= 2);
  BOOST_REQUIRE_EQUAL(-2_money, -4_money /= 2);
  BOOST_REQUIRE_EQUAL(2_money, -4_money /= -2);
  BOOST_REQUIRE_EQUAL(-2_money, 4_money /= -2);
  BOOST_REQUIRE_EQUAL(0_money, 0_money /= -2);

  BOOST_REQUIRE_THROW(4_money /= 3, io1::Money::InexactDivision);

  {
    auto m = 4_money;
    try
    {
      m /= 3;
    }
    catch (io1::Money::InexactDivision const & e)
    {
      BOOST_REQUIRE_EQUAL(4, e.dividend);
      BOOST_REQUIRE_EQUAL(3, e.divisor);
      BOOST_REQUIRE_EQUAL(4_money, m);
    }
  }

  BOOST_REQUIRE_THROW(auto const m = 4_money / 3, io1::Money::InexactDivision);

  {
    auto m = 4_money;
    try
    {
      auto const m2 = m / 3;
    }
    catch (io1::Money::InexactDivision const & e)
    {
      BOOST_REQUIRE_EQUAL(4, e.dividend);
      BOOST_REQUIRE_EQUAL(3, e.divisor);
      BOOST_REQUIRE_EQUAL(4_money, m);
    }
  }

  {
    auto const result = div(10_money, 2);
    BOOST_REQUIRE_EQUAL(5_money, result.quot);
    BOOST_REQUIRE_EQUAL(0_money, result.rem);
  }

  {
    auto const result = div(-10_money, 2);
    BOOST_REQUIRE_EQUAL(-5_money, result.quot);
    BOOST_REQUIRE_EQUAL(0_money, result.rem);
  }

  {
    auto const result = div(10_money, -2);
    BOOST_REQUIRE_EQUAL(-5_money, result.quot);
    BOOST_REQUIRE_EQUAL(0_money, result.rem);
  }

  {
    auto const result = div(-10_money, -2);
    BOOST_REQUIRE_EQUAL(5_money, result.quot);
    BOOST_REQUIRE_EQUAL(0_money, result.rem);
  }

  {
    auto const result = div(10_money, 3);
    BOOST_REQUIRE_EQUAL(3_money, result.quot);
    BOOST_REQUIRE_EQUAL(1_money, result.rem);
  }

  {
    auto const result = div(-10_money, 3);
    BOOST_REQUIRE_EQUAL(-3_money, result.quot);
    BOOST_REQUIRE_EQUAL(-1_money, result.rem);
  }

  {
    auto const result = div(10_money, -3);
    BOOST_REQUIRE_EQUAL(-3_money, result.quot);
    BOOST_REQUIRE_EQUAL(1_money, result.rem);
  }

  {
    auto const result = div(-10_money, -3);
    BOOST_REQUIRE_EQUAL(3_money, result.quot);
    BOOST_REQUIRE_EQUAL(-1_money, result.rem);
  }

  return;
}

BOOST_AUTO_TEST_CASE(FloatArithmetic)
{
  BOOST_REQUIRE_EQUAL(10_money, 2.f * 5_money);
  BOOST_REQUIRE_EQUAL(-10_money, -2.f * 5_money);
  BOOST_REQUIRE_EQUAL(-10_money, 2.f * -5_money);
  BOOST_REQUIRE_EQUAL(10_money, -2.f * -5_money);
  BOOST_REQUIRE_EQUAL(0_money, 0.f * 5_money);
  BOOST_REQUIRE_EQUAL(0_money, 0.f * -5_money);

  BOOST_REQUIRE_EQUAL(10_money, 5_money *= 2.f);
  BOOST_REQUIRE_EQUAL(-10_money, 5_money *= -2.f);
  BOOST_REQUIRE_EQUAL(-10_money, -5_money *= 2.f);
  BOOST_REQUIRE_EQUAL(10_money, -5_money *= -2.f);
  BOOST_REQUIRE_EQUAL(0_money, 5_money *= 0.f);
  BOOST_REQUIRE_EQUAL(0_money, -5_money *= 0.f);

  BOOST_REQUIRE_EQUAL(5_money, 10_money / 2.f);
  BOOST_REQUIRE_EQUAL(-5_money, 10_money / -2.f);
  BOOST_REQUIRE_EQUAL(-5_money, -10_money / 2.f);
  BOOST_REQUIRE_EQUAL(5_money, -10_money / -2.f);
  BOOST_REQUIRE_EQUAL(0_money, 0_money / 5.f);
  BOOST_REQUIRE_EQUAL(0_money, 0_money / -5.f);

  BOOST_REQUIRE_EQUAL(5_money, 10_money /= 2.f);
  BOOST_REQUIRE_EQUAL(-5_money, 10_money /= -2.f);
  BOOST_REQUIRE_EQUAL(-5_money, -10_money /= 2.f);
  BOOST_REQUIRE_EQUAL(5_money, -10_money /= -2.f);
  BOOST_REQUIRE_EQUAL(0_money, 0_money /= 5.f);
  BOOST_REQUIRE_EQUAL(0_money, 0_money /= -5.f);

  return;
}

BOOST_AUTO_TEST_CASE(FloatArithmeticOrderOfEvaluation)
{
  BOOST_REQUIRE_EQUAL(2_money, 0.1 * 10. * 2_money);
  BOOST_REQUIRE_EQUAL(2_money, 2_money *= 0.1 * 10.);

  return;
}

BOOST_AUTO_TEST_CASE(RoundingFloatArithmetic)
{
  BOOST_REQUIRE_EQUAL(0_money, 0.1f * 1_money);
  BOOST_REQUIRE_EQUAL(0_money, 0.5f * 1_money);
  BOOST_REQUIRE_EQUAL(1_money, 0.9f * 1_money);
  BOOST_REQUIRE_EQUAL(2_money, 2.1f * 1_money);
  BOOST_REQUIRE_EQUAL(2_money, 2.5f * 1_money);
  BOOST_REQUIRE_EQUAL(3_money, 2.9f * 1_money);
  BOOST_REQUIRE_EQUAL(3_money, 3.1f * 1_money);
  BOOST_REQUIRE_EQUAL(4_money, 3.5f * 1_money);
  BOOST_REQUIRE_EQUAL(4_money, 3.9f * 1_money);

  BOOST_REQUIRE_EQUAL(0_money, 1_money *= 0.1f);
  BOOST_REQUIRE_EQUAL(0_money, 1_money *= 0.5f);
  BOOST_REQUIRE_EQUAL(1_money, 1_money *= 0.9f);
  BOOST_REQUIRE_EQUAL(2_money, 1_money *= 2.1f);
  BOOST_REQUIRE_EQUAL(2_money, 1_money *= 2.5f);
  BOOST_REQUIRE_EQUAL(3_money, 1_money *= 2.9f);
  BOOST_REQUIRE_EQUAL(3_money, 1_money *= 3.1f);
  BOOST_REQUIRE_EQUAL(4_money, 1_money *= 3.5f);
  BOOST_REQUIRE_EQUAL(4_money, 1_money *= 3.9f);

  BOOST_REQUIRE_EQUAL(0_money, -0.1f * 1_money);
  BOOST_REQUIRE_EQUAL(0_money, -0.5f * 1_money);
  BOOST_REQUIRE_EQUAL(-1_money, -0.9f * 1_money);
  BOOST_REQUIRE_EQUAL(-2_money, -2.1f * 1_money);
  BOOST_REQUIRE_EQUAL(-2_money, -2.5f * 1_money);
  BOOST_REQUIRE_EQUAL(-3_money, -2.9f * 1_money);
  BOOST_REQUIRE_EQUAL(-3_money, -3.1f * 1_money);
  BOOST_REQUIRE_EQUAL(-4_money, -3.5f * 1_money);
  BOOST_REQUIRE_EQUAL(-4_money, -3.9f * 1_money);

  BOOST_REQUIRE_EQUAL(0_money, 1_money *= -0.1f);
  BOOST_REQUIRE_EQUAL(0_money, 1_money *= -0.5f);
  BOOST_REQUIRE_EQUAL(-1_money, 1_money *= -0.9f);
  BOOST_REQUIRE_EQUAL(-2_money, 1_money *= -2.1f);
  BOOST_REQUIRE_EQUAL(-2_money, 1_money *= -2.5f);
  BOOST_REQUIRE_EQUAL(-3_money, 1_money *= -2.9f);
  BOOST_REQUIRE_EQUAL(-3_money, 1_money *= -3.1f);
  BOOST_REQUIRE_EQUAL(-4_money, 1_money *= -3.5f);
  BOOST_REQUIRE_EQUAL(-4_money, 1_money *= -3.9f);

  BOOST_REQUIRE_EQUAL(0_money, 0.1f * -1_money);
  BOOST_REQUIRE_EQUAL(0_money, 0.5f * -1_money);
  BOOST_REQUIRE_EQUAL(-1_money, 0.9f * -1_money);
  BOOST_REQUIRE_EQUAL(-2_money, 2.1f * -1_money);
  BOOST_REQUIRE_EQUAL(-2_money, 2.5f * -1_money);
  BOOST_REQUIRE_EQUAL(-3_money, 2.9f * -1_money);
  BOOST_REQUIRE_EQUAL(-3_money, 3.1f * -1_money);
  BOOST_REQUIRE_EQUAL(-4_money, 3.5f * -1_money);
  BOOST_REQUIRE_EQUAL(-4_money, 3.9f * -1_money);

  BOOST_REQUIRE_EQUAL(0_money, -1_money *= 0.1f);
  BOOST_REQUIRE_EQUAL(0_money, -1_money *= 0.5f);
  BOOST_REQUIRE_EQUAL(-1_money, -1_money *= 0.9f);
  BOOST_REQUIRE_EQUAL(-2_money, -1_money *= 2.1f);
  BOOST_REQUIRE_EQUAL(-2_money, -1_money *= 2.5f);
  BOOST_REQUIRE_EQUAL(-3_money, -1_money *= 2.9f);
  BOOST_REQUIRE_EQUAL(-3_money, -1_money *= 3.1f);
  BOOST_REQUIRE_EQUAL(-4_money, -1_money *= 3.5f);
  BOOST_REQUIRE_EQUAL(-4_money, -1_money *= 3.9f);

  BOOST_REQUIRE_EQUAL(0_money, -0.1f * -1_money);
  BOOST_REQUIRE_EQUAL(0_money, -0.5f * -1_money);
  BOOST_REQUIRE_EQUAL(1_money, -0.9f * -1_money);
  BOOST_REQUIRE_EQUAL(2_money, -2.1f * -1_money);
  BOOST_REQUIRE_EQUAL(2_money, -2.5f * -1_money);
  BOOST_REQUIRE_EQUAL(3_money, -2.9f * -1_money);
  BOOST_REQUIRE_EQUAL(3_money, -3.1f * -1_money);
  BOOST_REQUIRE_EQUAL(4_money, -3.5f * -1_money);
  BOOST_REQUIRE_EQUAL(4_money, -3.9f * -1_money);

  BOOST_REQUIRE_EQUAL(0_money, -1_money *= -0.1f);
  BOOST_REQUIRE_EQUAL(0_money, -1_money *= -0.5f);
  BOOST_REQUIRE_EQUAL(1_money, -1_money *= -0.9f);
  BOOST_REQUIRE_EQUAL(2_money, -1_money *= -2.1f);
  BOOST_REQUIRE_EQUAL(2_money, -1_money *= -2.5f);
  BOOST_REQUIRE_EQUAL(3_money, -1_money *= -2.9f);
  BOOST_REQUIRE_EQUAL(3_money, -1_money *= -3.1f);
  BOOST_REQUIRE_EQUAL(4_money, -1_money *= -3.5f);
  BOOST_REQUIRE_EQUAL(4_money, -1_money *= -3.9f);

  BOOST_REQUIRE_EQUAL(1_money, 1_money / 1.9f);
  BOOST_REQUIRE_EQUAL(0_money, 1_money / 2.f);
  BOOST_REQUIRE_EQUAL(0_money, 1_money / 2.1f);
  BOOST_REQUIRE_EQUAL(2_money, 3_money / 1.9f);
  BOOST_REQUIRE_EQUAL(2_money, 3_money / 2.f);
  BOOST_REQUIRE_EQUAL(1_money, 3_money / 2.1f);
  BOOST_REQUIRE_EQUAL(3_money, 5_money / 1.9f);
  BOOST_REQUIRE_EQUAL(2_money, 5_money / 2.f);
  BOOST_REQUIRE_EQUAL(2_money, 5_money / 2.1f);

  BOOST_REQUIRE_EQUAL(1_money, 1_money /= 1.9f);
  BOOST_REQUIRE_EQUAL(0_money, 1_money /= 2.f);
  BOOST_REQUIRE_EQUAL(0_money, 1_money /= 2.1f);
  BOOST_REQUIRE_EQUAL(2_money, 3_money /= 1.9f);
  BOOST_REQUIRE_EQUAL(2_money, 3_money /= 2.f);
  BOOST_REQUIRE_EQUAL(1_money, 3_money /= 2.1f);
  BOOST_REQUIRE_EQUAL(3_money, 5_money /= 1.9f);
  BOOST_REQUIRE_EQUAL(2_money, 5_money /= 2.f);
  BOOST_REQUIRE_EQUAL(2_money, 5_money /= 2.1f);

  BOOST_REQUIRE_EQUAL(-1_money, -1_money / 1.9f);
  BOOST_REQUIRE_EQUAL(0_money, -1_money / 2.f);
  BOOST_REQUIRE_EQUAL(0_money, -1_money / 2.1f);
  BOOST_REQUIRE_EQUAL(-2_money, -3_money / 1.9f);
  BOOST_REQUIRE_EQUAL(-2_money, -3_money / 2.f);
  BOOST_REQUIRE_EQUAL(-1_money, -3_money / 2.1f);
  BOOST_REQUIRE_EQUAL(-3_money, -5_money / 1.9f);
  BOOST_REQUIRE_EQUAL(-2_money, -5_money / 2.f);
  BOOST_REQUIRE_EQUAL(-2_money, -5_money / 2.1f);

  BOOST_REQUIRE_EQUAL(-1_money, -1_money /= 1.9f);
  BOOST_REQUIRE_EQUAL(0_money, -1_money /= 2.f);
  BOOST_REQUIRE_EQUAL(0_money, -1_money /= 2.1f);
  BOOST_REQUIRE_EQUAL(-2_money, -3_money /= 1.9f);
  BOOST_REQUIRE_EQUAL(-2_money, -3_money /= 2.f);
  BOOST_REQUIRE_EQUAL(-1_money, -3_money /= 2.1f);
  BOOST_REQUIRE_EQUAL(-3_money, -5_money /= 1.9f);
  BOOST_REQUIRE_EQUAL(-2_money, -5_money /= 2.f);
  BOOST_REQUIRE_EQUAL(-2_money, -5_money /= 2.1f);

  BOOST_REQUIRE_EQUAL(-1_money, 1_money / -1.9f);
  BOOST_REQUIRE_EQUAL(0_money, 1_money / -2.f);
  BOOST_REQUIRE_EQUAL(0_money, 1_money / -2.1f);
  BOOST_REQUIRE_EQUAL(-2_money, 3_money / -1.9f);
  BOOST_REQUIRE_EQUAL(-2_money, 3_money / -2.f);
  BOOST_REQUIRE_EQUAL(-1_money, 3_money / -2.1f);
  BOOST_REQUIRE_EQUAL(-3_money, 5_money / -1.9f);
  BOOST_REQUIRE_EQUAL(-2_money, 5_money / -2.f);
  BOOST_REQUIRE_EQUAL(-2_money, 5_money / -2.1f);

  BOOST_REQUIRE_EQUAL(-1_money, 1_money /= -1.9f);
  BOOST_REQUIRE_EQUAL(0_money, 1_money /= -2.f);
  BOOST_REQUIRE_EQUAL(0_money, 1_money /= -2.1f);
  BOOST_REQUIRE_EQUAL(-2_money, 3_money /= -1.9f);
  BOOST_REQUIRE_EQUAL(-2_money, 3_money /= -2.f);
  BOOST_REQUIRE_EQUAL(-1_money, 3_money /= -2.1f);
  BOOST_REQUIRE_EQUAL(-3_money, 5_money /= -1.9f);
  BOOST_REQUIRE_EQUAL(-2_money, 5_money /= -2.f);
  BOOST_REQUIRE_EQUAL(-2_money, 5_money /= -2.1f);

  BOOST_REQUIRE_EQUAL(1_money, -1_money / -1.9f);
  BOOST_REQUIRE_EQUAL(0_money, -1_money / -2.f);
  BOOST_REQUIRE_EQUAL(0_money, -1_money / -2.1f);
  BOOST_REQUIRE_EQUAL(2_money, -3_money / -1.9f);
  BOOST_REQUIRE_EQUAL(2_money, -3_money / -2.f);
  BOOST_REQUIRE_EQUAL(1_money, -3_money / -2.1f);
  BOOST_REQUIRE_EQUAL(3_money, -5_money / -1.9f);
  BOOST_REQUIRE_EQUAL(2_money, -5_money / -2.f);
  BOOST_REQUIRE_EQUAL(2_money, -5_money / -2.1f);

  BOOST_REQUIRE_EQUAL(1_money, -1_money /= -1.9f);
  BOOST_REQUIRE_EQUAL(0_money, -1_money /= -2.f);
  BOOST_REQUIRE_EQUAL(0_money, -1_money /= -2.1f);
  BOOST_REQUIRE_EQUAL(2_money, -3_money /= -1.9f);
  BOOST_REQUIRE_EQUAL(2_money, -3_money /= -2.f);
  BOOST_REQUIRE_EQUAL(1_money, -3_money /= -2.1f);
  BOOST_REQUIRE_EQUAL(3_money, -5_money /= -1.9f);
  BOOST_REQUIRE_EQUAL(2_money, -5_money /= -2.f);
  BOOST_REQUIRE_EQUAL(2_money, -5_money /= -2.1f);

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
    pattern do_pos_format() const override { return {{sign, space, value, symbol}}; };
    pattern do_neg_format() const override { return {{sign, space, value, symbol}}; };
  };
} // namespace

BOOST_AUTO_TEST_CASE(ClassicFormat)
{
  std::stringstream stream;

  stream << 0_money << ' ' << -0_money << ' ' << 1_money << ' ' << -1_money << ' ' << 1.5_money << ' ' << -1.5_money
         << ' ' << -0.1_money << ' ' << 0.1_money << ' ' << -123456_money << ' ' << 123456_money << ' '
         << -123.456_money << ' ' << 123.456_money;

  BOOST_REQUIRE_EQUAL("0 0 1 -1 15 -15 -1 1 -123456 123456 -123456 123456", stream.str());

  return;
}

BOOST_AUTO_TEST_CASE(Format)
{
  std::stringstream stream;
  stream.imbue(std::locale(stream.getloc(), std::make_unique<moneypunct_facet>().release()));

  BOOST_REQUIRE_NO_THROW(stream << put_money(1.5_money) << ' ' << put_money(-1.5_money) << ' '
                                << put_money(-123456_money) << ' ' << put_money(123456_money) << ' '
                                << put_money(-123.456_money) << ' ' << put_money(123.456_money));
  BOOST_REQUIRE_EQUAL("+ 1_5+ - 1_5- - 1-23-45_6- + 1-23-45_6+ - 1-23-45_6- + 1-23-45_6+", stream.str());

  return;
}

BOOST_AUTO_TEST_CASE(FormatLeadingZero)
{
  // gcc is not displaying the leading zero in 0.1 (i prints .1) so both outputs are valid.
  auto is_either = [](std::string_view ref1, std::string_view ref2,
                      std::string_view result) -> boost::test_tools::predicate_result {
    if (result == ref1 || result == ref2) return true;

    boost::test_tools::predicate_result res(false);
    res.message() << "Result (" << result << ") does not match \"" << ref1 << "\" nor \"" << ref2 << "\".";

    return res;
  };

  std::stringstream stream;
  stream.imbue(std::locale(stream.getloc(), std::make_unique<moneypunct_facet>().release()));

  BOOST_REQUIRE_NO_THROW(stream << put_money(0_money) << ' ' << put_money(-0_money) << ' ' << put_money(1_money) << ' '
                                << put_money(-1_money) << ' ' << put_money(-0.1_money) << ' ' << put_money(0.1_money));
  BOOST_TEST(
      is_either("+ _0+ + _0+ + _1+ - _1- - _1- + _1+", "+ 0_0+ + 0_0+ + 0_1+ - 0_1- - 0_1- + 0_1+", stream.str()));

  return;
}

BOOST_AUTO_TEST_CASE(ParseClassic)
{
  std::stringstream stream;
  stream << "0 1 -1 15 -15 20745 -20745 90000000000000000000 $9";

  io1::Money m;
  BOOST_REQUIRE_NO_THROW(stream >> m);
  BOOST_REQUIRE_EQUAL(0_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> m);
  BOOST_REQUIRE_EQUAL(1_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> m);
  BOOST_REQUIRE_EQUAL(-1_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> m);
  BOOST_REQUIRE_EQUAL(1.5_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> m);
  BOOST_REQUIRE_EQUAL(-1.5_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> m);
  BOOST_REQUIRE_EQUAL(2074.5_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> m);
  BOOST_REQUIRE_EQUAL(-2074.5_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> m);
  BOOST_REQUIRE_EQUAL(false, static_cast<bool>(stream));
  stream.clear();
  BOOST_REQUIRE_NO_THROW(stream >> m);
  BOOST_REQUIRE_EQUAL(false, static_cast<bool>(stream));
  stream.clear();

  return;
}

BOOST_AUTO_TEST_CASE(Parse)
{
  std::stringstream stream;
  stream << "+ 0_0+ + 0_1+ - 0_1- + _0+ + _1+ - _1- + 1_5+ - 1_5- + 20-74_5+ - 20-74_5- + "
            "9-00-00-00-00-00-00-00-00-00_0+ -9_0-";

  stream.imbue(std::locale(stream.getloc(), new moneypunct_facet));

  io1::Money m;
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(0_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(1_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(-1_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(0_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(1_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(-1_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(1.5_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(-1.5_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(2074.5_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(-2074.5_money, m);
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(false, static_cast<bool>(stream));
  stream.clear();
  BOOST_REQUIRE_NO_THROW(stream >> get_money(m));
  BOOST_REQUIRE_EQUAL(false, static_cast<bool>(stream));
  stream.clear();

  return;
}

BOOST_AUTO_TEST_CASE(Comparisons)
{
  BOOST_TEST(1_money == 1_money);
  BOOST_TEST(!(2_money == 1_money));
  BOOST_TEST(1_money == -(-1_money));
  BOOST_TEST(-(-1_money) == 1_money);
  BOOST_TEST(-1_money == -1_money);

  BOOST_TEST(-1_money != 1_money);
  BOOST_TEST(1_money != -1_money);
  BOOST_TEST(!(1_money != 1_money));
  BOOST_TEST(-1_money != 1_money);

  BOOST_TEST(-1_money <= 1_money);
  BOOST_TEST(-1_money <= -1_money);
  BOOST_TEST(-1_money < 1_money);
  BOOST_TEST(!(1_money <= -1_money));
  BOOST_TEST(!(1_money < 1_money));

  BOOST_TEST(1_money >= -1_money);
  BOOST_TEST(1_money >= 1_money);
  BOOST_TEST(1_money > -1_money);
  BOOST_TEST(!(-1_money >= 1_money));
  BOOST_TEST(!(1_money > 1_money));

  BOOST_TEST(1_money <= 2_money);
  BOOST_TEST(1_money < 2_money);
  BOOST_TEST(!(2_money < 2_money));
  BOOST_TEST(!(2_money <= 1_money));

  BOOST_TEST(2_money >= 1_money);
  BOOST_TEST(2_money > 1_money);
  BOOST_TEST(!(2_money > 2_money));
  BOOST_TEST(!(1_money >= 2_money));

  return;
}
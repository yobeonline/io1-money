#include "io1/money.hpp"

#include <doctest/doctest.h>
#include <memory>
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
  pattern do_pos_format() const override { return {symbol, sign, value}; };
  pattern do_neg_format() const override { return {symbol, sign, value}; };
};

[[nodiscard]] auto compute_installment_plan(io1::Money price, std::size_t count) noexcept
{
  assert(0 != count && "Not doing a plan for no payment.");

  auto const div_result = div(price, count);
  assert(div_result.rem >= 0_money && "Count was unsigned.");

  auto const m = static_cast<std::size_t>(div_result.rem.data());
  std::vector<io1::Money> plan(count, div_result.quot);
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

  cout << "Total: "
       << std::format(std::locale(cout.getloc(), std::make_unique<american_moneypunct_facet>().release()), "{:#m}\n",
                      final_price);

  auto const payment_plan = compute_installment_plan(final_price, installments_nb);
  cout << installments_nb << " payments:";

  cout.imbue(std::locale(cout.getloc(), std::make_unique<american_moneypunct_facet>().release()));
  for (auto const & amount : payment_plan) cout << ' ' << std::showbase << put_money(amount);

  CHECK_EQ(std::accumulate(payment_plan.begin(), payment_plan.end(), 0_money), final_price);
  CHECK_EQ(cout.str(), "Total: $26.92\n5 payments: $5.39 $5.39 $5.38 $5.38 $5.38");

  return;
}
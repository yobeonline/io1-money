#include "io1/money.hpp"

io1::money add_twice(io1::money const & lhs, io1::money const & rhs) noexcept
{
  return lhs + 2 * rhs;
}

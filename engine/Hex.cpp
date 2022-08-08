#include "Hex.hpp"

#include <assert.h>   /* assert */

Hex::Hex(const int& w_q, const int& w_r, const int& w_s) :
m_q(w_q),
m_r(w_r),
m_s(w_s)
{
  assert(m_q + m_r + m_s == 0);
}

Hex::~Hex()
{

}

Hex& Hex::operator = (const Hex& other)
{
  if (this != &other)
  {
    m_q = other.m_q;
    m_r = other.m_r;
    m_s = other.m_s;
  }
  return *this;
}

bool Hex::operator == (const Hex& other) const
{
  return m_q == other.m_q && m_r == other.m_r && m_s == other.m_s;
}

bool Hex::operator != (const Hex& other) const
{
  return !(*this == other);
}

std::ostream& operator << (std::ostream& out, const Hex& h)
{
  out << "Q: " << h.m_q << " R: " << h.m_r << " S: " << h.m_s;
  return out;
}


Hex hex_direction(int direction /* 0 to 5 */)
{
  assert (0 <= direction && direction < 6);
  return hex_directions[direction];
}

Hex hex_neighbor(Hex hex, int direction)
{
  return hex_add(hex, hex_direction(direction));
}
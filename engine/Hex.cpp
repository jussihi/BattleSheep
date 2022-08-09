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

int hex_get_line_direction(const Hex& w_from, const Hex& w_to)
{
  /* If none of the directions match, the hexes are not on the same "line" */
  if(w_from.m_q != w_to.m_q &&
     w_from.m_r != w_to.m_r &&
     w_from.m_s != w_to.m_s)
  {
    return -1;
  }

  /* Get the difference between the hexes, then scale to right length (distance 1) */
  Hex sub = hex_subtract(w_to, w_from);
  unsigned int len = hex_length(sub);
  Hex sub_scaled = Hex(sub.m_q / len, sub.m_r / len, sub.m_s / len);

  /* Find the correct direction from the direction vector, return index */
  for(int i = 0; i < hex_directions.size(); i++)
  {
    if(sub_scaled == hex_directions[i])
      return i;
  }

  return -1;
}
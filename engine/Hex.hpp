#pragma once


#include <vector>       /* std::vector */
#include <iostream>     /* ostream */

#include <cassert>      /* assert */
#include <cmath>        /* std::abs */


class Hex
{
public:
  Hex(const int& w_q, const int& w_r, const int& w_s);
  ~Hex();

  Hex& operator = (const Hex& other);
  bool operator == (const Hex& other) const;
  bool operator != (const Hex& other) const;
  friend std::ostream& operator << (std::ostream& out, const Hex& h);

  /* Storage for cube coordinates */
  int m_q, m_r, m_s;

private:
  
};


static inline Hex hex_add(const Hex& a, const Hex& b)
{
  return Hex(a.m_q + b.m_q, a.m_r + b.m_r, a.m_s + b.m_s);
}

static inline Hex hex_subtract(const Hex& a, const Hex& b)
{
  return Hex(a.m_q - b.m_q, a.m_r - b.m_r, a.m_s - b.m_s);
}

static inline Hex hex_multiply(const Hex& hex, int k)
{
  return Hex(hex.m_q * k, hex.m_r * k, hex.m_s * k);
}

static inline int hex_length(const Hex& hex)
{
  return int((std::abs(hex.m_q) + std::abs(hex.m_r) + std::abs(hex.m_s)) / 2);
}

static inline int hex_distance(const Hex& a, const Hex& b)
{
  return hex_length(hex_subtract(a, b));
}

/*
 * The directions "clockwise". Starts always from the right-top Hex,
 * both in flat and pointy hex systems.
 */
const std::vector<Hex> hex_directions =
{
  Hex(1, -1, 0), Hex(1, 0, -1), Hex(0, 1, -1),
  Hex(-1, 1, 0), Hex(-1, 0, 1), Hex(0, -1, 1)
};

Hex hex_direction(int direction /* 0 to 5 */);

Hex hex_neighbor(Hex hex, int direction);
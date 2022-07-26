#pragma once


#include "Hex.hpp"

#include <unordered_map>    /* std::unordered_map */

/* 
 * Custom specialization of std::hash for the Hex object, 
 * needed for std::unordered_map
 */
template<>
struct std::hash<Hex>
{
  std::size_t operator()(Hex const& hex) const noexcept
  {
    return hex.m_q ^ (hex.m_r + 0x9e3779b9 + (hex.m_q << 6) + (hex.m_q >> 2));
  }
};

const std::vector<std::vector<Hex>> piece_rotations =
{
  {Hex(0, 0, 0), Hex(1, -1, 0), Hex(0, -1, 1), Hex(1, 0, -1)},
  {Hex(0, 0, 0), Hex(1, 0, -1), Hex(0, 1, -1), Hex(-1, 1, 0)},
  {Hex(0, 0, 0), Hex(0, 1, -1), Hex(-1, 1, 0), Hex(-1, 0, 1)}
};

class Map
{
public:
  Map(const int w_players);
  ~Map();
  bool InsertPiece(const Hex& h, unsigned int rotation);
  void InsertRandomPiece();
  void RandomGenerateMap();
  int GetPiecesLeft() { return m_pieces_left; };
private:
  std::unordered_map<Hex, bool> m_playground;
  // std::unordered_map<Hex,  int> m_allocated_playground;
  int m_pieces_left;
};

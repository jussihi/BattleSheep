#pragma once


#include "Hex.hpp"
#include "Point.hpp"

#include <unordered_map>    /* std::unordered_map */
#include <utility>          /* std::pair */
#include <cstdint>          /* uintN_t */

/*
 * Bitmasks for playground manipulation
 */
#define HEX_FREE      0x00
#define HEX_WHITE     0x01
#define HEX_BLUE      0x02
#define HEX_RED       0x04
#define HEX_BLACK     0x08
#define HEX_OOB       0x80
#define HEX_NONFREE   0xFF

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
  Point2i GetSize() const { return m_size; };
  Point2i GetOrigin() const { return m_origin; };
  const std::unordered_map<Hex, std::pair<uint8_t, uint8_t>>& GetPlayground() const { return m_playground; };
private:
  std::unordered_map<Hex, std::pair<uint8_t, uint8_t>> m_playground;
  // std::unordered_map<Hex,  int> m_allocated_playground;
  int m_pieces_left;
  Point2i m_size;
  Point2i m_origin;
};

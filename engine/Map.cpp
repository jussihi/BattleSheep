#include "Map.hpp"


#include <random>     /* random_device and friends */
#include <iterator>   /* std::next */

#include <iostream>



Map::Map(const int w_players) :
m_pieces_left(w_players * 4)
{
  /*
   * First we create an empty playing field with enough hexes to accommodate
   * maximum width/height allocated play fields, boundaries calculated below:
   */
  int right = ( 9 + (w_players - 1) * 10);
  int left = (right - 1) * -1;
  int bottom = ( 10 + (w_players - 1) * 12);
  int top = bottom * -1;

  m_size   = Point2i((left * -1) + right + 1, bottom * 2 + 1);
  m_origin = Point2i(left * -1, bottom);
  
  /* Generate a pointy-top field */
  for (int r = top; r <= bottom; r++)
  { 
    int r_offset = r >> 1; // or r>>1
    for (int q = left - r_offset; q <= right - r_offset; q++)
    {
      /* At start no playing field is activated for use */
      m_playground.insert( { Hex(q, r, -q-r), { HEX_OOB, 0 } } );
    }
  }

  /* Place the first piece to the field */
  m_playground[Hex(0, 0, 0)]  = { HEX_FREE, 0 };
  m_playground[Hex(1, -1, 0)] = { HEX_FREE, 0 };
  m_playground[Hex(1, 0, -1)] = { HEX_FREE, 0 };
  m_playground[Hex(0, 1, -1)] = { HEX_FREE, 0 };
  m_pieces_left -= 1;
}


Map::~Map()
{

}

bool Map::InsertPiece(const Hex& h, unsigned int rotation)
{
  /*
   * The new piece must have at least one shared side with already-accommodated
   * Hex. On top of this, none of the Hexes in the piece shall overlap with
   * already-accommodated Hexes or be out of bounds.
   */

  bool has_neighbour = false;
  std::vector<Hex> candidateHexes;

  for(auto& elem: piece_rotations[rotation])
  {
    /* Add this Hex element of the piece to candidate array */
    Hex candidate = hex_add(h, elem);
    candidateHexes.push_back(candidate);
    /* First we check that the hex is not oob */
    auto search = m_playground.find(candidate);
    if(search == m_playground.end())
    {
      return false;
    }

    /* Then that it is not already occupied, i.e., it still needs to be oob */
    if(search->second.first != HEX_OOB)
    {
      return false;
    }

    /* Finally, check that it has at least one already-occupied neighbour */
    for(int curr_direction = 0; curr_direction < 6; curr_direction++)
    {
      auto search_neighbor = m_playground.find(hex_neighbor(search->first, curr_direction));
      if(search_neighbor == m_playground.end())
      {
        /* 
         * It is fine if we go out of bounds here, 
         * we might be in the very edge of the play field 
         */
        continue;
      }
      if(search_neighbor->second.first == HEX_FREE)
      {
        has_neighbour = true;
        break;
      }
    }
  }

  /* All checks passed, we will insert the piece */
  if(has_neighbour == true)
  {
    for(auto& elem: candidateHexes)
    {
      m_playground[elem] = { HEX_FREE, 0 };
      std::cout << "Inserted hex " << elem << std::endl;
    }
    std::cout << "\n\n" << std::endl;
    m_pieces_left -= 1;
  }

  return has_neighbour;
}

void Map::InsertRandomPiece()
{
  /* Sanity check */
  if(m_pieces_left <= 0)
    return;
  
  /* Try adding a piece until it succeeds */
  std::unordered_map<Hex, std::pair<uint8_t, uint8_t>>::iterator random_hex;
  int random_rotation;
  do
  {
    std::random_device rd;
    std::mt19937 rng(rd());

    /* Get random hex that is not yet allocated, i.e., find a currently-OOB Hex */
    std::uniform_int_distribution<int> rand_hex_uni(0, m_playground.size() - 1);
    do
    {
      auto random_hexno = rand_hex_uni(rng);
      random_hex = std::next(std::begin(m_playground), random_hexno);
    } while(random_hex->second.first != HEX_OOB);

    /* Generate random rotation */
    std::uniform_int_distribution<int> rand_rot_uni(0, 2);
    random_rotation = rand_rot_uni(rng);
  } while(InsertPiece(random_hex->first, random_rotation) == false);
}

void Map::RandomGenerateMap()
{
  while(m_pieces_left > 0)
    InsertRandomPiece();
}
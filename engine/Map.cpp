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

  /* When all of the pieces have been inserted, we can find the edges */
  if(m_pieces_left == 0)
    FindEdges();

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

void Map::ClearMap()
{
  for(auto& it : m_playground)
  {
    if(it.second.first != HEX_OOB)
    {
      /* Clear the tile */
      it.second.first &= (HEX_FREE | HEX_EDGE);
      it.second.second = 0;
    }
  }
}

void Map::FindEdges()
{
  /*
   * First we start moving on the grid from the origin to the upper left
   * direction, when we hit the edge of the playfield, we come back to the last
   * Hex which was marked as HEX_FREE.
   */
  Hex last_free_hex = Hex(0, 0, 0);
  Hex curr_hex = Hex(0, 0, 0);

  /* direction 5 is the "10 or 11 o-clock" direction */
  std::unordered_map<Hex, std::pair<uint8_t, uint8_t>>::iterator search;
  while((search = m_playground.find(hex_neighbor(curr_hex, 5))) !=
         m_playground.end())
  {
    curr_hex = search->first;
    if(search->second.first == HEX_FREE)
      last_free_hex = search->first;
  }

  /*
   * When we have found at least one edge Hex, we can start going through the rest
   * of the Hexes
   *
   * First we start from the direction 1, because it is the "rightmost" direction where
   * another free Hex can be located.
   *
   * After the first special case, we will continue with an algorithm that works as
   * follows: Find the next neighbour by rotating the direction from the last found
   * neighbour's direction substituted by one. Move on to the next Hex and repeat
   * the process, until we have come back and found the first Hex added at least twice.
   */
  curr_hex = last_free_hex;

  /* Large number, mod6 == 1 */
  int last_direction = 199;
  int last_free_hex_cnt = 0;
  /* Lazy way to iterate through the playground at worst 3 times */
  while(last_free_hex_cnt < 3)
  {
    if(curr_hex == last_free_hex)
      last_free_hex_cnt++;
    for(last_direction = last_direction - 1; last_direction < last_direction + 5; last_direction++)
    {
      auto next_hex_search = m_playground.find(hex_neighbor(curr_hex, last_direction % 6));
      if(next_hex_search == m_playground.end())
        continue;
      if(next_hex_search->second.first == HEX_FREE || next_hex_search->second.first == HEX_EDGE)
      {
        next_hex_search->second.first = HEX_EDGE;
        curr_hex = next_hex_search->first;
        break;
      }
    }
  }
}
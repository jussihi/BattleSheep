#include "Game.hpp"

#include <iterator>     // std::next
#include <algorithm>    // std::find


Game::Game(const int w_players) :
m_map(w_players),
m_pieces_inserted(1),
m_round(0),
m_game_state(STATE_MAPGEN)
{
  for(int i= 0; i < w_players; i++)
  {
    m_players.push_back(1 << i);
  }
  m_curr_turn = m_players.begin();
}

Game::~Game()
{

}

void Game::Restart(bool w_keep_map)
{
  if(w_keep_map)
    m_map.ClearMap();
  else
  {
    m_active_map.clear();
    m_map = std::move(Map(m_players.size()));
  }

  m_curr_turn = m_players.begin();
  m_round = 0;
}

bool Game::InsertMapPiece(const Hex& h, unsigned int rotation)
{
  /* Sanity check */
  if(m_pieces_inserted >= m_players.size() * 4 || m_game_state != STATE_MAPGEN)
    return false;

  bool ret = m_map.InsertPiece(h, rotation);
  if(ret)
  {
    m_pieces_inserted++;
    NextTurn(false);
  }

  /* This was the last piece to be inserted, do needed internal changes */
  if(m_pieces_inserted == m_players.size() * 4)
  {
    m_active_map = std::move(m_map.GetActiveMap());
    m_game_state = STATE_GAMEPLAY;
  }

  return ret;
}

void Game::RandomGenerateMap()
{
  m_map.RandomGenerateMap(m_players.size() * 4 - m_pieces_inserted);
  m_active_map = std::move(m_map.GetActiveMap());
  m_game_state = STATE_GAMEPLAY;
  m_curr_turn = m_players.begin();
}

void Game::NextTurn(bool w_advance_round)
{
  if(std::next(m_curr_turn) != m_players.end())
    m_curr_turn = std::next(m_curr_turn);
  else
  {
    m_curr_turn = m_players.begin();
    if(w_advance_round) m_round++;
  }
}

std::vector<std::pair<std::pair<Hex, Hex>, int>> Game::GetLegalMoves()
{
  /* TODO */
  return std::vector<std::pair<std::pair<Hex, Hex>, int>>();
}

bool Game::CheckMoveLegality(const Hex& w_hex_from, const Hex& w_hex_to, const int w_pieces)
{
  /* Sanity check that both hexes are part of the active playfield */
  if(std::find(m_active_map.begin(), m_active_map.end(), w_hex_from) == m_active_map.end() ||
     std::find(m_active_map.begin(), m_active_map.end(), w_hex_to)   == m_active_map.end())
  {
    return false;
  }

  /*
   * If the Hex we are ought to move from does not match the current player in turn or has
   * less pieces than "w_pieces" +1 on it, we can bail out
   */
  const std::pair<uint8_t, uint8_t>& from_hex_state = m_map.GetHexState(w_hex_from);
  if(from_hex_state.first & *m_curr_turn == 0 || from_hex_state.second < w_pieces + 1)
  {
    return false;
  }

  /*
   * Get the direction of the move, if the return value is negative, the hexes are not on same
   * straight line in the hex grid.
   */
  int line_direction = hex_get_line_direction(w_hex_from, w_hex_to);
  if(line_direction == -1)
    return false;

  /*
   * Traverse through the line from "hex_from" to "hex_to". If all hexes on the way are free
   * and the next hex we would check is not free, the move is legal
   */
  unsigned int line_len = hex_distance(w_hex_from, w_hex_to);
  Hex curr_hex = w_hex_from;
  for(int i = 0; i < line_len; i++)
  {
    curr_hex = std::move(hex_neighbor(curr_hex, line_direction));
    const std::pair<uint8_t, uint8_t>& curr_hex_state = m_map.GetHexState(curr_hex);
    if((curr_hex_state.first & HEX_NONFREE) != 0 || curr_hex_state.second > 0)
      return false;
  }
  /* Get the hex state "behind" the "hex_to" Hex */
  curr_hex = std::move(hex_neighbor(curr_hex, line_direction));
  const std::pair<uint8_t, uint8_t>& curr_hex_state = m_map.GetHexState(curr_hex);
  if((curr_hex_state.first & HEX_NONFREE) == 0)
      return false;

  return true;
}
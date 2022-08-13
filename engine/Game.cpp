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

void Game::StartGameplay()
{
  m_active_map = std::move(m_map.GetActiveMap());
  m_game_state = STATE_GAMEPLAY;
  m_curr_turn = m_players.begin();
  m_map.FindEdges();
  FindLegalMoves();
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
    StartGameplay();
  }

  return ret;
}

void Game::RandomGenerateMap()
{
  m_map.RandomGenerateMap(m_players.size() * 4 - m_pieces_inserted);
  StartGameplay();
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
  FindLegalMoves();
}

bool Game::MakeMove(const Hex& w_hex_from, const Hex& w_hex_to, const int w_pieces)
{
  /* Sanity check that we are in correct game state */
  if(m_game_state != STATE_GAMEPLAY)
    return false;

  /* Lazy way to find a pair from the vector */
  auto it = std::find_if(m_legal_moves.moves.begin(), m_legal_moves.moves.end(),
                         [& w_hex_from, & w_hex_to] (const std::pair<std::pair<Hex, Hex>, int>& S)
  {
    return S.first.first == w_hex_from && S.first.second == w_hex_to;
  });

  /* If the move was not found */
  if(it == std::end(m_legal_moves.moves))
    return false;

  uint8_t hex_pieces = m_map.GetHexState(w_hex_from).second;
  /* Check that the starting Hex has enough pieces for the move */
  if(hex_pieces <= w_pieces && m_round != 0 || (m_round == 0 && w_pieces != 16))
    return false;

  /* Finally, make the move and hand the turn over */
  m_map.SetHexState(w_hex_from, std::make_pair(*m_curr_turn, hex_pieces - w_pieces));
  m_map.SetHexState(w_hex_to,   std::make_pair(*m_curr_turn, w_pieces));

  NextTurn();

  return true;
}

void Game::FindLegalMoves()
{
  /* Sanity check for the right game state */
  if(m_game_state != STATE_GAMEPLAY)
    return;

  /* If the legal moves were already checked, bail out */
  if(m_legal_moves.curr_turn == *m_curr_turn && m_legal_moves.round == m_round)
    return;

  /* Reset the legal moves data for this turn and round */
  m_legal_moves.curr_turn = *m_curr_turn;
  m_legal_moves.round = m_round;
  m_legal_moves.moves.clear();


  /* If we are on round 0, we should allow moving from edge Hex to itself with 16 pieces */
  if(m_round == 0)
  {
    for(auto& elem : m_active_map)
    {
      if(m_map.GetHexState(elem).first == HEX_EDGE)
        m_legal_moves.moves.push_back(std::make_pair(std::make_pair(elem, elem), 16));
    }
  }
  else /* Basic round when we move pieces from stacks to other stacks */
  {
    for(auto& elem : m_active_map)
    {
      if(m_map.GetHexState(elem).first == *m_curr_turn && m_map.GetHexState(elem).second > 1)
      {
        /* Iterate through all the directions, find */
        for(int i = 0; i < hex_directions.size(); i++)
        {
          Hex neigh = hex_neighbor(elem, i);
          /*
           * If we cannot traverse to this location (first neighbour is blocked),
           * bail out and proceed to next direction
           */
          if((m_map.GetHexState(neigh).first & HEX_NONFREE) != 0)
            continue;
          /* Otherwise continue until we hit an edge */
          do
          {
            neigh = hex_neighbor(neigh, i);
          } while((m_map.GetHexState(hex_neighbor(neigh, i)).first & HEX_NONFREE) == 0);
          m_legal_moves.moves.push_back(std::make_pair(std::make_pair(elem, neigh),
                                                       m_map.GetHexState(elem).second - 1));
        }
      }
    }
  }
}

const std::vector<std::pair<std::pair<Hex, Hex>, int>>& Game::GetLegalMoves()
{
  return m_legal_moves.moves;
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
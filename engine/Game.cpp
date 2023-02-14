#include "Game.hpp"

#include <iterator>     // std::next
#include <algorithm>    // std::find


Game::Game(const int w_players, const bool w_auto_next_move) :
m_map(w_players),
m_pieces_inserted(1),
m_round(0),
m_game_state(STATE_MAPGEN),
m_auto_next_move(w_auto_next_move),
m_empty_moves(0)
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

std::pair<uint8_t, uint8_t> Game::GetWinner() const
{
  /* Sanity check */
  if(m_game_state != STATE_GAME_OVER)
    return std::make_pair(0, 0);

  std::vector<std::pair<uint8_t, uint8_t>> scores;
  for(auto player : m_players)
  {
    scores.emplace_back(std::make_pair(player, static_cast<uint8_t>(0)));
  }
  for(auto hex : m_active_map)
  {
    for(auto& score : scores)
    {
      if(score.first == (m_map.GetHexState(hex).first & HEX_PLAYER))
        score.second++;
    }
  }

  /* Sort scores */
  std::sort(scores.begin(), scores.end(), []
  (const std::pair<uint8_t, uint8_t>& left, const std::pair<uint8_t, uint8_t>& right)
  {
    return left.second > right.second;
  });
  /* Get the difference between the first and second */
  uint8_t score_diff = scores[0].second - scores[1].second;
  /* If there is no difference, the game ends in a draw */
  if(score_diff == 0)
    return std::make_pair(0, 0);
  return std::make_pair(scores[0].first, score_diff);
}

void Game::NextTurn(bool w_advance_round)
{
  /* Sanity check for game end */
  if(m_game_state == STATE_GAME_OVER)
    return;

  if(std::next(m_curr_turn) != m_players.end())
    m_curr_turn = std::next(m_curr_turn);
  else
  {
    m_curr_turn = m_players.begin();
    if(w_advance_round) m_round++;
  }
  FindLegalMoves();

  if(m_legal_moves.moves.empty())
  {
    m_empty_moves++;
    if(m_empty_moves >= m_players.size())
    {
      m_game_state = STATE_GAME_OVER;
      return;
    }
    /* For GUI */
    if(m_auto_next_move)
      NextTurn();
  }
  else
  {
    m_empty_moves = 0;
  }
}

bool Game::MakeMove(const Hex& w_hex_from, const Hex& w_hex_to,
                    const int w_pieces, const bool w_auto_next_turn)
{
  /* Sanity check that we are in correct game state */
  if(m_game_state != STATE_GAMEPLAY)
    return false;

  /* Sanity check that we are trying to move at least one piece */
  if(w_pieces < 1)
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

  if(w_auto_next_turn)
    NextTurn();

  return true;
}

bool Game::MakeMove(const Hex& w_hex_from, const int w_direction,
                    const int w_pieces, const bool w_auto_next_turn)
{
  /* Traverse until the next hex is not free anymore */
  Hex hex_to = hex_neighbor(w_hex_from, w_direction);
  while(m_map.GetHexState(hex_neighbor(hex_to, w_direction)).first == HEX_FREE)
  {
    hex_to = hex_neighbor(hex_to, w_direction);
  }

  /* Make move using the original implementation, fails if this diraction is blocked */
  return MakeMove(w_hex_from, hex_to, w_pieces, w_auto_next_turn);
}

bool Game::IsHexBlocked(const Hex& w_hex) const
{
  /* If this hex only has 1 piece on it, it doesn't matter if it's blocked */
  if(m_map.GetHexState(w_hex).second <= 1)
    return false;
  /* Iterate through all the directions */
  for(int i = 0; i < hex_directions.size(); i++)
  {
    Hex neigh = hex_neighbor(w_hex, i);
    /*
     * If we cannot traverse to this direction (neighbour is blocked),
     * bail out and proceed to next direction
     */
    if((m_map.GetHexState(neigh).first & HEX_NONFREE) != 0)
      continue;
    /* Otherwise the Hex is not blocked */
    return false;
  }
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
          while((m_map.GetHexState(hex_neighbor(neigh, i)).first & HEX_NONFREE) == 0)
          {
            neigh = hex_neighbor(neigh, i);
          }
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

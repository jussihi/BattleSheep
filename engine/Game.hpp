#pragma once

#include "Map.hpp"

#include <vector>


typedef enum eGameState_t
{
  STATE_MAPGEN    = 0x00000000,
  STATE_GAMEPLAY  = 0x00000001,
  STATE_GAME_OVER = 0x00000002
} GameState_t;

class Game
{
public:
  Game(const int w_players);
  ~Game();

  void Restart(bool w_keep_map);
  bool InsertMapPiece(const Hex& h, unsigned int rotation);
  void RandomGenerateMap();
  GameState_t GetCurrGameState() { return m_game_state; };
  int GetCurrRound() { return m_round; };
  uint8_t GetCurrTurn() { return *m_curr_turn; };
  std::vector<std::pair<std::pair<Hex, Hex>, int>> GetLegalMoves();
  bool CheckMoveLegality(const Hex& w_hex_from, const Hex& w_hex_to, const int w_pieces);
  const Map& GetMap() const { return m_map; };

private:
  void StartGameplay();
  void NextTurn(bool w_advance_round = true);

  Map m_map;
  std::vector<Hex> m_active_map;
  int m_pieces_inserted;
  std::vector<uint8_t> m_players;
  std::vector<uint8_t>::iterator m_curr_turn;
  int m_round;
  GameState_t m_game_state;
};
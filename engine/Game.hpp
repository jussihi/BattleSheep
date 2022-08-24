#pragma once

#include "Map.hpp"

#include <vector>
#include <map>
#include <string>


typedef enum eGameState_t
{
  STATE_MAPGEN    = 0x00000000,
  STATE_GAMEPLAY  = 0x00000001,
  STATE_GAME_OVER = 0x00000002
} GameState_t;

const std::map<uint8_t, std::string> player_str
{ {HEX_WHITE, "White"}, {HEX_BLUE, "Blue"}, {HEX_RED, "Red"}, {HEX_BLACK, "Black"} };

const std::map<GameState_t, std::string> game_state_str
{ {STATE_MAPGEN, "MAPGEN"}, {STATE_GAMEPLAY, "GAMEPLAY"}, {STATE_GAME_OVER, "GAME_OVER"} };

struct LegalMoves
{
  uint8_t curr_turn;
  int round;
  std::vector<std::pair<std::pair<Hex, Hex>, int>> moves;
};

class Game
{
public:
  Game(const int w_players, const bool w_auto_next_move = true);
  ~Game();

  void Restart(bool w_keep_map);
  bool InsertMapPiece(const Hex& h, unsigned int rotation);
  void RandomGenerateMap();
  GameState_t GetCurrGameState() { return m_game_state; };
  int GetCurrRound() { return m_round; };
  uint8_t GetCurrTurn() { return *m_curr_turn; };
  std::pair<uint8_t, uint8_t> GetWinner() const;
  bool MakeMove(const Hex& w_hex_from, const Hex& w_hex_to,
                const int w_pieces, const bool w_auto_next_turn = true);
  bool IsHexBlocked(const Hex& w_hex) const;
  void FindLegalMoves();
  const std::vector<std::pair<std::pair<Hex, Hex>, int>>& GetLegalMoves();
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
  LegalMoves m_legal_moves;
  bool m_auto_next_move;
  int m_empty_moves;
};
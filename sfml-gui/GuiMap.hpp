#pragma once

#include "Orientation.hpp"

#include "GuiHex.hpp"
#include "Map.hpp"

class GuiMap
{
public:
  GuiMap(Orientation orientation_, const Map& engineMap_, int hexSize_, const sf::Font& w_font);
  ~GuiMap() {};
  void DrawMap(sf::RenderTarget& _target, sf::RenderStates _states);
  Hex PixelToHex(Point2f p);
  std::vector<GuiHex>::iterator PixelToGuiHex(Point2f p);
  GuiHex& HexToGuiHex(const Hex& w_hex);
  void MouseHover(Point2f p);
  void SetChosenHex(const Hex& w_chosen_hex);
  Point2f GetOriginPixel();

private:
  const Orientation m_gridOrientation;
  const Point2i m_hexSize;
  const Point2i m_gridOrigin;
  const std::unordered_map<Hex, std::pair<uint8_t, uint8_t>>& m_playground;
  std::vector<GuiHex> m_guiHexes;
  const sf::Font& m_font;
};
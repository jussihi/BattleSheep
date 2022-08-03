#pragma once

#include "Orientation.hpp"

#include "GuiHex.hpp"
#include "Map.hpp"

class GuiMap
{
public:
  GuiMap(Orientation orientation_, const Map& engineMap_, int hexSize_);
  ~GuiMap() {};
  void DrawMap(sf::RenderTarget& _target, sf::RenderStates _states);
  std::vector<GuiHex>::iterator PixelToGuiHex(Point2f p);
  void MouseHover(Point2f p);
  Point2f GetOriginPixel();

private:
  const Orientation m_gridOrientation;
  const Point2i m_hexSize;
  const Point2i m_gridOrigin;
  const std::unordered_map<Hex, std::pair<uint8_t, uint8_t>>& m_playground;
  std::vector<GuiHex> m_guiHexes;
};
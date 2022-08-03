#include "GuiMap.hpp"

#include <iostream>


GuiMap::GuiMap(Orientation orientation_, const Map& engineMap_, int hexSize_) :
m_gridOrientation(orientation_),
m_hexSize(Point2i(50, 50)),
m_gridOrigin(engineMap_.GetOrigin()),
m_playground(engineMap_.GetPlayground())
{
  std::cout << "initialized map with size " << m_hexSize.x << ", " << m_hexSize.y << " and origin " << m_gridOrigin.x << ", " << m_gridOrigin.y << std::endl;

  for(auto i : m_playground)
  {
    double x = (m_gridOrientation.f0 * i.first.m_q + m_gridOrientation.f1 * i.first.m_r) * m_hexSize.x;
    double y = (m_gridOrientation.f2 * i.first.m_q + m_gridOrientation.f3 * i.first.m_r) * m_hexSize.y;

    // add origin offset
    x += (m_gridOrigin.x + 1) * m_hexSize.x * std::sqrt(3);
    y += (m_gridOrigin.y + 1) * m_hexSize.y * 1.5f;

    GuiHex newHex(m_hexSize.x - 2, true, sf::Color::Green, i.first);
    newHex.setPosition(x, y);
    m_guiHexes.push_back(newHex);
  }
}


std::unordered_map<int, sf::Color> GridColors {
  { HEX_FREE,  sf::Color::Green },
  { HEX_WHITE, sf::Color::White },
  { HEX_BLUE,  sf::Color::Blue  },
  { HEX_RED,   sf::Color::Red   },
  { HEX_BLACK, sf::Color::Black },
  { HEX_OOB,   sf::Color(0, 0, 0, 200)  }
};

void GuiMap::DrawMap(sf::RenderTarget& _target, sf::RenderStates _states)
{
  for(auto& i : m_guiHexes)
  {
    auto search = m_playground.find(i.GetHex());
    if(search == m_playground.end())
    {
      continue;
    }

    // Update tile color
    i.SetColor(GridColors.at(search->second.first));

    /* set hilighted hex color if we should */
    if(i.GetHighlight())
    {
      i.SetColor(sf::Color::Cyan);
      i.SetHighlight(false);
    }

    i.draw(_target, _states);
  }
}

std::vector<GuiHex>::iterator GuiMap::PixelToGuiHex(Point2f p)
{
    Point2f pt = Point2f((p.x - (m_gridOrigin.x + 1) * m_hexSize.x  * std::sqrt(3)) / m_hexSize.x,
                         (p.y - (m_gridOrigin.y + 1) * m_hexSize.y * 1.5f) / m_hexSize.y);
    double qF = m_gridOrientation.b0 * pt.x + m_gridOrientation.b1 * pt.y;
    double rF = m_gridOrientation.b2 * pt.x + m_gridOrientation.b3 * pt.y;

    int q = int(round(qF));
    int r = int(round(rF));
    int s = int(round(-qF - rF));

    double q_diff = abs(q - qF);
    double r_diff = abs(r - rF);
    double s_diff = abs(s - (-qF - rF));
    if (q_diff > r_diff and q_diff > s_diff)
      q = -r - s;
    else if (r_diff > s_diff)
      r = -q - s;
    else
      s = -q - r;
    
    return std::find_if(m_guiHexes.begin(), m_guiHexes.end(), [&](const GuiHex& o) {
      return o.GetHex() == Hex(q, r, s);
    });
}

void GuiMap::MouseHover(Point2f p)
{
  std::vector<GuiHex>::iterator searchHex = PixelToGuiHex(p);

  if(searchHex == m_guiHexes.end())
    return;

  searchHex->SetHighlight(true);
}

Point2f GuiMap::GetOriginPixel()
{
  return Point2f((m_gridOrigin.x + 1) * m_hexSize.x  * std::sqrt(3),
                 (m_gridOrigin.y + 1) * m_hexSize.y * 1.5f);
}
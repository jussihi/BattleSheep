#include "GuiHex.hpp"

#include <cmath>


GuiHex::GuiHex(int w_radius, bool w_visible, sf::Color w_color, const Hex& w_hex) :
m_visible(w_visible),
m_active(false),
m_highlight(false),
m_radius(w_radius),
m_hex(w_hex)
{
  int nPoints = 8;
  m_shader = NULL;

  m_vertex = sf::VertexArray(sf::TrianglesFan, nPoints);
  m_vertex[0].position = sf::Vector2f(0, 0);
  m_vertex[0].color = w_color;

  for(int i = 1; i < m_vertex.getVertexCount(); i++)
  {
    double angle = (i * (2 * M_PI) / 6) - (M_PI / 6);
    m_vertex[i].position = sf::Vector2f(m_radius * std::cos(angle), m_radius * std::sin(angle));
    m_vertex[i].color = w_color;
  }
}

void GuiHex::SetVisible(bool w_visible)
{
  m_visible = w_visible;
}

bool GuiHex::GetVisible(void)
{
  return m_visible;
}

int GuiHex::GetRadius(void)
{
  return m_radius;
}

std::vector<sf::Vector2f> GuiHex::GetBoundary(void)
{
  std::vector<sf::Vector2f> lstPoints;

  for(int i = 1; i < 7; i++)
    lstPoints.push_back(getTransform().transformPoint(m_vertex[i].position));

  return lstPoints;
}

bool GuiHex::SetTexture(std::string w_strTexture)
{
  if (!m_texture.loadFromFile(w_strTexture))
    return false;

  m_texture.setSmooth(true);
  for(int i = 0; i < m_vertex.getVertexCount(); i++)
  {
    m_vertex[i].texCoords = sf::Vector2f(
      (m_vertex[i].position.x + m_radius) / (m_radius * 2) * m_texture.getSize().x,
      (m_vertex[i].position.y + m_radius) / (m_radius * 2) * m_texture.getSize().y
    );
  }

  return true;
}

void GuiHex::SetTexture(sf::Texture w_texture)
{
  m_texture = w_texture;
  m_texture.setSmooth(true);

  for(int i = 0; i < m_vertex.getVertexCount(); i++)
  {
    m_vertex[i].texCoords = sf::Vector2f(
      (m_vertex[i].position.x + m_radius) / (m_radius * 2) * m_texture.getSize().x,
      (m_vertex[i].position.y + m_radius) / (m_radius * 2) * m_texture.getSize().y
    );
  }
}

void GuiHex::SetColor(sf::Color w_color)
{
  for(int i = 0; i < m_vertex.getVertexCount(); i++)
    m_vertex[i].color = w_color;
}

void GuiHex::SetShader(sf::Shader* w_shader)
{
  m_shader = w_shader;
}

void GuiHex::draw(sf::RenderTarget& w_target, sf::RenderStates w_states) const
{
  if(m_visible)
  {
    w_states.transform *= getTransform();
    w_states.texture = &m_texture;
    if(sf::Shader::isAvailable())
      w_states.shader = m_shader;

    w_target.draw(m_vertex, w_states);
  }
}


void GuiHex::SetActive(bool w_active)
{
  m_active = w_active;
}


bool GuiHex::GetActive(void)
{
  return m_active;
}


void GuiHex::SetHighlight(bool w_highlight)
{
  m_highlight = w_highlight;
}


bool GuiHex::GetHighlight(void)
{
  return m_highlight;
}

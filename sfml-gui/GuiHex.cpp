#include "GuiHex.hpp"

#include <cmath>


GuiHex::GuiHex(int w_radius, bool w_visible, sf::Color w_color,
               const Hex& w_hex, const sf::Font& w_font) :
m_visible(w_visible),
m_active(false),
m_highlight(false),
m_radius(w_radius),
m_hex(w_hex),
m_circle(w_radius * 0.6f, 50),
m_font(w_font)
{
  /* Create hex */
  int nPoints = 8;
  m_shader = NULL;

  m_hex_vertex = sf::VertexArray(sf::TrianglesFan, nPoints);
  m_hex_vertex[0].position = sf::Vector2f(0, 0);
  m_hex_vertex[0].color = w_color;

  for(int i = 1; i < m_hex_vertex.getVertexCount(); i++)
  {
    double angle = (i * (2 * M_PI) / 6) - (M_PI / 6);
    m_hex_vertex[i].position = sf::Vector2f(m_radius * std::cos(angle), m_radius * std::sin(angle));
    m_hex_vertex[i].color = w_color;
  }

  /* Create circle inside the hex */
  m_circle.setFillColor(sf::Color::White);
  m_circle.setOutlineColor(sf::Color::Black);
  m_circle.setOutlineThickness(2.f);
  m_circle.move(sf::Vector2f(static_cast<float>(-m_radius) + w_radius * 0.4f,
                             static_cast<float>(-m_radius) + w_radius * 0.4f));

  /* Create text to be placed inside the circle */
  m_text.setFont(m_font);
  m_text.setFillColor(sf::Color::White);
  m_text.setOutlineColor(sf::Color::Black);
  m_text.setOutlineThickness(2.f);
  m_text.setCharacterSize(22);
  m_text.setString("16");
  sf::FloatRect numRect = m_text.getGlobalBounds();
  sf::Vector2f numRectCenter(numRect.width / 2.0 + numRect.left, numRect.height / 2.0 + numRect.top);
  m_text.setOrigin(numRectCenter);
  m_text.setPosition(m_circle.getPosition().x + m_circle.getRadius(),
                     m_circle.getPosition().y + m_circle.getRadius());
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
    lstPoints.push_back(getTransform().transformPoint(m_hex_vertex[i].position));

  return lstPoints;
}

bool GuiHex::SetTexture(std::string w_strTexture)
{
  if (!m_hex_texture.loadFromFile(w_strTexture))
    return false;

  m_hex_texture.setSmooth(true);
  for(int i = 0; i < m_hex_vertex.getVertexCount(); i++)
  {
    m_hex_vertex[i].texCoords = sf::Vector2f(
      (m_hex_vertex[i].position.x + m_radius) / (m_radius * 2) * m_hex_texture.getSize().x,
      (m_hex_vertex[i].position.y + m_radius) / (m_radius * 2) * m_hex_texture.getSize().y
    );
  }

  return true;
}

void GuiHex::SetTexture(sf::Texture w_texture)
{
  m_hex_texture = w_texture;
  m_hex_texture.setSmooth(true);

  for(int i = 0; i < m_hex_vertex.getVertexCount(); i++)
  {
    m_hex_vertex[i].texCoords = sf::Vector2f(
      (m_hex_vertex[i].position.x + m_radius) / (m_radius * 2) * m_hex_texture.getSize().x,
      (m_hex_vertex[i].position.y + m_radius) / (m_radius * 2) * m_hex_texture.getSize().y
    );
  }
}

void GuiHex::SetColor(sf::Color w_color)
{
  for(int i = 0; i < m_hex_vertex.getVertexCount(); i++)
    m_hex_vertex[i].color = w_color;
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
    w_states.texture = &m_hex_texture;
    if(sf::Shader::isAvailable())
      w_states.shader = m_shader;

    w_target.draw(m_hex_vertex, w_states);
    w_target.draw(m_circle, w_states);
    w_target.draw(m_text, w_states);
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

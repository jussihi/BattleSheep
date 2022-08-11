#pragma once

#include <vector>
#include <string>

#include "SFML/Graphics.hpp"

#include "Hex.hpp"


class GuiHex: public sf::Drawable, public sf::Transformable
{
public:
  GuiHex(int nRadius, bool bVisible, sf::Color _color, const Hex& hex, const sf::Font& w_font);
  ~GuiHex() {};
  void SetVisible(bool w_visible);
  bool GetVisible(void);
  int GetRadius(void);
  std::vector<sf::Vector2f> GetBoundary(void);
  bool SetTexture(std::string w_strTexture);
  void SetTexture(sf::Texture w_texture);
  void SetColor(sf::Color w_color);
  void SetShader(sf::Shader* w_shader);
  void SetActive(bool w_ctive);
  bool GetActive(void);
  void SetHighlight(bool w_highlight);
  bool GetHighlight(void);
  virtual void draw(sf::RenderTarget& w_target, sf::RenderStates w_states) const;

  Hex GetHex() const { return m_hex; };

private:
  sf::VertexArray m_hex_vertex;
  sf::Texture m_hex_texture;
  sf::Shader *m_shader;
  sf::CircleShape m_circle;
  sf::Text m_text;
  const sf::Font& m_font;
  bool m_visible;
  bool m_active;
  bool m_highlight;
  int m_radius;
  const Hex m_hex;
};
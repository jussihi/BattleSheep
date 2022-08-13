#pragma once

#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>

// Define keys:
#define DELETE_KEY 8
#define ENTER_KEY 13
#define ESCAPE_KEY 27

class Textbox : public sf::Drawable, public sf::Transformable
{
public:
  Textbox(int w_size, sf::Color w_color, bool w_sel);

  void SetSelected(bool w_sel);
  void TypedOn(sf::Event w_input);

  void SetFont(sf::Font& w_font) { m_textbox.setFont(w_font); m_hint_text.setFont(w_font); };
  void SetLimit(bool w_limit) { m_has_limit = w_limit; };
  void SetLimit(bool w_limit, int w_lim) { m_has_limit = w_limit; m_limit = w_lim - 1; };
  virtual void draw(sf::RenderTarget& w_target, sf::RenderStates w_states) const;
  std::string GetText();
  void SetHintText(const std::string& w_hint);

private:
  void DeleteLastChar();
  void InputLogic(int w_char_typed);

  sf::RectangleShape m_background_rect;
  sf::Text m_hint_text;
  sf::Text m_textbox;

  std::ostringstream m_text;
  bool m_is_selected = false;
  bool m_has_limit = false;
  int m_limit = 0;
};
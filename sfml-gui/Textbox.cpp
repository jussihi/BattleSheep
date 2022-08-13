#include "Textbox.hpp"

#include <cctype>

Textbox::Textbox(int w_size, sf::Color w_color, bool w_sel)
{
  m_hint_text.setCharacterSize(w_size);
  m_hint_text.setFillColor(w_color);

  m_textbox.setCharacterSize(w_size + 6);
  m_textbox.setFillColor(w_color);

  m_background_rect.setSize(sf::Vector2f(300, 150));
  m_background_rect.setOutlineColor(sf::Color::Black);
  m_background_rect.setFillColor(sf::Color::White);
  m_background_rect.setOutlineThickness(5);
  m_background_rect.setOrigin(150, 75);


  m_is_selected = w_sel;

  // Check if the m_textbox is selected upon creation and display it accordingly:
  if(m_is_selected)
    m_textbox.setString("_");
  else
    m_textbox.setString("");

  sf::FloatRect numRect = m_textbox.getLocalBounds();
  sf::Vector2f numRectCenter(numRect.width / 2.0 + numRect.left, numRect.height / 2.0 + numRect.top);
  m_textbox.setOrigin(numRectCenter);
  m_textbox.setPosition(m_background_rect.getPosition().x,
                        m_background_rect.getPosition().y + m_background_rect.getSize().y * 0.25);
}

void Textbox::SetSelected(bool w_sel)
{
  m_is_selected = w_sel;

  // If not selected, remove the '_' at the end:
  if (!w_sel)
  {
    std::string t = m_text.str();
    std::string new_text = "";
    for (int i = 0; i < t.length(); i++)
    {
      new_text += t[i];
    }
    m_textbox.setString(new_text);
  }
}

void Textbox::TypedOn(sf::Event w_input)
{
  if (m_is_selected)
  {
    int char_typed = w_input.text.unicode;

    /* Only allow digits */
    if(!isdigit(char_typed) && char_typed != DELETE_KEY)
      return;
    
    if(char_typed == DELETE_KEY)
    {
      DeleteLastChar();
      goto end;
    }

    if (m_has_limit)
    {
      // If there's a limit, don't go over it:
      if (m_text.str().length() <= m_limit)
      {
        InputLogic(char_typed);
      }
    }
    // If no limit exists, just run the function:
    else
    {
      InputLogic(char_typed);
    }
  }

end:
  sf::FloatRect numRect = m_textbox.getLocalBounds();
  sf::Vector2f numRectCenter(numRect.width / 2.0 + numRect.left, numRect.height / 2.0 + numRect.top);
  m_textbox.setOrigin(numRectCenter);
  m_textbox.setPosition(m_background_rect.getPosition().x,
                        m_background_rect.getPosition().y + m_background_rect.getSize().y * 0.25);
}


void Textbox::draw(sf::RenderTarget& w_target, sf::RenderStates w_states) const
{
  if(m_is_selected)
  {
    w_states.transform *= getTransform();

    w_target.draw(m_background_rect, w_states);
    w_target.draw(m_hint_text, w_states);
    w_target.draw(m_textbox, w_states);
  }
}

void Textbox::DeleteLastChar()
{
  if(m_text.str().length() < 1)
    return;
  std::string t = m_text.str();
  std::string new_text = "";
  for (int i = 0; i < t.length() - 1; i++)
  {
    new_text += t[i];
  }
  m_text.str("");
  m_text << new_text;
  m_textbox.setString(m_text.str() + "_");
}


void Textbox::InputLogic(int w_char_typed)
{
  // If the key pressed isn't delete, or the two selection keys, then append the text with the char:
  if (w_char_typed != DELETE_KEY && w_char_typed != ENTER_KEY && w_char_typed != ESCAPE_KEY)
  {
    m_text << static_cast<char>(w_char_typed);
  }
  // If the key is delete, then delete the char:
  if (w_char_typed == DELETE_KEY)
  {
    DeleteLastChar();
  }
  // Set the m_textbox text:
  m_textbox.setString(m_text.str() + "_");
}

std::string Textbox::GetText()
{
  std::string ret = m_text.str();
  m_text.str("");
  return ret;
}

void Textbox::SetHintText(const std::string& w_hint)
{
  m_hint_text.setString(w_hint);
  sf::FloatRect numRect = m_hint_text.getLocalBounds();
  sf::Vector2f numRectCenter(numRect.width / 2.0 + numRect.left, numRect.height / 2.0 + numRect.top);
  m_hint_text.setOrigin(numRectCenter);
  m_hint_text.setPosition(m_background_rect.getPosition().x,
                          m_background_rect.getPosition().y - m_background_rect.getSize().y * 0.25);
}
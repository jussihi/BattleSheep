#include "Textbox.hpp"

Textbox::Textbox(int w_size, sf::Color w_color, bool w_sel)
{
  m_textbox.setCharacterSize(w_size);
  m_textbox.setFillColor(w_color);
  m_is_selected = w_sel;

  // Check if the m_textbox is selected upon creation and display it accordingly:
  if(m_is_selected)
    m_textbox.setString("_");
  else
    m_textbox.setString("");
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

    // Only allow normal inputs:
    if (char_typed < 128)
    {
      if (m_has_limit)
      {
        // If there's a limit, don't go over it:
        if (m_text.str().length() <= m_limit)
        {
          InputLogic(char_typed);
        }
        // But allow for char deletions:
        else if (m_text.str().length() > m_limit && char_typed == DELETE_KEY)
        {
          DeleteLastChar();
        }
      }
      // If no limit exists, just run the function:
      else
      {
        InputLogic(char_typed);
      }
    }
  }
}

void Textbox::DrawTo(sf::RenderWindow& w_window)
{
  w_window.draw(m_textbox);
}

void Textbox::DeleteLastChar()
{
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
  else if (w_char_typed == DELETE_KEY)
  {
    if (m_text.str().length() > 0)
    {
      DeleteLastChar();
    }
  }
  // Set the m_textbox text:
  m_textbox.setString(m_text.str() + "_");
}
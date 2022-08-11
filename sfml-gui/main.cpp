#include <iostream>
#include <sstream>

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"

#include "GuiHex.hpp"
#include "GuiMap.hpp"
#include "Game.hpp"

#include "Textbox.hpp"

typedef enum eSelectionStateGUI_t
{
  SELECTION_NONE  = 0x00000000,
  SELECTION_FIRST = 0x00000001
} SelectionStateGUI_t;

struct BattleSheepStateGUI
{
  BattleSheepStateGUI() : 
  game_state(STATE_MAPGEN),
  curr_round(0),
  curr_turn(1),
  selection_state(SELECTION_NONE),
  selected_hex(hex_invalid)
  {}
  GameState_t game_state;
  int curr_round;
  uint8_t curr_turn;
  SelectionStateGUI_t selection_state;
  Hex selected_hex;
};

int main(void)
{
  // Status variables
  sf::Vector2i mousePosScreen;
  sf::Vector2i mousePosWindow;
  sf::Vector2f mousePosView;
  sf::Vector2i mousePosGrid;
  sf::Text     debugText;
  sf::Text     statusText;
  sf::Text     hintText;
  sf::Font     defaultTextFont;
  defaultTextFont.loadFromFile("DroidSans.ttf");


  // Init game settings
  float gridSizeF = 100.f;
  unsigned int gridSizeU = static_cast<unsigned int>(gridSizeF);
  float viewSpeed = 200.f;
  float dt = 0.f;
  sf::Clock dtClock;
  int mapsize = 30;
  float currZoom = 1.8f;
  bool dragging = false;
  sf::Vector2f mouseOldPos;

  BattleSheepStateGUI stateGUI;

  // Initialize the main window
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8.0;
  sf::RenderWindow window(sf::VideoMode(1000, 800), "battlesheep", sf::Style::Default, settings);
  window.setFramerateLimit(60);

  // initialize the engine map and GUI map classes
  Game battleSheepGame(2);
  battleSheepGame.RandomGenerateMap();
  GuiMap guiMap(layout_pointy, battleSheepGame.GetMap(), 50, defaultTextFont);

  // Main camera view
  sf::View camera;
  // the camera size
  camera.setSize(window.getSize().x * currZoom, window.getSize().y * currZoom);
  // camera position (origin!)
  Point2f mapOrigin = guiMap.GetOriginPixel();
  camera.setCenter(mapOrigin.x, mapOrigin.y);

  /* Add status text to the screen */
  statusText.setCharacterSize(24);
  statusText.setFillColor(sf::Color::White);
  statusText.setFont(defaultTextFont);
  statusText.setPosition(20.f, 20.f);

  /* Add hint text to the screen */
  hintText.setCharacterSize(24);
  hintText.setFillColor(sf::Color::White);
  hintText.setFont(defaultTextFont);
  hintText.setPosition(20.f, 50.f);

  /* Add debug text to the screen */
  debugText.setCharacterSize(30);
  debugText.setFillColor(sf::Color::White);
  debugText.setFont(defaultTextFont);
  debugText.setPosition(20.f, static_cast<float>(window.getSize().y) - 120);

  /* The textbox instance */
  Textbox text1(20, sf::Color::White, true);
	text1.SetPosition({ static_cast<float>(window.getSize().x) - 100,
                      static_cast<float>(window.getSize().y) - 100 });
	text1.SetLimit(true, 2);
	text1.SetFont(defaultTextFont);


  while(window.isOpen())
  {
    // Update dt clock
    dt = dtClock.restart().asSeconds();

    sf::Event event;
    while(window.pollEvent(event))
    {
      if(event.type == sf::Event::Closed)
        window.close();
      // set the camera if window size is changed
      if (event.type == sf::Event::Resized)
      {
        camera.setSize(window.getSize().x * currZoom, window.getSize().y * currZoom);
      }
      // mouse wheel events for zooming
      if (event.type == sf::Event::MouseWheelScrolled)
      {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
        {
          currZoom += (event.mouseWheelScroll.delta * 0.1);
          std::cout << currZoom << std::endl;
          camera.setSize(window.getSize().x * currZoom, window.getSize().y * currZoom);
        }
      }
      if (event.type == sf::Event::TextEntered)
      {
        text1.TypedOn(event);
      }
      if (event.type == sf::Event::MouseButtonPressed)
      {
        if (event.mouseButton.button == sf::Mouse::Right)
        {
          mouseOldPos = sf::Vector2f(sf::Mouse::getPosition(window));
          dragging = true;
        }
        if (event.mouseButton.button == sf::Mouse::Left)
        {
          std::cout << "left" << std::endl;
          switch(stateGUI.game_state)
          {
            case(STATE_MAPGEN):
            {
              /* TODO, do mapgen state event handling here */
              break;
            }
            case(STATE_GAMEPLAY):
            {
              std::cout << "GAMEPLAY" << std::endl;
              /*
               * If it is the first round of the gameplay, the player only chooses one starting Hex
               * which should be on the edge of the playfield. This is a special case.
               */
              if(stateGUI.curr_round == 0)
              {
                std::cout << "round0" << std::endl;
                /* 
                 * The first round, when players choose to put their complete stack on one hex,
                 * only requires one click
                 */
                std::vector<GuiHex>::iterator selected_gui_hex = 
                                  guiMap.PixelToGuiHex(Point2f(mousePosView.x, mousePosView.y));
                Hex selected_hex = selected_gui_hex->GetHex();
                if(battleSheepGame.MakeMove(selected_hex, selected_hex, 16) == true)
                {
                  selected_gui_hex->UpdateStatus(16, stateGUI.curr_turn);
                }
                break;
              }
              /*
               * TODO: Rest of the moves should be handled here - requires 2 mouse presses - one
               * for the Source Hex and one for the destination Hex. The source Hex is kept inside
               * the BattleSheepStateGUI struct's "selected_hex" variable 
               */
              
              break;
            }
            case(STATE_GAME_OVER):
            {
              /* TODO: most prolly we can just ignore this */
              break;
            }
            default:
            {
              /* TODO: most prolly we can just ignore this */
              break;
            }
          }
        }
      }
    }

    // Get game state
    stateGUI.game_state = battleSheepGame.GetCurrGameState();
    stateGUI.curr_round = battleSheepGame.GetCurrRound();
    stateGUI.curr_turn = battleSheepGame.GetCurrTurn();

    // Update input
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
      camera.move(-viewSpeed * dt * currZoom, 0.f);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
      camera.move(viewSpeed * dt * currZoom, 0.f);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
      camera.move(0.f, -viewSpeed * dt * currZoom);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
      camera.move(0.f, viewSpeed * dt * currZoom);
    }

    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
    {
      if(!dragging)
      {
        mouseOldPos = sf::Vector2f(sf::Mouse::getPosition(window));
        dragging=true;
      }
      else
      {
        // Determine the new position in world coordinates
        const sf::Vector2f newPos = sf::Vector2f(sf::Mouse::getPosition(window));
        // Determine how the cursor has moved
        // Swap these to invert the movement direction
        sf::Vector2f deltaPos = mouseOldPos - newPos;

        // Applying zoom "reduction" (or "augmentation")
        deltaPos.x *= currZoom;
        deltaPos.y *= currZoom;

        // Move our view accordingly and update the window
        camera.move(deltaPos);
        window.setView(camera);

        // Save the new position as the old one
        // We're recalculating this, since we've changed the view
        mouseOldPos = newPos;
      }
    }
    else
    {
      dragging = false;
    }

    // Render Game-related stuff after setting our camera view
    window.clear();
    window.setView(camera);

    // Update mouse position on the game
    // This needs to be done in our main camera view
    mousePosScreen = sf::Mouse::getPosition();
    mousePosWindow = sf::Mouse::getPosition(window);
    mousePosView   = window.mapPixelToCoords(mousePosWindow);
    if(mousePosView.x >= 0.f)
      mousePosGrid.x = mousePosView.x / gridSizeU;
    if(mousePosView.y >= 0.f)
      mousePosGrid.y = mousePosView.y / gridSizeU;
    
    /* Tell GUIMap where the pointer is */
    guiMap.MouseHover(Point2f(mousePosView.x, mousePosView.y));

    // draw hexmap completely
    guiMap.DrawMap(window, sf::RenderStates::Default);

    // Reset view after rendering, render UI-related stuff
    window.setView(window.getDefaultView());

    // Render textbox at constant location
    text1.DrawTo(window);

    // Update debug text for mouse positioning
    std::stringstream ss_dbg;
    ss_dbg << "mousePosScreen: " << mousePosScreen.x << ", " << mousePosScreen.y << "\n"
           << "mousePosWindow: " << mousePosWindow.x << ", " << mousePosWindow.y << "\n"
           << "mousePosView:   " << mousePosView.x   << ", " << mousePosView.y   << "\n";
    debugText.setString(ss_dbg.str());
    window.draw(debugText);

    // Update status text for current game status tracking
    std::stringstream ss_stat;
    ss_stat << "Current turn: " << player_str.at(stateGUI.curr_turn)
            << "  Game state: " << game_state_str.at(stateGUI.game_state) 
            << "  Current round: " << stateGUI.curr_round << "\n";
    statusText.setString(ss_stat.str());
    window.draw(statusText);

    // Update hint text
    std::stringstream ss_hint;
    switch(stateGUI.game_state)
    {
      case(STATE_MAPGEN):
      {
        ss_hint << player_str.at(stateGUI.curr_turn) << " to add a piece to map!\n";
        break;
      }
      case(STATE_GAMEPLAY):
      {
        if(stateGUI.curr_round == 0)
        {
          ss_hint << player_str.at(stateGUI.curr_turn) << " to choose starting hex\n";
        }
        break;
      }
      case(STATE_GAME_OVER):
      {
        ss_hint << "Game over!\n";
        break;
      }
      default:
      {
        ss_hint << "Unknown game state!\n";
        break;
      }
    }
    
    hintText.setString(ss_hint.str());
    window.draw(hintText);

    window.display();
  }

  return 0;
}
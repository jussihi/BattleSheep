#include <iostream>
#include <sstream>

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"

#include "GuiHex.hpp"
#include "GuiMap.hpp"
#include "Map.hpp"

int main(void)
{
  // Status variables
  sf::Vector2i mousePosScreen;
  sf::Vector2i mousePosWindow;
  sf::Vector2f mousePosView;
  sf::Vector2i mousePosGrid;
  sf::Text     debugText;
  sf::Font debugTextFont;
  debugTextFont.loadFromFile("DroidSans.ttf");
  debugText.setCharacterSize(30);
  debugText.setFillColor(sf::Color::White);
  debugText.setFont(debugTextFont);
  debugText.setPosition(20.f, 20.f);


  // Init game settings
  float gridSizeF = 100.f;
  unsigned int gridSizeU = static_cast<unsigned int>(gridSizeF);
  float viewSpeed = 200.f;
  float dt = 0.f;
  sf::Clock dtClock;
  int mapsize = 30;
  float currZoom = 1.8f;


  // Initialize the main window
  sf::RenderWindow window(sf::VideoMode(1000, 800), "battlesheep");

  // initialize the engine map and GUI map classes
  Map map(2);
  map.RandomGenerateMap();
  GuiMap guiMap(layout_pointy, map, 50);

  // Main camera view
  sf::View camera;
  // the camera size
  camera.setSize(window.getSize().x * currZoom, window.getSize().y * currZoom);
  // camera position (origin!)
  Point2f mapOrigin = guiMap.GetOriginPixel();
  camera.setCenter(mapOrigin.x, mapOrigin.y);
  

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
    }

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

    // Update debug text for mouse positiong
    std::stringstream ss;
    ss << "mousePosScreen: " << mousePosScreen.x << ", " << mousePosScreen.y << "\n"
       << "mousePosWindow: " << mousePosWindow.x << ", " << mousePosWindow.y << "\n"
       << "mousePosView:   " << mousePosView.x   << ", " << mousePosView.y   << "\n";

    debugText.setString(ss.str());
    window.draw(debugText);

    window.display();
  }

  return 0;
}
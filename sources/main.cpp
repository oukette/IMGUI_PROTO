#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include "GameGUI.h"

int main()
{
    try
    {
        auto window = sf::RenderWindow(sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), 
                                                        "Game", 
                                                        sf::Style::Default);
        window.setFramerateLimit(60);
        if (!ImGui::SFML::Init(window))
        {
            throw std::runtime_error("Failed to initialize ImGui-SFML");
        }

        GameGUI gui(window);

        sf::Clock deltaClock;

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                gui.handleEvent(event);
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
            }
            // PROCESS the fullscreen toggle request
            gui.processFullscreenToggle();

            window.clear();
            gui.update();

            gui.render();
            window.display();
        }

        ImGui::SFML::Shutdown();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        ImGui::SFML::Shutdown();
        return 1;
    }

    return 0;
}

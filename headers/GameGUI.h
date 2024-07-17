#ifndef GAMEGUI_H
#define GAMEGUI_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <unordered_map>
#include <set>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "constants.h"

enum class MenuState
{
    MENU_MAIN,
    MENU_PLAY,
    MENU_OPTIONS,
    MENU_KEY_BINDINGS,
    MENU_CREDITS,
    MENU_QUIT,
};

enum class FrameRateOption
{
    FPS_UNCAPPED,
    FPS_30,
    FPS_60,
    FPS_90,
    FPS_120,
    FPS_144,
    FPS_240,
    FPS_CUSTOM
};

class GameGUI
{
private:
    sf::RenderWindow &m_window;
    MenuState m_currentState;
    sf::Vector2u m_windowSize;

    // Options
    bool m_isFullscreen = false;
    sf::Vector2u m_windowedSize = {DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT};
    sf::Vector2u m_fullscreenSize;
    bool m_fullscreen_toggle_pending = false;
    int m_resolutionIndex = 0;
    int m_framerateIndex = 0;
    bool m_vsync = false;
    int m_masterVolume = 77;
    int m_fxVolume = 77;
    int m_mouseSensitivity = 77;
    std::vector<sf::VideoMode> m_resolutions_list;
    const std::array<const char*, 8> m_frameRateOptions = {"Uncapped", "30", "60", "90", "120", "144", "240", "Custom"};
    int m_frameRateCap = 60;
    bool m_isFrameRateUncapped = false;
    FrameRateOption m_selectedFrameRateOption = FrameRateOption::FPS_60;
    int m_customFrameRate = 60;
    std::vector<s_keyBinding> m_keyBindings;
    std::string m_keyBindingErrorMessage;

private:
    void mainMenu();
    void playMenu();
    void optionsMenu();
    void creditsMenu();
    void quitMenu();
    void keyBindingsMenu();
    void applyResolution();
    void applyFrameRateCap();
    bool isInputValid(const s_inputBinding &input);
    std::string getInputName(const s_inputBinding &input);
    void ensureImGuiContext();

public:
    GameGUI(sf::RenderWindow& window);
    void setStyle();
    void handleEvent(sf::Event &event);
    void update();
    void render();
    void showMenuTitle(const char *title);
    void requestFullscreenToggle();
    void processFullscreenToggle();
    sf::Vector2u getWindowSize() const { return m_windowSize; }
};

#endif // GAMEGUI_H

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <unordered_map>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

constexpr int   DEFAULT_WINDOW_WIDTH = 1280;
constexpr int   DEFAULT_WINDOW_HEIGHT = 720;
constexpr float MENU_WINDOW_SCALE = 0.8f;
constexpr float MENU_FONT_SCALE = 3.5f;
constexpr float BUTTON_WIDTH = 200.0f;
constexpr float BUTTON_HEIGHT = 50.0f;
constexpr float ITEM_SPACING = 20.0f;
constexpr float WINDOW_PADDING = 20.0f;

const std::unordered_map<int, std::string> KEY_NAMES_MAP = {
    {sf::Keyboard::A, "A"}, 
    {sf::Keyboard::B, "B"}, 
    {sf::Keyboard::C, "C"}, 
    {sf::Keyboard::D, "D"}, 
    {sf::Keyboard::E, "E"}, 
    {sf::Keyboard::F, "F"}, 
    {sf::Keyboard::G, "G"}, 
    {sf::Keyboard::H, "H"}, 
    {sf::Keyboard::I, "I"}, 
    {sf::Keyboard::J, "J"}, 
    {sf::Keyboard::K, "K"}, 
    {sf::Keyboard::L, "L"}, 
    {sf::Keyboard::M, "M"}, 
    {sf::Keyboard::N, "N"}, 
    {sf::Keyboard::O, "O"}, 
    {sf::Keyboard::P, "P"}, 
    {sf::Keyboard::Q, "Q"}, 
    {sf::Keyboard::R, "R"}, 
    {sf::Keyboard::S, "S"}, 
    {sf::Keyboard::T, "T"}, 
    {sf::Keyboard::U, "U"}, 
    {sf::Keyboard::V, "V"}, 
    {sf::Keyboard::W, "W"}, 
    {sf::Keyboard::X, "X"}, 
    {sf::Keyboard::Y, "Y"}, 
    {sf::Keyboard::Z, "Z"},
    {sf::Keyboard::Left, "Left Arrow"},
    {sf::Keyboard::Right, "Right Arrow"},
    {sf::Keyboard::Up, "Up Arrow"},
    {sf::Keyboard::Down, "Down Arrow"},
    {sf::Keyboard::Space, "Space"},
    {sf::Keyboard::LControl, "Left Ctrl"},
    {sf::Keyboard::RControl, "Right Ctrl"},
    {sf::Keyboard::LShift, "Left Shift"},
    {sf::Keyboard::RShift, "Right Shift"},
    {sf::Keyboard::Tab, "Tab"},
    {sf::Keyboard::Return, "Enter"},
    {sf::Keyboard::BackSpace, "Backspace"},
    // other keys ...
};

enum class InputType
{
    Keyboard,
    Mouse
};

struct s_inputBinding
{
    InputType type;
    int code;  // This will store either keyboard key code or mouse button code
};

struct s_keyBinding
{
    std::string action;
    s_inputBinding input;
    bool isListening;
};

#endif // CONSTANTS_H
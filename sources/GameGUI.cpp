#include "GameGUI.h"

GameGUI::GameGUI(std::unique_ptr<sf::RenderWindow>& window) : 
    m_window(window.get()),
    m_currentState(MenuState::MENU_MAIN),
    m_windowSize(window->getSize())
{
    setStyle();

    // Retrieve supported resolutions and framerates
    m_resolutions_list = sf::VideoMode::getFullscreenModes();

    // Initialize key bindings
    m_keyBindings = {
        {"Move Left", {InputType::Keyboard, sf::Keyboard::Q}, false},
        {"Move Right", {InputType::Keyboard, sf::Keyboard::D}, false},
        {"Climb Up", {InputType::Keyboard, sf::Keyboard::Z}, false},
        {"Climb Down", {InputType::Keyboard, sf::Keyboard::S}, false},
        {"Primary Action", {InputType::Mouse, sf::Mouse::Left}, false},
        {"Secondary Action", {InputType::Mouse, sf::Mouse::Right}, false},
        {"Interact", {InputType::Keyboard, sf::Keyboard::E}, false},
        {"Jump", {InputType::Keyboard, sf::Keyboard::Space}, false},
        {"Sprint", {InputType::Keyboard, sf::Keyboard::LShift}, false}};
}


void GameGUI::applyResolution()
{
    if (m_resolutionIndex >= 0 && m_resolutionIndex < m_resolutions_list.size())
    {
        sf::VideoMode newMode = m_resolutions_list[m_resolutionIndex];
        if (m_isFullscreen)
        {
            m_window->create(newMode, "Game", sf::Style::Fullscreen);
        }
        else
        {
            m_window->create(newMode, "Game", sf::Style::Default);

            // Center the window on the screen
            sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
            m_window->setPosition(sf::Vector2i(
                (desktopMode.width - newMode.width) / 2,
                (desktopMode.height - newMode.height) / 2));
        }
        // Update ImGui SFML
        ImGui::SFML::SetCurrentWindow(*m_window);
    }
}

void GameGUI::applyFrameRateCap()
{
    switch (m_selectedFrameRateOption)
    {
        case FrameRateOption::FPS_UNCAPPED:
            m_window->setFramerateLimit(0);
            break;
        case FrameRateOption::FPS_CUSTOM:
            m_window->setFramerateLimit(m_customFrameRate);
            break;
        default:
            m_window->setFramerateLimit(std::stoi(m_frameRateOptions[static_cast<int>(m_selectedFrameRateOption)]));
            break;
    }
}

void GameGUI::setStyle()
{
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f; // Disable window borders
    style.WindowPadding = ImVec2(WINDOW_PADDING, WINDOW_PADDING);
    style.ItemSpacing = ImVec2(ITEM_SPACING, ITEM_SPACING);
    style.ScaleAllSizes(1.5f); // Scale all sizes by 1.5
}

void GameGUI::handleEvent(sf::Event &event)
{
    ImGui::SFML::ProcessEvent(event);

    if (event.type == sf::Event::Resized)
    {
        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        m_window->setView(sf::View(visibleArea));
        m_windowSize = sf::Vector2u(event.size.width, event.size.height);
        ImGui::SFML::UpdateFontTexture(); // Update ImGui font texture
    }

    for (auto &binding : m_keyBindings)
    {
        if (binding.isListening)
        {
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    binding.isListening = false;
                    m_keyBindingErrorMessage.clear();
                }
                else if (isInputValid({InputType::Keyboard, event.key.code}))
                {
                    binding.input = {InputType::Keyboard, event.key.code};
                    binding.isListening = false;
                    m_keyBindingErrorMessage.clear();
                }
                else
                {
                    m_keyBindingErrorMessage = "Input already assigned or invalid!";
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (isInputValid({InputType::Mouse, event.mouseButton.button}))
                {
                    binding.input = {InputType::Mouse, event.mouseButton.button};
                    binding.isListening = false;
                    m_keyBindingErrorMessage.clear();
                }
                else
                {
                    m_keyBindingErrorMessage = "Input already assigned or invalid!";
                }
            }
            break; // Only one binding can be listening at a time
        }
    }
}

void GameGUI::update()
{
    ensureImGuiContext();
    m_windowSize = m_window->getSize();
    ImGui::SFML::Update(*m_window, sf::seconds(1.f / 60.f));

    switch (m_currentState)
    {
        case MenuState::MENU_MAIN:
            mainMenu();
            break;
        case MenuState::MENU_PLAY:
            playMenu();
            break;
        case MenuState::MENU_OPTIONS:
            optionsMenu();
            break;
        case MenuState::MENU_KEY_BINDINGS:
            keyBindingsMenu();
            break;
        case MenuState::MENU_CREDITS:
            creditsMenu();
            break;
        case MenuState::MENU_QUIT:
            quitMenu();
            break;
    }
}

void GameGUI::render()
{
    ensureImGuiContext();
    ImGui::SFML::Render(*m_window);
}


void GameGUI::showMenuTitle(const char *title)
{
    ImGui::SetCursorPosY(10);
    ImGui::Text(title);
}


void GameGUI::requestFullscreenToggle()
{
    m_fullscreen_toggle_pending = true;
}


void GameGUI::processFullscreenToggle()
{
    if (!m_fullscreen_toggle_pending) return; // NO toggle requested

    m_fullscreen_toggle_pending = false; // RESET the flag
    m_isFullscreen = !m_isFullscreen; // TOGGLE the fullscreen flag
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode(); // GET desktop video mode

    std::cout << "Toggling fullscreen. New state: " << (m_isFullscreen ? "Fullscreen" : "Windowed") << std::endl;

    // SAVE the current view settings before changing the window
    sf::View currentView = m_window->getView();

    // SHUTDOWN ImGui before recreating the window
    ImGui::SFML::Shutdown(); 

    if (m_isFullscreen)
    {
        m_windowedSize = m_window->getSize();
        std::cout << "Stored windowed size: " << m_windowedSize.x << "x" << m_windowedSize.y << std::endl;
        m_window->create(desktopMode, "Game Title", sf::Style::Fullscreen);
    }
    else
    {
        m_window->create(sf::VideoMode(m_windowedSize.x, m_windowedSize.y), "Game Title", sf::Style::Default);
        sf::Vector2i windowPosition(
            (desktopMode.width - m_windowedSize.x) / 2,
            (desktopMode.height - m_windowedSize.y) / 2);
        m_window->setPosition(windowPosition);
    }

    if (!ImGui::SFML::Init(*m_window))
    {
        std::cerr << "Failed to initialize ImGui-SFML after toggling fullscreen" << std::endl;
    }

    // RESTORE the previous view settings
    m_window->setView(currentView);

    std::cout << "New window size: " << m_window->getSize().x << "x" << m_window->getSize().y << std::endl;

    setStyle();
}


void GameGUI::mainMenu()
{
    ImGui::SetNextWindowSize(ImVec2(m_windowSize.x * 0.8f, m_windowSize.y * 0.8f), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(m_windowSize.x * 0.1f, m_windowSize.y * 0.1f), ImGuiCond_Always);
    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    ImGui::SetCursorPosY(10);
    ImGui::SetWindowFontScale(3.5f); // Increase game title size
    ImGui::Text("Game Name");        // Display game name
    ImGui::SetWindowFontScale(1.0f); // Reset font scale

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - 50); // Center buttons
    if (ImGui::Button("PLAY", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT)))
    {
        m_currentState = MenuState::MENU_PLAY;
    }
    if (ImGui::Button("OPTIONS", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT)))
    {
        m_currentState = MenuState::MENU_OPTIONS;
    }
    if (ImGui::Button("CREDITS", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT)))
    {
        m_currentState = MenuState::MENU_CREDITS;
    }
    if (ImGui::Button("QUIT", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT)))
    {
        m_currentState = MenuState::MENU_QUIT;
    }
    ImGui::End();
}

void GameGUI::playMenu()
{
    ImGui::SetNextWindowSize(ImVec2(m_windowSize.x * 0.8f, m_windowSize.y * 0.8f), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(m_windowSize.x * 0.1f, m_windowSize.y * 0.1f), ImGuiCond_Always);
    ImGui::Begin("Play Menu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    showMenuTitle("Play");

    if (ImGui::Button("BACK", ImVec2(BUTTON_WIDTH/2, BUTTON_HEIGHT)))
    {
        m_currentState = MenuState::MENU_MAIN;
    }

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - 50); // Center buttons
    if (ImGui::Button("HOST", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT)))
    {
        // Host game logic here
    }
    if (ImGui::Button("JOIN", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT)))
    {
        // Join game logic here
    }
    if (ImGui::Button("SOLO", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT)))
    {
        // Solo game logic here
    }

    ImGui::End();
}

void GameGUI::optionsMenu()
{
    ensureImGuiContext();

    ImGui::SetNextWindowSize(ImVec2(m_windowSize.x * 0.8f, m_windowSize.y * 0.8f), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(m_windowSize.x * 0.1f, m_windowSize.y * 0.1f), ImGuiCond_Always);
    ImGui::Begin("Options Menu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar);

    showMenuTitle("Options");

    if (ImGui::Button("BACK", ImVec2(BUTTON_WIDTH/2, BUTTON_HEIGHT)))
    {
        m_currentState = MenuState::MENU_MAIN;
        // ImGui::End();
        // return;
    }

    // GRAPHICS - MODE
    bool currentFullscreen = m_isFullscreen;
    if (ImGui::Checkbox("Fullscreen", &currentFullscreen))
    {
        if (currentFullscreen != m_isFullscreen)
        {
            requestFullscreenToggle();
        }
    }
 
    // GRAPHICS - RESOLUTION
    ImGui::Text("Screen Resolution");
    std::vector<std::string> resolutionLabels;
    for (const auto &res : m_resolutions_list)
    {
        resolutionLabels.push_back(std::to_string(res.width) + "x" + std::to_string(res.height));
    }
    std::vector<const char *> resolutionLabelsCStr;
    for (const auto &label : resolutionLabels)
    {
        resolutionLabelsCStr.push_back(label.c_str());
    }
    int prevResolutionIndex = m_resolutionIndex;
    if (ImGui::Combo("##resolutions", &m_resolutionIndex, resolutionLabelsCStr.data(), resolutionLabelsCStr.size()))
    {
        applyResolution();
    }

    // GRAPHICS - FRAMERATE
    ImGui::Text("Frame Rate Cap");
    int currentOption = static_cast<int>(m_selectedFrameRateOption);
    if (ImGui::Combo("Frame Rate", &currentOption, m_frameRateOptions.data(), m_frameRateOptions.size()))
    {
        m_selectedFrameRateOption = static_cast<FrameRateOption>(currentOption);
        applyFrameRateCap();
    }

    if (m_selectedFrameRateOption == FrameRateOption::FPS_CUSTOM)
    {
        ImGui::SliderInt("Custom Frame Rate", &m_customFrameRate, 30, 400);
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            applyFrameRateCap();
        }
    }

    // GRAPHICS - VSYNC
    ImGui::Text("Vertical Sync");
    if (ImGui::Checkbox("##vsync", &m_vsync))
    {
        m_window->setVerticalSyncEnabled(m_vsync);
    }

    // AUDIO
    ImGui::Text("Audio");
    ImGui::SliderInt("Master Volume", &m_masterVolume, 0, 100);
    ImGui::SliderInt("FX Volume", &m_fxVolume, 0, 100);

    // CONTROLS
    ImGui::Text("Mouse");
    ImGui::SliderInt("Mouse Sensitivity", &m_mouseSensitivity, 0, 100);

    ImGui::Text("Controls");
    if (ImGui::Button("Key Bindings", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT)))
    {
        m_currentState = MenuState::MENU_KEY_BINDINGS;
    }

    ImGui::End();
}

void GameGUI::creditsMenu()
{
    ImGui::SetNextWindowSize(ImVec2(m_windowSize.x * 0.8f, m_windowSize.y * 0.8f), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(m_windowSize.x * 0.1f, m_windowSize.y * 0.1f), ImGuiCond_Always);
    ImGui::Begin("Credits Menu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    showMenuTitle("Credits");

    if (ImGui::Button("BACK", ImVec2(BUTTON_WIDTH/2, BUTTON_HEIGHT)))
    {
        m_currentState = MenuState::MENU_MAIN;
    }

    ImGui::Text("Contributors:");
    ImGui::Text("John Doe - Developer\nJane Smith - Artist\n..."); // Add more as needed

    ImGui::End();
}

void GameGUI::quitMenu()
{
    ImGui::SetNextWindowSize(ImVec2(m_windowSize.x * 0.8f, m_windowSize.y * 0.8f), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(m_windowSize.x * 0.1f, m_windowSize.y * 0.1f), ImGuiCond_Always);
    ImGui::Begin("Quit Menu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    showMenuTitle("Quit Menu");

    ImGui::Text("Quit the game?");
    ImGui::Spacing();
    if (ImGui::Button("NO", ImVec2(BUTTON_WIDTH/2, BUTTON_HEIGHT)))
    {
        m_currentState = MenuState::MENU_MAIN;
    }
    ImGui::SameLine();
    if (ImGui::Button("YES", ImVec2(BUTTON_WIDTH/2, BUTTON_HEIGHT)))
    {
        m_window->close();
    }

    ImGui::End();
}

void GameGUI::keyBindingsMenu()
{
    ImGui::SetNextWindowSize(ImVec2(m_windowSize.x * 0.8f, m_windowSize.y * 0.8f), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(m_windowSize.x * 0.1f, m_windowSize.y * 0.1f), ImGuiCond_Always);
    ImGui::Begin("Key Bindings Menu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    showMenuTitle("Key Bindings");

    if (ImGui::Button("BACK", ImVec2(BUTTON_WIDTH/2, BUTTON_HEIGHT)))
    {
        m_currentState = MenuState::MENU_OPTIONS;
        m_keyBindingErrorMessage.clear();
    }

    for (auto &binding : m_keyBindings)
    {
        ImGui::Text("%s", binding.action.c_str());
        ImGui::SameLine(200);

        std::string buttonLabel = binding.isListening
                                      ? "Press a key..."
                                      : getInputName(binding.input);

        if (ImGui::Button((buttonLabel + "##" + binding.action).c_str(), ImVec2(150, 0)))
        {
            // Reset all bindings to not listening
            for (auto &bind : m_keyBindings)
            {
                bind.isListening = false;
            }
            // Set this binding to listening
            binding.isListening = true;
            m_keyBindingErrorMessage.clear();
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Click to rebind");
        }
    }

    // Display error message if there is one
    if (!m_keyBindingErrorMessage.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red color
        ImGui::Text("%s", m_keyBindingErrorMessage.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::End();
}

bool GameGUI::isInputValid(const s_inputBinding &input)
{
    if (input.type == InputType::Keyboard)
    {
        std::vector<int> invalidKeys = {
            sf::Keyboard::F1, sf::Keyboard::F2, sf::Keyboard::F3,
            sf::Keyboard::F4, sf::Keyboard::F5, sf::Keyboard::F6, sf::Keyboard::F7,
            sf::Keyboard::F8, sf::Keyboard::F9, sf::Keyboard::F10, sf::Keyboard::F11,
            sf::Keyboard::F12};
        if (std::find(invalidKeys.begin(), invalidKeys.end(), input.code) != invalidKeys.end())
        {
            return false;
        }
    }

    // Check for conflicts with existing bindings
    for (const auto &binding : m_keyBindings)
    {
        if (binding.input.type == input.type && binding.input.code == input.code)
        {
            return false;
        }
    }

    return true;
}

std::string GameGUI::getInputName(const s_inputBinding &input)
{
    if (input.type == InputType::Keyboard)
    {
        auto it = KEY_NAMES_MAP.find(input.code);
        return (it != KEY_NAMES_MAP.end()) ? it->second : "Unknown";
    }
    else
    {
        switch (static_cast<sf::Mouse::Button>(input.code))
        {
        case sf::Mouse::Left:
            return "Left Click";
        case sf::Mouse::Right:
            return "Right Click";
        case sf::Mouse::Middle:
            return "Middle Click";
        case sf::Mouse::XButton1:
            return "Mouse 4";
        case sf::Mouse::XButton2:
            return "Mouse 5";
        default:
            return "Unknown Mouse Button";
        }
    }
}

void GameGUI::ensureImGuiContext()
{
    if (!ImGui::GetCurrentContext())
    {
        std::cerr << "ImGui context is null, reinitializing..." << std::endl;
        if (!ImGui::SFML::Init(*m_window))
        {
            std::cerr << "Failed to reinitialize ImGui-SFML" << std::endl;
            // Consider throwing an exception or handling this error appropriately
        }
        setStyle();
    }
}

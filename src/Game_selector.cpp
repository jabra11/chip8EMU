#include "Game_selector.hpp"

#include <filesystem>

Game_selector::Game_selector(sf::RenderWindow* w) noexcept
    :window{w}
{
    f.loadFromFile("../resources/fonts/arial.ttf");
    search_games("../roms/");
    title.setFont(f);
    title.setString("Select a game");
    title.setCharacterSize(40);
    title.setFillColor(sf::Color::Green);
    title.setPosition(window->getSize().x / 100 * 35, window->getSize().y / 100 * 5);
}

std::string Game_selector::get()
{
    search_games("../roms/");
    while (window->isOpen())
    {
        handle_events();

        if (handle_userinput())
            return "../roms/" + game;

        if (timer.is_ready(Timer::Type::framerate, 144))
            render();
    }
    return {};
}

void Game_selector::search_games(const std::string& dir)
{
    games = {};
    int x_offset = 1;
    int y_offset = 1;

    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        sf::Text tmp;
        tmp.setFont(f);
        tmp.setFillColor(sf::Color::Green);
        tmp.setString(entry.path().filename().string());
        tmp.setCharacterSize(20);
        tmp.setPosition(x_offset + (window->getSize().x / 100.0 * 15),
                y_offset + (window->getSize().y / 100.0 * 30));
    
        games.push_back(tmp);

        if (y_offset % 201 == 0)
        {
            y_offset = 1;
            x_offset += 10;
        }
        else
            y_offset += 40;
    }
}

bool Game_selector::handle_userinput()
{
    if (mouse_pressed)
    {
        mouse_pressed = false;

        auto [x, y] = sf::Mouse::getPosition(*window);
        for (const auto& i : games)
        {
            if (i.getGlobalBounds().contains(x, y))
            {
                game = i.getString();
                return true;
            }
        }
    }
    return false;
}

void Game_selector::handle_events()
{
    sf::Event evt;
    while (window->pollEvent(evt))
    {
        switch (evt.type)
        {
            case sf::Event::Closed: 
            {
                window->close(); 
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                mouse_pressed = true;
                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                mouse_pressed = false;
                break;
            }
            default: break;
        }
    }
}

void Game_selector::render()
{
    window->clear();
    
    window->draw(title);
    for (const auto& i : games)
        window->draw(i);
    window->display();
}
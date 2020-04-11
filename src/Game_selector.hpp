#ifndef GAME_SELECTOR
#define GAME_SELECTOR

#include <SFML/Graphics.hpp>

#include "utility/Timer.hpp"

class Game_selector
{
public:
    explicit Game_selector(sf::RenderWindow* window) noexcept;

    // return the path of the
    // desired ROM
    std::string get();

private:

    void search_games(const std::string& dir);

    // return true if a game has been
    // selected
    bool handle_userinput();

    void handle_events();
    void render();

    bool mouse_pressed = false;

    Timer timer; 

    sf::Font f;
    sf::Text title;
    
    std::string game;
    std::vector<sf::Text> games;
    sf::RenderWindow* window;
};

#endif // GAME_SELECTOR
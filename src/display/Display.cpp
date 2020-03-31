#include "Display.hpp"

Display::Display()
{
    sf::RectangleShape frame {sf::Vector2f(dimension.x, dimension.y)};
    frame.setOutlineThickness(0.5f);
    frame.setFillColor(sf::Color::Black);
    frame.setOutlineColor(sf::Color::Green);
    rects.push_back(frame);
}

void Display::clear_display()
{
    rects = {};

    // readd frame
    sf::RectangleShape frame {sf::Vector2f(dimension.x, dimension.y)};
    frame.setOutlineThickness(0.5f);
    frame.setFillColor(sf::Color::Black);
    frame.setOutlineColor(sf::Color::Green);
    rects.push_back(frame);
}


const std::vector<sf::RectangleShape>& Display::get_graphics() const
{
    return rects;
}
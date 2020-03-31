#include "Display.hpp"

Display::Display(Dim d, Pos p)
    :dimension{d}, position{p}
{
}

void Display::clear_display()
{
    rects = {};
}


const std::vector<sf::RectangleShape>& Display::get_graphics() const
{
    return rects;
}
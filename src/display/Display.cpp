#include "Display.hpp"
#include <iostream>

Display::Display()
{
    //sf::RectangleShape frame {sf::Vector2f(dimension.x, dimension.y)};
    //frame.setOutlineThickness(100.f);
    //frame.setFillColor(sf::Color::Black);
    //frame.setOutlineColor(sf::Color::Green);
    //rects.push_back(frame);
}

void Display::clear_display()
{
    rects = {};
    grid = {};
    // readd frame
    //sf::RectangleShape frame {sf::Vector2f(dimension.x, dimension.y)};
    //frame.setOutlineThickness(0.5f);
    //frame.setFillColor(sf::Color::Black);
    //frame.setOutlineColor(sf::Color::Green);
    //rects.push_back(frame);
}

const std::vector<sf::RectangleShape>& Display::get_graphics()
{
    //rects={};
    //grid={};
    //std::cout << rects.size() << std::endl;
    construct_frame();
    return rects;
}

bool Display::add_graphic(uint8_t x, uint8_t y, uint8_t byte)
{
    bool collision = false;
    for (size_t i = 0; i < 8; ++i)
    {
        if (((byte >> (8 - (i + 1))) & 0b1) == 1)
        {
            uint8_t draw_index_x = x + i;
            uint8_t draw_index_y = y;

            // if trying to draw outside of the frame
            // wrap to the opposing side
            draw_index_x %= 64;
            draw_index_y %= 32;

            if (grid[draw_index_x][draw_index_y])
            {
                collision = true;
                grid[draw_index_x][draw_index_y] = false;
                continue;
            }
            grid[draw_index_x][draw_index_y] = true;
        }
    }
    return collision;
}

void Display::construct_frame()
{
    rects = {};
    sf::RectangleShape frame {sf::Vector2f(dimension.x, dimension.y)};
    frame.setOutlineThickness(1.F);
    frame.setFillColor(sf::Color::Black);
    frame.setOutlineColor(sf::Color::Green);
    rects.push_back(frame);
    for (size_t i = 0; i < grid.size(); ++i)
    {
        for (size_t j = 0; j < grid[i].size(); ++j)
        {
            if (grid[i][j])
            {
                //std::cout << "drawing at " << i << "|" << j << '\n';
                sf::RectangleShape tmp(sf::Vector2f{1.F,1.F});
                tmp.setPosition(sf::Vector2f(i,j));
                tmp.setFillColor(sf::Color::Green);
                rects.push_back(tmp);
                //std::cout << rects.size() << std::endl;
            }
        }
    }
}
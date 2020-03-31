#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <SFML/Graphics.hpp>

class Display
{
public:
    struct Dim
    {
        int x;
        int y;
    };
    struct Pos
    {
        int x;
        int y;
    };

    Display(Dim d, Pos p);

    void clear_display();
    const std::vector<sf::RectangleShape>& get_graphics() const;

    // return true if collisions happen
    bool add_graphic(Dim d, Pos p);
private:
    Dim dimension;
    Pos position;
    std::vector<sf::RectangleShape> rects;
};

#endif // DISPLAY_HPP
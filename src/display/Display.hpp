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

    Display();

    void clear_display();
    const std::vector<sf::RectangleShape>& get_graphics() const;

    // return true if collisions happen
    bool add_graphic(Dim d, Pos p);
private:
    Dim dimension {64, 32};
    Pos position;
    std::vector<sf::RectangleShape> rects;
};

#endif // DISPLAY_HPP
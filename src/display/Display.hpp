#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <array>
#include <SFML/Graphics.hpp>

class Display
{
public:
    struct Dim
    {
        int x;
        int y;
    };

    Display();

    void clear_display();
    const std::vector<sf::RectangleShape>& get_graphics();

    // return true if collisions happen
    bool add_graphic(uint8_t x, uint8_t y, uint8_t byte);

private:
    void construct_frame();

    Dim dimension {64, 32};

    std::array<std::array<bool, 32>, 64> grid{};
    std::vector<sf::RectangleShape> rects;
};

#endif // DISPLAY_HPP
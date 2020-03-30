#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <array>
#include <SFML/Graphics.hpp>

class Keyboard
{
public:
    Keyboard();
    explicit Keyboard(std::array<sf::Keyboard::Key, 16> key_map);

    bool check(uint8_t k) const;
    void notify(sf::Keyboard::Key k, bool pressed);
private:  
    struct Key_state
    {
        sf::Keyboard::Key type;
        bool pressed = false;
    };

    std::array<Key_state, 16> keys;
};

#endif // KEYBOARD_HPP
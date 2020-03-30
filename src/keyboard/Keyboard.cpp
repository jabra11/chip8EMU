#include "Keyboard.hpp"

Keyboard::Keyboard()
{
    using k = sf::Keyboard::Key;
    keys[0] = {k::Num0};
    keys[1] = {k::Num1};
    keys[2] = {k::Num2};
    keys[3] = {k::Num3};
    keys[4] = {k::Num4};
    keys[5] = {k::Num5};
    keys[6] = {k::Num6};
    keys[7] = {k::Num7};
    keys[8] = {k::Num8};
    keys[9] = {k::Num9};
    keys[0xA] = {k::A};
    keys[0xB] = {k::B};
    keys[0xC] = {k::C};
    keys[0xD] = {k::D};
    keys[0xE] = {k::E};
    keys[0xF] = {k::F};
}

Keyboard::Keyboard(std::array<sf::Keyboard::Key, 16> key_map)
{
    for (size_t i = 0; i < keys.size(); ++i)
    {
        keys[i] = {key_map[i]};
    }
}

bool Keyboard::check(uint8_t k) const
{
    if (keys[k].pressed)
        return true;
    return false;
}

void Keyboard::notify(sf::Keyboard::Key k, bool pressed)
{
    auto it = std::find_if(keys.begin(), keys.end(), 
            [k](Keyboard::Key_state key){return key.type == k;});
    if (it != keys.end())
        it->pressed = pressed;
}
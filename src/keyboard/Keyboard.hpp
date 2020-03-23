#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <map>

class Keyboard
{
public:
    Keyboard keyboard();

    // implement some routine which converts
    // the default mapping to a user-defined mapping
private:  
    std::map<int, int> remap;
};

#endif // KEYBOARD_HPP
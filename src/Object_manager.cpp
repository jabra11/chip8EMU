#include <utility>
#include <exception>

#include "Object_manager.hpp"

Object_manager::Object_manager()
{
    default_font.loadFromFile("../resources/fonts/arial.ttf");
}

void Object_manager::add_text(const Object_manager::Object_property &properties)
{
    sf::Text tmp{properties.name, default_font};
    tmp.setPosition(properties.pos);
    tmp.setFillColor(properties.color);
    tmp.setCharacterSize(properties.font_size);
    tmp.setOrigin(properties.dim.x/2, properties.dim.y/2);

    texts.insert(std::make_pair(properties.name, std::move(tmp)));
}

void Object_manager::remove_text(const std::string& name)
{
    texts.erase(name);
}

void Object_manager::add_rectangle(const Object_manager::Object_property &properties)
{
    sf::RectangleShape tmp {properties.dim};
    tmp.setPosition(properties.pos);
    tmp.setFillColor(properties.color);
    tmp.setOrigin(properties.dim.x/2, properties.dim.y/2);

    rectangles.insert(std::make_pair(properties.name, std::move(tmp)));
}

void Object_manager::remove_rectangle(const std::string& name)
{
    rectangles.erase(name);
}

sf::Text& Object_manager::get_text_ref(const std::string &name)
{
    if (texts.find(name) == texts.end())
        throw std::runtime_error{"Did not find rectangle."};
    return texts[name];
}

const sf::Text& Object_manager::get_text_cref(const std::string &name)
{
    if (texts.find(name) == texts.end())
        throw std::runtime_error{"Did not find rectangle."};
    return texts[name];
}

sf::RectangleShape& Object_manager::get_rectangle_ref(const std::string &name)
{
    if (rectangles.find(name) == rectangles.end())
        throw std::runtime_error{"Did not find rectangle."};
    return rectangles[name];
}

const sf::RectangleShape& Object_manager::get_rectangle_cref(const std::string &name)
{
    if (rectangles.find(name) == rectangles.end())
        throw std::runtime_error{"Did not find rectangle."};
    return rectangles[name];
}
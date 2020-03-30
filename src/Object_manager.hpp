#ifndef OBJECT_MANAGER_HPP
#define OBJECT_MANAGER_HPP

#include <map>
#include <SFML/Graphics.hpp>

class Object_manager
{
public:
    struct Object_property
    {
        //Object_property(std::string n, sf::Vector2f d, sf::Vector2f p,
        //        sf::Color c, int fs = 10)
        //    :name{std::move(n)}, dim{d}, pos{p}, color{c}, font_size{fs}
        //{
        //}
        std::string name;
        sf::Vector2f dim;
        sf::Vector2f pos;
        sf::Color color;
        int font_size = 15;
    };

    Object_manager();

    void add_text(const Object_property& properties);
    void remove_text(const std::string& size);

    void add_rectangle(const Object_property& properties);
    void remove_rectangle(const std::string& name);

    sf::Text& get_text_ref(const std::string& name);

    // const reference to text
    const sf::Text& get_text_cref(const std::string& name);
    
    sf::RectangleShape& get_rectangle_ref(const std::string& name);

    // const reference to rectangle
    const sf::RectangleShape& get_rectangle_cref(const std::string& name);

    Object_property get_properties_of_text(const std::string& name);
    Object_property get_properties_of_rectangle(const std::string& name);

    void modify_string(const std::string& name, const std::string& new_string);

    void modify_text(const std::string& name, const Object_property& new_properties);
    void modify_rectangle(const std::string& name, const Object_property& new_properties);

private:
    sf::Font default_font;

    std::map<std::string, sf::RectangleShape> rectangles;
    std::map<std::string, sf::Text> texts;
};

#endif // OBJECT_MANAGER_HPP
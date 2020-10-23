#pragma once

#include <SFML/Graphics.hpp>

#include <string>

namespace resources {

class textured_sprite : public sf::Drawable {
    struct texture_part {
        sf::Texture texture;
        sf::Sprite sprite;
    };

    std::vector<texture_part> m_parts;

public:
    explicit textured_sprite() = default;

    void load(const std::string& path);

    // sf::Sprite-like interface

    void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

    void setOrigin(const sf::Vector2f& origin);

    void setColor(const sf::Color& color);

    void setScale(float scale_x, float scale_y);

    sf::FloatRect getLocalBounds() const;
};

} // namespace resources

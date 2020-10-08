#pragma once

#include <SFML/Graphics.hpp>

#include <string>

namespace resources {

class textured_sprite : public sf::Sprite {
    sf::Texture m_texture;

public:
    explicit textured_sprite() = default;

    void load(const std::string& path);
};

} // namespace resources

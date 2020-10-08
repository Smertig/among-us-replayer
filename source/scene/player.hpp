#pragma once

#include <resources/textured_sprite.hpp>

namespace scene {

class player {
    std::uint8_t m_id;
    resources::textured_sprite m_alive_sprite;
    resources::textured_sprite m_dead_sprite;
    resources::textured_sprite m_dead_body_sprite;
    sf::Transform m_body_transform;
    sf::Transform m_body_direction;
    sf::Transform m_dead_body_transform;
    bool m_is_dead = false;
    bool m_is_reported = false;
    sf::Text m_name;

public:
    explicit player(std::uint8_t id, std::uint8_t color, const std::string& name, bool is_impostor);

    std::uint8_t get_id() const;

    void set_scale(float alive_scale, float dead_scale, float dead_body_scale);

    void set_position(sf::Vector2f pos, bool dir_is_right);

    void set_is_dead(bool is_dead);

    void on_meeting();

    void draw(sf::RenderTarget& target, const sf::Transform& parent_transform) const;
};

} // namespace scene

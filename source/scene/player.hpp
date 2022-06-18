#pragma once

#include <resources/textured_sprite.hpp>

namespace scene {

class player {
    struct colored_sprite : public sf::Drawable {
        resources::textured_sprite frame;
        resources::textured_sprite inner;

        void load(const std::string& frame_path, const std::string& inner_path) {
            frame.load(frame_path);
            inner.load(inner_path);
        }

        void setColor(sf::Color color) {
            frame.setColor(sf::Color(255, 255, 255, color.a));
            inner.setColor(color);
        }

        void setOrigin(const sf::Vector2f& origin) {
            frame.setOrigin(origin);
            inner.setOrigin(origin);
        }

        void setScale(float scale_x, float scale_y) {
            frame.setScale(scale_x, scale_y);
            inner.setScale(scale_x, scale_y);
        }

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
            target.draw(frame, states);
            target.draw(inner, states);
        }
    };

    std::uint8_t m_id;
    colored_sprite m_alive_sprite;
    colored_sprite m_dead_sprite;
    colored_sprite m_dead_body_sprite;
    sf::Transform m_body_transform;
    sf::Transform m_body_direction;
    sf::Transform m_dead_body_transform;
    bool m_is_dead = false;
    bool m_is_reported = false;
    sf::Text m_name;

public:
    explicit player(std::uint8_t id, std::uint8_t color_id, const std::string& name, bool is_impostor);

    std::uint8_t get_id() const;

    void set_scale(float alive_scale, float dead_scale, float dead_body_scale);

    void set_position(sf::Vector2f pos, bool dir_is_right);

    void set_is_dead(bool is_dead);

    void on_meeting();

    void draw(sf::RenderTarget& target, const sf::Transform& parent_transform) const;
};

} // namespace scene

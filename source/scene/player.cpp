#include "player.hpp"
#include <resources/config.hpp>

#include <fmt/format.h>

namespace scene {

player::player(std::uint8_t id, std::uint8_t color, const std::string& name, bool is_impostor) {
    m_id = id;

    m_alive_sprite.setOrigin(resources::config::get_player_origin());
    m_alive_sprite.load(resources::config::get_player_path(color));

    m_dead_sprite.setOrigin(resources::config::get_ghost_origin());
    m_dead_sprite.load(resources::config::get_ghost_path(color));
    m_dead_sprite.setColor(sf::Color(255, 255, 255, 100));

    m_dead_body_sprite.setOrigin(resources::config::get_body_origin());
    m_dead_body_sprite.load(resources::config::get_body_path(color));

    static sf::Font font = []{
        sf::Font font;
        if (!font.loadFromFile("res/arial.ttf")) {
            throw std::runtime_error("unable to load res/arial.ttf font");
        }
        return font;
    }();

    m_name.setFont(font);
    m_name.setString(sf::String::fromUtf8(name.begin(), name.end()));
    m_name.setStyle(sf::Text::Bold);
    m_name.setOutlineColor(sf::Color::Black);
    m_name.setFillColor(is_impostor ? sf::Color::Red : sf::Color::White);
}

std::uint8_t player::get_id() const {
    return m_id;
}

void player::set_scale(float alive_scale, float dead_scale, float dead_body_scale) {
    // TODO: move '-' to config
    m_alive_sprite.setScale(-alive_scale, alive_scale);
    m_dead_sprite.setScale(dead_scale, dead_scale);
    m_dead_body_sprite.setScale(dead_body_scale, dead_body_scale);

    // dirty hacks..
    const auto expected_width = 87.f;
    const auto real_width = m_alive_sprite.getLocalBounds().width * alive_scale;

    m_name.setCharacterSize(static_cast<unsigned>(25 * real_width / expected_width));
    m_name.setOutlineThickness(2.0f * real_width / expected_width);
    m_name.setPosition(-m_name.getLocalBounds().width/2, -m_alive_sprite.getLocalBounds().height * 1.2 * alive_scale);
}

void player::set_position(sf::Vector2f pos, bool dir_is_right) {
    m_body_transform = sf::Transform().translate(pos);
    m_body_direction = sf::Transform().scale(dir_is_right ? 1.0f : -1.0f, 1.0f);
}

void player::set_is_dead(bool is_dead) {
    if (!m_is_dead && is_dead) {
        m_dead_body_transform = m_body_transform;
        m_is_reported = false;
    }

    m_is_dead = is_dead;
}

void player::on_meeting() {
    m_is_reported = true;
}

void player::draw(sf::RenderTarget& target, const sf::Transform& parent_transform) const {
    if (m_is_dead) {
        target.draw(m_dead_sprite, parent_transform * m_body_transform * m_body_direction);
        if (!m_is_reported) {
            target.draw(m_dead_body_sprite, parent_transform * m_dead_body_transform);
        }
    }
    else {
        target.draw(m_alive_sprite, parent_transform * m_body_transform * m_body_direction);
    }

    target.draw(m_name, parent_transform * m_body_transform);
}

} // namespace scene

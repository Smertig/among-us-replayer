#include "player.hpp"
#include <resources/config.hpp>

#include <fmt/format.h>

namespace scene {

player::player(std::uint8_t id, std::uint8_t color_id, const std::string& name, bool is_impostor) {
    m_id = id;

    auto color = [color_id] {
        const auto result = resources::config::try_get_color(color_id);
        if (!result) {
            throw std::runtime_error(fmt::format("unexpected color id #{}", color_id));
        }

        return *result;
    }();

    auto init_sprite = [color](colored_sprite& sprite, const auto& config, std::uint8_t alpha = 0xFF) {
        sprite.load(config.frame_path, config.inner_path);
        sprite.setColor(sf::Color(color.r, color.g, color.b, alpha));
        sprite.setOrigin(config.origin);
    };

    init_sprite(m_alive_sprite, resources::config::get_player_texture());
    init_sprite(m_dead_sprite, resources::config::get_ghost_texture(), 130);
    init_sprite(m_dead_body_sprite, resources::config::get_body_texture());

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
    const auto actual_size = m_alive_sprite.frame.getLocalBounds();
    const auto real_width = actual_size.width * alive_scale;

    m_name.setCharacterSize(static_cast<unsigned>(25 * real_width / expected_width));
    m_name.setOutlineThickness(2.0f * real_width / expected_width);
    m_name.setPosition(std::round(-m_name.getLocalBounds().width / 2.f), std::round(-actual_size.height * 1.2f * alive_scale));
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

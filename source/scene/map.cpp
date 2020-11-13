#include "map.hpp"
#include "player.hpp"
#include <resources/config.hpp>
#include <resources/cache.hpp>

#include <fmt/format.h>

#include <stdexcept>

namespace scene {

player& map::get_player(std::uint8_t id) {
    auto it = std::find_if(m_players.begin(), m_players.end(), [id](auto& pl) {
        return pl->get_id() == id;
    });

    if (it == m_players.end()) {
        throw std::runtime_error(fmt::format("player {} not found", id));
    }

    return **it;
}

map::map(int id) : m_id(id), m_background(resources::get_cached_sprite(resources::config::get_map_path(m_id))) {
    m_converter = resources::config::get_map_position_converter(m_id);
}

map::~map() = default;

void map::add_player(std::unique_ptr<player> player) {
    player->set_scale(
            resources::config::get_player_scale(m_id),
            resources::config::get_ghost_scale(m_id),
            resources::config::get_body_scale(m_id)
    );

    m_players.emplace_back(std::move(player));
}

void map::set_player_state(std::uint8_t id, const std::array<float, 2>& position, const std::array<float, 2>& velocity, bool is_dead) {
    auto& player = get_player(id);

    player.set_position(m_converter({ position[0], position[1] }), velocity[0] > 0);
    player.set_is_dead(is_dead);
}

void map::set_meeting(std::uint8_t id) {
    get_player(id).on_meeting();
}

sf::Vector2f map::get_center() const {
    return m_converter(resources::config::get_center(m_id));
}

float map::get_default_zoom() const {
    return resources::config::get_default_zoom(m_id);
}

void map::draw(sf::RenderTarget& target, const sf::Transform& parent_transform) const {
    const sf::Transform transform = parent_transform * m_transform;

    target.draw(m_background, transform);

    for (const auto& player : m_players) {
        player->draw(target, transform);
    }
}

sf::Vector2f map::convert_position(const std::array<float, 2>& game_position) const {
    return m_converter({ game_position[0], game_position[1] });
}

} // namespace scene

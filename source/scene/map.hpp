#pragma once

#include <resources/textured_sprite.hpp>

#include <array>
#include <functional>

namespace scene {

class player;

class map {
    resources::textured_sprite m_background;
    sf::Transform m_transform;
    std::vector<std::unique_ptr<player>> m_players;
    int m_id;
    std::function<sf::Vector2f(const sf::Vector2f&)> m_converter;

    void load_map();

    player& get_player(std::uint8_t id);

public:
    explicit map(int id);

    ~map();

    void add_player(std::unique_ptr<player> player);

    void set_player_state(std::uint8_t id, const std::array<float, 2>& position, const std::array<float, 2>& velocity, bool is_dead);

    void set_meeting(std::uint8_t id);

    sf::Vector2f get_center() const;

    float get_default_zoom() const;

    void draw(sf::RenderTarget& target, const sf::Transform& parent_transform) const;

    sf::Vector2f convert_position(const std::array<float, 2>& game_position) const;
};

} // namespace scene

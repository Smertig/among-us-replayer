#pragma once

#include <SFML/System/Vector2.hpp>

#include <vector>
#include <string>
#include <functional>
#include <optional>

namespace resources {

class config {
public:
    struct map_config {
        std::string path;
        float player_pixel_width;
        float ghost_pixel_width;
        float body_pixel_width;
        sf::Vector2f center;
        float default_zoom;
        std::function<sf::Vector2f(const sf::Vector2f&)> position_converter;
    };

    struct texture_config {
        std::string path;
        float width;
        sf::Vector2f origin;
    };

private:
    std::vector<map_config> m_maps;
    texture_config m_player;
    texture_config m_ghost;
    texture_config m_body;
    std::vector<std::uint32_t> m_colors;

    config();

    static const config& instance();

public:
    static const sf::Vector2f& get_player_origin();
    static const sf::Vector2f& get_ghost_origin();
    static const sf::Vector2f& get_body_origin();

    static float get_player_scale(int map_id);
    static float get_ghost_scale(int map_id);
    static float get_body_scale(int map_id);

    static const std::string& get_map_path(int map_id);
    static const std::function<sf::Vector2f(const sf::Vector2f&)>& get_map_position_converter(int map_id);
    static float get_default_zoom(int map_id);
    static const sf::Vector2f& get_center(int map_id);

    static std::string get_player_path(int color);
    static std::string get_ghost_path(int color);
    static std::string get_body_path(int color);

    static std::optional<std::uint32_t> try_get_color(int color_id);
};

} // namespace resources


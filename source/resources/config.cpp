#include "config.hpp"

#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include <fstream>

namespace sf {

template <class T>
void from_json(const nlohmann::json& j, sf::Vector2<T>& result) {
    auto value = j.get<std::array<T, 2>>();
    result.x = value[0];
    result.y = value[1];
}

} // namespace sf

namespace resources {

void from_json(const nlohmann::json& j, config::texture_config& result) {
    result.frame_path = j.at("frame_path").get<std::string>();
    result.inner_path = j.at("inner_path").get<std::string>();
    result.width = j.at("pixel_width").get<float>();
    result.origin = j.at("origin").get<sf::Vector2f>();
}

void from_json(const nlohmann::json& j, config::map_config& result) {
    result.path = j.at("path").get<std::string>();
    result.player_pixel_width = j.at("player_pixel_width").get<float>();
    result.ghost_pixel_width = j.at("ghost_pixel_width").get<float>();
    result.body_pixel_width = j.at("body_pixel_width").get<float>();
    result.center = j.at("center").get<sf::Vector2f>();
    result.default_zoom = j.at("default_zoom").get<float>();

    const auto calibration = j.at("calibration");
    const auto game_pos = calibration.at("game_pos").get<std::array<sf::Vector2f, 2>>();
    const auto tex_pos = calibration.at("texture_pos").get<std::array<sf::Vector2f, 2>>();

    const auto delta_game_pos = game_pos[1] - game_pos[0];
    const auto delta_tex_pos = tex_pos[1] - tex_pos[0];

    // texture = game * scale + shift
    const auto scale = sf::Vector2f(delta_tex_pos.x / delta_game_pos.x, delta_tex_pos.y / delta_game_pos.y);
    const auto shift = sf::Vector2f(tex_pos[0].x - game_pos[0].x * scale.x, tex_pos[0].y - game_pos[0].y * scale.y);

    result.position_converter = [scale, shift](const sf::Vector2f& game_pos) {
        return sf::Vector2f {
                game_pos.x * scale.x + shift.x,
                game_pos.y * scale.y + shift.y
        };
    };
}

resources::config::config() {
    std::ifstream ifs("res/config.json");
    if (!ifs) {
        throw std::runtime_error("unable to open res/config.json");
    }

    auto config = nlohmann::json::parse(ifs);

    m_player = config.at("player").get<config::texture_config>();
    m_ghost = config.at("ghost").get<config::texture_config>();
    m_body = config.at("body").get<config::texture_config>();
    m_maps = config.at("maps").get<std::vector<config::map_config>>();

    if (m_maps.size() != 3) {
        throw std::runtime_error("expected 3 maps in config");
    }

    const auto color_strings = config.at("colors").get<std::vector<std::string>>();
    for (const auto& color_string : color_strings) {
        if (color_string.size() != 7 || color_string[0] != '#') {
            throw std::runtime_error("expected color in form #RRGGBB");
        }

        std::uint32_t color = std::stol(color_string.substr(1), nullptr, 16);
        m_colors.emplace_back(rgb_color{
            /* .r = */ static_cast<std::uint8_t>(color >> 16),
            /* .g = */ static_cast<std::uint8_t>(color >>  8),
            /* .b = */ static_cast<std::uint8_t>(color >>  0)
        });
    }
}

const config &config::instance() {
    static const config instance;
    return instance;
}

float config::get_player_scale(int map_id) {
    return instance().m_maps.at(map_id).player_pixel_width / instance().m_player.width;
}

float config::get_ghost_scale(int map_id) {
    return instance().m_maps.at(map_id).ghost_pixel_width / instance().m_ghost.width;
}

float config::get_body_scale(int map_id) {
    return instance().m_maps.at(map_id).body_pixel_width / instance().m_body.width;
}

const std::string& config::get_map_path(int map_id) {
    return instance().m_maps.at(map_id).path;
}

const std::function<sf::Vector2f(const sf::Vector2f &)>& config::get_map_position_converter(int map_id) {
    return instance().m_maps.at(map_id).position_converter;
}

float config::get_default_zoom(int map_id) {
    return instance().m_maps.at(map_id).default_zoom;
}

const sf::Vector2f &config::get_center(int map_id) {
    return instance().m_maps.at(map_id).center;
}

const config::texture_config& config::get_player_texture() {
    return instance().m_player;
}

const config::texture_config& config::get_ghost_texture() {
    return instance().m_ghost;
}

const config::texture_config& config::get_body_texture() {
    return instance().m_body;
}

std::optional<rgb_color> config::try_get_color(int color_id) {
    const auto& colors = instance().m_colors;
    if (color_id >= colors.size()) {
        return std::nullopt;
    }

    return colors[color_id];
}

} // namespace resources

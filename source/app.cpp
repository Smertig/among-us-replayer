#include "app.hpp"

#include <resources/font.hpp>
#include <resources/config.hpp>
#include <resources/cache.hpp>
#include <util/platform.hpp>
#include <scene/player.hpp>

#include <imgui.h>
#include <imgui-SFML.h>
#include <fmt/format.h>

#include <fstream>

app::app(const std::string& app_name)
    : m_window(sf::VideoMode::getFullscreenModes().front(), app_name)
    , m_imgui_inited((ImGui::SFML::Init(m_window, false), true))
    , m_camera(m_window)
    , m_ui_file_dialog([this](std::filesystem::path replay_path) { open_replay(std::move(replay_path)); })
{
    m_window.setVerticalSyncEnabled(true);

    ::init_fonts();

    for (int i = 0; i < 3; i++) {
        resources::preload_sprite(resources::config::get_map_path(i));
    }
}

app::~app() {
    resources::clear_cache();

    ImGui::SFML::Shutdown();
}

void app::run() {
    sf::Clock deltaClock;
    while (m_window.isOpen()) {
        const auto dt = deltaClock.restart();

        // Process events
        sf::Event event{};
        while (m_window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (!ImGui::GetIO().WantCaptureMouse) {
                m_camera.process_event(event);
            }

            if (event.type == sf::Event::Closed) {
                m_window.close();
            }
        }

        // Update
        ImGui::SFML::Update(m_window, dt);
        if (m_current_map) {
            m_current_map->update(dt.asMilliseconds());
        }
        m_ui_file_dialog.update(dt.asMilliseconds());

        // Render
        m_window.clear();
        m_window.setView(m_camera.view());
        if (m_current_map) {
            m_current_map->render();
        }
        m_ui_file_dialog.render();
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
}

void app::open_replay(std::filesystem::path replay_path) {
    try {
        std::fstream ifs(replay_path, std::fstream::binary | std::fstream::in);
        ifs.exceptions(std::fstream::eofbit | std::fstream::failbit | std::fstream::badbit);

        ::replay replay_data;
        replay_data.parse(ifs);

        float old_zoom = m_current_map ? m_current_map->map().get_default_zoom() : 1.0f;

        m_current_map.emplace(m_window, m_camera, std::move(replay_data));

        m_camera.zoom(m_current_map->map().get_default_zoom() / old_zoom);
    }
    catch (std::exception& e) {
        platform::msgbox_warning(fmt::format("Unable to parse replay: {}", e.what()));
    }
}

app::map_state::map_state(sf::RenderWindow &window, scene::camera &camera, ::replay replay)
    : m_window(window)
    , m_camera(camera)
    , m_replay(std::move(replay))
    , m_ui_player(m_window, m_replay)
    , m_map(m_replay.get_map_id())
{
    std::vector<std::unique_ptr<scene::player>> players;
    for (const auto& [id, info] : m_replay.get_players()) {
        m_map.add_player(std::make_unique<scene::player>(info.id, info.color, info.name, info.impostor));
    }

    m_camera.set_center(m_map.get_center());
}

void app::map_state::update(int dt) {
    m_ui_player.update(dt);

    if (m_replay.is_meeting(m_ui_player.get_time())) {
        for (const auto&[id, info] : m_replay.get_players()) {
            m_map.set_meeting(id);
        }

        m_ui_player.update(1000);
    }

    for (const auto& [id, info] : m_replay.get_players()) {
        const auto state = info.get_interpolated(m_ui_player.get_time());
        m_map.set_player_state(id, state.position, state.velocity, state.is_dead);
    }

    if (m_camera.was_dragged()) {
        m_ui_player.reset_traced_player();
    }
    else if (auto player_id = m_ui_player.get_traced_player_id(); player_id.has_value()) {
        auto player_state = m_replay.get_players().at(*player_id).get_interpolated(m_ui_player.get_time());
        m_camera.set_center(m_map.convert_position(player_state.position));
    }
}

void app::map_state::render() {
    m_map.draw(m_window, sf::Transform::Identity);
    m_ui_player.render();
}

const scene::map &app::map_state::map() const {
    return m_map;
}

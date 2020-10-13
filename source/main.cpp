#include <scene/map.hpp>
#include <scene/player.hpp>
#include <scene/camera.hpp>
#include <ui/player.hpp>
#include <replay/replay.hpp>
#include <util/platform.hpp>
#include <resources/font.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include <fmt/format.h>

#include <optional>
#include <fstream>
#include <filesystem>

int main(int argc, char** argv) {
    std::string replay_path;
    if (argc < 2) {
        auto maybe_path = platform::choose_replay_file();
        if (!maybe_path) {
            platform::msgbox_warning("No file selected");
            return 1;
        }

        replay_path = *maybe_path;
    }
    else {
        replay_path = argv[1];
    }

    try {
        // Open replay data
        std::fstream ifs(replay_path, std::fstream::binary | std::fstream::in);
        ifs.exceptions(std::fstream::eofbit | std::fstream::failbit | std::fstream::badbit);

        // Parse it
        replay r;
        r.parse(ifs);

        // Try fix paths
        if (!std::filesystem::exists("res/config.json")) {
            platform::fix_working_directory();
        }

        // Make window
        sf::RenderWindow window(sf::VideoMode::getFullscreenModes().front(), platform::get_app_fullname());
        window.setVerticalSyncEnabled(true);

        // Prepare UI
        ImGui::SFML::Init(window, false);
        init_fonts();
        ui::player ui_player(window, r);

        // Prepare map and camera
        scene::map m(r.get_map_id());
        scene::camera camera(window);

        for (const auto& [id, info] : r.get_players()) {
            m.add_player(std::make_unique<scene::player>(info.id, info.color, info.name, info.impostor));
        }

        camera.set_center(m.get_center());
        camera.zoom(m.get_default_zoom());

        sf::Clock deltaClock;
        while (window.isOpen()) {
            const auto dt = deltaClock.restart();

            // Process events
            bool was_dragged = false;
            sf::Event event{};
            while (window.pollEvent(event)) {
                ImGui::SFML::ProcessEvent(event);

                if (!ImGui::GetIO().WantCaptureMouse) {
                    camera.process_event(event, was_dragged);
                }

                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            // Update
            ImGui::SFML::Update(window, dt);
            ui_player.update(dt.asMilliseconds());

            if (r.is_meeting(ui_player.get_time())) {
                for (const auto& [id, info] : r.get_players()) {
                    m.set_meeting(id);
                }

                ui_player.update(1000);
            }

            for (const auto& [id, info] : r.get_players()) {
                const auto state = info.get_interpolated(ui_player.get_time());
                m.set_player_state(id, state.position, state.velocity, state.is_dead);
            }

            if (was_dragged) {
                ui_player.reset_traced_player();
            }
            else if (auto player_id = ui_player.get_traced_player_id(); player_id.has_value()) {
                auto player_state = r.get_players().at(*player_id).get_interpolated(ui_player.get_time());
                camera.set_center(m.convert_position(player_state.position));
            }

            // Render
            window.clear();
            window.setView(camera.view());
            m.draw(window, sf::Transform::Identity);
            ui_player.render();
            ImGui::SFML::Render(window);
            window.display();
        }

        ImGui::SFML::Shutdown();

        return 0;
    }
    catch (std::exception& e) {
        platform::msgbox_warning(fmt::format("ERROR:\n{}", e.what()));
        return 2;
    }
}

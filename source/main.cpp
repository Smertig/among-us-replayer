#include <scene/map.hpp>
#include <scene/player.hpp>
#include <scene/camera.hpp>
#include <ui/player.hpp>
#include <replay/replay.hpp>
#include <util/platform.hpp>

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
        ui::player player(window);
        ImGui::SFML::Init(window);

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
            sf::Event event{};
            while (window.pollEvent(event)) {
                ImGui::SFML::ProcessEvent(event);

                if (!ImGui::GetIO().WantCaptureMouse) {
                    camera.process_event(event);
                }

                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            // Update
            ImGui::SFML::Update(window, dt);
            player.update(dt.asMilliseconds());

            if (r.is_meeting(player.get_time())) {
                for (const auto& [id, info] : r.get_players()) {
                    m.set_meeting(id);
                }

                player.update(1000);
            }
            else {
                for (const auto& [id, info] : r.get_players()) {
                    const auto state = info.get_interpolated(player.get_time());
                    m.set_player_state(id, state.position, state.velocity, state.is_dead);
                }
            }

            if (player.get_time() >= r.get_duration()) {
                if (platform::msgbox_ask("End of round. Restart?")) {
                    player.set_time(0);
                    deltaClock.restart();
                }
                else {
                    window.close();
                }
            }

            // Render
            window.clear();
            window.setView(camera.view());
            m.draw(window, sf::Transform::Identity);
            player.render();
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

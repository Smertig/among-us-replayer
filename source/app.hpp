#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include <scene/map.hpp>
#include <scene/camera.hpp>
#include <replay/replay.hpp>
#include <ui/player.hpp>
#include <ui/file_dialog.hpp>

#include <optional>
#include <string>
#include <filesystem>

class app {
    class map_state {
        sf::RenderWindow& m_window;
        scene::camera&    m_camera;

        ::replay   m_replay;
        scene::map m_map;
        ui::player m_ui_player;

    public:
        explicit map_state(sf::RenderWindow& window, scene::camera& camera, ::replay replay);

        void update(int dt);

        void render();

        const scene::map& map() const;
    };

    sf::RenderWindow         m_window;
    scene::camera            m_camera;
    ui::file_dialog          m_ui_file_dialog;
    std::optional<map_state> m_current_map;

public:
    explicit app(const std::string& app_name);

    ~app();

    void run();

private:
    void open_replay(std::filesystem::path replay_path);
};



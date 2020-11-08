#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <optional>
#include <functional>

#include <replay/replay.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

class replay;

namespace ui {

class file_dialog {
    struct file_entry {
        bool                  is_directory;
        std::string           display_name;
        std::filesystem::path path;
        std::optional<replay> replay_data;
    };

    std::vector<file_entry> m_cache;
    std::filesystem::path m_current_path;
    int m_time_to_cache_refresh = 0;
    std::function<void(const std::filesystem::path&)> m_open_file_callback;

public:
    explicit file_dialog(std::function<void(const std::filesystem::path&)> open_file_callback);

    void update(int dt);

    void render();

private:
    bool try_set_path(std::filesystem::path path);

    void refresh_cache();
};

} // namespace ui
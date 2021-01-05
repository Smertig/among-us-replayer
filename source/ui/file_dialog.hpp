#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <variant>
#include <functional>

#include <replay/replay.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

class replay;

namespace ui {

class file_dialog {
    struct file_entry {
        struct dir_data {};

        struct replay_data {
            replay r;
        };

        struct unknown_data {
            std::string exception;
        };

        std::string                                       display_name;
        std::filesystem::path                             path;
        std::variant<dir_data, replay_data, unknown_data> parse_result;

        bool is_directory() const {
            return parse_result.index() == 0;
        }
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

    void add_settings_handler();
};

} // namespace ui
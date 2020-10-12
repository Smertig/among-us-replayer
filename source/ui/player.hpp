#pragma once

#include <chrono>
#include <optional>

#include <SFML/Graphics/RenderWindow.hpp>

class replay;

namespace ui {

class player {
    int m_time = 0; // milliseconds
    int m_time_min = 0;
    int m_time_max = 0;

    bool m_paused = false;
    float m_speed = 1.0f;
    std::optional<std::uint8_t> m_traced_player_id;

    sf::RenderWindow& m_window;
    const replay& m_replay;

public:
    explicit player(sf::RenderWindow& window, const replay& replay);

    void update(int dt);

    void render();

    int get_time() const;

    void set_time(int t);

    std::optional<std::uint8_t> get_traced_player_id() const;

    void reset_traced_player();
};

} // namespace ui
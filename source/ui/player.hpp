#pragma once

#include <chrono>

#include <SFML/Graphics/RenderWindow.hpp>

class replay;

namespace ui {

class player {
    int m_time = 0; // milliseconds
    int m_time_min = 0;
    int m_time_max = 0;

    bool m_paused = false;
    float m_speed = 1.0f;

    sf::RenderWindow& m_window;

public:
    explicit player(sf::RenderWindow& window, const replay& replay);

    void update(int dt);

    void render();

    int get_time() const;

    void set_time(int t);
};

} // namespace ui
#pragma once

#include <chrono>

#include <SFML/Graphics/RenderWindow.hpp>

namespace ui {

class player {
    int m_time = 0; // milliseconds
    sf::RenderWindow& m_window;

public:
    explicit player(sf::RenderWindow& window);

    void update(int dt);

    void render();

    int get_time() const;

    void set_time(int t);
};

} // namespace ui
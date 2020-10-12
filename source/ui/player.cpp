#include "player.hpp"

namespace ui {

player::player(sf::RenderWindow& window) : m_time(0), m_window(window) {
    // nothing
}

void player::update(int dt) {
    m_time += dt * 3;
}

void player::render() {
    // nothing
}

int player::get_time() const {
    return m_time;
}

void player::set_time(int t) {
    m_time = t;
}

} // namespace ui

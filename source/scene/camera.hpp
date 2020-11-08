#pragma once

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <optional>

namespace scene {

class camera {
    sf::RenderWindow& m_window;
    sf::View m_view;
    float m_zoom = 1.0f;
    std::optional<sf::Vector2i> m_last_mouse_drag_position;
    bool m_was_dragged = false;

public:
    explicit camera(sf::RenderWindow& window);

    void zoom(float value);

    float zoom() const;

    void set_center(sf::Vector2f center);

    const sf::View& view() const;

    void resize(sf::Vector2f size);

    void move(sf::Vector2f delta);

    void process_event(const sf::Event& event);

    bool was_dragged() const;
};

} // namespace scene

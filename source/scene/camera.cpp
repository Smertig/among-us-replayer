#include "camera.hpp"

namespace scene {

camera::camera(sf::RenderWindow& window) : m_window(window), m_view(m_window.getDefaultView()) {
    // nothing
}

void camera::zoom(float value) {
    m_view.zoom(value);
    m_zoom *= value;
}

void camera::set_center(sf::Vector2f center) {
    m_view.setCenter(center);
}

const sf::View& camera::view() const {
    return m_view;
}

void camera::resize(sf::Vector2f size) {
    m_view.setSize(size);
    m_view.zoom(m_zoom);
}

void camera::move(sf::Vector2f delta) {
    m_view.move(delta);
}

void camera::process_event(const sf::Event& event) {
    switch (event.type) {
        case sf::Event::Resized: {
            resize({
                static_cast<float>(event.size.width),
                static_cast<float>(event.size.height)
            });
            break;
        }

        case sf::Event::MouseButtonPressed: {
            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                m_last_mouse_drag_position = { event.mouseButton.x, event.mouseButton.y };
            }
            break;
        }

        case sf::Event::MouseButtonReleased: {
            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                m_last_mouse_drag_position.reset();
            }
            break;
        }

        case sf::Event::MouseWheelScrolled: {
            zoom(1.f - event.mouseWheelScroll.delta / 30.f);
            break;
        }

        case sf::Event::MouseMoved: {
            if (m_last_mouse_drag_position) {
                sf::Vector2i current_mouse_position = { event.mouseMove.x, event.mouseMove.y };

                move(m_window.mapPixelToCoords(*m_last_mouse_drag_position) - m_window.mapPixelToCoords(current_mouse_position));

                m_last_mouse_drag_position = current_mouse_position;
            }
            break;
        }

        default: {
            // nothing
        }
    }
}

} // namespace scene
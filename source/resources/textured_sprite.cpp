#include "textured_sprite.hpp"

#include <fmt/format.h>

namespace resources {

void textured_sprite::load(const std::string &path) {
    if (!m_texture.loadFromFile(path)) {
        throw std::runtime_error(fmt::format("textured_sprite::load: unable to load '{}'", path));
    }

    m_sprite.setTexture(m_texture);
}

void textured_sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_sprite, states);
}

void textured_sprite::setOrigin(const sf::Vector2f &origin) {
    m_sprite.setOrigin(origin);
}

void textured_sprite::setColor(const sf::Color &color) {
    m_sprite.setColor(color);
}

void textured_sprite::setScale(float scale_x, float scale_y) {
    m_sprite.setScale(scale_x, scale_y);
}

sf::FloatRect textured_sprite::getLocalBounds() const {
    return m_sprite.getLocalBounds();
}

} // namespace resources

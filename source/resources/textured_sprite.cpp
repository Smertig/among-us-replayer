#include "textured_sprite.hpp"

#include <fmt/format.h>

namespace resources {

void textured_sprite::load(const std::string &path) {
    sf::Image image;
    if (!image.loadFromFile(path)) {
        throw std::runtime_error(fmt::format("textured_sprite::load: unable to open '{}'", path));
    }

    const auto texture_size      = image.getSize();
    const auto max_texture_width = static_cast<int>(sf::Texture::getMaximumSize());

    for (unsigned int y = 0; y < texture_size.y; y += max_texture_width) {
        for (unsigned int x = 0; x < texture_size.x; x += max_texture_width) {
            sf::IntRect rect{
                { static_cast<int>(x), static_cast<int>(y) },
                { max_texture_width, max_texture_width }
            };

            auto& part = m_parts.emplace_back(std::make_unique<texture_part>());
            if (!part->texture.loadFromFile(path, rect)) {
                throw std::runtime_error(fmt::format("textured_sprite::load: unable to load texture part at {}x{} from '{}'", x, y, path));
            }

            part->sprite.setTexture(part->texture);
            part->transform = sf::Transform().translate(static_cast<float>(x), static_cast<float>(y));
        }
    }
}

void textured_sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& part : m_parts) {
        sf::RenderStates part_state = states;

        part_state.transform = part->transform * part_state.transform;

        target.draw(part->sprite, part_state);
    }
}

void textured_sprite::setOrigin(const sf::Vector2f &origin) {
    for (const auto& part : m_parts) {
        part->sprite.setOrigin(origin.x, origin.y);
    }
}

void textured_sprite::setColor(const sf::Color &color) {
    for (const auto& part : m_parts) {
        part->sprite.setColor(color);
    }
}

void textured_sprite::setScale(float scale_x, float scale_y) {
    for (const auto& part : m_parts) {
        part->sprite.setScale(scale_x, scale_y);
    }
}

sf::FloatRect textured_sprite::getLocalBounds() const {
    auto left_top  = m_parts.front()->sprite.getLocalBounds();
    auto right_bot = m_parts.back()->sprite.getLocalBounds();

    return sf::FloatRect{
        left_top.left,
        left_top.top,
        right_bot.left + right_bot.width - left_top.left,
        right_bot.top + right_bot.height - left_top.top
    };
}

} // namespace resources

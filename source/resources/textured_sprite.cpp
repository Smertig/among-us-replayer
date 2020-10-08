#include "textured_sprite.hpp"

#include <fmt/format.h>

namespace resources {

void textured_sprite::load(const std::string &path) {
    if (!m_texture.loadFromFile(path)) {
        throw std::runtime_error(fmt::format("textured_sprite::load: unable to load '{}'", path));
    }

    setTexture(m_texture);
}

} // namespace resources

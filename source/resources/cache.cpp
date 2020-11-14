#include "cache.hpp"

#include <resources/textured_sprite.hpp>

#include <fmt/format.h>

#include <map>
#include <future>
#include <string>
#include <type_traits>

namespace {

template <class R>
class cached_resource_t {
    R                            m_resource;
    std::shared_future<const R&> m_thread;

public:
    template <class Loader>
    cached_resource_t(Loader&& loader) {
        static_assert(std::is_invocable_r_v<void, Loader&&, R&>);

        m_thread = std::async(std::launch::async, [this, loader = std::forward<Loader>(loader)]() mutable -> const R& {
            std::forward<Loader>(loader)(m_resource);
            return m_resource;
        });
    }

    const R& get() {
        return m_thread.get();
    }

    cached_resource_t(const cached_resource_t&) = delete;
    cached_resource_t& operator=(const cached_resource_t&) = delete;
};

std::map<std::string, cached_resource_t<resources::textured_sprite>, std::less<>> g_sprite_cache;

} // unnamed namespace

namespace resources {

void preload_sprite(std::string path) {
    auto [_, ok] = g_sprite_cache.emplace(path, [path](textured_sprite& sprite) {
        sprite.load(path);
    });

    if (!ok) {
        throw std::runtime_error(fmt::format("sprite '{}' is already cache", path));
    }
}

const textured_sprite& get_cached_sprite(std::string_view path) {
    auto it = g_sprite_cache.find(path);
    if (it == g_sprite_cache.end()) {
        throw std::runtime_error(fmt::format("sprite '{}' not found in cache", path));
    }

    return it->second.get();
}

void clear_cache() {
    g_sprite_cache.clear();
}

} // namespace resources

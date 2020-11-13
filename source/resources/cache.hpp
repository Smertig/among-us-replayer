#include <string>

namespace resources {

class textured_sprite;

void preload_sprite(std::string path);

const textured_sprite& get_cached_sprite(std::string_view path);

} // namespace resources

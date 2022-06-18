#include "utils.hpp"

#include <resources/config.hpp>

namespace ui::utils {

const char* get_map_name(int map_id) {
    switch (map_id) {
        case 0: return "Skeld";
        case 1: return "Mira HQ";
        case 2: return "Polus";
        default: return "???";
    }
}

ImColor ui::utils::convert_color(int color_id) {
    const auto color = resources::config::try_get_color(color_id);
    return color ? ImColor(color->r, color->g, color->b) : ImColor();
}

} // namespace ui::utils

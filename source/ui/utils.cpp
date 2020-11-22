#include "utils.hpp"

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
    switch (color_id) {
        case  0: return ImColor(0xC5, 0x11, 0x11);
        case  1: return ImColor(0x13, 0x2E, 0xD1);
        case  2: return ImColor(0x11, 0x7F, 0x2D);
        case  3: return ImColor(0xED, 0x54, 0xBA);
        case  4: return ImColor(0xEF, 0x7D, 0x0D);
        case  5: return ImColor(0xF5, 0xF5, 0x57);
        case  6: return ImColor(0x3F, 0x47, 0x4E);
        case  7: return ImColor(0xD6, 0xE0, 0xF0);
        case  8: return ImColor(0x6B, 0x2F, 0xBB);
        case  9: return ImColor(0x71, 0x49, 0x1E);
        case 10: return ImColor(0x38, 0xFE, 0xDC);
        case 11: return ImColor(0x50, 0xEF, 0x39);
        default: return ImColor();
    }
}

} // namespace ui::utils

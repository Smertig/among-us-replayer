#pragma once

#include <imgui.h>

namespace ui::utils {

const char* get_map_name(int map_id);

ImColor convert_color(int color_id);

} // namespace ui::utils
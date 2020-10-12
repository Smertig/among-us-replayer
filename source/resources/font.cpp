#include "font.hpp"

#include <imgui.h>
#include <imgui-SFML.h>

void init_fonts() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();

    // merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = {
        ICON_MIN_FA, ICON_MAX_FA,
        0
    };

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("res/" FONT_ICON_FILE_NAME_FAS, 13.0f, &icons_config, icons_ranges);

    ImGui::SFML::UpdateFontTexture();
}

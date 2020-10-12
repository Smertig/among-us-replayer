#include "font.hpp"

#include <imgui.h>
#include <imgui-SFML.h>

void init_fonts() {
    ImGuiIO& io = ImGui::GetIO();

    static const ImWchar glyph_ranges[] = {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // [Cyrillic] Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // [Cyrillic] Cyrillic Extended-A
        0xA640, 0xA69F, // [Cyrillic] Cyrillic Extended-B
        0x2600, 0x26ff, // https://unicode-table.com/ru/blocks/miscellaneous-symbols/
        0
    };

    io.Fonts->AddFontFromFileTTF("res/arial.ttf", 18.0f, nullptr, glyph_ranges);

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

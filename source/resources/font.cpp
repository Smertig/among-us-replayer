#include "font.hpp"

#include <imgui.h>
#include <imgui-SFML.h>

void init_fonts() {
    ImGuiIO& io = ImGui::GetIO();

    ImFontGlyphRangesBuilder glyph_builder;
    glyph_builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    glyph_builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
    glyph_builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
    glyph_builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

    ImVector<ImWchar> glyph_ranges;
    glyph_builder.BuildRanges(&glyph_ranges);

    io.Fonts->AddFontFromFileTTF("res/NotoSerifCJK-Black.ttc", 18.0f, nullptr, glyph_ranges.Data);

    // merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = {
        ICON_MIN_FA, ICON_MAX_FA,
        0
    };

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("res/" FONT_ICON_FILE_NAME_FAS, 15.0f, &icons_config, icons_ranges);

    ImGui::SFML::UpdateFontTexture();
}

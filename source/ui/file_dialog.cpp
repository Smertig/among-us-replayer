#include "file_dialog.hpp"

#include <resources/font.hpp>

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_stdlib.h>

#include <fstream>

namespace fs = std::filesystem;

namespace {

const char* get_map_name(int map_id) {
    switch (map_id) {
        case 0: return "Skeld";
        case 1: return "Mira HQ";
        case 2: return "Polus";
        default: return "???";
    }
}

ImColor convert_color(int color_id) {
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

} // unnamed namespace

namespace ui {

file_dialog::file_dialog(std::function<void(const std::filesystem::path&)> open_file_callback)
    : m_open_file_callback(std::move(open_file_callback))
{
    if (try_set_path(fs::current_path()) || try_set_path(fs::temp_directory_path())) {
        return;
    }

    throw std::runtime_error("Unable to set default path");
}

void file_dialog::update(int dt) {
    m_time_to_cache_refresh -= dt;

    if (m_time_to_cache_refresh <= 0) {
        refresh_cache();
        m_time_to_cache_refresh = 1000;
    }
}

void file_dialog::render() {
    ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2{ 600, 500 }, ImGuiCond_Always);

    ImGui::Begin("File Choose Dialog", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);

    ImGui::Text("%s", ICON_FA_FOLDER);
    ImGui::SameLine();
    ImGui::PushItemWidth(-1.0f);
    std::string path_buffer = m_current_path.generic_u8string();
    if (ImGui::InputText("##CurrentPath", &path_buffer)) {
        try_set_path(path_buffer);
    }

    ImGui::BeginChild("##Files", {}, true);

    ImGui::Columns(2);

    ImGui::SetColumnWidth(0, 380.0f);

    const replay* hovered_replay = nullptr;
    for (const file_entry& entry : m_cache) {
        if (entry.is_directory) {
            if (ImGui::Selectable(fmt::format("{} {}", ICON_FA_FOLDER_OPEN, entry.display_name).c_str())) {
                try_set_path(entry.path);
            }
            ImGui::NextColumn();
        }
        else if (entry.replay_data) {
            const auto& r = entry.replay_data.value();
            const auto& players = r.get_players();

            ImGui::PushStyleColor(ImGuiCol_Text, ImColor(153, 255, 128).Value);
            if (ImGui::Selectable(fmt::format("{} {}", ICON_FA_FILE_CODE, entry.display_name.c_str()).c_str())) {
                m_open_file_callback(entry.path);
            }
            ImGui::PopStyleColor(1);
            ImGui::NextColumn();

            if (ImGui::IsItemHovered()) {
                hovered_replay = &r;
            }

            ImGui::TextColored(ImColor(43, 255, 54), "%s", get_map_name(r.get_map_id()));
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(ImColor(227, 60, 54), " %zu", std::count_if(players.begin(), players.end(), [](const auto& pair) {
                return pair.second.impostor;
            }));
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(ImColor(255, 255, 255), "/");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(ImColor(47, 250, 250), "%zu", players.size());
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(ImColor(216, 255, 77), " v%s", r.get_game_version().c_str());
            ImGui::SameLine(0.0f, 0.0f);
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImColor(180, 180, 180).Value);
            ImGui::Selectable(fmt::format("{} {}", ICON_FA_FILE, entry.display_name).c_str());
            ImGui::PopStyleColor(1);
            ImGui::NextColumn();
        }

        ImGui::NextColumn();
    }

    ImGui::Columns();

    if (hovered_replay) {
        ImGui::BeginTooltip();
        ImGui::Text("Map: %s", get_map_name(hovered_replay->get_map_id()));
        ImGui::Text("Game version: %s", hovered_replay->get_game_version().c_str());
        ImGui::Text("Mod version: %s", hovered_replay->get_mod_version().c_str());

        ImGui::Separator();

        for (const auto& [id, player] : hovered_replay->get_players()) {
            ImGui::ColorButton("##ColorButton", convert_color(player.color));
            ImGui::SameLine();
            ImGui::TextColored(player.impostor ? ImColor(0xFF, 0x33, 0x33) : ImColor(0xFF, 0xFF, 0xFF), "%s", player.name.c_str());
        }

        ImGui::EndTooltip();
    }

    ImGui::EndChild();

    ImGui::End();
}

bool file_dialog::try_set_path(std::filesystem::path path) {
    std::error_code err;
    if (path = fs::canonical(path, err); err != std::error_code{}) {
        return false;
    }

    if (fs::directory_iterator(path, err); err != std::error_code{}) {
        return false;
    }

    m_current_path = std::move(path);
    m_time_to_cache_refresh = 0;
    return true;
}

void file_dialog::refresh_cache() {
    m_cache.clear();

    if (m_current_path.has_parent_path()) {
        m_cache.push_back(file_entry{ true, "..", m_current_path.parent_path(), std::nullopt });
    }

    for (const auto& dir_entry : fs::directory_iterator{ m_current_path }) {
        const auto& entry_path = dir_entry.path();

        std::optional<replay> replay_data;
        if (!dir_entry.is_directory()) {
            try {
                replay r;
                std::fstream ifs(dir_entry.path(), std::fstream::binary | std::fstream::in);
                ifs.exceptions(std::fstream::eofbit | std::fstream::failbit | std::fstream::badbit);
                r.parse(ifs, true);
                replay_data = std::move(r);
            }
            catch (...) {
                // nothing
            }
        }

        m_cache.push_back(file_entry{
            dir_entry.is_directory(),
            entry_path.filename().generic_u8string(),
            entry_path,
            std::move(replay_data)
        });
    }

    std::sort(m_cache.begin(), m_cache.end(), [](const auto& left, const auto& right) {
        if (left.is_directory != right.is_directory) {
            return left.is_directory;
        }

        return left.display_name < right.display_name;
    });
}

} // namespace ui

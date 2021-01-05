#include "file_dialog.hpp"

#include <resources/font.hpp>
#include <ui/utils.hpp>

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include <fstream>
#include <optional>

namespace fs = std::filesystem;

namespace {

bool starts_with(std::string_view string, std::string_view prefix) {
    return string.length() >= prefix.length() && string.substr(0, prefix.length()) == prefix;
}

template <class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

} // unnamed namespace

namespace ui {

file_dialog::file_dialog(std::function<void(const std::filesystem::path&)> open_file_callback)
    : m_open_file_callback(std::move(open_file_callback))
{
    if (!try_set_path(fs::current_path()) && !try_set_path(fs::temp_directory_path())) {
        throw std::runtime_error("Unable to set default path");
    }

    add_settings_handler();
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

    ImGui::Begin("Choose Replay File", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

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

    for (const file_entry& entry : m_cache) {
        std::visit(overloaded{
            [&](const file_entry::dir_data&) {
                if (ImGui::Selectable(fmt::format("{} {}", ICON_FA_FOLDER_OPEN, entry.display_name).c_str())) {
                    try_set_path(entry.path);
                }
                ImGui::NextColumn();
            },
            [&](const file_entry::replay_data& data) {
                const auto& r = data.r;
                const auto& players = r.get_players();

                ImGui::PushStyleColor(ImGuiCol_Text, ImColor(153, 255, 128).Value);
                if (ImGui::Selectable(fmt::format("{} {}", ICON_FA_FILE_CODE, entry.display_name.c_str()).c_str())) {
                    m_open_file_callback(entry.path);
                }
                ImGui::PopStyleColor(1);
                ImGui::NextColumn();

                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Map: %s", utils::get_map_name(r.get_map_id()));
                    ImGui::Text("Game version: %s", r.get_game_version().c_str());
                    ImGui::Text("Mod version: %s", r.get_mod_version().c_str());

                    ImGui::Separator();

                    for (const auto& [id, player] : r.get_players()) {
                        ImGui::ColorButton("##ColorButton", utils::convert_color(player.color));
                        ImGui::SameLine();
                        ImGui::TextColored(player.impostor ? ImColor(0xFF, 0x33, 0x33) : ImColor(0xFF, 0xFF, 0xFF), "%s", player.name.c_str());
                    }

                    ImGui::EndTooltip();
                }

                ImGui::TextColored(ImColor(43, 255, 54), "%s", utils::get_map_name(r.get_map_id()));
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
            },
            [&](const file_entry::unknown_data& data) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImColor(180, 180, 180).Value);
                ImGui::Selectable(fmt::format("{} {}", ICON_FA_FILE, entry.display_name).c_str());
                ImGui::PopStyleColor(1);
                ImGui::NextColumn();
            }
        }, entry.parse_result);

        ImGui::NextColumn();
    }

    ImGui::Columns();

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
        m_cache.push_back(file_entry{ "..", m_current_path.parent_path(), file_entry::dir_data{} });
    }

    for (const auto& dir_entry : fs::directory_iterator{ m_current_path }) {
        auto parse_result = [&]() -> decltype(file_entry::parse_result) {
            if (dir_entry.is_directory()) {
                return file_entry::dir_data{};
            }

            try {
                replay r;
                std::fstream ifs(dir_entry.path(), std::fstream::binary | std::fstream::in);
                ifs.exceptions(std::fstream::eofbit | std::fstream::failbit | std::fstream::badbit);
                r.parse(ifs, true);
                return file_entry::replay_data{ std::move(r) };
            }
            catch (std::exception& e) {
                return file_entry::unknown_data{ e.what() };
            }
            catch (...) {
                return file_entry::unknown_data{ "???" };
            }
        }();

        const auto& entry_path = dir_entry.path();

        m_cache.push_back(file_entry{
            entry_path.filename().generic_u8string(),
            entry_path,
            std::move(parse_result)
        });
    }

    std::sort(m_cache.begin(), m_cache.end(), [](const auto& left, const auto& right) {
        if (left.is_directory() != right.is_directory()) {
            return left.is_directory();
        }

        return left.display_name < right.display_name;
    });
}

void file_dialog::add_settings_handler() {
    ImGuiSettingsHandler ini_handler;

    ini_handler.TypeName = "FileDialog";
    ini_handler.TypeHash = ImHashStr("FileDialog");
    ini_handler.UserData = this; // TODO:  lifetime

    ini_handler.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler*, const char* name) -> void* {
        static int dummy_var;
        return &dummy_var;
    };

    ini_handler.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler* handler, void* entry, const char* line) {
        auto self = static_cast<file_dialog*>(handler->UserData);

        auto try_extract = [line](std::string_view prefix) -> std::optional<std::string_view> {
            if (starts_with(line, prefix)) {
                return line + prefix.length();
            }
            return std::nullopt;
        };

        if (auto saved_path = try_extract("path=")) {
            self->try_set_path(std::filesystem::path{ *saved_path, std::filesystem::path::format::generic_format });
        }
    };

    ini_handler.WriteAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf) {
        auto self = static_cast<file_dialog*>(handler->UserData);

        out_buf->appendf("[%s][Settings]\n", handler->TypeName);
        out_buf->appendf("path=%s\n", self->m_current_path.generic_u8string().c_str());
    };

    ImGui::GetCurrentContext()->SettingsHandlers.push_back(ini_handler);
}

} // namespace ui

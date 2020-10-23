#include "player.hpp"

#include <replay/replay.hpp>
#include <resources/font.hpp>

#include <imgui.h>

namespace ui {

player::player(sf::RenderWindow& window, const replay& replay) : m_window(window), m_replay(replay) {
    m_time     = 0;
    m_time_min = 0;
    m_time_max = m_replay.get_duration();
}

void player::update(int dt) {
    if (!m_paused) {
        set_time(m_time + static_cast<int>(dt * m_speed));
    }
}

void player::render() {
    const float FIRST_COLUMN_WIDTH    = 150.0f;
    const float SECOND_COLUMN_WIDTH   = 200.0f;
    const float EXPECTED_WINDOW_WIDTH = FIRST_COLUMN_WIDTH + SECOND_COLUMN_WIDTH + 20.0f;

    ImGui::SetNextWindowPos({ m_window.getSize().x / 2.0f - EXPECTED_WINDOW_WIDTH / 2.0f, 200.f }, ImGuiCond_FirstUseEver);

    ImGui::Begin("Player", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);

    const auto   BUTTON_SIZE = ImVec2(25, 25);
    const ImVec4 COLOR_WHITE = ImColor(0xFF, 0xFF, 0xFF);
    const ImVec4 COLOR_RED   = ImColor(0xFF, 0x33, 0x33);

    if (ImGui::Button(m_paused ? ICON_FA_PLAY : ICON_FA_PAUSE, BUTTON_SIZE)) {
        m_paused = !m_paused;
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_REDO, BUTTON_SIZE)) {
        m_paused = false;
        set_time(0);
    }
    ImGui::SameLine();
    ImGui::SliderInt("##Frame", &m_time, m_time_min, m_time_max);

    ImGui::SliderFloat("Speed", &m_speed, 0.0f, 30.0f, "%.2f", 2.f);

    ImGui::Separator();

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, FIRST_COLUMN_WIDTH);
    ImGui::SetColumnWidth(1, SECOND_COLUMN_WIDTH);

    std::optional<std::uint8_t> selected_player_id = m_traced_player_id;
    for (const auto& [id, player] : m_replay.get_players()) {
        auto color = player.impostor ? COLOR_RED : COLOR_WHITE;
        if (player.get_interpolated(m_time).is_dead) {
            color.w *= 0.3;
        }
        ImGui::PushStyleColor(ImGuiCol_Text, color);

        if (ImGui::Selectable(player.name.c_str(), id == m_traced_player_id)) {
            m_traced_player_id = id;
        }

        if (ImGui::IsItemHovered()) {
            selected_player_id = id;
        }

        ImGui::PopStyleColor();
    }

    ImGui::NextColumn();

    if (selected_player_id) {
        auto& player = m_replay.get_players().at(*selected_player_id);

        ImGui::Text("ID: %d", static_cast<int>(player.id));
        ImGui::Text("Name: %s", player.name.c_str());
        ImGui::Text("Color: %d", static_cast<int>(player.color));
        ImGui::Text("Hat: %u", player.hat);
        ImGui::Text("Pet: %u", player.pet);
        ImGui::Text("Skin: %u", player.skin);
        ImGui::Text("Impostor: %s", player.impostor ? "yes" : "no");

        const auto state = player.get_interpolated(m_time);
        ImGui::Text("Position: %.2f, %.2f", state.position[0], state.position[1]);
        ImGui::Text("Speed: %.2f, %.2f", state.velocity[0], state.velocity[1]);
        ImGui::Text("Is dead: %s", state.is_dead ? "yes" : "no");
    }
    else {
        ImGui::Text("Select player to trace");
        for (std::size_t i = 1; i < 10; i++) {
            ImGui::Text(" ");
        }
    }

    ImGui::Columns();

    ImGui::End();
}

int player::get_time() const {
    return m_time;
}

void player::set_time(int t) {
    m_time = std::clamp(t, m_time_min, m_time_max);
}

std::optional<std::uint8_t> player::get_traced_player_id() const {
    return m_traced_player_id;
}

void player::reset_traced_player() {
    m_traced_player_id.reset();
}

} // namespace ui

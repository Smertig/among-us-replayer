#include "player.hpp"

#include <replay/replay.hpp>
#include <resources/font.hpp>

#include <imgui.h>

namespace ui {

player::player(sf::RenderWindow& window, const replay& replay) : m_window(window) {
    m_time     = 0;
    m_time_min = 0;
    m_time_max = replay.get_duration();
}

void player::update(int dt) {
    if (!m_paused) {
        set_time(m_time + static_cast<int>(dt * m_speed));
    }
}

void player::render() {
    ImGui::Begin("Player", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    const auto button_size = ImVec2(25, 25);

    if (ImGui::Button(m_paused ? ICON_FA_PLAY : ICON_FA_PAUSE, button_size)) {
        m_paused = !m_paused;
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_REDO, button_size)) {
        m_paused = false;
        set_time(0);
    }
    ImGui::SameLine();
    ImGui::SliderInt("##Frame", &m_time, m_time_min, m_time_max);

    ImGui::SliderFloat("Speed", &m_speed, 0.0f, 10.0f);

    ImGui::SetWindowPos({ m_window.getSize().x / 2.0f - ImGui::GetWindowSize().x / 2.0f, m_window.getSize().y / 4.0f });

    ImGui::End();
}

int player::get_time() const {
    return m_time;
}

void player::set_time(int t) {
    m_time = std::clamp(t, m_time_min, m_time_max);
}

} // namespace ui

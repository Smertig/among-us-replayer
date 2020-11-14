#include "platform.hpp"
#include "app_info.hpp"

#include <fmt/format.h>

#include <filesystem>
#include <array>

#include <Windows.h>

namespace platform {

std::string get_app_fullname() {
    return fmt::format("{} v{}", app_info::name, app_info::version);
}

void msgbox_warning(std::string_view text) {
    MessageBoxA(nullptr, std::string(text).c_str(), get_app_fullname().c_str(), MB_ICONWARNING);
}

void fix_working_directory() {
    std::array<wchar_t, MAX_PATH> path = { 0 };
    GetModuleFileNameW(nullptr, path.data(), path.size());

    std::filesystem::current_path(std::filesystem::path(path.data()).parent_path());
}

} // namespace platform

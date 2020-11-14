#include "platform.hpp"
#include "app_info.hpp"

#include <fmt/format.h>

#include <filesystem>

// Platform specific stuff

#if defined(_WIN32)

#include <Windows.h>
#include <array>

void show_warning_impl(std::string_view text) {
    MessageBoxA(nullptr, std::string(text).c_str(), platform::get_app_fullname().c_str(), MB_ICONWARNING);
}

std::filesystem::path get_self_path_impl() {
    std::array<wchar_t, MAX_PATH> path = {};
    GetModuleFileNameW(nullptr, path.data(), path.size());

    return path.data();
}

#else

static_assert(false, "unsupported platform");

#endif // defined(_WIN32)

namespace {

void fix_working_directory() {
    const auto self_path = get_self_path_impl();

    std::filesystem::current_path(self_path.parent_path());
}

} // unnamed namespace

namespace platform {

std::string get_app_fullname() {
    return fmt::format("{} v{}", app_info::name, app_info::version);
}

void show_warning(std::string_view text) {
    show_warning_impl(text);
}

void on_app_start() {
    // Try fix paths
    if (!std::filesystem::exists("res/config.json")) {
        fix_working_directory();
    }
}

} // namespace platform

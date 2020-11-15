#include "platform.hpp"
#include "app_info.hpp"

#include <fmt/format.h>

#include <filesystem>

// Platform specific stuff

#if defined(_WIN32)

#include <Windows.h>
#include <array>

void prepare_render_impl() {
    // nothing
}

void show_warning_impl(std::string_view text) {
    MessageBoxA(nullptr, std::string(text).c_str(), platform::get_app_fullname().c_str(), MB_ICONWARNING);
}

std::filesystem::path get_self_path_impl() {
    std::array<wchar_t, MAX_PATH> path = {};
    GetModuleFileNameW(nullptr, path.data(), path.size());

    return path.data();
}

#elif defined(__linux__)

#include <fmt/color.h>

#include <X11/Xlib.h>

void prepare_render_impl() {
    XInitThreads();
}

void show_warning_impl(std::string_view text) {
    fmt::print(fg(fmt::color::orange_red), "{}", text);
    fmt::print("\n");
}

std::filesystem::path get_self_path_impl() {
    return std::filesystem::read_symlink("/proc/self/exe");
}

#else

static_assert(false, "unsupported platform");

#endif

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
    prepare_render_impl();

    // Try fix paths
    if (!std::filesystem::exists("res/config.json")) {
        fix_working_directory();
    }
}

} // namespace platform

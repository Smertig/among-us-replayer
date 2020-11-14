#include "platform.hpp"
#include "app_info.hpp"

#include <fmt/format.h>

#include <filesystem>
#include <array>

#if defined(_WIN32)

#include <Windows.h>

void show_warning_impl(std::string_view text) {
    MessageBoxA(nullptr, std::string(text).c_str(), platform::get_app_fullname().c_str(), MB_ICONWARNING);
}

void fix_working_directory_impl() {
    std::array<wchar_t, MAX_PATH> path = {};
    GetModuleFileNameW(nullptr, path.data(), path.size());

    std::filesystem::current_path(std::filesystem::path(path.data()).parent_path());
}

#else

static_assert(false, "unsupported platform");

#endif // defined(_WIN32)

namespace platform {

std::string get_app_fullname() {
    return fmt::format("{} v{}", app_info::name, app_info::version);
}

void show_warning(std::string_view text) {
    show_warning_impl(text);
}

void fix_working_directory() {
    fix_working_directory_impl();
}

} // namespace platform

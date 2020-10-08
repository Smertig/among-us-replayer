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

bool msgbox_ask(std::string_view text) {
    const auto result = MessageBoxA(nullptr, std::string(text).c_str(), get_app_fullname().c_str(), MB_YESNO | MB_ICONINFORMATION);

    return result == IDYES;
}

void fix_working_directory() {
    std::array<wchar_t, MAX_PATH> path = { 0 };
    GetModuleFileNameW(nullptr, path.data(), path.size());

    std::filesystem::current_path(std::filesystem::path(path.data()).parent_path());
}

std::optional<std::string> choose_replay_file() {
    std::array<char, MAX_PATH> filename{};

    ::OPENFILENAME ofn{};

    ofn.lStructSize  = sizeof(ofn);
    ofn.hwndOwner    = nullptr;
    ofn.lpstrFilter  = "Among Us Replay Data (*.aurp)\0*.aurp\0Any File (*.*)\0*.*\0";
    ofn.lpstrFile    = filename.data();
    ofn.nMaxFile     = filename.size();
    ofn.lpstrTitle   = "Choose replay data";
    ofn.Flags        = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if (!GetOpenFileNameA(&ofn))
        return {};

    return filename.data();
}

} // namespace platform

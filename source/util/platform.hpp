#pragma once

#include <string_view>
#include <string>
#include <optional>

namespace platform {

std::string get_app_fullname();

void msgbox_warning(std::string_view text);

bool msgbox_ask(std::string_view text);

void fix_working_directory();

std::optional<std::string> choose_replay_file();

} // namespace platform

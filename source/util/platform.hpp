#pragma once

#include <string_view>
#include <string>
#include <optional>

namespace platform {

std::string get_app_fullname();

void msgbox_warning(std::string_view text);

void fix_working_directory();

} // namespace platform

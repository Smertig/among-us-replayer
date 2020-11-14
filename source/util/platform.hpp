#pragma once

#include <string_view>
#include <string>

namespace platform {

std::string get_app_fullname();

void show_warning(std::string_view text);

void on_app_start();

} // namespace platform

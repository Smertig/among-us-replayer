#include <app.hpp>
#include <util/platform.hpp>

#include <fmt/format.h>

#include <optional>
#include <filesystem>

int main(int argc, char** argv) {
    try {
        // Try fix paths
        if (!std::filesystem::exists("res/config.json")) {
            platform::fix_working_directory();
        }

        app{ platform::get_app_fullname() }.run();

        return 0;
    }
    catch (std::exception& e) {
        platform::msgbox_warning(fmt::format("ERROR:\n{}", e.what()));
        throw;
    }
}

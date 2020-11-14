#include <app.hpp>
#include <util/platform.hpp>

#include <fmt/format.h>

int main(int argc, char** argv) {
    try {
        platform::on_app_start();

        app{ platform::get_app_fullname() }.run();

        return 0;
    }
    catch (std::exception& e) {
        platform::show_warning(fmt::format("ERROR:\n{}", e.what()));
        throw;
    }
}

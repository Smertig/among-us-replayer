from conans import ConanFile, CMake


class AmongUsReplayerConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    requires = [
        "sfml/2.5.1",
        "imgui-sfml/2.1@bincrafters/stable",
        "fmt/7.0.3",
        "nlohmann_json/3.9.1"
    ]

    generators = "cmake"

    def configure(self):
        self.options["sfml"].audio = False
        self.options["sfml"].network = False
        self.options["nlohmann_json"].implicit_conversions = False
        self.options["freetype"].with_bzip2 = False

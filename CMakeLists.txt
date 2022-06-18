cmake_minimum_required(VERSION 3.15)
project(among-us-replayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # CMake 3.15+

# conan
include(cmake/conan.cmake)

conan_check()
conan_add_remote(NAME conan-center URL https://center.conan.io)
conan_add_remote(NAME bincrafters URL https://bincrafters.jfrog.io/artifactory/api/conan/conan)

if(MSVC)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(REPLAYER_CONAN_SETTINGS "compiler.runtime=MTd")
    else()
        set(REPLAYER_CONAN_SETTINGS "compiler.runtime=MT")
    endif()
endif()

conan_cmake_run(
        CONANFILE conanfile.py
        BASIC_SETUP CMAKE_TARGETS
        BUILD missing
        SETTINGS "${REPLAYER_CONAN_SETTINGS}"
)

add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCE_DIR}/source/main.cpp
        ${PROJECT_SOURCE_DIR}/source/app.cpp
        ${PROJECT_SOURCE_DIR}/source/scene/map.cpp
        ${PROJECT_SOURCE_DIR}/source/scene/player.cpp
        ${PROJECT_SOURCE_DIR}/source/scene/camera.cpp
        ${PROJECT_SOURCE_DIR}/source/resources/config.cpp
        ${PROJECT_SOURCE_DIR}/source/resources/textured_sprite.cpp
        ${PROJECT_SOURCE_DIR}/source/resources/font.cpp
        ${PROJECT_SOURCE_DIR}/source/resources/cache.cpp
        ${PROJECT_SOURCE_DIR}/source/ui/utils.cpp
        ${PROJECT_SOURCE_DIR}/source/ui/player.cpp
        ${PROJECT_SOURCE_DIR}/source/ui/file_dialog.cpp
        ${PROJECT_SOURCE_DIR}/source/replay/replay.cpp
        ${PROJECT_SOURCE_DIR}/source/util/platform.cpp
        )

target_link_libraries(${PROJECT_NAME} PRIVATE ${CONAN_TARGETS})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/source)
cmake_minimum_required(VERSION 3.15)
project(among-us-replayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # CMake 3.15+

# conan
include(cmake/conan.cmake)

conan_check()
conan_add_remote(NAME conan-center URL https://conan.bintray.com)
conan_add_remote(NAME bincrafters URL https://api.bintray.com/conan/bincrafters/public-conan)

if(MSVC)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        list(APPEND REPLAYER_CONAN_SETTINGS SETTINGS compiler.runtime=MTd)
    else()
        list(APPEND REPLAYER_CONAN_SETTINGS SETTINGS compiler.runtime=MT)
    endif()
endif()

if (REPLAYER_CONAN_ARCH)
    list(APPEND REPLAYER_CONAN_SETTINGS SETTINGS arch=${REPLAYER_CONAN_ARCH})
endif()

conan_cmake_run(
        CONANFILE conanfile.py
        BASIC_SETUP CMAKE_TARGETS
        BUILD missing
        ${REPLAYER_CONAN_SETTINGS}
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
        ${PROJECT_SOURCE_DIR}/source/ui/player.cpp
        ${PROJECT_SOURCE_DIR}/source/ui/file_dialog.cpp
        ${PROJECT_SOURCE_DIR}/source/replay/replay.cpp
        ${PROJECT_SOURCE_DIR}/source/util/platform.cpp
        )

target_link_libraries(${PROJECT_NAME} PRIVATE ${CONAN_TARGETS})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/source)
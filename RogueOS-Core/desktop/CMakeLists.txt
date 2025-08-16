cmake_minimum_required(VERSION 3.16)
project(RogueOSDesktop)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

add_executable(rogueos_desktop
    ${SRC_DIR}/main_desktop.cpp
    ${SRC_DIR}/modules/command_router.cpp
    ${SRC_DIR}/cli/help.cpp
)

find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LVGL REQUIRED lvgl)

include_directories(${SDL2_INCLUDE_DIRS} ${LVGL_INCLUDE_DIRS} ${SRC_DIR} ${SRC_DIR}/modules ${SRC_DIR}/cli)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED COMPONENTS system)

add_library(boost_beast INTERFACE)
target_include_directories(boost_beast INTERFACE /usr/include)

add_custom_target(run_iso
    COMMAND lb build
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/live-build
)

target_link_libraries(rogueos_desktop
    ${SDL2_LIBRARIES}
    ${LVGL_LIBRARIES}
    Boost::system
    Threads::Threads
)

cmake_minimum_required(VERSION 3.16)
set(IDF_TARGET "esp32")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(RogueOSESP32)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

set(COMPONENT_SRCS
    ${SRC_DIR}/main_esp32.cpp
    ${SRC_DIR}/modules/command_router.cpp
    ${SRC_DIR}/modules/serial_cli.cpp
    ${SRC_DIR}/cli/help.cpp
)

set(COMPONENT_ADD_INCLUDEDIRS
    ${SRC_DIR}
    ${SRC_DIR}/modules
    ${SRC_DIR}/cli
)

idf_component_register(SRCS ${COMPONENT_SRCS} INCLUDE_DIRS ${COMPONENT_ADD_INCLUDEDIRS})

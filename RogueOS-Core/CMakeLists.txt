cmake_minimum_required(VERSION 3.16)
project(RogueOSCore)

set(PROFILE "desktop" CACHE STRING "Build profile: desktop or esp32")
message(STATUS "Selected build profile: ${PROFILE}")

if(PROFILE STREQUAL "desktop")
    add_subdirectory(desktop)
elseif(PROFILE STREQUAL "esp32")
    add_subdirectory(esp32)
else()
    message(FATAL_ERROR "Unknown build PROFILE: ${PROFILE}")
endif()

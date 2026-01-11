cmake_minimum_required(VERSION 3.16)

project(DataCenterSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set Qt path if not found automatically
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "D:/qt/6.10.1/mingw_64")
endif()

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Include directories - adjusted for your structure
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/gui
)

# Core library sources (your existing code - directly in src/)
set(CORE_SOURCES
    src/logger.cpp
    src/resource_manager.cpp
    src/deadlock.cpp
    src/job.cpp
    src/scheduler.cpp
)

set(CORE_HEADERS
    include/logger.hpp
    include/resource_manager.hpp
    include/deadlock.hpp
    include/job.hpp
    include/scheduler.hpp
)

# GUI sources
set(GUI_SOURCES
    src/gui/main_gui.cpp
    src/gui/mainwindow.cpp
)

set(GUI_HEADERS
    include/gui/mainwindow.h
)

# Create GUI executable
add_executable(datacenter_gui
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${GUI_SOURCES}
    ${GUI_HEADERS}
)

# Link Qt libraries
target_link_libraries(datacenter_gui
    Qt6::Core
    Qt6::Widgets
)

# Windows specific settings
if(WIN32)
    set_target_properties(datacenter_gui PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Console version (optional - your original program)
add_executable(datacenter_console
    src/main.cpp
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

# Output directories
set_target_properties(datacenter_gui datacenter_console PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS datacenter_gui datacenter_console
    RUNTIME DESTINATION bin
)

message(STATUS "Qt version: ${Qt6_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")

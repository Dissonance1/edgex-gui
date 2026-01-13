cmake_minimum_required(VERSION 3.16)

project(edgex-qt-ui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find Qt
find_package(Qt6 COMPONENTS Widgets Network REQUIRED)
if (Qt6_FOUND)
    set(QT_LIBRARIES Qt6::Widgets Qt6::Network)
else()
    find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
    set(QT_LIBRARIES Qt5::Widgets Qt5::Network)
endif()

# Include the src directory
include_directories(${CMAKE_SOURCE_DIR}/src)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE UI_FILES "src/*.ui")
set(RESOURCES "src/resources.qrc")

# Create executable
add_executable(edgex-qt-ui ${SOURCES} ${HEADERS} ${UI_FILES} ${RESOURCES})

# Link Qt libraries
target_link_libraries(edgex-qt-ui ${QT_LIBRARIES})

# Set output directory
set_target_properties(edgex-qt-ui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# --- Packaging and Installation ---

# Install rules
install(TARGETS edgex-qt-ui DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/resources DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/ModernTheme.qss DESTINATION bin)

# CPack configuration for DEB
set(CPACK_PACKAGE_NAME "edgex-gui")
set(CPACK_PACKAGE_VENDOR "EdgeX Foundry")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern GUI for EdgeX Foundry")
set(CPACK_PACKAGE_CONTACT "support@example.com")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "EdgeX Foundry <support@example.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6 (>= 6.2.4), libqt6network6 (>= 6.2.4), libqt6core6 (>= 6.2.4), libc6 (>= 2.34), libstdc++6 (>= 11)")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

# Set architecture for cross-compilation
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
endif()

# Specify the generator
set(CPACK_GENERATOR "DEB")

include(CPack)

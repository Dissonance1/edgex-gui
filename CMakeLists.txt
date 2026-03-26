cmake_minimum_required(VERSION 3.16)

project(edgex-qt-ui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-enum-enum-conversion")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Configure RPATH for Aetina Metis
set(CMAKE_BUILD_RPATH "/data/voyager-sdk/operators/lib;/opt/axelera/runtime-1.5.2-1/lib")
set(CMAKE_INSTALL_RPATH "/data/voyager-sdk/operators/lib;/opt/axelera/runtime-1.5.2-1/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Force DT_RPATH instead of DT_RUNPATH for transitive dependency resolution
if(UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")
endif()

# Find Qt
find_package(Qt6 COMPONENTS Widgets Network Multimedia MultimediaWidgets QUIET)
if (Qt6_FOUND)
    set(QT_LIBRARIES Qt6::Widgets Qt6::Network Qt6::Multimedia Qt6::MultimediaWidgets)
    message(STATUS "Using Qt6")
else()
    find_package(Qt5 COMPONENTS Widgets Network Multimedia MultimediaWidgets REQUIRED)
    set(QT_LIBRARIES Qt5::Widgets Qt5::Network Qt5::Multimedia Qt5::MultimediaWidgets)
    message(STATUS "Using Qt5")
endif()

# Find PkgConfig for OpenCV and Axelera SDK dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(axruntime axruntime)
pkg_check_modules(axstreamer axstreamer)
pkg_check_modules(opencv4 REQUIRED opencv4)
pkg_check_modules(gstreamer REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)



# Set up include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${axruntime_INCLUDE_DIRS}/include
    ${opencv4_INCLUDE_DIRS}
    ${axstreamer_INCLUDE_DIRS}
    ${gstreamer_INCLUDE_DIRS}
)


link_directories(
    ${axruntime_LIBRARY_DIRS}
    ${opencv4_LIBRARY_DIRS}
    ${axstreamer_LIBRARY_DIRS}
)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE UI_FILES "src/*.ui")

# Ensure AI Runtime files are explicitly included to avoid GLOB issues with AUTOMOC
set(AI_RUNTIME_FILES
    src/AIRuntimeView.cpp
    src/AIRuntimeView.h
    src/AIRuntimeView.ui
    src/VideoFrameWidget.cpp
    src/VideoFrameWidget.h
    src/LiveMonitoringWidget.cpp
    src/LiveMonitoringWidget.h
    src/AIInferenceWorker.cpp
    src/AIInferenceWorker.h
)

set(RESOURCES "src/resources.qrc")

# Create executable
add_executable(edgex-qt-ui ${SOURCES} ${HEADERS} ${UI_FILES} ${AI_RUNTIME_FILES} ${RESOURCES})

# Link compiled libraries
target_link_libraries(edgex-qt-ui 
    ${QT_LIBRARIES}
    ${axruntime_LIBRARIES}
    ${opencv4_LIBRARIES}
    ${axstreamer_LIBRARIES}
    ${gstreamer_LIBRARIES}
)


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

# Set architecture for cross-compilation
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
endif()

# Specify the generator
set(CPACK_GENERATOR "DEB")

include(CPack)

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
# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE UI_FILES "src/*.ui")
set(RESOURCES "src/resources.qrc")

# Copy Resources and Theme to Build Directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/ModernTheme.qss ${CMAKE_CURRENT_BINARY_DIR}/ModernTheme.qss COPYONLY)

# Create executable
add_executable(edgex-qt-ui ${SOURCES} ${HEADERS} ${UI_FILES} ${RESOURCES})

# Link Qt libraries
target_link_libraries(edgex-qt-ui ${QT_LIBRARIES})

# Set output directory
set_target_properties(edgex-qt-ui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

cmake_minimum_required(VERSION 3.17.0)
project(CHIP8-Emu CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# Prevent In Source Builds
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-Source Builds are not recommended. Please Create a build/ directory and run CMake specifying Build Directory." )
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Specify Stack Size
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,/stack:8388608")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,8388608")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:8388608")
endif()


# Source Files
set(SOURCES 
    main.cc
    app.cc 
    chip8.cc 
    shader_utils.cc
    gui/mainWindow.cc
    gui/mainWindow.ui
)

add_executable( chip8emu ${SOURCES} )

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)

# Libraries :
if(WIN32)
    set(LIBRARY Qt5::Widgets GLEW::GLEW OpenGL::GL glfw gdi32 winmm SDL2::SDL2)
elseif(UNIX)
    set(LIBRARY Qt5::Widgets GLEW::GLEW OpenGL::GL glfw pthread Xi X11 dl SDL2::SDL2)
endif()

target_link_libraries( chip8emu ${LIBRARY} )

# Copy ROM and shaders to build directory
file(COPY rom shaders DESTINATION ${CMAKE_BINARY_DIR})
cmake_minimum_required(VERSION 3.11)
project(opengl-on-wayland)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
find_package(OpenGL REQUIRED)

add_executable(main main.c)
target_link_libraries(main ${SDL_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
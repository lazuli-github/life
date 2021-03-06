cmake_minimum_required(VERSION 3.17)
project(life C)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED SDL2)

add_executable(life main.c graphics.c graphics.h glad/glad.c glad/glad.h universe.c universe.h)
target_include_directories(life PRIVATE .)
target_link_libraries(life PRIVATE SDL2 ${CMAKE_DL_LIBS})
file(COPY vshader.glsl DESTINATION ${CMAKE_BINARY_DIR})
file(COPY fshader.glsl DESTINATION ${CMAKE_BINARY_DIR})

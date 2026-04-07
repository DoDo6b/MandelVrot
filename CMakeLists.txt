cmake_minimum_required(VERSION 3.15)
project(Mandelvrot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-O3 -mavx2)

set(RAYLIB_PATH "D:/DEVKIT/raylib/")
set(RAYLIB_LIBRARY "${RAYLIB_PATH}/libraylib.a")

add_library(raylib STATIC IMPORTED)
set_target_properties(raylib PROPERTIES
    IMPORTED_LOCATION "${RAYLIB_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${RAYLIB_PATH}"
)

add_executable(Mandelvrot 
    src/main.cpp 
    src/mandelvrot/src/stupid.cpp
    src/mandelvrot/src/avx.cpp
)

target_link_libraries(Mandelvrot PRIVATE raylib)

target_link_libraries(Mandelvrot PRIVATE
    winmm
    opengl32
    gdi32
    user32
    shell32
)


target_compile_definitions(Mandelvrot PRIVATE)
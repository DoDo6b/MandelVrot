cmake_minimum_required(VERSION 3.15)
project(Mandelvrot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(GUI "Build with raylib graphical interface" OFF)

set(RAYLIB_PATH "D:/DEVKIT/raylib/")
set(RAYLIB_LIBRARY "${RAYLIB_PATH}/libraylib.a")

set(ALGORITHM "ADAPTIVE" CACHE STRING "Algorithm to use: ADAPTIVE, AVX, or STUPID")
set_property(CACHE ALGORITHM PROPERTY STRINGS "ADAPTIVE" "AVX" "STUPID")

add_compile_options(-O3 -march=znver5)

string(TOUPPER ${ALGORITHM} ALGORITHM_UPPER)
add_compile_options(-D${ALGORITHM_UPPER})

if(GUI)
    add_library(raylib STATIC IMPORTED)
    set_target_properties(raylib PROPERTIES
        IMPORTED_LOCATION "${RAYLIB_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${RAYLIB_PATH}"
    )

    set(DRIVER "src/gui.cpp")
else()
    add_compile_options(-DNOGUI)
    set(DRIVER "src/benchmark.cpp")
endif()
add_executable(Mandelvrot 
    ${DRIVER}
    src/mandelvrot/src/stupid.cpp
    src/mandelvrot/src/avx.cpp
    src/mandelvrot/src/adaptive.cpp
)
if(GUI)
    target_link_libraries(Mandelvrot PRIVATE raylib)

    target_link_libraries(Mandelvrot PRIVATE
        winmm
        opengl32
        gdi32
        user32
        shell32
    )


    target_compile_definitions(Mandelvrot PRIVATE)
endif()
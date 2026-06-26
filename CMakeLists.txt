cmake_minimum_required(VERSION 3.12)
project(phys C) 
set(CMAKE_C_STANDARD 11)

file(GLOB SOURCES "src/*.c")
add_executable(phys ${SOURCES})
target_include_directories(phys PRIVATE 
    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include"
)
target_link_libraries(phys PRIVATE raylib m)

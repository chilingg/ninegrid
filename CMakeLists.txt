cmake_minimum_required(VERSION 3.1)

project(Ninegrid)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DROOT_DIR="${PROJECT_SOURCE_DIR}/")

find_file(REDOPERA redopera/CMakeLists.txt .)
if(REDOPERA)
    include_directories(redopera/include/redopera)
    link_directories(${CMAKE_BINARY_DIR}/redopera/src)
    add_subdirectory(redopera redopera)
endif()

add_subdirectory(src src)

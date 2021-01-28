cmake_minimum_required(VERSION 3.1)

aux_source_directory(. NINEGRID_SRC)

find_file(NINE_TEST test.cpp .)
if(NINE_TEST)
    list(REMOVE_ITEM NINEGRID_SRC "./test.cpp")

    aux_source_directory(. TEST_NINEGRID_SRC)
    list(REMOVE_ITEM TEST_NINEGRID_SRC "./main.cpp")
    add_executable(test ${TEST_NINEGRID_SRC})
    target_link_libraries(test redopera)
endif()

add_executable(ninegrid ${NINEGRID_SRC})
target_link_libraries(ninegrid redopera)

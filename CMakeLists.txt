cmake_minimum_required(VERSION 3.17)

project(CollisionDestection
    VERSION 0.1
    DESCRIPTION "working collision detection hopefully"
    LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS unit_test_framework NO_MODULE)

file(GLOB APP_SOURCES src/lib.cpp)

add_library(collision_lib STATIC ${APP_SOURCES})
target_compile_options(collision_lib PUBLIC -Wall -Wextra -g -O0 -pedantic)
target_include_directories(collision_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)

add_executable(collision_detection src/main.cpp)
target_compile_options(collision_detection PUBLIC -Wall -Wextra -g -O0 -pedantic)
target_link_libraries(collision_detection PRIVATE collision_lib)

enable_testing()

file(GLOB TEST_SOURCES test/*.cpp)
add_executable(all_tests ${TEST_SOURCES})
target_compile_options(all_tests PUBLIC -Wall -Wextra -g -O0 -pedantic)
target_link_libraries(all_tests PRIVATE collision_lib Boost::unit_test_framework)

add_test(NAME all_tests COMMAND all_tests)

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project("Tests" LANGUAGES CXX)

set(TESTS_SRC
    common/Result.test.cpp
    common/UUID.test.cpp

    scene/Entity.test.cpp
    scene/Scene.test.cpp
    scene/Transform2D.test.cpp
    scene/RigidBody2D.test.cpp
)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/gtest ${CMAKE_CURRENT_BINARY_DIR}/gtest)


add_executable("Tests" ${TESTS_SRC})
target_include_directories("Tests"
    PRIVATE ${CMAKE_SOURCE_DIR}/engine/
    PRIVATE ${CMAKE_SOURCE_DIR}/third_party/gtest/googletest/include
    PRIVATE ${CMAKE_SOURCE_DIR}/third_party/glm/
)

target_link_libraries("Tests"
    PRIVATE ${ENGINE_LIBRARY_NAME}
    PRIVATE GTest::gtest_main
)


include(CTest)
add_test(NAME "Tests" COMMAND "Tests")


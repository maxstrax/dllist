cmake_minimum_required(VERSION 3.21.0)
project(dllist VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(CTest)
include(FetchContent)
enable_testing()

add_compile_definitions(DLLIST_MEMORY_EXPORTS _WINDLL)

file(GLOB SRC_Cs "src/*.c" "src/*.cpp" "include/c/*.h" "include/cpp/*.hpp")
add_library(${PROJECT_NAME} SHARED ${SRC_Cs})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SRCs "tests/*.cpp")

add_executable(${PROJECT_NAME}_tests ${TEST_SRCs})
set_property(TARGET ${PROJECT_NAME}_tests PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME}_tests PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)
add_dependencies(${PROJECT_NAME}_tests ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME} gtest_main)
target_include_directories(${PROJECT_NAME}_tests INTERFACE ${PROJECT_NAME} gtest_main)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_tests) 
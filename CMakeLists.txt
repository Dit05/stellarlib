cmake_minimum_required(VERSION 4.0)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_CXX_COMPILER clang++)
file(READ ${CMAKE_SOURCE_DIR}/compile_flags.txt CMAKE_CXX_FLAGS)
string(REPLACE \n \  CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG "-save-temps -Wno-gnu-line-marker")
set(CMAKE_CXX_FLAGS_RELEASE "-flto -march=native")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_CLANG_TIDY clang-tidy;-p;${CMAKE_BINARY_DIR}/)

project(stellarlib LANGUAGES CXX VERSION 0.1.0)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} ${SOURCES})

find_package(GTest OPTIONAL CONFIG)
if(TARGET GTest::gtest_main)
	enable_testing()
	file(GLOB_RECURSE TESTS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
	add_executable(tests ${TESTS})
	target_link_libraries(tests PRIVATE GTest::gtest_main ${PROJECT_NAME})
	include(GoogleTest)
	gtest_discover_tests(tests)
endif()

file(GLOB_RECURSE EXAMPLES ${CMAKE_SOURCE_DIR}/examples/*.cpp)
foreach(SOURCE ${EXAMPLES})
	get_filename_component(EXAMPLE ${SOURCE} NAME_WLE)
	add_executable(${EXAMPLE} WIN32 ${SOURCE})
	target_link_libraries(${EXAMPLE} PRIVATE ${PROJECT_NAME})
	add_custom_command(
		TARGET ${EXAMPLE} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy -t
			$<TARGET_FILE_DIR:${EXAMPLE}>
			$<TARGET_RUNTIME_DLLS:${EXAMPLE}>
		COMMAND_EXPAND_LISTS
	)
endforeach()

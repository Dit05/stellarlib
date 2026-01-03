cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_EXTENSIONS OFF)
file(READ ${CMAKE_SOURCE_DIR}/compile_flags.txt CMAKE_CXX_FLAGS)
string(REPLACE \n \  CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS_RELEASE -march=native)

find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
	set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY};-p;${CMAKE_BINARY_DIR})
endif()

project(stellarlib LANGUAGES CXX)
include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} ${SOURCES})

if(${WIN32})
	file(
		COPY
		${CMAKE_FIND_ROOT_PATH}/bin/libgcc_s_seh-1.dll
		${CMAKE_FIND_ROOT_PATH}/bin/libssp-0.dll
		${CMAKE_FIND_ROOT_PATH}/bin/libstdc++-6.dll
		${CMAKE_FIND_ROOT_PATH}/bin/libwinpthread-1.dll
		DESTINATION
		${CMAKE_BINARY_DIR}
	)
endif()

find_package(GTest)
if(TARGET GTest::gtest_main)
	file(GLOB_RECURSE TESTS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
	add_executable(tests ${TESTS})
	target_link_libraries(tests GTest::gtest_main ${PROJECT_NAME})
	add_custom_command(
		TARGET tests POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:tests> $<TARGET_RUNTIME_DLLS:tests>
		COMMAND_EXPAND_LISTS
	)
endif()

file(GLOB_RECURSE EXAMPLES ${CMAKE_SOURCE_DIR}/examples/*.cpp)
foreach(SOURCE ${EXAMPLES})
	get_filename_component(EXAMPLE ${SOURCE} NAME_WLE)
	add_executable(${EXAMPLE} WIN32 ${SOURCE})
	target_link_libraries(${EXAMPLE} ${PROJECT_NAME})
	add_custom_command(
		TARGET ${EXAMPLE} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${EXAMPLE}> $<TARGET_RUNTIME_DLLS:${EXAMPLE}>
		COMMAND_EXPAND_LISTS
	)
endforeach()

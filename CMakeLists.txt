cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_EXTENSIONS OFF)
file(READ ${CMAKE_SOURCE_DIR}/compile_flags.txt CMAKE_CXX_FLAGS)
string(REPLACE \n \  CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
	set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY};-p;${CMAKE_BINARY_DIR})
endif()

project(stellarlib LANGUAGES CXX)
include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} STATIC ${SOURCES})
find_package(SDL3 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL3::SDL3)

include(GNUInstallDirs)
if(${WIN32})
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_FIND_ROOT_PATH}/${CMAKE_INSTALL_BINDIR}/libgcc_s_seh-1.dll
			${CMAKE_FIND_ROOT_PATH}/${CMAKE_INSTALL_BINDIR}/libssp-0.dll
			${CMAKE_FIND_ROOT_PATH}/${CMAKE_INSTALL_BINDIR}/libstdc++-6.dll
			${CMAKE_FIND_ROOT_PATH}/${CMAKE_INSTALL_BINDIR}/libwinpthread-1.dll
			$<TARGET_FILE_DIR:${PROJECT_NAME}>
		COMMAND_EXPAND_LISTS
	)
elseif(${ANDROID})
	set(ANDROID_JNI_DIR ${CMAKE_SOURCE_DIR}/android/app/src/main/jniLibs/${ANDROID_ABI})
	file(MAKE_DIRECTORY ${ANDROID_JNI_DIR})
	get_target_property(SDL3_LOCATION SDL3::SDL3 LOCATION)
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL3_LOCATION} ${ANDROID_JNI_DIR}
	)
endif()

find_package(GTest)
if(TARGET GTest::gtest_main)
	file(GLOB_RECURSE TESTS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
	add_executable(tests EXCLUDE_FROM_ALL ${TESTS})
	target_link_libraries(tests GTest::gtest_main ${PROJECT_NAME})
	if(${WIN32})
		add_custom_command(
			TARGET tests POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:tests> $<TARGET_RUNTIME_DLLS:tests>
			COMMAND_EXPAND_LISTS
		)
	endif()
endif()

file(GLOB_RECURSE EXAMPLES ${CMAKE_SOURCE_DIR}/examples/*.cpp)
foreach(SOURCE ${EXAMPLES})
	get_filename_component(EXAMPLE ${SOURCE} NAME_WLE)
	if(${ANDROID})
		add_library(${EXAMPLE} SHARED EXCLUDE_FROM_ALL ${SOURCE})
	else()
		add_executable(${EXAMPLE} WIN32 EXCLUDE_FROM_ALL ${SOURCE})
	endif()
	target_link_libraries(${EXAMPLE} ${PROJECT_NAME})
	if(${WIN32})
		add_custom_command(
			TARGET ${EXAMPLE} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${EXAMPLE}> $<TARGET_RUNTIME_DLLS:${EXAMPLE}>
			COMMAND_EXPAND_LISTS
		)
	elseif(${ANDROID})
		add_custom_command(
			TARGET ${EXAMPLE} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${EXAMPLE}> ${ANDROID_JNI_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}main${CMAKE_SHARED_LIBRARY_SUFFIX}
			COMMAND ./gradlew assembleDebug
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/android/app/build/outputs/apk/debug/app-debug.apk ${CMAKE_BINARY_DIR}/${EXAMPLE}.apk
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/android
			COMMAND_EXPAND_LISTS
		)
	endif()
endforeach()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS ${PROJECT_NAME})
install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/${PROJECT_NAME})

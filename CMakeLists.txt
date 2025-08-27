cmake_minimum_required(VERSION 4.1)
project(SteelBrick VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

option(OPTION_BUILD_TESTS "Whether to build tests" OFF)

find_package(SDL3 3.2 REQUIRED)
find_package(SDL3_image 3.2 REQUIRED)
find_package(SDL3_shadercross REQUIRED)
find_package(Lua 5.4 REQUIRED)

if(LUA_FOUND AND NOT TARGET Lua::Lua)
	add_library(Lua::Lua INTERFACE IMPORTED)
	target_include_directories(Lua::Lua INTERFACE ${LUA_INCLUDE_DIR})
	target_link_libraries(Lua::Lua INTERFACE ${LUA_LIBRARIES})
endif()

add_library(gamelib STATIC)
target_sources(gamelib
	PUBLIC FILE_SET HEADERS BASE_DIRS "source" FILES
		"source/luax.hpp"
		"source/debug.hpp"
		"source/thash.hpp"
		"source/hashmap.hpp"
		"source/program.hpp"
		"source/bindings.hpp"
		"source/bindings/shader.hpp"
		"source/bindings/pipeline.hpp"
		"source/bindings/commandbuffer.hpp"
	PRIVATE
		"source/luax.cpp"
		"source/program.cpp"
		"source/bindings.cpp"
		"source/bindings/shader.cpp"
		"source/bindings/pipeline.cpp"
		"source/bindings/commandbuffer.cpp"
)
target_link_libraries(gamelib PUBLIC
	SDL3::SDL3
	SDL3_image::SDL3_image
	SDL3_shadercross::SDL3_shadercross
	Lua::Lua
)

add_executable(game "source/main.cpp")
target_link_libraries(game PRIVATE SDL3::SDL3 gamelib)

if(OPTION_BUILD_TESTS)
	include(CTest)
	enable_testing()
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

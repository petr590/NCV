cmake_minimum_required(VERSION 3.3)
project(NewConsoleViewer VERSION 2.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SOURCE_DIR, src)

include_directories(include)

add_compile_options(
	-Wall
	-Wextra
	-Wpedantic

	-Wcast-align
	-Wcast-qual
	-Wenum-compare

	-Wctor-dtor-privacy
	-Wnon-virtual-dtor
	-Woverloaded-virtual
	-Wredundant-decls
	-Wsign-promo
	
	-Wduplicated-branches
	-Wduplicated-cond
	-Wshadow=compatible-local
	-Wzero-as-null-pointer-constant
#	-Wconversion
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	message("Debug build")
	
	add_compile_options(-g)
	add_compile_definitions(DEBUG)

elseif(CMAKE_BUILD_TYPE MATCHES "Release")
	message("Release build")
	
	add_compile_options(-O3)
	
else()
	message("Unknown build: " ${CMAKE_BUILD_TYPE})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
endif()


if(NOT CMAKE_CXX_EXTENSIONS)
	set(CMAKE_CXX_EXTENSIONS OFF)
endif()


set(SOURCES
	src/palette.cpp
	src/files.cpp
	src/arg_parser.cpp
	src/alert.cpp
	src/ncurses_wrap.cpp
	src/dynamic_bitset.cpp
	
	src/frame.cpp
	src/frame_group.cpp
	src/reader.cpp
	src/drawer.cpp
	src/interthread.cpp
	
	src/start.cpp
	src/main.cpp
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	set(SOURCES ${SOURCES} src/debug.cpp)
endif()


add_executable(ncv ${SOURCES})

target_link_libraries(ncv ncursesw avformat avcodec avutil swscale pthread SDL2 SDL_mixer)

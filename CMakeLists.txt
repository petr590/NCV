cmake_minimum_required(VERSION 3.3)
project(NewConsoleViewer VERSION 2.1.2 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_SOURCE_DIR src)

include_directories(include)

add_compile_options(
	-Wall
	-Wextra
	-Wpedantic

	-Wcast-align
	-Wcast-qual
	-Wold-style-cast
	-Wenum-compare
#	-Wfloat-equal
#	-Wconversion
#	-Wsign-conversion

	-Wctor-dtor-privacy
	-Wnon-virtual-dtor
	-Woverloaded-virtual
	
	-Wduplicated-branches
	-Wduplicated-cond
	-Wextra-semi
	-Wlogical-op
	-Wredundant-decls
	-Wshadow=compatible-local
	-Wsign-promo
	-Wzero-as-null-pointer-constant
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
	src/args.cpp
	src/alert.cpp
	src/ncurses_wrap.cpp
	src/dynamic_bitset.cpp
	
	src/frame.cpp
	src/frame_group.cpp
	src/reader.cpp
	src/renderer.cpp
	src/double_buffer.cpp
	src/interthread.cpp
	
	src/start.cpp
	src/main.cpp
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	set(SOURCES ${SOURCES} src/debug.cpp)
endif()


add_executable(ncv ${SOURCES})

target_link_libraries(ncv
	ncursesw avformat avcodec avutil swscale pthread
	#SDL2 SDL_mixer
)

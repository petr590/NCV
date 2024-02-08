cmake_minimum_required(VERSION 3.16)

project(NewConsoleViewer VERSION 1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_SOURCE_DIR, src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)

add_compile_options(
#	-Werror

	-Wall
	-Wextra
	-Wpedantic

	-Wcast-align
	-Wcast-qual

#	-Wconversion
#	-Wsign-conversion
	-Wenum-compare
#	-Wfloat-equal

	-Wctor-dtor-privacy
	-Wnon-virtual-dtor
	-Woverloaded-virtual
	-Wredundant-decls
	-Wsign-promo
)

if(NOT CMAKE_BUILD_TYPE MATCHES "Windows")
	add_compile_options(-Wold-style-cast)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	message("Debug build")
	
	add_compile_options(-g)
	add_compile_definitions(DEBUG)
	
	set(CMAKE_C_COMPILER "g++")
	set(CMAKE_CXX_COMPILER "g++")

elseif(CMAKE_BUILD_TYPE MATCHES "Release")
	message("Release build")
	
	add_compile_options(-O3)

	set(CMAKE_C_COMPILER "g++")
	set(CMAKE_CXX_COMPILER "g++")
	
elseif(CMAKE_BUILD_TYPE MATCHES "Windows")
	message("Windows build")
	
	include_directories(/home/winch/0x590/c++/ncv/PDCurses/ /home/winch/0x590/c++/ncv/PDCurses/wincon/)
	
	set(CMAKE_C_COMPILER "i686-w64-mingw32-g++")
	set(CMAKE_CXX_COMPILER "i686-w64-mingw32-g++")
	add_link_options(PDCurses/wincon/libpdcurses.a)
	add_compile_definitions(NO_USE_SET_ESCDELAY)
else()
	
	message("Unknown build: " ${CMAKE_BUILD_TYPE})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
endif()


if(NOT CMAKE_CXX_EXTENSIONS)
	set(CMAKE_CXX_EXTENSIONS OFF)
endif()


if(CMAKE_BUILD_TYPE MATCHES "Windows")
	add_library(pdcurses STATIC PDCurses/wincon/)
else()
	find_library(NCURSESW_LIBRARY ncursesw)
endif()


add_executable(ncv src/main.cpp)

if(NOT CMAKE_BUILD_TYPE MATCHES "Windows")
	target_link_libraries(ncv ${NCURSESW_LIBRARY})
endif()

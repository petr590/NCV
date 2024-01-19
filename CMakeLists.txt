cmake_minimum_required(VERSION 3.16)

project(NewConsoleViewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_SOURCE_DIR, src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

add_compile_options(
#	-Werror

	-Wall
	-Wextra
	-Wpedantic

	-Wcast-align
	-Wcast-qual
	-Wold-style-cast

#	-Wconversion
#	-Wsign-conversion
	-Wenum-compare
#	-Wfloat-equal

	-Wctor-dtor-privacy
	-Wnon-virtual-dtor
	-Woverloaded-virtual
	-Wredundant-decls
	-Wsign-promo
	
	-g
)

if(NOT CMAKE_CXX_EXTENSIONS)
	set(CMAKE_CXX_EXTENSIONS OFF)
endif()

find_library(NCURSES_LIBRARY ncurses)

add_executable(ncv src/main.cpp)

target_link_libraries(ncv ${NCURSES_LIBRARY})

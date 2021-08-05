cmake_minimum_required(VERSION 3.10)

project(shell)

set(SHELL_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sources)
set(SHELL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SHELL_SOURCES
	${SHELL_SOURCES_DIR}/parser.cpp
	${SHELL_SOURCES_DIR}/shell.cpp
)

add_library(shell STATIC ${SHELL_SOURCES})
target_include_directories(shell PRIVATE ${SHELL_INCLUDE_DIR})
cmake_minimum_required(VERSION 3.8)
project(OpenGS)

option(OGS_DEV "Developer Build" OFF)

if(OGS_DEV)
	add_definitions(-DOGS_DEV)
endif()

add_subdirectory(goldsrc)
cmake_minimum_required(VERSION 3.8)
project(OGSEngine VERSION 0.6.3)

#####################################################################

# Define available options

option(OGS_DEV "Developer Build" OFF)
option(OGS_DEBUG "Debug Build" OFF)
option(OGS_USE_SDL "Use SDL2 for platform-specific functionality implementation" OFF)
option(OGS_USE_IMGUI "Use ImGui for internal GUI system implementation" OFF)

option(OGS_4554 "4554 preset")
option(OGS_6153 "6153+ preset")
option(OGS_DEDICATED "Skip building of client-side modules" OFF)

#####################################################################

# Specify intermediate directory (for building purposes)
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

# Specify output directory (for compiled binaries)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Set additional paths to search for cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

# Set path to CEF
set(CEF_ROOT ${CMAKE_SOURCE_DIR}/../cef_binary/latest
	CACHE PATH
	"Path to CEF binary distribution"
)

# Set path to imgui
set(OGS_IMGUI_PATH ${CMAKE_SOURCE_DIR}/../imgui
	CACHE PATH
	"Path to imgui library"
)

#####################################################################

# Set C language standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_STANDARD_EXTENSIONS ON)

# Set C++ language standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_STANDARD_EXTENSIONS ON)

#####################################################################

if(OGS_DEV)
	#add_definitions(-DOGS_DEV)
	add_definitions(-DOGS_DEBUG)
endif()

if(OGS_DEDICATED)
	add_definitions(-DSWDS)
endif()

if(OGS_USE_SDL)
	add_definitions(-DOGS_USE_SDL)
endif()

#####################################################################

# Add subdirectories

# Add engine root directory
add_subdirectory(goldsrc)
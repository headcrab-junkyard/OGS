cmake_minimum_required(VERSION 3.8)
project(OGSEngine VERSION 0.5)

# Define available options
option(OGS_DEV "Developer Build" OFF)
option(OGS_DEBUG "Debug Build" OFF)
option(OGS_4554 "4554 preset")
option(OGS_6153 "6153+ preset")
option(OGS_DEDICATED "" OFF)

if(OGS_DEV)
	#add_definitions(-DOGS_DEV)
	add_definitions(-DOGS_DEBUG)
endif()

if(OGS_DEDICATED)
	add_definitions(-DSWDS)
endif()

# Add subdirectories

add_subdirectory(goldsrc)
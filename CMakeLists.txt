cmake_minimum_required(VERSION 3.8)
project(OGSEngine)

option(OGS_DEV "Developer Build" OFF)
option(OGS_DEBUG "Debug Build" OFF)
option(OGS_4554 "4554 preset")
option(OGS_6153 "6153+ preset")

if(NOT OGS_PRESET_TYPE)
    set(OGS_PRESET_TYPE Vanilla CACHE STRING
        "Choose the type of engine features preset"
        FORCE
	)
    set_property(CACHE OGS_PRESET_TYPE PROPERTY STRINGS
		Vanilla
	)
endif()

if(OGS_DEV)
	#add_definitions(-DOGS_DEV)
	add_definitions(-DOGS_DEBUG)
endif()

add_subdirectory(goldsrc)
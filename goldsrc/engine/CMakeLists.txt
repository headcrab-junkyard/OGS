project(ogs-engine)

option(OGS_ENGINE_HW "Build the engine module with hardware render (OpenGL) support, it will be using software render otherwise" ON)

if(OGS_ENGINE_HW)
	add_definitions(-DGLQUAKE) # TODO
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(render)

file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp

if(WIN32)
	${CMAKE_CURRENT_SOURCE_DIR}/win/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/win/*.cpp
#else(UNIX)
	#${CMAKE_CURRENT_SOURCE_DIR}/linux/*.c
	#${CMAKE_CURRENT_SOURCE_DIR}/linux/*.cpp
#else()
	#${CMAKE_CURRENT_SOURCE_DIR}/null/*.c
	#${CMAKE_CURRENT_SOURCE_DIR}/null/*.cpp
endif()
)

add_library(${PROJECT_NAME} MODULE ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} ogs-interface ogs-mathlib)

if(OGS_USE_IMGUI)
	target_link_libraries(${PROJECT_NAME} imgui)
	if(WIN32)
		target_link_libraries(${PROJECT_NAME} XInput1_3)
	endif()
endif()

if(WIN32)
	# NOTE: looks like wsock32 is loaded dynamically
	# ws2_32
	target_link_libraries(${PROJECT_NAME} wsock32 winmm dxguid)
endif()

if(NOT OGS_DEDICATED)
	if(OGS_ENGINE_HW)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			OUTPUT_NAME "hw"
		)
		
		target_link_libraries(${PROJECT_NAME} ogs-render-gl)
	else()
		set_target_properties(${PROJECT_NAME} PROPERTIES
			OUTPUT_NAME "sw"
		)
		
		target_link_libraries(${PROJECT_NAME} ogs-render-soft)
	endif()
else()
	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME "swds"
	)
	
	target_link_libraries(${PROJECT_NAME} ogs-render-soft)
endif()
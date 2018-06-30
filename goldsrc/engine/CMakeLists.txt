project(ogs-engine)

option(OGS_ENGINE_HW "Build the engine module with hardware render (OpenGL) support, it will be using software render otherwise" ON)

if(OGS_ENGINE_HW)
	add_definitions(-DGLQUAKE) # TODO
endif()

file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/null/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/null/*.cpp
)

add_library(${PROJECT_NAME} MODULE ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} ogs-interface)

if(NOT OGS_DEDICATED)
	if(OGS_ENGINE_HW)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			OUTPUT_NAME "hw"
		)
		
		#target_link_libraries(${PROJECT_NAME} ogs-render-gl)
	else()
		set_target_properties(${PROJECT_NAME} PROPERTIES
			OUTPUT_NAME "sw"
		)
		
		#target_link_libraries(${PROJECT_NAME} ogs-render-soft)
	endif()
else()
	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME "swds"
	)
	
	#target_link_libraries(${PROJECT_NAME} ogs-render-soft)
endif()
project(ogs-render-gl)

file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
if(WIN32)
	${CMAKE_CURRENT_SOURCE_DIR}/win/*.c
elseif(UNIX)
	#${CMAKE_CURRENT_SOURCE_DIR}/linux/*.c
endif()
)

if(OGS_USE_INTERNAL_RENDER) #if(OGS_EVOL)
	add_library(${PROJECT_NAME} MODULE ${PROJECT_SOURCES})

	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME "r_gl"
	)

	target_link_libraries(${PROJECT_NAME} ogs-tier1)
else()
	add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
endif()
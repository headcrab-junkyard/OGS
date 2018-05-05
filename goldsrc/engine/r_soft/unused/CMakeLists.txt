project(gskiller-render-soft)

file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

if(OGS_USE_INTERNAL_RENDER) #if(OGS_EVOL)
	add_library(${PROJECT_NAME} MODULE ${PROJECT_SOURCES})

	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME "r_soft"
	)

	target_link_libraries(${PROJECT_NAME} ogs-interface)
else()
	add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
endif()
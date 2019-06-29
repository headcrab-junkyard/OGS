project(ogs-render-gl)

#if(NOT OGS_USE_INTERNAL_RENDER) #if(OGS_EVOL)
#	add_library(${PROJECT_NAME} MODULE)
#
#	set_target_properties(${PROJECT_NAME} PROPERTIES
#		OUTPUT_NAME "r_gl"
#	)
#
#	target_link_libraries(${PROJECT_NAME} ogs-tier1)
#else()
	add_library(${PROJECT_NAME} STATIC)
#endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

if(WIN32)
	set(PLATFORM_NAME win)
elseif(UNIX)
	set(PLATFORM_NAME linux)
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME} PROJECT_SOURCES_PLATFORM)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
	PRIVATE ${PROJECT_SOURCES_PLATFORM}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLQUAKE)
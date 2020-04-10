project(ogs-dedicated)

add_executable(${PROJECT_NAME})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
)

if(WIN32)
	aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/win PROJECT_SOURCES_PLATFORM)
else()
	aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/unix PROJECT_SOURCES_PLATFORM)
endif()

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES_PLATFORM}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "ogsds"
)

target_link_libraries(${PROJECT_NAME} ogs-tier1)

if(UNIX)
	target_link_libraries(${PROJECT_NAME} -ldl)
endif()
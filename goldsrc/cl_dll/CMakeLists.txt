project(ogs-cl_dll)

add_library(${PROJECT_NAME} MODULE)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "client"
)

target_link_libraries(${PROJECT_NAME} ogs-tier1 ogs-pm_shared ogs-mathlib ogs_game_shared-client)

if(WIN32)
	target_link_libraries(${PROJECT_NAME} winmm) #joyGetPosEx
endif()
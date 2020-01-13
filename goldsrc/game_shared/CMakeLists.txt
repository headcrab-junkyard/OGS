project(ogs-game_shared)

###############################################################################

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} COMMON_SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hl HL_SOURCES)

###############################################################################

add_library(${PROJECT_NAME}-client STATIC)

target_compile_definitions(${PROJECT_NAME}-client PRIVATE OGS_CLIENT_DLL)

target_sources(${PROJECT_NAME}-client
	PRIVATE ${COMMON_SOURCES}
	PRIVATE ${HL_SOURCES}
)

###############################################################################

add_library(${PROJECT_NAME}-server STATIC)

target_compile_definitions(${PROJECT_NAME}-server PRIVATE OGS_GAME_DLL)

target_sources(${PROJECT_NAME}-server
	PRIVATE ${COMMON_SOURCES}
	PRIVATE ${HL_SOURCES}
)
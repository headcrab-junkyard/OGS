project(OGS-voice)

add_library(${PROJECT_NAME} MODULE ${VOICE_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "voice_null"
)
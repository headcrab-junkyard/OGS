project(gskiller-engine)

add_definitions(-DGLQUAKE)

file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/null/*.cpp
)

add_library(${PROJECT_NAME} MODULE ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "engine"
)
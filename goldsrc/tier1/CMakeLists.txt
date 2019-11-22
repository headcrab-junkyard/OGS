project(ogs-tier1)

add_library(${PROJECT_NAME} STATIC interface.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
	POSITION_INDEPENDENT_CODE ON
)
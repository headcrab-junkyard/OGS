project(ogs-vguiwrap)

###############################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vgui/include)

if(WIN32)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/vgui/lib/win)
elseif(UNIX)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/vgui/lib/linux)
endif()

###############################################################################

# Build as dynamic library
add_library(${PROJECT_NAME} MODULE)

# Build a list of sources
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

target_sources(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCES}
)

###############################################################################

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "vguiwrap"
)

# Link with external libraries
target_link_libraries(${PROJECT_NAME} ogs-tier1)
target_link_libraries(${PROJECT_NAME} vgui)

###############################################################################

install(TARGETS ${PROJECT_NAME} DESTINATION .)
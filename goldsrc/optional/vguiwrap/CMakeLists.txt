project(ogs-vguiwrap)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vgui/include)

if(WIN32)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/vgui/lib/win)
elseif(UNIX)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/vgui/lib/linux)
endif()

# Build a list of sources
file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Build as dynamic library
add_library(${PROJECT_NAME} MODULE ${PROJECT_SOURCES})

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "vguiwrap"
)

# Link with external libraries
target_link_libraries(${PROJECT_NAME} ogs-interface)
target_link_libraries(${PROJECT_NAME} vgui)
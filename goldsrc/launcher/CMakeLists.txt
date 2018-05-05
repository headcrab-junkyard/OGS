project(gskiller-launcher)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

if(WIN32)
	set(PROJECT_TARGET_PLATFORM win)
elseif(UNIX)
	set(PROJECT_TARGET_PLATFORM linux)
else()
	message(FATAL_ERROR "Unsupported platform!")
endif()

file(GLOB PROJECT_PLATFORM_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_TARGET_PLATFORM}/*.cpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_PLATFORM_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "launcher"
)

target_link_libraries(${PROJECT_NAME} ogs-interface)

if(UNIX)
	target_link_libraries(${PROJECT_NAME} -ldl)
endif()
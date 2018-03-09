set(OGS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
	${CMAKE_SOURCE_DIR}/../ogs-sdk
	${CMAKE_SOURCE_DIR}/../ogs-sdk/common
	${CMAKE_SOURCE_DIR}/../ogs-sdk/engine
	${OGS_ROOT}/engine
)

add_subdirectory(filesystem)

#add_subdirectory(gameui)

#add_subdirectory(render)

add_subdirectory(engine)

add_subdirectory(dedicated)
add_subdirectory(launcher)
set(OGS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
	${CMAKE_SOURCE_DIR}/../ogs-sdk/ogs
	${CMAKE_SOURCE_DIR}/../ogs-sdk/ogs/common
	${CMAKE_SOURCE_DIR}/../ogs-sdk/ogs/engine
	${CMAKE_SOURCE_DIR}/../ogs-sdk/ogs/public
	${OGS_ROOT}/public
	${OGS_ROOT}/engine
)

add_subdirectory(${CMAKE_SOURCE_DIR}/../ogs-sdk ${CMAKE_BINARY_DIR}/ogs-sdk)

#add_subdirectory(filesystem)

add_subdirectory(gameui)

add_subdirectory(engine)

add_subdirectory(dedicated)
add_subdirectory(launcher)
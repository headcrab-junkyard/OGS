set(OGS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
	${CMAKE_SOURCE_DIR}/../gskiller-sdk/gskiller
	${CMAKE_SOURCE_DIR}/../gskiller-sdk/gskiller/common
	${CMAKE_SOURCE_DIR}/../gskiller-sdk/gskiller/engine
	${CMAKE_SOURCE_DIR}/../gskiller-sdk/gskiller/public
	${OGS_ROOT}/engine
)

add_subdirectory(${CMAKE_SOURCE_DIR}/../gskiller-sdk ${CMAKE_BINARY_DIR}/gskiller-sdk)

#add_subdirectory(filesystem)

add_subdirectory(gameui)

#add_subdirectory(render)

#add_subdirectory(engine)

add_subdirectory(dedicated)
add_subdirectory(launcher)
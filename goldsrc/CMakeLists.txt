# We want shared libraries (and modules) without any prefix in name
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_MODULE_PREFIX "")

# Set this dir as a root
set(OGS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Add include directories
include_directories(
	${CMAKE_SOURCE_DIR}/../ogs-sdk/ogs
	${CMAKE_SOURCE_DIR}/../ogs-sdk/ogs/common
	${CMAKE_SOURCE_DIR}/../ogs-sdk/ogs/engine
	${CMAKE_SOURCE_DIR}/../ogs-sdk/ogs/pm_shared
	${CMAKE_SOURCE_DIR}/../ogs-sdk/ogs/public
	${OGS_ROOT}/public
	${OGS_ROOT}/engine
)

# Add subdirectories

add_subdirectory(${CMAKE_SOURCE_DIR}/../ogs-sdk ${CMAKE_BINARY_DIR}/ogs-sdk)

add_subdirectory(filesystem)
add_subdirectory(engine)
add_subdirectory(dedicated)

# If not building as dedicated server then build these modules too
if(NOT OGS_DEDICATED)
	#add_subdirectory(vgui3) # vgui2?

	#add_subdirectory(gameui)
	
	add_subdirectory(launcher)
endif()
# We want shared libraries (and modules) without any prefix in name
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_MODULE_PREFIX "")

# Set this dir as a root
set(OGS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Add include directories
include_directories(
	${CMAKE_SOURCE_DIR}/../ogs-sdk/goldsrc
	${CMAKE_SOURCE_DIR}/../ogs-sdk/goldsrc/common
	${CMAKE_SOURCE_DIR}/../ogs-sdk/goldsrc/engine
	${CMAKE_SOURCE_DIR}/../ogs-sdk/goldsrc/pm_shared
	${CMAKE_SOURCE_DIR}/../ogs-sdk/goldsrc/public
	${OGS_ROOT}/public
	${OGS_ROOT}/engine
)

# Add subdirectories

add_subdirectory(${CMAKE_SOURCE_DIR}/../ogs-sdk ${CMAKE_BINARY_DIR}/ogs-sdk)

add_subdirectory(dlls)

add_subdirectory(filesystem)
add_subdirectory(engine)
add_subdirectory(dedicated)

# If not building as dedicated server then build these modules too
if(NOT OGS_DEDICATED)
	add_subdirectory(vgui2)

	add_subdirectory(cl_dll)
	
	#add_subdirectory(gameui)
	
	add_subdirectory(launcher)
endif()
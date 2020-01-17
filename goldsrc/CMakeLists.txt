# We want shared libraries (and modules) without any prefix in name
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_MODULE_PREFIX "")

# Set this dir as a root
set(OGS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Add include directories
include_directories(
	${OGS_ROOT}
	${OGS_ROOT}/common
	${OGS_ROOT}/engine
	${OGS_ROOT}/pm_shared
	${OGS_ROOT}/public
	${OGS_SDK_PATH}
	${OGS_SDK_PATH}/common
	${OGS_SDK_PATH}/engine
	${OGS_SDK_PATH}/pm_shared
	${OGS_SDK_PATH}/public
)

# Add subdirectories

add_subdirectory(pm_shared)
#add_subdirectory(utils)

add_subdirectory(optional)

add_subdirectory(dlls)

add_subdirectory(filesystem)
add_subdirectory(engine)
add_subdirectory(dedicated)

# If not building as dedicated server then build these modules too
if(NOT OGS_DEDICATED)
	add_subdirectory(vgui2)

	#add_subdirectory(cl_dll)
	
	add_subdirectory(gameui)
	
	#add_subdirectory(launcher)
endif()
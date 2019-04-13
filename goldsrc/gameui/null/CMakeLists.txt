project(OGS-gameui)

# Detect the impl type
if(NOT OGS_GAMEUI_IMPL_TYPE)
    set(OGS_GAMEUI_IMPL_TYPE GSR CACHE STRING
        "Choose the type of GameUI module implementation"
        FORCE
	)
    set_property(CACHE OGS_GAMEUI_IMPL_TYPE PROPERTY STRINGS
		Null    # Null impl (just for test)
		Default # Default implementation
		GSR
	)
endif()

string(TOLOWER ${OGS_GAMEUI_IMPL_TYPE} OGS_GAMEUI_IMPL_TYPE)
add_subdirectory(${OGS_GAMEUI_IMPL_TYPE})
function (alimer_setup_library TARGET_NAME)
    if (MSVC)
        # Set warning level 3
        target_compile_options(${TARGET_NAME} PRIVATE /W3)

        # Warnings as Errors Enabled
        target_compile_options(${TARGET_NAME} PRIVATE /WX)

        # Debug information
	    target_compile_options(${TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:/Zi>)

        # Enable full optimization in dev/release
	    target_compile_options(${TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:/Od> $<$<NOT:$<CONFIG:Debug>>:/Ox>)

        # Inline function expansion
	    target_compile_options(${TARGET_NAME} PRIVATE /Ob2)

        # Enable intrinsic functions in dev/release
	    target_compile_options(${TARGET_NAME} PRIVATE $<$<NOT:$<CONFIG:Debug>>:/Oi>)

        # Favor fast code
	    target_compile_options(${TARGET_NAME} PRIVATE /Ot)

        # Enable fiber-safe optimizations in dev/release
	    target_compile_options(${TARGET_NAME} PRIVATE $<$<NOT:$<CONFIG:Debug>>:/GT>)

	    # Enable string pooling
	    target_compile_options(${TARGET_NAME} PRIVATE /GF)

        # Use security checks only in debug
	    target_compile_options(${TARGET_NAME} PRIVATE $<$<CONFIG:DEBUG>:/sdl> $<$<NOT:$<CONFIG:DEBUG>>:/sdl->)

	    # Enable function-level linking
	    target_compile_options(${TARGET_NAME} PRIVATE /Gy)

        # Use fast floating point model
        target_compile_options(${TARGET_NAME} PRIVATE /fp:fast "$<$<NOT:$<CONFIG:DEBUG>>:/guard:cf>")
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra)
    endif()

    if(WIN32)
        target_compile_definitions(${TARGET_NAME} PRIVATE _UNICODE UNICODE _CRT_SECURE_NO_WARNINGS)
        target_compile_definitions(${TARGET_NAME} PRIVATE NOMINMAX)
        target_compile_definitions(${TARGET_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
    endif ()

    set_target_properties(
        ${TARGET_NAME} PROPERTIES

        # Postfix for different profiles
        DEBUG_POSTFIX "d"
        RELWITHDEBINFO_POSTFIX "rd"
        MINSIZEREL_POSTFIX "s"
    )
endfunction()

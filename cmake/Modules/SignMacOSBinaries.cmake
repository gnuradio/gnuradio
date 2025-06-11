macro(REPORT_MISSING_SIGNING_CONTEXT)
    message(FATAL_ERROR [=[
        Signing requested but no identity configured.
        Please set the correct env variable or provide the MAC_SIGNING_IDENTITY argument on the command line
        ]=])
endmacro()

function(install_sign_osx tgt)
    set_target_properties(${tgt} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER ${tgt}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        OUTPUT_NAME ${tgt}
    )

    if(NOT MAC_SIGNING_IDENTITY)
        if(NOT DEFINED ENV{MAC_SIGNING_CERT_NAME} AND GR_SIGN)
            REPORT_MISSING_SIGNING_CONTEXT()
        endif()
        set(MAC_SIGNING_IDENTITY $ENV{MAC_SIGNING_CERT_NAME})
    endif()
    if(NOT MAC_SIGNING_TID)
        if(NOT DEFINED ENV{MAC_NOTARIZATION_TID} AND GR_SIGN)
            REPORT_MISSING_SIGNING_CONTEXT()
        endif()
        set(MAC_SIGNING_TID $ENV{MAC_NOTARIZATION_TID})
    endif()

    # Setup MacOS signing for individual binaries
    set_target_properties(${tgt} PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${MAC_SIGNING_TID}
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${MAC_SIGNING_IDENTITY}
        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED True
        XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "--timestamp=http://timestamp.apple.com/ts01  --options=runtime,library"
    )
    install(CODE "execute_process(COMMAND codesign --options runtime --timestamp -s \"${MAC_SIGNING_IDENTITY}\" $<TARGET_FILE:${tgt}>)")
endfunction()
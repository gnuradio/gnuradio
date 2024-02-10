include(ExternalProject)

if(BUILD_UHD)
    include(deps/FetchAndBuildLibUSB.cmake)
    find_package(Boost REQUIRED)
    include(CMakePrintHelpers)

    PYCHECK_MODULE(mako)
    if(NOT mako_IMPORTED)
        message(FATAL_ERROR "Mako required for UHD build")
    else()
        message(STATUS "Mako found")
    endif()
    include(deps/FetchAndBuildUHD.cmake)
endif()

if(BUILD_PORTAUDIO)
    include(deps/FetchAndBuildPortAudio.cmake)
endif()
cmake_minimum_required(VERSION 3.20)

project(FetchAndBuildGNURadioDeps)

cmake_policy(SET CMP0135 NEW)

# Setup CLI - this project is effectively just the CLI and some external Projects
# All deps are built by default: all options "on"
# Ideally this project will eventually become moot as the packages it vendors
# will be incorporated into either Conan or Spack with Windows and MacOS support
option(BUILD_UHD "Build UHD Project" ON)
option(BUILD_PORTAUDIO "Build PortAudio Project" ON)
option(BUILD_VOLK "Build VOLK Project" ON)
option(BUILD_GOBJECT "Build GObjectIntrospection Project" OFF)

include(utils/CheckPythonModule.cmake)

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
    include(tpls/FetchAndBuildUHD.cmake)
endif()

if(BUILD_PORTAUDIO)
    include(tpls/FetchAndBuildPortAudio.cmake)
endif()

if(BUILD_VOLK)
    include(tpls/FetchAndBuildVOLK.cmake)
endif()

if(BUILD_GOBJECT)
    include(tpls/FetchAndBuildGObject.cmake)
endif()

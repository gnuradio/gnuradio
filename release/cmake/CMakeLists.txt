cmake_minimum_required(VERSION 3.20)

project(FetchAndBuildGNURadioDeps)

cmake_policy(SET CMP0135 NEW)

# Setup CLI - this project is effectively just the CLI and some external Projects
# All deps are built by default: all options "on"
# Ideally this project will eventually become moot as the packages it vendors
# will be incorporated into either Conan or Spack with Windows and MacOS support
option(BUILD_UHD "Build UHD Project" ON)
option(BUILD_PORTAUDIO "Build PortAudio Project" ON)

include(utils/CheckPythonModule.cmake)
include(DriveInstallers.cmake)

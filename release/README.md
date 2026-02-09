# GNU Installer

This directory contains components utilized in the building of GNU Radio + its installers
in a redistributable and repoducible way


## CMake Directory

This directory contains and independent CMake project that can be used to build some dependencies
of GNU Radio that would otherwise have to be built by hand on Windows

This includes

* portaudio
* libusb
* UHD
* volk
* gobject

Coming soon via dockerfile: codec2

## Conanfile

This is the conan file that will install all other dependencies

via command `conan install  --output-folder conan .` from the root of this directory

when building the CMake project above, point `CMAKE_PREFIX_PATH` to the directory containing the config module
which, if using the command above, is `conan\build\generators`

> **DO NOT USE THE CONAN GENERATED CMAKE TOOLCHAIN**
    It is fundamentally broken as far as I was able to test

## Win32 Folder

This is files used by Wix for the construction of the installer

Currently a couple of Wix templates to supplement what CPack will generate

At the moment most of the functionality is to setup the registry to OOT modules can install themselves
and the GNU Radio can locate them
also the mechanism to be leveraged to "register" the installations by the gui/installer for oot modules


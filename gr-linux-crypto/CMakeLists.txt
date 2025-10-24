# Copyright 2024 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Find dependencies
########################################################################
include(GrBoost)

# Required dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(KEYUTILS REQUIRED libkeyutils)

# Optional dependencies for integration
pkg_check_modules(OPENSSL QUIET openssl)
pkg_check_modules(SODIUM QUIET libsodium)

# Optional hardware security module support
pkg_check_modules(NITROKEY QUIET libnitrokey)

########################################################################
# Register component
########################################################################
include(GrComponent)
gr_register_component("gr-linux-crypto" ENABLE_GR_LINUX_CRYPTO KEYUTILS_FOUND ENABLE_GNURADIO_RUNTIME)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_LINUX_CRYPTO)

########################################################################
# Compiler setup
########################################################################
if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
    add_definitions(-fvisibility=hidden)
endif()

########################################################################
# Include directories
########################################################################
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${GNURADIO_INCLUDE_DIRS})

########################################################################
# API definition
########################################################################
add_definitions(-DLINUX_CRYPTO_API=__attribute__((visibility("default"))))

########################################################################
# Library sources
########################################################################
set(gr_linux_crypto_sources
    lib/kernel_keyring_source_impl.cc
    lib/nitrokey_interface_impl.cc
    lib/kernel_crypto_aes_impl.cc
)

set(gr_linux_crypto_headers
    lib/kernel_keyring_source_impl.h
    lib/nitrokey_interface_impl.h
    lib/kernel_crypto_aes_impl.h
)

########################################################################
# Create library
########################################################################
add_library(gnuradio-linux-crypto SHARED ${gr_linux_crypto_sources})

target_link_libraries(gnuradio-linux-crypto
    ${GNURADIO_LIBRARIES}
    ${KEYUTILS_LIBRARIES}
)

# Add optional dependencies
if(OPENSSL_FOUND)
    target_link_libraries(gnuradio-linux-crypto ${OPENSSL_LIBRARIES})
    target_compile_definitions(gnuradio-linux-crypto PRIVATE HAVE_OPENSSL)
endif()

if(SODIUM_FOUND)
    target_link_libraries(gnuradio-linux-crypto ${SODIUM_LIBRARIES})
    target_compile_definitions(gnuradio-linux-crypto PRIVATE HAVE_SODIUM)
endif()

if(NITROKEY_FOUND)
    target_link_libraries(gnuradio-linux-crypto ${NITROKEY_LIBRARIES})
    target_compile_definitions(gnuradio-linux-crypto PRIVATE HAVE_NITROKEY)
endif()

# Set library properties
set_target_properties(gnuradio-linux-crypto PROPERTIES
    VERSION ${GNURADIO_VERSION}
    SOVERSION ${GNURADIO_SOVERSION}
)

########################################################################
# Python bindings
########################################################################
if(ENABLE_PYTHON)
    find_package(Python3 COMPONENTS Interpreter Development)
    find_package(pybind11 REQUIRED)
    
    # Create Python bindings
    pybind11_add_module(linux_crypto_python
        python/linux_crypto_python.cc
        python/linux_crypto_integration.py
    )
    
    target_link_libraries(linux_crypto_python PRIVATE
        gnuradio-linux-crypto
        ${GNURADIO_LIBRARIES}
    )
    
    # Install Python module
    install(TARGETS linux_crypto_python
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages
    )
endif()

########################################################################
# GRC blocks
########################################################################
if(ENABLE_GRC)
    install(DIRECTORY grc/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/gnuradio/grc/blocks
        FILES_MATCHING PATTERN "*.yml"
    )
endif()

########################################################################
# Examples
########################################################################
if(ENABLE_EXAMPLES)
    install(DIRECTORY examples/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/gr-linux-crypto/examples
        FILES_MATCHING PATTERN "*.py"
    )
endif()

########################################################################
# Documentation
########################################################################
if(ENABLE_DOXYGEN)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        install(DIRECTORY docs/
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/gr-linux-crypto/docs
        )
    endif()
endif()

########################################################################
# Installation
########################################################################
install(TARGETS gnuradio-linux-crypto
    EXPORT gnuradio-linux-crypto-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

install(DIRECTORY include/gnuradio/linux_crypto/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/gnuradio/linux_crypto
    FILES_MATCHING PATTERN "*.h"
)

########################################################################
# Testing
########################################################################
if(ENABLE_TESTING)
    enable_testing()
    
    # Test targets will be added when test files are created
    message(STATUS "  Testing enabled but no test files found")
endif()

########################################################################
# Fuzzing (optional)
########################################################################
if(ENABLE_FUZZING)
    find_program(AFL_PLUS_PLUS afl-fuzz)
    if(AFL_PLUS_PLUS)
        # Add fuzzing targets
        add_executable(fuzz_kernel_keyring
            security/fuzzing/kernel_keyring_fuzz_adapted.cpp
        )
        target_link_libraries(fuzz_kernel_keyring
            gnuradio-linux-crypto
            ${GNURADIO_LIBRARIES}
        )
        
        # Install fuzzing scripts
        install(DIRECTORY security/fuzzing/
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/gr-linux-crypto/fuzzing
            FILES_MATCHING PATTERN "*.cpp" PATTERN "*.sh"
        )
    endif()
endif()

########################################################################
# Configuration summary
########################################################################
message(STATUS "gr-linux-crypto configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  GNU Radio version: ${GNURADIO_VERSION}")
message(STATUS "  OpenSSL support: ${OPENSSL_FOUND}")
message(STATUS "  Sodium support: ${SODIUM_FOUND}")
message(STATUS "  Nitrokey support: ${NITROKEY_FOUND}")
message(STATUS "  Python bindings: ${ENABLE_PYTHON}")
message(STATUS "  GRC blocks: ${ENABLE_GRC}")
message(STATUS "  Examples: ${ENABLE_EXAMPLES}")
message(STATUS "  Documentation: ${ENABLE_DOXYGEN}")
message(STATUS "  Testing: ${ENABLE_TESTING}")
message(STATUS "  Fuzzing: ${ENABLE_FUZZING}")

endif(ENABLE_GR_LINUX_CRYPTO)
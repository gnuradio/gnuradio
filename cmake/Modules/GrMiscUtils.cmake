# Copyright 2010-2011,2014,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

if(DEFINED __INCLUDED_GR_MISC_UTILS_CMAKE)
    return()
endif()
set(__INCLUDED_GR_MISC_UTILS_CMAKE TRUE)

########################################################################
# Compute cpack component name from GR component name that falls within
# Cpack Generator component naming constraints
# ASCII characters A-Z, a-z, digits, underscores (_), or periods (.)
# Every identifier must begin with either a letter or an underscore.
#
# Right now this just replaces a dash with an underscore as GR Component
# names are generally pretty safe, but a decision will need to be made
# on whether to mangle or fail on ill-named components without
# obvious conversions
########################################################################
function(_compute_safe_component_name name)
    string(REPLACE "-" "_" _component_safe_name ${name})
    set(_component_safe_name ${_component_safe_name} PARENT_SCOPE)
endfunction()

########################################################################
# Installs third party runtime dependencies alongside their appropriate
# targets so they can be bundled and redistributed by installers
########################################################################
function(GR_BUNDLE_INSTALL)
    set(oneValueArgs TARGET COMPONENT)
    set(multiValueArgs DEPENDENCIES)
    cmake_parse_arguments(GR_BUNDLE "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if(APPLE)
        set_target_properties(${GR_BUNDLE_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH ON)
        # works with both bin and lib dirs
        set_target_properties(${GR_BUNDLE_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
    endif()
    if(WIN32)
        set(GR_BUNDLE_OUT bin)
    else()
        set(GR_BUNDLE_OUT lib)
    endif()
    message(STATUS "Bundling target: ${GR_BUNDLE_TARGET}")
    message(STATUS "  belongs to component ${GR_BUNDLE_COMPONENT}")
    get_property(component_dependencies GLOBAL PROPERTY COMPONENT_${GR_BUNDLE_COMPONENT}_DEPS)
    message(STATUS "  target depends on components: ")
    foreach(comp ${component_dependencies})
        message(STATUS "    ${comp}")
    endforeach()
    foreach(ext_dep ${GR_BUNDLE_DEPENDENCIES})
        if(TARGET ${ext_dep})
            message(STATUS "  Processing additional dependency: ${ext_dep}")
            get_target_property(IS_IMPORTED ${ext_dep} IMPORTED)
            if(NOT IS_IMPORTED)
                list(APPEND TARGET_FILES "$<TARGET_FILE:${ext_dep}>")
            else()
                list(APPEND TARGET_FILES "$<TARGET_PROPERTY:${ext_dep},LOCATION>")
            endif()
            list(APPEND EXT_DEP_DIRS "$<TARGET_PROPERTY:${ext_dep},LINK_DIRECTORIES>")
            list(APPEND TARGET_FILES "$<TARGET_PROPERTY:${ext_dep},INTERFACE_LINK_LIBRARIES>")
        endif()
    endforeach()
    list(APPEND TARGET_FILES "$<TARGET_FILE:${GR_BUNDLE_TARGET}>")
    list(APPEND EXT_DEP_DIRS "$<TARGET_PROPERTY:${GR_BUNDLE_TARGET},LINK_DIRECTORIES>")
    get_target_property(target_type ${GR_BUNDLE_TARGET} TYPE)
    set(BUNDLE_TARGET_FILE "$<TARGET_FILE:${GR_BUNDLE_TARGET}>")
    if(target_type STREQUAL "EXECUTABLE")
        set(GR_BUNDLE_EXES ${BUNDLE_TARGET_FILE})
    elseif(target_type STREQUAL "SHARED_LIBRARY")
        set(GR_BUNDLE_LIBS ${BUNDLE_TARGET_FILE})
    endif()
    _compute_safe_component_name(${GR_BUNDLE_COMPONENT})
    configure_file(${PROJECT_SOURCE_DIR}/cmake/Modules/BundleInstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/BundleInstall-${GR_BUNDLE_TARGET}.cmake.in @ONLY)
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/BundleInstall-${GR_BUNDLE_TARGET}.cmake INPUT ${CMAKE_CURRENT_BINARY_DIR}/BundleInstall-${GR_BUNDLE_TARGET}.cmake.in)
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/BundleInstall-${GR_BUNDLE_TARGET}.cmake COMPONENT ${_component_safe_name}_DEPS)
endfunction(GR_BUNDLE_INSTALL)

########################################################################
# Set the pre-processor definition if the condition is true.
#  - def the pre-processor definition to set and condition name
########################################################################
function(GR_ADD_COND_DEF def)
    if(${def})
        add_definitions(-D${def})
    endif(${def})
endfunction(GR_ADD_COND_DEF)

########################################################################
# Check for a header and conditionally set a compile define.
#  - hdr the relative path to the header file
#  - def the pre-processor definition to set
########################################################################
function(GR_CHECK_HDR_N_DEF hdr def)
    include(CheckIncludeFileCXX)
    check_include_file_cxx(${hdr} ${def})
    gr_add_cond_def(${def})
endfunction(GR_CHECK_HDR_N_DEF)

########################################################################
# Check if a compiler flag works and conditionally set a compile define.
#  - flag the compiler flag to check for
#  - have the variable to set with result
########################################################################
macro(GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE flag have)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(${flag} ${have})
    if(${have})
        string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_dup)
        if(${flag_dup} EQUAL -1)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
        endif(${flag_dup} EQUAL -1)
    endif(${have})
endmacro(GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE)

########################################################################
# Setup the library target for installation
# - set target properties
# - add target + module dependencies to component interface
# Also handle gnuradio custom naming conventions w/ extras mode.
# This method is a wrapper around GR_REGISTER_LIBRARY for backwards compat
# to OOT modules - Do not create new calls to this library, use GR_REGISTER_LIBRARY
########################################################################
function(GR_LIBRARY_FOO target)
    GR_REGISTER_LIBRARY(${target} ${ARGN})
endfunction(GR_LIBRARY_FOO)

########################################################################
# Setup the library target for installation
# - set target properties
# - add target + module dependencies to component interface
# Also handle gnuradio custom naming conventions w/ extras mode.
########################################################################
function(GR_REGISTER_LIBRARY target)
    #set additional target properties
    set_target_properties(${target} PROPERTIES SOVERSION ${LIBVER})
    set_target_properties(${target} PROPERTIES VERSION ${VERSION})
    get_property(COMPONENT_SET GLOBAL PROPERTY GR_ACTIVE_COMPONENT SET)
    if(${COMPONENT_SET})
        get_property(COMPONENT GLOBAL PROPERTY GR_ACTIVE_COMPONENT)
        message(STATUS "Assigning ${target} to component ${COMPONENT}")
        set_property(GLOBAL APPEND PROPERTY COMPONENT_${COMPONENT}_TARGETS ${target})
    endif()
    if(ARGN)
        define_property(GLOBAL PROPERTY TARGET_${target}_DIST_DEPS)
        define_property(GLOBAL PROPERTY TARGET_${target}_EXPORT_DEPS)
        set_property(GLOBAL PROPERTY COMPONENT_${COMPONENT}_NEEDS_DEPS TRUE)
        # targets included in GR_REGISTER_LIBRARY or GR_LIBRARY_FOO
        # are for including in a distribution interface
        # anything else is treated as a name for a find dependency call
        foreach(dep ${ARGN})
            if(TARGET ${dep})
                set_property(GLOBAL APPEND PROPERTY TARGET_${target}_DIST_DEPS ${dep})
            else()
                set_property(GLOBAL APPEND PROPERTY TARGET_${target}_EXPORT_DEPS ${dep})
            endif()
        endforeach()
    endif()
endfunction()

########################################################################
# Establish install rules for target TARGET for given COMPONENT
# Configure module export interface for TARGET and MODULE_DEPENDENCIES
# TARGET - target to gererate install interface for
# COMPONENT - component to which the target belongs
# MODULE_DEPENDENCIES - CMake module dependencies to generate find_dep calls for
########################################################################
function(GR_INSTALL_LIBRARY)
    set(singleValueArgs TARGET COMPONENT)
    set(multiValueArgs EXPORT_DEPS DISTRIBUTION_DEPS)
    cmake_parse_arguments(GR_LIB "" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    # install the generated files like so...
    _compute_safe_component_name(${GR_LIB_COMPONENT})
    install(
        TARGETS ${GR_LIB_TARGET}
        EXPORT ${GR_LIB_TARGET}-export
        COMPONENT ${_component_safe_name}
        LIBRARY DESTINATION ${GR_LIBRARY_DIR} # .so/.dylib file
        ARCHIVE DESTINATION ${GR_LIBRARY_DIR} # .lib file
        RUNTIME DESTINATION ${GR_RUNTIME_DIR} # .dll file
    )

    #install the exported target files
    install(
        EXPORT ${GR_LIB_TARGET}-export
        FILE ${GR_LIB_TARGET}Targets.cmake
        COMPONENT CMAKE_EXPORT_CONFIG
        NAMESPACE gnuradio::
        DESTINATION ${GR_CMAKE_DIR})

    set(GR_LIB_EXPORT_DEPENDENCIES ${GR_LIB_EXPORT_DEPS})
    set(GR_LIB_DISTRIBUTION_DEPENDENCIES ${GR_LIB_DISTRIBUTION_DEPS})
    # Generate and install config module
    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/Modules/targetConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/Modules/${GR_LIB_TARGET}Config.cmake
        INSTALL_DESTINATION ${GR_CMAKE_DIR})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/Modules/${GR_LIB_TARGET}Config.cmake
            COMPONENT CMAKE_EXPORT_CONFIG
            DESTINATION ${GR_CMAKE_DIR})

    # Setup install rule for target dependencies for installers
    # this is not neccesary if we're not distributing GR radio as the
    # deps have to exist on the host system already if we're building
    # GR
    if(GR_BUILD_INSTALLER AND GR_LIB_DISTRIBUTION_DEPS)
        GR_BUNDLE_INSTALL(
            TARGET ${GR_LIB_TARGET}
            COMPONENT ${GR_LIB_COMPONENT}
            DEPENDENCIES ${GR_LIB_DISTRIBUTION_DEPS}
        )
    endif()
endfunction()

function(GR_GENERATE_INSTALL)
    foreach(component ${_gr_enabled_components})
        get_property(comp_targets GLOBAL PROPERTY COMPONENT_${component}_TARGETS)
        foreach(tgt ${comp_targets})
            get_property(DIST_DEPS GLOBAL PROPERTY TARGET_${tgt}_DIST_DEPS)
            get_property(EXP_DEPS GLOBAL PROPERTY TARGET_${tgt}_EXPORT_DEPS)
            GR_INSTALL_LIBRARY(
                TARGET ${tgt}
                COMPONENT ${component}
                EXPORT_DEPS ${EXP_DEPS}
                DISTRIBUTION_DEPS ${DIST_DEPS}
            )
        endforeach()
    endforeach()
endfunction()

########################################################################
# Establish install rules for target TARGET for given COMPONENT
# Configure module export interface for TARGET and MODULE_DEPENDENCIES
# TARGET - target to gererate install interface for
# COMPONENT - component to which the target belongs
# MODULE_DEPENDENCIES - CMake module dependencies to generate find_dep calls for
########################################################################
function(GR_CONFIGURE_INSTALLER_COMPONENT name)
    get_property(comp_deps GLOBAL PROPERTY COMPONENT_${name}_DEPS)
    _compute_safe_component_name(${name})
    foreach(comp ${comp_deps})
        _compute_safe_component_name(${comp})
        list(APPEND _safe_comp_deps ${_component_safe_name})
    endforeach()
    get_property(NEEDS_DEPS GLOBAL PROPERTY COMPONENT_${name}_NEEDS_DEPS)
    if(NEEDS_DEPS)
        cpack_add_component(${_component_safe_name}_DEPS
            HIDDEN
        )
        list(APPEND _safe_comp_deps ${_component_safe_name}_DEPS)
    endif()
    cpack_add_component(${_component_safe_name}
                        DISPLAY_NAME ${name}
                        DEPENDS ${_safe_comp_deps}
    )
endfunction()

########################################################################
# Create a dummy custom command that depends on other targets.
# Usage:
#   GR_GEN_TARGET_DEPS(unique_name target_deps <target1> <target2> ...)
#   ADD_CUSTOM_COMMAND(<the usual args> ${target_deps})
#
# Custom command can't depend on targets, but can depend on executables,
# and executables can depend on targets. So this is the process:
########################################################################
function(GR_GEN_TARGET_DEPS name var)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp.in "int main(void){return 0;}\n")
    execute_process(
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp.in
            ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp)
    add_executable(${name} ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp)
    if(ARGN)
        add_dependencies(${name} ${ARGN})
    endif(ARGN)

    if(CMAKE_CROSSCOMPILING)
        set(${var}
            "DEPENDS;${name}"
            PARENT_SCOPE) #can't call command when cross
    else()
        set(${var}
            "DEPENDS;${name};COMMAND;${name}"
            PARENT_SCOPE)
    endif()
endfunction(GR_GEN_TARGET_DEPS)

########################################################################
# Run GRCC to compile .grc files into .py files.
#
# Usage: GRCC(filename, directory)
#    - filenames: List of file name of .grc file
#    - directory: directory of built .py file - usually in
#                 ${CMAKE_CURRENT_BINARY_DIR}
#    - Sets PYFILES: output converted GRC file names to Python files.
########################################################################
function(GRCC)
    # Extract directory from list of args, remove it for the list of filenames.
    list(GET ARGV -1 directory)
    list(REMOVE_AT ARGV -1)
    set(filenames ${ARGV})
    file(MAKE_DIRECTORY ${directory})

    set(GRCC_COMMAND ${PROJECT_SOURCE_DIR}/gr-utils/python/grcc)

    # GRCC uses some stuff in grc and gnuradio-runtime, so we force
    # the known paths here
    list(APPEND PYTHONPATHS ${PROJECT_SOURCE_DIR}
         ${PROJECT_SOURCE_DIR}/gnuradio-runtime/python)

    if(WIN32)
        #SWIG generates the python library files into a subdirectory.
        #Therefore, we must append this subdirectory into PYTHONPATH.
        #Only do this for the python directories matching the following:
        foreach(pydir ${PYTHONPATHS})
            get_filename_component(NAME ${pydir} NAME)
            if(name MATCHES "^(lib|src)$")
                list(APPEND PYTHONPATHS ${pydir}/${CMAKE_BUILD_TYPE})
            endif()
        endforeach(pydir)
    endif(WIN32)

    file(TO_NATIVE_PATH "${PYTHONPATHS}" pypath)

    if(UNIX)
        list(APPEND pypath "$PYTHONPATH")
        string(REPLACE ";" ":" pypath "${pypath}")
        set(ENV{PYTHONPATH} ${pypath})
    endif(UNIX)

    if(WIN32)
        list(APPEND pypath "%PYTHONPATH%")
        string(REPLACE ";" "\\;" pypath "${pypath}")
        #list(APPEND environs "PYTHONPATH=${pypath}")
        set(ENV{PYTHONPATH} ${pypath})
    endif(WIN32)

    foreach(f ${filenames})
        execute_process(COMMAND ${GRCC_COMMAND} -d ${directory} ${f})
        string(REPLACE ".grc" ".py" pyfile "${f}")
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" pyfile
                       "${pyfile}")
        list(APPEND pyfiles ${pyfile})
    endforeach(f)

    set(PYFILES
        ${pyfiles}
        PARENT_SCOPE)
endfunction(GRCC)

########################################################################
# Check if HAVE_PTHREAD_SETSCHEDPARAM and HAVE_SCHED_SETSCHEDULER
#  should be defined
########################################################################
macro(GR_CHECK_LINUX_SCHED_AVAIL)
    set(CMAKE_REQUIRED_LIBRARIES -lpthread)
    check_cxx_source_compiles(
        "
        #include <pthread.h>
        int main(){
            pthread_t pthread;
            pthread_setschedparam(pthread,  0, 0);
            return 0;
        } "
        HAVE_PTHREAD_SETSCHEDPARAM)
    gr_add_cond_def(HAVE_PTHREAD_SETSCHEDPARAM)

    check_cxx_source_compiles(
        "
        #include <sched.h>
        int main(){
            pid_t pid;
            sched_setscheduler(pid, 0, 0);
            return 0;
        } "
        HAVE_SCHED_SETSCHEDULER)
    gr_add_cond_def(HAVE_SCHED_SETSCHEDULER)
    set(CMAKE_REQUIRED_LIBRARIES)
endmacro(GR_CHECK_LINUX_SCHED_AVAIL)

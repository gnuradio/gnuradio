#
# Copyright 2011 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

########################################################################
# Parse the arches xml file:
#  Test each arch to see if the compiler supports the flag.
#  If the test passes append the arch to the available list.
########################################################################
#extract the arch lines from the xml file using crazy python
EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c
    "from xml.dom import minidom; print ';'.join(map(lambda a: '%s %s'%(a.attributes['name'].value,a.getElementsByTagName('flag')[0].firstChild.data),minidom.parse('${CMAKE_SOURCE_DIR}/gen/archs.xml').getElementsByTagName('arch')))"
    OUTPUT_VARIABLE arch_lines OUTPUT_STRIP_TRAILING_WHITESPACE
)

#get any mutually exclusive archs so we can exclude them
#this is really for compilers which can do both 32- and 64-bit compilations.
EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c
    "from xml.dom import minidom; print ';'.join(map(lambda a: '%s %s'%(a.parentNode.attributes['name'].value,a.firstChild.data),minidom.parse('../gen/archs.xml').getElementsByTagName('mutex')))"
    OUTPUT_VARIABLE mutex_lines OUTPUT_STRIP_TRAILING_WHITESPACE
)

#This macro sets the ${arch}_flag variable,
#and handles special cases for MSVC arch flags.
MACRO(set_arch_flag name flag)
    IF(MSVC AND ${name} STREQUAL "mmx")
        SET(${name}_flag "/arch:SSE") #no /arch:MMX
    ELSEIF(MSVC AND ${name} STREQUAL "sse")
        SET(${name}_flag "/arch:SSE")
    ELSEIF(MSVC AND ${name} STREQUAL "sse2")
        SET(${name}_flag "/arch:SSE2")
    ELSE()
        SET(${name}_flag -${flag})
    ENDIF()
ENDMACRO(set_arch_flag)

MACRO(handle_arch name flag)

    #handle special case for none flag
    IF(${flag} STREQUAL "none")
        SET(have_${name} TRUE)

    #otherwise test the flag against the compiler
    ELSE()
        INCLUDE(CheckCXXCompilerFlag)
        set_arch_flag(${name} ${flag})
        CHECK_CXX_COMPILER_FLAG(${${name}_flag} have_${name})
    ENDIF()

    IF(have_${name})
        LIST(APPEND available_arches ${name})
    ENDIF()
ENDMACRO(handle_arch)

MACRO(remove_mutex name mutex)
    IF(have_${name})
        UNSET(have_${mutex})
    ENDIF()
    LIST(REMOVE_ITEM available_arches ${mutex})
ENDMACRO(remove_mutex)

#create a list of available arches
FOREACH(arch_line ${arch_lines})
    SEPARATE_ARGUMENTS(args UNIX_COMMAND "${arch_line}")
    handle_arch(${args})
ENDFOREACH(arch_line)

#strip out mutex archs
FOREACH(mutex_line ${mutex_lines})
    SEPARATE_ARGUMENTS(args UNIX_COMMAND "${mutex_line}")
    remove_mutex(${args})
ENDFOREACH(mutex_line)

MESSAGE(STATUS "Available arches: ${available_arches}")

########################################################################
# Parse the machines xml file:
#  Test each machine to see if its arch dependencies are supported.
#  Build a list of supported machines and the machine definitions.
########################################################################
#extract the machine lines from the xml file using crazy python
EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c
    "from xml.dom import minidom; print ';'.join(map(lambda a: '%s %s'%(a.attributes['name'].value,a.getElementsByTagName('archs')[0].firstChild.data),minidom.parse('${CMAKE_SOURCE_DIR}/gen/machines.xml').getElementsByTagName('machine')))"
    OUTPUT_VARIABLE machine_lines OUTPUT_STRIP_TRAILING_WHITESPACE
)

MACRO(handle_machine1 name)
    UNSET(machine_flags)
    STRING(TOUPPER LV_MACHINE_${name} machine_def)

    #check if all the arches are supported
    FOREACH(arch ${ARGN})
        SET(is_match ${have_${arch}})
        IF(NOT is_match)
            SET(is_match FALSE)
            BREAK()
        ENDIF(NOT is_match)
        SET(machine_flags "${machine_flags} ${${arch}_flag}")
    ENDFOREACH(arch)

    IF(is_match)
        #this is a match, append the source and set its flags
        SET(machine_source ${CMAKE_CURRENT_BINARY_DIR}/volk_machine_${name}.c)
        SET_SOURCE_FILES_PROPERTIES(${machine_source} PROPERTIES COMPILE_FLAGS ${machine_flags})
        LIST(APPEND machine_sources ${machine_source})
        LIST(APPEND machine_defs ${machine_def})
        LIST(APPEND available_machines ${name})
    ENDIF()
ENDMACRO(handle_machine1)

MACRO(handle_machine name)
    SET(arches ${ARGN})
    LIST(FIND arches "32|64" index)
    IF(${index} EQUAL -1)
        handle_machine1(${name} ${arches})
    ELSE()
        LIST(REMOVE_ITEM arches "32|64")
        handle_machine1(${name}_32 32 ${arches})
        handle_machine1(${name}_64 64 ${arches})
    ENDIF()
ENDMACRO(handle_machine)

#setup the available machines
FOREACH(machine_line ${machine_lines})
    SEPARATE_ARGUMENTS(args UNIX_COMMAND "${machine_line}")
    handle_machine(${args})
ENDFOREACH(machine_line)

MESSAGE(STATUS "Available machines: ${available_machines}")

########################################################################
# Create rules to run the volk generator
########################################################################
#list of the generated sources
SET(volk_gen_sources
    ${CMAKE_BINARY_DIR}/include/volk/volk.h
    ${CMAKE_BINARY_DIR}/lib/volk.c
    ${CMAKE_BINARY_DIR}/lib/volk_init.h
    ${CMAKE_BINARY_DIR}/include/volk/volk_typedefs.h
    ${CMAKE_BINARY_DIR}/include/volk/volk_cpu.h
    ${CMAKE_BINARY_DIR}/lib/volk_cpu.c
    ${CMAKE_BINARY_DIR}/include/volk/volk_config_fixed.h
    ${CMAKE_BINARY_DIR}/lib/volk_environment_init.c
    ${CMAKE_BINARY_DIR}/lib/volk_environment_init.h
    ${CMAKE_BINARY_DIR}/lib/volk_machines.h
    ${CMAKE_BINARY_DIR}/lib/volk_machines.c
    ${machine_sources}
)

#dependencies are all python, xml, and header implementation files
FILE(GLOB xml_files ${CMAKE_SOURCE_DIR}/gen/*.xml)
FILE(GLOB py_files ${CMAKE_SOURCE_DIR}/gen/*.py)
FILE(GLOB h_files ${CMAKE_SOURCE_DIR}/include/volk/*.h)

ADD_CUSTOM_COMMAND(
    OUTPUT ${volk_gen_sources}
    DEPENDS ${xml_files} ${py_files} ${h_files}
    COMMAND ${PYTHON_EXECUTABLE} -B
        ${CMAKE_SOURCE_DIR}/gen/volk_register.py
        ${CMAKE_BINARY_DIR}
)

########################################################################
# Handle orc support
########################################################################
FIND_PACKAGE(PkgConfig)
IF(PKG_CONFIG_FOUND)
PKG_CHECK_MODULES(ORC "orc-0.4")
ENDIF(PKG_CONFIG_FOUND)

FIND_PROGRAM(ORCC_EXECUTABLE orcc)

IF(ORC_FOUND AND ORCC_EXECUTABLE)
    #setup orc library usage
    INCLUDE_DIRECTORIES(${ORC_INCLUDE_DIRS})
    LINK_DIRECTORIES(${ORC_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DLV_HAVE_ORC)

    #setup orc functions
    FILE(GLOB orc_files ${CMAKE_SOURCE_DIR}/orc/*.orc)
    FOREACH(orc_file ${orc_files})

        #extract the name for the generated c source from the orc file
        GET_FILENAME_COMPONENT(orc_file_name_we ${orc_file} NAME_WE)
        SET(orcc_gen ${CMAKE_CURRENT_BINARY_DIR}/${orc_file_name_we}.c)

        #create a rule to generate the source and add to the list of sources
        ADD_CUSTOM_COMMAND(
            COMMAND ${ORCC_EXECUTABLE} --implementation -o ${orcc_gen} ${orc_file}
            DEPENDS ${orc_file} OUTPUT ${orcc_gen}
        )
        LIST(APPEND volk_sources ${orcc_gen})

    ENDFOREACH(orc_file)
ELSE()
    MESSAGE(STATUS "Did not find liborc and orcc, disabling orc support...")
ENDIF()

########################################################################
# Setup the volk sources list and library
########################################################################
IF(NOT WIN32)
    ADD_DEFINITIONS(-fvisibility=hidden)
ENDIF()

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

LIST(APPEND volk_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/volk_prefs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/volk_rank_archs.c
    ${volk_gen_sources}
)

#set the machine definitions where applicable
SET_SOURCE_FILES_PROPERTIES(
    ${CMAKE_CURRENT_BINARY_DIR}/volk.c
    ${CMAKE_CURRENT_BINARY_DIR}/volk_machines.c
PROPERTIES COMPILE_DEFINITIONS "${machine_defs}")

IF(MSVC)
    #add compatibility includes for stdint types
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/msvc)
    #compile the sources as C++ due to the lack of complex.h under MSVC
    SET_SOURCE_FILES_PROPERTIES(${volk_sources} PROPERTIES LANGUAGE CXX)
ENDIF(MSVC)

#create the volk runtime library
ADD_LIBRARY(volk SHARED ${volk_sources})
TARGET_LINK_LIBRARIES(volk ${ORC_LIBRARIES})
SET_TARGET_PROPERTIES(volk PROPERTIES SOVERSION ${LIBVER})
SET_TARGET_PROPERTIES(volk PROPERTIES DEFINE_SYMBOL "volk_EXPORTS")

INSTALL(TARGETS volk
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin # .dll file
)

########################################################################
# Build the QA test application
########################################################################
FIND_PACKAGE(Boost COMPONENTS unit_test_framework)

IF(Boost_FOUND)

SET_SOURCE_FILES_PROPERTIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/testqa.cc PROPERTIES
    COMPILE_DEFINITIONS "BOOST_TEST_DYN_LINK;BOOST_TEST_MAIN"
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

ADD_EXECUTABLE(test_all
    ${CMAKE_CURRENT_SOURCE_DIR}/testqa.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_utils.cc
)
TARGET_LINK_LIBRARIES(test_all volk ${Boost_LIBRARIES})
#ADD_TEST(qa_volk_test_all test_all)

ENDIF()

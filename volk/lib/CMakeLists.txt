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
# Setup the compiler name
########################################################################
set(COMPILER_NAME ${CMAKE_C_COMPILER_ID})
if(MSVC) #its not set otherwise
    set(COMPILER_NAME MSVC)
endif()

if(NOT DEFINED COMPILER_NAME)
    message(FATAL_ERROR "COMPILER_NAME undefined. Volk build may not support this compiler.")
endif()

########################################################################
# Parse the arches xml file:
#  Test each arch to see if the compiler supports the flag.
#  If the test passes append the arch to the available list.
########################################################################
#extract the compiler lines from the xml file using abusive python



execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c
    "from xml.dom import minidom; print ';'.join(map(lambda b: ','.join([','.join([b.attributes['name'].value,item.attributes['name'].value,item.firstChild.data]) for item in b.getElementsByTagName('remap')]), minidom.parse('${CMAKE_SOURCE_DIR}/gen/compilers.xml').getElementsByTagName('compiler')))"

    OUTPUT_VARIABLE compiler_lines OUTPUT_STRIP_TRAILING_WHITESPACE
)

foreach(thing ${compiler_lines})
    string(REGEX REPLACE "," ";" thing_list ${thing})
    list(FIND thing_list ${COMPILER_NAME} check_val)
    if(NOT ("${check_val}" STREQUAL "-1"))
        string(REGEX REPLACE "${COMPILER_NAME}," ";" filter_string ${thing})
    endif()
endforeach()


#extract compiler prefixes from the xml file using abusive python
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c
    "from xml.dom import minidom; print ';'.join(map(lambda b: ','.join([','.join([b.attributes['name'].value,item.firstChild.data]) for item in b.getElementsByTagName('prefix')]), minidom.parse('${CMAKE_SOURCE_DIR}/gen/compilers.xml').getElementsByTagName('compiler')))"

    OUTPUT_VARIABLE compiler_prefixes OUTPUT_STRIP_TRAILING_WHITESPACE
)

foreach(thing ${compiler_prefixes})
    string(REGEX REPLACE "," ";" thing_list ${thing})
    list(FIND thing_list ${COMPILER_NAME} check_val)
    if(NOT ("${check_val}" STREQUAL "-1"))
        list(GET thing_list "1" prefix)
    endif()
endforeach()




#extract the arch lines from the xml file using abusive python
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c
    "from xml.dom import minidom; print ';'.join(map(lambda a: '%s %s %s %s'%(a.attributes['name'].value,a.getElementsByTagName('flag')[0].firstChild.data,a.getElementsByTagName('overrule')[0].firstChild.data,a.getElementsByTagName('overrule_val')[0].firstChild.data) if (len(a.getElementsByTagName('overrule'))) else '%s %s %s %s'%(a.attributes['name'].value,a.getElementsByTagName('flag')[0].firstChild.data,'no_overrule', 'no_overrule_val'), minidom.parse('${CMAKE_SOURCE_DIR}/gen/archs.xml').getElementsByTagName('arch')))"

    OUTPUT_VARIABLE arch_lines OUTPUT_STRIP_TRAILING_WHITESPACE
)




#set the various overrule values (see archs.xml)
#a lot of this is translating between automake and cmake
if(NOT "${CROSSCOMPILE_MULTILIB}" STREQUAL "true")
    set(MD_SUBCPU ${CMAKE_SYSTEM_PROCESSOR})
    #detect 32 or 64 bit compiler
    if(MD_SUBCPU MATCHES "^(i.86|x86|x86_64|amd64)$")
        include(CheckTypeSize)
        check_type_size("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)
        if (${SIZEOF_VOID_P} EQUAL 8)
            set(MD_SUBCPU x86_64)
        else()
            set(MD_SUBCPU x86)
        endif()
    endif()
endif()
if(NOT "${ORC_FOUND}" STREQUAL "TRUE")
    set(LV_HAVE_ORC "no")
endif()






macro(compiler_filter name flag)
    set(filtered_flag ${flag})
    foreach(thing ${filter_string})
        string(REGEX REPLACE "," ";" flagmap ${thing})
        list(GET flagmap "0" key)
        list(GET flagmap "1" val)
        string(REGEX MATCH "^${key}$" found ${flag})
        if("${found}" STREQUAL "${key}")
            string(REGEX REPLACE "^${key}$" "${val}" filtered_flag ${flag})
        endif()
    endforeach()
    set(${name}_flag "${prefix}${filtered_flag}")
endmacro()







macro(handle_arch name flag overrule overrule_val)

    #handle overrule
    if("${${overrule}}" STREQUAL "${overrule_val}")
        set(have_${name} FALSE)
        message(STATUS "${name} overruled")
    #handle special case for none flag
    elseif(${flag} STREQUAL "none")
        set(have_${name} TRUE)
    #otherwise test the flag(s) against the compiler
    else()
        include(CheckCXXCompilerFlag)
        string(REGEX REPLACE "," ";" flag_list ${flag})
        set(have_${name} 1)
        foreach(thing ${flag_list})
            compiler_filter(${name} ${thing})
            CHECK_CXX_COMPILER_FLAG(${${name}_flag} have_${thing})
            if(NOT (${have_${name}} AND ("${have_${thing}}" STREQUAL "1")))
                set(have_${name} 0)
            endif()
        endforeach()
    endif()

    if(have_${name})
        list(APPEND available_arches ${name})
    endif()

endmacro(handle_arch)

#create a list of available arches
foreach(arch_line ${arch_lines})
    string(REPLACE " " ";" args "${arch_line}")
    handle_arch(${args})
endforeach(arch_line)

message(STATUS "Available arches: ${available_arches}")

########################################################################
# Parse the machines xml file:
#  Test each machine to see if its arch dependencies are supported.
#  Build a list of supported machines and the machine definitions.
########################################################################
#extract the machine lines from the xml file using crazy python
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c
    "from xml.dom import minidom; print ';'.join(map(lambda a: '%s %s'%(a.attributes['name'].value,a.getElementsByTagName('archs')[0].firstChild.data),minidom.parse('${CMAKE_SOURCE_DIR}/gen/machines.xml').getElementsByTagName('machine')))"
    OUTPUT_VARIABLE machine_lines OUTPUT_STRIP_TRAILING_WHITESPACE
)

macro(handle_machine1 name)
    unset(machine_flags)
    string(TOUPPER LV_MACHINE_${name} machine_def)

    #check if all the arches are supported
    foreach(arch ${ARGN})
        set(is_match ${have_${arch}})
        if(NOT is_match)
            set(is_match FALSE)
            break()
        endif(NOT is_match)
        set(machine_flags "${machine_flags} ${${arch}_flag}")
    endforeach(arch)

    string(REGEX REPLACE "^[ \t]+" "" machine_flags "${machine_flags}")

    if(is_match)
        #this is a match, append the source and set its flags
        set(machine_source ${CMAKE_CURRENT_BINARY_DIR}/volk_machine_${name}.c)
        set_source_files_properties(${machine_source} PROPERTIES COMPILE_FLAGS "${machine_flags}")
        list(APPEND machine_sources ${machine_source})
        list(APPEND machine_defs ${machine_def})
        list(APPEND available_machines ${name})
    endif()
endmacro(handle_machine1)

macro(handle_machine name)
    set(arches ${ARGN})
    list(FIND arches "32|64" index)
    if(${index} EQUAL -1)
        handle_machine1(${name} ${arches})
    else()
        list(REMOVE_ITEM arches "32|64")
        handle_machine1(${name}_32 32 ${arches})
        handle_machine1(${name}_64 64 ${arches})
    endif()
endmacro(handle_machine)

#setup the available machines
foreach(machine_line ${machine_lines})
    string(REPLACE " " ";" args "${machine_line}")
    handle_machine(${args})
endforeach(machine_line)

message(STATUS "Available machines: ${available_machines}")

########################################################################
# Create rules to run the volk generator
########################################################################
#list of the generated sources
set(volk_gen_sources
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
file(GLOB xml_files ${CMAKE_SOURCE_DIR}/gen/*.xml)
file(GLOB py_files ${CMAKE_SOURCE_DIR}/gen/*.py)
file(GLOB h_files ${CMAKE_SOURCE_DIR}/include/volk/*.h)

add_custom_command(
    OUTPUT ${volk_gen_sources}
    DEPENDS ${xml_files} ${py_files} ${h_files}
    COMMAND ${PYTHON_EXECUTABLE} -B
        ${CMAKE_SOURCE_DIR}/gen/volk_register.py
        ${CMAKE_BINARY_DIR}
)

########################################################################
# Set local include directories first
########################################################################
include_directories(
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

########################################################################
# Handle orc support
########################################################################
if(ORC_FOUND)
    #setup orc library usage
    include_directories(${ORC_INCLUDE_DIRS})
    link_directories(${ORC_LIBRARY_DIRS})
    add_definitions(-DLV_HAVE_ORC)

    #setup orc functions
    file(GLOB orc_files ${CMAKE_SOURCE_DIR}/orc/*.orc)
    foreach(orc_file ${orc_files})

        #extract the name for the generated c source from the orc file
        get_filename_component(orc_file_name_we ${orc_file} NAME_WE)
        set(orcc_gen ${CMAKE_CURRENT_BINARY_DIR}/${orc_file_name_we}.c)

        #create a rule to generate the source and add to the list of sources
        add_custom_command(
            COMMAND ${ORCC_EXECUTABLE} --include math.h --implementation -o ${orcc_gen} ${orc_file}
            DEPENDS ${orc_file} OUTPUT ${orcc_gen}
        )
        list(APPEND volk_sources ${orcc_gen})

    endforeach(orc_file)
else()
    message(STATUS "Did not find liborc and orcc, disabling orc support...")
endif()

########################################################################
# Setup the volk sources list and library
########################################################################
if(NOT WIN32)
    add_definitions(-fvisibility=hidden)
endif()

list(APPEND volk_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/volk_prefs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/volk_rank_archs.c
    ${volk_gen_sources}
)

#set the machine definitions where applicable
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/volk.c
    ${CMAKE_CURRENT_BINARY_DIR}/volk_machines.c
PROPERTIES COMPILE_DEFINITIONS "${machine_defs}")

if(MSVC)
    #add compatibility includes for stdint types
    include_directories(${CMAKE_SOURCE_DIR}/msvc)
    #compile the sources as C++ due to the lack of complex.h under MSVC
    set_source_files_properties(${volk_sources} PROPERTIES LANGUAGE CXX)
endif()

#create the volk runtime library
add_library(volk SHARED ${volk_sources})
if(ORC_FOUND)
    target_link_libraries(volk ${ORC_LIBRARIES})
endif(ORC_FOUND)
set_target_properties(volk PROPERTIES SOVERSION ${LIBVER})
set_target_properties(volk PROPERTIES DEFINE_SYMBOL "volk_EXPORTS")

install(TARGETS volk
    LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT "volk_runtime" # .so file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "volk_devel"   # .lib file
    RUNTIME DESTINATION bin              COMPONENT "volk_runtime" # .dll file
)

########################################################################
# Build the QA test application
########################################################################


if(Boost_FOUND)

    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/testqa.cc PROPERTIES
        COMPILE_DEFINITIONS "BOOST_TEST_DYN_LINK;BOOST_TEST_MAIN"
    )

    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})

    add_executable(test_all
        ${CMAKE_CURRENT_SOURCE_DIR}/testqa.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/qa_utils.cc
    )
    target_link_libraries(test_all volk ${Boost_LIBRARIES})
    add_test(qa_volk_test_all test_all)

endif(Boost_FOUND)


#
# Copyright 2011-2012 Free Software Foundation, Inc.
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
# header file detection
########################################################################
include(CheckIncludeFile)
CHECK_INCLUDE_FILE(cpuid.h HAVE_CPUID_H)
if(HAVE_CPUID_H)
    add_definitions(-DHAVE_CPUID_H)
endif()

CHECK_INCLUDE_FILE(intrin.h HAVE_INTRIN_H)
if(HAVE_INTRIN_H)
    add_definitions(-DHAVE_INTRIN_H)
endif()

CHECK_INCLUDE_FILE(fenv.h HAVE_FENV_H)
if(HAVE_FENV_H)
    add_definitions(-DHAVE_FENV_H)
endif()

CHECK_INCLUDE_FILE(dlfcn.h HAVE_DLFCN_H)
if(HAVE_DLFCN_H)
    add_definitions(-DHAVE_DLFCN_H)
    list(APPEND volk_libraries ${CMAKE_DL_LIBS})
endif()

########################################################################
# Setup the compiler name
########################################################################
set(COMPILER_NAME ${CMAKE_C_COMPILER_ID})
if(MSVC) #its not set otherwise
    set(COMPILER_NAME MSVC)
endif()

message(STATUS "Compiler name: ${COMPILER_NAME}")

if(NOT DEFINED COMPILER_NAME)
    message(FATAL_ERROR "COMPILER_NAME undefined. Volk build may not support this compiler.")
endif()

########################################################################
# detect x86 flavor of CPU
########################################################################
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^(i.86|x86|x86_64|amd64)$")
    message(STATUS "x86* CPU detected")
    set(CPU_IS_x86 TRUE)
endif()

########################################################################
# determine passing architectures based on compile flag tests
########################################################################
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_SOURCE_DIR}/gen/volk_compile_utils.py
    --mode "arch_flags" --compiler "${COMPILER_NAME}"
    OUTPUT_VARIABLE arch_flag_lines OUTPUT_STRIP_TRAILING_WHITESPACE
)

macro(check_arch arch_name)
    set(flags ${ARGN})
    set(have_${arch_name} TRUE)
    foreach(flag ${flags})
        include(CheckCXXCompilerFlag)
        set(have_flag have${flag})
        execute_process( #make the have_flag have nice alphanum chars (just for looks/not necessary)
            COMMAND ${PYTHON_EXECUTABLE} -c "import re; print(re.sub('\\W', '_', '${have_flag}'))"
            OUTPUT_VARIABLE have_flag OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        CHECK_CXX_COMPILER_FLAG(${flag} ${have_flag})
        if (NOT ${have_flag})
            set(have_${arch_name} FALSE)
        endif()
    endforeach(flag)
    if (have_${arch_name})
        list(APPEND available_archs ${arch_name})
    endif()
endmacro(check_arch)

foreach(line ${arch_flag_lines})
    string(REGEX REPLACE "," ";" arch_flags ${line})
    check_arch(${arch_flags})
endforeach(line)

macro(OVERRULE_ARCH arch reason)
    message(STATUS "${reason}, Overruled arch ${arch}")
    list(REMOVE_ITEM available_archs ${arch})
endmacro(OVERRULE_ARCH)

########################################################################
# eliminate AVX on GCC < 4.4
# even though it accepts -mavx, as won't assemble xgetbv, which we need
########################################################################
if(CPU_IS_x86 AND COMPILER_NAME MATCHES "GNU")
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
        OUTPUT_VARIABLE GCC_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(GCC_VERSION VERSION_LESS "4.4")
        OVERRULE_ARCH(avx "GCC missing xgetbv")
    endif()
endif()

########################################################################
# implement overruling in the ORC case,
# since ORC always passes flag detection
########################################################################
if(NOT ORC_FOUND)
    OVERRULE_ARCH(orc "ORC support not found")
endif()

########################################################################
# implement overruling in the non-multilib case
# this makes things work when both -m32 and -m64 pass
########################################################################
if(NOT CROSSCOMPILE_MULTILIB AND CPU_IS_x86)
    include(CheckTypeSize)
    check_type_size("void*[8]" SIZEOF_CPU BUILTIN_TYPES_ONLY)
    if (${SIZEOF_CPU} EQUAL 64)
        OVERRULE_ARCH(32 "CPU width is 64 bits")
    endif()
    if (${SIZEOF_CPU} EQUAL 32)
        OVERRULE_ARCH(64 "CPU width is 32 bits")
    endif()
endif()

########################################################################
# done overrules! print the result
########################################################################
message(STATUS "Available architectures: ${available_archs}")

########################################################################
# determine available machines given the available architectures
########################################################################
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_SOURCE_DIR}/gen/volk_compile_utils.py
    --mode "machines" --archs "${available_archs}"
    OUTPUT_VARIABLE available_machines OUTPUT_STRIP_TRAILING_WHITESPACE
)

########################################################################
# Implement machine overruling for redundant machines:
# A machine is redundant when expansion rules occur,
# and the arch superset passes configuration checks.
# When this occurs, eliminate the redundant machines
# to avoid unnecessary compilation of subset machines.
########################################################################
foreach(arch orc 64 32)
    foreach(machine_name ${available_machines})
        string(REPLACE "_${arch}" "" machine_name_no_arch ${machine_name})
        if (${machine_name} STREQUAL ${machine_name_no_arch})
        else()
            list(REMOVE_ITEM available_machines ${machine_name_no_arch})
        endif()
    endforeach(machine_name)
endforeach(arch)

########################################################################
# done overrules! print the result
########################################################################
message(STATUS "Available machines: ${available_machines}")

########################################################################
# Create rules to run the volk generator
########################################################################

#dependencies are all python, xml, and header implementation files
file(GLOB xml_files ${CMAKE_SOURCE_DIR}/gen/*.xml)
file(GLOB py_files ${CMAKE_SOURCE_DIR}/gen/*.py)
file(GLOB h_files ${CMAKE_SOURCE_DIR}/include/volk/*.h)

macro(gen_template tmpl output)
    list(APPEND volk_gen_sources ${output})
    add_custom_command(
        OUTPUT ${output}
        DEPENDS ${xml_files} ${py_files} ${h_files} ${tmpl}
        COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
        ${CMAKE_SOURCE_DIR}/gen/volk_tmpl_utils.py
        --input ${tmpl} --output ${output} ${ARGN}
    )
endmacro(gen_template)

make_directory(${CMAKE_BINARY_DIR}/include/volk)

gen_template(${CMAKE_SOURCE_DIR}/tmpl/volk.tmpl.h              ${CMAKE_BINARY_DIR}/include/volk/volk.h)
gen_template(${CMAKE_SOURCE_DIR}/tmpl/volk.tmpl.c              ${CMAKE_BINARY_DIR}/lib/volk.c)
gen_template(${CMAKE_SOURCE_DIR}/tmpl/volk_typedefs.tmpl.h     ${CMAKE_BINARY_DIR}/include/volk/volk_typedefs.h)
gen_template(${CMAKE_SOURCE_DIR}/tmpl/volk_cpu.tmpl.h          ${CMAKE_BINARY_DIR}/include/volk/volk_cpu.h)
gen_template(${CMAKE_SOURCE_DIR}/tmpl/volk_cpu.tmpl.c          ${CMAKE_BINARY_DIR}/lib/volk_cpu.c)
gen_template(${CMAKE_SOURCE_DIR}/tmpl/volk_config_fixed.tmpl.h ${CMAKE_BINARY_DIR}/include/volk/volk_config_fixed.h)
gen_template(${CMAKE_SOURCE_DIR}/tmpl/volk_machines.tmpl.h     ${CMAKE_BINARY_DIR}/lib/volk_machines.h)
gen_template(${CMAKE_SOURCE_DIR}/tmpl/volk_machines.tmpl.c     ${CMAKE_BINARY_DIR}/lib/volk_machines.c)

foreach(machine_name ${available_machines})
    #generate machine source
    set(machine_source ${CMAKE_CURRENT_BINARY_DIR}/volk_machine_${machine_name}.c)
    gen_template(${CMAKE_SOURCE_DIR}/tmpl/volk_machine_xxx.tmpl.c ${machine_source} ${machine_name})

    #determine machine flags
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
        ${CMAKE_SOURCE_DIR}/gen/volk_compile_utils.py
        --mode "machine_flags" --machine "${machine_name}" --compiler "${COMPILER_NAME}"
        OUTPUT_VARIABLE ${machine_name}_flags OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(${machine_name}_flags)
        set_source_files_properties(${machine_source} PROPERTIES COMPILE_FLAGS "${${machine_name}_flags}")
    endif()

    #add to available machine defs
    string(TOUPPER LV_MACHINE_${machine_name} machine_def)
    list(APPEND machine_defs ${machine_def})
endforeach(machine_name)

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
    list(APPEND volk_libraries ${ORC_LIBRARIES})

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
    include_directories(${CMAKE_SOURCE_DIR}/cmake/msvc)
    add_definitions(-DHAVE_CONFIG_H)
    #compile the sources as C++ due to the lack of complex.h under MSVC
    set_source_files_properties(${volk_sources} PROPERTIES LANGUAGE CXX)
endif()

#create the volk runtime library
add_library(volk SHARED ${volk_sources})
target_link_libraries(volk ${volk_libraries})
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


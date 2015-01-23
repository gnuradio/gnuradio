#
# Copyright 2011-2012,2014 Free Software Foundation, Inc.
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
# Special clang flag so flag checks can fail
########################################################################
if(COMPILER_NAME MATCHES "GNU")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-Werror=unused-command-line-argument" HAVE_WERROR_UNUSED_CMD_LINE_ARG)
    if(HAVE_WERROR_UNUSED_CMD_LINE_ARG)
        set(VOLK_FLAG_CHECK_FLAGS "-Werror=unused-command-line-argument")
    endif()
endif()

########################################################################
# check for posix_memalign, since some OSs do not internally define
# _XOPEN_SOURCE or _POSIX_C_SOURCE; they leave this to the user.
########################################################################

include(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(posix_memalign HAVE_POSIX_MEMALIGN)

if(HAVE_POSIX_MEMALIGN)
      add_definitions(-DHAVE_POSIX_MEMALIGN)
endif(HAVE_POSIX_MEMALIGN)

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
        if(VOLK_FLAG_CHECK_FLAGS)
            set(CMAKE_REQUIRED_FLAGS ${VOLK_FLAG_CHECK_FLAGS})
        endif()
        CHECK_CXX_COMPILER_FLAG(${flag} ${have_flag})
        unset(CMAKE_REQUIRED_FLAGS)
        if (NOT ${have_flag})
            set(have_${arch_name} FALSE)
        endif()
    endforeach()
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
# eliminate AVX on if not on x86, or if the compiler does not accept
# the xgetbv instruction, or {if not cross-compiling and the xgetbv
# executable does not function correctly}.
########################################################################
set(HAVE_XGETBV 0)
set(HAVE_AVX_CVTPI32_PS 0)
if(CPU_IS_x86)
    # check to see if the compiler/linker works with xgetb instruction
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/test_xgetbv.c "unsigned long long _xgetbv(unsigned int index) { unsigned int eax, edx; __asm__ __volatile__(\"xgetbv\" : \"=a\"(eax), \"=d\"(edx) : \"c\"(index)); return ((unsigned long long)edx << 32) | eax; } int main (void) { (void) _xgetbv(0); return (0); }")
    execute_process(COMMAND ${CMAKE_C_COMPILER} -o
        ${CMAKE_CURRENT_BINARY_DIR}/test_xgetbv
        ${CMAKE_CURRENT_BINARY_DIR}/test_xgetbv.c
        OUTPUT_QUIET ERROR_QUIET
        RESULT_VARIABLE avx_compile_result)
    if(NOT ${avx_compile_result} EQUAL 0)
        OVERRULE_ARCH(avx "Compiler or linker missing xgetbv instruction")
    elseif(NOT CROSSCOMPILE_MULTILIB)
        execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_xgetbv
            OUTPUT_QUIET ERROR_QUIET
            RESULT_VARIABLE avx_exe_result)
        if(NOT ${avx_exe_result} EQUAL 0)
            OVERRULE_ARCH(avx "CPU missing xgetbv")
        else()
            set(HAVE_XGETBV 1)
        endif()
    else()
        # cross compiling and compiler/linker seems to work; assume working
        set(HAVE_XGETBV 1)
    endif()
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/test_xgetbv
        ${CMAKE_CURRENT_BINARY_DIR}/test_xgetbv.c)

    #########################################################################
    # eliminate AVX if cvtpi32_ps intrinsic fails like some versions of clang
    #########################################################################

    # check to see if the compiler/linker works with cvtpi32_ps instrinsic when using AVX
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/test_cvtpi32_ps.c "#include <immintrin.h>\nint main (void) {__m128 __a; __m64 __b; __m128 foo = _mm_cvtpi32_ps(__a, __b); return (0); }")
    execute_process(COMMAND ${CMAKE_C_COMPILER} -mavx -o
      ${CMAKE_CURRENT_BINARY_DIR}/test_cvtpi32_ps
      ${CMAKE_CURRENT_BINARY_DIR}/test_cvtpi32_ps.c
      OUTPUT_QUIET ERROR_QUIET
      RESULT_VARIABLE avx_compile_result)
    if(NOT ${avx_compile_result} EQUAL 0)
      OVERRULE_ARCH(avx "Compiler missing cvtpi32_ps instrinsic")
    elseif(NOT CROSSCOMPILE_MULTILIB)
      execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_cvtpi32_ps
        OUTPUT_QUIET ERROR_QUIET
        RESULT_VARIABLE avx_exe_result)
      if(NOT ${avx_exe_result} EQUAL 0)
        OVERRULE_ARCH(avx "CPU missing cvtpi32_ps")
      else()
        set(HAVE_AVX_CVTPI32_PS 1)
      endif()
    else()
      set(HAVE_AVX_CVTPI32_PS 1)
    endif()
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/test_cvtpi32_ps
      ${CMAKE_CURRENT_BINARY_DIR}/test_cvtpi32_ps.c)

    # Disable SSE4a if Clang is less than version 3.2
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
      # Figure out the version of Clang
      if(CMAKE_VERSION VERSION_LESS "2.8.10")
        # Exctract the Clang version from the --version string.
        # In cmake 2.8.10, we can just use CMAKE_C_COMPILER_VERSION
        # without having to go through these string manipulations
        execute_process(COMMAND ${CMAKE_C_COMPILER} --version
          OUTPUT_VARIABLE clang_version)
        string(REGEX MATCH "[0-9].[0-9]" CMAKE_C_COMPILER_VERSION ${clang_version})
      endif(CMAKE_VERSION VERSION_LESS "2.8.10")

      if(CMAKE_C_COMPILER_VERSION VERSION_LESS "3.2")
        OVERRULE_ARCH(sse4_a "Clang >= 3.2 required for SSE4a")
      endif(CMAKE_C_COMPILER_VERSION VERSION_LESS "3.2")
    endif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")

endif(CPU_IS_x86)

if(${HAVE_XGETBV})
    add_definitions(-DHAVE_XGETBV)
endif()

if(${HAVE_AVX_CVTPI32_PS})
    add_definitions(-DHAVE_AVX_CVTPI32_PS)
endif()

########################################################################
# if the CPU is not x86, eliminate all Intel SIMD
########################################################################

if(NOT CPU_IS_x86)
    OVERRULE_ARCH(3dnow "Architecture is not x86 or x86_64")
    OVERRULE_ARCH(mmx "Architecture is not x86 or x86_64")
    OVERRULE_ARCH(sse "Architecture is not x86 or x86_64")
    OVERRULE_ARCH(sse2 "Architecture is not x86 or x86_64")
    OVERRULE_ARCH(sse3 "Architecture is not x86 or x86_64")
    OVERRULE_ARCH(ssse3 "Architecture is not x86 or x86_64")
    OVERRULE_ARCH(sse4_a "Architecture is not x86 or x86_64")
    OVERRULE_ARCH(sse4_1 "Architecture is not x86 or x86_64")
    OVERRULE_ARCH(sse4_2 "Architecture is not x86 or x86_64")
    OVERRULE_ARCH(avx "Architecture is not x86 or x86_64")
endif(NOT CPU_IS_x86)

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

    #MSVC 64 bit does not have MMX, overrule it
    if (${SIZEOF_CPU} EQUAL 64 AND MSVC)
        OVERRULE_ARCH(mmx "No MMX for Win64")
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
foreach(arch mmx orc 64 32)
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
file(GLOB h_files ${CMAKE_SOURCE_DIR}/kernels/volk/*.h)

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

set(BASE_CFLAGS NONE)
STRING(TOUPPER ${CMAKE_BUILD_TYPE} CBTU)
MESSAGE(STATUS BUILT TYPE ${CBTU})
MESSAGE(STATUS "Base cflags = ${CMAKE_C_FLAGS_${CBTU}} ${CMAKE_C_FLAGS}")
set(COMPILER_INFO "")
IF(MSVC)
    IF(MSVC90)   #Visual Studio 9
        SET(cmake_c_compiler_version "Microsoft Visual Studio 9.0")
    ELSE(MSVC10) #Visual Studio 10
        SET(cmake_c_compiler_version "Microsoft Visual Studio 10.0")
    ELSE(MSVC11) #Visual Studio 11
        SET(cmake_c_compiler_version "Microsoft Visual Studio 11.0")
    ELSE(MSVC12) #Visual Studio 12
        SET(cmake_c_compiler_version "Microsoft Visual Studio 12.0")
    ENDIF()
ELSE()
    execute_process(COMMAND ${CMAKE_C_COMPILER} --version
            OUTPUT_VARIABLE cmake_c_compiler_version)
ENDIF(MSVC)
set(COMPILER_INFO "${CMAKE_C_COMPILER}:::${CMAKE_C_FLAGS_${GRCBTU}} ${CMAKE_C_FLAGS}\n${CMAKE_CXX_COMPILER}:::${CMAKE_CXX_FLAGS_${GRCBTU}} ${CMAKE_CXX_FLAGS}\n" )

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
    MESSAGE(STATUS "BUILD INFO ::: ${machine_name} ::: ${COMPILER_NAME} ::: ${CMAKE_C_FLAGS_${CBTU}} ${CMAKE_C_FLAGS} ${${machine_name}_flags}")
    set(COMPILER_INFO "${COMPILER_INFO}${machine_name}:::${COMPILER_NAME}:::${CMAKE_C_FLAGS_${CBTU}} ${CMAKE_C_FLAGS} ${${machine_name}_flags}\n" )
    if(${machine_name}_flags)
        set_source_files_properties(${machine_source} PROPERTIES COMPILE_FLAGS "${${machine_name}_flags}")
    endif()

    #add to available machine defs
    string(TOUPPER LV_MACHINE_${machine_name} machine_def)
    list(APPEND machine_defs ${machine_def})
endforeach(machine_name)

# Convert to a C string to compile and display properly
string(STRIP "${cmake_c_compiler_version}" cmake_c_compiler_version)
string(STRIP ${COMPILER_INFO} COMPILER_INFO)
MESSAGE(STATUS "Compiler Version: ${cmake_c_compiler_version}")
string(REPLACE "\n" " \\n" cmake_c_compiler_version ${cmake_c_compiler_version})
string(REPLACE "\n" " \\n" COMPILER_INFO ${COMPILER_INFO})

########################################################################
# Set local include directories first
########################################################################
include_directories(
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/kernels
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

########################################################################
# Handle ASM support
#  on by default, but let users turn it off
########################################################################
if(${CMAKE_VERSION} VERSION_GREATER "2.8.9")
  set(ASM_ARCHS_AVAILABLE "armv7")

  set(FULL_C_FLAGS "${CMAKE_C_FLAGS}" "${CMAKE_CXX_COMPILER_ARG1}")

  # sort through a list of all architectures we have ASM for
  # if we find one that matches our current system architecture
  # set up the assembler flags and include the source files
  foreach(ARCH ${ASM_ARCHS_AVAILABLE})
    string(REGEX MATCH "${ARCH}" ASM_ARCH "${FULL_C_FLAGS}")
    if( ASM_ARCH STREQUAL "armv7" )
      message(STATUS "---- Adding ASM files") # we always use ATT syntax
      message(STATUS "-- Detected armv7 architecture; enabling ASM")
      # setup architecture specific assembler flags
      set(ARCH_ASM_FLAGS "-mfpu=neon -g")
      # then add the files
      include_directories(${CMAKE_SOURCE_DIR}/kernels/volk/asm/neon)
      file(GLOB asm_files ${CMAKE_SOURCE_DIR}/kernels/volk/asm/neon/*.s)
      foreach(asm_file ${asm_files})
        list(APPEND volk_sources ${asm_file})
        message(STATUS "Adding source file: ${asm_file}")
      endforeach(asm_file)
    endif()
    enable_language(ASM)
    set(CMAKE_ASM_FLAGS ${ARCH_ASM_FLAGS})
    message(STATUS "c flags: ${FULL_C_FLAGS}")
    message(STATUS "asm flags: ${CMAKE_ASM_FLAGS}")
  endforeach(ARCH)

else(${CMAKE_VERSION} VERSION_GREATER "2.8.9")
  message(STATUS "Not enabling ASM support. CMake >= 2.8.10 required.")
  foreach(machine_name ${available_machines})
    string(REGEX MATCH "neon" NEON_MACHINE ${machine_name})
    if( NEON_MACHINE STREQUAL "neon")
      message(FATAL_ERROR "CMake >= 2.8.10 is required for ARM NEON support")
    endif()
  endforeach()
endif(${CMAKE_VERSION} VERSION_GREATER "2.8.9")

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
# Handle the generated constants
########################################################################

execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
    "import time;print time.strftime('%a, %d %b %Y %H:%M:%S', time.gmtime())"
    OUTPUT_VARIABLE BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Loading build date ${BUILD_DATE} into constants...")
message(STATUS "Loading version ${VERSION} into constants...")

#double escape for windows backslash path separators
string(REPLACE "\\" "\\\\" prefix ${prefix})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/constants.c.in
    ${CMAKE_CURRENT_BINARY_DIR}/constants.c
@ONLY)

list(APPEND volk_sources ${CMAKE_CURRENT_BINARY_DIR}/constants.c)

########################################################################
# Setup the volk sources list and library
########################################################################
if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
endif()

list(APPEND volk_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/volk_prefs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/volk_rank_archs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/volk_malloc.c
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

if(ENABLE_STATIC_LIBS)
  add_library(volk_static STATIC ${volk_sources})

  if(NOT WIN32)
    set_target_properties(volk_static
      PROPERTIES OUTPUT_NAME volk)
  endif(NOT WIN32)

  install(TARGETS volk_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "volk_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)

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

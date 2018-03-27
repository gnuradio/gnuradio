# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

include(GrMiscUtils)
GR_CHECK_HDR_N_DEF(sys/resource.h HAVE_SYS_RESOURCE_H)

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
string(REPLACE "\\" "\\\\" prefix "${prefix}")
string(REPLACE "\\" "\\\\" SYSCONFDIR "${SYSCONFDIR}")
string(REPLACE "\\" "\\\\" GR_PREFSDIR "${GR_PREFSDIR}")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/constants.cc.in
    ${CMAKE_CURRENT_BINARY_DIR}/constants.cc
    ESCAPE_QUOTES
@ONLY)

list(APPEND gnuradio_runtime_sources ${CMAKE_CURRENT_BINARY_DIR}/constants.cc)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(${GNURADIO_RUNTIME_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}
		    ${CMAKE_CURRENT_BINARY_DIR}/../include/
		    ${VOLK_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
)

if(ENABLE_CTRLPORT_THRIFT)
    list(APPEND include_directories
        ${THRIFT_INCLUDE_DIRS}
    )
endif(ENABLE_CTRLPORT_THRIFT)

########################################################################
# Include subdirs rather to populate to the sources lists.
########################################################################
add_subdirectory(pmt)
GR_INCLUDE_SUBDIRECTORY(messages)
GR_INCLUDE_SUBDIRECTORY(thread)
GR_INCLUDE_SUBDIRECTORY(math)
GR_INCLUDE_SUBDIRECTORY(controlport)

########################################################################
# Setup library
########################################################################
list(APPEND gnuradio_runtime_sources
  basic_block.cc
  block.cc
  block_detail.cc
  block_executor.cc
  block_gateway_impl.cc
  block_registry.cc
  buffer.cc
  circular_file.cc
  complex_vec_test.cc
  feval.cc
  flat_flowgraph.cc
  flowgraph.cc
  hier_block2.cc
  hier_block2_detail.cc
  high_res_timer.cc
  io_signature.cc
  local_sighandler.cc
  logger.cc
  malloc16.c
  message.cc
  misc.cc
  msg_accepter.cc
  msg_handler.cc
  msg_queue.cc
  pagesize.cc
  prefs.cc
  realtime.cc
  realtime_impl.cc
  scheduler.cc
  scheduler_sts.cc
  scheduler_tpb.cc
  single_threaded_scheduler.cc
  sptr_magic.cc
  sync_block.cc
  sync_decimator.cc
  sync_interpolator.cc
  sys_paths.cc
  tagged_stream_block.cc
  test.cc
  top_block.cc
  top_block_impl.cc
  tpb_detail.cc
  tpb_thread_body.cc
  vmcircbuf.cc
  vmcircbuf_createfilemapping.cc
  vmcircbuf_mmap_shm_open.cc
  vmcircbuf_mmap_tmpfile.cc
  vmcircbuf_prefs.cc
  vmcircbuf_sysv_shm.cc
  ${gnuradio_ctrlport_sources}
)

# PowerPC workaround for posix_memalign
# Might not be needed, but we'll keep it for now.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)")
    list(APPEND gnuradio_runtime_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/posix_memalign.cc
    )
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)")

list(APPEND gnuradio_runtime_libs
    gnuradio-pmt
    ${VOLK_LIBRARIES}
    ${Boost_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
)

#Add libraries for winsock2.h on Windows
CHECK_INCLUDE_FILE_CXX(windows.h HAVE_WINDOWS_H)
IF(HAVE_WINDOWS_H)
    ADD_DEFINITIONS(-DHAVE_WINDOWS_H -DUSING_WINSOCK -DWIN32_LEAN_AND_MEAN)
    MESSAGE(STATUS "Adding windows libs to gnuradio runtime libs...")
    LIST(APPEND gnuradio_runtime_libs ws2_32 wsock32)
ENDIF(HAVE_WINDOWS_H)

#need to link with librt on ubuntu 11.10 for shm_*
if((LINUX) OR (CMAKE_SYSTEM_NAME STREQUAL "kFreeBSD"))
    list(APPEND gnuradio_runtime_libs rt)
endif()

########################################################################
# Add DLL resource file when using MSVC
########################################################################

if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-runtime.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.rc
    @ONLY)

    list(APPEND gnuradio_runtime_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.rc
    )
endif(MSVC)

########################################################################
# Control availability of vmcircbuf methods.
# For now, only allows disabling of shm methods, which cause uncatchable
#    segmentation faults on Cygwin with gcc 4.x (x <= 5)
# Usage:
#   GR_VMCIRCBUF()
#
# Will set TRY_SHM_VMCIRCBUF to 1 by default except on Windows machines.
# Can manually set with -DTRY_SHM_VMCIRCBUF=0|1
########################################################################

if(WIN32)
    OPTION(TRY_SHM_VMCIRCBUF "Try SHM VMCIRCBUF" OFF)
else(WIN32)
    OPTION(TRY_SHM_VMCIRCBUF "Try SHM VMCIRCBUF" ON)
endif(WIN32)

message(STATUS "TRY_SHM_VMCIRCBUF set to ${TRY_SHM_VMCIRCBUF}.")

if(TRY_SHM_VMCIRCBUF)
    add_definitions( -DTRY_SHM_VMCIRCBUF )
endif(TRY_SHM_VMCIRCBUF)


#######################################################
# SHARED LIB BUILD
#######################################################
add_library(gnuradio-runtime SHARED ${gnuradio_runtime_sources})
target_link_libraries(gnuradio-runtime ${gnuradio_runtime_libs})
GR_LIBRARY_FOO(gnuradio-runtime RUNTIME_COMPONENT "runtime_runtime" DEVEL_COMPONENT "runtime_devel")

add_dependencies(gnuradio-runtime
  pmt_generated runtime_generated_includes
)


#######################################################
# STATIC LIB BUILD
#######################################################
if(ENABLE_STATIC_LIBS)
  # Remove controlport-specific source files from staticlibs build if
  # ICE is the backend since it does not build statically.
  if(ENABLE_GR_CTRLPORT)
    if(ICE_FOUND)
      list(REMOVE_ITEM gnuradio_runtime_sources
        ${gnuradio_ctrlport_sources}
        )

      # Remove GR_CTRLPORT set this target's definitions.
      # Makes sure we don't try to use ControlPort stuff in source files
      GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
      list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
      SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

      # readd it to the target since we removed it from the directory-wide list.
      SET_PROPERTY(TARGET gnuradio-runtime APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
    endif(ICE_FOUND)
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-runtime_static STATIC ${gnuradio_runtime_sources})

  if(NOT WIN32)
    set_target_properties(gnuradio-runtime_static
      PROPERTIES OUTPUT_NAME gnuradio-runtime)
  endif(NOT WIN32)

  install(TARGETS gnuradio-runtime_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "runtime_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)

########################################################################
# Setup tests
########################################################################
if(ENABLE_TESTING)
include(GrTest)

########################################################################
# Append gnuradio-runtime test sources
########################################################################
list(APPEND test_gnuradio_runtime_sources
  math/qa_fxpt.cc
  math/qa_fxpt_nco.cc
  math/qa_fxpt_vco.cc
  math/qa_math.cc
  math/qa_sincos.cc
  math/qa_fast_atan2f.cc
  qa_buffer.cc
  qa_io_signature.cc
  qa_circular_file.cc
  qa_logger.cc
  qa_vmcircbuf.cc
  qa_runtime.cc
)

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/math)
include_directories(${CPPUNIT_INCLUDE_DIRS})
link_directories(${CPPUNIT_LIBRARY_DIRS})

add_library(test-gnuradio-runtime SHARED ${test_gnuradio_runtime_sources})
target_link_libraries(test-gnuradio-runtime gnuradio-runtime gnuradio-pmt ${CPPUNIT_LIBRARIES} ${Boost_LIBRARIES} ${LOG4CPP_LIBRARIES})

########################################################################
# Build the test executable
# Set the test environment so the build libs will be found under MSVC.
########################################################################
list(APPEND GR_TEST_TARGET_DEPS test-gnuradio-runtime)
add_executable(gr_runtime_test test_runtime.cc)
target_link_libraries(gr_runtime_test test-gnuradio-runtime)
GR_ADD_TEST(gr-runtime-test gr_runtime_test)

endif(ENABLE_TESTING)

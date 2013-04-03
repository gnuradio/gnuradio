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
message(STATUS "Loading build date ${BUILD_DATE} into gr_constants...")
message(STATUS "Loading version ${VERSION} into gr_constants...")

#double escape for windows backslash path separators
string(REPLACE "\\" "\\\\" prefix ${prefix})
string(REPLACE "\\" "\\\\" SYSCONFDIR ${SYSCONFDIR})
string(REPLACE "\\" "\\\\" GR_PREFSDIR ${GR_PREFSDIR})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_constants.cc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gr_constants.cc
@ONLY)

list(APPEND gnuradio_runtime_sources ${CMAKE_CURRENT_BINARY_DIR}/gr_constants.cc)

########################################################################
# Include subdirs rather to populate to the sources lists.
########################################################################
#GR_INCLUDE_SUBDIRECTORY(foo)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(${GNURADIO_RUNTIME_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}
		    ${CMAKE_CURRENT_BINARY_DIR}/../include/
		    ${VOLK_INCLUDE_DIRS}
		    ${ICE_INCLUDE_DIR}
                    ${Boost_INCLUDE_DIRS}
)

########################################################################
# Include subdirs rather to populate to the sources lists.
########################################################################
GR_INCLUDE_SUBDIRECTORY(messages)
GR_INCLUDE_SUBDIRECTORY(pmt)
GR_INCLUDE_SUBDIRECTORY(thread)

########################################################################
# Setup library
########################################################################
list(APPEND gnuradio_runtime_sources
  complex_vec_test.cc
  gr_basic_block.cc
  gr_block.cc
  gr_block_detail.cc
  gr_block_executor.cc
  gr_block_registry.cc
  gr_buffer.cc
  gr_circular_file.cc
  gr_dispatcher.cc
  gr_error_handler.cc
  gr_fast_atan2f.cc
  gr_feval.cc
  gr_flat_flowgraph.cc
  gr_flowgraph.cc
  gr_fxpt.cc
  gr_hier_block2.cc
  gr_hier_block2_detail.cc
  gri_debugger_hook.cc
  gr_io_signature.cc
  gr_local_sighandler.cc
  gr_logger.cc
  gr_message.cc
  gr_misc.cc
  gr_msg_accepter.cc
  gr_msg_handler.cc
  gr_msg_queue.cc
  gr_pagesize.cc
  gr_preferences.cc
  gr_prefs.cc
  gr_random.cc
  gr_realtime.cc
  gr_reverse.cc
  gr_scheduler.cc
  gr_scheduler_sts.cc
  gr_scheduler_tpb.cc
  gr_select_handler.cc
  gr_sincos.c
  gr_single_threaded_scheduler.cc
  gr_sptr_magic.cc
  gr_sync_block.cc
  gr_sync_decimator.cc
  gr_sync_interpolator.cc
  gr_sys_paths.cc
  gr_tagged_stream_block.cc
  gr_test.cc
  gr_top_block.cc
  gr_top_block_impl.cc
  gr_tpb_detail.cc
  gr_tpb_thread_body.cc
  gr_vmcircbuf.cc
  gr_vmcircbuf_createfilemapping.cc
  gr_vmcircbuf_mmap_shm_open.cc
  gr_vmcircbuf_mmap_tmpfile.cc
  gr_vmcircbuf_sysv_shm.cc
  malloc16.c
  runtime_block_gateway.cc
)

# PowerPC workaround for posix_memalign
# Might not be needed, but we'll keep it for now.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)")
    list(APPEND gnuradio_runtime_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/posix_memalign.cc
    )
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)")

list(APPEND gnuradio_runtime_libs
    volk
    ${Boost_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
)

#Add libraries for winsock2.h on Windows
CHECK_INCLUDE_FILE_CXX(windows.h HAVE_WINDOWS_H)
IF(HAVE_WINDOWS_H)
    ADD_DEFINITIONS(-DHAVE_WINDOWS_H -DUSING_WINSOCK -DWIN32_LEAN_AND_MEAN)
    MESSAGE(STATUS "Adding windows libs to gnuradio runtime libs...")
    LIST(APPEND gnuradio_runtime_libs WS2_32.lib WSock32.lib)
ENDIF(HAVE_WINDOWS_H)

#need to link with librt on ubuntu 11.10 for shm_*
if(LINUX)
    list(APPEND gnuradio_runtime_libs rt)
endif()

if(ENABLE_GR_CTRLPORT)

include_directories(${ICE_INCLUDE_DIR})

# Add definition so we can compile in ControlPort to the blocks.
ADD_DEFINITIONS(-DGR_CTRLPORT)

########################################################################
# Run ICE To compile Slice files
########################################################################
EXECUTE_PROCESS(
   COMMAND "${ICE_SLICE2CPP}" "-I${CMAKE_CURRENT_SOURCE_DIR}"
       "--output-dir=${CMAKE_CURRENT_BINARY_DIR}"
       "${CMAKE_CURRENT_SOURCE_DIR}/gnuradio.ice"
   )

list(APPEND gnuradio_runtime_sources
    ice_application_base.cc
    rpcmanager.cc
    rpcpmtconverters_ice.cc
    rpcserver_aggregator.cc
    rpcserver_booter_aggregator.cc
    rpcserver_booter_ice.cc
    rpcserver_ice.cc
    rpcserver_selector.cc
    rpcpmtconverters_ice.cc
)

# Append generated file in build directory
list(APPEND gnuradio_runtime_sources 
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio.cpp
)

########################################################################
# Add controlport stuff to gnuradio-runtime
########################################################################

include_directories(${CMAKE_CURRENT_BINARY_DIR})

list(APPEND gnuradio_runtime_libs
  ${ICE_LIBRARIES}
)

endif(ENABLE_GR_CTRLPORT)

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

add_library(gnuradio-runtime SHARED ${gnuradio_runtime_sources})
target_link_libraries(gnuradio-runtime ${gnuradio_runtime_libs})
GR_LIBRARY_FOO(gnuradio-runtime RUNTIME_COMPONENT "runtime" DEVEL_COMPONENT "runtime_devel")

add_dependencies(gnuradio-runtime
  runtime_generated_includes
)

########################################################################
# Setup tests
########################################################################
if(ENABLE_TESTING)
include(GrTest)

########################################################################
# Append gnuradio-runtime test sources
########################################################################
list(APPEND test_gnuradio_runtime_sources
  qa_gr_buffer.cc
  qa_gr_circular_file.cc
  qa_gr_fxpt.cc
  qa_gr_fxpt_nco.cc
  qa_gr_fxpt_vco.cc
  qa_gr_io_signature.cc
  qa_gr_logger.cc
  qa_gr_math.cc
  qa_gr_vmcircbuf.cc
  qa_runtime.cc
  qa_sincos.cc
  pmt/qa_pmt.cc
  pmt/qa_pmt_prims.cc
)

include_directories(${CPPUNIT_INCLUDE_DIRS})
link_directories(${CPPUNIT_LIBRARY_DIRS})

add_library(test-gnuradio-runtime SHARED ${test_gnuradio_runtime_sources})
target_link_libraries(test-gnuradio-runtime gnuradio-runtime ${CPPUNIT_LIBRARIES} ${Boost_LIBRARIES} ${LOG4CPP_LIBRARIES})

########################################################################
# Build the test executable
# Set the test environment so the build libs will be found under MSVC.
########################################################################
list(APPEND GR_TEST_TARGET_DEPS test-gnuradio-runtime)
add_executable(gr_runtime_test test_runtime.cc)
target_link_libraries(gr_runtime_test test-gnuradio-runtime)
GR_ADD_TEST(gr-runtime-test gr_runtime_test)

endif(ENABLE_TESTING)


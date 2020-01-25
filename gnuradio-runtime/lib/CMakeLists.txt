# Copyright 2013,2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrMiscUtils)
gr_check_hdr_n_def(sys/resource.h HAVE_SYS_RESOURCE_H)

########################################################################
# Handle the generated constants
########################################################################
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
    "import time;print(time.strftime('%a, %d %b %Y %H:%M:%S', time.gmtime()))"
    OUTPUT_VARIABLE BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Loading build date ${BUILD_DATE} into constants...")
message(STATUS "Loading version ${VERSION} into constants...")

#double escape for windows backslash path separators
string(REPLACE "\\" "\\\\" prefix "${prefix}")
string(REPLACE "\\" "\\\\" SYSCONFDIR "${SYSCONFDIR}")
string(REPLACE "\\" "\\\\" GR_PREFSDIR "${GR_PREFSDIR}")


#########################################################################
# Include subdirs rather to populate to the sources lists.
########################################################################
add_subdirectory(pmt)

########################################################################
# Setup library
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/constants.cc.in
    ${CMAKE_CURRENT_BINARY_DIR}/constants.cc
    ESCAPE_QUOTES
    @ONLY)

add_library(gnuradio-runtime
  ${CMAKE_CURRENT_BINARY_DIR}/constants.cc
  basic_block.cc
  block.cc
  block_detail.cc
  block_executor.cc
  block_gateway_impl.cc
  block_registry.cc
  buffer.cc
  feval.cc
  flat_flowgraph.cc
  flowgraph.cc
  hier_block2.cc
  hier_block2_detail.cc
  high_res_timer.cc
  io_signature.cc
  local_sighandler.cc
  logger.cc
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
  scheduler_tpb.cc
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
  )

# Messages
target_sources(gnuradio-runtime PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/messages/msg_accepter.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/messages/msg_accepter_msgq.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/messages/msg_producer.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/messages/msg_queue.cc
  )

# Thread
target_sources(gnuradio-runtime PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/thread/thread.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/thread/thread_body_wrapper.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/thread/thread_group.cc
  )

# Math
target_sources(gnuradio-runtime PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/math/fast_atan2f.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/math/fxpt.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/math/random.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/math/sincos.cc
  )

# Controlport
if(ENABLE_GR_CTRLPORT)

# Keep track of the number of backends ControlPort supports
SET(CTRLPORT_BACKENDS 0)

target_sources(gnuradio-runtime PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/controlport/rpcmanager.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/controlport/rpcserver_aggregator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/controlport/rpcserver_booter_aggregator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/controlport/rpcserver_selector.cc
)

OPTION(ENABLE_CTRLPORT_THRIFT "Enable ControlPort Thrift support" ON)

if(ENABLE_CTRLPORT_THRIFT)
# Look if Thrift is installed and use it as a ControlPort backend.
FIND_PACKAGE(THRIFT)

if(THRIFT_FOUND)
list(APPEND EXTRA_DEPS "THRIFT")
MATH(EXPR CTRLPORT_BACKENDS "${CTRLPORT_BACKENDS} + 1")

# Indicate thrift as an installed backend in the cmake summary.
message(STATUS "Found and enabling Thrift backend to ControlPort")
GR_APPEND_SUBCOMPONENT("thrift")

# Run Thrift To compile C++ and Python files
message(STATUS "Running thrift to build C++ bindings")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/controlport/thrift/)

list(APPEND gnuradio_thrift_generated_sources
  ${CMAKE_CURRENT_BINARY_DIR}/controlport/thrift/gnuradio_types.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/controlport/thrift/gnuradio_constants.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/controlport/thrift/ControlPort.cpp
  )
add_custom_command(
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/gnuradio.thrift
  OUTPUT ${gnuradio_thrift_generated_sources}
  COMMAND ${THRIFT_BIN} --gen cpp -out ${CMAKE_CURRENT_BINARY_DIR}/controlport/thrift/ ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/gnuradio.thrift
  )

target_include_directories(gnuradio-runtime PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/controlport>)

target_sources(gnuradio-runtime PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/rpcserver_thrift.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/rpcpmtconverters_thrift.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/rpcserver_booter_thrift.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/thrift_application_base.cc
  # add files built by compiling gnuradio.thrift
  ${gnuradio_thrift_generated_sources}
  )
target_link_libraries(gnuradio-runtime PUBLIC
  Thrift::thrift
  )

# Add install rule to move example Thrift configuration file into a
# documentation directory
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/thrift.conf.example
  DESTINATION ${GR_PKG_DOC_DIR}/config
)

endif(THRIFT_FOUND)
endif(ENABLE_CTRLPORT_THRIFT)

########################################################################
# Add controlport stuff to gnuradio-runtime
########################################################################

# Save the number of backends for testing against later
set(
  CTRLPORT_BACKENDS ${CTRLPORT_BACKENDS}
  CACHE INTERNAL "Number of ControlPort backends available"
)

endif(ENABLE_GR_CTRLPORT)

target_link_libraries(gnuradio-runtime PUBLIC
  gnuradio-pmt
  ${GR_VOLK_LIB}
  Boost::program_options
  Boost::filesystem
  Boost::system
  Boost::regex
  Boost::thread
  Log4Cpp::log4cpp
  MPLib::mplib
  )

target_include_directories(gnuradio-runtime
  PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

  if(ENABLE_GR_CTRLPORT)
    target_compile_definitions(gnuradio-runtime PUBLIC GR_CTRLPORT)
  endif()


#Add libraries for winsock2.h on Windows
check_include_file_cxx(windows.h HAVE_WINDOWS_H)
if(HAVE_WINDOWS_H)
    target_compile_definitions(gnuradio-runtime PUBLIC -DHAVE_WINDOWS_H -DUSING_WINSOCK -DWIN32_LEAN_AND_MEAN)
    message(STATUS "Adding windows libs to gnuradio runtime libs...")
    target_link_libraries(gnuradio-runtime PUBLIC ws2_32 wsock32)
endif(HAVE_WINDOWS_H)

#need to link with librt on ubuntu 11.10 for shm_*
if((LINUX) OR (CMAKE_SYSTEM_NAME STREQUAL "kFreeBSD"))
    target_link_libraries(gnuradio-runtime PUBLIC rt)
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

    target_sources(gnuradio-runtime PRIVATE
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
    option(TRY_SHM_VMCIRCBUF "Try SHM VMCIRCBUF" OFF)
else(WIN32)
    option(TRY_SHM_VMCIRCBUF "Try SHM VMCIRCBUF" ON)
endif(WIN32)

message(STATUS "TRY_SHM_VMCIRCBUF set to ${TRY_SHM_VMCIRCBUF}.")

if(TRY_SHM_VMCIRCBUF)
    target_compile_definitions(gnuradio-runtime PRIVATE -DTRY_SHM_VMCIRCBUF )
endif(TRY_SHM_VMCIRCBUF)

set(EXTRA_DEPS "")
if(ENABLE_CTRLPORT_THRIFT)
  if(THRIFT_FOUND)
    list(APPEND EXTRA_DEPS THRIFT)
  endif()
endif()
gr_library_foo(gnuradio-runtime ${EXTRA_DEPS})


########################################################################
# Setup tests
########################################################################
if(ENABLE_TESTING)
  include(GrTest)

  # Regular runtime tests:
  list(APPEND test_gnuradio_runtime_sources
    qa_buffer.cc
    qa_io_signature.cc
    qa_logger.cc
    qa_vmcircbuf.cc
  )
  list(APPEND GR_TEST_TARGET_DEPS gnuradio-runtime gnuradio-pmt)

  foreach(qa_file ${test_gnuradio_runtime_sources})
    gr_add_cpp_test("runtime_${qa_file}"
      ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file}
      )

  endforeach(qa_file)

  # Math tests:
  list(APPEND test_gnuradio_math_sources
    qa_fxpt.cc
    qa_fxpt_nco.cc
    qa_fxpt_vco.cc
    qa_math.cc
    qa_sincos.cc
    qa_fast_atan2f.cc
  )

  foreach(qa_file ${test_gnuradio_math_sources})
    gr_add_cpp_test("math_${qa_file}"
      ${CMAKE_CURRENT_SOURCE_DIR}/math/${qa_file}
      )
    target_include_directories("math_${qa_file}"
      PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/math
      )
  endforeach(qa_file)

  # PMT tests:
  gr_add_cpp_test("pmt_prims" ${CMAKE_CURRENT_SOURCE_DIR}/pmt/qa_pmt_prims.cc)
  gr_add_cpp_test("pmt_unv" ${CMAKE_CURRENT_SOURCE_DIR}/pmt/qa_pmt_unv.cc)
  target_include_directories("pmt_prims" PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/pmt)
  target_include_directories("pmt_unv" PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/pmt)

endif(ENABLE_TESTING)

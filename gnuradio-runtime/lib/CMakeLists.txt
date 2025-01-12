# Copyright 2013,2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrMiscUtils)
gr_check_hdr_n_def(sys/resource.h HAVE_SYS_RESOURCE_H)

########################################################################
# Look for libunwind
########################################################################
find_package(libunwind)

########################################################################
# Handle the generated constants
########################################################################
message(STATUS "Loading build date ${BUILD_DATE} into constants...")
message(STATUS "Loading version ${VERSION} into constants...")

#Generate the constants file, now that we actually know which components will be enabled.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/constants.cc.in
               ${CMAKE_CURRENT_BINARY_DIR}/constants.cc ESCAPE_QUOTES @ONLY)

#########################################################################
# Include subdirs rather to populate to the sources lists.
########################################################################
add_subdirectory(pmt)

########################################################################
# Setup library
########################################################################

#Generate sine table header file for Math part of library
set(MATH_SINE_TABLE_HEADER ${CMAKE_CURRENT_BINARY_DIR}/math/generated/sine_table.h)
set(MATH_SINE_TABLE_GENERATOR ${CMAKE_CURRENT_SOURCE_DIR}/math/gen_sine_table.py)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory
                        "${CMAKE_CURRENT_BINARY_DIR}/math/generated")
add_custom_command(
    DEPENDS ${MATH_SINE_TABLE_GENERATOR}
    OUTPUT ${MATH_SINE_TABLE_HEADER}
    COMMAND ${PYTHON_EXECUTABLE} ${MATH_SINE_TABLE_GENERATOR} > ${MATH_SINE_TABLE_HEADER})

add_library(
    gnuradio-runtime
    ${CMAKE_CURRENT_BINARY_DIR}/constants.cc
    basic_block.cc
    block.cc
    block_detail.cc
    block_executor.cc
    block_registry.cc
    buffer.cc
    buffer_double_mapped.cc
    buffer_reader.cc
    buffer_reader_sm.cc
    buffer_single_mapped.cc
    dictionary_logger_backend.cc
    flat_flowgraph.cc
    flowgraph.cc
    hier_block2.cc
    hier_block2_detail.cc
    high_res_timer.cc
    host_buffer.cc
    io_signature.cc
    local_sighandler.cc
    logger.cc
    message.cc
    msg_accepter.cc
    msg_handler.cc
    msg_queue.cc
    pagesize.cc
    pdu.cc
    pmt_fmt.cc
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
    tag_fmt.cc
    tagged_stream_block.cc
    terminate_handler.cc
    top_block.cc
    top_block_impl.cc
    tpb_detail.cc
    tpb_thread_body.cc
    transfer_type.cc
    vmcircbuf.cc
    vmcircbuf_createfilemapping.cc
    vmcircbuf_mmap_shm_open.cc
    vmcircbuf_mmap_tmpfile.cc
    vmcircbuf_prefs.cc
    vmcircbuf_sysv_shm.cc)

# Messages
target_sources(
    gnuradio-runtime
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/messages/msg_accepter.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/messages/msg_accepter_msgq.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/messages/msg_producer.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/messages/msg_queue.cc)

# Thread
target_sources(
    gnuradio-runtime
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thread/thread.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/thread/thread_body_wrapper.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/thread/thread_group.cc)

# Math
target_include_directories(gnuradio-runtime PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/math)
target_sources(
    gnuradio-runtime
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/math/fast_atan2f.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/math/fxpt.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/math/random.cc ${MATH_SINE_TABLE_HEADER})

target_link_libraries(
    gnuradio-runtime
    PUBLIC
        gnuradio-pmt
        Volk::volk
        Boost::thread
        spdlog::spdlog
        MPLib::mplib
    PRIVATE
        Boost::program_options
        libunwind::libunwind)

# Address linker issues with std::filesystem on Centos 8 and Debian
target_link_libraries(
    gnuradio-runtime
    PUBLIC
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>
)

target_include_directories(
    gnuradio-runtime
    PUBLIC $<INSTALL_INTERFACE:include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(gnuradio-runtime PUBLIC cxx_std_17)

target_compile_definitions(gnuradio-runtime PUBLIC ${MPLIB_DEFINITIONS})

# constants.cc includes boost::dll headers, force them to use std::filesystem
target_compile_definitions(gnuradio-runtime PRIVATE BOOST_DLL_USE_STD_FS)

########################################################################
# Add controlport stuff to gnuradio-runtime
########################################################################
# Controlport
if(ENABLE_GR_CTRLPORT)

    # Keep track of the number of backends ControlPort supports
    set(CTRLPORT_BACKENDS 0)

    target_sources(
        gnuradio-runtime
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/controlport/rpcmanager.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/controlport/rpcserver_aggregator.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/controlport/rpcserver_booter_aggregator.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/controlport/rpcserver_selector.cc)

    if(ENABLE_CTRLPORT_THRIFT)
        math(EXPR CTRLPORT_BACKENDS "${CTRLPORT_BACKENDS} + 1")

        # Indicate thrift as an installed backend in the cmake summary.
        message(STATUS "Enabling Thrift backend to ControlPort")
        gr_append_subcomponent("thrift")

        # Run Thrift To compile C++ and Python files
        message(STATUS "Running thrift to build C++ bindings")
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/controlport/thrift/)

        list(
            APPEND
            gnuradio_thrift_generated_sources
            ${CMAKE_CURRENT_BINARY_DIR}/controlport/thrift/gnuradio_types.cpp
            ${CMAKE_CURRENT_BINARY_DIR}/controlport/thrift/gnuradio_constants.cpp
            ${CMAKE_CURRENT_BINARY_DIR}/controlport/thrift/ControlPort.cpp)
        add_custom_command(
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/gnuradio.thrift
            OUTPUT ${gnuradio_thrift_generated_sources}
            COMMAND
                ${THRIFT_BIN} --gen cpp -out
                ${CMAKE_CURRENT_BINARY_DIR}/controlport/thrift/
                ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/gnuradio.thrift)

        target_include_directories(
            gnuradio-runtime
            PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/controlport>)

        target_sources(
            gnuradio-runtime
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/rpcserver_thrift.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/rpcpmtconverters_thrift.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/rpcserver_booter_thrift.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/thrift_application_base.cc
                # add files built by compiling gnuradio.thrift
                ${gnuradio_thrift_generated_sources})
        target_link_libraries(gnuradio-runtime PRIVATE Thrift::thrift)

        target_compile_definitions(gnuradio-runtime PUBLIC GR_CTRLPORT)

        # Add install rule to move example Thrift configuration file into a
        # documentation directory
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/controlport/thrift/thrift.conf.example
                DESTINATION ${GR_PKG_DOC_DIR}/config)

    endif(ENABLE_CTRLPORT_THRIFT)

    # Save the number of backends for testing against later
    set(CTRLPORT_BACKENDS
        ${CTRLPORT_BACKENDS}
        CACHE INTERNAL "Number of ControlPort backends available")

endif(ENABLE_GR_CTRLPORT)

########################################################################
# Add misc conditional includes/definitions to gnuradio-runtime
########################################################################

if(ENABLE_PERFORMANCE_COUNTERS)
    target_compile_definitions(gnuradio-runtime PUBLIC -DGR_PERFORMANCE_COUNTERS)
endif()

if(GR_IS_BIG_ENDIAN)
    target_compile_definitions(gnuradio-runtime PUBLIC -DGR_IS_BIG_ENDIAN)
endif(GR_IS_BIG_ENDIAN)

if(MSVC)
    target_compile_definitions(
        gnuradio-runtime
        PUBLIC # Minimum version: "Windows Server 2003 with SP1, Windows XP with SP2"
               -D_WIN32_WINNT=0x0502
               # disables stupidity and enables std::min and std::max
               -DNOMINMAX
               # stop all kinds of compatibility warnings
               -D_SCL_SECURE_NO_WARNINGS
               -D_CRT_SECURE_NO_WARNINGS
               -D_CRT_SECURE_NO_DEPRECATE
               -D_CRT_NONSTDC_NO_DEPRECATE)
endif(MSVC)

if(WIN32)
    target_compile_definitions(gnuradio-runtime PUBLIC -D_USE_MATH_DEFINES)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        target_compile_definitions(gnuradio-runtime PUBLIC -DMS_WIN64)
    endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
endif(WIN32)

if(WIN32)
    target_compile_definitions(gnuradio-runtime PUBLIC -DWIN32_LEAN_AND_MEAN)
endif(WIN32)

#need to link with librt on ubuntu 11.10 and Hurd for shm_*
if((LINUX) OR (CMAKE_SYSTEM_NAME MATCHES "^(kFreeBSD|GNU)$"))
    target_link_libraries(gnuradio-runtime PUBLIC rt)
endif()

########################################################################
# Add DLL resource file when using MSVC
########################################################################

if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-runtime.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.rc @ONLY)

    target_sources(gnuradio-runtime
                   PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.rc)
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
    target_compile_definitions(gnuradio-runtime PRIVATE -DTRY_SHM_VMCIRCBUF)
endif(TRY_SHM_VMCIRCBUF)

gr_library_foo(gnuradio-runtime)

########################################################################
# Setup tests
########################################################################
if(ENABLE_TESTING)
    include(GrTest)

    # Regular runtime tests:
    list(
        APPEND
        test_gnuradio_runtime_sources
        qa_buffer.cc
        qa_io_signature.cc
        qa_logger.cc
        qa_dictionary_logger.cc
        qa_host_buffer.cc
        qa_tags.cc
        qa_vmcircbuf.cc)
    list(APPEND GR_TEST_TARGET_DEPS gnuradio-runtime gnuradio-pmt)

    foreach(qa_file ${test_gnuradio_runtime_sources})
        gr_add_cpp_test("runtime_${qa_file}" ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file})
        target_link_libraries("runtime_${qa_file}" spdlog::spdlog)
    endforeach(qa_file)

    # Math tests:
    list(
        APPEND
        test_gnuradio_math_sources
        qa_fxpt.cc
        qa_fxpt_nco.cc
        qa_fxpt_vco.cc
        qa_math.cc
        qa_sincos.cc
        qa_fast_atan2f.cc)

    foreach(qa_file ${test_gnuradio_math_sources})
        gr_add_cpp_test("math_${qa_file}" ${CMAKE_CURRENT_SOURCE_DIR}/math/${qa_file})
        target_link_libraries("math_${qa_file}" spdlog::spdlog)
        target_include_directories("math_${qa_file}"
                                   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/math)
    endforeach(qa_file)

    # PMT tests:
    gr_add_cpp_test("pmt_prims" ${CMAKE_CURRENT_SOURCE_DIR}/pmt/qa_pmt_prims.cc)
    gr_add_cpp_test("pmt_unv" ${CMAKE_CURRENT_SOURCE_DIR}/pmt/qa_pmt_unv.cc)
    target_include_directories("pmt_prims" PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/pmt)
    target_include_directories("pmt_unv" PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/pmt)

endif(ENABLE_TESTING)

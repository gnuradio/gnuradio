# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

add_subdirectory(messages)
add_subdirectory(thread)

# Create a version.h with the project version
configure_file(version.h.in
               version.h)
# Install version.h in include/gnuradio
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.h
        DESTINATION ${GR_INCLUDE_DIR}/gnuradio)
########################################################################
# Install header files
########################################################################
install(
    FILES api.h
          attributes.h
          basic_block.h
          block.h
          block_detail.h
          block_registry.h
          buffer.h
          buffer_double_mapped.h
          buffer_reader.h
          buffer_reader_sm.h
          buffer_single_mapped.h
          buffer_type.h
          constants.h
          custom_lock.h
          dictionary_logger_backend.h
          endianness.h
          expj.h
          flowgraph.h
          fxpt.h
          fxpt_nco.h
          fxpt_vco.h
          gr_complex.h
          hier_block2.h
          high_res_timer.h
          host_buffer.h
          io_signature.h
          logger.h
          math.h
          message.h
          msg_accepter.h
          msg_handler.h
          msg_queue.h
          nco.h
          pmt_fmt.h
          pdu.h
          prefs.h
          pycallback_object.h
          random.h
          realtime.h
          runtime_types.h
          tags.h
          tagged_stream_block.h
          top_block.h
          tpb_detail.h
          transfer_type.h
          sincos.h
          sptr_magic.h
          sync_block.h
          sync_decimator.h
          sync_interpolator.h
          sys_paths.h
          types.h
          rpccallbackregister_base.h
          rpcmanager_base.h
          rpcmanager.h
          rpcregisterhelpers.h
          rpcserver_aggregator.h
          rpcserver_base.h
          rpcserver_booter_aggregator.h
          rpcserver_booter_base.h
          rpcserver_selector.h
          xoroshiro128p.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio)

if(ENABLE_CTRLPORT_THRIFT)
    install(FILES rpcserver_booter_thrift.h thrift_application_base.h
                  thrift_server_template.h DESTINATION ${GR_INCLUDE_DIR}/gnuradio)


endif(ENABLE_CTRLPORT_THRIFT)

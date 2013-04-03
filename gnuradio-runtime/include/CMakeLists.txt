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

add_subdirectory(messages)
add_subdirectory(pmt)
add_subdirectory(thread)

########################################################################
# Install header files
########################################################################
install(FILES
  gr_basic_block.h
  gr_block_detail.h
  gr_block.h
  gr_block_registry.h
  gr_buffer.h
  gr_complex.h
  gr_constants.h
  gr_dispatcher.h
  gr_endianness.h
  gr_error_handler.h
  gr_expj.h
  gr_feval.h
  gr_flowgraph.h
  gr_fxpt.h
  gr_fxpt_nco.h
  gr_fxpt_vco.h
  gr_hier_block2.h
  gr_io_signature.h
  gr_logger.h
  gr_math.h
  gr_message.h
  gr_misc.h
  gr_msg_accepter.h
  gr_msg_handler.h
  gr_msg_queue.h
  gr_nco.h
  gr_preferences.h
  gr_prefs.h
  gr_py_feval.h
  gr_random.h
  gr_realtime.h
  gr_runtime_api.h
  gr_runtime_types.h
  gr_select_handler.h
  gr_sincos.h
  gr_single_threaded_scheduler.h
  gr_sptr_magic.h
  gr_sync_block.h
  gr_sync_decimator.h
  gr_sync_interpolator.h
  gr_sys_paths.h
  gr_tagged_stream_block.h
  gr_tags.h
  gr_timer.h
  gr_top_block.h
  gr_tpb_detail.h
  gr_types.h
  gr_unittests.h
  ice_application_base.h
  IcePy_Communicator.h
  ice_server_template.h
  pycallback_object.h
  random.h
  rpccallbackregister_base.h
  rpcmanager_base.h
  rpcmanager.h
  rpcpmtconverters_ice.h
  rpcregisterhelpers.h
  rpcserver_aggregator.h
  rpcserver_base.h
  rpcserver_booter_aggregator.h
  rpcserver_booter_base.h
  rpcserver_booter_ice.h
  rpcserver_ice.h
  rpcserver_selector.h
  runtime_block_gateway.h
  attributes.h
  high_res_timer.h
  realtime.h
  sys_pri.h
  DESTINATION ${GR_INCLUDE_DIR}/gnuradio
  COMPONENT "runtime_devel"
)

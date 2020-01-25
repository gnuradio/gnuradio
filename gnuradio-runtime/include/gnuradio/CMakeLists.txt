# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

add_subdirectory(messages)
add_subdirectory(thread)

########################################################################
# Install header files
########################################################################
install(FILES
  api.h
  attributes.h
  basic_block.h
  block.h
  block_detail.h
  block_gateway.h
  block_registry.h
  buffer.h
  constants.h
  endianness.h
  expj.h
  feval.h
  flowgraph.h
  fxpt.h
  fxpt_nco.h
  fxpt_vco.h
  gr_complex.h
  hier_block2.h
  high_res_timer.h
  io_signature.h
  logger.h
  math.h
  message.h
  misc.h
  msg_accepter.h
  msg_handler.h
  msg_queue.h
  nco.h
  prefs.h
  py_feval.h
  pycallback_object.h
  random.h
  realtime.h
  realtime_impl.h
  runtime_types.h
  tags.h
  tagged_stream_block.h
  top_block.h
  tpb_detail.h
  sincos.h
  sptr_magic.h
  sync_block.h
  sync_decimator.h
  sync_interpolator.h
  sys_paths.h
  tag_checker.h
  types.h
  unittests.h
  rpccallbackregister_base.h
  rpcmanager_base.h
  rpcmanager.h
  rpcregisterhelpers.h
  rpcserver_aggregator.h
  rpcserver_base.h
  rpcserver_booter_aggregator.h
  rpcserver_booter_base.h
  rpcserver_selector.h
  DESTINATION ${GR_INCLUDE_DIR}/gnuradio
)

if(THRIFT_FOUND)
install(FILES
  rpcserver_booter_thrift.h
  thrift_application_base.h
  thrift_server_template.h
  DESTINATION ${GR_INCLUDE_DIR}/gnuradio
)
endif(THRIFT_FOUND)

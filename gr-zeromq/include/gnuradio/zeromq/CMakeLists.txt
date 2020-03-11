# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(FILES
  api.h
  pub_sink.h
  pub_msg_sink.h
  sub_source.h
  sub_msg_source.h
  pull_source.h
  pull_msg_source.h
  push_sink.h
  push_msg_sink.h
  rep_sink.h
  rep_msg_sink.h
  req_source.h
  req_msg_source.h

  DESTINATION ${GR_INCLUDE_DIR}/gnuradio/zeromq
)

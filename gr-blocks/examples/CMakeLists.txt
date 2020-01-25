# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

install(
  FILES
  matrix_multiplexer.grc
  peak_detector2.grc
  selector.grc
  test_stream_mux_tags.grc
  vector_source_with_tags.grc
  DESTINATION ${GR_PKG_DATA_DIR}/examples/blocks
)

add_subdirectory(metadata)
add_subdirectory(tags)

if(ENABLE_GR_CTRLPORT)
add_subdirectory(ctrlport)
endif(ENABLE_GR_CTRLPORT)

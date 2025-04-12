# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

install(
    FILES burst_to_stream.grc
          matrix_interleaver.grc
          matrix_multiplexer.grc
          peak_detector2.grc
          py_snippets_demo.grc
          selector.grc
          stream_mux_demo.grc
          stream_demux_demo.grc
          vector_source_with_tags.grc
          msg_to_var.grc
          var_to_msg.grc
    DESTINATION ${GR_PKG_DATA_DIR}/examples/blocks)

add_subdirectory(metadata)
add_subdirectory(tags)

if(ENABLE_GR_CTRLPORT)
    add_subdirectory(ctrlport)
endif(ENABLE_GR_CTRLPORT)

# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

install(FILES
    zeromq_pub_sink.block.yml
    zeromq_pub_msg_sink.block.yml
    zeromq_sub_source.block.yml
    zeromq_sub_msg_source.block.yml
    zeromq_push_sink.block.yml
    zeromq_push_msg_sink.block.yml
    zeromq_pull_source.block.yml
    zeromq_pull_msg_source.block.yml
    zeromq_rep_sink.block.yml
    zeromq_rep_msg_sink.block.yml
    zeromq_req_source.block.yml
    zeromq_req_msg_source.block.yml

    DESTINATION share/gnuradio/grc/blocks
)



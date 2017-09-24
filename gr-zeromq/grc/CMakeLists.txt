# Copyright 2011 Free Software Foundation, Inc.
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



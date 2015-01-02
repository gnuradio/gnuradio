#
# Copyright 2015 Free Software Foundation, Inc.
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
#

from gnuradio import gr, blocks
import socket
import os

class local_stream_source(gr.hier_block2):
    def __init__(self, itemsize, addr):
        gr.hier_block2.__init__(
            self, 'local_stream_source',
            gr.io_signature(0, 0, 0),
            gr.io_signature(1, 1, itemsize),
        )
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock.connect(addr)
        fd = os.dup(sock.fileno())
        self.connect(blocks.file_descriptor_source(itemsize, fd), self)

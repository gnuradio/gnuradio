from __future__ import unicode_literals
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import time

import zmq
import numpy

class probe_manager(object):
    def __init__(self):
        self.zmq_context = zmq.Context()
        self.poller = zmq.Poller()
        self.interfaces = []

    def add_socket(self, address, data_type, callback_func):
        socket = self.zmq_context.socket(zmq.SUB)
        socket.setsockopt(zmq.SUBSCRIBE, b"")
        # Do not wait more than 5 seconds for a message
        socket.setsockopt(zmq.RCVTIMEO, 100)
        socket.connect(address)
        # use a tuple to store interface elements
        self.interfaces.append((socket, data_type, callback_func))
        self.poller.register(socket, zmq.POLLIN)
        # Give it time for the socket to be ready
        time.sleep(0.5)

    def watcher(self):
        poll = dict(self.poller.poll(1000))
        for i in self.interfaces:
            # i = (socket, data_type, callback_func)
            if poll.get(i[0]) == zmq.POLLIN:
                # receive data
                msg_packed = i[0].recv()
                # use numpy to unpack the data
                msg_unpacked = numpy.frombuffer(msg_packed, numpy.dtype(i[1]))
                # invoke callback function
                i[2](msg_unpacked)

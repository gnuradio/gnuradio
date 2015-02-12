#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
#from ControlPort.GNURadio import ControlPort
from gnuradio.ControlPort.GNURadio import ControlPort
import sys

class ThriftRadioClient:
    def __init__(self, host, port):
        self.tsocket = TSocket.TSocket(host, port)
        self.transport = TTransport.TBufferedTransport(self.tsocket)
        self.protocol = TBinaryProtocol.TBinaryProtocol(self.transport)

        self.radio = ControlPort.Client(self.protocol)
        self.transport.open()

    def __del__(self):
        self.transport.close()

    def getRadio(self, host, port):
        return self.radio

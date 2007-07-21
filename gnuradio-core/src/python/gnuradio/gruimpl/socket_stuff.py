#
# Copyright 2005 Free Software Foundation, Inc.
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

# random socket related stuff

import socket
import os
import sys

def tcp_connect_or_die(sock_addr):
    """
    @param sock_addr: (host, port) to connect to
    @type sock_addr: tuple
    @returns: socket or exits
    """
    s = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect(sock_addr)
    except socket.error, err:
        sys.stderr.write('Failed to connect to %s: %s\n' %
                         (sock_addr, os.strerror (err.args[0]),))
        sys.exit(1)
    return s

def udp_connect_or_die(sock_addr):
    """
    @param sock_addr: (host, port) to connect to
    @type sock_addr: tuple
    @returns: socket or exits
    """
    s = socket.socket (socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(sock_addr)
    except socket.error, err:
        sys.stderr.write('Failed to connect to %s: %s\n' %
                         (sock_addr, os.strerror (err.args[0]),))
        sys.exit(1)
    return s

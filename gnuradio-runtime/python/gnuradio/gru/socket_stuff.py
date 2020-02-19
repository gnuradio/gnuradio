from __future__ import unicode_literals
#
# Copyright 2005 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

# random socket related stuff

import socket
import os
import sys

def tcp_connect_or_die(sock_addr):
    """

    Args:
        sock_addr: (host, port) to connect to (tuple)

    Returns:
        : socket or exits
    """
    s = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect(sock_addr)
    except socket.error as err:
        sys.stderr.write('Failed to connect to %s: %s\n' %
                         (sock_addr, os.strerror (err.args[0]),))
        sys.exit(1)
    return s

def udp_connect_or_die(sock_addr):
    """

    Args:
        sock_addr: (host, port) to connect to (tuple)

    Returns:
        : socket or exits
    """
    s = socket.socket (socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(sock_addr)
    except socket.error as err:
        sys.stderr.write('Failed to connect to %s: %s\n' %
                         (sock_addr, os.strerror (err.args[0]),))
        sys.exit(1)
    return s

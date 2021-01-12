#
# Copyright 2009,2019,2020 Free Software Foundation, Inc.
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

import socket
import os
from gnuradio import gr, blocks

def _get_sock_fd(addr, port, server):
    """
    Get the file descriptor for the socket.
    As a client, block on connect, dup the socket descriptor.
    As a server, block on accept, dup the client descriptor.

    Args:
        addr: the ip address string
        port: the tcp port number
        server: true for server mode, false for client mode

    Returns:
        the file descriptor number
    """
    is_ipv6 = False

    if ":" in addr:
        sock = socket.socket(socket.AF_INET6, socket.SOCK_STREAM, 0)
        is_ipv6 = True
    else:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    if server:
        try:
            if is_ipv6:
                bind_addr = addr.replace("::ffff:", "")
                sock.bind((bind_addr, port))
            else:
                sock.bind((addr, port))

            gr.log.info('Waiting for a connection on port ' + str(port))

            sock.listen(1)
            clientsock, address = sock.accept()
            return os.dup(clientsock.fileno())
        except OSError as e:
            gr.log.error('Unable to bind to port ' + str(port))
            gr.log.error('Error: ' + e.strerror)

            if is_ipv6:
                gr.log.error('IPv6 HINT: If trying to start a local listener, '
                             'try "::" for the address.')
            return None
        except:
            gr.log.error('Unable to bind to port ' + str(port))
            return None

    else:
        sock.connect((addr, port))
        return os.dup(sock.fileno())

class tcp_source(gr.hier_block2):
    def __init__(self, itemsize, addr, port, server=True):
        #init hier block
        gr.hier_block2.__init__(
            self, 'tcp_source',
            gr.io_signature(0, 0, 0),
            gr.io_signature(1, 1, itemsize),
        )
        fd = _get_sock_fd(addr, port, server)
        self.connect(blocks.file_descriptor_source(itemsize, fd), self)

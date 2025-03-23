#
# Copyright 2009,2019,2020,2025 Free Software Foundation, Inc.
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

import socket, os, threading
class tcp_sink_source(gr.hier_block2):
    def __init__(self, itemsize, addr, port, server=True, mtu=1024):
        # init hier block
        gr.hier_block2.__init__(
            self, 'tcp_sink_source',
            gr.io_signature(1, 1, itemsize),
            gr.io_signature(1, 1, itemsize),
        )

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
                client_socket, address = sock.accept()

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
            client_socket = sock

        self.recv_read_fd, self.recv_write_fd = os.pipe()
        self.send_read_fd, self.send_write_fd = os.pipe()

        def socket_to_pipe():
            with os.fdopen(self.recv_write_fd, 'wb') as pipe_out:
                while self.threads_run:
                    data = client_socket.recv(mtu)
                    if not data:
                        break
                    pipe_out.write(data)
                    pipe_out.flush()

        def pipe_to_socket():
            with os.fdopen(self.send_read_fd, 'rb') as pipe_in:
                while self.threads_run:
                    data = pipe_in.read(mtu)
                    if not data:
                        break
                    client_socket.sendall(data)

        self.threads_run = True
        self.threads = (threading.Thread(target=socket_to_pipe, daemon=True),
                        threading.Thread(target=pipe_to_socket, daemon=True))
        for t in self.threads:
            t.start()

        self.connect(self, blocks.file_descriptor_sink(itemsize, self.send_write_fd))
        self.connect(blocks.file_descriptor_source(itemsize, self.recv_read_fd), self)

    def __del__(self):
        try:
            self.threads_run = False
            for t in self.threads:
                t.join()
        except AttributeError:
            pass

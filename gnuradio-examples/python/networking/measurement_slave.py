#!/usr/bin/env python
#
# Copyright 2006 Free Software Foundation, Inc.
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

import struct
import socket
import asyncore
import sys
import optparse
import random
from gnuradio.eng_option import eng_option
import gnuradio.gr.gr_threading as _threading

LOOP_TIMEOUT = 0.001
BROADCAST_ADDR = '255.255.255.255'

BROADCAST_PORT = 27010   # UDP
CONTROL_PORT   = 27011   # TCP

PKT_HEADER_SIZE = 4      # 32-bit int

logfile = None

def unpack_header(s):
    (len,) = struct.unpack('!i', s)
    return (len,)

def pack_header(len):
    return struct.pack('!i', len)


class control_port_listener(asyncore.dispatcher):
    def __init__(self, port=CONTROL_PORT, udp_socket=None, verbose=False):
        """
        @param port: TCP port to listen on.
        @type port: int
        """
        asyncore.dispatcher.__init__(self)

        self._verbose = verbose
        self._udp_socket = udp_socket

        host = ''                       # symbolic name for localhost
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.set_reuse_addr()

        try:
            self.bind((host, port))
        except socket.error, err:
            sys.stderr.write('Failed to bind to %s: %s\n' %
                             ((host, port), os.strerror (err.args[0]),))
            sys.exit(1)

        self.listen(3)
        
    def handle_accept(self):
        socket, addr = self.accept()
        sys.stderr.write("handle_accept: %r\n" % (addr,))
        if not(socket is None):
            # instantiate a server
            s = control_server(socket, addr, self._udp_socket, self._verbose)
        

class gr_dispatcher(asyncore.dispatcher):
    def __init__(self, sock=None, map=None):
        asyncore.dispatcher.__init__(self, sock=sock, map=map)

    def read_packet(self):
        """
        Returns None or packet
        """
        s = self.recvall(PKT_HEADER_SIZE)
        if s == '':
            return None
        
        (payload_len,) = unpack_header(s)

        payload_len = int(payload_len)
        
        if payload_len == 0:
            payload = ''
        else:
            payload = self.recvall(payload_len)
            if len(payload) != payload_len:
                sys.stderr.write ('short recv, expected %d bytes, got %d\n' % (
                    payload_len, len(payload)))
                raise RuntimeError, "short recv"

        return payload

    def recvall(self, buffer_size):
        result = ''
        while len(result) < buffer_size:
            data = self.recv(buffer_size - len(result))
            if not data:
                return ''
            result += data
        return result


class pkt_receiver_thread(_threading.Thread):
    def __init__(self, socket):
        _threading.Thread.__init__(self)
        self.setDaemon(1)
        self.socket = socket
        self.keep_running = True
        self.start()

    def run(self):
        while self.keep_running:
            pkt, sender = self.socket.recvfrom(10000)
            if pkt:
                if len(pkt) > 2:
                    t = struct.unpack('!H', pkt[0:2])
                    seqno = t[0]
                else:
                    seqno = -1
                    
                logfile.write('RCVD seqno %4d len %4d from %s\n' % (seqno, len(pkt), sender))
                logfile.flush()


class control_server(gr_dispatcher):
    def __init__(self, socket, addr, udp_socket, verbose=False):
        gr_dispatcher.__init__(self, sock=socket)
        
        self._udp_socket = udp_socket
        self.verbose = verbose
        self.setblocking(1)

    def writable(self):
        return False

    def handle_read(self):
        pkt = self.read_packet()
        if pkt:
            annotate = 'ANNOTATE'
            if pkt.startswith(annotate):
                logfile.write(pkt[len(annotate)+1:])
                logfile.write('\n')
                logfile.flush()
            elif pkt.startswith('SEND'):
                tokens = pkt.split()
                if len(tokens) < 4:
                    invalid_packet(pkt)
                else:
                    npkts = int(tokens[1])
                    size = int(tokens[2])
                    power = float(tokens[3])
                    send_test_packets(self._udp_socket, npkts, size, power)
            else:
                invalid_packet(pkt)

    def handle_close(self):
        self.close()


def invalid_packet(pkt):
    sys.stderr.write('received unrecognized packet: %s\n' % (pkt,))


def make_random_payload(size):
    p = [0] * size
    if 1:
        for i in range(size):
            p[i] = chr(random.randint(0, 255))
    else:
        for i in range(size):
            p[i] = chr(i % 256)
    return ''.join(p)


def send_test_packets(udp_socket, npkts, size, power):
    # we ignore power for now...
    size = max(2, size)
    payload = make_random_payload(size - 2)
    for n in range(npkts):
        pkt = struct.pack('!H', n) + payload
        udp_socket.sendto(pkt, (BROADCAST_ADDR, BROADCAST_PORT))
        #udp_socket.sendall(pkt)


def open_udp_broadcast_socket(gr0_host_ip, port):
    s  = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#    s.bind((gr0_host_ip, port))
    s.bind(('', port))
    s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
#    s.connect((BROADCAST_ADDR, port))
    return s


def main():
    global logfile
    
    usage = 'usage: %prog [options] gr0-ip-addr'
    parser = optparse.OptionParser (option_class=eng_option, usage=usage)
    parser.add_option('-l', '--logfile', type='string', default=None,
                      help="specify log file name [default=<stdout>]")
    parser.add_option('-v', '--verbose', action="store_true", default=False,
                      help="enable verbose diagnostics")

    (options, args) = parser.parse_args ()
    if len(args) != 1:
        parser.print_help()
        sys.exit(1)

    gr0_ip_addr = args[0]
    if options.logfile is None:
        logfile = sys.stdout
    else:
        logfile = file(options.logfile, 'w')

    udp_socket = open_udp_broadcast_socket(gr0_ip_addr, BROADCAST_PORT)
    R = pkt_receiver_thread(udp_socket)
    L = control_port_listener(CONTROL_PORT, udp_socket=udp_socket, verbose=options.verbose)
    asyncore.loop(LOOP_TIMEOUT)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass

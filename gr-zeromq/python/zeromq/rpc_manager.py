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

import zmq
import pmt
import threading


class rpc_manager():
    def __init__(self):
        self.zmq_context = zmq.Context()
        self.poller_rep = zmq.Poller()
        self.poller_req_out = zmq.Poller()
        self.poller_req_in = zmq.Poller()
        self.interfaces = dict()

    def __del__(self):
        self.stop_watcher()
        self.watcher_thread.join()

    def set_reply_socket(self, address):
        self.rep_socket = self.zmq_context.socket(zmq.REP)
        self.rep_socket.bind(address)
        print "[RPC] reply socket bound to: ", address
        self.poller_rep.register(self.rep_socket, zmq.POLLIN)

    def set_request_socket(self, address):
        self.req_socket = self.zmq_context.socket(zmq.REQ)
        self.req_socket.connect(address)
        print "[RPC] request socket connected to: ", address
        self.poller_req_out.register(self.req_socket, zmq.POLLOUT)
        self.poller_req_in.register(self.req_socket, zmq.POLLIN)

    def add_interface(self, id_str, callback_func):
        if not self.interfaces.has_key(id_str):
            self.interfaces[id_str] = callback_func
            print "[RPC] added reply interface:", id_str
        else:
            print "[RPC] ERROR: duplicate id_str:", id_str

    def watcher(self):
        self.keep_running = True
        while self.keep_running:
            # poll for calls
            socks = dict(self.poller_rep.poll(10))
            if socks.get(self.rep_socket) == zmq.POLLIN:
                # receive call
                msg = self.rep_socket.recv()
                (id_str, args) = pmt.to_python(pmt.deserialize_str(msg))
                print "[RPC] request:", id_str, ", args:", args
                reply = self.callback(id_str, args)
                self.rep_socket.send(pmt.serialize_str(pmt.to_pmt(reply)))

    def start_watcher(self):
        self.watcher_thread = threading.Thread(target=self.watcher,args=())
        self.watcher_thread.daemon = True
        self.watcher_thread.start()

    def stop_watcher(self):
        self.keep_running = False
        self.watcher_thread.join()

    def request(self, id_str, args=None):
        socks = dict(self.poller_req_out.poll(10))
        if socks.get(self.req_socket) == zmq.POLLOUT:
            self.req_socket.send(pmt.serialize_str(pmt.to_pmt((id_str,args))))
        socks = dict(self.poller_req_in.poll(10))
        if socks.get(self.req_socket) == zmq.POLLIN:
            reply = pmt.to_python(pmt.deserialize_str(self.req_socket.recv()))
            print "[RPC] reply:", reply
            return reply

    def callback(self, id_str, args):
        if self.interfaces.has_key(id_str):
            callback_func = self.interfaces.get(id_str)
            if not args == None:
                # use unpacking or splat operator * to unpack argument list
                return(callback_func(*args))
            else:
                return(callback_func())
        else:
            print "[RPC] ERROR: id_str not found:", id_str
            return None

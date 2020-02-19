#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from __future__ import print_function
from __future__ import unicode_literals

import zmq
import pmt
import threading


class rpc_manager(object):
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
        print("[RPC] reply socket bound to: ", address)
        self.poller_rep.register(self.rep_socket, zmq.POLLIN)

    def set_request_socket(self, address):
        self.req_socket = self.zmq_context.socket(zmq.REQ)
        self.req_socket.connect(address)
        print("[RPC] request socket connected to: ", address)
        self.poller_req_out.register(self.req_socket, zmq.POLLOUT)
        self.poller_req_in.register(self.req_socket, zmq.POLLIN)

    def add_interface(self, id_str, callback_func):
        if id_str not in self.interfaces:
            self.interfaces[id_str] = callback_func
            print("[RPC] added reply interface:", id_str)
        else:
            print("[RPC] ERROR: duplicate id_str:", id_str)

    def watcher(self):
        self.keep_running = True
        while self.keep_running:
            # poll for calls
            socks = dict(self.poller_rep.poll(10))
            if socks.get(self.rep_socket) == zmq.POLLIN:
                # receive call
                msg = self.rep_socket.recv()
                (id_str, args) = pmt.to_python(pmt.deserialize_str(msg))
                print("[RPC] request:", id_str, ", args:", args)
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
            print("[RPC] reply:", reply)
            return reply

    def callback(self, id_str, args):
        if id_str in self.interfaces:
            callback_func = self.interfaces.get(id_str)
            if not args == None:
                # use unpacking or splat operator * to unpack argument list
                return(callback_func(*args))
            else:
                return(callback_func())
        else:
            print("[RPC] ERROR: id_str not found:", id_str)
            return None

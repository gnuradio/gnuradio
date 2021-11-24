#
# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

"""
Python Client classes for interfacing with the GNU Radio ControlPort interface
and for accessing Performance Counters.

While ControlPort and these client classes are designed to support multiple
Remote Procedure Call (RPC) transports, the Apache Thrift middle-ware RPC
is currently the only supported transport.

"""

from gnuradio.ctrlport.RPCConnection import RPCMethods
try:
    from gnuradio.ctrlport.RPCConnectionThrift import RPCConnectionThrift
    from thrift.transport.TTransport import TTransportException
except ImportError:
    print("ControlPort requires Thrift, but it was not found!")
    pass


"""
GNURadioControlPortClient is the main class for creating a GNU Radio
ControlPort client application for all transports.

"""


class GNURadioControlPortClient(object):
    """
    Constructor for creating a ControlPort connection to a specified host / port

    Args:
        host: hostname of the connection. Specifying None (default) will
            select the loopback interface.

        port: port number to use for the connection. Specifying None (default)
            will select the specified RPC transport's default port number, if
            the transport has a default.

        rpcmethod: This string specifies the RPC transport to use for the
            client connection. The default implementation currently uses
            the Apache Thrift RPC transport. The value specified here must
            be one of the transport keys listed in the RPCMethods dictionary
            above

        callback: This optional parameter is a callback function that will be passed
            a reference to the Client implementation for the RPC transport specified
            by rpcmethod. The callback will be executed after the client has been
            constructed, but before __init__ returns.

        blockingcallback: This optional parameter is a callback function with
            no parameters that will be executed after callback() is executed,
            but before __init__ returns. It is useful if your application
            requires that a blocking function be called to start the application,
            such as QtGui.QApplication.exec_

    """

    def __init__(self, host=None, port=None, rpcmethod='thrift', callback=None, blockingcallback=None):
        self.client = None

        if rpcmethod in RPCMethods:
            if rpcmethod == 'thrift':
                self.client = RPCConnectionThrift(host, port)

                if not callback is None:
                    callback(self.client)

                if not blockingcallback is None:
                    blockingcallback()
        else:
            print(("Unsupported RPC method: ", rpcmethod))
            raise ValueError()

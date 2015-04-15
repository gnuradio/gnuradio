#
# Copyright 2015 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

"""
Python Client classes for interfacing with the GNU Radio ControlPort interface
and for accessing Performance Counters.

While ControlPort and these client classes are designed to support multiple
Remote Procedure Call (RPC) transports, the Apache Thrift middle-ware RPC
is currently the only supported transport.

"""

import exceptions

"""
GNURadioControlPortClient is the main class for creating a GNU Radio
ControlPort client application for all transports.

Two constructors are provided for creating a connection to ControlPort.

"""

class GNURadioControlPortClient():
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

    def __init__(self, host = None, port = None, rpcmethod = 'thrift', callback = None, blockingcallback = None):
        __init__([host, port], rpcmethod, callback, blockingcallback)

    """
    Constructor for creating a ControlPort from a tuple of command line arguments (i.e. sys.argv)

    Args:
        argv: List of command line arguments. Future implementations may parse the argument list
            for OptionParser style key / value pairs, however the current implementation
            simply takes argv[1] and argv[2] as the connection hostname and port, respectively.

    Example Usage:

        In the following QT client example, the ControlPort host and port are specified to
        the Client application as the first two command line arguments. The MAINWindow class is
        of the type QtGui.QMainWindow, and is the main window for the QT application. MyApp
        is a simple helper class for starting the application.

        class MAINWindow(QtGui.QMainWindow):
            ... QT Application implementation ...

        class MyApp(object):
            def __init__(self, args):
                from GNURadioControlPortClient import GNURadioControlPortClient
                GNURadioControlPortClient(args, 'thrift', self.run, QtGui.QApplication(sys.argv).exec_)

            def run(self, client):
                MAINWindow(client).show()

        MyApp(sys.argv)


    """

    def __init__(self, argv = [], rpcmethod = 'thrift', callback = None, blockingcallback = None):
        if len(argv) > 1: host = argv[1]
        else: host = None

        if len(argv) > 2: port = argv[2]
        else: port = None

        self.client = None

        from gnuradio.ctrlport.RPCConnection import RPCMethods
        if RPCMethods.has_key(rpcmethod):
            from gnuradio.ctrlport.RPCConnectionThrift import RPCConnectionThrift
            if rpcmethod == 'thrift':
                #print("making RPCConnectionThrift")
                self.client = RPCConnectionThrift(host, port)
                #print("made %s" % self.client)

                #print("making callback call")
                if not callback is None:
                    callback(self.client)

                #print("making blockingcallback call")
                if not blockingcallback is None:
                    blockingcallback()
        else:
            print("Unsupported RPC method: ", rpcmethod)
            raise exceptions.ValueError()

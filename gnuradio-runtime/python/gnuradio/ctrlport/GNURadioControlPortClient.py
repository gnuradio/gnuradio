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

"""
RPCMethods is a dictionary listing RPC transports currently supported
by this client.

Args:
    function: the function whose parameter list will be examined
    excluded_args: function arguments that are NOT to be added to the dictionary (sequence of strings)
    options: result of command argument parsing (optparse.Values)
"""

RPCMethods = {'thrift': 'Apache Thrift',
             #'ice': 'Zeroc ICE'
             }

import exceptions

"""
Base class for RPC transport clients

Methods that all RPC clients should implement include:

    def newConnection(host,port): Method for re-establishing a new client
        connection to a different host / port

    def properties([]): Given a list of ControlPort property names,
        or an empty list to specify all currently registered properties,
        this method returns a dictionary of metadata describing the
        the specified properties. The dictionary key contains the name
        of each returned properties.

    def getKnobs([]): Given a list of ControlPort property names,
        or an empty list to specify all currently registered properties,
        this method returns a dictionary of the current value of
        the specified properties.

    def getRe([]): Given a list of regular expression strings,
        this method returns a dictionary of the current value of
        the all properties with names that match the specified
        expressions.

    def setKnobs({}): Given a dictionary of ControlPort property
        key / value pairs, this method requests that ControlPort
        attempt to set the specified named properties to the
        value given. Success in setting each property to the
        value specified requires that the property be registered
        as a 'setable' ControlPort property, that the client have the
        requisite privilege level to set the property, and
        the underlying Block's implementation in handling
        the set request.

Args:
    method: name of the RPC transport
    port: port number of the connection
    host: hostname of the connection
"""

class RPCConnection(object):
    def __init__(self, method, port, host=None):
        (self.method, self.port) = (method, port)
        if host is None: self.host = '127.0.0.1'
        else: self.host = host

    def __str__(self):
        return "%s connection on %s:%s"%(self.getName(), self.getHost(), self.getPort())

    def getName(self):
        return RPCMethods[self.method]

    def getHost(self):
        return self.host

    def getPort(self):
        return self.port

    def newConnection(self, host=None, port=None):
        raise exceptions.NotImplementedError()

    def properties(self, *args):
        raise exceptions.NotImplementedError()

    def getKnobs(self, *args):
        raise exceptions.NotImplementedError()

    def getRe(self,*args):
        raise exceptions.NotImplementedError()

    def setKnobs(self,*args):
        raise exceptions.NotImplementedError()

    def shutdown(self):
        raise exceptions.NotImplementedError()

"""
RPC Client interface for the Apache Thrift middle-ware RPC transport.

Args:
    port: port number of the connection
    host: hostname of the connection
"""

from gnuradio.ctrlport.GNURadio.ttypes import BaseTypes

class RPCConnectionThrift(RPCConnection):
    def __init__(self, host=None, port=None):
        if port is None: port = 9090
        super(RPCConnectionThrift, self).__init__(method='thrift', port=port, host=host)
        self.newConnection(host, port)

    def newConnection(self, host=None, port=None):
        from gnuradio.ctrlport.ThriftRadioClient import ThriftRadioClient
        self.thriftclient = ThriftRadioClient(self.getHost(), self.getPort())

    def properties(self, *args):
        return self.thriftclient.radio.properties(*args)

    def getKnobs(self, *args):
        class Knob():
            def __init__(self, key, value):
                (self.key, self.value) = (key, value)

        result = {}
        for key, knob in self.thriftclient.radio.getKnobs(*args).iteritems():
            if knob.type ==   BaseTypes.BOOL:      result[key] = Knob(key, knob.value.a_bool)
            elif knob.type == BaseTypes.BYTE:      result[key] = Knob(key, knob.value.a_byte)
            elif knob.type == BaseTypes.SHORT:     result[key] = Knob(key, knob.value.a_short)
            elif knob.type == BaseTypes.INT:       result[key] = Knob(key, knob.value.a_int)
            elif knob.type == BaseTypes.LONG:      result[key] = Knob(key, knob.value.a_long)
            elif knob.type == BaseTypes.DOUBLE:    result[key] = Knob(key, knob.value.a_double)
            elif knob.type == BaseTypes.STRING:    result[key] = Knob(key, knob.value.a_string)
            elif knob.type == BaseTypes.COMPLEX:   result[key] = Knob(key, knob.value.a_complex)
            elif knob.type == BaseTypes.F32VECTOR: result[key] = Knob(key, knob.value.a_f32vector)
            elif knob.type == BaseTypes.F64VECTOR: result[key] = Knob(key, knob.value.a_f64vector)
            elif knob.type == BaseTypes.S64VECTOR: result[key] = Knob(key, knob.value.a_s64vector)
            elif knob.type == BaseTypes.S32VECTOR: result[key] = Knob(key, knob.value.a_s32vector)
            elif knob.type == BaseTypes.S16VECTOR: result[key] = Knob(key, knob.value.a_s16vector)
            elif knob.type == BaseTypes.S8VECTOR:  result[key] = Knob(key, knob.value.a_s8vector)
            elif knob.type == BaseTypes.C32VECTOR: result[key] = Knob(key, knob.value.a_c32vector)
            else:
                raise exceptions.ValueError

        return result

    def getRe(self,*args):
        class Knob():
            def __init__(self, key, value):
                (self.key, self.value) = (key, value)

        result = {}
        for key, knob in self.thriftclient.radio.getRe(*args).iteritems():
            if knob.type ==   BaseTypes.BOOL:      result[key] = Knob(key, knob.value.a_bool)
            elif knob.type == BaseTypes.BYTE:      result[key] = Knob(key, knob.value.a_byte)
            elif knob.type == BaseTypes.SHORT:     result[key] = Knob(key, knob.value.a_short)
            elif knob.type == BaseTypes.INT:       result[key] = Knob(key, knob.value.a_int)
            elif knob.type == BaseTypes.LONG:      result[key] = Knob(key, knob.value.a_long)
            elif knob.type == BaseTypes.DOUBLE:    result[key] = Knob(key, knob.value.a_double)
            elif knob.type == BaseTypes.STRING:    result[key] = Knob(key, knob.value.a_string)
            elif knob.type == BaseTypes.COMPLEX:   result[key] = Knob(key, knob.value.a_complex)
            elif knob.type == BaseTypes.F32VECTOR: result[key] = Knob(key, knob.value.a_f32vector)
            elif knob.type == BaseTypes.F64VECTOR: result[key] = Knob(key, knob.value.a_f64vector)
            elif knob.type == BaseTypes.S64VECTOR: result[key] = Knob(key, knob.value.a_s64vector)
            elif knob.type == BaseTypes.S32VECTOR: result[key] = Knob(key, knob.value.a_s32vector)
            elif knob.type == BaseTypes.S16VECTOR: result[key] = Knob(key, knob.value.a_s16vector)
            elif knob.type == BaseTypes.S8VECTOR:  result[key] = Knob(key, knob.value.a_s8vector)
            elif knob.type == BaseTypes.C32VECTOR: result[key] = Knob(key, knob.value.a_c32vector)
            else:
                raise exceptions.ValueError
        return result

    def setKnobs(self,*args):
        self.thriftclient.radio.setKnobs(*args)

    def shutdown(self):
        self.thriftclient.radio.shutdown()

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

        if RPCMethods.has_key(rpcmethod):
            if rpcmethod == 'thrift':
#                 print("making RPCConnectionThrift")
                self.client = RPCConnectionThrift(host, port)
#                 print("made %s" % self.client)

#                 print("making callback call")
                if not callback is None: callback(self.client)

#                 print("making blockingcallback call")
                if not blockingcallback is None: blockingcallback()
        else:
            print("Unsupported RPC method: ", rpcmethod)
            raise exceptions.ValueError()

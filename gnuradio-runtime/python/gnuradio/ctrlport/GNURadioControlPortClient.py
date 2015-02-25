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

import sys, exceptions

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

    def printProperties(self, props):
        raise exceptions.NotImplementedError()

"""
RPC Client interface for the Apache Thrift middle-ware RPC transport.

Args:
    port: port number of the connection
    host: hostname of the connection
"""

class RPCConnectionThrift(RPCConnection):
    class Knob():
        def __init__(self, key, value=None, ktype=0):
            (self.key, self.value, self.ktype) = (key, value, ktype)

        def __repr__(self):
            return "({0} = {1})".format(self.key, self.value)

    def __init__(self, host=None, port=None):
        from gnuradio.ctrlport.GNURadio import ttypes
        self.BaseTypes = ttypes.BaseTypes
        self.KnobBase = ttypes.KnobBase

        if port is None:
            port = 9090
        super(RPCConnectionThrift, self).__init__(method='thrift', port=port, host=host)
        self.newConnection(host, port)

        self.unpack_dict = {
            self.BaseTypes.BOOL:      lambda k,b: self.Knob(k, b.value.a_bool, self.BaseTypes.BOOL),
            self.BaseTypes.BYTE:      lambda k,b: self.Knob(k, b.value.a_byte, self.BaseTypes.BYTE),
            self.BaseTypes.SHORT:     lambda k,b: self.Knob(k, b.value.a_short, self.BaseTypes.SHORT),
            self.BaseTypes.INT:       lambda k,b: self.Knob(k, b.value.a_int, self.BaseTypes.INT),
            self.BaseTypes.LONG:      lambda k,b: self.Knob(k, b.value.a_long, self.BaseTypes.LONG),
            self.BaseTypes.DOUBLE:    lambda k,b: self.Knob(k, b.value.a_double, self.BaseTypes.DOUBLE),
            self.BaseTypes.STRING:    lambda k,b: self.Knob(k, b.value.a_string, self.BaseTypes.STRING),
            self.BaseTypes.COMPLEX:   lambda k,b: self.Knob(k, b.value.a_complex, self.BaseTypes.COMPLEX),
            self.BaseTypes.F32VECTOR: lambda k,b: self.Knob(k, b.value.a_f32vector, self.BaseTypes.F32VECTOR),
            self.BaseTypes.F64VECTOR: lambda k,b: self.Knob(k, b.value.a_f64vector, self.BaseTypes.F64VECTOR),
            self.BaseTypes.S64VECTOR: lambda k,b: self.Knob(k, b.value.a_s64vector, self.BaseTypes.S64VECTOR),
            self.BaseTypes.S32VECTOR: lambda k,b: self.Knob(k, b.value.a_s32vector, self.BaseTypes.S32VECTOR),
            self.BaseTypes.S16VECTOR: lambda k,b: self.Knob(k, b.value.a_s16vector, self.BaseTypes.S16VECTOR),
            self.BaseTypes.S8VECTOR:  lambda k,b: self.Knob(k, b.value.a_s8vector, self.BaseTypes.S8VECTOR),
            self.BaseTypes.C32VECTOR: lambda k,b: self.Knob(k, b.value.a_c32vector, self.BaseTypes.C32VECTOR),
        }

        self.pack_dict = {
            self.BaseTypes.BOOL:      lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_bool = k.value)),
            self.BaseTypes.BYTE:      lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_byte = k.value)),
            self.BaseTypes.SHORT:     lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_short = k.value)),
            self.BaseTypes.INT:       lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_int = k.value)),
            self.BaseTypes.LONG:      lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_long = k.value)),
            self.BaseTypes.DOUBLE:    lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_double = k.value)),
            self.BaseTypes.STRING:    lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_string = k.value)),
            self.BaseTypes.COMPLEX:   lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_complex = k.value)),
            self.BaseTypes.F32VECTOR: lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_f32vector = k.value)),
            self.BaseTypes.F64VECTOR: lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_f64vector = k.value)),
            self.BaseTypes.S64VECTOR: lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_s64vector = k.value)),
            self.BaseTypes.S32VECTOR: lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_s32vector = k.value)),
            self.BaseTypes.S16VECTOR: lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_s16vector = k.value)),
            self.BaseTypes.S8VECTOR:  lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_s8vector = k.value)),
            self.BaseTypes.C32VECTOR: lambda k: ttypes.Knob(type=k.ktype, value=ttypes.KnobBase(a_c32vector = k.value)),
        }

    def unpackKnob(self, key, knob):
        f = self.unpack_dict.get(knob.type, None)
        if(f):
            return f(key, knob)
        else:
            sys.stderr.write("unpackKnobs: Incorrect Knob type: {0}\n".format(knob.type))
            raise exceptions.ValueError

    def packKnob(self, knob):
        f = self.pack_dict.get(knob.ktype, None)
        if(f):
            return f(knob)
        else:
            sys.stderr.write("packKnobs: Incorrect Knob type: {0}\n".format(knob.type))
            raise exceptions.ValueError

    def newConnection(self, host=None, port=None):
        from gnuradio.ctrlport.ThriftRadioClient import ThriftRadioClient
        import thrift
        try:
            self.thriftclient = ThriftRadioClient(self.getHost(), self.getPort())
        except thrift.transport.TTransport.TTransportException:
            sys.stderr.write("Could not connect to ControlPort endpoint at {0}:{1}.\n\n".format(host, port))
            sys.exit(1)

    def properties(self, *args):
        knobprops = self.thriftclient.radio.properties(*args)
        for key, knobprop in knobprops.iteritems():
            #print("key:", key, "value:", knobprop, "type:", knobprop.type)
            knobprops[key].min = self.unpackKnob(key, knobprop.min)
            knobprops[key].max = self.unpackKnob(key, knobprop.max)
            knobprops[key].defaultvalue = self.unpackKnob(key, knobprop.defaultvalue)
        return knobprops

    def getKnobs(self, *args):
        result = {}
        for key, knob in self.thriftclient.radio.getKnobs(*args).iteritems():
            #print("key:", key, "value:", knob, "type:", knob.type)
            result[key] = self.unpackKnob(key, knob)
        return result

    def getKnobsRaw(self, *args):
        result = {}
        for key, knob in self.thriftclient.radio.getKnobs(*args).iteritems():
            #print("key:", key, "value:", knob, "type:", knob.type)
            result[key] = knob
        return result

    def getRe(self,*args):
        result = {}
        for key, knob in self.thriftclient.radio.getRe(*args).iteritems():
            result[key] = self.unpackKnob(key, knob)
        return result

    def setKnobs(self, *args):
        if(type(*args) == dict):
            a = dict(*args)
            result = {}
            for key, knob in a.iteritems():
                result[key] = self.packKnob(knob)
            self.thriftclient.radio.setKnobs(result)
        elif(type(*args) == list or type(*args) == tuple):
            a = list(*args)
            result = {}
            for k in a:
                result[k.key] = self.packKnob(k)
            self.thriftclient.radio.setKnobs(result)
        else:
            sys.stderr.write("setKnobs: Invalid type; must be dict, list, or tuple\n")

    def shutdown(self):
        self.thriftclient.radio.shutdown()

    def printProperties(self, props):
        info = ""
        info += "Item:\t\t{0}\n".format(props.description)
        info += "units:\t\t{0}\n".format(props.units)
        info += "min:\t\t{0}\n".format(props.min.value)
        info += "max:\t\t{0}\n".format(props.max.value)
        info += "default:\t\t{0}\n".format(props.defaultvalue.value)
        info += "Type Code:\t0x{0:x}\n".format(props.type)
        info += "Disp Code:\t0x{0:x}\n".format(props.display)
        return info

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

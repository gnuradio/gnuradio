#
# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""
RPCMethods is a dictionary listing RPC transports currently supported
by this client.

Args:
    function: the function whose parameter list will be examined
    excluded_args: function arguments that are NOT to be added to the dictionary (sequence of strings)
    options: result of command argument parsing (optparse.Values)
"""


RPCMethods = {'thrift': 'Apache Thrift',
              # 'ice': 'Zeroc ICE'
              }


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
        if host is None:
            self.host = '127.0.0.1'
        else:
            self.host = host

    def __str__(self):
        return "%s connection on %s:%s" % (self.getName(), self.getHost(), self.getPort())

    def getName(self):
        return RPCMethods[self.method]

    def getHost(self):
        return self.host

    def getPort(self):
        return self.port

    def newConnection(self, host=None, port=None):
        raise NotImplementedError()

    def properties(self, *args):
        raise NotImplementedError()

    def getKnobs(self, *args):
        raise NotImplementedError()

    def getRe(self, *args):
        raise NotImplementedError()

    def postMessage(self, *args):
        raise NotImplementedError()

    def setKnobs(self, *args):
        raise NotImplementedError()

    def shutdown(self):
        raise NotImplementedError()

    def printProperties(self, props):
        raise NotImplementedError()

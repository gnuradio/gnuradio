#!/usr/bin/env python
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
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from gnuradio.ctrlport.GNURadio import ControlPort
from gnuradio.ctrlport import RPCConnection
from gnuradio import gr
import pmt
import sys

class ThriftRadioClient:
    def __init__(self, host, port):
        self.tsocket = TSocket.TSocket(host, port)
        self.transport = TTransport.TBufferedTransport(self.tsocket)
        self.protocol = TBinaryProtocol.TBinaryProtocol(self.transport)

        self.radio = ControlPort.Client(self.protocol)
        self.transport.open()

    def __del__(self):
        self.radio.shutdown()
        self.transport.close()

    def getRadio(self, host, port):
        return self.radio

"""
RPC Client interface for the Apache Thrift middle-ware RPC transport.

Args:
    port: port number of the connection
    host: hostname of the connection
"""

class RPCConnectionThrift(RPCConnection.RPCConnection):
    class Knob():
        def __init__(self, key, value=None, ktype=0):
            (self.key, self.value, self.ktype) = (key, value, ktype)

        def __repr__(self):
            return "({0} = {1})".format(self.key, self.value)

    def __init__(self, host=None, port=None):
        from gnuradio.ctrlport.GNURadio import ttypes
        self.BaseTypes = ttypes.BaseTypes
        self.KnobBase = ttypes.KnobBase

        # If not set by the user, get the port number from the thrift
        # config file, if one is set. Defaults to 9090 otherwise.
        if port is None:
            p = gr.prefs()
            thrift_config_file = p.get_string("ControlPort", "config", "");
            if(len(thrift_config_file) > 0):
                p.add_config_file(thrift_config_file)
                port = p.get_long("thrift", "port", 9090)
            else:
                port = 9090
        else:
            port = int(port)

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
        try:
            self.thriftclient = ThriftRadioClient(self.getHost(), self.getPort())
        except TTransport.TTransportException:
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

            # If complex, convert to Python complex
            # FIXME: better list iterator way to handle this?
            if(knob.type == self.BaseTypes.C32VECTOR):
                for i in xrange(len(result[key].value)):
                    result[key].value[i] = complex(result[key].value[i].re,
                                                   result[key].value[i].im)
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

    def postMessage(self, blk_alias, port, msg):
        '''
        blk_alias: the alias of the block we are posting the message
                   to; must have an open message port named 'port'.
                   Provide as a string.
        port: The name of the message port we are sending the message to.
              Provide as a string.
        msg: The actual message. Provide this as a PMT of the form
             right for the message port.
        The alias and port names are converted to PMT symbols and
        serialized. The msg is already a PMT and so just serialized.
        '''
        self.thriftclient.radio.postMessage(pmt.serialize_str(pmt.intern(blk_alias)),
                                            pmt.serialize_str(pmt.intern(port)),
                                            pmt.serialize_str(msg));

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

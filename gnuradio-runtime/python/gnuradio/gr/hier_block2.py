#
# Copyright 2006,2007,2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import functools

# from .runtime_swig import hier_block2_swig, dot_graph
from .gr_python import hier_block2_pb

import pmt


def _multiple_endpoints(func):
    @functools.wraps(func)
    def wrapped(self, *points):
        if not points:
            raise ValueError(
                "At least one block required for " + func.__name__)
        elif len(points) == 1:
            try:
                block = points[0].to_basic_block()
            except AttributeError:
                raise ValueError(
                    "At least two endpoints required for " + func.__name__)
            func(self, block)
        else:
            try:
                endp = [(p.to_basic_block(), 0) if hasattr(p, 'to_basic_block')
                        else (p[0].to_basic_block(), p[1]) for p in points]
            except (ValueError, TypeError, AttributeError) as err:
                raise ValueError("Unable to coerce endpoints: " + str(err))

            for (src, src_port), (dst, dst_port) in zip(endp, endp[1:]):
                func(self, src, src_port, dst, dst_port)
    return wrapped


def _optional_endpoints(func):
    @functools.wraps(func)
    def wrapped(self, src, srcport, dst=None, dstport=None):
        if dst is None and dstport is None:
            try:
                (src, srcport), (dst, dstport) = src, srcport
            except (ValueError, TypeError) as err:
                raise ValueError("Unable to coerce endpoints: " + str(err))
        func(self, src.to_basic_block(), srcport, dst.to_basic_block(), dstport)
    return wrapped


# This makes a 'has-a' relationship to look like an 'is-a' one.
#
# It allows Python classes to subclass this one, while passing through
# method calls to the C++ class shared pointer from SWIG.
#
# It also allows us to intercept method calls if needed
#
class hier_block2(object):
    """
    Subclass this to create a python hierarchical block.

    This is a python wrapper around the C++ hierarchical block implementation.
    Provides convenience functions and allows proper Python subclassing.
    """

    def __init__(self, name, input_signature, output_signature):
        """
        Create a hierarchical block with a given name and I/O signatures.
        """
        self._impl = hier_block2_pb(name, input_signature, output_signature)

    def __getattr__(self, name):
        """
        Pass-through member requests to the C++ object.
        """

        try:
            object.__getattribute__(self, "_impl")
        except AttributeError as exception:
            raise RuntimeError(
                "{0}: invalid state -- did you forget to call {0}.__init__ in "
                "a derived class?".format(object.__getattribute__(self.__class__, "__name__"))) from exception

        return getattr(self._impl, name)

    # FIXME: these should really be implemented
    # in the original C++ class (gr_hier_block2), then they would all be inherited here

    @_multiple_endpoints
    def connect(self, *args):
        """
        Connect two or more block endpoints.  An endpoint is either a (block, port)
        tuple or a block instance.  In the latter case, the port number is assumed
        to be zero.

        To connect the hierarchical block external inputs or outputs to internal block
        inputs or outputs, use 'self' in the connect call.

        If multiple arguments are provided, connect will attempt to wire them in series,
        interpreting the endpoints as inputs or outputs as appropriate.
        """
        self.primitive_connect(*args)

    @_multiple_endpoints
    def disconnect(self, *args):
        """
        Disconnect two or more endpoints in the flowgraph.

        To disconnect the hierarchical block external inputs or outputs to internal block
        inputs or outputs, use 'self' in the connect call.

        If more than two arguments are provided, they are disconnected successively.
        """
        self.primitive_disconnect(*args)

    @_optional_endpoints
    def msg_connect(self, *args):
        """
        Connect two message ports in the flowgraph.

        If only two arguments are provided, they must be endpoints (block, port)
        """
        self.primitive_msg_connect(*args)

    @_optional_endpoints
    def msg_disconnect(self, *args):
        """
        Disconnect two message ports in the flowgraph.

        If only two arguments are provided, they must be endpoints (block, port)
        """
        self.primitive_msg_disconnect(*args)

    def message_port_register_hier_in(self, portname):
        """
        Register a message port for this hier block
        """
        self.primitive_message_port_register_hier_in(pmt.intern(portname))

    def message_port_register_hier_out(self, portname):
        """
        Register a message port for this hier block
        """
        self.primitive_message_port_register_hier_out(pmt.intern(portname))

    # def dot_graph(self):
    #     """
    #     Return graph representation in dot language
    #     """
    #     return dot_graph(self._impl)

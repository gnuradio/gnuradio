#
# Copyright 2011-2012, 2018, 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import numpy
import ctypes

from . import gr_python as gr
from .gr_python import io_signature  # , io_signaturev
from .gr_python import block_gateway


########################################################################
# Magic to turn pointers into numpy arrays
# http://docs.scipy.org/doc/numpy/reference/arrays.interface.html
########################################################################
def pointer_to_ndarray(addr, dtype, nitems):
    class array_like(object):
        __array_interface__ = {
            'data': (int(addr), False),
            'typestr': dtype.base.str,
            'descr': dtype.base.descr,
            'shape': (nitems,) + dtype.shape,
            'strides': None,
            'version': 3
        }
    return numpy.asarray(array_like()).view(dtype.base)

########################################################################
# io_signature for Python
########################################################################


class py_io_signature(object):
    """
    Describes the type/number of ports for block input or output.
    """

    # Minimum and maximum number of ports, and a list of numpy types.
    def __init__(self, min_ports, max_ports, type_list):
        """
        Args:

        min_ports (int): minimum number of connected ports.

        max_ports (int): maximum number of connected ports. -1 indicates
        no limit.

        type_list (list[str]): numpy type names for each port. If the
        number of connected ports is greater than the number of types
        provided, the last type in the list is repeated.
        """
        self.__min_ports = min_ports
        self.__max_ports = max_ports
        self.__types = tuple(numpy.dtype(t) for t in type_list)

    def gr_io_signature(self):
        """
        Make/return a gr.io_signature. A non-empty list of sizes is
        required, even if there are no ports.
        """
        return io_signature.makev(self.__min_ports, self.__max_ports,
                                  [t.itemsize for t in self.__types] or [0])

    def port_types(self, nports):
        """
        Return data types for the first nports ports. If nports is
        smaller than the provided type list, return a truncated list. If
        larger, fill with the last type.
        """
        ntypes = len(self.__types)
        if ntypes == 0:
            return ()
        if nports <= ntypes:
            return self.__types[:nports]
        return self.__types + [self.__types[-1]] * (nports - ntypes)

    def __iter__(self):
        """
        Return the iterator over the maximum ports type list.
        """
        return iter(self.port_types(self.__max_ports))

    def __hash__(self):
        return hash((self.__min_ports, self.__max_ports, self.__types))

########################################################################
# The guts that make this into a gr block
########################################################################


class gateway_block(object):

    def __init__(self, name, in_sig, out_sig, block_type):
        self._decim = 1
        self._interp = 1
        self._block_type = block_type

        # Normalize the many Python ways of saying 'nothing' to '()'
        in_sig = in_sig or ()
        out_sig = out_sig or ()

        # Backward compatibility: array of type strings -> py_io_signature
        if type(in_sig) is py_io_signature:
            self.__in_sig = in_sig
        else:
            self.__in_sig = py_io_signature(len(in_sig), len(in_sig), in_sig)
        if type(out_sig) is py_io_signature:
            self.__out_sig = out_sig
        else:
            self.__out_sig = py_io_signature(
                len(out_sig), len(out_sig), out_sig)

        self.gateway = block_gateway(
            self, name, self.__in_sig.gr_io_signature(), self.__out_sig.gr_io_signature())

        self.msg_handlers = {}

    def __getattr__(self, name):
        """
        Pass-through member requests to the C++ object.
        """
        if not hasattr(self, "gateway"):
            raise RuntimeError(
                "{0}: invalid state -- did you forget to call {0}.__init__ in "
                "a derived class?".format(self.__class__.__name__))
        return getattr(self.gateway, name)

    def to_basic_block(self):
        """
        Makes this block connectable by hier/top block python
        """
        return self.gateway.to_basic_block()

    def fixed_rate_noutput_to_ninput(self, noutput_items):
        return int((noutput_items * self._decim / self._interp) + self.gateway.history() - 1)

    def handle_forecast(self, noutput_items, ninputs):
        """
        This is the handler function for forecast calls from
        block_gateway in c++ across pybind11 wrappers
        """
        return self.forecast(noutput_items, ninputs)

        # return ninput_items_required

    def forecast(self, noutput_items, ninputs):
        """
        forecast is only called from a general block
        this is the default implementation
        """
        ninput_items_required = [0] * ninputs
        for i in range(ninputs):
            ninput_items_required[i] = noutput_items + \
                self.gateway.history() - 1

        return ninput_items_required

    def handle_general_work(self, noutput_items,
                            ninput_items,
                            input_items,
                            output_items):

        ninputs = len(input_items)
        noutputs = len(output_items)
        in_types = self.in_sig().port_types(ninputs)
        out_types = self.out_sig().port_types(noutputs)

        ctypes.pythonapi.PyCapsule_GetPointer.restype = ctypes.c_void_p
        ctypes.pythonapi.PyCapsule_GetPointer.argtypes = [
            ctypes.py_object, ctypes.c_char_p]

        if self._block_type != gr.GW_BLOCK_GENERAL:
            ii = [pointer_to_ndarray(
                ctypes.pythonapi.PyCapsule_GetPointer(input_items[i], None),
                in_types[i],
                self.fixed_rate_noutput_to_ninput(noutput_items)
            ) for i in range(ninputs)]
        else:
            ii = [pointer_to_ndarray(
                ctypes.pythonapi.PyCapsule_GetPointer(input_items[i], None),
                in_types[i],
                ninput_items[i]
            ) for i in range(ninputs)]

        oo = [pointer_to_ndarray(
            ctypes.pythonapi.PyCapsule_GetPointer(output_items[i], None),
            out_types[i],
            noutput_items
        ) for i in range(noutputs)]

        if self._block_type != gr.GW_BLOCK_GENERAL:
            r = self.work(ii, oo)
            self.consume_items(r)
        else:
            r = self.general_work(ii, oo)

        return r

    def general_work(self, *args, **kwargs):
        """general work to be overloaded in a derived class"""
        raise NotImplementedError("general work not implemented")

    def work(self, *args, **kwargs):
        """work to be overloaded in a derived class"""
        raise NotImplementedError("work not implemented")

    def start(self):
        return True

    def stop(self):
        return True

    def in_sig(self):
        return self.__in_sig

    def out_sig(self):
        return self.__out_sig

    def set_msg_handler(self, which_port, handler_function):
        self.gateway.set_msg_handler_pybind(
            which_port, handler_function.__name__)
        # Save handler object in class so it's not garbage collected
        self.msg_handlers[which_port] = handler_function

    # TODO: Make gateway_block have an is-a instead of has-a relationship
    #  That way we don't have to wrap all these functions

    # def message_port_register_in(self, port_id):
    #     return self.gateway.message_port_register_in(port_id)

    # def message_port_register_out(self, port_id):
    #     return self.gateway.message_port_register_out(port_id)

    # def consume_each(self, how_many_items):
    #     return self.gateway.consume_each( how_many_items)

    # def consume(self, which_input, how_many_items):
    #     return self.gateway.consume(which_input, how_many_items)

    # def produce(self, which_output, how_many_items):
    #     return self.gateway.produce(which_output, how_many_items)


########################################################################
# Wrappers for the user to inherit from
########################################################################
class basic_block(gateway_block):
    """
    Args:
    name (str): block name

    in_sig (gr.py_io_signature): input port signature

    out_sig (gr.py_io_signature): output port signature

    For backward compatibility, a sequence of numpy type names is also
    accepted as an io signature.
    """

    def __init__(self, name, in_sig, out_sig):
        gateway_block.__init__(self,
                               name=name,
                               in_sig=in_sig,
                               out_sig=out_sig,
                               block_type=gr.GW_BLOCK_GENERAL
                               )

    def consume_items(self, nitems):
        pass


class sync_block(gateway_block):
    """
    Args:
    name (str): block name

    in_sig (gr.py_io_signature): input port signature

    out_sig (gr.py_io_signature): output port signature

    For backward compatibility, a sequence of numpy type names is also
    accepted as an io signature.
    """

    def __init__(self, name, in_sig, out_sig):
        gateway_block.__init__(self,
                               name=name,
                               in_sig=in_sig,
                               out_sig=out_sig,
                               block_type=gr.GW_BLOCK_SYNC
                               )
        self._decim = 1
        self._interp = 1

    def consume_items(self, nitems):
        if (nitems > 0):
            self.gateway.consume_each(nitems)


class decim_block(gateway_block):
    """
    Args:
    name (str): block name

    in_sig (gr.py_io_signature): input port signature

    out_sig (gr.py_io_signature): output port signature

    For backward compatibility, a sequence of numpy type names is also
    accepted as an io signature.
    """

    def __init__(self, name, in_sig, out_sig, decim):
        gateway_block.__init__(self,
                               name=name,
                               in_sig=in_sig,
                               out_sig=out_sig,
                               block_type=gr.GW_BLOCK_DECIM
                               )
        self._decim = decim
        self._interp = 1
        self.gateway.set_relative_rate(self._interp, self._decim)
        self.gateway.set_output_multiple(self._interp)

    def consume_items(self, nitems):
        if (nitems > 0):
            self.gateway.consume_each(int(nitems * self._decim))

    def forecast(self, noutput_items, ninputs):
        return [self.fixed_rate_noutput_to_ninput(noutput_items) for ii in range(ninputs)]


class interp_block(gateway_block):
    """
    Args:
    name (str): block name

    in_sig (gr.py_io_signature): input port signature

    out_sig (gr.py_io_signature): output port signature

    For backward compatibility, a sequence of numpy type names is also
    accepted as an io signature.
    """

    def __init__(self, name, in_sig, out_sig, interp):
        gateway_block.__init__(self,
                               name=name,
                               in_sig=in_sig,
                               out_sig=out_sig,
                               block_type=gr.GW_BLOCK_DECIM
                               )
        self._decim = 1
        self._interp = interp
        self.gateway.set_relative_rate(self._interp, self._decim)
        self.gateway.set_output_multiple(self._interp)

    def consume_items(self, nitems):
        if (nitems > 0):
            self.gateway.consume_each(int(nitems / self._interp))

    def forecast(self, noutput_items, ninputs):
        # print("{},{},{}".format(noutput_items, ninputs, [self.fixed_rate_noutput_to_ninput(noutput_items)  for ii in range(ninputs)]))

        return [self.fixed_rate_noutput_to_ninput(noutput_items) for ii in range(ninputs)]

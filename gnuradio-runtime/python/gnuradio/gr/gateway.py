#
# Copyright 2011-2012 Free Software Foundation, Inc.
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

import runtime_swig as gr
from runtime_swig import io_signature, io_signaturev
from runtime_swig import block_gw_message_type
from runtime_swig import block_gateway
import numpy

########################################################################
# Magic to turn pointers into numpy arrays
# http://docs.scipy.org/doc/numpy/reference/arrays.interface.html
########################################################################
def pointer_to_ndarray(addr, dtype, nitems):
    class array_like:
        __array_interface__ = {
            'data' : (int(addr), False),
            'typestr' : dtype.base.str,
            'descr' : dtype.base.descr,
            'shape' : (nitems,) + dtype.shape,
            'strides' : None,
            'version' : 3
        }
    return numpy.asarray(array_like()).view(dtype.base)

########################################################################
# Handler that does callbacks from C++
########################################################################
class gateway_handler(gr.feval_ll):

    #dont put a constructor, it wont work

    def init(self, callback):
        self._callback = callback

    def eval(self, arg):
        try: self._callback()
        except Exception as ex:
            print("handler caught exception: %s"%ex)
            import traceback; traceback.print_exc()
            raise ex
        return 0

########################################################################
# Handler that does callbacks from C++
########################################################################
class msg_handler(gr.feval_p):

    #dont put a constructor, it wont work

    def init(self, callback):
        self._callback = callback

    def eval(self, arg):
        try: self._callback(arg)
        except Exception as ex:
            print("handler caught exception: %s"%ex)
            import traceback; traceback.print_exc()
            raise ex
        return 0

########################################################################
# The guts that make this into a gr block
########################################################################
class gateway_block(object):

    def __init__(self, name, in_sig, out_sig, work_type, factor):

        #ensure that the sigs are iterable dtypes
        def sig_to_dtype_sig(sig):
            if sig is None: sig = ()
            return map(numpy.dtype, sig)
        self.__in_sig = sig_to_dtype_sig(in_sig)
        self.__out_sig = sig_to_dtype_sig(out_sig)

        #cache the ranges to iterate when dispatching work
        self.__in_indexes = range(len(self.__in_sig))
        self.__out_indexes = range(len(self.__out_sig))

        #convert the signatures into gr.io_signatures
        def sig_to_gr_io_sigv(sig):
            if not len(sig): return io_signature(0, 0, 0)
            return io_signaturev(len(sig), len(sig), [s.itemsize for s in sig])
        gr_in_sig = sig_to_gr_io_sigv(self.__in_sig)
        gr_out_sig = sig_to_gr_io_sigv(self.__out_sig)

        #create internal gateway block
        self.__handler = gateway_handler()
        self.__handler.init(self.__gr_block_handle)
        self.__gateway = block_gateway(
            self.__handler, name, gr_in_sig, gr_out_sig, work_type, factor)
        self.__message = self.__gateway.block_message()

        #dict to keep references to all message handlers
        self.__msg_handlers = {}

        #register block functions
        prefix = 'block__'
        for attr in [x for x in dir(self.__gateway) if x.startswith(prefix)]:
            setattr(self, attr.replace(prefix, ''), getattr(self.__gateway, attr))
        self.pop_msg_queue = lambda: gr.block_gw_pop_msg_queue_safe(self.__gateway)

    def to_basic_block(self):
        """
        Makes this block connectable by hier/top block python
        """
        return self.__gateway.to_basic_block()

    def __gr_block_handle(self):
        """
        Dispatch tasks according to the action type specified in the message.
        """
        if self.__message.action == gr.block_gw_message_type.ACTION_GENERAL_WORK:
            self.__message.general_work_args_return_value = self.general_work(

                input_items=[pointer_to_ndarray(
                    self.__message.general_work_args_input_items[i],
                    self.__in_sig[i],
                    self.__message.general_work_args_ninput_items[i]
                ) for i in self.__in_indexes],

                output_items=[pointer_to_ndarray(
                    self.__message.general_work_args_output_items[i],
                    self.__out_sig[i],
                    self.__message.general_work_args_noutput_items
                ) for i in self.__out_indexes],
            )

        elif self.__message.action == gr.block_gw_message_type.ACTION_WORK:
            self.__message.work_args_return_value = self.work(

                input_items=[pointer_to_ndarray(
                    self.__message.work_args_input_items[i],
                    self.__in_sig[i],
                    self.__message.work_args_ninput_items
                ) for i in self.__in_indexes],

                output_items=[pointer_to_ndarray(
                    self.__message.work_args_output_items[i],
                    self.__out_sig[i],
                    self.__message.work_args_noutput_items
                ) for i in self.__out_indexes],
            )

        elif self.__message.action == gr.block_gw_message_type.ACTION_FORECAST:
            self.forecast(
                noutput_items=self.__message.forecast_args_noutput_items,
                ninput_items_required=self.__message.forecast_args_ninput_items_required,
            )

        elif self.__message.action == gr.block_gw_message_type.ACTION_START:
            self.__message.start_args_return_value = self.start()

        elif self.__message.action == gr.block_gw_message_type.ACTION_STOP:
            self.__message.stop_args_return_value = self.stop()

    def forecast(self, noutput_items, ninput_items_required):
        """
        forecast is only called from a general block
        this is the default implementation
        """
        for ninput_item in ninput_items_required:
            ninput_item = noutput_items + self.history() - 1;
        return

    def general_work(self, *args, **kwargs):
        """general work to be overloaded in a derived class"""
        raise NotImplementedError("general work not implemented")

    def work(self, *args, **kwargs):
        """work to be overloaded in a derived class"""
        raise NotImplementedError("work not implemented")

    def start(self): return True
    def stop(self): return True

    def set_msg_handler(self, which_port, handler_func):
        handler = msg_handler()
        handler.init(handler_func)
        self.__gateway.set_msg_handler_feval(which_port, handler)
        # Save handler object in class so it's not garbage collected
        self.__msg_handlers[which_port] = handler

########################################################################
# Wrappers for the user to inherit from
########################################################################
class basic_block(gateway_block):
    def __init__(self, name, in_sig, out_sig):
        gateway_block.__init__(self,
            name=name,
            in_sig=in_sig,
            out_sig=out_sig,
            work_type=gr.GR_BLOCK_GW_WORK_GENERAL,
            factor=1, #not relevant factor
        )

class sync_block(gateway_block):
    def __init__(self, name, in_sig, out_sig):
        gateway_block.__init__(self,
            name=name,
            in_sig=in_sig,
            out_sig=out_sig,
            work_type=gr.GR_BLOCK_GW_WORK_SYNC,
            factor=1,
        )

class decim_block(gateway_block):
    def __init__(self, name, in_sig, out_sig, decim):
        gateway_block.__init__(self,
            name=name,
            in_sig=in_sig,
            out_sig=out_sig,
            work_type=gr.GR_BLOCK_GW_WORK_DECIM,
            factor=decim,
        )

class interp_block(gateway_block):
    def __init__(self, name, in_sig, out_sig, interp):
        gateway_block.__init__(self,
            name=name,
            in_sig=in_sig,
            out_sig=out_sig,
            work_type=gr.GR_BLOCK_GW_WORK_INTERP,
            factor=interp,
        )

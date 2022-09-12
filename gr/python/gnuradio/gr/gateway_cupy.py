#
# Copyright 2011-2012, 2018, 2020 Free Software Foundation, Inc.
# Copyright 2021 Josh Morman
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#



import numpy
import cupy
import ctypes

from . import gr_python as gr
from .gr_python import python_block, python_sync_block


########################################################################
# Magic to turn pointers into numpy arrays
# http://docs.scipy.org/doc/numpy/reference/arrays.interface.html
########################################################################

def pointer_to_cupyarray(addr, typestr, shape, nitems):

    if shape[-1] == 1:
        shape = shape[:-1]
    shape = (nitems,) + tuple(shape)
    class array_like(object):
        __cuda_array_interface__ = {
            'data': (int(addr), False),
            'typestr': typestr,
            'descr': None,
            'shape': shape,
            'strides': None,
            'version': 3
        }
    return cupy.asarray(array_like()).view()

########################################################################
# io_signature for Python
########################################################################

class block(python_block):

    def __init__(self, name):
        python_block.__init__(self, self, name)

    def handle_work(self, *args, **kwargs):
        print ('handle_work')
        return self.work(*args, *kwargs)

    def work(self, *args, **kwargs):
        """work to be overloaded in a derived class"""
        raise NotImplementedError("work not implemented")

    def start(self):
        return True

    def stop(self):
        return True

class sync_block(python_sync_block):

    def __init__(self, name):
        python_sync_block.__init__(self, self, name)

    def handle_work(self, *args, **kwargs):
        print ('handle_work')
        return self.work(*args, *kwargs)

    def work(self, *args, **kwargs):
        """general work to be overloaded in a derived class"""
        raise NotImplementedError("general work not implemented")

    def start(self):
        return True

    def stop(self):
        return True

    def get_input_array(self, work_input, index):
        ctypes.pythonapi.PyCapsule_GetPointer.restype = ctypes.c_void_p
        ctypes.pythonapi.PyCapsule_GetPointer.argtypes = [
            ctypes.py_object, ctypes.c_char_p]

        port = self.get_port(index, gr.STREAM, gr.INPUT)

        return pointer_to_cupyarray(
                ctypes.pythonapi.PyCapsule_GetPointer(work_input[index].raw_items(), None),
                port.format_descriptor(),
                port.shape(),
                work_input[index].n_items)

    def get_output_array(self, work_output, index):
        ctypes.pythonapi.PyCapsule_GetPointer.restype = ctypes.c_void_p
        ctypes.pythonapi.PyCapsule_GetPointer.argtypes = [
            ctypes.py_object, ctypes.c_char_p]

        port = self.get_port(index, gr.STREAM, gr.OUTPUT)

        return pointer_to_cupyarray(
                ctypes.pythonapi.PyCapsule_GetPointer(work_output[index].raw_items(), None),
                port.format_descriptor(),
                port.shape(),
                work_output[index].n_items)
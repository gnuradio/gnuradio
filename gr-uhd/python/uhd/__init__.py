#
# Copyright 2010-2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Provides source and sink blocks to interface with the UHD library.
Used to send and receive data between the Ettus Research, LLC product
line.
'''

from __future__ import absolute_import
from __future__ import unicode_literals

import uhd  # TODO: verify uhd python is installed as a dependency for gr-uhd with python

########################################################################
# Prepare uhd swig module to make it more pythonic
########################################################################
def _prepare_uhd_python():
    try:
        from . import uhd_python
    except ImportError:
        import os
        dirname, filename = os.path.split(os.path.abspath(__file__))
        __path__.append(os.path.join(dirname, "bindings"))
        from . import uhd_python

    #some useful typedefs for the user
    setattr(uhd_python, 'freq_range_t', uhd_python.meta_range_t)
    setattr(uhd_python, 'gain_range_t', uhd_python.meta_range_t)

    #Make the python tune request object inherit from float
    #so that it can be passed in GRC as a frequency parameter.
    #The type checking in GRC will accept the tune request.
    #Also use kwargs to construct individual struct elements.
    class tune_request_t(uhd_python.tune_request_t):
        # def __new__(self, *args, **kwargs): return float.__new__(self)
        def __float__(self): return self.target_freq
        def __init__(self, *args, **kwargs):
            super().__init__(*args)
            for key, val in list(kwargs.items()): setattr(self, key, val)
    setattr(uhd_python, 'tune_request_t', tune_request_t)

    #make the streamer args take **kwargs on init
    class stream_args_t(uhd_python.stream_args_t):
        def __init__(self, *args, **kwargs):
            # UHD Python API doesn't have default args for stream_args_t
            # If empty args, then append empty str's
            while len(args) < 2:
                args += ("",)
            super(stream_args_t, self).__init__(*args)
            for key, val in list(kwargs.items()):
                #for some reason, I can't assign a list in the constructor
                #but what I can do is append the elements individually
                if key == 'channels':
                    for v in val: self.channels.append(v)
                elif key == 'args':
                    self.args = uhd_python.device_addr_t(val)
                else: setattr(self, key, val)

    # FIXME: stream_args_t.channels.append does not work due to copy operation of STL vectors
    setattr(uhd_python, 'stream_args_t', stream_args_t)

    #handle general things on all uhd_python attributes
    #Install the __str__ and __repr__ handlers if applicable
    #Create aliases for uhd swig attributes to avoid the "_t"
    for attr in dir(uhd_python):
        myobj = getattr(uhd_python, attr)
        if hasattr(myobj, 'to_string'):    myobj.__repr__     = lambda o: o.to_string().strip()
        if hasattr(myobj, 'to_pp_string'): myobj.__str__      = lambda o: o.to_pp_string().strip()
        if hasattr(myobj, 'to_bool'):      myobj.__nonzero__  = lambda o: o.to_bool()
        if hasattr(myobj, 'to_int'):       myobj.__int__      = lambda o: o.to_int()
        if hasattr(myobj, 'to_real'):      myobj.__float__    = lambda o: o.to_real()
        if attr.endswith('_t'): setattr(uhd_python, attr[:-2], myobj)

    #make a new find devices that casts everything with the pythonized device_addr_t which has __str__
    def find_devices(*args, **kwargs):
        def to_pythonized_dev_addr(dev_addr):
            new_dev_addr = uhd_python.device_addr_t()
            for key in list(dev_addr.keys()): new_dev_addr[key] = dev_addr.get(key)
            return new_dev_addr
        return __builtins__['map'](to_pythonized_dev_addr, uhd_python.find_devices_raw(*args, **kwargs))
    setattr(uhd_python, 'find_devices', find_devices)

########################################################################
# Initialize this module with the contents of uhd pybind
########################################################################
_prepare_uhd_python()
from .uhd_python import *

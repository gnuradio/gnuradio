#
# Copyright 2010-2012 Free Software Foundation, Inc.
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

'''
Provides source and sink blocks to interface with the UHD library.
Used to send and receive data between the Ettus Research, LLC product
line.
'''

########################################################################
# Prepare uhd swig module to make it more pythonic
########################################################################
def _prepare_uhd_swig():
    try:
        import uhd_swig
    except ImportError:
        import os
        dirname, filename = os.path.split(os.path.abspath(__file__))
        __path__.append(os.path.join(dirname, "..", "..", "swig"))
        import uhd_swig

    #some useful typedefs for the user
    setattr(uhd_swig, 'freq_range_t', uhd_swig.meta_range_t)
    setattr(uhd_swig, 'gain_range_t', uhd_swig.meta_range_t)

    #Make the python tune request object inherit from float
    #so that it can be passed in GRC as a frequency parameter.
    #The type checking in GRC will accept the tune request.
    #Also use kwargs to construct individual struct elements.
    class tune_request_t(uhd_swig.tune_request_t, float):
        def __new__(self, *args, **kwargs): return float.__new__(self)
        def __float__(self): return self.target_freq
        def __init__(self, *args, **kwargs):
            super(tune_request_t, self).__init__(*args)
            for key, val in kwargs.iteritems(): setattr(self, key, val)
    setattr(uhd_swig, 'tune_request_t', tune_request_t)

    #Make the python tune request object inherit from string
    #so that it can be passed in GRC as a string parameter.
    #The type checking in GRC will accept the device address.
    #Define the set/get item special methods for dict access.
    class device_addr_t(uhd_swig.device_addr_t, str):
        def __new__(self, *args): return str.__new__(self)
        def __getitem__(self, key): return self.get(key)
        def __setitem__(self, key, val): self.set(key, val)
        def __init__(self, *args, **kwargs):
            super(device_addr_t, self).__init__(*args)
            if args and isinstance(args[0], device_addr_t):
                for key in args[0].keys(): self[key] = args[0][key]
    setattr(uhd_swig, 'device_addr_t', device_addr_t)

    #make the streamer args take **kwargs on init
    class stream_args_t(uhd_swig.stream_args_t):
        def __init__(self, *args, **kwargs):
            super(stream_args_t, self).__init__(*args)
            for key, val in kwargs.iteritems():
                #for some reason, I can't assign a list in the constructor
                #but what I can do is append the elements individually
                if key == 'channels':
                    for v in val: self.channels.append(v)
                elif key == 'args':
                    self.args = device_addr_t(val)
                else: setattr(self, key, val)
    setattr(uhd_swig, 'stream_args_t', stream_args_t)

    #handle general things on all uhd_swig attributes
    #Install the __str__ and __repr__ handlers if applicable
    #Create aliases for uhd swig attributes to avoid the "_t"
    for attr in dir(uhd_swig):
        myobj = getattr(uhd_swig, attr)
        if hasattr(myobj, 'to_string'):    myobj.__repr__     = lambda o: o.to_string().strip()
        if hasattr(myobj, 'to_pp_string'): myobj.__str__      = lambda o: o.to_pp_string().strip()
        if hasattr(myobj, 'to_bool'):      myobj.__nonzero__  = lambda o: o.to_bool()
        if hasattr(myobj, 'to_int'):       myobj.__int__      = lambda o: o.to_int()
        if hasattr(myobj, 'to_real'):      myobj.__float__    = lambda o: o.to_real()
        if attr.endswith('_t'): setattr(uhd_swig, attr[:-2], myobj)

    #make a new find devices that casts everything with the pythonized device_addr_t which has __str__
    def find_devices(*args, **kwargs):
        def to_pythonized_dev_addr(dev_addr):
            new_dev_addr = uhd_swig.device_addr_t()
            for key in dev_addr.keys(): new_dev_addr[key] = dev_addr.get(key)
            return new_dev_addr
        return __builtins__['map'](to_pythonized_dev_addr, uhd_swig.find_devices_raw(*args, **kwargs))
    setattr(uhd_swig, 'find_devices', find_devices)

    #Cast constructor args (FIXME swig handle overloads?)
    for attr in ('usrp_source', 'usrp_sink', 'amsg_source'):
        def constructor_factory(old_constructor):
            def constructor_interceptor(*args, **kwargs):
                args = list(args)
                kwargs = dict(kwargs)
                for index, key, cast in (
                    (0, 'device_addr', device_addr),
                    (1, 'io_type', io_type),
                ):
                    try:
                        if len(args) > index: args[index] = cast(args[index])
                        if kwargs.has_key(key): kwargs[key] = cast(kwargs[key])
                    except: pass
                #don't pass kwargs, it confuses swig, map into args list:
                for key in ('device_addr', 'stream_args', 'io_type', 'num_channels', 'msgq'):
                    if kwargs.has_key(key): args.append(kwargs[key])
                return old_constructor(*args)
            return constructor_interceptor
        setattr(uhd_swig, attr, constructor_factory(getattr(uhd_swig, attr)))

    #Aliases for deprecated constructors
    setattr(uhd_swig, 'single_usrp_source', uhd_swig.usrp_source)
    setattr(uhd_swig, 'single_usrp_sink', uhd_swig.usrp_sink)
    setattr(uhd_swig, 'multi_usrp_source', uhd_swig.usrp_source)
    setattr(uhd_swig, 'multi_usrp_sink', uhd_swig.usrp_sink)

########################################################################
# Initialize this module with the contents of uhd swig
########################################################################
_prepare_uhd_swig()
from uhd_swig import *

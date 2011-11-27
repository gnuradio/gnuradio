#
# Copyright 2011 Free Software Foundation, Inc.
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
GNU Radio package for the Symplex Hardware Driver.
'''

########################################################################
# Prepare shd swig module to make it more pythonic
########################################################################
def _prepare_shd_swig():
    import shd_swig

    #some useful typedefs for the user
    setattr(shd_swig, 'freq_range_t', shd_swig.meta_range_t)
    setattr(shd_swig, 'gain_range_t', shd_swig.meta_range_t)

    #Make the python tune request object inherit from float
    #so that it can be passed in GRC as a frequency parameter.
    #The type checking in GRC will accept the tune request.
    class tune_request_t(shd_swig.tune_request_t, float):
        def __new__(self, *args): return float.__new__(self)
        def __float__(self): return self.target_freq
    setattr(shd_swig, 'tune_request_t', tune_request_t)

    #Make the python tune request object inherit from string
    #so that it can be passed in GRC as a string parameter.
    #The type checking in GRC will accept the device address.
    #Define the set/get item special methods for dict access.
    class device_addr_t(shd_swig.device_addr_t, str):
        def __new__(self, *args): return str.__new__(self)
        def __getitem__(self, key): return self.get(key)
        def __setitem__(self, key, val): self.set(key, val)
    setattr(shd_swig, 'device_addr_t', device_addr_t)

    #handle general things on all shd_swig attributes
    #Install the __str__ and __repr__ handlers if applicable
    #Create aliases for shd swig attributes to avoid the "_t"
    for attr in dir(shd_swig):
        myobj = getattr(shd_swig, attr)
        if hasattr(myobj, 'to_string'):    myobj.__repr__     = lambda o: o.to_string().strip()
        if hasattr(myobj, 'to_pp_string'): myobj.__str__      = lambda o: o.to_pp_string().strip()
        if hasattr(myobj, 'to_bool'):      myobj.__nonzero__  = lambda o: o.to_bool()
        if hasattr(myobj, 'to_int'):       myobj.__int__      = lambda o: o.to_int()
        if hasattr(myobj, 'to_real'):      myobj.__float__    = lambda o: o.to_real()
        if attr.endswith('_t'): setattr(shd_swig, attr[:-2], myobj)

    #Cast constructor args (FIXME swig handle overloads?)
    for attr in ('smini_source', 'smini_sink'):
        def constructor_factory(old_constructor):
            def constructor_interceptor(*args, **kwargs):
                args = list(args)
                kwargs = dict(kwargs)
                for index, key, cast in (
                    (0, 'device_addr', device_addr),
                    (1, 'io_type', io_type),
                ):
                    if len(args) > index: args[index] = cast(args[index])
                    if kwargs.has_key(key): kwargs[key] = cast(kwargs[key])
                return old_constructor(*args, **kwargs)
            return constructor_interceptor
        setattr(shd_swig, attr, constructor_factory(getattr(shd_swig, attr)))

    #Aliases for deprecated constructors
    setattr(shd_swig, 'single_smini_source', shd_swig.smini_source)
    setattr(shd_swig, 'single_smini_sink', shd_swig.smini_sink)
    setattr(shd_swig, 'multi_smini_source', shd_swig.smini_source)
    setattr(shd_swig, 'multi_smini_sink', shd_swig.smini_sink)

########################################################################
# Initialize this module with the contents of shd swig
########################################################################
_prepare_shd_swig()
from shd_swig import *

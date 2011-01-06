#
# Copyright 2010-2011 Free Software Foundation, Inc.
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

# The presence of this file turns this directory into a Python package

########################################################################
# Add SWIG generated code to this namespace
########################################################################
from uhd_swig import *

########################################################################
# Make types with to_string functions printable
########################################################################
range_t.__str__ = lambda s: s.to_pp_string().strip()
meta_range_t.__str__ = lambda s: s.to_pp_string().strip()
device_addr_t.__str__ = lambda s: s.to_pp_string().strip()

########################################################################
# Add other content from pure-Python modules here
########################################################################
class freq_range_t(meta_range_t): pass #a typedef for the user
class gain_range_t(meta_range_t): pass #a typedef for the user

class tune_request_t(tune_request_t, float):
    """
    Make the python tune request object inherit from float
    so that it can be passed in GRC as a frequency parameter.
    The type checking in GRC will accept the tune request.
    """
    def __new__(self, *args): return float.__new__(self)
    def __float__(self): return self.target_freq

class device_addr_t(device_addr_t, str):
    """
    Make the python tune request object inherit from string
    so that it can be passed in GRC as a string parameter.
    The type checking in GRC will accept the device address.
    Define the set/get item special methods for dict access.
    """
    def __new__(self, *args): return str.__new__(self)
    def __getitem__(self, key): return self.get(key)
    def __setitem__(self, key, val): self.set(key, val)

########################################################################
# Create aliases for global attributes to avoid the "_t"
########################################################################
for attr in globals().keys():
    if attr.endswith('_t'): globals()[attr[:-2]] = globals()[attr]

########################################################################
# Cast constructor args (FIXME swig handle overloads?)
########################################################################
for attr in (
    'single_usrp_source', 'single_usrp_sink',
    'multi_usrp_source', 'multi_usrp_sink'
):
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
    import uhd_swig
    globals()[attr] = constructor_factory(getattr(uhd_swig, attr))

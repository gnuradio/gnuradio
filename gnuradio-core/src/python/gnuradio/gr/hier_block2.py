#
# Copyright 2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from gnuradio_swig_python import hier_block2_swig, gr_make_runtime, \
    runtime_run_unlocked, runtime_start_unlocked, runtime_stop_unlocked, \
    runtime_wait_unlocked 

#
# This hack forces a 'has-a' relationship to look like an 'is-a' one.
#
# It allows Python classes to subclass this one, while passing through
# method calls to the C++ class shared pointer from SWIG.
#
# It also allows us to intercept method calls if needed
#
class hier_block2(object):
    def __init__(self, name, input_signature, output_signature):
	self._hb = hier_block2_swig(name, input_signature, output_signature)

    def __getattr__(self, name):
	return getattr(self._hb, name)

    def define_component(self, name, comp):
	return self._hb.define_component(name, comp.basic_block())

class runtime(object):
    def __init__(self, top_block):
        if (isinstance(top_block, hier_block2)):
            self._r = gr_make_runtime(top_block._hb)            
        else:
            self._r = gr_make_runtime(top_block)

    def run(self):
        runtime_run_unlocked(self._r)

    def start(self):
        runtime_start_unlocked(self._r)

    def stop(self):
        runtime_stop_unlocked(self._r)

    def wait(self):
        runtime_wait_unlocked(self._r)

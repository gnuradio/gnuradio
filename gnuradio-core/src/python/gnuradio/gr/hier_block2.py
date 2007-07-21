#
# Copyright 2006 Free Software Foundation, Inc.
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

from gnuradio_swig_python import hier_block2_swig, gr_make_runtime, \
    runtime_run_unlocked, runtime_start_unlocked, runtime_stop_unlocked, \
    runtime_wait_unlocked, runtime_restart_unlocked, io_signature

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

    def connect(self, *points):
        '''connect requires two or more arguments that can be coerced to endpoints.
        If more than two arguments are provided, they are connected together successively.
        '''
        if len (points) < 2:
            raise ValueError, ("connect requires at least two endpoints; %d provided." % (len (points),))
        for i in range (1, len (points)):
            self._connect(points[i-1], points[i])

    def _connect(self, src, dst):
        (src_block, src_port) = self._coerce_endpoint(src)
        (dst_block, dst_port) = self._coerce_endpoint(dst)
        self._hb.connect(src_block.basic_block(), src_port,
                         dst_block.basic_block(), dst_port)

    def _coerce_endpoint(self, endp):
        if hasattr(endp, 'basic_block'):
            return (endp, 0)
        else:
            if hasattr(endp, "__getitem__") and len(endp) == 2:
                return endp # Assume user put (block, port)
            else:
                raise ValueError("unable to coerce endpoint")

    def disconnect(self, *points):
        '''connect requires two or more arguments that can be coerced to endpoints.
        If more than two arguments are provided, they are disconnected successively.
        '''
        if len (points) < 2:
            raise ValueError, ("disconnect requires at least two endpoints; %d provided." % (len (points),))
        for i in range (1, len (points)):
            self._disconnect(points[i-1], points[i])

    def _disconnect(self, src, dst):
        (src_block, src_port) = self._coerce_endpoint(src)
        (dst_block, dst_port) = self._coerce_endpoint(dst)
        self._hb.disconnect(src_block.basic_block(), src_port,
                            dst_block.basic_block(), dst_port)

# Convenience class to create a no input, no output block for runtime top block
class top_block(hier_block2):
    def __init__(self, name):
        hier_block2.__init__(self, name, io_signature(0,0,0), io_signature(0,0,0))

# This allows the 'run_locked' methods, which are defined in gr_runtime.i,
# to release the Python global interpreter lock before calling the actual
# method in gr.runtime
#
# This probably should be elsewhere but it works here
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

    def restart(self):
        runtime_restart_unlocked(self._r)

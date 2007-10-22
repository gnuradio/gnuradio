#
# Copyright 2007 Free Software Foundation, Inc.
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

from gnuradio_swig_python import top_block_swig, \
    top_block_wait_unlocked, top_block_run_unlocked

#
# This hack forces a 'has-a' relationship to look like an 'is-a' one.
#
# It allows Python classes to subclass this one, while passing through
# method calls to the C++ class shared pointer from SWIG.
#
# It also allows us to intercept method calls if needed.
#
# This allows the 'run_locked' methods, which are defined in gr_top_block.i,
# to release the Python global interpreter lock before calling the actual
# method in gr_top_block
#
class top_block(object):
    def __init__(self, name="top_block"):
	self._tb = top_block_swig(name)

    def __getattr__(self, name):
	return getattr(self._tb, name)

    def start(self):
    	self._tb.start()
	
    def stop(self):
    	self._tb.stop()

    def run(self):
        top_block_run_unlocked(self._tb)

    def wait(self):
        top_block_wait_unlocked(self._tb)

    # FIXME: these are duplicated from hier_block2.py; they should really be implemented
    # in the original C++ class (gr_hier_block2), then they would all be inherited here

    def connect(self, *points):
        '''connect requires one or more arguments that can be coerced to endpoints.
        If more than two arguments are provided, they are connected together successively.
        '''
        if len (points) < 1:
            raise ValueError, ("connect requires at least one endpoint; %d provided." % (len (points),))
	else:
	    if len(points) == 1:
		self._tb.connect(points[0].basic_block())
	    else:
		for i in range (1, len (points)):
        	    self._connect(points[i-1], points[i])

    def _connect(self, src, dst):
        (src_block, src_port) = self._coerce_endpoint(src)
        (dst_block, dst_port) = self._coerce_endpoint(dst)
        self._tb.connect(src_block.basic_block(), src_port,
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
        '''connect requires one or more arguments that can be coerced to endpoints.
        If more than two arguments are provided, they are disconnected successively.
        '''
        if len (points) < 1:
            raise ValueError, ("disconnect requires at least two endpoints; %d provided." % (len (points),))
        else:
            if len(points) == 1:
                self._tb.disconnect(points[0].basic_block())
            else:
                for i in range (1, len (points)):
                    self._disconnect(points[i-1], points[i])

    def _disconnect(self, src, dst):
        (src_block, src_port) = self._coerce_endpoint(src)
        (dst_block, dst_port) = self._coerce_endpoint(dst)
        self._tb.disconnect(src_block.basic_block(), src_port,
                            dst_block.basic_block(), dst_port)


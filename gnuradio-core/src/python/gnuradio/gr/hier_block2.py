#
# Copyright 2006,2007 Free Software Foundation, Inc.
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

from gnuradio_core import hier_block2_swig

#
# This hack forces a 'has-a' relationship to look like an 'is-a' one.
#
# It allows Python classes to subclass this one, while passing through
# method calls to the C++ class shared pointer from SWIG.
#
# It also allows us to intercept method calls if needed
#
class hier_block2(object):
    """
    Python wrapper around the C++ hierarchical block implementation.
    Provides convenience functions and allows proper Python subclassing.
    """

    def __init__(self, name, input_signature, output_signature):
        """
        Create a hierarchical block with a given name and I/O signatures.
        """
	self._hb = hier_block2_swig(name, input_signature, output_signature)

    def __getattr__(self, name):
        """
        Pass-through member requests to the C++ object.
        """
        if not hasattr(self, "_hb"):
            raise RuntimeError("hier_block2: invalid state--did you forget to call gr.hier_block2.__init__ in a derived class?")
	return getattr(self._hb, name)

    def connect(self, *points):
        """
        Connect two or more block endpoints.  An endpoint is either a (block, port)
        tuple or a block instance.  In the latter case, the port number is assumed
        to be zero.

        To connect the hierarchical block external inputs or outputs to internal block
        inputs or outputs, use 'self' in the connect call.

        If multiple arguments are provided, connect will attempt to wire them in series,
        interpreting the endpoints as inputs or outputs as appropriate.
        """

        if len (points) < 1:
            raise ValueError, ("connect requires at least one endpoint; %d provided." % (len (points),))
	else:
	    if len(points) == 1:
		self._hb.primitive_connect(points[0].to_basic_block())
	    else:
		for i in range (1, len (points)):
        	    self._connect(points[i-1], points[i])

    def _connect(self, src, dst):
        (src_block, src_port) = self._coerce_endpoint(src)
        (dst_block, dst_port) = self._coerce_endpoint(dst)
        self._hb.primitive_connect(src_block.to_basic_block(), src_port,
                                   dst_block.to_basic_block(), dst_port)

    def _coerce_endpoint(self, endp):
        if hasattr(endp, 'to_basic_block'):
            return (endp, 0)
        else:
            if hasattr(endp, "__getitem__") and len(endp) == 2:
                return endp # Assume user put (block, port)
            else:
                raise ValueError("unable to coerce endpoint")

    def disconnect(self, *points):
        """
        Disconnect two endpoints in the flowgraph.

        To disconnect the hierarchical block external inputs or outputs to internal block
        inputs or outputs, use 'self' in the connect call.

        If more than two arguments are provided, they are disconnected successively.
        """
        
        if len (points) < 1:
            raise ValueError, ("disconnect requires at least one endpoint; %d provided." % (len (points),))
        else:
            if len (points) == 1:
                self._hb.primitive_disconnect(points[0].to_basic_block())
            else:
                for i in range (1, len (points)):
                    self._disconnect(points[i-1], points[i])

    def _disconnect(self, src, dst):
        (src_block, src_port) = self._coerce_endpoint(src)
        (dst_block, dst_port) = self._coerce_endpoint(dst)
        self._hb.primitive_disconnect(src_block.to_basic_block(), src_port,
                                      dst_block.to_basic_block(), dst_port)


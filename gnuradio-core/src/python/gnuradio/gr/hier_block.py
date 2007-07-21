#
# Copyright 2005 Free Software Foundation, Inc.
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

from gnuradio_swig_python import io_signature
import weakref

class hier_block_base(object):
    """
    Abstract base class for hierarchical signal processing blocks.
    """
    def __init__(self, fg):
        """
        @param fg: The flow graph that contains this hierarchical block.
        @type fg: gr.flow_graph
        """
        self.fg = weakref.proxy(fg)

    def input_signature(self):
        """
        @return input signature of hierarchical block.
        @rtype gr.io_signature
        """
        raise NotImplemented

    def output_signature(self):
        """
        @return output signature of hierarchical block.
        @rtype gr.io_signature
        """
        raise NotImplemented

    def resolve_input_port(self, port_number):
        """
        @param port_number: which input port number to resolve to an endpoint.
        @type port_number: int
        @return: sequence of endpoints
        @rtype: sequence of endpoint

        Note that an input port can resolve to more than one endpoint.
        """
        raise NotImplemented
    
    def resolve_output_port(self, port_number):
        """
        @param port_number: which output port number to resolve to an endpoint.
        @type port_number: int
        @return: endpoint
        @rtype: endpoint

        Output ports resolve to a single endpoint.
        """
        raise NotImplemented
    

class hier_block(hier_block_base):
    """
    Simple concrete class for building hierarchical blocks.

    This class assumes that there is at most a single block at the
    head of the chain and a single block at the end of the chain.
    Either head or tail may be None indicating a sink or source respectively.

    If you needs something more elaborate than this, derive a new class from
    hier_block_base.
    """
    def __init__(self, fg, head_block, tail_block):
        """
        @param fg: The flow graph that contains this hierarchical block.
        @type fg: flow_graph
        @param head_block: the first block in the signal processing chain.
        @type head_block: None or subclass of gr.block or gr.hier_block_base
        @param tail_block: the last block in the signal processing chain.
        @type tail_block: None or subclass of gr.block or gr.hier_block_base
        """
        hier_block_base.__init__(self, fg)
        # FIXME add type checks here for easier debugging of misuse
        self.head = head_block
        self.tail = tail_block

    def input_signature(self):
        if self.head:
            return self.head.input_signature()
        else:
            return io_signature(0,0,0)
    
    def output_signature(self):
        if self.tail:
            return self.tail.output_signature()
        else:
            return io_signature(0,0,0)
    
    def resolve_input_port(self, port_number):
        return ((self.head, port_number),)

    def resolve_output_port(self, port_number):
        return (self.tail, port_number)


class compose(hier_block):
    """
    Compose one or more blocks (primitive or hierarchical) into a new hierarchical block.
    """
    def __init__(self, fg, *blocks):
        """
        @param fg: flow graph
        @type fg: gr.flow_graph
        @param *blocks: list of blocks
        @type *blocks: list of blocks
        """
        if len(blocks) < 1:
            raise ValueError, ("compose requires at least one block; none provided.")
        if len(blocks) > 1:
            fg.connect(*blocks)
        hier_block.__init__(self, fg, blocks[0], blocks[-1])

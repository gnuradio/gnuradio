#
# Copyright 2004 Free Software Foundation, Inc.
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

from gnuradio_swig_python import gr_block_sptr
import types
import hier_block

def remove_duplicates (seq):
    new = []
    for x in seq:
        if not x in new:
            new.append (x)
    return new


class endpoint (object):
    __slots__ = ['block', 'port']
    def __init__ (self, block, port):
        self.block = block
        self.port = port

    def __cmp__ (self, other):
        if self.block == other.block and self.port == other.port:
            return 0
        return 1

    def __str__ (self):
        return '<endpoint (%s, %s)>' % (self.block, self.port)
        
def expand_src_endpoint (src_endpoint):
    # A src_endpoint is an output of a block
    src_endpoint = coerce_endpoint (src_endpoint)
    if isinstance (src_endpoint.block, hier_block.hier_block_base):
        return expand_src_endpoint (
            coerce_endpoint (src_endpoint.block.resolve_output_port(src_endpoint.port)))
    else:
        return src_endpoint

def expand_dst_endpoint (dst_endpoint):
    # a dst_endpoint is the input to a block
    dst_endpoint = coerce_endpoint (dst_endpoint)
    if isinstance (dst_endpoint.block, hier_block.hier_block_base):
        exp = [coerce_endpoint(x) for x in
               dst_endpoint.block.resolve_input_port(dst_endpoint.port)]
        return expand_dst_endpoints (exp)
    else:
        return [dst_endpoint]

def expand_dst_endpoints (endpoint_list):
    r = []
    for e in endpoint_list:
        r.extend (expand_dst_endpoint (e))
    return r
           

def coerce_endpoint (x):
    if isinstance (x, endpoint):
        return x
    elif isinstance (x, types.TupleType) and len (x) == 2:
        return endpoint (x[0], x[1])
    elif hasattr (x, 'block'):          # assume it's a block
        return endpoint (x, 0)
    elif isinstance(x, hier_block.hier_block_base):
        return endpoint (x, 0)
    else:
        raise ValueError, "Not coercible to endpoint: %s" % (x,)
    

class edge (object):
    __slots__ = ['src', 'dst']
    def __init__ (self, src_endpoint, dst_endpoint):
        self.src = src_endpoint
        self.dst = dst_endpoint

    def __cmp__ (self, other):
        if self.src == other.src and self.dst == other.dst:
            return 0
        return 1

    def __repr__ (self):
        return '<edge (%s, %s)>' % (self.src, self.dst)

class basic_flow_graph (object):
    '''basic_flow_graph -- describe connections between blocks'''
    # __slots__ is incompatible with weakrefs (for some reason!)
    # __slots__ = ['edge_list']
    def __init__ (self):
        self.edge_list = []

    def connect (self, *points):
        '''connect requires two or more arguments that can be coerced to endpoints.
        If more than two arguments are provided, they are connected together successively.
        '''
        if len (points) < 2:
            raise ValueError, ("connect requires at least two endpoints; %d provided." % (len (points),))
        for i in range (1, len (points)):
            self._connect (points[i-1], points[i])

    def _connect (self, src_endpoint, dst_endpoint):
        s = expand_src_endpoint (src_endpoint)
        for d in expand_dst_endpoint (dst_endpoint):
            self._connect_prim (s, d)

    def _connect_prim (self, src_endpoint, dst_endpoint):
        src_endpoint = coerce_endpoint (src_endpoint)
        dst_endpoint = coerce_endpoint (dst_endpoint)
        self._check_valid_src_port (src_endpoint)
        self._check_valid_dst_port (dst_endpoint)
        self._check_dst_in_use (dst_endpoint)
        self._check_type_match (src_endpoint, dst_endpoint)
        self.edge_list.append (edge (src_endpoint, dst_endpoint))

    def disconnect (self, src_endpoint, dst_endpoint):
        s = expand_src_endpoint (src_endpoint)
        for d in expand_dst_endpoint (dst_endpoint):
            self._disconnect_prim (s, d)

    def _disconnect_prim (self, src_endpoint, dst_endpoint):
        src_endpoint = coerce_endpoint (src_endpoint)
        dst_endpoint = coerce_endpoint (dst_endpoint)
        e = edge (src_endpoint, dst_endpoint)
        self.edge_list.remove (e)

    def disconnect_all (self):
        self.edge_list = []
        
    def validate (self):
        # check all blocks to ensure:
        #  (1a) their input ports are contiguously assigned
        #  (1b) the number of input ports is between min and max
        #  (2a) their output ports are contiguously assigned
        #  (2b) the number of output ports is between min and max
        #  (3)  check_topology returns true

        for m in self.all_blocks ():
            # print m

            edges = self.in_edges (m)
            used_ports = [e.dst.port for e in edges]
            ninputs = self._check_contiguity (m, m.input_signature (), used_ports, "input")

            edges = self.out_edges (m)
            used_ports = [e.src.port for e in edges]
            noutputs = self._check_contiguity (m, m.output_signature (), used_ports, "output")

            if not m.check_topology (ninputs, noutputs):
                raise ValueError, ("%s::check_topology (%d, %d) failed" % (m, ninputs, noutputs))
            
        
    # --- public utilities ---
                
    def all_blocks (self):
        '''return list of all blocks in the graph'''
        all_blocks = []
        for edge in self.edge_list:
            m = edge.src.block
            if not m in all_blocks:
                all_blocks.append (m)
            m = edge.dst.block
            if not m in all_blocks:
                all_blocks.append (m)
        return all_blocks
        
    def in_edges (self, m):
        '''return list of all edges that have M as a destination'''
        return [e for e in self.edge_list if e.dst.block == m]
    
    def out_edges (self, m):
        '''return list of all edges that have M as a source'''
        return [e for e in self.edge_list if e.src.block == m]
    
    def downstream_verticies (self, m):
        return [e.dst.block for e in self.out_edges (m)]

    def downstream_verticies_port (self, m, port):
        return [e.dst.block for e in self.out_edges(m) if e.src.port == port]

    def upstream_verticies (self, m):
        return [e.src.block for e in self.in_edges (m)]

    def adjacent_verticies (self, m):
        '''return list of all verticies adjacent to M'''
        return self.downstream_verticies (m) + self.upstream_verticies (m)

    def sink_p (self, m):
        '''return True iff this block is a sink'''
        e = self.out_edges (m)
        return len (e) == 0

    def source_p (self, m):
        '''return True iff this block is a source'''
        e = self.in_edges (m)
        return len (e) == 0
        
    # --- internal methods ---
    
    def _check_dst_in_use (self, dst_endpoint):
        '''Ensure that there is not already an endpoint that terminates at dst_endpoint.'''
        x = [ep for ep in self.edge_list if ep.dst == dst_endpoint]
        if x:    # already in use
            raise ValueError, ("destination endpoint already in use: %s" % (dst_endpoint))

    def _check_valid_src_port (self, src_endpoint):
        self._check_port (src_endpoint.block.output_signature(), src_endpoint.port)
        
    def _check_valid_dst_port (self, dst_endpoint):
        self._check_port (dst_endpoint.block.input_signature(), dst_endpoint.port)
        
    def _check_port (self, signature, port):
        if port < 0:
            raise ValueError, 'port number out of range.'
        if signature.max_streams () == -1: # infinite
            return                         # OK
        if port >= signature.max_streams ():
            raise ValueError, 'port number out of range.'

    def _check_type_match (self, src_endpoint, dst_endpoint):
        # for now, we just ensure that the stream item sizes match
        src_sig = src_endpoint.block.output_signature ()
        dst_sig = dst_endpoint.block.input_signature ()
        src_size = src_sig.sizeof_stream_item (src_endpoint.port)
        dst_size = dst_sig.sizeof_stream_item (dst_endpoint.port)
        if src_size != dst_size:
            raise ValueError, (
' '.join(('source and destination data sizes are different:',
src_endpoint.block.name(),
dst_endpoint.block.name())))

    def _check_contiguity (self, m, sig, used_ports, dir):
        used_ports.sort ()
        used_ports = remove_duplicates (used_ports)
        min_s = sig.min_streams ()

        l = len (used_ports)
        if l == 0:
            if min_s == 0:
                return l
            raise ValueError, ("%s requires %d %s connections.  It has none." %
                               (m, min_s, dir))
        
        if used_ports[-1] + 1 < min_s:
            raise ValueError, ("%s requires %d %s connections.  It has %d." %
                               (m, min_s, dir, used_ports[-1] + 1))
            
        if used_ports[-1] + 1 != l:
            for i in range (l):
                if used_ports[i] != i:
                    raise ValueError, ("%s %s port %d is not connected" %
                                       (m, dir, i))
        
        # print "%s ports: %s" % (dir, used_ports)
        return l

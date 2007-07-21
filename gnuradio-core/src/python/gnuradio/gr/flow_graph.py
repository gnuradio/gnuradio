#
# Copyright 2004,2006 Free Software Foundation, Inc.
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

from gnuradio.gr.basic_flow_graph import remove_duplicates, endpoint, edge, \
     basic_flow_graph

from gnuradio.gr.exceptions import *
from gnuradio_swig_python import buffer, buffer_add_reader, block_detail, \
     single_threaded_scheduler

from gnuradio.gr.scheduler import scheduler

_WHITE = 0                          # graph coloring tags
_GRAY  = 1
_BLACK = 2

_flow_graph_debug = False

def set_flow_graph_debug(on):
    global _flow_graph_debug
    _flow_graph_debug = on
    

class buffer_sizes (object):
    """compute buffer sizes to use"""
    def __init__ (self, flow_graph):
        # We could scan the graph here and determine individual sizes
        # based on relative_rate, number of readers, etc.

        # The simplest thing that could possibly work: all buffers same size
        self.flow_graph = flow_graph
        self.fixed_buffer_size = 32*1024

    def allocate (self, m, index):
        """allocate buffer for output index of block m"""
        item_size = m.output_signature().sizeof_stream_item (index)
        nitems = self.fixed_buffer_size / item_size
        if nitems < 2 * m.output_multiple ():
            nitems = 2 * m.output_multiple ()
            
        # if any downstream blocks is a decimator and/or has a large output_multiple, 
        # ensure that we have a buffer at least 2 * their decimation_factor*output_multiple
        for mdown in self.flow_graph.downstream_verticies_port(m, index):
	    decimation = int(1.0 / mdown.relative_rate())
	    nitems = max(nitems, 2 * (decimation * mdown.output_multiple() + mdown.history()))

        return buffer (nitems, item_size)
    

class flow_graph (basic_flow_graph):
    """add physical connection info to simple_flow_graph
    """
    # __slots__ is incompatible with weakrefs (for some reason!)
    # __slots__ = ['blocks', 'scheduler']

    def __init__ (self):
        basic_flow_graph.__init__ (self);
        self.blocks = None
        self.scheduler = None

    def __del__(self):
        # print "\nflow_graph.__del__"
        # this ensures that i/o devices such as the USRP get shutdown gracefully
        self.stop()
        
    def start (self):
        '''start graph, forking thread(s), return immediately'''
        if self.scheduler:
            raise RuntimeError, "Scheduler already running"
        self._setup_connections ()

        # cast down to gr_module_sptr
        # t = [x.block () for x in self.topological_sort (self.blocks)]
        self.scheduler = scheduler (self)
        self.scheduler.start ()

    def stop (self):
        '''tells scheduler to stop and waits for it to happen'''
        if self.scheduler:
            self.scheduler.stop ()
            self.scheduler = None
        
    def wait (self):
        '''waits for scheduler to stop'''
        if self.scheduler:
            self.scheduler.wait ()
            self.scheduler = None
        
    def is_running (self):
        return not not self.scheduler
    
    def run (self):
        '''start graph, wait for completion'''
        self.start ()
        self.wait ()
        
    def _setup_connections (self):
        """given the basic flow graph, setup all the physical connections"""
        self.validate ()
        self.blocks = self.all_blocks ()
        self._assign_details ()
        self._assign_buffers ()
        self._connect_inputs ()

    def _assign_details (self):
        for m in self.blocks:
            edges = self.in_edges (m)
            used_ports = remove_duplicates ([e.dst.port for e in edges])
            ninputs = len (used_ports)

            edges = self.out_edges (m)
            used_ports = remove_duplicates ([e.src.port for e in edges])
            noutputs = len (used_ports)

            m.set_detail (block_detail (ninputs, noutputs))

    def _assign_buffers (self):
        """determine the buffer sizes to use, allocate them and attach to detail"""
        sizes = buffer_sizes (self)
        for m in self.blocks:
            d = m.detail ()
            for index in range (d.noutputs ()):
                d.set_output (index, sizes.allocate (m, index))

    def _connect_inputs (self):
        """connect all block inputs to appropriate upstream buffers"""
        for m in self.blocks:
            d = m.detail ()
            # print "%r history = %d" % (m, m.history())
            for e in self.in_edges(m):
                # FYI, sources don't have any in_edges
                our_port = e.dst.port
                upstream_block = e.src.block
                upstream_port   = e.src.port
                upstream_buffer = upstream_block.detail().output(upstream_port)
                d.set_input(our_port, buffer_add_reader(upstream_buffer, m.history()-1))
    

    def topological_sort (self, all_v):
        '''
        Return a topologically sorted list of vertices.
        This is basically a depth-first search with checks
        for back edges (the non-DAG condition)

        '''

        # it's correct without this sort, but this
        # should give better ordering for cache utilization
        all_v = self._sort_sources_first (all_v)

        output = []
        for v in all_v:
            v.ts_color = _WHITE
        for v in all_v:
            if v.ts_color == _WHITE:
                self._dfs_visit (v, output)
        output.reverse ()
        return output

    def _dfs_visit (self, u, output):
        # print "dfs_visit (enter): ", u
        u.ts_color = _GRAY
        for v in self.downstream_verticies (u):
            if v.ts_color == _WHITE:    # (u, v) is a tree edge
                self._dfs_visit (v, output)
            elif v.ts_color == _GRAY:   # (u, v) is a back edge
                raise NotDAG, "The graph is not an acyclic graph (It's got a loop)"
            elif v.ts_color == _BLACK:  # (u, v) is a cross or forward edge
                pass
            else:
                raise CantHappen, "Invalid color on vertex"
        u.ts_color = _BLACK
        output.append (u)
        # print "dfs_visit (exit):  ", u, output

    def _sort_sources_first (self, all_v):
        # the sort function is not guaranteed to be stable.
        # We add the unique_id in to the key so we're guaranteed
        # of reproducible results.  This is important for the test
        # code.  There is often more than one valid topological sort.
        # We want to force a reproducible choice.
        x = [(not self.source_p(v), v.unique_id(), v) for v in all_v]
        x.sort ()
        x = [v[2] for v in x]
        # print "sorted: ", x
        return x
        
    def partition_graph (self, all_v):
        '''Return a list of lists of nodes that are connected.
        The result is a list of disjoint graphs.
        The sublists are topologically sorted.
        '''
        result = []
        working_v = all_v[:]    # make copy
        while working_v:
            rv = self._reachable_verticies (working_v[0], working_v)
            result.append (self.topological_sort (rv))
            for v in rv:
                working_v.remove (v)
        if _flow_graph_debug:
            print "partition_graph:", result
        return result
        
    def _reachable_verticies (self, start, all_v):
        for v in all_v:
            v.ts_color = _WHITE

        self._reachable_dfs_visit (start)
        return [v for v in all_v if v.ts_color == _BLACK]

    def _reachable_dfs_visit (self, u):
        u.ts_color = _BLACK
        for v in self.adjacent_verticies (u):
            if v.ts_color == _WHITE:
                self._reachable_dfs_visit (v)
        return None

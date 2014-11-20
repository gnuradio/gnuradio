"""
Copyright 2008-2011 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

import re

import expr_utils
from . base.FlowGraph import FlowGraph as _FlowGraph


_variable_matcher = re.compile('^(variable\w*)$')
_parameter_matcher = re.compile('^(parameter)$')
_monitors_searcher = re.compile('(ctrlport_monitor)')
_bussink_searcher = re.compile('^(bus_sink)$')
_bussrc_searcher = re.compile('^(bus_source)$')
_bus_struct_sink_searcher = re.compile('^(bus_structure_sink)$')
_bus_struct_src_searcher = re.compile('^(bus_structure_source)$')

class FlowGraph(_FlowGraph):

    def __init__(self, **kwargs):
        _FlowGraph.__init__(self, **kwargs)
        self._eval_cache = dict()

    def _eval(self, code, namespace, namespace_hash):
        """
        Evaluate the code with the given namespace.

        Args:
            code: a string with python code
            namespace: a dict representing the namespace
            namespace_hash: a unique hash for the namespace

        Returns:
            the resultant object
        """
        if not code: raise Exception, 'Cannot evaluate empty statement.'
        my_hash = hash(code) ^ namespace_hash
        #cache if does not exist
        if not self._eval_cache.has_key(my_hash):
            self._eval_cache[my_hash] = eval(code, namespace, namespace)
        #return from cache
        return self._eval_cache[my_hash]

    def get_hier_block_stream_io(self, direction):
        """
        Get a list of stream io signatures for this flow graph.

        Args:
            direction: a string of 'in' or 'out'

        Returns:
            a list of dicts with: type, label, vlen, size, optional
        """
        return filter(lambda p: p['type'] != "message",
                      self.get_hier_block_io(direction))

    def get_hier_block_message_io(self, direction):
        """
        Get a list of message io signatures for this flow graph.

        Args:
            direction: a string of 'in' or 'out'

        Returns:
            a list of dicts with: type, label, vlen, size, optional
        """
        return filter(lambda p: p['type'] == "message",
                      self.get_hier_block_io(direction))

    def get_hier_block_io(self, direction):
        """
        Get a list of io ports for this flow graph.

        Args:
            direction: a string of 'in' or 'out'

        Returns:
            a list of dicts with: type, label, vlen, size, optional
        """
        pads = self.get_pad_sources() if direction in ('sink', 'in') else \
            self.get_pad_sinks() if direction in ('source', 'out') else []
        ports = []
        for pad in pads:
            master = {
                'label': str(pad.get_param('label').get_evaluated()),
                'type': str(pad.get_param('type').get_evaluated()),
                'vlen': str(pad.get_param('vlen').get_value()),
                'size': pad.get_param('type').get_opt('size'),
                'optional': bool(pad.get_param('optional').get_evaluated()),
            }
            num_ports = pad.get_param('num_streams').get_evaluated()
            if num_ports > 1:
                for i in xrange(num_ports):
                    clone = master.copy()
                    clone['label'] += str(i)
                    ports.append(clone)
            else:
                ports.append(master)
        return ports

    def get_pad_sources(self):
        """
        Get a list of pad source blocks sorted by id order.

        Returns:
            a list of pad source blocks in this flow graph
        """
        pads = filter(lambda b: b.get_key() == 'pad_source', self.get_enabled_blocks())
        return sorted(pads, lambda x, y: cmp(x.get_id(), y.get_id()))

    def get_pad_sinks(self):
        """
        Get a list of pad sink blocks sorted by id order.

        Returns:
            a list of pad sink blocks in this flow graph
        """
        pads = filter(lambda b: b.get_key() == 'pad_sink', self.get_enabled_blocks())
        return sorted(pads, lambda x, y: cmp(x.get_id(), y.get_id()))

    def get_pad_port_global_key(self, port):
        """
        Get the key for a port of a pad source/sink to use in connect()
        This takes into account that pad blocks may have multiple ports

        Returns:
            the key (str)
        """
        key_offset = 0
        pads = self.get_pad_sources() if port.is_source() else self.get_pad_sinks()
        for pad in pads:
            if pad.get_param('type').get_evaluated() == "message":
                continue
            if port.get_parent() == pad:
                return str(key_offset + int(port.get_key()))
            # assuming we have either only sources or sinks
            key_offset += len(pad.get_ports())
        return -1

    def get_imports(self):
        """
        Get a set of all import statments in this flow graph namespace.

        Returns:
            a set of import statements
        """
        imports = sum([block.get_imports() for block in self.get_enabled_blocks()], [])
        imports = sorted(set(imports))
        return imports

    def get_variables(self):
        """
        Get a list of all variables in this flow graph namespace.
        Exclude paramterized variables.

        Returns:
            a sorted list of variable blocks in order of dependency (indep -> dep)
        """
        variables = filter(lambda b: _variable_matcher.match(b.get_key()), self.get_enabled_blocks())
        return expr_utils.sort_objects(variables, lambda v: v.get_id(), lambda v: v.get_var_make())

    def get_parameters(self):
        """
        Get a list of all paramterized variables in this flow graph namespace.

        Returns:
            a list of paramterized variables
        """
        parameters = filter(lambda b: _parameter_matcher.match(b.get_key()), self.get_enabled_blocks())
        return parameters

    def get_monitors(self):
        """
        Get a list of all ControlPort monitors
        """
        monitors = filter(lambda b: _monitors_searcher.search(b.get_key()), self.get_enabled_blocks())
        return monitors


    def get_bussink(self):
        bussink = filter(lambda b: _bussink_searcher.search(b.get_key()), self.get_enabled_blocks())

        for i in bussink:
            for j in i.get_params():
                if j.get_name() == 'On/Off' and j.get_value() == 'on':
                    return True;

        return False



    def get_bussrc(self):
        bussrc = filter(lambda b: _bussrc_searcher.search(b.get_key()), self.get_enabled_blocks())

        for i in bussrc:
            for j in i.get_params():
                if j.get_name() == 'On/Off' and j.get_value() == 'on':
                    return True;

        return False

    def get_bus_structure_sink(self):
        bussink = filter(lambda b: _bus_struct_sink_searcher.search(b.get_key()), self.get_enabled_blocks())

        return bussink

    def get_bus_structure_src(self):
        bussrc = filter(lambda b: _bus_struct_src_searcher.search(b.get_key()), self.get_enabled_blocks())

        return bussrc


    def rewrite(self):
        """
        Flag the namespace to be renewed.
        """
        def reconnect_bus_blocks():
            for block in self.get_blocks():

                if 'bus' in map(lambda a: a.get_type(), block.get_sources_gui()):


                    for i in range(len(block.get_sources_gui())):
                        if len(block.get_sources_gui()[i].get_connections()) > 0:
                            source = block.get_sources_gui()[i]
                            sink = []

                            for j in range(len(source.get_connections())):
                                sink.append(source.get_connections()[j].get_sink());


                            for elt in source.get_connections():
                                self.remove_element(elt);
                            for j in sink:
                                self.connect(source, j);
        self._renew_eval_ns = True
        _FlowGraph.rewrite(self);
        reconnect_bus_blocks();

    def evaluate(self, expr):
        """
        Evaluate the expression.

        Args:
            expr: the string expression
        @throw Exception bad expression

        Returns:
            the evaluated data
        """
        if self._renew_eval_ns:
            self._renew_eval_ns = False
            #reload namespace
            n = dict()
            #load imports
            for imp in self.get_imports():
                try: exec imp in n
                except: pass
            #load parameters
            np = dict()
            for parameter in self.get_parameters():
                try:
                    e = eval(parameter.get_param('value').to_code(), n, n)
                    np[parameter.get_id()] = e
                except: pass
            n.update(np) #merge param namespace
            #load variables
            for variable in self.get_variables():
                try:
                    e = eval(variable.get_var_value(), n, n)
                    n[variable.get_id()] = e
                except: pass
            #make namespace public
            self.n = n
            self.n_hash = hash(str(n))
        #evaluate
        e = self._eval(expr, self.n, self.n_hash)
        return e

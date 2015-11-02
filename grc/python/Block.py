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

import itertools
import collections

from .. base.Constants import BLOCK_FLAG_NEED_QT_GUI, BLOCK_FLAG_NEED_WX_GUI
from .. base.Block import Block as _Block
from .. gui.Block import Block as _GUIBlock

from . FlowGraph import _variable_matcher
import extract_docs


class Block(_Block, _GUIBlock):

    def __init__(self, flow_graph, n):
        """
        Make a new block from nested data.

        Args:
            flow: graph the parent element
            n: the nested odict

        Returns:
            block a new block
        """
        #grab the data
        self._doc = n.find('doc') or ''
        self._imports = map(lambda i: i.strip(), n.findall('import'))
        self._make = n.find('make')
        self._var_make = n.find('var_make')
        self._checks = n.findall('check')
        self._callbacks = n.findall('callback')
        self._bus_structure_source = n.find('bus_structure_source') or ''
        self._bus_structure_sink = n.find('bus_structure_sink') or ''
        self.port_counters = [itertools.count(), itertools.count()]
        #build the block
        _Block.__init__(
            self,
            flow_graph=flow_graph,
            n=n,
        )
        _GUIBlock.__init__(self)

    def get_bus_structure(self, direction):
        if direction == 'source':
            bus_structure = self._bus_structure_source;
        else:
            bus_structure = self._bus_structure_sink;

        bus_structure = self.resolve_dependencies(bus_structure);

        if not bus_structure: return ''
        try:
            clean_bus_structure = self.get_parent().evaluate(bus_structure)
            return clean_bus_structure

        except: return ''

    def validate(self):
        """
        Validate this block.
        Call the base class validate.
        Evaluate the checks: each check must evaluate to True.
        """
        _Block.validate(self)
        #evaluate the checks
        for check in self._checks:
            check_res = self.resolve_dependencies(check)
            try:
                if not self.get_parent().evaluate(check_res):
                    self.add_error_message('Check "%s" failed.'%check)
            except: self.add_error_message('Check "%s" did not evaluate.'%check)
        # for variables check the value (only if var_value is used
        if _variable_matcher.match(self.get_key()) and self._var_value != '$value':
            value = self._var_value
            try:
                value = self.get_var_value()
                self.get_parent().evaluate(value)
            except Exception as err:
                self.add_error_message('Value "%s" cannot be evaluated:\n%s' % (value, err))

        # check if this is a GUI block and matches the selected generate option
        current_generate_option = self.get_parent().get_option('generate_options')

        def check_generate_mode(label, flag, valid_options):
            block_requires_mode = (
                flag in self.get_flags() or
                self.get_name().upper().startswith(label)
            )
            if block_requires_mode and current_generate_option not in valid_options:
                self.add_error_message("Can't generate this block in mode " +
                                       repr(current_generate_option))

        check_generate_mode('WX GUI', BLOCK_FLAG_NEED_WX_GUI, ('wx_gui',))
        check_generate_mode('QT GUI', BLOCK_FLAG_NEED_QT_GUI, ('qt_gui', 'hb_qt_gui'))

    def rewrite(self):
        """
        Add and remove ports to adjust for the nports.
        """
        _Block.rewrite(self)

        # adjust nports
        for ports in (self.get_sources(), self.get_sinks()):
            for i, master_port in enumerate(ports):
                nports = master_port.get_nports() or 1
                num_ports = 1 + len(master_port.get_clones())
                if not nports and num_ports == 1:  # not a master port and no left-over clones
                    continue
                # remove excess cloned ports
                for port in master_port.get_clones()[nports-1:]:
                    # remove excess connections
                    for connection in port.get_connections():
                        self.get_parent().remove_element(connection)
                    master_port.remove_clone(port)
                    ports.remove(port)
                # add more cloned ports
                for j in range(num_ports, nports):
                    port = master_port.add_clone()
                    ports.insert(ports.index(master_port) + j, port)

            self.back_ofthe_bus(ports)
            # renumber non-message/-msg ports
            domain_specific_port_index = collections.defaultdict(int)
            for port in filter(lambda p: p.get_key().isdigit(), ports):
                domain = port.get_domain()
                port._key = str(domain_specific_port_index[domain])
                domain_specific_port_index[domain] += 1

    def port_controller_modify(self, direction):
        """
        Change the port controller.

        Args:
            direction: +1 or -1

        Returns:
            true for change
        """
        changed = False
        #concat the nports string from the private nports settings of all ports
        nports_str = ' '.join([port._nports for port in self.get_ports()])
        #modify all params whose keys appear in the nports string
        for param in self.get_params():
            if param.is_enum() or param.get_key() not in nports_str: continue
            #try to increment the port controller by direction
            try:
                value = param.get_evaluated()
                value = value + direction
                if 0 < value:
                    param.set_value(value)
                    changed = True
            except: pass
        return changed

    def get_doc(self):
        doc = self._doc.strip('\n').replace('\\\n', '')
        #merge custom doc with doxygen docs
        return '\n'.join([doc, extract_docs.extract(self.get_key())]).strip('\n')

    def get_category(self):
        return _Block.get_category(self)

    def get_imports(self):
        """
        Resolve all import statements.
        Split each import statement at newlines.
        Combine all import statments into a list.
        Filter empty imports.

        Returns:
            a list of import statements
        """
        return filter(lambda i: i, sum(map(lambda i: self.resolve_dependencies(i).split('\n'), self._imports), []))

    def get_make(self): return self.resolve_dependencies(self._make)
    def get_var_make(self): return self.resolve_dependencies(self._var_make)
    def get_var_value(self): return self.resolve_dependencies(self._var_value)

    def get_callbacks(self):
        """
        Get a list of function callbacks for this block.

        Returns:
            a list of strings
        """
        def make_callback(callback):
            callback = self.resolve_dependencies(callback)
            if 'self.' in callback: return callback
            return 'self.%s.%s'%(self.get_id(), callback)
        return map(make_callback, self._callbacks)

    def is_virtual_sink(self):
        return self.get_key() == 'virtual_sink'

    def is_virtual_source(self):
        return self.get_key() == 'virtual_source'

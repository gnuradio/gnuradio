# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


from ..utils import expr_utils
from operator import methodcaller, attrgetter


class FlowGraphProxy(object):  # TODO: move this in a refactored Generator

    def __init__(self, fg):
        self.orignal_flowgraph = fg

    def __getattr__(self, item):
        return getattr(self.orignal_flowgraph, item)

    def get_hier_block_stream_io(self, direction):
        """
        Get a list of stream io signatures for this flow graph.

        Args:
            direction: a string of 'in' or 'out'

        Returns:
            a list of dicts with: type, label, vlen, size, optional
        """
        return [p for p in self.get_hier_block_io(direction) if p['type'] != "message"]

    def get_hier_block_message_io(self, direction):
        """
        Get a list of message io signatures for this flow graph.

        Args:
            direction: a string of 'in' or 'out'

        Returns:
            a list of dicts with: type, label, vlen, size, optional
        """
        return [p for p in self.get_hier_block_io(direction) if p['type'] == "message"]

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
            type_param = pad.params['type']
            master = {
                'label': str(pad.params['label'].get_evaluated()),
                'type': str(pad.params['type'].get_evaluated()),
                'vlen': str(pad.params['vlen'].get_value()),
                'size': type_param.options.attributes[type_param.get_value()]['size'],
                'cpp_size': type_param.options.attributes[type_param.get_value()]['cpp_size'],
                'optional': bool(pad.params['optional'].get_evaluated()),
            }
            num_ports = pad.params['num_streams'].get_evaluated()
            if num_ports > 1:
                for i in range(num_ports):
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
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_source']
        return sorted(pads, key=lambda x: x.name)

    def get_pad_sinks(self):
        """
        Get a list of pad sink blocks sorted by id order.

        Returns:
            a list of pad sink blocks in this flow graph
        """
        pads = [b for b in self.get_enabled_blocks() if b.key == 'pad_sink']
        return sorted(pads, key=lambda x: x.name)

    def get_pad_port_global_key(self, port):
        """
        Get the key for a port of a pad source/sink to use in connect()
        This takes into account that pad blocks may have multiple ports

        Returns:
            the key (str)
        """
        key_offset = 0
        pads = self.get_pad_sources() if port.is_source else self.get_pad_sinks()
        for pad in pads:
            # using the block param 'type' instead of the port domain here
            # to emphasize that hier block generation is domain agnostic
            is_message_pad = pad.params['type'].get_evaluated() == "message"
            if port.parent == pad:
                if is_message_pad:
                    key = pad.params['label'].get_value()
                else:
                    key = str(key_offset + int(port.key))
                return key
            else:
                # assuming we have either only sources or sinks
                if not is_message_pad:
                    key_offset += len(pad.sinks) + len(pad.sources)
        return -1

    def get_cpp_variables(self):
        """
        Get a list of all variables (C++) in this flow graph namespace.
        Exclude parameterized variables.

        Returns:
            a sorted list of variable blocks in order of dependency (indep -> dep)
        """
        variables = [block for block in self.iter_enabled_blocks()
                     if block.is_variable]
        return expr_utils.sort_objects(variables, attrgetter('name'), methodcaller('get_cpp_var_make'))

    def includes(self):
        """
        Get a set of all include statements (C++) in this flow graph namespace.

        Returns:
            a list of #include statements
        """
        return [block.cpp_templates.render('includes') for block in self.iter_enabled_blocks() if not (block.is_virtual_sink() or block.is_virtual_source())]

    def links(self):
        """
        Get a set of all libraries to link against (C++) in this flow graph namespace.

        Returns:
            a list of GNU Radio modules
        """
        return [block.cpp_templates.render('link') for block in self.iter_enabled_blocks() if not (block.is_virtual_sink() or block.is_virtual_source())]

    def packages(self):
        """
        Get a set of all packages  to find (C++) ( especially for oot modules ) in this flow graph namespace.

        Returns:
            a list of required packages
        """
        return [block.cpp_templates.render('packages') for block in self.iter_enabled_blocks() if not (block.is_virtual_sink() or block.is_virtual_source())]


def get_hier_block_io(flow_graph, direction, domain=None):
    """
    Get a list of io ports for this flow graph.

    Returns a list of dicts with: type, label, vlen, size, optional
    """
    pads = flow_graph.get_pad_sources() if direction in ('sink', 'in') else \
        flow_graph.get_pad_sinks() if direction in ('source', 'out') else []
    ports = []
    for pad in pads:
        type_param = pad.params['type']
        master = {
            'label': str(pad.params['label'].get_evaluated()),
            'type': str(pad.params['type'].get_evaluated()),
            'vlen': str(pad.params['vlen'].get_value()),
            'size': type_param.options.attributes[type_param.get_value()]['size'],
            'optional': bool(pad.params['optional'].get_evaluated()),
        }
        num_ports = pad.params['num_streams'].get_evaluated()
        if num_ports > 1:
            for i in range(num_ports):
                clone = master.copy()
                clone['label'] += str(i)
                ports.append(clone)
        else:
            ports.append(master)
    if domain is not None:
        ports = [p for p in ports if p.domain == domain]
    return ports

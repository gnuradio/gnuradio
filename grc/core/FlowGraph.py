# Copyright 2008-2015 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import, print_function

import imp
import time
import re
from operator import methodcaller
import collections
import sys

from . import Messages
from .Constants import FLOW_GRAPH_FILE_FORMAT_VERSION
from .Element import Element
from .utils import expr_utils, shlex

_parameter_matcher = re.compile('^(parameter)$')
_monitors_searcher = re.compile('(ctrlport_monitor)')
_bussink_searcher = re.compile('^(bus_sink)$')
_bussrc_searcher = re.compile('^(bus_source)$')
_bus_struct_sink_searcher = re.compile('^(bus_structure_sink)$')
_bus_struct_src_searcher = re.compile('^(bus_structure_source)$')


class FlowGraph(Element):

    is_flow_graph = True

    def __init__(self, parent):
        """
        Make a flow graph from the arguments.

        Args:
            parent: a platforms with blocks and element factories

        Returns:
            the flow graph object
        """
        Element.__init__(self, parent)
        self._timestamp = time.ctime()
        self._options_block = self.parent_platform.get_new_block(self, 'options')

        self.blocks = [self._options_block]
        self.connections = []

        self._eval_cache = {}
        self.namespace = {}

        self.grc_file_path = ''

    def __str__(self):
        return 'FlowGraph - {}({})'.format(self.get_option('title'), self.get_option('id'))

    def get_imports(self):
        """
        Get a set of all import statements in this flow graph namespace.

        Returns:
            a set of import statements
        """
        imports = sum([block.get_imports() for block in self.iter_enabled_blocks()], [])
        return sorted(set(imports))

    def get_variables(self):
        """
        Get a list of all variables in this flow graph namespace.
        Exclude parameterized variables.

        Returns:
            a sorted list of variable blocks in order of dependency (indep -> dep)
        """
        variables = [block for block in self.iter_enabled_blocks() if block.is_variable]
        return expr_utils.sort_objects(variables, methodcaller('get_id'), methodcaller('get_var_make'))

    def get_parameters(self):
        """
        Get a list of all parameterized variables in this flow graph namespace.

        Returns:
            a list of parameterized variables
        """
        parameters = [b for b in self.iter_enabled_blocks() if _parameter_matcher.match(b.key)]
        return parameters

    def get_monitors(self):
        """
        Get a list of all ControlPort monitors
        """
        monitors = [b for b in self.iter_enabled_blocks() if _monitors_searcher.search(b.key)]
        return monitors

    def get_python_modules(self):
        """Iterate over custom code block ID and Source"""
        for block in self.iter_enabled_blocks():
            if block.key == 'epy_module':
                yield block.get_id(), block.get_param('source_code').get_value()

    def get_bussink(self):
        bussink = [b for b in self.get_enabled_blocks() if _bussink_searcher.search(b.key)]

        for i in bussink:
            for j in i.params.values():
                if j.name == 'On/Off' and j.get_value() == 'on':
                    return True
        return False

    def get_bussrc(self):
        bussrc = [b for b in self.get_enabled_blocks() if _bussrc_searcher.search(b.key)]

        for i in bussrc:
            for j in i.params.values():
                if j.name == 'On/Off' and j.get_value() == 'on':
                    return True
        return False

    def get_bus_structure_sink(self):
        bussink = [b for b in self.get_enabled_blocks() if _bus_struct_sink_searcher.search(b.key)]
        return bussink

    def get_bus_structure_src(self):
        bussrc = [b for b in self.get_enabled_blocks() if _bus_struct_src_searcher.search(b.key)]
        return bussrc

    def iter_enabled_blocks(self):
        """
        Get an iterator of all blocks that are enabled and not bypassed.
        """
        return (block for block in self.blocks if block.enabled)

    def get_enabled_blocks(self):
        """
        Get a list of all blocks that are enabled and not bypassed.

        Returns:
            a list of blocks
        """
        return list(self.iter_enabled_blocks())

    def get_bypassed_blocks(self):
        """
        Get a list of all blocks that are bypassed.

        Returns:
            a list of blocks
        """
        return [block for block in self.blocks if block.get_bypassed()]

    def get_enabled_connections(self):
        """
        Get a list of all connections that are enabled.

        Returns:
            a list of connections
        """
        return [connection for connection in self.connections if connection.enabled]

    def get_option(self, key):
        """
        Get the option for a given key.
        The option comes from the special options block.

        Args:
            key: the param key for the options block

        Returns:
            the value held by that param
        """
        return self._options_block.get_param(key).get_evaluated()

    def get_run_command(self, file_path, split=False):
        run_command = self.get_option('run_command')
        try:
            run_command = run_command.format(
                python=shlex.quote(sys.executable),
                filename=shlex.quote(file_path))
            return shlex.split(run_command) if split else run_command
        except Exception as e:
            raise ValueError("Can't parse run command {!r}: {}".format(run_command, e))

    ##############################################
    # Access Elements
    ##############################################
    def get_block(self, id):
        for block in self.blocks:
            if block.get_id() == id:
                return block
        raise KeyError('No block with ID {!r}'.format(id))

    def get_elements(self):
        return self.blocks + self.connections

    get_children = get_elements

    def rewrite(self):
        """
        Flag the namespace to be renewed.
        """
        self.renew_namespace()
        Element.rewrite(self)

    def renew_namespace(self):
        namespace = {}
        # Load imports
        for expr in self.get_imports():
            try:
                exec(expr, namespace)
            except:
                pass

        for id, expr in self.get_python_modules():
            try:
                module = imp.new_module(id)
                exec(expr, module.__dict__)
                namespace[id] = module
            except:
                pass

        # Load parameters
        np = {}  # params don't know each other
        for parameter in self.get_parameters():
            try:
                value = eval(parameter.get_param('value').to_code(), namespace)
                np[parameter.get_id()] = value
            except:
                pass
        namespace.update(np)  # Merge param namespace

        # Load variables
        for variable in self.get_variables():
            try:
                value = eval(variable.get_var_value(), namespace)
                namespace[variable.get_id()] = value
            except:
                pass

        self.namespace.clear()
        self._eval_cache.clear()
        self.namespace.update(namespace)

    def evaluate(self, expr):
        """
        Evaluate the expression.

        Args:
            expr: the string expression
        @throw Exception bad expression

        Returns:
            the evaluated data
        """
        # Evaluate
        if not expr:
            raise Exception('Cannot evaluate empty statement.')
        return self._eval_cache.setdefault(expr, eval(expr, self.namespace))

    ##############################################
    # Add/remove stuff
    ##############################################

    def new_block(self, key, **kwargs):
        """
        Get a new block of the specified key.
        Add the block to the list of elements.

        Args:
            key: the block key

        Returns:
            the new block or None if not found
        """
        if key == 'options':
            return self._options_block
        try:
            block = self.parent_platform.get_new_block(self, key, **kwargs)
            self.blocks.append(block)
        except KeyError:
            block = None
        return block

    def connect(self, porta, portb):
        """
        Create a connection between porta and portb.

        Args:
            porta: a port
            portb: another port
        @throw Exception bad connection

        Returns:
            the new connection
        """

        connection = self.parent_platform.Connection(
            parent=self, porta=porta, portb=portb)
        self.connections.append(connection)
        return connection

    def remove_element(self, element):
        """
        Remove the element from the list of elements.
        If the element is a port, remove the whole block.
        If the element is a block, remove its connections.
        If the element is a connection, just remove the connection.
        """
        if element is self._options_block:
            return

        if element.is_port:
            # Found a port, set to parent signal block
            element = element.parent

        if element in self.blocks:
            # Remove block, remove all involved connections
            for port in element.get_ports():
                for connection in port.get_connections():
                    self.remove_element(connection)
            self.blocks.remove(element)

        elif element in self.connections:
            if element.is_bus():
                for port in element.source_port.get_associated_ports():
                    for connection in port.get_connections():
                        self.remove_element(connection)
            self.connections.remove(element)

    ##############################################
    # Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this flow graph to nested data.
        Export all block and connection data.

        Returns:
            a nested data odict
        """
        # sort blocks and connections for nicer diffs
        blocks = sorted(self.blocks, key=lambda b: (
            b.key != 'options',  # options to the front
            not b.key.startswith('variable'),  # then vars
            str(b)
        ))
        connections = sorted(self.connections, key=str)
        n = collections.OrderedDict()
        n['timestamp'] = self._timestamp
        n['block'] = [b.export_data() for b in blocks]
        n['connection'] = [c.export_data() for c in connections]
        instructions = collections.OrderedDict()
        instructions['created'] = '.'.join(self.parent.config.version_parts)
        instructions['format'] = FLOW_GRAPH_FILE_FORMAT_VERSION
        return {'flow_graph': n, '_instructions': instructions}

    def import_data(self, n):
        """
        Import blocks and connections into this flow graph.
        Clear this flow graph of all previous blocks and connections.
        Any blocks or connections in error will be ignored.

        Args:
            n: the nested data odict
        """
        # Remove previous elements
        del self.blocks[:]
        del self.connections[:]
        # set file format
        try:
            instructions = n.get('_instructions', {})
            file_format = int(instructions.get('format', '0')) or _guess_file_format_1(n)
        except:
            file_format = 0

        fg_n = n and n.get('flow_graph', {})  # use blank data if none provided
        self._timestamp = fg_n.get('timestamp', time.ctime())

        # build the blocks
        self.blocks.append(self._options_block)
        for block_n in fg_n.get('block', []):
            key = block_n['key']
            block = self.new_block(key)

            if not block:
                # we're before the initial fg update(), so no evaluated values!
                # --> use raw value instead
                path_param = self._options_block.get_param('hier_block_src_path')
                file_path = self.parent_platform.find_file_in_paths(
                    filename=key + '.grc',
                    paths=path_param.get_value(),
                    cwd=self.grc_file_path
                )
                if file_path:  # grc file found. load and get block
                    self.parent_platform.load_and_generate_flow_graph(file_path, hier_only=True)
                    block = self.new_block(key)  # can be None

            if not block:  # looks like this block key cannot be found
                # create a dummy block instead
                block = self.new_block('_dummy', missing_key=key,
                                       params_n=block_n.get('param', []))
                print('Block key "%s" not found' % key)

            block.import_data(block_n)

        self.rewrite()  # evaluate stuff like nports before adding connections

        # build the connections
        def verify_and_get_port(key, block, dir):
            ports = block.sinks if dir == 'sink' else block.sources
            for port in ports:
                if key == port.key:
                    break
                if not key.isdigit() and port.get_type() == '' and key == port.name:
                    break
            else:
                if block.is_dummy_block:
                    port = block.add_missing_port(key, dir)
                else:
                    raise LookupError('%s key %r not in %s block keys' % (dir, key, dir))
            return port

        errors = False
        for connection_n in fg_n.get('connection', []):
            # get the block ids and port keys
            source_block_id = connection_n.get('source_block_id')
            sink_block_id = connection_n.get('sink_block_id')
            source_key = connection_n.get('source_key')
            sink_key = connection_n.get('sink_key')
            try:
                source_block = self.get_block(source_block_id)
                sink_block = self.get_block(sink_block_id)

                # fix old, numeric message ports keys
                if file_format < 1:
                    source_key, sink_key = _update_old_message_port_keys(
                        source_key, sink_key, source_block, sink_block)

                # build the connection
                source_port = verify_and_get_port(source_key, source_block, 'source')
                sink_port = verify_and_get_port(sink_key, sink_block, 'sink')
                self.connect(source_port, sink_port)
            except LookupError as e:
                Messages.send_error_load(
                    'Connection between {}({}) and {}({}) could not be made.\n\t{}'.format(
                        source_block_id, source_key, sink_block_id, sink_key, e))
                errors = True

        self.rewrite()  # global rewrite
        return errors


def _update_old_message_port_keys(source_key, sink_key, source_block, sink_block):
    """
    Backward compatibility for message port keys

    Message ports use their names as key (like in the 'connect' method).
    Flowgraph files from former versions still have numeric keys stored for
    message connections. These have to be replaced by the name of the
    respective port. The correct message port is deduced from the integer
    value of the key (assuming the order has not changed).

    The connection ends are updated only if both ends translate into a
    message port.
    """
    try:
        # get ports using the "old way" (assuming liner indexed keys)
        source_port = source_block.sources[int(source_key)]
        sink_port = sink_block.sinks[int(sink_key)]
        if source_port.get_type() == "message" and sink_port.get_type() == "message":
            source_key, sink_key = source_port.key, sink_port.key
    except (ValueError, IndexError):
        pass
    return source_key, sink_key  # do nothing


def _guess_file_format_1(n):
    """
    Try to guess the file format for flow-graph files without version tag
    """
    try:
        has_non_numeric_message_keys = any(not (
            connection_n.get('source_key', '').isdigit() and
            connection_n.get('sink_key', '').isdigit()
        ) for connection_n in n.get('flow_graph', []).get('connection', []))
        if has_non_numeric_message_keys:
            return 1
    except:
        pass
    return 0

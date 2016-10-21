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

import imp
import time
from itertools import ifilter, chain
from operator import methodcaller, attrgetter

import re

from . import Messages
from .Constants import FLOW_GRAPH_FILE_FORMAT_VERSION
from .Element import Element
from .utils import odict, expr_utils

_parameter_matcher = re.compile('^(parameter)$')
_monitors_searcher = re.compile('(ctrlport_monitor)')
_bussink_searcher = re.compile('^(bus_sink)$')
_bussrc_searcher = re.compile('^(bus_source)$')
_bus_struct_sink_searcher = re.compile('^(bus_structure_sink)$')
_bus_struct_src_searcher = re.compile('^(bus_structure_source)$')


class FlowGraph(Element):

    is_flow_graph = True

    def __init__(self, platform):
        """
        Make a flow graph from the arguments.

        Args:
            platform: a platforms with blocks and contrcutors

        Returns:
            the flow graph object
        """
        Element.__init__(self, platform)
        self._elements = []
        self._timestamp = time.ctime()

        self.platform = platform  # todo: make this a lazy prop
        self.blocks = []
        self.connections = []

        self._eval_cache = {}
        self.namespace = {}

        self.grc_file_path = ''
        self._options_block = self.new_block('options')

    def __str__(self):
        return 'FlowGraph - {0}({1})'.format(self.get_option('title'), self.get_option('id'))

    ##############################################
    # TODO: Move these to new generator package
    ##############################################
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
        variables = filter(attrgetter('is_variable'), self.iter_enabled_blocks())
        return expr_utils.sort_objects(variables, methodcaller('get_id'), methodcaller('get_var_make'))

    def get_parameters(self):
        """
        Get a list of all parameterized variables in this flow graph namespace.

        Returns:
            a list of parameterized variables
        """
        parameters = filter(lambda b: _parameter_matcher.match(b.get_key()), self.iter_enabled_blocks())
        return parameters

    def get_monitors(self):
        """
        Get a list of all ControlPort monitors
        """
        monitors = filter(lambda b: _monitors_searcher.search(b.get_key()),
                          self.iter_enabled_blocks())
        return monitors

    def get_python_modules(self):
        """Iterate over custom code block ID and Source"""
        for block in self.iter_enabled_blocks():
            if block.get_key() == 'epy_module':
                yield block.get_id(), block.get_param('source_code').get_value()

    def get_bussink(self):
        bussink = filter(lambda b: _bussink_searcher.search(b.get_key()), self.get_enabled_blocks())

        for i in bussink:
            for j in i.get_params():
                if j.get_name() == 'On/Off' and j.get_value() == 'on':
                    return True
        return False

    def get_bussrc(self):
        bussrc = filter(lambda b: _bussrc_searcher.search(b.get_key()), self.get_enabled_blocks())

        for i in bussrc:
            for j in i.get_params():
                if j.get_name() == 'On/Off' and j.get_value() == 'on':
                    return True
        return False

    def get_bus_structure_sink(self):
        bussink = filter(lambda b: _bus_struct_sink_searcher.search(b.get_key()), self.get_enabled_blocks())
        return bussink

    def get_bus_structure_src(self):
        bussrc = filter(lambda b: _bus_struct_src_searcher.search(b.get_key()), self.get_enabled_blocks())
        return bussrc

    def iter_enabled_blocks(self):
        """
        Get an iterator of all blocks that are enabled and not bypassed.
        """
        return ifilter(methodcaller('get_enabled'), self.blocks)

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
        return filter(methodcaller('get_bypassed'), self.blocks)

    def get_enabled_connections(self):
        """
        Get a list of all connections that are enabled.

        Returns:
            a list of connections
        """
        return filter(methodcaller('get_enabled'), self.connections)

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

    ##############################################
    # Access Elements
    ##############################################
    def get_block(self, id):
        for block in self.blocks:
            if block.get_id() == id:
                return block
        raise KeyError('No block with ID {!r}'.format(id))

    def get_elements(self):
        """
        Get a list of all the elements.
        Always ensure that the options block is in the list (only once).

        Returns:
            the element list
        """
        options_block_count = self.blocks.count(self._options_block)
        if not options_block_count:
            self.blocks.append(self._options_block)
        for i in range(options_block_count-1):
            self.blocks.remove(self._options_block)

        return self.blocks + self.connections

    get_children = get_elements

    def rewrite(self):
        """
        Flag the namespace to be renewed.
        """

        self.renew_namespace()
        for child in chain(self.blocks, self.connections):
            child.rewrite()

        self.bus_ports_rewrite()

    def renew_namespace(self):
        namespace = {}
        # Load imports
        for expr in self.get_imports():
            try:
                exec expr in namespace
            except:
                pass

        for id, expr in self.get_python_modules():
            try:
                module = imp.new_module(id)
                exec expr in module.__dict__
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

    def new_block(self, key):
        """
        Get a new block of the specified key.
        Add the block to the list of elements.

        Args:
            key: the block key

        Returns:
            the new block or None if not found
        """
        try:
            block = self.platform.get_new_block(self, key)
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

        connection = self.platform.Connection(
            flow_graph=self, porta=porta, portb=portb)
        self.connections.append(connection)
        return connection

    def remove_element(self, element):
        """
        Remove the element from the list of elements.
        If the element is a port, remove the whole block.
        If the element is a block, remove its connections.
        If the element is a connection, just remove the connection.
        """
        if element.is_port:
            # Found a port, set to parent signal block
            element = element.get_parent()

        if element in self.blocks:
            # Remove block, remove all involved connections
            for port in element.get_ports():
                map(self.remove_element, port.get_connections())
            self.blocks.remove(element)

        elif element in self.connections:
            if element.is_bus():
                cons_list = []
                for i in map(lambda a: a.get_connections(), element.get_source().get_associated_ports()):
                    cons_list.extend(i)
                map(self.remove_element, cons_list)
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
            b.get_key() != 'options',  # options to the front
            not b.get_key().startswith('variable'),  # then vars
            str(b)
        ))
        connections = sorted(self.connections, key=str)
        n = odict()
        n['timestamp'] = self._timestamp
        n['block'] = [b.export_data() for b in blocks]
        n['connection'] = [c.export_data() for c in connections]
        instructions = odict({
            'created': '.'.join(self.get_parent().config.version_parts),
            'format': FLOW_GRAPH_FILE_FORMAT_VERSION,
        })
        return odict({'flow_graph': n, '_instructions': instructions})

    def import_data(self, n):
        """
        Import blocks and connections into this flow graph.
        Clear this flowgraph of all previous blocks and connections.
        Any blocks or connections in error will be ignored.

        Args:
            n: the nested data odict
        """
        # Remove previous elements
        del self.blocks[:]
        del self.connections[:]
        # set file format
        try:
            instructions = n.find('_instructions') or {}
            file_format = int(instructions.get('format', '0')) or _guess_file_format_1(n)
        except:
            file_format = 0

        fg_n = n and n.find('flow_graph') or odict()  # use blank data if none provided
        self._timestamp = fg_n.find('timestamp') or time.ctime()

        # build the blocks
        self._options_block = self.new_block('options')
        for block_n in fg_n.findall('block'):
            key = block_n.find('key')
            block = self._options_block if key == 'options' else self.new_block(key)

            if not block:
                # we're before the initial fg update(), so no evaluated values!
                # --> use raw value instead
                path_param = self._options_block.get_param('hier_block_src_path')
                file_path = self.platform.find_file_in_paths(
                    filename=key + '.grc',
                    paths=path_param.get_value(),
                    cwd=self.grc_file_path
                )
                if file_path:  # grc file found. load and get block
                    self.platform.load_and_generate_flow_graph(file_path)
                    block = self.new_block(key)  # can be None

            if not block:  # looks like this block key cannot be found
                # create a dummy block instead
                block = self.new_block('dummy_block')
                # Ugly ugly ugly
                _initialize_dummy_block(block, block_n)
                print('Block key "%s" not found' % key)

            block.import_data(block_n)

        self.rewrite()  # evaluate stuff like nports before adding connections

        # build the connections
        def verify_and_get_port(key, block, dir):
            ports = block.get_sinks() if dir == 'sink' else block.get_sources()
            for port in ports:
                if key == port.get_key():
                    break
                if not key.isdigit() and port.get_type() == '' and key == port.get_name():
                    break
            else:
                if block.is_dummy_block:
                    port = _dummy_block_add_port(block, key, dir)
                else:
                    raise LookupError('%s key %r not in %s block keys' % (dir, key, dir))
            return port

        errors = False
        for connection_n in fg_n.findall('connection'):
            # get the block ids and port keys
            source_block_id = connection_n.find('source_block_id')
            sink_block_id = connection_n.find('sink_block_id')
            source_key = connection_n.find('source_key')
            sink_key = connection_n.find('sink_key')
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
                    'Connection between {0}({1}) and {2}({3}) could not be made.\n\t{4}'.format(
                        source_block_id, source_key, sink_block_id, sink_key, e))
                errors = True

        self.rewrite()  # global rewrite
        return errors

    ##############################################
    # Needs to go
    ##############################################
    def bus_ports_rewrite(self):
        # todo: move to block.rewrite()
        for block in self.blocks:
            for direc in ['source', 'sink']:
                if direc == 'source':
                    get_p = block.get_sources
                    get_p_gui = block.get_sources_gui
                    bus_structure = block.form_bus_structure('source')
                else:
                    get_p = block.get_sinks
                    get_p_gui = block.get_sinks_gui
                    bus_structure = block.form_bus_structure('sink')

                if 'bus' in map(lambda a: a.get_type(), get_p_gui()):
                    if len(get_p_gui()) > len(bus_structure):
                        times = range(len(bus_structure), len(get_p_gui()))
                        for i in times:
                            for connect in get_p_gui()[-1].get_connections():
                                block.get_parent().remove_element(connect)
                            get_p().remove(get_p_gui()[-1])
                    elif len(get_p_gui()) < len(bus_structure):
                        n = {'name': 'bus', 'type': 'bus'}
                        if True in map(
                                lambda a: isinstance(a.get_nports(), int),
                                get_p()):
                            n['nports'] = str(1)

                        times = range(len(get_p_gui()), len(bus_structure))

                        for i in times:
                            n['key'] = str(len(get_p()))
                            n = odict(n)
                            port = block.get_parent().get_parent().Port(
                                block=block, n=n, dir=direc)
                            get_p().append(port)

                if 'bus' in map(lambda a: a.get_type(),
                                block.get_sources_gui()):
                    for i in range(len(block.get_sources_gui())):
                        if len(block.get_sources_gui()[
                                   i].get_connections()) > 0:
                            source = block.get_sources_gui()[i]
                            sink = []

                            for j in range(len(source.get_connections())):
                                sink.append(
                                    source.get_connections()[j].get_sink())
                            for elt in source.get_connections():
                                self.remove_element(elt)
                            for j in sink:
                                self.connect(source, j)


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
        source_port = source_block.get_sources()[int(source_key)]
        sink_port = sink_block.get_sinks()[int(sink_key)]
        if source_port.get_type() == "message" and sink_port.get_type() == "message":
            source_key, sink_key = source_port.get_key(), sink_port.get_key()
    except (ValueError, IndexError):
        pass
    return source_key, sink_key  # do nothing


def _guess_file_format_1(n):
    """
    Try to guess the file format for flow-graph files without version tag
    """
    try:
        has_non_numeric_message_keys = any(not (
            connection_n.find('source_key').isdigit() and
            connection_n.find('sink_key').isdigit()
        ) for connection_n in n.find('flow_graph').findall('connection'))
        if has_non_numeric_message_keys:
            return 1
    except:
        pass
    return 0


def _initialize_dummy_block(block, block_n):
    """
    This is so ugly... dummy-fy a block
    Modify block object to get the behaviour for a missing block
    """

    block._key = block_n.find('key')
    block.is_dummy_block = lambda: True
    block.is_valid = lambda: False
    block.get_enabled = lambda: False
    for param_n in block_n.findall('param'):
        if param_n['key'] not in block.get_param_keys():
            new_param_n = odict({'key': param_n['key'], 'name': param_n['key'], 'type': 'string'})
            params = block.get_parent().get_parent().Param(block=block, n=new_param_n)
            block.get_params().append(params)


def _dummy_block_add_port(block, key, dir):
    """ This is so ugly... Add a port to a dummy-field block """
    port_n = odict({'name': '?', 'key': key, 'type': ''})
    port = block.get_parent().get_parent().Port(block=block, n=port_n, dir=dir)
    if port.is_source:
        block.get_sources().append(port)
    else:
        block.get_sinks().append(port)
    return port

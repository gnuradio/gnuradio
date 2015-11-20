"""
Copyright 2008-2015 Free Software Foundation, Inc.
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
import imp
import time
from operator import methodcaller
from itertools import ifilter

from ..gui import Messages

from . import expr_utils
from .odict import odict
from .Element import Element
from .Constants import FLOW_GRAPH_FILE_FORMAT_VERSION

_variable_matcher = re.compile('^(variable\w*)$')
_parameter_matcher = re.compile('^(parameter)$')
_monitors_searcher = re.compile('(ctrlport_monitor)')
_bussink_searcher = re.compile('^(bus_sink)$')
_bussrc_searcher = re.compile('^(bus_source)$')
_bus_struct_sink_searcher = re.compile('^(bus_structure_sink)$')
_bus_struct_src_searcher = re.compile('^(bus_structure_source)$')


def _initialize_dummy_block(block, block_n):
    """This is so ugly... dummy-fy a block

    Modify block object to get the behaviour for a missing block
    """
    block._key = block_n.find('key')
    block.is_dummy_block = lambda: True
    block.is_valid = lambda: False
    block.get_enabled = lambda: False
    for param_n in block_n.findall('param'):
        if param_n['key'] not in block.get_param_keys():
            new_param_n = odict({'key': param_n['key'], 'name': param_n['key'], 'type': 'string'})
            block.get_params().append(block.get_parent().get_parent().Param(block=block, n=new_param_n))


def _dummy_block_add_port(block, key, dir):
    """This is so ugly... Add a port to a dummy-field block"""
    port_n = odict({'name': '?', 'key': key, 'type': ''})
    port = block.get_parent().get_parent().Port(block=block, n=port_n, dir=dir)
    if port.is_source():
        block.get_sources().append(port)
    else:
        block.get_sinks().append(port)
    return port


class FlowGraph(Element):

    def __init__(self, platform):
        self.grc_file_path = ''
        """
        Make a flow graph from the arguments.

        Args:
            platform: a platforms with blocks and contrcutors

        Returns:
            the flow graph object
        """
        #initialize
        Element.__init__(self, platform)
        self._elements = []
        self._timestamp = time.ctime()
        #inital blank import
        self.import_data()

        self.n = {}
        self.n_hash = -1
        self._renew_eval_ns = True
        self._eval_cache = {}

    def _get_unique_id(self, base_id=''):
        """
        Get a unique id starting with the base id.

        Args:
            base_id: the id starts with this and appends a count

        Returns:
            a unique id
        """
        index = 0
        while True:
            id = '%s_%d' % (base_id, index)
            index += 1
            #make sure that the id is not used by another block
            if not filter(lambda b: b.get_id() == id, self.get_blocks()): return id

    def __str__(self):
        return 'FlowGraph - %s(%s)' % (self.get_option('title'), self.get_option('id'))

    def get_complexity(self):
        """
        Determines the complexity of a flowgraph
        """
        dbal = 0
        block_list = self.get_blocks()
        for block in block_list:
            # Skip options block
            if block.get_key() == 'options':
                continue

            # Don't worry about optional sinks?
            sink_list = filter(lambda c: not c.get_optional(), block.get_sinks())
            source_list = filter(lambda c: not c.get_optional(), block.get_sources())
            sinks = float(len(sink_list))
            sources = float(len(source_list))
            base = max(min(sinks, sources), 1)

            # Port ratio multiplier
            if min(sinks, sources) > 0:
                multi = sinks / sources
                multi = (1 / multi) if multi > 1 else multi
            else:
                multi = 1

            # Connection ratio multiplier
            sink_multi = max(float(sum(map(lambda c: len(c.get_connections()), sink_list)) / max(sinks, 1.0)), 1.0)
            source_multi = max(float(sum(map(lambda c: len(c.get_connections()), source_list)) / max(sources, 1.0)), 1.0)
            dbal = dbal + (base * multi * sink_multi * source_multi)

        elements = float(len(self.get_elements()))
        connections = float(len(self.get_connections()))
        disabled_connections = len(filter(lambda c: not c.get_enabled(), self.get_connections()))
        blocks = float(len(block_list))
        variables = elements - blocks - connections
        enabled = float(len(self.get_enabled_blocks()))

        # Disabled multiplier
        if enabled > 0:
            disabled_multi = 1 / (max(1 - ((blocks - enabled) / max(blocks, 1)), 0.05))
        else:
            disabled_multi = 1

        # Connection multiplier (How many connections )
        if (connections - disabled_connections) > 0:
            conn_multi = 1 / (max(1 - (disabled_connections / max(connections, 1)), 0.05))
        else:
            conn_multi = 1

        final = round(max((dbal - 1) * disabled_multi * conn_multi * connections, 0.0) / 1000000, 6)
        return final

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
            # using the block param 'type' instead of the port domain here
            # to emphasize that hier block generation is domain agnostic
            is_message_pad = pad.get_param('type').get_evaluated() == "message"
            if port.get_parent() == pad:
                if is_message_pad:
                    key = pad.get_param('label').get_value()
                else:
                    key = str(key_offset + int(port.get_key()))
                return key
            else:
                # assuming we have either only sources or sinks
                if not is_message_pad:
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
        variables = filter(lambda b: _variable_matcher.match(b.get_key()), self.iter_enabled_blocks())
        return expr_utils.sort_objects(variables, methodcaller('get_id'), methodcaller('get_var_make'))

    def get_parameters(self):
        """
        Get a list of all paramterized variables in this flow graph namespace.

        Returns:
            a list of paramterized variables
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

    def iter_enabled_blocks(self):
        """
        Get an iterator of all blocks that are enabled and not bypassed.
        """
        return ifilter(methodcaller('get_enabled'), self.iter_blocks())

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
        return filter(methodcaller('get_bypassed'), self.iter_blocks())

    def get_enabled_connections(self):
        """
        Get a list of all connections that are enabled.

        Returns:
            a list of connections
        """
        return filter(methodcaller('get_enabled'), self.get_connections())

    def _get_new_block(self, key):
        """
        Get a new block of the specified key.
        Add the block to the list of elements.

        Args:
            key: the block key

        Returns:
            the new block or None if not found
        """
        if key not in self.get_parent().get_block_keys(): return None
        block = self.get_parent().get_new_block(self, key)
        self.get_elements().append(block);
        if block._bussify_sink:
            block.bussify({'name':'bus','type':'bus'}, 'sink')
        if block._bussify_source:
            block.bussify({'name':'bus','type':'bus'}, 'source')
        return block;

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
        connection = self.get_parent().Connection(flow_graph=self, porta=porta, portb=portb)
        self.get_elements().append(connection)
        return connection

    def remove_element(self, element):
        """
        Remove the element from the list of elements.
        If the element is a port, remove the whole block.
        If the element is a block, remove its connections.
        If the element is a connection, just remove the connection.
        """
        if element not in self.get_elements(): return
        #found a port, set to parent signal block
        if element.is_port():
            element = element.get_parent()
        #remove block, remove all involved connections
        if element.is_block():
            for port in element.get_ports():
                map(self.remove_element, port.get_connections())
        if element.is_connection():
            if element.is_bus():
                cons_list = []
                for i in map(lambda a: a.get_connections(), element.get_source().get_associated_ports()):
                    cons_list.extend(i);
                map(self.remove_element, cons_list);
        self.get_elements().remove(element)

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

    def is_flow_graph(self): return True

    ##############################################
    ## Access Elements
    ##############################################
    def get_block(self, id):
        for block in self.iter_blocks():
            if block.get_id() == id:
                return block
        raise KeyError('No block with ID {0!r}'.format(id))

    def iter_blocks(self):
        return ifilter(methodcaller('is_block'), self.get_elements())

    def get_blocks(self):
        return list(self.iter_blocks())

    def iter_connections(self):
        return ifilter(methodcaller('is_connection'), self.get_elements())

    def get_connections(self):
        return list(self.iter_connections())

    def get_elements(self):
        """
        Get a list of all the elements.
        Always ensure that the options block is in the list (only once).

        Returns:
            the element list
        """
        options_block_count = self._elements.count(self._options_block)
        if not options_block_count:
            self._elements.append(self._options_block)
        for i in range(options_block_count-1):
            self._elements.remove(self._options_block)
        return self._elements

    get_children = get_elements

    ### TODO >>> THIS SUCKS ###
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

        def refactor_bus_structure():

            for block in self.get_blocks():
                for direc in ['source', 'sink']:
                    if direc == 'source':
                        get_p = block.get_sources;
                        get_p_gui = block.get_sources_gui;
                        bus_structure = block.form_bus_structure('source');
                    else:
                        get_p = block.get_sinks;
                        get_p_gui = block.get_sinks_gui
                        bus_structure = block.form_bus_structure('sink');

                    if 'bus' in map(lambda a: a.get_type(), get_p_gui()):
                        if len(get_p_gui()) > len(bus_structure):
                            times = range(len(bus_structure), len(get_p_gui()));
                            for i in times:
                                for connect in get_p_gui()[-1].get_connections():
                                    block.get_parent().remove_element(connect);
                                get_p().remove(get_p_gui()[-1]);
                        elif len(get_p_gui()) < len(bus_structure):
                            n = {'name':'bus','type':'bus'};
                            if True in map(lambda a: isinstance(a.get_nports(), int), get_p()):
                                n['nports'] = str(1);

                            times = range(len(get_p_gui()), len(bus_structure));

                            for i in times:
                                n['key'] = str(len(get_p()));
                                n = odict(n);
                                port = block.get_parent().get_parent().Port(block=block, n=n, dir=direc);
                                get_p().append(port);

        for child in self.get_children(): child.rewrite()

        refactor_bus_structure()
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
            for code in self.get_imports():
                try: exec code in n
                except: pass

            for id, code in self.get_python_modules():
                try:
                    module = imp.new_module(id)
                    exec code in module.__dict__
                    n[id] = module
                except:
                    pass

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

    def get_new_block(self, key):
        """Try to auto-generate the block from file if missing"""
        block = self._get_new_block(key)
        if not block:
            platform = self.get_parent()
            # we're before the initial fg rewrite(), so no evaluated values!
            # --> use raw value instead
            path_param = self._options_block.get_param('hier_block_src_path')
            file_path = platform.find_file_in_paths(
                filename=key + '.' + platform.get_key(),
                paths=path_param.get_value(),
                cwd=self.grc_file_path
            )
            if file_path:  # grc file found. load and get block
                platform.load_and_generate_flow_graph(file_path)
                block = self._get_new_block(key)  # can be None
        return block

    ##############################################
    ## Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this flow graph to nested data.
        Export all block and connection data.

        Returns:
            a nested data odict
        """
        # sort blocks and connections for nicer diffs
        blocks = sorted(self.iter_blocks(), key=lambda b: (
            b.get_key() != 'options',  # options to the front
            not b.get_key().startswith('variable'),  # then vars
            str(b)
        ))
        connections = sorted(self.get_connections(), key=str)
        n = odict()
        n['timestamp'] = self._timestamp
        n['block'] = [b.export_data() for b in blocks]
        n['connection'] = [c.export_data() for c in connections]
        instructions = odict({
            'created': self.get_parent().get_version_short(),
            'format': FLOW_GRAPH_FILE_FORMAT_VERSION,
        })
        return odict({'flow_graph': n, '_instructions': instructions})

    def import_data(self, n=None):
        """
        Import blocks and connections into this flow graph.
        Clear this flowgraph of all previous blocks and connections.
        Any blocks or connections in error will be ignored.

        Args:
            n: the nested data odict
        """
        errors = False
        self._elements = list()  # remove previous elements
        # set file format
        try:
            instructions = n.find('_instructions') or {}
            file_format = int(instructions.get('format', '0')) or self._guess_file_format_1(n)
        except:
            file_format = 0

        fg_n = n and n.find('flow_graph') or odict()  # use blank data if none provided
        self._timestamp = fg_n.find('timestamp') or time.ctime()

        # build the blocks
        self._options_block = self.get_parent().get_new_block(self, 'options')
        for block_n in fg_n.findall('block'):
            key = block_n.find('key')
            block = self._options_block if key == 'options' else self.get_new_block(key)

            if not block:  # looks like this block key cannot be found
                # create a dummy block instead
                block = self.get_new_block('dummy_block')
                # Ugly ugly ugly
                _initialize_dummy_block(block, block_n)
                print('Block key "%s" not found' % key)

            block.import_data(block_n)

        # build the connections
        def verify_and_get_port(key, block, dir):
            ports = block.get_sinks() if dir == 'sink' else block.get_sources()
            for port in ports:
                if key == port.get_key():
                    break
                if not key.isdigit() and port.get_type() == '' and key == port.get_name():
                    break
            else:
                if block.is_dummy_block():
                    port = _dummy_block_add_port(block, key, dir)
                else:
                    raise LookupError('%s key %r not in %s block keys' % (dir, key, dir))
            return port

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
                    source_key, sink_key = self._update_old_message_port_keys(
                        source_key, sink_key, source_block, sink_block)

                # build the connection
                source_port = verify_and_get_port(source_key, source_block, 'source')
                sink_port = verify_and_get_port(sink_key, sink_block, 'sink')
                self.connect(source_port, sink_port)
            except LookupError as e:
                Messages.send_error_load(
                    'Connection between %s(%s) and %s(%s) could not be made.\n\t%s' % (
                        source_block_id, source_key, sink_block_id, sink_key, e))
                errors = True

        self.rewrite()  # global rewrite
        return errors

    @staticmethod
    def _update_old_message_port_keys(source_key, sink_key, source_block, sink_block):
        """Backward compatibility for message port keys

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

    @staticmethod
    def _guess_file_format_1(n):
        """Try to guess the file format for flow-graph files without version tag"""
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

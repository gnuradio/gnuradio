# Copyright 2008-2015 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


import collections
import itertools
import sys
import types
import logging

from operator import methodcaller, attrgetter

from . import Messages, blocks
from .Constants import FLOW_GRAPH_FILE_FORMAT_VERSION
from .base import Element
from .utils import expr_utils
from .utils.backports import shlex

log = logging.getLogger(__name__)


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
        self.options_block = self.parent_platform.make_block(self, 'options')

        self.blocks = [self.options_block]
        self.connections = set()

        self._eval_cache = {}
        self.namespace = {}
        self.imported_names = []

        self.grc_file_path = ''

    def __str__(self):
        return 'FlowGraph - {}({})'.format(self.get_option('title'), self.get_option('id'))

    def imports(self):
        """
        Get a set of all import statements (Python) in this flow graph namespace.

        Returns:
            a list of import statements
        """
        return [block.templates.render('imports') for block in self.iter_enabled_blocks()]

    def get_variables(self):
        """
        Get a list of all variables (Python) in this flow graph namespace.
        Exclude parameterized variables.

        Returns:
            a sorted list of variable blocks in order of dependency (indep -> dep)
        """
        variables = [block for block in self.iter_enabled_blocks()
                     if block.is_variable]
        return expr_utils.sort_objects(variables, attrgetter('name'), methodcaller('get_var_make'))

    def get_parameters(self):
        """
        Get a list of all parameterized variables in this flow graph namespace.

        Returns:
            a list of parameterized variables
        """
        parameters = [b for b in self.iter_enabled_blocks()
                      if b.key == 'parameter']
        return parameters

    def get_snippets(self):
        """
        Get a set of all code snippets (Python) in this flow graph namespace.

        Returns:
            a list of code snippets
        """
        return [b for b in self.iter_enabled_blocks() if b.key == 'snippet']

    def get_snippets_dict(self, section=None):
        """
        Get a dictionary of code snippet information for a particular section.

        Args:
            section: string specifier of section of snippets to return, section=None returns all

        Returns:
            a list of code snippets dicts
        """
        snippets = self.get_snippets()
        if not snippets:
            return []

        output = []
        for snip in snippets:
            d = {}
            sect = snip.params['section'].value
            d['section'] = sect
            d['priority'] = snip.params['priority'].value
            d['lines'] = snip.params['code'].value.splitlines()
            d['def'] = 'def snipfcn_{}(self):'.format(snip.name)
            d['call'] = 'snipfcn_{}(tb)'.format(snip.name)
            if not section or sect == section:
                output.append(d)

        # Sort by descending priority
        if section:
            output = sorted(output, key=lambda x: x['priority'], reverse=True)

        return output

    def get_monitors(self):
        """
        Get a list of all ControlPort monitors
        """
        monitors = [b for b in self.iter_enabled_blocks()
                    if 'ctrlport_monitor' in b.key]
        return monitors

    def get_python_modules(self):
        """Iterate over custom code block ID and Source"""
        for block in self.iter_enabled_blocks():
            if block.key == 'epy_module':
                yield block.name, block.params['source_code'].get_value()

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
        return self.options_block.params[key].get_evaluated()

    def get_run_command(self, file_path, split=False):
        run_command = self.get_option('run_command')
        try:
            run_command = run_command.format(
                python=shlex.quote(sys.executable),
                filename=shlex.quote(file_path))
            return shlex.split(run_command) if split else run_command
        except Exception as e:
            raise ValueError(
                "Can't parse run command {!r}: {}".format(run_command, e))

    def get_imported_names(self):
        """
        Get a lis of imported names.
        These names may not be used as id's

        Returns:
            a list of imported names
        """
        return self.imported_names

    ##############################################
    # Access Elements
    ##############################################
    def get_block(self, name):
        for block in self.blocks:
            if block.name == name:
                return block
        raise KeyError('No block with name {!r}'.format(name))

    def get_elements(self):
        elements = list(self.blocks)
        elements.extend(self.connections)
        return elements

    def children(self):
        return itertools.chain(self.blocks, self.connections)

    def rewrite(self):
        """
        Flag the namespace to be renewed.
        """
        self.renew_namespace()
        Element.rewrite(self)

    def renew_namespace(self):
        namespace = {}
        # Before renewing the namespace, clear it
        # to get rid of entries of blocks that
        # are no longer valid ( deleted, disabled, ...)
        self.namespace.clear()
        # Load imports
        for expr in self.imports():
            try:
                exec(expr, namespace)
            except ImportError:
                # We do not have a good way right now to determine if an import is for a
                # hier block, these imports will fail as they are not in the search path
                # this is ok behavior, unfortunately we could be hiding other import bugs
                pass
            except Exception:
                log.exception('Failed to evaluate import expression "{0}"'.format(
                    expr), exc_info=True)
                pass

        self.imported_names = list(namespace.keys())

        for id, expr in self.get_python_modules():
            try:
                module = types.ModuleType(id)
                exec(expr, module.__dict__)
                namespace[id] = module
            except Exception:
                log.exception(
                    'Failed to evaluate expression in module {0}'.format(id), exc_info=True)
                pass

        # Load parameters
        np = {}  # params don't know each other
        for parameter_block in self.get_parameters():
            try:
                value = eval(
                    parameter_block.params['value'].to_code(), namespace)
                np[parameter_block.name] = value
            except Exception:
                log.exception('Failed to evaluate parameter block {0}'.format(
                    parameter_block.name), exc_info=True)
                pass
        namespace.update(np)  # Merge param namespace

        # We need the updated namespace to evaluate the variable blocks
        # otherwise sometimes variable_block rewrite / eval fails
        self.namespace.update(namespace)
        # Load variables
        for variable_block in self.get_variables():
            try:
                variable_block.rewrite()
                value = eval(variable_block.value, namespace,
                             variable_block.namespace)
                namespace[variable_block.name] = value
                # rewrite on subsequent blocks depends on an updated self.namespace
                self.namespace.update(namespace)
            except TypeError:  # Type Errors may happen, but that doesn't matter as they are displayed in the gui
                pass
            except Exception:
                log.exception('Failed to evaluate variable block {0}'.format(
                    variable_block.name), exc_info=True)
                pass

        self._eval_cache.clear()

    def evaluate(self, expr, namespace=None, local_namespace=None):
        """
        Evaluate the expression.
        """
        # Evaluate
        if not expr:
            raise Exception('Cannot evaluate empty statement.')
        if namespace is not None:
            return eval(expr, namespace, local_namespace)
        else:
            return self._eval_cache.setdefault(expr, eval(expr, self.namespace, local_namespace))

    ##############################################
    # Add/remove stuff
    ##############################################

    def new_block(self, block_id, **kwargs):
        """
        Get a new block of the specified key.
        Add the block to the list of elements.

        Args:
            block_id: the block key

        Returns:
            the new block or None if not found
        """
        if block_id == 'options':
            return self.options_block
        try:
            block = self.parent_platform.make_block(self, block_id, **kwargs)
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
            parent=self, source=porta, sink=portb)
        self.connections.add(connection)

        return connection

    def disconnect(self, *ports):
        to_be_removed = [con for con in self.connections
                         if any(port in con for port in ports)]
        for con in to_be_removed:
            self.remove_element(con)

    def remove_element(self, element):
        """
        Remove the element from the list of elements.
        If the element is a port, remove the whole block.
        If the element is a block, remove its connections.
        If the element is a connection, just remove the connection.
        """
        if element is self.options_block:
            return

        if element.is_port:
            element = element.parent_block  # remove parent block

        if element in self.blocks:
            # Remove block, remove all involved connections
            self.disconnect(*element.ports())
            self.blocks.remove(element)

        elif element in self.connections:
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
        def block_order(b):
            return not b.is_variable, b.name  # todo: vars still first ?!?

        data = collections.OrderedDict()
        data['options'] = self.options_block.export_data()
        data['blocks'] = [b.export_data() for b in sorted(self.blocks, key=block_order)
                          if b is not self.options_block]
        data['connections'] = sorted(c.export_data() for c in self.connections)
        data['metadata'] = {
            'file_format': FLOW_GRAPH_FILE_FORMAT_VERSION,
            'grc_version': self.parent_platform.config.version
        }
        return data

    def _build_depending_hier_block(self, block_id):
        # we're before the initial fg update(), so no evaluated values!
        # --> use raw value instead
        path_param = self.options_block.params['hier_block_src_path']
        file_path = self.parent_platform.find_file_in_paths(
            filename=block_id + '.grc',
            paths=path_param.get_value(),
            cwd=self.grc_file_path
        )
        if file_path:  # grc file found. load and get block
            self.parent_platform.load_and_generate_flow_graph(
                file_path, hier_only=True)
            return self.new_block(block_id)  # can be None

    def import_data(self, data):
        """
        Import blocks and connections into this flow graph.
        Clear this flow graph of all previous blocks and connections.
        Any blocks or connections in error will be ignored.

        Args:
            data: the nested data odict
        """
        # Remove previous elements
        del self.blocks[:]
        self.connections.clear()

        file_format = data['metadata']['file_format']

        # build the blocks
        self.options_block.import_data(name='', **data.get('options', {}))
        self.blocks.append(self.options_block)

        for block_data in data.get('blocks', []):
            block_id = block_data['id']
            block = (
                self.new_block(block_id) or
                self._build_depending_hier_block(block_id) or
                self.new_block(block_id='_dummy',
                               missing_block_id=block_id, **block_data)
            )

            block.import_data(**block_data)

        self.rewrite()

        # build the connections
        def verify_and_get_port(key, block, dir):
            ports = block.sinks if dir == 'sink' else block.sources
            for port in ports:
                if key == port.key or key + '0' == port.key:
                    break
                if not key.isdigit() and port.dtype == '' and key == port.name:
                    break
            else:
                if block.is_dummy_block:
                    port = block.add_missing_port(key, dir)
                else:
                    raise LookupError(
                        '%s key %r not in %s block keys' % (dir, key, dir))
            return port

        had_connect_errors = False
        _blocks = {block.name: block for block in self.blocks}

        try:
            # TODO: Add better error handling if no connections exist in the flowgraph file.
            for src_blk_id, src_port_id, snk_blk_id, snk_port_id in data.get('connections', []):
                source_block = _blocks[src_blk_id]
                sink_block = _blocks[snk_blk_id]

                # fix old, numeric message ports keys
                if file_format < 1:
                    src_port_id, snk_port_id = _update_old_message_port_keys(
                        src_port_id, snk_port_id, source_block, sink_block)

                # build the connection
                source_port = verify_and_get_port(
                    src_port_id, source_block, 'source')
                sink_port = verify_and_get_port(
                    snk_port_id, sink_block, 'sink')

                self.connect(source_port, sink_port)

        except (KeyError, LookupError) as e:
            Messages.send_error_load(
                'Connection between {}({}) and {}({}) could not be made.\n\t{}'.format(
                    src_blk_id, src_port_id, snk_blk_id, snk_port_id, e))
            had_connect_errors = True

        for block in self.blocks:
            if block.is_dummy_block:
                block.rewrite()      # Make ports visible
                # Flowgraph errors depending on disabled blocks are not displayed
                # in the error dialog box
                # So put a message into the Property window of the dummy block
                block.add_error_message(
                    'Block id "{}" not found.'.format(block.key))

        self.rewrite()  # global rewrite
        return had_connect_errors


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
        # get ports using the "old way" (assuming linear indexed keys)
        source_port = source_block.sources[int(source_key)]
        sink_port = sink_block.sinks[int(sink_key)]
        if source_port.dtype == "message" and sink_port.dtype == "message":
            source_key, sink_key = source_port.key, sink_port.key
    except (ValueError, IndexError):
        pass
    return source_key, sink_key  # do nothing

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

from __future__ import absolute_import

import collections
import itertools
import ast

import six
from six.moves import map, range

from Cheetah.Template import Template

from . import utils

from . Constants import (
    BLOCK_FLAG_NEED_QT_GUI, BLOCK_FLAG_NEED_WX_GUI,
    ADVANCED_PARAM_TAB,
    BLOCK_FLAG_THROTTLE, BLOCK_FLAG_DISABLE_BYPASS,
    BLOCK_FLAG_DEPRECATED,
)
from . Element import Element


def _get_elem(iterable, key):
    items = list(iterable)
    for item in items:
        if item.key == key:
            return item
    return ValueError('Key "{}" not found in {}.'.format(key, items))


class Block(Element):

    is_block = True

    STATE_LABELS = ['disabled', 'enabled', 'bypassed']

    def __init__(self, flow_graph, n):
        """
        Make a new block from nested data.

        Args:
            flow_graph: the parent element
            n: the nested odict

        Returns:
            block a new block
        """
        Element.__init__(self, parent=flow_graph)

        self.name = n['name']
        self.key = n['key']
        self.category = [cat.strip() for cat in n.get('category', '').split('/') if cat.strip()]
        self.flags = n.get('flags', '')
        self._doc = n.get('doc', '').strip('\n').replace('\\\n', '')

        # Backwards compatibility
        if n.get('throttle') and BLOCK_FLAG_THROTTLE not in self.flags:
            self.flags += BLOCK_FLAG_THROTTLE

        self._imports = [i.strip() for i in n.get('import', [])]
        self._make = n.get('make')
        self._var_make = n.get('var_make')
        self._var_value = n.get('var_value', '$value')
        self._checks = n.get('check', [])
        self._callbacks = n.get('callback', [])

        self._grc_source = n.get('grc_source', '')
        self.block_wrapper_path = n.get('block_wrapper_path')

        params_n = n.get('param', [])
        sources_n = n.get('source', [])
        sinks_n = n.get('sink', [])

        # Get list of param tabs
        self.params = collections.OrderedDict()
        self._init_params(
            params_n=params_n,
            has_sinks=len(sinks_n),
            has_sources=len(sources_n)
        )

        self.port_counters = [itertools.count(), itertools.count()]
        self.sources = self._init_ports(sources_n, direction='source')
        self.sinks = self._init_ports(sinks_n, direction='sink')
        self.active_sources = []  # on rewrite
        self.active_sinks = []  # on rewrite

        self.states = {'_enabled': True}

        self._epy_source_hash = -1  # for epy blocks
        self._epy_reload_error = None

        self._init_bus_ports(n)

    def _add_param(self, key, name, value='', type='raw', **kwargs):
        n = {'key': key, 'name': name, 'value': value, 'type': type}
        n.update(kwargs)
        self.params[key] = self.parent_platform.Param(block=self, n=n)

    def _init_params(self, params_n, has_sources, has_sinks):
        self._add_param(key='id', name='ID', type='id')

        # Virtual source/sink and pad source/sink blocks are
        # indistinguishable from normal GR blocks. Make explicit
        # checks for them here since they have no work function or
        # buffers to manage.
        self.is_virtual_or_pad = is_virtual_or_pad = self.key in (
            "virtual_source", "virtual_sink", "pad_source", "pad_sink")
        self.is_variable = is_variable = self.key.startswith('variable')
        self.is_import = (self.key == 'import')

        # Disable blocks that are virtual/pads or variables
        if self.is_virtual_or_pad or self.is_variable:
            self.flags += BLOCK_FLAG_DISABLE_BYPASS

        if not (is_virtual_or_pad or is_variable or self.key == 'options'):
            self._add_param(key='alias', name='Block Alias', type='string',
                            hide='part', tab=ADVANCED_PARAM_TAB)

        if not is_virtual_or_pad and (has_sources or has_sinks):
            self._add_param(key='affinity', name='Core Affinity', type='int_vector',
                            hide='part', tab=ADVANCED_PARAM_TAB)

        if not is_virtual_or_pad and has_sources:
            self._add_param(key='minoutbuf', name='Min Output Buffer', type='int',
                            hide='part', value='0', tab=ADVANCED_PARAM_TAB)
            self._add_param(key='maxoutbuf', name='Max Output Buffer', type='int',
                            hide='part', value='0', tab=ADVANCED_PARAM_TAB)

        for param_n in params_n:
            param = self.parent_platform.Param(block=self, n=param_n)
            key = param.key
            if key in self.params:
                raise Exception('Key "{}" already exists in params'.format(key))
            self.params[key] = param

        self._add_param(key='comment', name='Comment', type='_multiline', hide='part',
                        value='', tab=ADVANCED_PARAM_TAB)

    def _init_ports(self, ports_n, direction):
        port_cls = self.parent_platform.Port
        ports = []
        port_keys = set()
        for port_n in ports_n:
            port = port_cls(block=self, n=port_n, dir=direction)
            key = port.key
            if key in port_keys:
                raise Exception('Key "{}" already exists in {}'.format(key, direction))
            port_keys.add(key)
            ports.append(port)
        return ports

    def _run_checks(self):
        """Evaluate the checks"""
        for check in self._checks:
            check_res = self.resolve_dependencies(check)
            try:
                if not self.parent.evaluate(check_res):
                    self.add_error_message('Check "{}" failed.'.format(check))
            except:
                self.add_error_message('Check "{}" did not evaluate.'.format(check))

    def _validate_generate_mode_compat(self):
        """check if this is a GUI block and matches the selected generate option"""
        current_generate_option = self.parent.get_option('generate_options')

        def check_generate_mode(label, flag, valid_options):
            block_requires_mode = (
                flag in self.flags or self.name.upper().startswith(label)
            )
            if block_requires_mode and current_generate_option not in valid_options:
                self.add_error_message("Can't generate this block in mode: {} ".format(
                                       repr(current_generate_option)))

        check_generate_mode('WX GUI', BLOCK_FLAG_NEED_WX_GUI, ('wx_gui',))
        check_generate_mode('QT GUI', BLOCK_FLAG_NEED_QT_GUI, ('qt_gui', 'hb_qt_gui'))

    def _validate_var_value(self):
        """or variables check the value (only if var_value is used)"""
        if self.is_variable and self._var_value != '$value':
            value = self._var_value
            try:
                value = self.get_var_value()
                self.parent.evaluate(value)
            except Exception as err:
                self.add_error_message('Value "{}" cannot be evaluated:\n{}'.format(value, err))

    def validate(self):
        """
        Validate this block.
        Call the base class validate.
        Evaluate the checks: each check must evaluate to True.
        """
        Element.validate(self)
        self._run_checks()
        self._validate_generate_mode_compat()
        self._validate_var_value()
        if self._epy_reload_error:
            self.params['_source_code'].add_error_message(str(self._epy_reload_error))

    def rewrite(self):
        """
        Add and remove ports to adjust for the nports.
        """
        Element.rewrite(self)
        # Check and run any custom rewrite function for this block
        getattr(self, 'rewrite_' + self.key, lambda: None)()

        # Adjust nports, disconnect hidden ports
        for ports in (self.sources, self.sinks):
            for i, master_port in enumerate(ports):
                nports = master_port.get_nports() or 1
                num_ports = 1 + len(master_port.get_clones())
                if master_port.get_hide():
                    for connection in master_port.get_connections():
                        self.parent.remove_element(connection)
                if not nports and num_ports == 1:  # Not a master port and no left-over clones
                    continue
                # Remove excess cloned ports
                for port in master_port.get_clones()[nports-1:]:
                    # Remove excess connections
                    for connection in port.get_connections():
                        self.parent.remove_element(connection)
                    master_port.remove_clone(port)
                    ports.remove(port)
                # Add more cloned ports
                for j in range(num_ports, nports):
                    port = master_port.add_clone()
                    ports.insert(ports.index(master_port) + j, port)

            self.back_ofthe_bus(ports)
            # Renumber non-message/message ports
            domain_specific_port_index = collections.defaultdict(int)
            for port in [p for p in ports if p.key.isdigit()]:
                domain = port.domain
                port.key = str(domain_specific_port_index[domain])
                domain_specific_port_index[domain] += 1

        self.active_sources = [p for p in self.get_sources_gui() if not p.get_hide()]
        self.active_sinks = [p for p in self.get_sinks_gui() if not p.get_hide()]

    def get_imports(self, raw=False):
        """
        Resolve all import statements.
        Split each import statement at newlines.
        Combine all import statements into a list.
        Filter empty imports.

        Returns:
            a list of import statements
        """
        if raw:
            return self._imports
        return [i for i in sum((self.resolve_dependencies(i).split('\n')
                                for i in self._imports), []) if i]

    def get_make(self, raw=False):
        if raw:
            return self._make
        return self.resolve_dependencies(self._make)

    def get_var_make(self):
        return self.resolve_dependencies(self._var_make)

    def get_var_value(self):
        return self.resolve_dependencies(self._var_value)

    def get_callbacks(self):
        """
        Get a list of function callbacks for this block.

        Returns:
            a list of strings
        """
        def make_callback(callback):
            callback = self.resolve_dependencies(callback)
            if 'self.' in callback:
                return callback
            return 'self.{}.{}'.format(self.get_id(), callback)
        return [make_callback(c) for c in self._callbacks]

    def is_virtual_sink(self):
        return self.key == 'virtual_sink'

    def is_virtual_source(self):
        return self.key == 'virtual_source'

    ###########################################################################
    # Custom rewrite functions
    ###########################################################################

    def rewrite_epy_block(self):
        flowgraph = self.parent_flowgraph
        platform = self.parent_platform
        param_blk = self.params['_io_cache']
        param_src = self.params['_source_code']

        src = param_src.get_value()
        src_hash = hash((self.get_id(), src))
        if src_hash == self._epy_source_hash:
            return

        try:
            blk_io = utils.epy_block_io.extract(src)

        except Exception as e:
            self._epy_reload_error = ValueError(str(e))
            try:  # Load last working block io
                blk_io_args = eval(param_blk.get_value())
                if len(blk_io_args) == 6:
                    blk_io_args += ([],)  # add empty callbacks
                blk_io = utils.epy_block_io.BlockIO(*blk_io_args)
            except Exception:
                return
        else:
            self._epy_reload_error = None  # Clear previous errors
            param_blk.set_value(repr(tuple(blk_io)))

        # print "Rewriting embedded python block {!r}".format(self.get_id())

        self._epy_source_hash = src_hash
        self.name = blk_io.name or blk_io.cls
        self._doc = blk_io.doc
        self._imports[0] = 'import ' + self.get_id()
        self._make = '{0}.{1}({2})'.format(self.get_id(), blk_io.cls, ', '.join(
            '{0}=${{ {0} }}'.format(key) for key, _ in blk_io.params))
        self._callbacks = ['{0} = ${{ {0} }}'.format(attr) for attr in blk_io.callbacks]

        params = {}
        for param in list(self.params):
            if hasattr(param, '__epy_param__'):
                params[param.key] = param
                del self.params[param.key]

        for key, value in blk_io.params:
            try:
                param = params[key]
                param.set_default(value)
            except KeyError:  # need to make a new param
                name = key.replace('_', ' ').title()
                n = dict(name=name, key=key, type='raw', value=value)
                param = platform.Param(block=self, n=n)
                setattr(param, '__epy_param__', True)
            self.params[key] = param

        def update_ports(label, ports, port_specs, direction):
            ports_to_remove = list(ports)
            iter_ports = iter(ports)
            ports_new = []
            port_current = next(iter_ports, None)
            for key, port_type in port_specs:
                reuse_port = (
                    port_current is not None and
                    port_current.get_type() == port_type and
                    (key.isdigit() or port_current.key == key)
                )
                if reuse_port:
                    ports_to_remove.remove(port_current)
                    port, port_current = port_current, next(iter_ports, None)
                else:
                    n = dict(name=label + str(key), type=port_type, key=key)
                    if port_type == 'message':
                        n['name'] = key
                        n['optional'] = '1'
                    port = platform.Port(block=self, n=n, dir=direction)
                ports_new.append(port)
            # replace old port list with new one
            del ports[:]
            ports.extend(ports_new)
            # remove excess port connections
            for port in ports_to_remove:
                for connection in port.get_connections():
                    flowgraph.remove_element(connection)

        update_ports('in', self.sinks, blk_io.sinks, 'sink')
        update_ports('out', self.sources, blk_io.sources, 'source')
        self.rewrite()

    @property
    def documentation(self):
        documentation = self.parent_platform.block_docstrings.get(self.key, {})
        from_xml = self._doc.strip()
        if from_xml:
            documentation[''] = from_xml
        return documentation

    # Main functions to get and set the block state
    # Also kept get_enabled and set_enabled to keep compatibility
    @property
    def state(self):
        """
        Gets the block's current state.

        Returns:
            ENABLED - 0
            BYPASSED - 1
            DISABLED - 2
        """
        try:
            return self.STATE_LABELS[int(self.states['_enabled'])]
        except ValueError:
            return 'enabled'

    @state.setter
    def state(self, value):
        """
        Sets the state for the block.

        Args:
            ENABLED - 0
            BYPASSED - 1
            DISABLED - 2
        """
        try:
            encoded = self.STATE_LABELS.index(value)
        except ValueError:
            encoded = 1
        self.states['_enabled'] = encoded

    # Enable/Disable Aliases
    def get_enabled(self):
        """
        Get the enabled state of the block.

        Returns:
            true for enabled
        """
        return self.state != 'disabled'

    def set_enabled(self, enabled):
        """
        Set the enabled state of the block.

        Args:
            enabled: true for enabled

        Returns:
            True if block changed state
        """
        old_state = self.state
        new_state = 'enabled' if enabled else 'disabled'
        self.state = new_state
        return old_state != new_state

    # Block bypassing
    def get_bypassed(self):
        """
        Check if the block is bypassed
        """
        return self.state == 'bypassed'

    def set_bypassed(self):
        """
        Bypass the block

        Returns:
            True if block chagnes state
        """
        if self.state != 'bypassed' and self.can_bypass():
            self.state = 'bypassed'
            return True
        return False

    def can_bypass(self):
        """ Check the number of sinks and sources and see if this block can be bypassed """
        # Check to make sure this is a single path block
        # Could possibly support 1 to many blocks
        if len(self.sources) != 1 or len(self.sinks) != 1:
            return False
        if not (self.sources[0].get_type() == self.sinks[0].get_type()):
            return False
        if BLOCK_FLAG_DISABLE_BYPASS in self.flags:
            return False
        return True

    def __str__(self):
        return 'Block - {} - {}({})'.format(self.get_id(), self.name, self.key)

    def get_id(self):
        return self.params['id'].get_value()

    def get_ports(self):
        return self.sources + self.sinks

    def get_ports_gui(self):
        return self.get_sources_gui() + self.get_sinks_gui()

    def active_ports(self):
        return itertools.chain(self.active_sources, self.active_sinks)

    def get_children(self):
        return self.get_ports() + self.params.values()

    def get_children_gui(self):
        return self.get_ports_gui() + self.params.values()

    def get_comment(self):
        return self.params['comment'].get_value()

    @property
    def is_throtteling(self):
        return BLOCK_FLAG_THROTTLE in self.flags

    @property
    def is_deprecated(self):
        return BLOCK_FLAG_DEPRECATED in self.flags

    ##############################################
    # Access Params
    ##############################################

    def get_param(self, key):
        return self.params[key]

    ##############################################
    # Access Sinks
    ##############################################
    def get_sink(self, key):
        return _get_elem(self.sinks, key)

    def get_sinks_gui(self):
        return self.filter_bus_port(self.sinks)

    ##############################################
    # Access Sources
    ##############################################
    def get_source(self, key):
        return _get_elem(self.sources, key)

    def get_sources_gui(self):
        return self.filter_bus_port(self.sources)

    def get_connections(self):
        return sum([port.get_connections() for port in self.get_ports()], [])

    ##############################################
    # Resolve
    ##############################################

    def resolve_dependencies(self, tmpl):
        """
        Resolve a paramater dependency with cheetah templates.

        Args:
            tmpl: the string with dependencies

        Returns:
            the resolved value
        """
        tmpl = str(tmpl)
        if '$' not in tmpl:
            return tmpl
        # TODO: cache that
        n = {key: param.template_arg for key, param in six.iteritems(self.params)}
        try:
            return str(Template(tmpl, n))
        except Exception as err:
            return "Template error: {}\n    {}".format(tmpl, err)

    ##############################################
    # Controller Modify
    ##############################################
    def type_controller_modify(self, direction):
        """
        Change the type controller.

        Args:
            direction: +1 or -1

        Returns:
            true for change
        """
        type_templates = ' '.join(p._type for p in self.get_children())
        type_param = None
        for key, param in six.iteritems(self.params):
            if not param.is_enum():
                continue
            # Priority to the type controller
            if param.key in type_templates:
                type_param = param
                break
            # Use param if type param is unset
            if not type_param:
                type_param = param
        if not type_param:
            return False

        # Try to increment the enum by direction
        try:
            keys = type_param.get_option_keys()
            old_index = keys.index(type_param.get_value())
            new_index = (old_index + direction + len(keys)) % len(keys)
            type_param.set_value(keys[new_index])
            return True
        except:
            return False

    def port_controller_modify(self, direction):
        """
        Change the port controller.

        Args:
            direction: +1 or -1

        Returns:
            true for change
        """
        changed = False
        # Concat the nports string from the private nports settings of all ports
        nports_str = ' '.join(port._nports for port in self.get_ports())
        # Modify all params whose keys appear in the nports string
        for key, param in six.iteritems(self.params):
            if param.is_enum() or param.key not in nports_str:
                continue
            # Try to increment the port controller by direction
            try:
                value = param.get_evaluated() + direction
                if value > 0:
                    param.set_value(value)
                    changed = True
            except:
                pass
        return changed

    ##############################################
    # Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this block's params to nested data.

        Returns:
            a nested data odict
        """
        n = collections.OrderedDict()
        n['key'] = self.key

        params = (param.export_data() for param in six.itervalues(self.params))
        states = (collections.OrderedDict([('key', key), ('value', repr(value))])
                  for key, value in six.iteritems(self.states))
        n['param'] = sorted(itertools.chain(states, params), key=lambda p: p['key'])

        if any('bus' in a.get_type() for a in self.sinks):
            n['bus_sink'] = '1'
        if any('bus' in a.get_type() for a in self.sources):
            n['bus_source'] = '1'
        return n

    def import_data(self, n):
        """
        Import this block's params from nested data.
        Any param keys that do not exist will be ignored.
        Since params can be dynamically created based another param,
        call rewrite, and repeat the load until the params stick.
        This call to rewrite will also create any dynamic ports
        that are needed for the connections creation phase.

        Args:
            n: the nested data odict
        """
        param_data = {p['key']: p['value'] for p in n.get('param', [])}

        for key in self.states:
            try:
                self.states[key] = ast.literal_eval(param_data.pop(key))
            except (KeyError, SyntaxError, ValueError):
                pass

        def get_hash():
            return hash(tuple(hash(v) for v in self.params.values()))

        pre_rewrite_hash = -1
        while pre_rewrite_hash != get_hash():
            for key, value in six.iteritems(param_data):
                try:
                    self.params[key].set_value(value)
                except KeyError:
                    continue
            # Store hash and call rewrite
            pre_rewrite_hash = get_hash()
            self.rewrite()

        self._import_bus_stuff(n)

    ##############################################
    # Bus ports stuff
    ##############################################

    def get_bus_structure(self, direction):
        bus_structure = self.resolve_dependencies(
            self._bus_structure_source if direction == 'source' else
            self._bus_structure_sink)

        if not bus_structure:
            return ''  # TODO: Don't like empty strings. should change this to None eventually

        try:
            clean_bus_structure = self.parent.evaluate(bus_structure)
            return clean_bus_structure
        except:
            return ''

    @staticmethod
    def back_ofthe_bus(portlist):
        portlist.sort(key=lambda p: p._type == 'bus')

    @staticmethod
    def filter_bus_port(ports):
        buslist = [p for p in ports if p._type == 'bus']
        return buslist or ports

    def _import_bus_stuff(self, n):
        bus_sinks = n.get('bus_sink', [])
        if len(bus_sinks) > 0 and not self._bussify_sink:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'sink')
        elif len(bus_sinks) > 0:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'sink')
            self.bussify({'name': 'bus', 'type': 'bus'}, 'sink')
        bus_sources = n.get('bus_source', [])
        if len(bus_sources) > 0 and not self._bussify_source:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'source')
        elif len(bus_sources) > 0:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'source')
            self.bussify({'name': 'bus', 'type': 'bus'}, 'source')

    def form_bus_structure(self, direc):
        ports = self.sources if direc == 'source' else self.sinks
        struct = self.get_bus_structure(direc)

        if not struct:
            struct = [list(range(len(ports)))]

        elif any(isinstance(p.get_nports(), int) for p in ports):
            last = 0
            structlet = []
            for port in ports:
                nports = port.get_nports()
                if not isinstance(nports, int):
                    continue
                structlet.extend(a + last for a in range(nports))
                last += nports
            struct = [structlet]

        self.current_bus_structure[direc] = struct
        return struct

    def bussify(self, n, direc):
        if direc == 'source':
            get_p_gui = self.get_sources_gui
        else:
            get_p_gui = self.get_sinks_gui

        ports = self.sources if direc == 'source' else self.sinks

        for elt in ports:
            for connect in elt.get_connections():
                self.parent.remove_element(connect)

        if ports and all('bus' != p.get_type() for p in ports):
            struct = self.form_bus_structure(direc)
            self.current_bus_structure[direc] = struct
            if ports[0].get_nports():
                n['nports'] = '1'

            for i in range(len(struct)):
                n['key'] = str(len(ports))
                n = dict(n)
                port = self.parent.parent.Port(block=self, n=n, dir=direc)
                ports.append(port)
        elif any('bus' == p.get_type() for p in ports):
            for elt in get_p_gui():
                ports.remove(elt)
            self.current_bus_structure[direc] = ''

    def _init_bus_ports(self, n):
        self.back_ofthe_bus(self.sources)
        self.back_ofthe_bus(self.sinks)
        self.current_bus_structure = {'source': '', 'sink': ''}
        self._bus_structure_source = n.get('bus_structure_source', '')
        self._bus_structure_sink = n.get('bus_structure_sink', '')
        self._bussify_sink = n.get('bus_sink')
        self._bussify_source = n.get('bus_source')
        if self._bussify_sink:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'sink')
        if self._bussify_source:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'source')

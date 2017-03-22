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
    BLOCK_FLAG_NEED_QT_GUI,
    ADVANCED_PARAM_TAB,
    BLOCK_FLAG_THROTTLE, BLOCK_FLAG_DISABLE_BYPASS,
    BLOCK_FLAG_DEPRECATED,
)
from . Element import Element, lazy_property


def _get_elem(iterable, key):
    items = list(iterable)
    for item in items:
        if item.key == key:
            return item
    return ValueError('Key "{}" not found in {}.'.format(key, items))


class Block(Element):

    is_block = True

    STATE_LABELS = ['disabled', 'enabled', 'bypassed']

    def __init__(self, parent, key, name, **n):
        """Make a new block from nested data."""
        super(Block, self).__init__(parent)

        self.key = key
        self.name = name
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

        # Virtual source/sink and pad source/sink blocks are
        # indistinguishable from normal GR blocks. Make explicit
        # checks for them here since they have no work function or
        # buffers to manage.
        self.is_virtual_or_pad = self.key in (
            "virtual_source", "virtual_sink", "pad_source", "pad_sink")
        self.is_variable = self.key.startswith('variable')
        self.is_import = (self.key == 'import')

        # Disable blocks that are virtual/pads or variables
        if self.is_virtual_or_pad or self.is_variable:
            self.flags += BLOCK_FLAG_DISABLE_BYPASS

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

        self.sources = self._init_ports(sources_n, direction='source')
        self.sinks = self._init_ports(sinks_n, direction='sink')
        self.active_sources = []  # on rewrite
        self.active_sinks = []  # on rewrite

        self.states = {'_enabled': True}

        self._init_bus_ports(n)

    def _init_params(self, params_n, has_sources, has_sinks):
        param_factory = self.parent_platform.get_new_param

        def add_param(key, **kwargs):
            self.params[key] = param_factory(self, key=key, **kwargs)

        add_param(key='id', name='ID', type='id')

        if not (self.is_virtual_or_pad or self.is_variable or self.key == 'options'):
            add_param(key='alias', name='Block Alias', type='string',
                      hide='part', tab=ADVANCED_PARAM_TAB)

        if not self.is_virtual_or_pad and (has_sources or has_sinks):
            add_param(key='affinity', name='Core Affinity', type='int_vector',
                      hide='part', tab=ADVANCED_PARAM_TAB)

        if not self.is_virtual_or_pad and has_sources:
            add_param(key='minoutbuf', name='Min Output Buffer', type='int',
                      hide='part', value='0', tab=ADVANCED_PARAM_TAB)
            add_param(key='maxoutbuf', name='Max Output Buffer', type='int',
                      hide='part', value='0', tab=ADVANCED_PARAM_TAB)

        base_params_n = {}
        for param_n in params_n:
            key = param_n['key']
            if key in self.params:
                raise Exception('Key "{}" already exists in params'.format(key))

            base_key = param_n.get('base_key', None)
            param_n_ext = base_params_n.get(base_key, {}).copy()
            param_n_ext.update(param_n)
            self.params[key] = param_factory(self, **param_n_ext)
            base_params_n[key] = param_n_ext

        add_param(key='comment', name='Comment', type='_multiline', hide='part',
                  value='', tab=ADVANCED_PARAM_TAB)

    def _init_ports(self, ports_n, direction):
        port_factory = self.parent_platform.get_new_port
        ports = []
        port_keys = set()
        stream_port_keys = itertools.count()
        for i, port_n in enumerate(ports_n):
            port_n.setdefault('key', str(next(stream_port_keys)))
            port = port_factory(parent=self, direction=direction, **port_n)
            key = port.key
            if key in port_keys:
                raise Exception('Key "{}" already exists in {}'.format(key, direction))
            port_keys.add(key)
            ports.append(port)
        return ports

    ##############################################
    # validation and rewrite
    ##############################################
    def rewrite(self):
        """
        Add and remove ports to adjust for the nports.
        """
        Element.rewrite(self)

        def rekey(ports):
            """Renumber non-message/message ports"""
            domain_specific_port_index = collections.defaultdict(int)
            for port in [p for p in ports if p.key.isdigit()]:
                domain = port.domain
                port.key = str(domain_specific_port_index[domain])
                domain_specific_port_index[domain] += 1

        # Adjust nports
        for ports in (self.sources, self.sinks):
            self._rewrite_nports(ports)
            self.back_ofthe_bus(ports)
            rekey(ports)

        self._rewrite_bus_ports()

        # disconnect hidden ports
        for port in itertools.chain(self.sources, self.sinks):
            if port.get_hide():
                for connection in port.get_connections():
                    self.parent_flowgraph.remove_element(connection)


        self.active_sources = [p for p in self.get_sources_gui() if not p.get_hide()]
        self.active_sinks = [p for p in self.get_sinks_gui() if not p.get_hide()]

    def _rewrite_nports(self, ports):
        for port in ports:
            if port.is_clone:  # Not a master port and no left-over clones
                continue
            nports = port.get_nports() or 1
            for clone in port.clones[nports-1:]:
                # Remove excess connections
                for connection in clone.get_connections():
                    self.parent_flowgraph.remove_element(connection)
                port.remove_clone(clone)
                ports.remove(clone)
            # Add more cloned ports
            for j in range(1 + len(port.clones), nports):
                clone = port.add_clone()
                ports.insert(ports.index(port) + j, clone)

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

    ##############################################
    # props
    ##############################################

    @lazy_property
    def is_throtteling(self):
        return BLOCK_FLAG_THROTTLE in self.flags

    @lazy_property
    def is_deprecated(self):
        return BLOCK_FLAG_DEPRECATED in self.flags

    @property
    def documentation(self):
        documentation = self.parent_platform.block_docstrings.get(self.key, {})
        from_xml = self._doc.strip()
        if from_xml:
            documentation[''] = from_xml
        return documentation

    @property
    def comment(self):
        return self.params['comment'].get_value()

    @property
    def state(self):
        """Gets the block's current state."""
        try:
            return self.STATE_LABELS[int(self.states['_enabled'])]
        except ValueError:
            return 'enabled'

    @state.setter
    def state(self, value):
        """Sets the state for the block."""
        try:
            encoded = self.STATE_LABELS.index(value)
        except ValueError:
            encoded = 1
        self.states['_enabled'] = encoded

    # Enable/Disable Aliases
    @property
    def enabled(self):
        """Get the enabled state of the block"""
        return self.state != 'disabled'

    ##############################################
    # Getters (old)
    ##############################################

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

    ##############################################
    # Access
    ##############################################

    def get_param(self, key):
        return self.params[key]

    def get_sink(self, key):
        return _get_elem(self.sinks, key)

    def get_sinks_gui(self):
        return self.filter_bus_port(self.sinks)

    def get_source(self, key):
        return _get_elem(self.sources, key)

    def get_sources_gui(self):
        return self.filter_bus_port(self.sources)

    def get_connections(self):
        return sum((port.get_connections() for port in self.get_ports()), [])

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
        bus_structure = self.resolve_dependencies(self._bus_structure[direction])
        if not bus_structure:
            return
        try:
            return self.parent_flowgraph.evaluate(bus_structure)
        except:
            return

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
            self.bussify('sink')
        elif len(bus_sinks) > 0:
            self.bussify('sink')
            self.bussify('sink')
        bus_sources = n.get('bus_source', [])
        if len(bus_sources) > 0 and not self._bussify_source:
            self.bussify('source')
        elif len(bus_sources) > 0:
            self.bussify('source')
            self.bussify('source')

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

    def bussify(self, direc):
        ports = self.sources if direc == 'source' else self.sinks

        for elt in ports:
            for connect in elt.get_connections():
                self.parent.remove_element(connect)

        if ports and all('bus' != p.get_type() for p in ports):
            struct = self.current_bus_structure[direc] = self.form_bus_structure(direc)
            n = {'type': 'bus'}
            if ports[0].get_nports():
                n['nports'] = '1'

            for i, structlet in enumerate(struct):
                name = 'bus{}#{}'.format(i, len(structlet))
                port = self.parent_platform.get_new_port(
                    self, direction=direc, key=str(len(ports)), name=name, **n)
                ports.append(port)
        elif any('bus' == p.get_type() for p in ports):
            get_p_gui = self.get_sources_gui if direc == 'source' else self.get_sinks_gui
            for elt in get_p_gui():
                ports.remove(elt)
            self.current_bus_structure[direc] = ''

    def _init_bus_ports(self, n):
        self.current_bus_structure = {'source': '', 'sink': ''}
        self._bus_structure = {'source': n.get('bus_structure_source', ''),
                               'sink': n.get('bus_structure_sink', '')}
        self._bussify_sink = n.get('bus_sink')
        self._bussify_source = n.get('bus_source')
        if self._bussify_sink:
            self.bussify('sink')
        if self._bussify_source:
            self.bussify('source')

    def _rewrite_bus_ports(self):
        return  # fixme: probably broken

        def doit(ports, ports_gui, direc):
            if not self.current_bus_structure[direc]:
                return

            bus_structure = self.form_bus_structure(direc)
            for port in ports_gui[len(bus_structure):]:
                for connect in port.get_connections():
                    self.parent_flowgraph.remove_element(connect)
                ports.remove(port)

            port_factory = self.parent_platform.get_new_port

            if len(ports_gui) < len(bus_structure):
                for i in range(len(ports_gui), len(bus_structure)):
                    port = port_factory(self, direction=direc, key=str(1 + i),
                                        name='bus', type='bus')
                    ports.append(port)

        doit(self.sources, self.get_sources_gui(), 'source')
        doit(self.sinks, self.get_sinks_gui(), 'sink')

        if 'bus' in [a.get_type() for a in self.get_sources_gui()]:
            for i in range(len(self.get_sources_gui())):
                if not self.get_sources_gui()[i].get_connections():
                    continue
                source = self.get_sources_gui()[i]
                sink = []

                for j in range(len(source.get_connections())):
                    sink.append(source.get_connections()[j].sink_port)
                for elt in source.get_connections():
                    self.parent_flowgraph.remove_element(elt)
                for j in sink:
                    self.parent_flowgraph.connect(source, j)


class EPyBlock(Block):

    def __init__(self, flow_graph, **n):
        super(EPyBlock, self).__init__(flow_graph, **n)
        self._epy_source_hash = -1  # for epy blocks
        self._epy_reload_error = None

    def rewrite(self):
        Element.rewrite(self)

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
        self._update_params(blk_io.params)
        self._update_ports('in', self.sinks, blk_io.sinks, 'sink')
        self._update_ports('out', self.sources, blk_io.sources, 'source')

        super(EPyBlock, self).rewrite()

    def _update_params(self, params_in_src):
        param_factory = self.parent_platform.get_new_param
        params = {}
        for param in list(self.params):
            if hasattr(param, '__epy_param__'):
                params[param.key] = param
                del self.params[param.key]

        for key, value in params_in_src:
            try:
                param = params[key]
                if param.default == param.value:
                    param.set_value(value)
                param.default = str(value)
            except KeyError:  # need to make a new param
                param = param_factory(
                    parent=self,  key=key, type='raw', value=value,
                    name=key.replace('_', ' ').title(),
                )
                setattr(param, '__epy_param__', True)
            self.params[key] = param

    def _update_ports(self, label, ports, port_specs, direction):
        port_factory = self.parent_platform.get_new_port
        ports_to_remove = list(ports)
        iter_ports = iter(ports)
        ports_new = []
        port_current = next(iter_ports, None)
        for key, port_type, vlen in port_specs:
            reuse_port = (
                port_current is not None and
                port_current.get_type() == port_type and
                port_current.get_vlen() == vlen and
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
                if vlen > 1:
                    n['vlen'] = str(vlen)
                port = port_factory(self, direction=direction, **n)
            ports_new.append(port)
        # replace old port list with new one
        del ports[:]
        ports.extend(ports_new)
        # remove excess port connections
        for port in ports_to_remove:
            for connection in port.get_connections():
                self.parent_flowgraph.remove_element(connection)

    def validate(self):
        super(EPyBlock, self).validate()
        if self._epy_reload_error:
            self.params['_source_code'].add_error_message(str(self._epy_reload_error))


class DummyBlock(Block):

    is_dummy_block = True
    build_in_param_keys = 'id alias affinity minoutbuf maxoutbuf comment'

    def __init__(self, parent, key, missing_key, params_n):
        super(DummyBlock, self).__init__(parent=parent, key=missing_key, name='Missing Block')
        param_factory = self.parent_platform.get_new_param
        for param_n in params_n:
            key = param_n['key']
            self.params.setdefault(key, param_factory(self, key=key, name=key, type='string'))

    def is_valid(self):
        return False

    @property
    def enabled(self):
        return False

    def add_missing_port(self, key, dir):
        port = self.parent_platform.get_new_port(
            parent=self, direction=dir, key=key, name='?', type='',
        )
        if port.is_source:
            self.sources.append(port)
        else:
            self.sinks.append(port)
        return port

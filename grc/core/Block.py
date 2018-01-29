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

import collections
import itertools

from Cheetah.Template import Template

from .utils import epy_block_io, odict
from . Constants import (
    BLOCK_FLAG_NEED_QT_GUI, BLOCK_FLAG_NEED_WX_GUI,
    ADVANCED_PARAM_TAB, DEFAULT_PARAM_TAB,
    BLOCK_FLAG_THROTTLE, BLOCK_FLAG_DISABLE_BYPASS,
    BLOCK_FLAG_DEPRECATED,
    BLOCK_ENABLED, BLOCK_BYPASSED, BLOCK_DISABLED
)
from . Element import Element


def _get_keys(lst):
    return [elem.get_key() for elem in lst]


def _get_elem(lst, key):
    try:
        return lst[_get_keys(lst).index(key)]
    except ValueError:
        raise ValueError('Key "{0}" not found in {1}.'.format(key, _get_keys(lst)))


class Block(Element):

    is_block = True

    def __init__(self, flow_graph, n):
        """
        Make a new block from nested data.

        Args:
            flow: graph the parent element
            n: the nested odict

        Returns:
            block a new block
        """
        # Grab the data
        self._doc = (n.find('doc') or '').strip('\n').replace('\\\n', '')
        self._imports = map(lambda i: i.strip(), n.findall('import'))
        self._make = n.find('make')
        self._var_make = n.find('var_make')
        self._checks = n.findall('check')
        self._callbacks = n.findall('callback')
        self._bus_structure_source = n.find('bus_structure_source') or ''
        self._bus_structure_sink = n.find('bus_structure_sink') or ''
        self.port_counters = [itertools.count(), itertools.count()]

        # Build the block
        Element.__init__(self, flow_graph)

        # Grab the data
        params = n.findall('param')
        sources = n.findall('source')
        sinks = n.findall('sink')
        self._name = n.find('name')
        self._key = n.find('key')
        category = (n.find('category') or '').split('/')
        self.category = [cat.strip() for cat in category if cat.strip()]
        self._flags = n.find('flags') or ''
        # Backwards compatibility
        if n.find('throttle') and BLOCK_FLAG_THROTTLE not in self._flags:
            self._flags += BLOCK_FLAG_THROTTLE
        self._grc_source = n.find('grc_source') or ''
        self._block_wrapper_path = n.find('block_wrapper_path')
        self._bussify_sink = n.find('bus_sink')
        self._bussify_source = n.find('bus_source')
        self._var_value = n.find('var_value') or '$value'

        # Get list of param tabs
        n_tabs = n.find('param_tab_order') or None
        self._param_tab_labels = n_tabs.findall('tab') if n_tabs is not None else [DEFAULT_PARAM_TAB]

        # Create the param objects
        self._params = list()

        # Add the id param
        self.get_params().append(self.get_parent().get_parent().Param(
            block=self,
            n=odict({
                'name': 'ID',
                'key': 'id',
                'type': 'id',
            })
        ))
        self.get_params().append(self.get_parent().get_parent().Param(
            block=self,
            n=odict({
                'name': 'Enabled',
                'key': '_enabled',
                'type': 'raw',
                'value': 'True',
                'hide': 'all',
            })
        ))
        for param in itertools.imap(lambda n: self.get_parent().get_parent().Param(block=self, n=n), params):
            key = param.get_key()
            # Test against repeated keys
            if key in self.get_param_keys():
                raise Exception('Key "{0}" already exists in params'.format(key))
            # Store the param
            self.get_params().append(param)
        # Create the source objects
        self._sources = list()
        for source in map(lambda n: self.get_parent().get_parent().Port(block=self, n=n, dir='source'), sources):
            key = source.get_key()
            # Test against repeated keys
            if key in self.get_source_keys():
                raise Exception('Key "{0}" already exists in sources'.format(key))
            # Store the port
            self.get_sources().append(source)
        self.back_ofthe_bus(self.get_sources())
        # Create the sink objects
        self._sinks = list()
        for sink in map(lambda n: self.get_parent().get_parent().Port(block=self, n=n, dir='sink'), sinks):
            key = sink.get_key()
            # Test against repeated keys
            if key in self.get_sink_keys():
                raise Exception('Key "{0}" already exists in sinks'.format(key))
            # Store the port
            self.get_sinks().append(sink)
        self.back_ofthe_bus(self.get_sinks())
        self.current_bus_structure = {'source': '', 'sink': ''}

        # Virtual source/sink and pad source/sink blocks are
        # indistinguishable from normal GR blocks. Make explicit
        # checks for them here since they have no work function or
        # buffers to manage.
        self.is_virtual_or_pad = self._key in (
            "virtual_source", "virtual_sink", "pad_source", "pad_sink")
        self.is_variable = self._key.startswith('variable')
        self.is_import = (self._key == 'import')

        # Disable blocks that are virtual/pads or variables
        if self.is_virtual_or_pad or self.is_variable:
            self._flags += BLOCK_FLAG_DISABLE_BYPASS

        if not (self.is_virtual_or_pad or self.is_variable or self._key == 'options'):
            self.get_params().append(self.get_parent().get_parent().Param(
                block=self,
                n=odict({'name': 'Block Alias',
                         'key': 'alias',
                         'type': 'string',
                         'hide': 'part',
                         'tab': ADVANCED_PARAM_TAB
                         })
            ))

        if (len(sources) or len(sinks)) and not self.is_virtual_or_pad:
            self.get_params().append(self.get_parent().get_parent().Param(
                    block=self,
                    n=odict({'name': 'Core Affinity',
                             'key': 'affinity',
                             'type': 'int_vector',
                             'hide': 'part',
                             'tab': ADVANCED_PARAM_TAB
                             })
                    ))
        if len(sources) and not self.is_virtual_or_pad:
            self.get_params().append(self.get_parent().get_parent().Param(
                    block=self,
                    n=odict({'name': 'Min Output Buffer',
                             'key': 'minoutbuf',
                             'type': 'int',
                             'hide': 'part',
                             'value': '0',
                             'tab': ADVANCED_PARAM_TAB
                             })
                    ))
            self.get_params().append(self.get_parent().get_parent().Param(
                    block=self,
                    n=odict({'name': 'Max Output Buffer',
                             'key': 'maxoutbuf',
                             'type': 'int',
                             'hide': 'part',
                             'value': '0',
                             'tab': ADVANCED_PARAM_TAB
                             })
                    ))

        self.get_params().append(self.get_parent().get_parent().Param(
                block=self,
                n=odict({'name': 'Comment',
                         'key': 'comment',
                         'type': '_multiline',
                         'hide': 'part',
                         'value': '',
                         'tab': ADVANCED_PARAM_TAB
                         })
                ))

        self._epy_source_hash = -1  # for epy blocks
        self._epy_reload_error = None

        if self._bussify_sink:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'sink')
        if self._bussify_source:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'source')

    def get_bus_structure(self, direction):
        if direction == 'source':
            bus_structure = self._bus_structure_source
        else:
            bus_structure = self._bus_structure_sink

        bus_structure = self.resolve_dependencies(bus_structure)

        if not bus_structure:
            return ''  # TODO: Don't like empty strings. should change this to None eventually

        try:
            clean_bus_structure = self.get_parent().evaluate(bus_structure)
            return clean_bus_structure
        except:
            return ''

    def validate(self):
        """
        Validate this block.
        Call the base class validate.
        Evaluate the checks: each check must evaluate to True.
        """
        Element.validate(self)
        # Evaluate the checks
        for check in self._checks:
            check_res = self.resolve_dependencies(check)
            try:
                if not self.get_parent().evaluate(check_res):
                    self.add_error_message('Check "{0}" failed.'.format(check))
            except:
                self.add_error_message('Check "{0}" did not evaluate.'.format(check))

        # For variables check the value (only if var_value is used
        if self.is_variable and self._var_value != '$value':
            value = self._var_value
            try:
                value = self.get_var_value()
                self.get_parent().evaluate(value)
            except Exception as err:
                self.add_error_message('Value "{0}" cannot be evaluated:\n{1}'.format(value, err))

        # check if this is a GUI block and matches the selected generate option
        current_generate_option = self.get_parent().get_option('generate_options')

        def check_generate_mode(label, flag, valid_options):
            block_requires_mode = (
                flag in self.get_flags() or
                self.get_name().upper().startswith(label)
            )
            if block_requires_mode and current_generate_option not in valid_options:
                self.add_error_message("Can't generate this block in mode: {0} ".format(
                                       repr(current_generate_option)))

        check_generate_mode('WX GUI', BLOCK_FLAG_NEED_WX_GUI, ('wx_gui',))
        check_generate_mode('QT GUI', BLOCK_FLAG_NEED_QT_GUI, ('qt_gui', 'hb_qt_gui'))
        if self._epy_reload_error:
            self.get_param('_source_code').add_error_message(str(self._epy_reload_error))

    def rewrite(self):
        """
        Add and remove ports to adjust for the nports.
        """
        Element.rewrite(self)
        # Check and run any custom rewrite function for this block
        getattr(self, 'rewrite_' + self._key, lambda: None)()

        # Adjust nports, disconnect hidden ports
        for ports in (self.get_sources(), self.get_sinks()):
            for i, master_port in enumerate(ports):
                nports = master_port.get_nports() or 1
                num_ports = 1 + len(master_port.get_clones())
                if master_port.get_hide():
                    for connection in master_port.get_connections():
                        self.get_parent().remove_element(connection)
                if not nports and num_ports == 1:  # Not a master port and no left-over clones
                    continue
                # Remove excess cloned ports
                for port in master_port.get_clones()[nports-1:]:
                    # Remove excess connections
                    for connection in port.get_connections():
                        self.get_parent().remove_element(connection)
                    master_port.remove_clone(port)
                    ports.remove(port)
                # Add more cloned ports
                for j in range(num_ports, nports):
                    port = master_port.add_clone()
                    ports.insert(ports.index(master_port) + j, port)

            self.back_ofthe_bus(ports)
            # Renumber non-message/message ports
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
        # Concat the nports string from the private nports settings of all ports
        nports_str = ' '.join([port._nports for port in self.get_ports()])
        # Modify all params whose keys appear in the nports string
        for param in self.get_params():
            if param.is_enum() or param.get_key() not in nports_str:
                continue
            # Try to increment the port controller by direction
            try:
                value = param.get_evaluated()
                value = value + direction
                if 0 < value:
                    param.set_value(value)
                    changed = True
            except:
                pass
        return changed

    def get_doc(self):
        platform = self.get_parent().get_parent()
        documentation = platform.block_docstrings.get(self._key, {})
        from_xml = self._doc.strip()
        if from_xml:
            documentation[''] = from_xml
        return documentation

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
        return filter(lambda i: i, sum(map(lambda i: self.resolve_dependencies(i).split('\n'), self._imports), []))

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
            return 'self.{0}.{1}'.format(self.get_id(), callback)
        return map(make_callback, self._callbacks)

    def is_virtual_sink(self):
        return self.get_key() == 'virtual_sink'

    def is_virtual_source(self):
        return self.get_key() == 'virtual_source'

    ###########################################################################
    # Custom rewrite functions
    ###########################################################################

    def rewrite_epy_block(self):
        flowgraph = self.get_parent()
        platform = flowgraph.get_parent()
        param_blk = self.get_param('_io_cache')
        param_src = self.get_param('_source_code')

        src = param_src.get_value()
        src_hash = hash((self.get_id(), src))
        if src_hash == self._epy_source_hash:
            return

        try:
            blk_io = epy_block_io.extract(src)

        except Exception as e:
            self._epy_reload_error = ValueError(str(e))
            try:  # Load last working block io
                blk_io_args = eval(param_blk.get_value())
                if len(blk_io_args) == 6:
                    blk_io_args += ([],)  # add empty callbacks
                blk_io = epy_block_io.BlockIO(*blk_io_args)
            except Exception:
                return
        else:
            self._epy_reload_error = None  # Clear previous errors
            param_blk.set_value(repr(tuple(blk_io)))

        # print "Rewriting embedded python block {!r}".format(self.get_id())

        self._epy_source_hash = src_hash
        self._name = blk_io.name or blk_io.cls
        self._doc = blk_io.doc
        self._imports[0] = 'import ' + self.get_id()
        self._make = '{0}.{1}({2})'.format(self.get_id(), blk_io.cls, ', '.join(
            '{0}=${{ {0} }}'.format(key) for key, _ in blk_io.params))
        self._callbacks = ['{0} = ${{ {0} }}'.format(attr) for attr in blk_io.callbacks]

        params = {}
        for param in list(self._params):
            if hasattr(param, '__epy_param__'):
                params[param.get_key()] = param
                self._params.remove(param)

        for key, value in blk_io.params:
            try:
                param = params[key]
                param.set_default(value)
            except KeyError:  # need to make a new param
                name = key.replace('_', ' ').title()
                n = odict(dict(name=name, key=key, type='raw', value=value))
                param = platform.Param(block=self, n=n)
                setattr(param, '__epy_param__', True)
            self._params.append(param)

        def update_ports(label, ports, port_specs, direction):
            ports_to_remove = list(ports)
            iter_ports = iter(ports)
            ports_new = []
            port_current = next(iter_ports, None)
            for key, port_type, vlen in port_specs:
                reuse_port = (
                    port_current is not None and
                    port_current.get_type() == port_type and
                    port_current.get_vlen() == vlen and
                    (key.isdigit() or port_current.get_key() == key)
                )
                if reuse_port:
                    ports_to_remove.remove(port_current)
                    port, port_current = port_current, next(iter_ports, None)
                else:
                    n = odict(dict(name=label + str(key), type=port_type, key=key))
                    if port_type == 'message':
                        n['name'] = key
                        n['optional'] = '1'
                    if vlen > 1:
                        n['vlen'] = str(vlen)
                    port = platform.Port(block=self, n=n, dir=direction)
                ports_new.append(port)
            # replace old port list with new one
            del ports[:]
            ports.extend(ports_new)
            # remove excess port connections
            for port in ports_to_remove:
                for connection in port.get_connections():
                    flowgraph.remove_element(connection)

        update_ports('in', self.get_sinks(), blk_io.sinks, 'sink')
        update_ports('out', self.get_sources(), blk_io.sources, 'source')
        self.rewrite()

    def back_ofthe_bus(self, portlist):
        portlist.sort(key=lambda p: p._type == 'bus')

    def filter_bus_port(self, ports):
        buslist = [p for p in ports if p._type == 'bus']
        return buslist or ports

    # Main functions to get and set the block state
    # Also kept get_enabled and set_enabled to keep compatibility
    def get_state(self):
        """
        Gets the block's current state.

        Returns:
            ENABLED - 0
            BYPASSED - 1
            DISABLED - 2
        """
        try:
            return int(eval(self.get_param('_enabled').get_value()))
        except:
            return BLOCK_ENABLED

    def set_state(self, state):
        """
        Sets the state for the block.

        Args:
            ENABLED - 0
            BYPASSED - 1
            DISABLED - 2
        """
        if state in [BLOCK_ENABLED, BLOCK_BYPASSED, BLOCK_DISABLED]:
            self.get_param('_enabled').set_value(str(state))
        else:
            self.get_param('_enabled').set_value(str(BLOCK_ENABLED))

    # Enable/Disable Aliases
    def get_enabled(self):
        """
        Get the enabled state of the block.

        Returns:
            true for enabled
        """
        return not (self.get_state() == BLOCK_DISABLED)

    def set_enabled(self, enabled):
        """
        Set the enabled state of the block.

        Args:
            enabled: true for enabled

        Returns:
            True if block changed state
        """
        old_state = self.get_state()
        new_state = BLOCK_ENABLED if enabled else BLOCK_DISABLED
        self.set_state(new_state)
        return old_state != new_state

    # Block bypassing
    def get_bypassed(self):
        """
        Check if the block is bypassed
        """
        return self.get_state() == BLOCK_BYPASSED

    def set_bypassed(self):
        """
        Bypass the block

        Returns:
            True if block chagnes state
        """
        if self.get_state() != BLOCK_BYPASSED and self.can_bypass():
            self.set_state(BLOCK_BYPASSED)
            return True
        return False

    def can_bypass(self):
        """ Check the number of sinks and sources and see if this block can be bypassed """
        # Check to make sure this is a single path block
        # Could possibly support 1 to many blocks
        if len(self.get_sources()) != 1 or len(self.get_sinks()) != 1:
            return False
        if not (self.get_sources()[0].get_type() == self.get_sinks()[0].get_type()):
            return False
        if self.bypass_disabled():
            return False
        return True

    def __str__(self):
        return 'Block - {0} - {1}({2})'.format(self.get_id(), self.get_name(), self.get_key())

    def get_id(self):
        return self.get_param('id').get_value()

    def get_name(self):
        return self._name

    def get_key(self):
        return self._key

    def get_ports(self):
        return self.get_sources() + self.get_sinks()

    def get_ports_gui(self):
        return self.filter_bus_port(self.get_sources()) + self.filter_bus_port(self.get_sinks())

    def get_children(self):
        return self.get_ports() + self.get_params()

    def get_children_gui(self):
        return self.get_ports_gui() + self.get_params()

    def get_block_wrapper_path(self):
        return self._block_wrapper_path

    def get_comment(self):
        return self.get_param('comment').get_value()

    def get_flags(self):
        return self._flags

    def throtteling(self):
        return BLOCK_FLAG_THROTTLE in self._flags

    def bypass_disabled(self):
        return BLOCK_FLAG_DISABLE_BYPASS in self._flags

    @property
    def is_deprecated(self):
        return BLOCK_FLAG_DEPRECATED in self._flags

    ##############################################
    # Access Params
    ##############################################
    def get_param_tab_labels(self):
        return self._param_tab_labels

    def get_param_keys(self):
        return _get_keys(self._params)

    def get_param(self, key):
        return _get_elem(self._params, key)

    def get_params(self):
        return self._params

    def has_param(self, key):
        try:
            _get_elem(self._params, key)
            return True
        except:
            return False

    ##############################################
    # Access Sinks
    ##############################################
    def get_sink_keys(self):
        return _get_keys(self._sinks)

    def get_sink(self, key):
        return _get_elem(self._sinks, key)

    def get_sinks(self):
        return self._sinks

    def get_sinks_gui(self):
        return self.filter_bus_port(self.get_sinks())

    ##############################################
    # Access Sources
    ##############################################
    def get_source_keys(self):
        return _get_keys(self._sources)

    def get_source(self, key):
        return _get_elem(self._sources, key)

    def get_sources(self):
        return self._sources

    def get_sources_gui(self):
        return self.filter_bus_port(self.get_sources())

    def get_connections(self):
        return sum([port.get_connections() for port in self.get_ports()], [])

    def resolve_dependencies(self, tmpl):
        """
        Resolve a parameter dependency with cheetah templates.

        Args:
            tmpl: the string with dependencies

        Returns:
            the resolved value
        """
        tmpl = str(tmpl)
        if '$' not in tmpl:
            return tmpl
        n = dict((param.get_key(), param.template_arg)
                 for param in self.get_params())  # TODO: cache that
        try:
            return str(Template(tmpl, n))
        except Exception as err:
            return "Template error: {0}\n    {1}".format(tmpl, err)

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
        changed = False
        type_param = None
        for param in filter(lambda p: p.is_enum(), self.get_params()):
            children = self.get_ports() + self.get_params()
            # Priority to the type controller
            if param.get_key() in ' '.join(map(lambda p: p._type, children)): type_param = param
            # Use param if type param is unset
            if not type_param:
                type_param = param
        if type_param:
            # Try to increment the enum by direction
            try:
                keys = type_param.get_option_keys()
                old_index = keys.index(type_param.get_value())
                new_index = (old_index + direction + len(keys)) % len(keys)
                type_param.set_value(keys[new_index])
                changed = True
            except:
                pass
        return changed

    def form_bus_structure(self, direc):
        if direc == 'source':
            get_p = self.get_sources
            get_p_gui = self.get_sources_gui
            bus_structure = self.get_bus_structure('source')
        else:
            get_p = self.get_sinks
            get_p_gui = self.get_sinks_gui
            bus_structure = self.get_bus_structure('sink')

        struct = [range(len(get_p()))]
        if True in map(lambda a: isinstance(a.get_nports(), int), get_p()):
            structlet = []
            last = 0
            for j in [i.get_nports() for i in get_p() if isinstance(i.get_nports(), int)]:
                structlet.extend(map(lambda a: a+last, range(j)))
                last = structlet[-1] + 1
                struct = [structlet]
        if bus_structure:

            struct = bus_structure

        self.current_bus_structure[direc] = struct
        return struct

    def bussify(self, n, direc):
        if direc == 'source':
            get_p = self.get_sources
            get_p_gui = self.get_sources_gui
            bus_structure = self.get_bus_structure('source')
        else:
            get_p = self.get_sinks
            get_p_gui = self.get_sinks_gui
            bus_structure = self.get_bus_structure('sink')

        for elt in get_p():
            for connect in elt.get_connections():
                self.get_parent().remove_element(connect)

        if ('bus' not in map(lambda a: a.get_type(), get_p())) and len(get_p()) > 0:
            struct = self.form_bus_structure(direc)
            self.current_bus_structure[direc] = struct
            if get_p()[0].get_nports():
                n['nports'] = str(1)

            for i in range(len(struct)):
                n['key'] = str(len(get_p()))
                n = odict(n)
                port = self.get_parent().get_parent().Port(block=self, n=n, dir=direc)
                get_p().append(port)
        elif 'bus' in map(lambda a: a.get_type(), get_p()):
            for elt in get_p_gui():
                get_p().remove(elt)
            self.current_bus_structure[direc] = ''

    ##############################################
    # Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this block's params to nested data.

        Returns:
            a nested data odict
        """
        n = odict()
        n['key'] = self.get_key()
        n['param'] = map(lambda p: p.export_data(), sorted(self.get_params(), key=str))
        if 'bus' in map(lambda a: a.get_type(), self.get_sinks()):
            n['bus_sink'] = str(1)
        if 'bus' in map(lambda a: a.get_type(), self.get_sources()):
            n['bus_source'] = str(1)
        return n

    def get_hash(self):
        return hash(tuple(map(hash, self.get_params())))

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
        my_hash = 0
        while self.get_hash() != my_hash:
            params_n = n.findall('param')
            for param_n in params_n:
                key = param_n.find('key')
                value = param_n.find('value')
                # The key must exist in this block's params
                if key in self.get_param_keys():
                    self.get_param(key).set_value(value)
            # Store hash and call rewrite
            my_hash = self.get_hash()
            self.rewrite()
        bussinks = n.findall('bus_sink')
        if len(bussinks) > 0 and not self._bussify_sink:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'sink')
        elif len(bussinks) > 0:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'sink')
            self.bussify({'name': 'bus', 'type': 'bus'}, 'sink')
        bussrcs = n.findall('bus_source')
        if len(bussrcs) > 0 and not self._bussify_source:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'source')
        elif len(bussrcs) > 0:
            self.bussify({'name': 'bus', 'type': 'bus'}, 'source')
            self.bussify({'name': 'bus', 'type': 'bus'}, 'source')

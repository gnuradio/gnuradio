"""
Copyright 2008-2020 Free Software Foundation, Inc.
Copyright 2021 GNU Radio contributors
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


import collections
import itertools
import copy

import re

import ast

from ._templates import MakoTemplates
from ._flags import Flags

from ..base import Element
from ..utils.descriptors import lazy_property


def _get_elem(iterable, key):
    items = list(iterable)
    for item in items:
        if item.key == key:
            return item
    return ValueError('Key "{}" not found in {}.'.format(key, items))


class Block(Element):

    is_block = True

    STATE_LABELS = ['disabled', 'enabled', 'bypassed']

    key = ''
    label = ''
    category = []
    vtype = ''  # This is only used for variables when we want C++ output
    flags = Flags('')
    documentation = {'': ''}

    value = None
    asserts = []

    templates = MakoTemplates()
    parameters_data = []
    inputs_data = []
    outputs_data = []

    extra_data = {}
    loaded_from = '(unknown)'

    def __init__(self, parent):
        """Make a new block from nested data."""
        super(Block, self).__init__(parent)
        param_factory = self.parent_platform.make_param
        port_factory = self.parent_platform.make_port

        self.params = collections.OrderedDict(
            (data['id'], param_factory(parent=self, **data))
            for data in self.parameters_data
        )
        if self.key == 'options':
            self.params['id'].hide = 'part'

        self.sinks = [port_factory(parent=self, **params)
                      for params in self.inputs_data]
        self.sources = [port_factory(parent=self, **params)
                        for params in self.outputs_data]

        self.active_sources = []  # on rewrite
        self.active_sinks = []  # on rewrite

        self.states = {'state': True, 'bus_source': False,
                       'bus_sink': False, 'bus_structure': None}
        self.block_namespace = {}
        self.deprecated = self.is_deprecated()

        if Flags.HAS_CPP in self.flags and self.enabled and not (self.is_virtual_source() or self.is_virtual_sink()):
            # This is a workaround to allow embedded python blocks/modules to load as there is
            # currently 'cpp' in the flags by default caused by the other built-in blocks
            if hasattr(self, 'cpp_templates'):
                # The original template, in case we have to edit it when transpiling to C++
                self.orig_cpp_templates = self.cpp_templates

        self.current_bus_structure = {'source': None, 'sink': None}

    def get_bus_structure(self, direction):
        if direction == 'source':
            bus_structure = self.bus_structure_source
        else:
            bus_structure = self.bus_structure_sink

        if not bus_structure:
            return None

        try:
            clean_bus_structure = self.evaluate(bus_structure)
            return clean_bus_structure
        except:
            return None

    # region Rewrite_and_Validation

    def rewrite(self):
        """
        Add and remove ports to adjust for the nports.
        """
        Element.rewrite(self)

        def rekey(ports):
            """Renumber non-message/message ports"""
            domain_specific_port_index = collections.defaultdict(int)
            for port in ports:
                if not port.key.isdigit():
                    continue
                domain = port.domain
                port.key = str(domain_specific_port_index[domain])
                domain_specific_port_index[domain] += 1

        # Adjust nports
        for ports in (self.sources, self.sinks):
            self._rewrite_nports(ports)
            rekey(ports)

        self.update_bus_logic()
        # disconnect hidden ports
        self.parent_flowgraph.disconnect(
            *[p for p in self.ports() if p.hidden])

        self.active_sources = [p for p in self.sources if not p.hidden]
        self.active_sinks = [p for p in self.sinks if not p.hidden]

        # namespaces may have changed, update them
        self.block_namespace.clear()
        imports = ""
        try:
            imports = self.templates.render('imports')
            exec(imports, self.block_namespace)
        except ImportError:
            # We do not have a good way right now to determine if an import is for a
            # hier block, these imports will fail as they are not in the search path
            # this is ok behavior, unfortunately we could be hiding other import bugs
            pass
        except Exception:
            self.add_error_message(
                f'Failed to evaluate import expression {imports!r}')

    def update_bus_logic(self):
        ###############################
        # Bus Logic
        ###############################

        for direc in {'source', 'sink'}:
            if direc == 'source':
                ports = self.sources
                ports_gui = self.filter_bus_port(self.sources)
                bus_state = self.bus_source
            else:
                ports = self.sinks
                ports_gui = self.filter_bus_port(self.sinks)
                bus_state = self.bus_sink

            # Remove the bus ports
            removed_bus_ports = []
            removed_bus_connections = []
            if 'bus' in map(lambda a: a.dtype, ports):
                for port in ports_gui:
                    for c in self.parent_flowgraph.connections:
                        if port is c.source_port or port is c.sink_port:
                            removed_bus_ports.append(port)
                            removed_bus_connections.append(c)
                    ports.remove(port)

            if (bus_state):
                struct = self.form_bus_structure(direc)
                self.current_bus_structure[direc] = struct

                # Hide ports that are not part of the bus structure
                # TODO: Blocks where it is desired to only have a subset
                # of ports included in the bus still has some issues
                for idx, port in enumerate(ports):
                    if any([idx in bus for bus in self.current_bus_structure[direc]]):
                        if (port.stored_hidden_state is None):
                            port.stored_hidden_state = port.hidden
                            port.hidden = True

                # Add the Bus Ports to the list of ports
                for i in range(len(struct)):
                    # self.sinks = [port_factory(parent=self, **params) for params in self.inputs_data]
                    port = self.parent.parent.make_port(self, direction=direc, id=str(
                        len(ports)), label='bus', dtype='bus', bus_struct=struct[i])
                    ports.append(port)

                    for (saved_port, connection) in zip(removed_bus_ports, removed_bus_connections):
                        if port.key == saved_port.key:
                            self.parent_flowgraph.connections.remove(
                                connection)
                            if saved_port.is_source:
                                connection.source_port = port
                            if saved_port.is_sink:
                                connection.sink_port = port
                            self.parent_flowgraph.connections.add(connection)

            else:
                self.current_bus_structure[direc] = None

                # Re-enable the hidden property of the ports
                for port in ports:
                    if (port.stored_hidden_state is not None):
                        port.hidden = port.stored_hidden_state
                        port.stored_hidden_state = None

    def _rewrite_nports(self, ports):
        for port in ports:
            if hasattr(port, 'master_port'):  # Not a master port and no left-over clones
                port.dtype = port.master_port.dtype
                port.vlen = port.master_port.vlen
                continue
            nports = port.multiplicity
            for clone in port.clones[nports - 1:]:
                # Remove excess connections
                self.parent_flowgraph.disconnect(clone)
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
        self._run_asserts()
        self._validate_generate_mode_compat()
        self._validate_output_language_compat()
        self._validate_var_value()

    def _run_asserts(self):
        """Evaluate the checks"""
        for expr in self.asserts:
            try:
                if not self.evaluate(expr):
                    self.add_error_message(
                        'Assertion "{}" failed.'.format(expr))
            except Exception:
                self.add_error_message(
                    'Assertion "{}" did not evaluate.'.format(expr))

    def _validate_generate_mode_compat(self):
        """check if this is a GUI block and matches the selected generate option"""
        current_generate_option = self.parent.get_option('generate_options')

        def check_generate_mode(label, flag, valid_options):
            block_requires_mode = (
                flag in self.flags or self.label.upper().startswith(label)
            )
            if block_requires_mode and current_generate_option not in valid_options:
                self.add_error_message("Can't generate this block in mode: {} ".format(
                    repr(current_generate_option)))

        check_generate_mode('QT GUI', Flags.NEED_QT_GUI,
                            ('qt_gui', 'hb_qt_gui'))

    def _validate_output_language_compat(self):
        """check if this block supports the selected output language"""
        current_output_language = self.parent.get_option('output_language')

        if current_output_language == 'cpp':
            if 'cpp' not in self.flags:
                self.add_error_message(
                    "This block does not support C++ output.")

            if self.key == 'parameter':
                if not self.params['type'].value:
                    self.add_error_message(
                        "C++ output requires you to choose a parameter type.")

    def _validate_var_value(self):
        """or variables check the value (only if var_value is used)"""
        if self.is_variable and self.value != 'value':
            try:
                self.parent_flowgraph.evaluate(
                    self.value, local_namespace=self.namespace)
            except Exception as err:
                self.add_error_message(
                    'Value "{}" cannot be evaluated:\n{}'.format(self.value, err))
    # endregion

    # region Properties

    def __str__(self):
        return 'Block - {} - {}({})'.format(self.name, self.label, self.key)

    def __repr__(self):
        try:
            name = self.name
        except Exception:
            name = self.key
        return 'block[' + name + ']'

    @property
    def name(self):
        return self.params['id'].value

    @lazy_property
    def is_virtual_or_pad(self):
        return self.key in ("virtual_source", "virtual_sink", "pad_source", "pad_sink")

    @lazy_property
    def is_variable(self):
        return bool(self.value)

    @lazy_property
    def is_import(self):
        return self.key == 'import'

    @lazy_property
    def is_snippet(self):
        return self.key == 'snippet'

    @property
    def comment(self):
        return self.params['comment'].value

    @property
    def state(self):
        """Gets the block's current state."""
        state = self.states['state']
        return state if state in self.STATE_LABELS else 'enabled'

    @state.setter
    def state(self, value):
        """Sets the state for the block."""
        self.states['state'] = value

    # Enable/Disable Aliases
    @property
    def enabled(self):
        """Get the enabled state of the block"""
        return self.state != 'disabled'

    @property
    def bus_sink(self):
        """Gets the block's current Toggle Bus Sink state."""
        return self.states['bus_sink']

    @bus_sink.setter
    def bus_sink(self, value):
        """Sets the Toggle Bus Sink state for the block."""
        self.states['bus_sink'] = value

    @property
    def bus_source(self):
        """Gets the block's current Toggle Bus Sink state."""
        return self.states['bus_source']

    @bus_source.setter
    def bus_source(self, value):
        """Sets the Toggle Bus Source state for the block."""
        self.states['bus_source'] = value

    @property
    def bus_structure_source(self):
        """Gets the block's current source bus structure."""
        try:
            bus_structure = self.params['bus_structure_source'].value or None
        except:
            bus_structure = None
        return bus_structure

    @property
    def bus_structure_sink(self):
        """Gets the block's current source bus structure."""
        try:
            bus_structure = self.params['bus_structure_sink'].value or None
        except:
            bus_structure = None
        return bus_structure

    # endregion

    ##############################################
    # Getters (old)
    ##############################################
    def get_var_make(self):
        return self.templates.render('var_make')

    def get_cpp_var_make(self):
        return self.cpp_templates.render('var_make')

    def get_var_value(self):
        return self.templates.render('var_value')

    def get_callbacks(self):
        """
        Get a list of function callbacks for this block.

        Returns:
            a list of strings
        """
        def make_callback(callback):
            if 'self.' in callback:
                return callback
            return 'self.{}.{}'.format(self.name, callback)

        return [make_callback(c) for c in self.templates.render('callbacks')]

    def get_cpp_callbacks(self):
        """
        Get a list of C++ function callbacks for this block.

        Returns:
            a list of strings
        """
        def make_callback(callback):
            if self.is_variable:
                return callback
            if 'this->' in callback:
                return callback
            return 'this->{}->{}'.format(self.name, callback)

        return [make_callback(c) for c in self.cpp_templates.render('callbacks')]

    def format_expr(self, py_type):
        """
        Evaluate the value of the variable block and decide its type.

        Returns:
            None
        """
        value = self.params['value'].value
        self.cpp_templates = copy.copy(self.orig_cpp_templates)

        # Determine the lvalue type
        def get_type(element, _vtype):
            evaluated = None
            try:
                evaluated = ast.literal_eval(element)
                if _vtype == None:
                    _vtype = type(evaluated)
            except ValueError or SyntaxError as excp:
                if _vtype == None:
                    print(excp)

            if _vtype in [int, float, bool, list, dict, str, complex]:
                if _vtype == (int or long):
                    return 'int'

                if _vtype == float:
                    return 'double'

                if _vtype == bool:
                    return 'bool'

                if _vtype == complex:
                    return 'gr_complex'

                if _vtype == list:
                    try:
                        # For container types we must also determine the type of the template parameter(s)
                        return 'std::vector<' + get_type(str(evaluated[0]), type(evaluated[0])) + '>'

                    except IndexError:  # empty list
                        return 'std::vector<std::string>'

                if _vtype == dict:
                    try:
                        # For container types we must also determine the type of the template parameter(s)
                        key = list(evaluated)[0]
                        val = list(evaluated.values())[0]
                        return 'std::map<' + get_type(str(key), type(key)) + ', ' + get_type(str(val), type(val)) + '>'

                    except IndexError:  # empty dict
                        return 'std::map<std::string, std::string>'

                else:
                    return 'std::string'

        # Get the lvalue type
        self.vtype = get_type(value, py_type)

        # The r-value for these types must be transformed to create legal C++ syntax.
        if self.vtype in ['bool', 'gr_complex'] or 'std::map' in self.vtype or 'std::vector' in self.vtype:
            evaluated = ast.literal_eval(value)
            self.cpp_templates['var_make'] = self.cpp_templates['var_make'].replace(
                '${value}', self.get_cpp_value(evaluated))

        if 'string' in self.vtype:
            self.cpp_templates['includes'].append('#include <string>')

    def get_cpp_value(self, pyval):

        if type(pyval) == int or type(pyval) == float:
            val_str = str(pyval)

            # Check for PI and replace with C++ constant
            pi_re = r'^(math|numpy|np|scipy|sp)\.pi$'
            if re.match(pi_re, str(pyval)):
                val_str = re.sub(
                    pi_re, 'boost::math::constants::pi<double>()', val_str)
                self.cpp_templates['includes'].append(
                    '#include <boost/math/constants/constants.hpp>')

            return str(pyval)

        elif type(pyval) == bool:
            return str(pyval)[0].lower() + str(pyval)[1:]

        elif type(pyval) == complex:
            self.cpp_templates['includes'].append(
                '#include <gnuradio/gr_complex.h>')
            evaluated = ast.literal_eval(str(pyval).strip())
            return '{' + str(evaluated.real) + ', ' + str(evaluated.imag) + '}'

        elif type(pyval) == list:
            self.cpp_templates['includes'].append('#include <vector>')
            val_str = '{'
            for element in pyval:
                val_str += self.get_cpp_value(element) + ', '

            if len(val_str) > 1:
                # truncate to trim superfluous ', ' from the end
                val_str = val_str[0:-2]

            return val_str + '}'

        elif type(pyval) == dict:
            self.cpp_templates['includes'].append('#include <map>')
            val_str = '{'
            for key in pyval:
                val_str += '{' + self.get_cpp_value(key) + \
                    ', ' + self.get_cpp_value(pyval[key]) + '}, '

            if len(val_str) > 1:
                # truncate to trim superfluous ', ' from the end
                val_str = val_str[0:-2]

            return val_str + '}'

        if type(self.vtype) == str:
            self.cpp_templates['includes'].append('#include <string>')
            return '"' + pyval + '"'

    def is_virtual_sink(self):
        return self.key == 'virtual_sink'

    def is_virtual_source(self):
        return self.key == 'virtual_source'

    def is_deprecated(self):
        """
        Check whether the block is deprecated.

        For now, we just check the category name for presence of "deprecated".

        As it might be desirable in the future to have such "tags" be stored
        explicitly, we're taking the detour of introducing a property.
        """
        if not self.category:
            return False

        try:
            return self.flags.deprecated or any("deprecated".casefold() in cat.casefold()
                                                for cat in self.category)
        except Exception as exception:
            print(exception.message)
        return False

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
            True if block changes state
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
        if not (self.sources[0].dtype == self.sinks[0].dtype):
            return False
        if self.flags.disable_bypass:
            return False
        return True

    def ports(self):
        return itertools.chain(self.sources, self.sinks)

    def active_ports(self):
        return itertools.chain(self.active_sources, self.active_sinks)

    def children(self):
        return itertools.chain(self.params.values(), self.ports())

    ##############################################
    # Access
    ##############################################

    def get_sink(self, key):
        return _get_elem(self.sinks, key)

    def get_source(self, key):
        return _get_elem(self.sources, key)

    ##############################################
    # Resolve
    ##############################################
    @property
    def namespace(self):
        # update block namespace
        self.block_namespace.update(
            {key: param.get_evaluated() for key, param in self.params.items()})
        return self.block_namespace

    @property
    def namespace_templates(self):
        return {key: param.template_arg for key, param in self.params.items()}

    def evaluate(self, expr):
        return self.parent_flowgraph.evaluate(expr, self.namespace)

    ##############################################
    # Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this block's params to nested data.

        Returns:
            a nested data odict
        """
        data = collections.OrderedDict()
        if self.key != 'options':
            data['name'] = self.name
            data['id'] = self.key
        data['parameters'] = collections.OrderedDict(sorted(
            (param_id, param.value) for param_id, param in self.params.items()
            if (param_id != 'id' or self.key == 'options')
        ))
        data['states'] = collections.OrderedDict(sorted(self.states.items()))
        return data

    def import_data(self, name, states, parameters, **_):
        """
        Import this block's params from nested data.
        Any param keys that do not exist will be ignored.
        Since params can be dynamically created based another param,
        call rewrite, and repeat the load until the params stick.
        """
        self.params['id'].value = name
        self.states.update(states)

        def get_hash():
            return hash(tuple(hash(v) for v in self.params.values()))

        pre_rewrite_hash = -1
        while pre_rewrite_hash != get_hash():
            for key, value in parameters.items():
                try:
                    self.params[key].set_value(value)
                except KeyError:
                    continue
            # Store hash and call rewrite
            pre_rewrite_hash = get_hash()
            self.rewrite()

    ##############################################
    # Controller Modify
    ##############################################
    def filter_bus_port(self, ports):
        buslist = [p for p in ports if p.dtype == 'bus']
        return buslist or ports

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
            ports = self.sources
            bus_structure = self.get_bus_structure('source')
        else:
            ports = self.sinks
            bus_structure = self.get_bus_structure('sink')

        struct = [range(len(ports))]
        # struct = list(range(len(ports)))
        # TODO for more complicated port structures, this code is needed but not working yet
        if any([p.multiplicity for p in ports]):
            structlet = []
            last = 0
            # group the ports with > n inputs together on the bus
            cnt = 0
            idx = 0
            for p in ports:
                if p.domain == 'message':
                    continue
                if cnt > 0:
                    cnt -= 1
                    continue

                if p.multiplicity > 1:
                    cnt = p.multiplicity - 1
                    structlet.append([idx + j for j in range(p.multiplicity)])
                else:
                    structlet.append([idx])

            struct = structlet
        if bus_structure:
            struct = bus_structure

        self.current_bus_structure[direc] = struct
        return struct

    def bussify(self, direc):
        if direc == 'source':
            ports = self.sources
            ports_gui = self.filter_bus_port(self.sources)
            self.bus_structure = self.get_bus_structure('source')
            self.bus_source = not self.bus_source
        else:
            ports = self.sinks
            ports_gui = self.filter_bus_port(self.sinks)
            self.bus_structure = self.get_bus_structure('sink')
            self.bus_sink = not self.bus_sink

        # Disconnect all the connections when toggling the bus state
        for port in ports:
            l_connections = list(port.connections())
            for connect in l_connections:
                self.parent.remove_element(connect)

        self.update_bus_logic()

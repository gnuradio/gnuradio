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
import copy

import six
from six.moves import range
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
    category = ''
    vtype = '' # This is only used for variables when we want C++ output
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
        if self.key == 'options' or self.is_variable:
            self.params['id'].hide = 'part'

        self.sinks = [port_factory(parent=self, **params) for params in self.inputs_data]
        self.sources = [port_factory(parent=self, **params) for params in self.outputs_data]

        self.active_sources = []  # on rewrite
        self.active_sinks = []  # on rewrite

        self.states = {'state': True}
        if 'cpp' in self.flags:
            self.orig_cpp_templates = self.cpp_templates # The original template, in case we have to edit it when transpiling to C++

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

        # disconnect hidden ports
        self.parent_flowgraph.disconnect(*[p for p in self.ports() if p.hidden])

        self.active_sources = [p for p in self.sources if not p.hidden]
        self.active_sinks = [p for p in self.sinks if not p.hidden]

    def _rewrite_nports(self, ports):
        for port in ports:
            if hasattr(port, 'master_port'):  # Not a master port and no left-over clones
                continue
            nports = port.multiplicity
            for clone in port.clones[nports-1:]:
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
                    self.add_error_message('Assertion "{}" failed.'.format(expr))
            except Exception:
                self.add_error_message('Assertion "{}" did not evaluate.'.format(expr))

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

        check_generate_mode('QT GUI', Flags.NEED_QT_GUI, ('qt_gui', 'hb_qt_gui'))

    def _validate_output_language_compat(self):
        """check if this block supports the selected output language"""
        current_output_language = self.parent.get_option('output_language')

        if current_output_language == 'cpp':
            if 'cpp' not in self.flags:
                self.add_error_message("This block does not support C++ output.")

            if self.key == 'parameter':
                if not self.params['type'].value:
                    self.add_error_message("C++ output requires you to choose a parameter type.")

    def _validate_var_value(self):
        """or variables check the value (only if var_value is used)"""
        if self.is_variable and self.value != 'value':
            try:
                self.parent_flowgraph.evaluate(self.value, local_namespace=self.namespace)
            except Exception as err:
                self.add_error_message('Value "{}" cannot be evaluated:\n{}'.format(self.value, err))
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

    def decide_type(self):
        """
        Evaluate the value of the variable block and decide its type.

        Returns:
            None
        """
        value = self.params['value'].value
        self.cpp_templates = copy.copy(self.orig_cpp_templates)

        def get_type(element):
            try:
                evaluated = ast.literal_eval(element)

            except ValueError or SyntaxError:
                if re.match(r'^(numpy|np|scipy|sp)\.pi$', value):
                    return 'pi'
                else:
                    return 'std::string'

            else:
                _vtype = type(evaluated)
                if _vtype in [int, float, bool, list]:
                    if _vtype == (int or long):
                        return 'int'

                    if _vtype == float:
                        return 'double'

                    if _vtype == bool:
                        return 'bool'

                    if _vtype == list:
                        try:
                            first_element_type = type(evaluated[0])
                            if first_element_type != str:
                                list_type = get_type(str(evaluated[0]))
                            else:
                                list_type = get_type(evaluated[0])

                        except IndexError: # empty list
                            return 'std::vector<std::string>'

                        else:
                            return 'std::vector<' + list_type + '>'

                else:
                    return 'std::string'

        self.vtype = get_type(value)
        if self.vtype == 'bool':
            self.cpp_templates['var_make'] = self.cpp_templates['var_make'].replace('${value}', (value[0].lower() + value[1:]))

        elif self.vtype == 'pi':
            self.vtype = 'double'
            self.cpp_templates['var_make'] = self.cpp_templates['var_make'].replace('${value}', 'boost::math::constants::pi<double>()')
            self.cpp_templates['includes'].append('#include <boost/math/constants/constants.hpp>')

        elif 'std::vector' in self.vtype:
            self.cpp_templates['includes'].append('#include <vector>')
            self.cpp_templates['var_make'] = self.cpp_templates['var_make'].replace('${value}', '{' + value[1:-1] + '}')

        if 'string' in self.vtype:
            self.cpp_templates['includes'].append('#include <string>')

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
        return itertools.chain(six.itervalues(self.params), self.ports())

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
        return {key: param.get_evaluated() for key, param in six.iteritems(self.params)}

    @property
    def namespace_templates(self):
        return {key: param.template_arg for key, param in six.iteritems(self.params)}

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
            for key, value in six.iteritems(parameters):
                try:
                    self.params[key].set_value(value)
                except KeyError:
                    continue
            # Store hash and call rewrite
            pre_rewrite_hash = get_hash()
            self.rewrite()

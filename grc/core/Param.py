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

import ast
import re
import collections

from six.moves import builtins, filter, map, range, zip

from . import Constants
from .Element import Element, nop_write

# Blacklist certain ids, its not complete, but should help
ID_BLACKLIST = ['self', 'options', 'gr', 'math', 'firdes'] + dir(builtins)
try:
    from gnuradio import gr
    ID_BLACKLIST.extend(attr for attr in dir(gr.top_block()) if not attr.startswith('_'))
except ImportError:
    pass

_check_id_matcher = re.compile('^[a-z|A-Z]\w*$')
_show_id_matcher = re.compile('^(variable\w*|parameter|options|notebook)$')


class TemplateArg(object):
    """
    A cheetah template argument created from a param.
    The str of this class evaluates to the param's to code method.
    The use of this class as a dictionary (enum only) will reveal the enum opts.
    The __call__ or () method can return the param evaluated to a raw python data type.
    """

    def __init__(self, param):
        self._param = param

    def __getitem__(self, item):
        param = self._param
        opts = param.options_opts[param.get_value()]
        return str(opts[item]) if param.is_enum() else NotImplemented

    def __str__(self):
        return str(self._param.to_code())

    def __call__(self):
        return self._param.get_evaluated()


class Param(Element):

    is_param = True

    def __init__(self, parent, key, name, type='raw', value='', **n):
        """Make a new param from nested data"""
        super(Param, self).__init__(parent)
        self.key = key
        self._name = name
        self.value = self.default = value
        self._type = type

        self._hide = n.get('hide', '')
        self.tab_label = n.get('tab', Constants.DEFAULT_PARAM_TAB)
        self._evaluated = None

        self.options = []
        self.options_names = []
        self.options_opts = {}
        self._init_options(options_n=n.get('option', []))

        self._init = False
        self._hostage_cells = list()
        self.template_arg = TemplateArg(self)

    def _init_options(self, options_n):
        """Create the Option objects from the n data"""
        option_keys = set()
        for option_n in options_n:
            key, name = option_n['key'], option_n['name']
            # Test against repeated keys
            if key in option_keys:
                raise KeyError('Key "{}" already exists in options'.format(key))
            option_keys.add(key)
            # Store the option
            self.options.append(key)
            self.options_names.append(name)

        if self.is_enum():
            self._init_enum(options_n)

    def _init_enum(self, options_n):
        opt_ref = None
        for option_n in options_n:
            key, opts_raw = option_n['key'], option_n.get('opt', [])
            try:
                self.options_opts[key] = opts = dict(opt.split(':') for opt in opts_raw)
            except TypeError:
                raise ValueError('Error separating opts into key:value')

            if opt_ref is None:
                opt_ref = set(opts.keys())
            elif opt_ref != set(opts):
                raise ValueError('Opt keys ({}) are not identical across all options.'
                                 ''.format(', '.join(opt_ref)))
        if not self.value:
            self.value = self.default = self.options[0]
        elif self.value not in self.options:
            self.value = self.default = self.options[0]  # TODO: warn
            # raise ValueError('The value {!r} is not in the possible values of {}.'
            #                  ''.format(self.get_value(), ', '.join(self.options)))

    def __str__(self):
        return 'Param - {}({})'.format(self.name, self.key)

    def get_hide(self):
        """
        Get the hide value from the base class.
        Hide the ID parameter for most blocks. Exceptions below.
        If the parameter controls a port type, vlen, or nports, return part.
        If the parameter is an empty grid position, return part.
        These parameters are redundant to display in the flow graph view.

        Returns:
            hide the hide property string
        """
        hide = self.parent.resolve_dependencies(self._hide).strip()
        if hide:
            return hide
        # Hide ID in non variable blocks
        if self.key == 'id' and not _show_id_matcher.match(self.parent.key):
            return 'part'
        # Hide port controllers for type and nports
        if self.key in ' '.join([' '.join([p._type, p._nports]) for p in self.parent.get_ports()]):
            return 'part'
        # Hide port controllers for vlen, when == 1
        if self.key in ' '.join(p._vlen for p in self.parent.get_ports()):
            try:
                if int(self.get_evaluated()) == 1:
                    return 'part'
            except:
                pass
        return hide

    def validate(self):
        """
        Validate the param.
        The value must be evaluated and type must a possible type.
        """
        Element.validate(self)
        if self.get_type() not in Constants.PARAM_TYPE_NAMES:
            self.add_error_message('Type "{}" is not a possible type.'.format(self.get_type()))

        self._evaluated = None
        try:
            self._evaluated = self.evaluate()
        except Exception as e:
            self.add_error_message(str(e))

    def get_evaluated(self):
        return self._evaluated

    def evaluate(self):
        """
        Evaluate the value.

        Returns:
            evaluated type
        """
        self._init = True
        self._lisitify_flag = False
        self._stringify_flag = False
        self._hostage_cells = list()
        t = self.get_type()
        v = self.get_value()

        #########################
        # Enum Type
        #########################
        if self.is_enum():
            return v

        #########################
        # Numeric Types
        #########################
        elif t in ('raw', 'complex', 'real', 'float', 'int', 'hex', 'bool'):
            # Raise exception if python cannot evaluate this value
            try:
                e = self.parent_flowgraph.evaluate(v)
            except Exception as e:
                raise Exception('Value "{}" cannot be evaluated:\n{}'.format(v, e))
            # Raise an exception if the data is invalid
            if t == 'raw':
                return e
            elif t == 'complex':
                if not isinstance(e, Constants.COMPLEX_TYPES):
                    raise Exception('Expression "{}" is invalid for type complex.'.format(str(e)))
                return e
            elif t == 'real' or t == 'float':
                if not isinstance(e, Constants.REAL_TYPES):
                    raise Exception('Expression "{}" is invalid for type float.'.format(str(e)))
                return e
            elif t == 'int':
                if not isinstance(e, Constants.INT_TYPES):
                    raise Exception('Expression "{}" is invalid for type integer.'.format(str(e)))
                return e
            elif t == 'hex':
                return hex(e)
            elif t == 'bool':
                if not isinstance(e, bool):
                    raise Exception('Expression "{}" is invalid for type bool.'.format(str(e)))
                return e
            else:
                raise TypeError('Type "{}" not handled'.format(t))
        #########################
        # Numeric Vector Types
        #########################
        elif t in ('complex_vector', 'real_vector', 'float_vector', 'int_vector'):
            if not v:
                # Turn a blank string into an empty list, so it will eval
                v = '()'
            # Raise exception if python cannot evaluate this value
            try:
                e = self.parent.parent.evaluate(v)
            except Exception as e:
                raise Exception('Value "{}" cannot be evaluated:\n{}'.format(v, e))
            # Raise an exception if the data is invalid
            if t == 'complex_vector':
                if not isinstance(e, Constants.VECTOR_TYPES):
                    self._lisitify_flag = True
                    e = [e]
                if not all([isinstance(ei, Constants.COMPLEX_TYPES) for ei in e]):
                    raise Exception('Expression "{}" is invalid for type complex vector.'.format(str(e)))
                return e
            elif t == 'real_vector' or t == 'float_vector':
                if not isinstance(e, Constants.VECTOR_TYPES):
                    self._lisitify_flag = True
                    e = [e]
                if not all([isinstance(ei, Constants.REAL_TYPES) for ei in e]):
                    raise Exception('Expression "{}" is invalid for type float vector.'.format(str(e)))
                return e
            elif t == 'int_vector':
                if not isinstance(e, Constants.VECTOR_TYPES):
                    self._lisitify_flag = True
                    e = [e]
                if not all([isinstance(ei, Constants.INT_TYPES) for ei in e]):
                    raise Exception('Expression "{}" is invalid for type integer vector.'.format(str(e)))
                return e
        #########################
        # String Types
        #########################
        elif t in ('string', 'file_open', 'file_save', '_multiline', '_multiline_python_external'):
            # Do not check if file/directory exists, that is a runtime issue
            try:
                e = self.parent.parent.evaluate(v)
                if not isinstance(e, str):
                    raise Exception()
            except:
                self._stringify_flag = True
                e = str(v)
            if t == '_multiline_python_external':
                ast.parse(e)  # Raises SyntaxError
            return e
        #########################
        # Unique ID Type
        #########################
        elif t == 'id':
            # Can python use this as a variable?
            if not _check_id_matcher.match(v):
                raise Exception('ID "{}" must begin with a letter and may contain letters, numbers, and underscores.'.format(v))
            ids = [param.get_value() for param in self.get_all_params(t)]

            # Id should only appear once, or zero times if block is disabled
            if ids.count(v) > 1:
                raise Exception('ID "{}" is not unique.'.format(v))
            if v in ID_BLACKLIST:
                raise Exception('ID "{}" is blacklisted.'.format(v))
            return v

        #########################
        # Stream ID Type
        #########################
        elif t == 'stream_id':
            # Get a list of all stream ids used in the virtual sinks
            ids = [param.get_value() for param in filter(
                lambda p: p.parent.is_virtual_sink(),
                self.get_all_params(t),
            )]
            # Check that the virtual sink's stream id is unique
            if self.parent.is_virtual_sink():
                # Id should only appear once, or zero times if block is disabled
                if ids.count(v) > 1:
                    raise Exception('Stream ID "{}" is not unique.'.format(v))
            # Check that the virtual source's steam id is found
            if self.parent.is_virtual_source():
                if v not in ids:
                    raise Exception('Stream ID "{}" is not found.'.format(v))
            return v

        #########################
        # GUI Position/Hint
        #########################
        elif t == 'gui_hint':
            if ':' in v:
                tab, pos = v.split(':')
            elif '@' in v:
                tab, pos = v, ''
            else:
                tab, pos = '', v

            if '@' in tab:
                tab, index = tab.split('@')
            else:
                index = '?'

            # TODO: Problem with this code. Produces bad tabs
            widget_str = ({
                (True, True): 'self.%(tab)s_grid_layout_%(index)s.addWidget(%(widget)s, %(pos)s)',
                (True, False): 'self.%(tab)s_layout_%(index)s.addWidget(%(widget)s)',
                (False, True): 'self.top_grid_layout.addWidget(%(widget)s, %(pos)s)',
                (False, False): 'self.top_layout.addWidget(%(widget)s)',
            }[bool(tab), bool(pos)]) % {'tab': tab, 'index': index, 'widget': '%s', 'pos': pos}

            # FIXME: Move replace(...) into the make template of the qtgui blocks
            # Return a string here
            class GuiHint(object):
                def __init__(self, ws):
                    self._ws = ws

                def __call__(self, w):
                    return (self._ws.replace('addWidget', 'addLayout') if 'layout' in w else self._ws) % w

                def __str__(self):
                    return self._ws
            return GuiHint(widget_str)
        #########################
        # Import Type
        #########################
        elif t == 'import':
            # New namespace
            n = dict()
            try:
                exec(v, n)
            except ImportError:
                raise Exception('Import "{}" failed.'.format(v))
            except Exception:
                raise Exception('Bad import syntax: "{}".'.format(v))
            return [k for k in list(n.keys()) if str(k) != '__builtins__']

        #########################
        else:
            raise TypeError('Type "{}" not handled'.format(t))

    def to_code(self):
        """
        Convert the value to code.
        For string and list types, check the init flag, call evaluate().
        This ensures that evaluate() was called to set the xxxify_flags.

        Returns:
            a string representing the code
        """
        v = self.get_value()
        t = self.get_type()
        # String types
        if t in ('string', 'file_open', 'file_save', '_multiline', '_multiline_python_external'):
            if not self._init:
                self.evaluate()
            return repr(v) if self._stringify_flag else v

        # Vector types
        elif t in ('complex_vector', 'real_vector', 'float_vector', 'int_vector'):
            if not self._init:
                self.evaluate()
            if self._lisitify_flag:
                return '(%s, )' % v
            else:
                return '(%s)' % v
        else:
            return v

    def get_all_params(self, type):
        """
        Get all the params from the flowgraph that have the given type.

        Args:
            type: the specified type

        Returns:
            a list of params
        """
        params = []
        for block in self.parent_flowgraph.get_enabled_blocks():
            params.extend(p for p in block.params.values() if p.get_type() == type)
        return params

    def is_enum(self):
        return self._type == 'enum'

    def get_value(self):
        value = self.value
        if self.is_enum() and value not in self.options:
            value = self.options[0]
            self.set_value(value)
        return value

    def set_value(self, value):
        # Must be a string
        self.value = str(value)

    def set_default(self, value):
        if self.default == self.value:
            self.set_value(value)
        self.default = str(value)

    def get_type(self):
        return self.parent.resolve_dependencies(self._type)

    def get_tab_label(self):
        return self.tab_label

    @nop_write
    @property
    def name(self):
        return self.parent.resolve_dependencies(self._name).strip()

    ##############################################
    # Access Options
    ##############################################
    def opt_value(self, key):
        return self.options_opts[self.get_value()][key]

    ##############################################
    # Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this param's key/value.

        Returns:
            a nested data odict
        """
        n = collections.OrderedDict()
        n['key'] = self.key
        n['value'] = self.get_value()
        return n

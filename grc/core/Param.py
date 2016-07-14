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
import weakref
import re
import collections

from six.moves import builtins, filter, map, range, zip

from . import Constants, utils
from .Element import Element

# Blacklist certain ids, its not complete, but should help
ID_BLACKLIST = ['self', 'options', 'gr', 'blks2', 'wxgui', 'wx', 'math', 'forms', 'firdes'] + dir(builtins)
try:
    from gnuradio import gr
    ID_BLACKLIST.extend(attr for attr in dir(gr.top_block()) if not attr.startswith('_'))
except ImportError:
    pass

_check_id_matcher = re.compile('^[a-z|A-Z]\w*$')
_show_id_matcher = re.compile('^(variable\w*|parameter|options|notebook)$')


class Option(Element):

    def __init__(self, param, n):
        Element.__init__(self, param)
        self._name = n.get('name')
        self.key = n.get('key')
        self._opts = dict()
        opts = n.get('opt', [])
        # Test against opts when non enum
        if not self.parent.is_enum() and opts:
            raise Exception('Options for non-enum types cannot have sub-options')
        # Extract opts
        for opt in opts:
            # Separate the key:value
            try:
                key, value = opt.split(':')
            except:
                raise Exception('Error separating "{}" into key:value'.format(opt))
            # Test against repeated keys
            if key in self._opts:
                raise Exception('Key "{}" already exists in option'.format(key))
            # Store the option
            self._opts[key] = value

    def __str__(self):
        return 'Option {}({})'.format(self.get_name(), self.key)

    def get_name(self):
        return self._name

    def get_key(self):
        return self.key

    ##############################################
    # Access Opts
    ##############################################
    def get_opt_keys(self):
        return list(self._opts.keys())

    def get_opt(self, key):
        return self._opts[key]

    def get_opts(self):
        return list(self._opts.values())


class TemplateArg(object):
    """
    A cheetah template argument created from a param.
    The str of this class evaluates to the param's to code method.
    The use of this class as a dictionary (enum only) will reveal the enum opts.
    The __call__ or () method can return the param evaluated to a raw python data type.
    """

    def __init__(self, param):
        self._param = weakref.proxy(param)

    def __getitem__(self, item):
        return str(self._param.get_opt(item)) if self._param.is_enum() else NotImplemented

    def __str__(self):
        return str(self._param.to_code())

    def __call__(self):
        return self._param.get_evaluated()


class Param(Element):

    is_param = True

    def __init__(self, block, n):
        """
        Make a new param from nested data.

        Args:
            block: the parent element
            n: the nested odict
        """
        # If the base key is a valid param key, copy its data and overlay this params data
        base_key = n.get('base_key')
        if base_key and base_key in block.params:
            n_expanded = block.params[base_key]._n.copy()
            n_expanded.update(n)
            n = n_expanded
        # Save odict in case this param will be base for another
        self._n = n
        # Parse the data
        self._name = n['name']
        self.key = n['key']
        value = n.get('value', '')
        self._type = n.get('type', 'raw')
        self._hide = n.get('hide', '')
        self.tab_label = n.get('tab', Constants.DEFAULT_PARAM_TAB)
        # Build the param
        Element.__init__(self, parent=block)
        # Create the Option objects from the n data
        self._options = list()
        self._evaluated = None
        for o_n in n.get('option', []):
            option = Option(param=self, n=o_n)
            key = option.key
            # Test against repeated keys
            if key in self.get_option_keys():
                raise Exception('Key "{}" already exists in options'.format(key))
            # Store the option
            self.get_options().append(option)
        # Test the enum options
        if self.is_enum():
            # Test against options with identical keys
            if len(set(self.get_option_keys())) != len(self.get_options()):
                raise Exception('Options keys "{}" are not unique.'.format(self.get_option_keys()))
            # Test against inconsistent keys in options
            opt_keys = self.get_options()[0].get_opt_keys()
            for option in self.get_options():
                if set(opt_keys) != set(option.get_opt_keys()):
                    raise Exception('Opt keys "{}" are not identical across all options.'.format(opt_keys))
            # If a value is specified, it must be in the options keys
            if value or value in self.get_option_keys():
                self._value = value
            else:
                self._value = self.get_option_keys()[0]
            if self.get_value() not in self.get_option_keys():
                raise Exception('The value "{}" is not in the possible values of "{}".'.format(self.get_value(), self.get_option_keys()))
        else:
            self._value = value or ''
        self._default = value
        self._init = False
        self._hostage_cells = list()
        self.template_arg = TemplateArg(self)

    def get_types(self):
        return (
            'raw', 'enum',
            'complex', 'real', 'float', 'int',
            'complex_vector', 'real_vector', 'float_vector', 'int_vector',
            'hex', 'string', 'bool',
            'file_open', 'file_save', '_multiline', '_multiline_python_external',
            'id', 'stream_id',
            'grid_pos', 'notebook', 'gui_hint',
            'import',
        )

    def __repr__(self):
        """
        Get the repr (nice string format) for this param.

        Returns:
            the string representation
        """
        ##################################################
        # Truncate helper method
        ##################################################
        def _truncate(string, style=0):
            max_len = max(27 - len(self.get_name()), 3)
            if len(string) > max_len:
                if style < 0:  # Front truncate
                    string = '...' + string[3-max_len:]
                elif style == 0:  # Center truncate
                    string = string[:max_len/2 - 3] + '...' + string[-max_len/2:]
                elif style > 0:  # Rear truncate
                    string = string[:max_len-3] + '...'
            return string

        ##################################################
        # Simple conditions
        ##################################################
        if not self.is_valid():
            return _truncate(self.get_value())
        if self.get_value() in self.get_option_keys():
            return self.get_option(self.get_value()).get_name()

        ##################################################
        # Split up formatting by type
        ##################################################
        # Default center truncate
        truncate = 0
        e = self.get_evaluated()
        t = self.get_type()
        if isinstance(e, bool):
            return str(e)
        elif isinstance(e, Constants.COMPLEX_TYPES):
            dt_str = utils.num_to_str(e)
        elif isinstance(e, Constants.VECTOR_TYPES):
            # Vector types
            if len(e) > 8:
                # Large vectors use code
                dt_str = self.get_value()
                truncate = 1
            else:
                # Small vectors use eval
                dt_str = ', '.join(map(utils.num_to_str, e))
        elif t in ('file_open', 'file_save'):
            dt_str = self.get_value()
            truncate = -1
        else:
            # Other types
            dt_str = str(e)

        # Done
        return _truncate(dt_str, truncate)

    def __repr2__(self):
        """
        Get the repr (nice string format) for this param.

        Returns:
            the string representation
        """
        if self.is_enum():
            return self.get_option(self.get_value()).get_name()
        return self.get_value()

    def __str__(self):
        return 'Param - {}({})'.format(self.get_name(), self.key)

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
        # Hide empty grid positions
        if self.key in ('grid_pos', 'notebook') and not self.get_value():
            return 'part'
        return hide

    def validate(self):
        """
        Validate the param.
        The value must be evaluated and type must a possible type.
        """
        Element.validate(self)
        if self.get_type() not in self.get_types():
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
        # Grid Position Type
        #########################
        elif t == 'grid_pos':
            if not v:
                # Allow for empty grid pos
                return ''
            e = self.parent_flowgraph.evaluate(v)
            if not isinstance(e, (list, tuple)) or len(e) != 4 or not all([isinstance(ei, int) for ei in e]):
                raise Exception('A grid position must be a list of 4 integers.')
            row, col, row_span, col_span = e
            # Check row, col
            if row < 0 or col < 0:
                raise Exception('Row and column must be non-negative.')
            # Check row span, col span
            if row_span <= 0 or col_span <= 0:
                raise Exception('Row and column span must be greater than zero.')
            # Get hostage cell parent
            try:
                my_parent = self.parent.get_param('notebook').evaluate()
            except:
                my_parent = ''
            # Calculate hostage cells
            for r in range(row_span):
                for c in range(col_span):
                    self._hostage_cells.append((my_parent, (row+r, col+c)))
            # Avoid collisions
            params = [p for p in self.get_all_params('grid_pos') if p is not self]
            for param in params:
                for parent, cell in param._hostage_cells:
                    if (parent, cell) in self._hostage_cells:
                        raise Exception('Another graphical element is using parent "{}", cell "{}".'.format(str(parent), str(cell)))
            return e
        #########################
        # Notebook Page Type
        #########################
        elif t == 'notebook':
            if not v:
                # Allow for empty notebook
                return ''

            # Get a list of all notebooks
            notebook_blocks = [b for b in self.parent_flowgraph.get_enabled_blocks() if b.key == 'notebook']
            # Check for notebook param syntax
            try:
                notebook_id, page_index = map(str.strip, v.split(','))
            except:
                raise Exception('Bad notebook page format.')
            # Check that the notebook id is valid
            try:
                notebook_block = [b for b in notebook_blocks if b.get_id() == notebook_id][0]
            except:
                raise Exception('Notebook id "{}" is not an existing notebook id.'.format(notebook_id))

            # Check that page index exists
            if int(page_index) not in range(len(notebook_block.get_param('labels').evaluate())):
                raise Exception('Page index "{}" is not a valid index number.'.format(page_index))
            return notebook_id, page_index

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
            if self._stringify_flag:
                return '"%s"' % v.replace('"', '\"')
            else:
                return v
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
        value = self._value
        if self.is_enum() and value not in self.get_option_keys():
            value = self.get_option_keys()[0]
            self.set_value(value)
        return value

    def set_value(self, value):
        # Must be a string
        self._value = str(value)

    def set_default(self, value):
        if self._default == self._value:
            self.set_value(value)
        self._default = str(value)

    def get_type(self):
        return self.parent.resolve_dependencies(self._type)

    def get_tab_label(self):
        return self.tab_label

    def get_name(self):
        return self.parent.resolve_dependencies(self._name).strip()

    ##############################################
    # Access Options
    ##############################################
    def get_option_keys(self):
        return [elem.key for elem in self._options]

    def get_option(self, key):
        for option in self._options:
            if option.key == key:
                return option
        return ValueError('Key "{}" not found in {}.'.format(key, self.get_option_keys()))

    def get_options(self):
        return self._options

    ##############################################
    # Access Opts
    ##############################################
    def get_opt_keys(self):
        return self.get_option(self.get_value()).get_opt_keys()

    def get_opt(self, key):
        return self.get_option(self.get_value()).get_opt(key)

    def get_opts(self):
        return self.get_option(self.get_value()).get_opts()

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

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

import ast
import weakref
import re
import textwrap

from . import Constants
from .Constants import VECTOR_TYPES, COMPLEX_TYPES, REAL_TYPES, INT_TYPES
from .Element import Element
from .utils import odict

# Blacklist certain ids, its not complete, but should help
import __builtin__


ID_BLACKLIST = ['self', 'options', 'gr', 'blks2', 'wxgui', 'wx', 'math', 'forms', 'firdes'] + dir(__builtin__)
try:
    from gnuradio import gr
    ID_BLACKLIST.extend(attr for attr in dir(gr.top_block()) if not attr.startswith('_'))
except ImportError:
    pass

_check_id_matcher = re.compile('^[a-z|A-Z]\w*$')
_show_id_matcher = re.compile('^(variable\w*|parameter|options|notebook)$')


def _get_keys(lst):
    return [elem.get_key() for elem in lst]


def _get_elem(lst, key):
    try:
        return lst[_get_keys(lst).index(key)]
    except ValueError:
        raise ValueError('Key "{0}" not found in {1}.'.format(key, _get_keys(lst)))


def num_to_str(num):
    """ Display logic for numbers """
    def eng_notation(value, fmt='g'):
        """Convert a number to a string in engineering notation.  E.g., 5e-9 -> 5n"""
        template = '{0:' + fmt + '}{1}'
        magnitude = abs(value)
        for exp, symbol in zip(range(9, -15-1, -3), 'GMk munpf'):
            factor = 10 ** exp
            if magnitude >= factor:
                return template.format(value / factor, symbol.strip())
        return template.format(value, '')

    if isinstance(num, COMPLEX_TYPES):
        num = complex(num)  # Cast to python complex
        if num == 0:
            return '0'
        output = eng_notation(num.real) if num.real else ''
        output += eng_notation(num.imag, '+g' if output else 'g') + 'j' if num.imag else ''
        return output
    else:
        return str(num)


class CallableString(str):
    """A string that perform old-style formatting when called

    Used as an adaptor for templates with gui_hint params
    ToDo: remove this in the YAML/Mako format
    """

    def __call__(self, *args):
        return self % args


class Option(Element):

    def __init__(self, param, n):
        Element.__init__(self, param)
        self._name = n.find('name')
        self._key = n.find('key')
        self._opts = dict()
        opts = n.findall('opt')
        # Test against opts when non enum
        if not self.get_parent().is_enum() and opts:
            raise Exception('Options for non-enum types cannot have sub-options')
        # Extract opts
        for opt in opts:
            # Separate the key:value
            try:
                key, value = opt.split(':')
            except:
                raise Exception('Error separating "{0}" into key:value'.format(opt))
            # Test against repeated keys
            if key in self._opts:
                raise Exception('Key "{0}" already exists in option'.format(key))
            # Store the option
            self._opts[key] = value

    def __str__(self):
        return 'Option {0}({1})'.format(self.get_name(), self.get_key())

    def get_name(self):
        return self._name

    def get_key(self):
        return self._key

    ##############################################
    # Access Opts
    ##############################################
    def get_opt_keys(self):
        return self._opts.keys()

    def get_opt(self, key):
        return self._opts[key]

    def get_opts(self):
        return self._opts.values()


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

    def __getattr__(self, item):
        if not self._param.is_enum():
            raise AttributeError()
        try:
            return str(self._param.get_opt(item))
        except KeyError:
            raise AttributeError()

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
        base_key = n.find('base_key')
        if base_key and base_key in block.get_param_keys():
            n_expanded = block.get_param(base_key)._n.copy()
            n_expanded.update(n)
            n = n_expanded
        # Save odict in case this param will be base for another
        self._n = n
        # Parse the data
        self._name = n.find('name')
        self._key = n.find('key')
        value = n.find('value') or ''
        self._type = n.find('type') or 'raw'
        self._hide = n.find('hide') or ''
        self._tab_label = n.find('tab') or block.get_param_tab_labels()[0]
        if self._tab_label not in block.get_param_tab_labels():
            block.get_param_tab_labels().append(self._tab_label)
        # Build the param
        Element.__init__(self, block)
        # Create the Option objects from the n data
        self._options = list()
        self._evaluated = None
        for option in map(lambda o: Option(param=self, n=o), n.findall('option')):
            key = option.get_key()
            # Test against repeated keys
            if key in self.get_option_keys():
                raise Exception('Key "{0}" already exists in options'.format(key))
            # Store the option
            self.get_options().append(option)
        # Test the enum options
        if self.is_enum():
            # Test against options with identical keys
            if len(set(self.get_option_keys())) != len(self.get_options()):
                raise Exception('Options keys "{0}" are not unique.'.format(self.get_option_keys()))
            # Test against inconsistent keys in options
            opt_keys = self.get_options()[0].get_opt_keys()
            for option in self.get_options():
                if set(opt_keys) != set(option.get_opt_keys()):
                    raise Exception('Opt keys "{0}" are not identical across all options.'.format(opt_keys))
            # If a value is specified, it must be in the options keys
            if value or value in self.get_option_keys():
                self._value = value
            else:
                self._value = self.get_option_keys()[0]
            if self.get_value() not in self.get_option_keys():
                raise Exception('The value "{0}" is not in the possible values of "{1}".'.format(self.get_value(), self.get_option_keys()))
        else:
            self._value = value or ''
        self._default = value
        self._init = False
        self.hostage_cells = set()
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
        elif isinstance(e, COMPLEX_TYPES):
            dt_str = num_to_str(e)
        elif isinstance(e, VECTOR_TYPES):
            # Vector types
            if len(e) > 8:
                # Large vectors use code
                dt_str = self.get_value()
                truncate = 1
            else:
                # Small vectors use eval
                dt_str = ', '.join(map(num_to_str, e))
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
        return 'Param - {0}({1})'.format(self.get_name(), self.get_key())

    def get_color(self):
        """
        Get the color that represents this param's type.

        Returns:
            a hex color code.
        """
        try:
            return {
                # Number types
                'complex': Constants.COMPLEX_COLOR_SPEC,
                'real': Constants.FLOAT_COLOR_SPEC,
                'float': Constants.FLOAT_COLOR_SPEC,
                'int': Constants.INT_COLOR_SPEC,
                # Vector types
                'complex_vector': Constants.COMPLEX_VECTOR_COLOR_SPEC,
                'real_vector': Constants.FLOAT_VECTOR_COLOR_SPEC,
                'float_vector': Constants.FLOAT_VECTOR_COLOR_SPEC,
                'int_vector': Constants.INT_VECTOR_COLOR_SPEC,
                # Special
                'bool': Constants.INT_COLOR_SPEC,
                'hex': Constants.INT_COLOR_SPEC,
                'string': Constants.BYTE_VECTOR_COLOR_SPEC,
                'id': Constants.ID_COLOR_SPEC,
                'stream_id': Constants.ID_COLOR_SPEC,
                'grid_pos': Constants.INT_VECTOR_COLOR_SPEC,
                'notebook': Constants.INT_VECTOR_COLOR_SPEC,
                'raw': Constants.WILDCARD_COLOR_SPEC,
            }[self.get_type()]
        except:
            return '#FFFFFF'

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
        hide = self.get_parent().resolve_dependencies(self._hide).strip()
        if hide:
            return hide
        # Hide ID in non variable blocks
        if self.get_key() == 'id' and not _show_id_matcher.match(self.get_parent().get_key()):
            return 'part'
        # Hide port controllers for type and nports
        if self.get_key() in ' '.join(map(lambda p: ' '.join([p._type, p._nports]),
                                          self.get_parent().get_ports())):
            return 'part'
        # Hide port controllers for vlen, when == 1
        if self.get_key() in ' '.join(map(
            lambda p: p._vlen, self.get_parent().get_ports())
        ):
            try:
                if int(self.get_evaluated()) == 1:
                    return 'part'
            except:
                pass
        # Hide empty grid positions
        if self.get_key() in ('grid_pos', 'notebook') and not self.get_value():
            return 'part'
        return hide

    def validate(self):
        """
        Validate the param.
        The value must be evaluated and type must a possible type.
        """
        Element.validate(self)
        if self.get_type() not in self.get_types():
            self.add_error_message('Type "{0}" is not a possible type.'.format(self.get_type()))

        self._evaluated = None
        try:
            self._evaluated = self.evaluate()
        except Exception, e:
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
                e = self.get_parent().get_parent().evaluate(v)
            except Exception, e:
                raise Exception('Value "{0}" cannot be evaluated:\n{1}'.format(v, e))
            # Raise an exception if the data is invalid
            if t == 'raw':
                return e
            elif t == 'complex':
                if not isinstance(e, COMPLEX_TYPES):
                    raise Exception('Expression "{0}" is invalid for type complex.'.format(str(e)))
                return e
            elif t == 'real' or t == 'float':
                if not isinstance(e, REAL_TYPES):
                    raise Exception('Expression "{0}" is invalid for type float.'.format(str(e)))
                return e
            elif t == 'int':
                if not isinstance(e, INT_TYPES):
                    raise Exception('Expression "{0}" is invalid for type integer.'.format(str(e)))
                return e
            elif t == 'hex':
                return hex(e)
            elif t == 'bool':
                if not isinstance(e, bool):
                    raise Exception('Expression "{0}" is invalid for type bool.'.format(str(e)))
                return e
            else:
                raise TypeError('Type "{0}" not handled'.format(t))
        #########################
        # Numeric Vector Types
        #########################
        elif t in ('complex_vector', 'real_vector', 'float_vector', 'int_vector'):
            if not v:
                # Turn a blank string into an empty list, so it will eval
                v = '()'
            # Raise exception if python cannot evaluate this value
            try:
                e = self.get_parent().get_parent().evaluate(v)
            except Exception, e:
                raise Exception('Value "{0}" cannot be evaluated:\n{1}'.format(v, e))
            # Raise an exception if the data is invalid
            if t == 'complex_vector':
                if not isinstance(e, VECTOR_TYPES):
                    self._lisitify_flag = True
                    e = [e]
                if not all([isinstance(ei, COMPLEX_TYPES) for ei in e]):
                    raise Exception('Expression "{0}" is invalid for type complex vector.'.format(str(e)))
                return e
            elif t == 'real_vector' or t == 'float_vector':
                if not isinstance(e, VECTOR_TYPES):
                    self._lisitify_flag = True
                    e = [e]
                if not all([isinstance(ei, REAL_TYPES) for ei in e]):
                    raise Exception('Expression "{0}" is invalid for type float vector.'.format(str(e)))
                return e
            elif t == 'int_vector':
                if not isinstance(e, VECTOR_TYPES):
                    self._lisitify_flag = True
                    e = [e]
                if not all([isinstance(ei, INT_TYPES) for ei in e]):
                    raise Exception('Expression "{0}" is invalid for type integer vector.'.format(str(e)))
                return e
        #########################
        # String Types
        #########################
        elif t in ('string', 'file_open', 'file_save', '_multiline', '_multiline_python_external'):
            # Do not check if file/directory exists, that is a runtime issue
            try:
                e = self.get_parent().get_parent().evaluate(v)
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
                raise Exception('ID "{0}" must begin with a letter and may contain letters, numbers, and underscores.'.format(v))
            ids = [param.get_value() for param in self.get_all_params(t, 'id')]

            if v in ID_BLACKLIST:
                raise Exception('ID "{0}" is blacklisted.'.format(v))

            if self._key == 'id':
                # Id should only appear once, or zero times if block is disabled
                if ids.count(v) > 1:
                    raise Exception('ID "{0}" is not unique.'.format(v))
            else:
                # Id should exist to be a reference
                if ids.count(v) < 1:
                    raise Exception('ID "{0}" does not exist.'.format(v))

            return v

        #########################
        # Stream ID Type
        #########################
        elif t == 'stream_id':
            # Get a list of all stream ids used in the virtual sinks
            ids = [param.get_value() for param in filter(
                lambda p: p.get_parent().is_virtual_sink(),
                self.get_all_params(t),
            )]
            # Check that the virtual sink's stream id is unique
            if self.get_parent().is_virtual_sink():
                # Id should only appear once, or zero times if block is disabled
                if ids.count(v) > 1:
                    raise Exception('Stream ID "{0}" is not unique.'.format(v))
            # Check that the virtual source's steam id is found
            if self.get_parent().is_virtual_source():
                if v not in ids:
                    raise Exception('Stream ID "{0}" is not found.'.format(v))
            return v

        #########################
        # GUI Position/Hint
        #########################
        elif t == 'gui_hint':
            if self.get_parent().get_state() == Constants.BLOCK_DISABLED:
                return ''
            else:
                return CallableString(self.parse_gui_hint(v))
        #########################
        # Grid Position Type
        #########################
        elif t == 'grid_pos':
            self.hostage_cells.clear()
            if not v:
                # Allow for empty grid pos
                return ''
            e = self.get_parent().get_parent().evaluate(v)
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
                my_parent = self.get_parent().get_param('notebook').evaluate()
            except:
                my_parent = ''
            # Calculate hostage cells
            for r in range(row, row + row_span):
                for c in range(col, col + col_span):
                    self.hostage_cells.add((my_parent, (r, c)))

            for other in self.get_all_params('grid_pos'):
                if other is self:
                    continue
                collision = next(iter(self.hostage_cells & other.hostage_cells), None)
                if collision:
                    raise Exception('Block {block!r} is also using parent {parent!r}, cell {cell!r}.'.format(
                        block=other.get_parent().get_id(), parent=collision[0] or 'main', cell=collision[1]
                    ))
            return e
        #########################
        # Notebook Page Type
        #########################
        elif t == 'notebook':
            if not v:
                # Allow for empty notebook
                return ''

            # Get a list of all notebooks
            notebook_blocks = filter(lambda b: b.get_key() == 'notebook', self.get_parent().get_parent().get_enabled_blocks())
            # Check for notebook param syntax
            try:
                notebook_id, page_index = map(str.strip, v.split(','))
            except:
                raise Exception('Bad notebook page format.')
            # Check that the notebook id is valid
            try:
                notebook_block = filter(lambda b: b.get_id() == notebook_id, notebook_blocks)[0]
            except:
                raise Exception('Notebook id "{0}" is not an existing notebook id.'.format(notebook_id))

            # Check that page index exists
            if int(page_index) not in range(len(notebook_block.get_param('labels').evaluate())):
                raise Exception('Page index "{0}" is not a valid index number.'.format(page_index))
            return notebook_id, page_index

        #########################
        # Import Type
        #########################
        elif t == 'import':
            # New namespace
            n = dict()
            try:
                exec v in n
            except ImportError:
                raise Exception('Import "{0}" failed.'.format(v))
            except Exception:
                raise Exception('Bad import syntax: "{0}".'.format(v))
            return filter(lambda k: str(k) != '__builtins__', n.keys())

        #########################
        else:
            raise TypeError('Type "{0}" not handled'.format(t))

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

    def get_all_params(self, type, key=None):
        """
        Get all the params from the flowgraph that have the given type and
        optionally a given key

        Args:
            type: the specified type
            key: the key to match against

        Returns:
            a list of params
        """
        return sum([filter(lambda p: ((p.get_type() == type) and ((key is None) or (p.get_key() == key))), block.get_params()) for block in self.get_parent().get_parent().get_enabled_blocks()], [])

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
        return self.get_parent().resolve_dependencies(self._type)

    def get_tab_label(self):
        return self._tab_label

    def get_name(self):
        return self.get_parent().resolve_dependencies(self._name).strip()

    def get_key(self):
        return self._key

    ##############################################
    # Access Options
    ##############################################
    def get_option_keys(self):
        return _get_keys(self.get_options())

    def get_option(self, key):
        return _get_elem(self.get_options(), key)

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
        n = odict()
        n['key'] = self.get_key()
        n['value'] = self.get_value()
        return n

    ##############################################
    # GUI Hint
    ##############################################
    def parse_gui_hint(self, v):
        """
        Parse/validate gui hint value.

        Args:
            v: gui_hint string from a block's 'gui_hint' param

        Returns:
            string of python code for positioning GUI elements in pyQT
        """
        self.hostage_cells.clear()

        # Parsing
        if ':' in v:
            tab, pos = v.split(':')
        elif ',' in v:
            tab, pos = '', v
        else:
            tab, pos = v, ''

        if '@' in tab:
            tab, index = tab.split('@')
        else:
            index = '0'
        index = int(index)

        # Validation
        def parse_pos():
            e = self.get_parent().get_parent().evaluate(pos)

            if not isinstance(e, (list, tuple)) or len(e) not in (2, 4) or not all(isinstance(ei, int) for ei in e):
                raise Exception('Invalid GUI Hint entered: {e!r} (Must be a list of {{2,4}} non-negative integers).'.format(e=e))

            if len(e) == 2:
                row, col = e
                row_span = col_span = 1
            else:
                row, col, row_span, col_span = e

            if (row < 0) or (col < 0):
                raise Exception('Invalid GUI Hint entered: {e!r} (non-negative integers only).'.format(e=e))

            if (row_span < 1) or (col_span < 1):
                raise Exception('Invalid GUI Hint entered: {e!r} (positive row/column span required).'.format(e=e))

            return row, col, row_span, col_span

        def validate_tab():
            enabled_blocks = self.get_parent().get_parent().iter_enabled_blocks()
            tabs = (block for block in enabled_blocks
                    if block.get_key() == 'qtgui_tab_widget' and block.get_id() == tab)
            tab_block = next(iter(tabs), None)
            if not tab_block:
                raise Exception('Invalid tab name entered: {tab} (Tab name not found).'.format(tab=tab))

            tab_index_size = int(tab_block.get_param('num_tabs').get_value())
            if index >= tab_index_size:
                raise Exception('Invalid tab index entered: {tab}@{index} (Index out of range).'.format(
                    tab=tab, index=index))

        # Collision Detection
        def collision_detection(row, col, row_span, col_span):
            my_parent = '{tab}@{index}'.format(tab=tab, index=index) if tab else 'main'
            # Calculate hostage cells
            for r in range(row, row + row_span):
                for c in range(col, col + col_span):
                    self.hostage_cells.add((my_parent, (r, c)))

            for other in self.get_all_params('gui_hint'):
                if other is self:
                    continue
                collision = next(iter(self.hostage_cells & other.hostage_cells), None)
                if collision:
                    raise Exception('Block {block!r} is also using parent {parent!r}, cell {cell!r}.'.format(
                        block=other.get_parent().get_id(), parent=collision[0], cell=collision[1]
                    ))

        # Code Generation
        if tab:
            validate_tab()
            layout = '{tab}_grid_layout_{index}'.format(tab=tab, index=index)
        else:
            layout = 'top_grid_layout'

        widget = '%s'  # to be fill-out in the main template

        if pos:
            row, col, row_span, col_span = parse_pos()
            collision_detection(row, col, row_span, col_span)

            widget_str = textwrap.dedent("""
                self.{layout}.addWidget({widget}, {row}, {col}, {row_span}, {col_span})
                for r in range({row}, {row_end}):
                    self.{layout}.setRowStretch(r, 1)
                for c in range({col}, {col_end}):
                    self.{layout}.setColumnStretch(c, 1)
            """.strip('\n')).format(
                layout=layout, widget=widget,
                row=row, row_span=row_span, row_end=row+row_span,
                col=col, col_span=col_span, col_end=col+col_span,
            )

        else:
            widget_str = 'self.{layout}.addWidget({widget})'.format(layout=layout, widget=widget)

        return widget_str

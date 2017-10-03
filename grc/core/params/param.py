# Copyright 2008-2017 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import

import ast
import numbers
import re
import collections
import textwrap

import six
from six.moves import builtins, range

from .. import Constants, blocks
from ..base import Element
from ..utils.descriptors import Evaluated, EvaluatedEnum, setup_names

from .template_arg import TemplateArg

# Blacklist certain ids, its not complete, but should help
ID_BLACKLIST = ['self', 'options', 'gr', 'math', 'firdes'] + dir(builtins)
try:
    from gnuradio import gr
    ID_BLACKLIST.extend(attr for attr in dir(gr.top_block()) if not attr.startswith('_'))
except (ImportError, AttributeError):
    pass


@setup_names
class Param(Element):

    is_param = True

    name = Evaluated(str, default='no name')
    dtype = EvaluatedEnum(Constants.PARAM_TYPE_NAMES, default='raw')
    hide = EvaluatedEnum('none all part')

    # region init
    def __init__(self, parent, id, label='', dtype='raw', default='',
                 options=None, option_labels=None, option_attributes=None,
                 category='', hide='none', **_):
        """Make a new param from nested data"""
        super(Param, self).__init__(parent)
        self.key = id
        self.name = label.strip() or id.title()
        self.category = category or Constants.DEFAULT_PARAM_TAB

        self.dtype = dtype
        self.value = self.default = str(default)

        self.options = self._init_options(options or [], option_labels or [],
                                          option_attributes or {})
        self.hide = hide or 'none'
        # end of args ########################################################

        self._evaluated = None
        self._stringify_flag = False
        self._lisitify_flag = False
        self.hostage_cells = set()
        self._init = False

    def _init_options(self, values, labels, attributes):
        """parse option and option attributes"""
        options = collections.OrderedDict()
        options.attributes = collections.defaultdict(dict)

        padding = [''] * max(len(values), len(labels))
        attributes = {key: value + padding for key, value in six.iteritems(attributes)}

        for i, option in enumerate(values):
            # Test against repeated keys
            if option in options:
                raise KeyError('Value "{}" already exists in options'.format(option))
            # get label
            try:
                label = str(labels[i])
            except IndexError:
                label = str(option)
            # Store the option
            options[option] = label
            options.attributes[option] = {attrib: values[i] for attrib, values in six.iteritems(attributes)}

        default = next(iter(options)) if options else ''
        if not self.value:
            self.value = self.default = default

        if self.is_enum() and self.value not in options:
            self.value = self.default = default  # TODO: warn
            # raise ValueError('The value {!r} is not in the possible values of {}.'
            #                  ''.format(self.get_value(), ', '.join(self.options)))
        return options
    # endregion

    @property
    def template_arg(self):
        return TemplateArg(self)

    def __str__(self):
        return 'Param - {}({})'.format(self.name, self.key)

    def __repr__(self):
        return '{!r}.param[{}]'.format(self.parent, self.key)

    def is_enum(self):
        return self.get_raw('dtype') == 'enum'

    def get_value(self):
        value = self.value
        if self.is_enum() and value not in self.options:
            value = self.default
            self.set_value(value)
        return value

    def set_value(self, value):
        # Must be a string
        self.value = str(value)

    def set_default(self, value):
        if self.default == self.value:
            self.set_value(value)
        self.default = str(value)

    def rewrite(self):
        Element.rewrite(self)
        del self.name
        del self.dtype
        del self.hide

        self._evaluated = None
        try:
            self._evaluated = self.evaluate()
        except Exception as e:
            self.add_error_message(str(e))

    def validate(self):
        """
        Validate the param.
        The value must be evaluated and type must a possible type.
        """
        Element.validate(self)
        if self.dtype not in Constants.PARAM_TYPE_NAMES:
            self.add_error_message('Type "{}" is not a possible type.'.format(self.dtype))

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
        dtype = self.dtype
        expr = self.get_value()

        #########################
        # Enum Type
        #########################
        if self.is_enum():
            return expr

        #########################
        # Numeric Types
        #########################
        elif dtype in ('raw', 'complex', 'real', 'float', 'int', 'hex', 'bool'):
            # Raise exception if python cannot evaluate this value
            try:
                value = self.parent_flowgraph.evaluate(expr)
            except Exception as value:
                raise Exception('Value "{}" cannot be evaluated:\n{}'.format(expr, value))
            # Raise an exception if the data is invalid
            if dtype == 'raw':
                return value
            elif dtype == 'complex':
                if not isinstance(value, Constants.COMPLEX_TYPES):
                    raise Exception('Expression "{}" is invalid for type complex.'.format(str(value)))
                return value
            elif dtype in ('real', 'float'):
                if not isinstance(value, Constants.REAL_TYPES):
                    raise Exception('Expression "{}" is invalid for type float.'.format(str(value)))
                return value
            elif dtype == 'int':
                if not isinstance(value, Constants.INT_TYPES):
                    raise Exception('Expression "{}" is invalid for type integer.'.format(str(value)))
                return value
            elif dtype == 'hex':
                return hex(value)
            elif dtype == 'bool':
                if not isinstance(value, bool):
                    raise Exception('Expression "{}" is invalid for type bool.'.format(str(value)))
                return value
            else:
                raise TypeError('Type "{}" not handled'.format(dtype))
        #########################
        # Numeric Vector Types
        #########################
        elif dtype in ('complex_vector', 'real_vector', 'float_vector', 'int_vector'):
            default = []

            if not expr:
                return default   # Turn a blank string into an empty list, so it will eval

            try:
                value = self.parent.parent.evaluate(expr)
            except Exception as value:
                raise Exception('Value "{}" cannot be evaluated:\n{}'.format(expr, value))

            if not isinstance(value, Constants.VECTOR_TYPES):
                self._lisitify_flag = True
                value = [value]

            # Raise an exception if the data is invalid
            if dtype == 'complex_vector' and not all(isinstance(item, numbers.Complex) for item in value):
                raise Exception('Expression "{}" is invalid for type complex vector.'.format(value))
            elif dtype in ('real_vector', 'float_vector') and not all(isinstance(item, numbers.Real) for item in value):
                raise Exception('Expression "{}" is invalid for type float vector.'.format(value))
            elif dtype == 'int_vector' and not all(isinstance(item, Constants.INT_TYPES) for item in value):
                raise Exception('Expression "{}" is invalid for type integer vector.'.format(str(value)))
            return value
        #########################
        # String Types
        #########################
        elif dtype in ('string', 'file_open', 'file_save', '_multiline', '_multiline_python_external'):
            # Do not check if file/directory exists, that is a runtime issue
            try:
                value = self.parent.parent.evaluate(expr)
                if not isinstance(value, str):
                    raise Exception()
            except:
                self._stringify_flag = True
                value = str(expr)
            if dtype == '_multiline_python_external':
                ast.parse(value)  # Raises SyntaxError
            return value
        #########################
        # Unique ID Type
        #########################
        elif dtype == 'id':
            self.validate_block_id()
            return expr

        #########################
        # Stream ID Type
        #########################
        elif dtype == 'stream_id':
            self.validate_stream_id()
            return expr

        #########################
        # GUI Position/Hint
        #########################
        elif dtype == 'gui_hint':
            if self.parent_block.state == 'disabled':
                return ''
            else:
                return self.parse_gui_hint(expr)

        #########################
        # Import Type
        #########################
        elif dtype == 'import':
            # New namespace
            n = dict()
            try:
                exec(expr, n)
            except ImportError:
                raise Exception('Import "{}" failed.'.format(expr))
            except Exception:
                raise Exception('Bad import syntax: "{}".'.format(expr))
            return [k for k in list(n.keys()) if str(k) != '__builtins__']

        #########################
        else:
            raise TypeError('Type "{}" not handled'.format(dtype))

    def validate_block_id(self):
        value = self.value
        # Can python use this as a variable?
        if not re.match(r'^[a-z|A-Z]\w*$', value):
            raise Exception('ID "{}" must begin with a letter and may contain letters, numbers, '
                            'and underscores.'.format(value))
        if value in ID_BLACKLIST:
            raise Exception('ID "{}" is blacklisted.'.format(value))
        block_names = [block.name for block in self.parent_flowgraph.iter_enabled_blocks()]
        # Id should only appear once, or zero times if block is disabled
        if self.key == 'id' and block_names.count(value) > 1:
            raise Exception('ID "{}" is not unique.'.format(value))
        elif value not in block_names:
            raise Exception('ID "{}" does not exist.'.format(value))
        return value

    def validate_stream_id(self):
        value = self.value
        stream_ids = [
            block.params['stream_id'].value
            for block in self.parent_flowgraph.iter_enabled_blocks()
            if isinstance(block, blocks.VirtualSink)
            ]
        # Check that the virtual sink's stream id is unique
        if isinstance(self.parent_block, blocks.VirtualSink) and stream_ids.count(value) >= 2:
            # Id should only appear once, or zero times if block is disabled
            raise Exception('Stream ID "{}" is not unique.'.format(value))
        # Check that the virtual source's steam id is found
        elif isinstance(self.parent_block, blocks.VirtualSource) and value not in stream_ids:
            raise Exception('Stream ID "{}" is not found.'.format(value))

    def to_code(self):
        """
        Convert the value to code.
        For string and list types, check the init flag, call evaluate().
        This ensures that evaluate() was called to set the xxxify_flags.

        Returns:
            a string representing the code
        """
        self._init = True
        v = self.get_value()
        t = self.dtype
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

    def get_opt(self, item):
        return self.options.attributes[self.get_value()][item]

    ##############################################
    # GUI Hint
    ##############################################
    def parse_gui_hint(self, expr):
        """
        Parse/validate gui hint value.

        Args:
            expr: gui_hint string from a block's 'gui_hint' param

        Returns:
            string of python code for positioning GUI elements in pyQT
        """
        self.hostage_cells.clear()

        # Parsing
        if ':' in expr:
            tab, pos = expr.split(':')
        elif ',' in expr:
            tab, pos = '', expr
        else:
            tab, pos = expr, ''

        if '@' in tab:
            tab, index = tab.split('@')
        else:
            index = '0'
        index = int(index)

        # Validation
        def parse_pos():
            e = self.parent_flowgraph.evaluate(pos)

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
            tabs = (block for block in self.parent_flowgraph.iter_enabled_blocks()
                    if block.key == 'qtgui_tab_widget' and block.name == tab)
            tab_block = next(iter(tabs), None)
            if not tab_block:
                raise Exception('Invalid tab name entered: {tab} (Tab name not found).'.format(tab=tab))

            tab_index_size = int(tab_block.params['num_tabs'].value)
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
                        block=other.parent_block.name, parent=collision[0], cell=collision[1]
                    ))

        # Code Generation
        if tab:
            validate_tab()
            layout = '{tab}_grid_layout_{index}'.format(tab=tab, index=index)
        else:
            layout = 'top_grid_layout'

        widget = '%s'  # to be fill-out in the mail template

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

    def get_all_params(self, dtype, key=None):
        """
        Get all the params from the flowgraph that have the given type and
        optionally a given key

        Args:
            type: the specified type
            key: the key to match against

        Returns:
            a list of params
        """
        params = []
        for block in self.parent_flowgraph.iter_enabled_blocks():
            params.extend(
                param for param in block.params.values()
                if param.dtype == dtype and (key is None or key == param.name)
            )
        return params

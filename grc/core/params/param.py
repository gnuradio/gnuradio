# Copyright 2008-2017 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


import ast
import collections
import textwrap

from .. import Constants
from ..base import Element
from ..utils.descriptors import Evaluated, EvaluatedEnum, setup_names

from . import dtypes
from .template_arg import TemplateArg

attributed_str = type('attributed_str', (str,), {})


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
        self.name = 'ID' if id == 'id' else (label.strip() or id.title())
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
        self.scale = {
            'E': 1e18,
            'P': 1e15,
            'T': 1e12,
            'G': 1e9,
            'M': 1e6,
            'k': 1e3,
            'm': 1e-3,
            'u': 1e-6,
            'n': 1e-9,
            'p': 1e-12,
            'f': 1e-15,
            'a': 1e-18,
        }
        self.scale_factor = None
        self.number = None

    def _init_options(self, values, labels, attributes):
        """parse option and option attributes"""
        options = collections.OrderedDict()
        options.attributes = collections.defaultdict(dict)

        padding = [''] * max(len(values), len(labels))
        attributes = {key: value + padding for key,
                      value in attributes.items()}

        for i, option in enumerate(values):
            # Test against repeated keys
            if option in options:
                raise KeyError(
                    'Value "{}" already exists in options'.format(option))
            # get label
            try:
                label = str(labels[i])
            except IndexError:
                label = str(option)
            # Store the option
            options[option] = label
            options.attributes[option] = {attrib: values[i]
                                          for attrib, values in attributes.items()}

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

        rewriter = getattr(dtypes, 'rewrite_' + self.dtype, None)
        if rewriter:
            rewriter(self)

    def validate(self):
        """
        Validate the param.
        The value must be evaluated and type must a possible type.
        """
        Element.validate(self)
        if self.dtype not in Constants.PARAM_TYPE_NAMES:
            self.add_error_message(
                'Type "{}" is not a possible type.'.format(self.dtype))

        validator = dtypes.validators.get(self.dtype, None)
        if self._init and validator:
            try:
                validator(self, self.parent_flowgraph.get_imported_names())
            except dtypes.ValidateError as e:
                self.add_error_message(str(e))

    def get_evaluated(self):
        return self._evaluated

    def is_float(self, num):
        """
        Check if string can be converted to float.

        Returns:
            bool type
        """
        try:
            float(num)
            return True
        except ValueError:
            return False

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
        scale_factor = self.scale_factor

        #########################
        # ID and Enum types (not evaled)
        #########################
        if dtype in ('id', 'stream_id', 'name') or self.is_enum():
            if self.options.attributes:
                expr = attributed_str(expr)
                for key, value in self.options.attributes[expr].items():
                    setattr(expr, key, value)
            return expr

        #########################
        # Numeric Types
        #########################
        elif dtype in ('raw', 'complex', 'real', 'float', 'int', 'short', 'byte', 'hex', 'bool'):
            if expr:
                try:
                    if isinstance(expr, str) and self.is_float(expr[:-1]):
                        scale_factor = expr[-1:]
                        if scale_factor in self.scale:
                            expr = str(float(expr[:-1]) *
                                       self.scale[scale_factor])
                    value = self.parent_flowgraph.evaluate(expr)
                except Exception as e:
                    raise Exception(
                        'Value "{}" cannot be evaluated:\n{}'.format(expr, e))
            else:
                value = None   # No parameter value provided
            if dtype == 'hex':
                value = hex(value)
            elif dtype == 'bool':
                value = bool(value)
            return value

        #########################
        # Numeric Vector Types
        #########################
        elif dtype in ('complex_vector', 'real_vector', 'float_vector', 'int_vector'):
            if not expr:
                return []   # Turn a blank string into an empty list, so it will eval
            try:
                value = self.parent.parent.evaluate(expr)
            except Exception as value:
                raise Exception(
                    'Value "{}" cannot be evaluated:\n{}'.format(expr, value))
            if not isinstance(value, Constants.VECTOR_TYPES):
                self._lisitify_flag = True
                value = [value]
            return value
        #########################
        # String Types
        #########################
        elif dtype in ('string', 'file_open', 'file_save', 'dir_select', '_multiline', '_multiline_python_external'):
            # Do not check if file/directory exists, that is a runtime issue
            try:
                # Do not evaluate multiline strings (code snippets or comments)
                if dtype not in ['_multiline', '_multiline_python_external']:
                    value = self.parent_flowgraph.evaluate(expr)
                    if not isinstance(value, str):
                        raise Exception()
                else:
                    value = str(expr)
            except Exception:
                self._stringify_flag = True
                value = str(expr)
            if dtype == '_multiline_python_external':
                ast.parse(value)  # Raises SyntaxError
            return value
        #########################
        # GUI Position/Hint
        #########################
        elif dtype == 'gui_hint':
            return self.parse_gui_hint(expr) if self.parent_block.state == 'enabled' else ''
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

    def to_code(self):
        """
        Convert the value to code.
        For string and list types, check the init flag, call evaluate().
        This ensures that evaluate() was called to set the xxxify_flags.

        Returns:
            a string representing the code
        """
        self._init = True
        value = self.get_value()
        # String types
        if self.dtype in ('string', 'file_open', 'file_save', 'dir_select', '_multiline', '_multiline_python_external'):
            if not self._init:
                self.evaluate()
            return repr(value) if self._stringify_flag else value

        # Vector types
        elif self.dtype in ('complex_vector', 'real_vector', 'float_vector', 'int_vector'):
            if not self._init:
                self.evaluate()
            return '[' + value + ']' if self._lisitify_flag else value
        else:
            return value

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
                raise Exception(
                    'Invalid GUI Hint entered: {e!r} (Must be a list of {{2,4}} non-negative integers).'.format(e=e))

            if len(e) == 2:
                row, col = e
                row_span = col_span = 1
            else:
                row, col, row_span, col_span = e

            if (row < 0) or (col < 0):
                raise Exception(
                    'Invalid GUI Hint entered: {e!r} (non-negative integers only).'.format(e=e))

            if (row_span < 1) or (col_span < 1):
                raise Exception(
                    'Invalid GUI Hint entered: {e!r} (positive row/column span required).'.format(e=e))

            return row, col, row_span, col_span

        def validate_tab():
            tabs = (block for block in self.parent_flowgraph.iter_enabled_blocks()
                    if block.key == 'qtgui_tab_widget' and block.name == tab)
            tab_block = next(iter(tabs), None)
            if not tab_block:
                raise Exception(
                    'Invalid tab name entered: {tab} (Tab name not found).'.format(tab=tab))

            tab_index_size = int(tab_block.params['num_tabs'].value)
            if index >= tab_index_size:
                raise Exception('Invalid tab index entered: {tab}@{index} (Index out of range).'.format(
                    tab=tab, index=index))

        # Collision Detection
        def collision_detection(row, col, row_span, col_span):
            my_parent = '{tab}@{index}'.format(tab=tab,
                                               index=index) if tab else 'main'
            # Calculate hostage cells
            for r in range(row, row + row_span):
                for c in range(col, col + col_span):
                    self.hostage_cells.add((my_parent, (r, c)))

            for other in self.get_all_params('gui_hint'):
                if other is self:
                    continue
                collision = next(
                    iter(self.hostage_cells & other.hostage_cells), None)
                if collision:
                    raise Exception('Block {block!r} is also using parent {parent!r}, cell {cell!r}.'.format(
                        block=other.parent_block.name, parent=collision[0], cell=collision[1]
                    ))

        # Code Generation
        if tab:
            validate_tab()
            if not pos:
                layout = '{tab}_layout_{index}'.format(tab=tab, index=index)
            else:
                layout = '{tab}_grid_layout_{index}'.format(
                    tab=tab, index=index)
        else:
            if not pos:
                layout = 'top_layout'
            else:
                layout = 'top_grid_layout'

        widget = '%s'  # to be fill-out in the mail template

        if pos:
            row, col, row_span, col_span = parse_pos()
            collision_detection(row, col, row_span, col_span)

            if self.parent_flowgraph.get_option('output_language') == 'python':
                widget_str = textwrap.dedent("""
                    self.{layout}.addWidget({widget}, {row}, {col}, {row_span}, {col_span})
                    for r in range({row}, {row_end}):
                        self.{layout}.setRowStretch(r, 1)
                    for c in range({col}, {col_end}):
                        self.{layout}.setColumnStretch(c, 1)
                """.strip('\n')).format(
                    layout=layout, widget=widget,
                    row=row, row_span=row_span, row_end=row + row_span,
                    col=col, col_span=col_span, col_end=col + col_span,
                )
            elif self.parent_flowgraph.get_option('output_language') == 'cpp':
                widget_str = textwrap.dedent("""
                    {layout}->addWidget({widget}, {row}, {col}, {row_span}, {col_span});
                    for(int r = {row};r < {row_end}; r++)
                        {layout}->setRowStretch(r, 1);
                    for(int c = {col}; c <{col_end}; c++)
                        {layout}->setColumnStretch(c, 1);
                """.strip('\n')).format(
                    layout=layout, widget=widget,
                    row=row, row_span=row_span, row_end=row + row_span,
                    col=col, col_span=col_span, col_end=col + col_span,
                )
            else:
                widget_str = ''

        else:
            if self.parent_flowgraph.get_option('output_language') == 'python':
                widget_str = 'self.{layout}.addWidget({widget})'.format(
                    layout=layout, widget=widget)
            elif self.parent_flowgraph.get_option('output_language') == 'cpp':
                widget_str = '{layout}->addWidget({widget});'.format(
                    layout=layout, widget=widget)
            else:
                widget_str = ''

        return widget_str

    def get_all_params(self, dtype, key=None):
        """
        Get all the params from the flowgraph that have the given type and
        optionally a given key

        Args:
            dtype: the specified type
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

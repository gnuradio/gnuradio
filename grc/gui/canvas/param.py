# Copyright 2007-2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


import numbers

from .drawable import Drawable
from .. import ParamWidgets, Utils, Constants, Actions
from ...core.params import Param as CoreParam


class Param(CoreParam):
    """The graphical parameter."""

    make_cls_with_base = classmethod(Drawable.make_cls_with_base.__func__)

    def get_input(self, *args, **kwargs):
        """
        Get the graphical gtk class to represent this parameter.
        An enum requires and combo parameter.
        A non-enum with options gets a combined entry/combo parameter.
        All others get a standard entry parameter.

        Returns:
            gtk input class
        """
        dtype = self.dtype
        if dtype in ('file_open', 'file_save'):
            input_widget_cls = ParamWidgets.FileParam

        elif dtype == 'dir_select':
            input_widget_cls = ParamWidgets.DirectoryParam

        elif dtype == 'enum':
            input_widget_cls = ParamWidgets.EnumParam

        elif self.options:
            input_widget_cls = ParamWidgets.EnumEntryParam

        elif dtype == '_multiline':
            input_widget_cls = ParamWidgets.MultiLineEntryParam

        elif dtype == '_multiline_python_external':
            input_widget_cls = ParamWidgets.PythonEditorParam

        else:
            input_widget_cls = ParamWidgets.EntryParam

        return input_widget_cls(self, *args, **kwargs)

    def format_label_markup(self, have_pending_changes=False):
        block = self.parent
        # fixme: using non-public attribute here
        has_callback = \
            hasattr(block, 'templates') and \
            any(self.key in callback for callback in block.templates.get('callbacks', ''))

        return '<span {underline} {foreground} font_desc="Sans 9">{label}</span>'.format(
            underline='underline="low"' if has_callback else '',
            foreground='foreground="blue"' if have_pending_changes else
                       'foreground="red"' if not self.is_valid() else '',
            label=Utils.encode(self.name)
        )

    def format_tooltip_text(self):
        errors = self.get_error_messages()
        tooltip_lines = ['Key: ' + self.key, 'Type: ' + self.dtype]
        if self.is_valid():
            value = self.get_evaluated()
            if hasattr(value, "__len__"):
                tooltip_lines.append('Length: {}'.format(len(value)))
            value = str(value)
            # ensure that value is a UTF-8 string
            # Old PMTs could produce non-UTF-8 strings
            value = value.encode('utf-8', 'backslashreplace').decode('utf-8')
            if len(value) > 100:
                value = '{}...{}'.format(value[:50], value[-50:])
            tooltip_lines.append('Value: ' + value)
        elif len(errors) == 1:
            tooltip_lines.append('Error: ' + errors[0])
        elif len(errors) > 1:
            tooltip_lines.append('Error:')
            tooltip_lines.extend(' * ' + msg for msg in errors)
        return '\n'.join(tooltip_lines)

    ##################################################
    # Truncate helper method
    ##################################################

    def truncate(self, string, style=0):
        max_len = max(27 - len(self.name), 3)
        if len(string) > max_len:
            if style < 0:  # Front truncate
                string = '...' + string[3 - max_len:]
            elif style == 0:  # Center truncate
                string = string[:max_len // 2 - 3] + \
                    '...' + string[-max_len // 2:]
            elif style > 0:  # Rear truncate
                string = string[:max_len - 3] + '...'
        return string

    def pretty_print(self):
        """
        Get the repr (nice string format) for this param.

        Returns:
            the string representation
        """

        ##################################################
        # Simple conditions
        ##################################################
        value = self.get_value()
        if not self.is_valid():
            return self.truncate(value)
        if value in self.options:
            return self.options[value]  # its name

        ##################################################
        # Split up formatting by type
        ##################################################
        # Default center truncate
        truncate = 0
        e = self.get_evaluated()
        t = self.dtype
        if isinstance(e, bool):
            return str(e)
        elif isinstance(e, numbers.Complex):
            dt_str = Utils.num_to_str(e)
        elif isinstance(e, Constants.VECTOR_TYPES):
            # Vector types
            if len(e) > 8:
                # Large vectors use code
                dt_str = self.get_value()
                truncate = 1
            else:
                # Small vectors use eval
                dt_str = ', '.join(map(Utils.num_to_str, e))
        elif t in ('file_open', 'file_save'):
            dt_str = self.get_value()
            truncate = -1
        else:
            # Other types
            dt_str = str(e)
            # ensure that value is a UTF-8 string
            # Old PMTs could produce non-UTF-8 strings
            dt_str = dt_str.encode('utf-8', 'backslashreplace').decode('utf-8')

        # Done
        return self.truncate(dt_str, truncate)

    def format_block_surface_markup(self):
        """
        Get the markup for this param.

        Returns:
            a pango markup string
        """

        # TODO: is this the correct way to do this?
        is_evaluated = self.value != str(self.get_evaluated())
        show_value = Actions.TOGGLE_SHOW_PARAMETER_EVALUATION.get_active()
        show_expr = Actions.TOGGLE_SHOW_PARAMETER_EXPRESSION.get_active()

        display_value = ""

        # Include the value defined by the user (after evaluation)
        if not is_evaluated or show_value or not show_expr:
            display_value += Utils.encode(
                self.pretty_print().replace('\n', ' '))

        # Include the expression that was evaluated to get the value
        if is_evaluated and show_expr:
            expr_string = "<i>" + \
                Utils.encode(self.truncate(self.value)) + "</i>"

            if display_value:  # We are already displaying the value
                display_value = expr_string + "=" + display_value
            else:
                display_value = expr_string

        return '<span {foreground} font_desc="{font}"><b>{label}:</b> {value}</span>'.format(
            foreground='foreground="red"' if not self.is_valid() else '', font=Constants.PARAM_FONT,
            label=Utils.encode(self.name), value=display_value)

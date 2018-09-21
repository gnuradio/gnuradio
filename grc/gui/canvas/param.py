# Copyright 2007-2016 Free Software Foundation, Inc.
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

import numbers

from .drawable import Drawable
from .. import ParamWidgets, Utils, Constants
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

        elif dtype == 'enum':
            input_widget_cls = ParamWidgets.EnumParam

        elif dtype == 'bool':
            input_widget_cls = ParamWidgets.BoolParam

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
            value = str(self.get_evaluated())
            if len(value) > 100:
                value = '{}...{}'.format(value[:50], value[-50:])
            tooltip_lines.append('Value: ' + value)
        elif len(errors) == 1:
            tooltip_lines.append('Error: ' + errors[0])
        elif len(errors) > 1:
            tooltip_lines.append('Error:')
            tooltip_lines.extend(' * ' + msg for msg in errors)
        return '\n'.join(tooltip_lines)

    def pretty_print(self):
        """
        Get the repr (nice string format) for this param.

        Returns:
            the string representation
        """
        ##################################################
        # Truncate helper method
        ##################################################
        def _truncate(string, style=0):
            max_len = max(27 - len(self.name), 3)
            if len(string) > max_len:
                if style < 0:  # Front truncate
                    string = '...' + string[3-max_len:]
                elif style == 0:  # Center truncate
                    string = string[:max_len//2 - 3] + '...' + string[-max_len//2:]
                elif style > 0:  # Rear truncate
                    string = string[:max_len-3] + '...'
            return string

        ##################################################
        # Simple conditions
        ##################################################
        value = self.get_value()
        if not self.is_valid():
            return _truncate(value)
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

        # Done
        return _truncate(dt_str, truncate)

    def format_block_surface_markup(self):
        """
        Get the markup for this param.

        Returns:
            a pango markup string
        """
        return '<span {foreground} font_desc="{font}"><b>{label}:</b> {value}</span>'.format(
            foreground='foreground="red"' if not self.is_valid() else '', font=Constants.PARAM_FONT,
            label=Utils.encode(self.name), value=Utils.encode(self.pretty_print().replace('\n', ' '))
        )

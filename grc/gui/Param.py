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
from . import Utils, Constants

from . import ParamWidgets
from .Element import Element

from ..core.Param import Param as _Param


class Param(Element, _Param):
    """The graphical parameter."""

    def __init__(self, **kwargs):
        Element.__init__(self)
        _Param.__init__(self, **kwargs)

    def get_input(self, *args, **kwargs):
        """
        Get the graphical gtk class to represent this parameter.
        An enum requires and combo parameter.
        A non-enum with options gets a combined entry/combo parameter.
        All others get a standard entry parameter.

        Returns:
            gtk input class
        """
        if self.get_type() in ('file_open', 'file_save'):
            input_widget_cls = ParamWidgets.FileParam

        elif self.is_enum():
            input_widget_cls = ParamWidgets.EnumParam

        elif self.get_options():
            input_widget_cls = ParamWidgets.EnumEntryParam

        elif self.get_type() == '_multiline':
            input_widget_cls = ParamWidgets.MultiLineEntryParam

        elif self.get_type() == '_multiline_python_external':
            input_widget_cls = ParamWidgets.PythonEditorParam

        else:
            input_widget_cls = ParamWidgets.EntryParam

        return input_widget_cls(self, *args, **kwargs)

    def format_label_markup(self, have_pending_changes=False):
        block = self.get_parent()
        # fixme: using non-public attribute here
        has_callback = \
            hasattr(block, 'get_callbacks') and \
            any(self.get_key() in callback for callback in block._callbacks)

        return '<span underline="{line}" foreground="{color}" font_desc="Sans 9">{label}</span>'.format(
            line='low' if has_callback else 'none',
            color='blue' if have_pending_changes else
            'black' if self.is_valid() else
            'red',
            label=Utils.encode(self.get_name())
        )

    def format_tooltip_text(self):
        errors = self.get_error_messages()
        tooltip_lines = ['Key: ' + self.get_key(), 'Type: ' + self.get_type()]
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

    def format_block_surface_markup(self):
        """
        Get the markup for this param.

        Returns:
            a pango markup string
        """
        return '<span foreground="{color}" font_desc="{font}"><b>{label}:</b> {value}</span>'.format(
            color='black' if self.is_valid() else 'red', font=Constants.PARAM_FONT,
            label=Utils.encode(self.get_name()), value=Utils.encode(repr(self).replace('\n', ' '))
        )

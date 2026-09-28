# Copyright 2026 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from grc.core.utils import hide_bokeh_gui_options_if_not_installed


class _DummyOptionsBlock:
    def __init__(self, parameters_data):
        self.parameters_data = parameters_data


def test_hide_bokeh_gui_options_when_bokeh_not_in_list():
    """Test that hide_bokeh_gui_options_if_not_installed does not crash when bokeh_gui is absent."""
    block = _DummyOptionsBlock([
        {
            'id': 'generate_options',
            'label': 'Generate Options',
            'dtype': 'enum',
            'options': ['qt_gui', 'no_gui'],
            'option_labels': ['QT GUI', 'No GUI'],
        }
    ])
    hide_bokeh_gui_options_if_not_installed(block)
    assert block.parameters_data[0]['options'] == ['qt_gui', 'no_gui']
    assert block.parameters_data[0]['option_labels'] == ['QT GUI', 'No GUI']


def test_hide_bokeh_gui_options_when_options_empty():
    """Test that hide_bokeh_gui_options_if_not_installed does not crash when options list is empty."""
    block = _DummyOptionsBlock([
        {
            'id': 'generate_options',
            'label': 'Generate Options',
            'dtype': 'enum',
            'options': [],
            'option_labels': [],
        }
    ])
    hide_bokeh_gui_options_if_not_installed(block)
    assert block.parameters_data[0]['options'] == []
    assert block.parameters_data[0]['option_labels'] == []


def test_hide_bokeh_gui_options_removes_bokeh_when_present():
    """Test that hide_bokeh_gui_options_if_not_installed properly removes bokeh_gui when present."""
    block = _DummyOptionsBlock([
        {
            'id': 'generate_options',
            'label': 'Generate Options',
            'dtype': 'enum',
            'options': ['qt_gui', 'bokeh_gui', 'no_gui'],
            'option_labels': ['QT GUI', 'Bokeh GUI', 'No GUI'],
        }
    ])
    hide_bokeh_gui_options_if_not_installed(block)
    assert block.parameters_data[0]['options'] == ['qt_gui', 'no_gui']
    assert block.parameters_data[0]['option_labels'] == ['QT GUI', 'No GUI']

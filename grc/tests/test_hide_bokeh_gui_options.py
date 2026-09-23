"""
Copyright 2026 Rishi Raj

SPDX-License-Identifier: GPL-3.0-or-later
"""
import sys

import pytest

from grc.core.utils import hide_bokeh_gui_options_if_not_installed


class _DummyOptionsBlock:
    """Minimal stand-in for the options block."""

    def __init__(self, options, option_labels):
        self.parameters_data = [
            dict(id='generate_options',
                 options=options,
                 option_labels=option_labels),
        ]


@pytest.fixture(autouse=True)
def no_bokehgui(monkeypatch):
    # a None entry makes "import bokehgui" raise ImportError
    monkeypatch.setitem(sys.modules, 'bokehgui', None)


def test_bokeh_gui_option_is_removed():
    options_blk = _DummyOptionsBlock(
        ['qt_gui', 'bokeh_gui', 'no_gui'], ['QT GUI', 'Bokeh GUI', 'No GUI'])
    hide_bokeh_gui_options_if_not_installed(options_blk)
    param = options_blk.parameters_data[0]
    assert param['options'] == ['qt_gui', 'no_gui']
    assert param['option_labels'] == ['QT GUI', 'No GUI']


def test_missing_bokeh_gui_option_is_not_an_error():
    options_blk = _DummyOptionsBlock(['qt_gui', 'no_gui'], ['QT GUI', 'No GUI'])
    hide_bokeh_gui_options_if_not_installed(options_blk)
    param = options_blk.parameters_data[0]
    assert param['options'] == ['qt_gui', 'no_gui']
    assert param['option_labels'] == ['QT GUI', 'No GUI']

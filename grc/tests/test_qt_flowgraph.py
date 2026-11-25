"""
Copyright 2026 Himamshu Soni

SPDX-License-Identifier: GPL-3.0-or-later
"""

import sys
import unittest.mock
from types import SimpleNamespace
import pytest

# Ensure gnuradio runtime module is mocked if not installed
if 'gnuradio' not in sys.modules:
    sys.modules['gnuradio'] = unittest.mock.MagicMock()

try:
    from PyQt6.QtWidgets import QApplication
    from grc.gui_qt.Platform import Platform
    from grc.gui_qt.components.canvas.flowgraph import Flowgraph
    HAS_QT = True
except ImportError:
    HAS_QT = False


@pytest.fixture(scope="module")
def qapp():
    if not HAS_QT:
        pytest.skip("PyQt6 is required for Qt GRC tests")
    app = QApplication.instance()
    if app is None:
        app = QApplication([])
    return app


def test_qt_platform_make_flow_graph(qapp):
    p = Platform(version='3.11', install_prefix='/usr')
    fg = p.make_flow_graph()
    assert fg is not None
    assert fg.parent_platform is p


def test_qt_flowgraph_constructor_signatures(qapp):
    p = Platform(version='3.11', install_prefix='/usr')
    dummy_gui = object()

    # Signature 1: Flowgraph(gui, platform)
    fg1 = Flowgraph(dummy_gui, p)
    assert fg1.gui is dummy_gui
    assert fg1.parent_platform is p

    # Signature 2: Flowgraph(parent=platform)
    fg2 = Flowgraph(parent=p)
    assert fg2.gui is None
    assert fg2.parent_platform is p

    # Signature 3: Flowgraph(platform)
    fg3 = Flowgraph(p)
    assert fg3.gui is None
    assert fg3.parent_platform is p


def test_platform_load_and_generate_flow_graph_error_handling(qapp):
    p = Platform(version='3.11', install_prefix='/usr')
    flow_graph, generator = p.load_and_generate_flow_graph('nonexistent_file.grc')
    assert flow_graph is None
    assert generator is None


@pytest.mark.parametrize("legacy_is_directory", [True, False])
def test_qt_preferences_migration(qapp, tmp_path, monkeypatch, legacy_is_directory):
    legacy_path = tmp_path / "legacy"
    prefs_file = tmp_path / "grc_qt.conf"
    if legacy_is_directory:
        legacy_path.mkdir()
        fixture = legacy_path / "fixture.grc"
        fixture.write_text("fixture", encoding="utf-8")
    else:
        legacy_path.write_text("[grc]\n", encoding="utf-8")
    monkeypatch.setenv("GRC_PREFS_PATH", str(legacy_path))
    platform = SimpleNamespace(config=SimpleNamespace(gui_prefs_file=str(prefs_file)))

    Platform._move_old_pref_file(platform)

    if legacy_is_directory:
        assert fixture.read_text(encoding="utf-8") == "fixture"
        assert not prefs_file.exists()
    else:
        assert not legacy_path.exists()
        assert prefs_file.read_text(encoding="utf-8") == "[grc]\n"


def test_qt_props_dialog_dropdown_handling(qapp):
    from grc.gui_qt.components.dialogs import PropsDialog
    from PyQt6.QtWidgets import QComboBox

    class MockParam:
        def __init__(self, name, dtype, value, options=None, category='General'):
            self.name = name
            self.dtype = dtype
            self.value = str(value)
            self.category = category
            self.hide = 'none'
            self.options = options or {}
            self.default = str(value)

        def get_value(self):
            return self.value

        def set_value(self, val):
            self.value = str(val)

    class MockBlock:
        def __init__(self, params):
            self.params = params
            self.label = 'Test Block'
            self.enabled = True
            self.gui = unittest.mock.MagicMock()
            self.parent = unittest.mock.MagicMock()
            self.old_data = {'name': 'test', 'states': {}, 'parameters': {}}

        def export_data(self):
            return self.old_data

        def rewrite(self):
            pass

        def validate(self):
            pass

        def get_error_messages(self):
            return []

    # 1. Float option key vs string value (UHD USRP Sink/Source bug #7826)
    p1 = MockParam('start_time', 'real', '-1.0', {-1.0: "Don't sync"})
    # 2. Options with custom variable name (Audio sink bug #7826)
    p2 = MockParam('samp_rate', 'int', 'audio_rate', {48000: '48 kHz', 44100: '44.1 kHz'})
    # 3. Strict enum
    p3 = MockParam('type', 'enum', 'fc32', {'fc32': 'Complex Float 32', 'f32': 'Float 32'})

    block = MockBlock({'start_time': p1, 'samp_rate': p2, 'type': p3})
    qapp.qsettings = unittest.mock.MagicMock()
    dialog = PropsDialog(block, force_show_id=False)

    w_start_time = dialog.edit_params[0]
    w_samp_rate = dialog.edit_params[1]
    w_type = dialog.edit_params[2]

    # Check start_time matched float option and did not crash
    assert isinstance(w_start_time, QComboBox)
    assert w_start_time.currentIndex() == 0
    assert w_start_time.currentText() == "Don't sync"

    # Check custom variable is preserved and editable
    assert isinstance(w_samp_rate, QComboBox)
    assert w_samp_rate.isEditable()
    assert w_samp_rate.currentText() == 'audio_rate'
    dialog._apply_param_value(w_samp_rate)
    assert p2.value == 'audio_rate'

    # Check selecting preset option sets the corresponding key value
    w_samp_rate.setCurrentIndex(0)  # 48 kHz
    dialog._apply_param_value(w_samp_rate)
    assert p2.value == '48000'

    # Check typing a custom value in editable dropdown sets custom value
    w_start_time.setEditText('-0.5')
    dialog._apply_param_value(w_start_time)
    assert p1.value == '-0.5'

    # Check strict enum behavior
    assert isinstance(w_type, QComboBox)
    assert not w_type.isEditable()
    assert w_type.currentIndex() == 0
    w_type.setCurrentIndex(1)
    dialog._apply_param_value(w_type)
    assert p3.value == 'f32'

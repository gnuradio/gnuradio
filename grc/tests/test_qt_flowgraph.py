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

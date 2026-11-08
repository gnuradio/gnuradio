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
    from grc.gui_qt import Constants
    from grc.gui_qt.Config import Config as QtConfig
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


def test_block_and_port_font_pixel_sizing(qapp):
    p = Platform(version='3.11', install_prefix='/usr')
    fg = p.make_flow_graph()
    block = fg.new_block('options')
    # Check that block font pixel size is set (not point size)
    assert block.gui.font.pixelSize() == Constants.BLOCK_FONT_PIXEL_SIZE
    assert block.gui.font.pixelSize() > 0

    if block.sources:
        port = block.sources[0]
        assert port.gui.font.pixelSize() == Constants.PORT_FONT_PIXEL_SIZE
        assert port.gui.font.pixelSize() > 0


def test_default_flowgraph_blocks_do_not_overlap(qapp, monkeypatch):
    import os
    blocks_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'blocks'))
    monkeypatch.setenv("GRC_BLOCKS_PATH", blocks_path)

    p = Platform(version='3.11', install_prefix=os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..')))
    p.build_library()

    fg = p.make_flow_graph()
    initial_state = p.parse_flow_graph("")
    fg.import_data(initial_state)

    blocks = fg.blocks
    assert len(blocks) >= 2
    options_block = next(b for b in blocks if b.key == 'options')
    var_block = next(b for b in blocks if b.key == 'variable' and b.name == 'samp_rate')

    opt_rect = options_block.gui.sceneBoundingRect()
    var_rect = var_block.gui.sceneBoundingRect()

    # Verify that blocks do not intersect / overlap
    assert not opt_rect.intersects(var_rect), f"Options block ({opt_rect}) overlaps Variable block ({var_rect})"
    assert opt_rect.right() < var_rect.left()


def test_qt_config_dpi_scaling_and_font_size(tmp_path, monkeypatch):
    prefs_file = tmp_path / "grc_qt.conf"
    prefs_file.write_text("[appearance]\ndpi_scaling=1.5\n", encoding="utf-8")
    monkeypatch.setenv("GRC_QT_PREFS_PATH", str(prefs_file))

    config = QtConfig(version='3.11', install_prefix='/usr')
    assert config.dpi_scaling == 1.5
    assert Constants.DPI_SCALING == 1.5
    assert Constants.BLOCK_FONT_PIXEL_SIZE > 0

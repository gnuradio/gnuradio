import sys
import unittest.mock
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

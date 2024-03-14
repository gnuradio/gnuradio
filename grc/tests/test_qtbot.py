import pytest
import gettext
import locale
import threading
import sys

from pytestqt.plugin import qapp

import time

import pyautogui as pag

import logging

from qtpy import QtTest, QtCore, QtGui, QtWidgets, QT6
from os import path, remove

from gnuradio import gr
from grc.gui_qt import properties
from grc.gui_qt.grc import Application
from grc.gui_qt.components.window import MainWindow
from grc.gui_qt.Platform import Platform

log = logging.getLogger("grc")


@pytest.fixture(scope="session")
def qapp_cls_():
    settings = properties.Properties([])
    settings.argv = [""]

    """ Translation Support """
    # Try to get the current locale. Always add English
    lc, encoding = locale.getlocale()
    if lc:
        languages = [lc]
    languages += settings.DEFAULT_LANGUAGE
    log.debug("Using locale - %s" % str(languages))

    # Still run even if the english translation isn't found
    language = gettext.translation(
        settings.APP_NAME, settings.path.LANGUAGE, languages=languages, fallback=True
    )
    if type(language) == gettext.NullTranslations:
        log.error("Unable to find any translation")
        log.error("Default English translation missing")
    else:
        log.info("Using translation - %s" % language.info()["language"])
    # Still need to install null translation to let the system handle calls to _()
    language.install()

    model = Platform(
        version=gr.version(),
        version_parts=(gr.major_version(), gr.api_version(), gr.minor_version()),
        prefs=gr.prefs(),
        install_prefix=gr.prefix(),
    )
    model.build_library()
    app = Application(settings, model)
    app.MainWindow.showMaximized()
    return app


def global_pos(block, view):
    scene_pos = block.mapToScene(block.boundingRect().center())
    global_pos = view.viewport().mapToGlobal(view.mapFromScene(scene_pos))
    return global_pos


def type_text(qtbot, app, keys):
    for key in keys:
        # Each sequence contains a single key.
        # That's why we use the first element
        keycode = QtGui.QKeySequence(key)[0]
        if QT6:
            qtbot.keyClick(app.focusWidget(), keycode.key(), QtCore.Qt.NoModifier)
        else:
            qtbot.keyClick(app.focusWidget(), keycode, QtCore.Qt.NoModifier)


def keystroke(qtbot, app, key):
    qtbot.keyClick(app.focusWidget(), key, QtCore.Qt.NoModifier)
    qtbot.wait(100)


def ctrl_keystroke(qtbot, app, key):
    qtbot.keyClick(app.focusWidget(), key, QtCore.Qt.ControlModifier)
    qtbot.wait(100)


def gather_menu_items(menu):
    ret = {}
    for act in menu.actions():
        ret[act.text()] = act
    return ret


def add_block_from_query(qtbot, app, query):
    qtbot.keyClick(app.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.ControlModifier)
    type_text(qtbot, app, query)
    keystroke(qtbot, app, QtCore.Qt.Key_Enter)


def find_blocks(flowgraph, block_type):
    blocks = []
    for b in flowgraph.blocks:
        if b.key == block_type:
            blocks.append(b)

    if len(blocks) == 0:
        return None
    if len(blocks) == 1:
        return blocks[0]
    return blocks


def click_on(qtbot, app, item, button="left"):
    scaling = app.MainWindow.screen().devicePixelRatio()
    view = app.MainWindow.currentView
    click_pos = scaling * global_pos(item.gui, view)
    pag.click(click_pos.x(), click_pos.y(), button=button)
    qtbot.wait(100)


def undo(qtbot, app):
    qtbot.keyClick(app.focusWidget(), QtCore.Qt.Key_Z, QtCore.Qt.ControlModifier)
    qtbot.wait(100)


def redo(qtbot, app):
    qtbot.keyClick(
        app.focusWidget(),
        QtCore.Qt.Key_Z,
        QtCore.Qt.ControlModifier | QtCore.Qt.ShiftModifier,
    )
    qtbot.wait(100)


def delete_block(qtbot, app, block):
    view = app.MainWindow.currentView
    scaling = app.MainWindow.screen().devicePixelRatio()
    click_pos = scaling * global_pos(block.gui, view)
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    qtbot.keyClick(app.focusWidget(), QtCore.Qt.Key_Delete)
    qtbot.wait(100)


def menu_shortcut(qtbot, app, menu_name, menu_key, shortcut_key):
    menu = app.MainWindow.menus[menu_name]
    qtbot.keyClick(app.focusWidget(), menu_key, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    qtbot.keyClick(menu, shortcut_key)
    qtbot.wait(100)

# Start by closing the flowgraph that pops up on start


def test_file_close_init(qtbot, qapp_cls_, monkeypatch):
    win = qapp_cls_.MainWindow
    monkeypatch.setattr(
        QtWidgets.QMessageBox,
        "question",
        lambda *args: QtWidgets.QMessageBox.Discard,
    )

    qtbot.wait(100)

    assert win.tabWidget.count() == 1
    menu_shortcut(qtbot, qapp_cls_, "file", QtCore.Qt.Key_F, QtCore.Qt.Key_L)
    assert win.tabWidget.count() == 1


def test_delete_block(qtbot, qapp_cls_):
    qtbot.wait(100)
    var = find_blocks(qapp_cls_.MainWindow.currentFlowgraph, "variable")
    assert var is not None

    delete_block(qtbot, qapp_cls_, var)
    qtbot.wait(100)
    assert len(qapp_cls_.MainWindow.currentFlowgraph.blocks) == 1
    undo(qtbot, qapp_cls_)
    assert len(qapp_cls_.MainWindow.currentFlowgraph.blocks) == 2


def test_add_null_sink(qtbot, qapp_cls_):
    qtbot.wait(100)
    add_block_from_query(qtbot, qapp_cls_, "null sin")
    qtbot.wait(100)

    n_sink = find_blocks(qapp_cls_.MainWindow.currentFlowgraph, "blocks_null_sink")
    assert n_sink is not None

    delete_block(qtbot, qapp_cls_, n_sink)


def test_add_null_source(qtbot, qapp_cls_):
    qtbot.wait(100)
    add_block_from_query(qtbot, qapp_cls_, "null sou")

    n_sou = find_blocks(qapp_cls_.MainWindow.currentFlowgraph, "blocks_null_source")
    assert n_sou is not None

    delete_block(qtbot, qapp_cls_, n_sou)


def test_add_throttle(qtbot, qapp_cls_):
    qtbot.wait(100)
    add_block_from_query(qtbot, qapp_cls_, "throttle")

    throttle = find_blocks(qapp_cls_.MainWindow.currentFlowgraph, "blocks_throttle")
    assert throttle is not None

    delete_block(qtbot, qapp_cls_, throttle)


def test_right_click(qtbot, qapp_cls_):
    qtbot.wait(100)
    add_block_from_query(qtbot, qapp_cls_, "throttle")

    throttle = find_blocks(qapp_cls_.MainWindow.currentFlowgraph, "blocks_throttle")
    assert throttle is not None
    qtbot.wait(100)

    def close():
        qtbot.keyClick(throttle.gui.right_click_menu, QtCore.Qt.Key_Escape)

    QtCore.QTimer.singleShot(200, close)
    click_on(qtbot, qapp_cls_, throttle, button="right")
    qtbot.wait(100)

    delete_block(qtbot, qapp_cls_, throttle)


def test_errors(qtbot, qapp_cls_):
    menu = qapp_cls_.MainWindow.menus["build"]

    def assert_and_close():
        assert qapp_cls_.activeWindow() != qapp_cls_.MainWindow
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Escape)

    qtbot.wait(100)
    add_block_from_query(qtbot, qapp_cls_, "throttle")
    qtbot.wait(100)
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_B, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(200, assert_and_close)
    # qtbot.keyClick(menu, QtCore.Qt.Key_E) # Not necessary since it's already selected (it's the first item)
    qtbot.keyClick(menu, QtCore.Qt.Key_Enter)
    qtbot.wait(300)

    throttle = find_blocks(qapp_cls_.MainWindow.currentFlowgraph, "blocks_throttle")
    assert throttle is not None

    delete_block(qtbot, qapp_cls_, throttle)


def test_open_properties(qtbot, qapp_cls_):
    qtbot.wait(100)
    qtbot.mouseDClick(
        qapp_cls_.MainWindow.currentView.viewport(),
        QtCore.Qt.LeftButton,
        pos=qapp_cls_.MainWindow.currentView.mapFromScene(
            qapp_cls_.MainWindow.currentFlowgraph.options_block.gui.pos() +
            QtCore.QPointF(15.0, 15.0)
        ),
    )
    qtbot.wait(100)
    assert qapp_cls_.MainWindow.currentFlowgraph.options_block.gui.props_dialog.isVisible()
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Escape)
    assert (
        not qapp_cls_.MainWindow.currentFlowgraph.options_block.gui.props_dialog.isVisible()
    )


def test_change_id(qtbot, qapp_cls_):
    opts = find_blocks(qapp_cls_.MainWindow.currentFlowgraph, "options")
    assert opts.params["title"].value == "Not titled yet"
    qtbot.mouseDClick(
        qapp_cls_.MainWindow.currentView.viewport(),
        QtCore.Qt.LeftButton,
        pos=qapp_cls_.MainWindow.currentView.mapFromScene(
            opts.gui.pos() + QtCore.QPointF(15.0, 15.0)
        ),
    )
    qtbot.wait(100)
    qtbot.mouseDClick(
        opts.gui.props_dialog.edit_params[1],
        QtCore.Qt.LeftButton,
    )
    type_text(qtbot, qapp_cls_, "changed")
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Enter)
    assert opts.params["title"].value == "Not titled changed"
    qtbot.wait(100)
    undo(qtbot, qapp_cls_)
    assert opts.params["title"].value == "Not titled yet"
    redo(qtbot, qapp_cls_)
    assert opts.params["title"].value == "Not titled changed"


def test_rotate_block(qtbot, qapp_cls_):
    qtbot.wait(100)
    opts = find_blocks(qapp_cls_.MainWindow.currentFlowgraph, "options")

    # Still running into issues with what has focus depending on the order of tests run.
    # This is a problem! Tests should be able to run independently without affecting other
    # tests. Need to track this down, but for those tests that are failing for this reason,
    # something like below seems to be a workaround
    click_on(qtbot, qapp_cls_, opts)
    qtbot.wait(400)
    click_on(qtbot, qapp_cls_, opts)

    old_rotation = opts.states["rotation"]

    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Left)
    new_rotation = opts.states["rotation"]
    assert new_rotation == old_rotation - 90

    undo(qtbot, qapp_cls_)
    new_rotation = opts.states["rotation"]
    assert new_rotation == old_rotation

    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Right)
    new_rotation = opts.states["rotation"]
    assert new_rotation == old_rotation + 90

    undo(qtbot, qapp_cls_)
    new_rotation = opts.states["rotation"]
    assert new_rotation == old_rotation


def test_disable_enable(qtbot, qapp_cls_):
    qtbot.wait(100)
    var = find_blocks(qapp_cls_.MainWindow.currentFlowgraph, "variable")
    click_on(qtbot, qapp_cls_, var)

    assert var is not None
    assert var.state == "enabled"

    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_D)
    assert var.state == "disabled"

    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_E)
    assert var.state == "enabled"


def test_move_blocks(qtbot, qapp_cls_):
    fg = qapp_cls_.MainWindow.currentFlowgraph
    view = qapp_cls_.MainWindow.currentView
    scaling = qapp_cls_.MainWindow.screen().devicePixelRatio()

    qtbot.wait(100)
    add_block_from_query(qtbot, qapp_cls_, "throttle")

    throttle = find_blocks(fg, "blocks_throttle")
    variable = find_blocks(fg, "variable")
    assert throttle is not None

    click_on(qtbot, qapp_cls_, variable)
    qtbot.wait(100)

    start_throttle = scaling * global_pos(throttle.gui, view)
    start_variable = scaling * global_pos(variable.gui, view)
    pag.moveTo(start_throttle.x(), start_throttle.y())
    pag.mouseDown()

    def drag():
        for i in range(20):
            pag.move(0, scaling * 10)

    drag_t = threading.Thread(target=drag)
    drag_t.start()
    while drag_t.is_alive():
        qtbot.wait(50)
    pag.mouseUp()
    qtbot.wait(100)
    assert scaling * global_pos(throttle.gui, view) != start_throttle
    undo(qtbot, qapp_cls_)
    assert scaling * global_pos(throttle.gui, view) == start_throttle
    redo(qtbot, qapp_cls_)
    assert scaling * global_pos(throttle.gui, view) != start_throttle

    # Variable shouldn't move
    assert scaling * global_pos(variable.gui, view) == start_variable
    delete_block(qtbot, qapp_cls_, throttle)


def test_connection(qtbot, qapp_cls_):
    fg = qapp_cls_.MainWindow.currentFlowgraph
    view = qapp_cls_.MainWindow.currentView
    scaling = qapp_cls_.MainWindow.screen().devicePixelRatio()

    qtbot.wait(100)
    for block in ["null sou", "null sin"]:
        add_block_from_query(qtbot, qapp_cls_, block)

    n_src = find_blocks(fg, "blocks_null_source")
    n_sink = find_blocks(fg, "blocks_null_sink")

    assert len(fg.connections) == 0

    start = scaling * global_pos(n_sink.gui, view)
    pag.moveTo(start.x(), start.y())
    pag.mouseDown()

    def drag():
        for i in range(20):
            pag.move(scaling * 10, 0)

    drag_t = threading.Thread(target=drag)
    drag_t.start()
    while drag_t.is_alive():
        qtbot.wait(50)
    pag.mouseUp()

    click_on(qtbot, qapp_cls_, n_src.sources[0])
    click_on(qtbot, qapp_cls_, n_sink.sinks[0])
    assert len(fg.connections) == 1

    undo(qtbot, qapp_cls_)
    assert len(fg.connections) == 0

    redo(qtbot, qapp_cls_)
    assert len(fg.connections) == 1

    for block in [n_src, n_sink]:
        delete_block(qtbot, qapp_cls_, block)


def test_num_inputs(qtbot, qapp_cls_):
    fg = qapp_cls_.MainWindow.currentFlowgraph
    view = qapp_cls_.MainWindow.currentView
    scaling = qapp_cls_.MainWindow.screen().devicePixelRatio()

    qtbot.wait(100)
    for block in ["null sou", "null sin"]:
        add_block_from_query(qtbot, qapp_cls_, block)

    n_src = find_blocks(fg, "blocks_null_source")
    n_sink = find_blocks(fg, "blocks_null_sink")

    assert len(n_sink.sinks) == 1

    start = scaling * global_pos(n_sink.gui, view)
    pag.moveTo(start.x(), start.y())
    pag.mouseDown()

    def drag():
        for i in range(20):
            pag.move(scaling * 10, 0)

    drag_t = threading.Thread(target=drag)
    drag_t.start()
    while drag_t.is_alive():
        qtbot.wait(50)
    pag.mouseUp()

    click_on(qtbot, qapp_cls_, n_src.sources[0])
    click_on(qtbot, qapp_cls_, n_sink.sinks[0])
    qtbot.wait(100)

    click_pos = scaling * global_pos(n_sink.gui, view)
    pag.doubleClick(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    param_index = 0
    for i in range(len(n_sink.gui.props_dialog.edit_params)):
        if n_sink.gui.props_dialog.edit_params[i].param.key == 'num_inputs':
            param_index = i

    qtbot.mouseDClick(n_sink.gui.props_dialog.edit_params[param_index], QtCore.Qt.LeftButton)
    type_text(qtbot, qapp_cls_, "2")
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Enter)
    assert len(n_sink.sinks) == 2
    assert len(fg.connections) == 1

    click_pos = scaling * global_pos(n_sink.gui, view)
    pag.doubleClick(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    qtbot.mouseDClick(n_sink.gui.props_dialog.edit_params[param_index], QtCore.Qt.LeftButton)
    type_text(qtbot, qapp_cls_, "1")
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Enter)
    qtbot.wait(100)
    assert len(n_sink.sinks) == 1
    assert len(fg.connections) == 1

    # I think loses focus makes delete_fail the first time. This makes it work, but is a hack
    #click_on(qtbot, qapp_cls_, n_src)
    pag.click(click_pos.x() + 50, click_pos.y() + 50, button="left")

    for block in [n_src, n_sink]:
        delete_block(qtbot, qapp_cls_, block)
    qtbot.wait(100)
    assert len(fg.blocks) == 2


def test_bus(qtbot, qapp_cls_):
    fg = qapp_cls_.MainWindow.currentFlowgraph
    view = qapp_cls_.MainWindow.currentView
    scaling = qapp_cls_.MainWindow.screen().devicePixelRatio()

    qtbot.wait(100)
    add_block_from_query(qtbot, qapp_cls_, "null sin")

    n_sink = find_blocks(fg, "blocks_null_sink")

    assert len(n_sink.sinks) == 1

    click_pos = scaling * global_pos(n_sink.gui, view)
    pag.doubleClick(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    param_index = 0
    for i in range(len(n_sink.gui.props_dialog.edit_params)):
        if n_sink.gui.props_dialog.edit_params[i].param.key == 'num_inputs':
            param_index = i

    qtbot.mouseDClick(n_sink.gui.props_dialog.edit_params[param_index], QtCore.Qt.LeftButton)
    type_text(qtbot, qapp_cls_, "2")
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Enter)
    qtbot.wait(100)
    assert len(n_sink.sinks) == 2

    # Enable bus port
    qtbot.wait(100)
    more_menu = qapp_cls_.MainWindow.menus["more"]
    menu_shortcut(qtbot, qapp_cls_, "edit", QtCore.Qt.Key_E, QtCore.Qt.Key_M)
    qtbot.wait(100)
    qtbot.keyClick(more_menu, QtCore.Qt.Key_Up)
    qtbot.wait(100)
    qtbot.keyClick(more_menu, QtCore.Qt.Key_Enter)
    qtbot.wait(100)
    assert len(n_sink.sinks) == 3
    assert n_sink.sinks[2].dtype == 'bus'

    # Disable bus port
    qtbot.wait(100)
    more_menu = qapp_cls_.MainWindow.menus["more"]
    menu_shortcut(qtbot, qapp_cls_, "edit", QtCore.Qt.Key_E, QtCore.Qt.Key_M)
    qtbot.wait(100)
    qtbot.keyClick(more_menu, QtCore.Qt.Key_Up)
    qtbot.wait(100)
    qtbot.keyClick(more_menu, QtCore.Qt.Key_Enter)
    qtbot.wait(100)
    assert len(n_sink.sinks) == 2

    # Test undo
    undo(qtbot, qapp_cls_)
    qtbot.wait(100)
    assert len(n_sink.sinks) == 3
    qtbot.wait(100)
    undo(qtbot, qapp_cls_)
    qtbot.wait(100)
    assert len(n_sink.sinks) == 2

    delete_block(qtbot, qapp_cls_, n_sink)
    qtbot.wait(100)


def test_bypass(qtbot, qapp_cls_):
    scaling = qapp_cls_.MainWindow.screen().devicePixelRatio()

    fg = qapp_cls_.MainWindow.currentFlowgraph
    view = qapp_cls_.MainWindow.currentView

    qtbot.wait(100)
    for block in ["null sou", "throttle"]:
        add_block_from_query(qtbot, qapp_cls_, block)

    n_src = find_blocks(fg, "blocks_null_source")
    throttle = find_blocks(fg, "blocks_throttle")

    # Bypass the throttle block
    click_on(qtbot, qapp_cls_, throttle)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_B)
    assert throttle.state == "bypassed"
    undo(qtbot, qapp_cls_)
    assert throttle.state == "enabled"
    redo(qtbot, qapp_cls_)
    assert throttle.state == "bypassed"
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_E)
    assert throttle.state == "enabled"

    # Try to bypass the null source, this shouldn't work
    click_on(qtbot, qapp_cls_, n_src)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_B)
    assert n_src.state == "enabled"

    for block in [throttle, n_src]:
        delete_block(qtbot, qapp_cls_, block)


def test_file_save(qtbot, qapp_cls_, monkeypatch, tmp_path):
    fg_path = tmp_path / "test_save.grc"
    monkeypatch.setattr(
        QtWidgets.QFileDialog, "getSaveFileName", lambda *args, **kargs: (fg_path, "")
    )

    assert not fg_path.exists(), "File/Save (setup): File already exists"
    ctrl_keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_S)
    assert fg_path.exists(), "File/Save: Could not save file"


def test_file_save_as(qtbot, qapp_cls_, monkeypatch, tmp_path):
    fg_path = tmp_path / "test.grc"
    monkeypatch.setattr(
        QtWidgets.QFileDialog, "getSaveFileName", lambda *args, **kargs: (fg_path, "")
    )

    qtbot.wait(100)

    menu_shortcut(qtbot, qapp_cls_, "file", QtCore.Qt.Key_F, QtCore.Qt.Key_A)
    assert fg_path.exists()


def test_file_save_copy(qtbot, qapp_cls_, monkeypatch, tmp_path):
    fg_path = tmp_path / "test_copy.grc"
    monkeypatch.setattr(
        QtWidgets.QFileDialog, "getSaveFileName", lambda *args, **kargs: (fg_path, "")
    )
    qtbot.wait(100)

    assert not fg_path.exists(), "File/Save Copy (setup): File already exists"
    menu_shortcut(qtbot, qapp_cls_, "file", QtCore.Qt.Key_F, QtCore.Qt.Key_Y)
    assert fg_path.exists(), "File/Save Copy: Could not save file"


# TODO: File/Open
@pytest.mark.xfail()
def test_file_screen_capture_pdf(qtbot, qapp_cls_, monkeypatch, tmp_path):
    fg_path = tmp_path / "test.pdf"
    monkeypatch.setattr(
        QtWidgets.QFileDialog, "getSaveFileName", lambda *args, **kargs: (fg_path, "")
    )
    qtbot.wait(100)

    assert not fg_path.exists(), "File/Screen Capture (setup): PDF already exists"
    ctrl_keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_P)
    assert fg_path.exists(), "File/Screen Capture: Could not create PDF"


def test_file_screen_capture_png(qtbot, qapp_cls_, monkeypatch, tmp_path):
    fg_path = tmp_path / "test.png"
    assert not fg_path.exists()
    monkeypatch.setattr(
        QtWidgets.QFileDialog, "getSaveFileName", lambda *args, **kargs: (fg_path, "")
    )
    qtbot.wait(100)

    assert not fg_path.exists(), "File/Screen Capture (setup): PNG already exists"
    ctrl_keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_P)
    assert fg_path.exists(), "File/Screen Capture: Could not create PNG"


def test_file_screen_capture_svg(qtbot, qapp_cls_, monkeypatch, tmp_path):
    fg_path = tmp_path / "test.svg"
    assert not fg_path.exists()
    monkeypatch.setattr(
        QtWidgets.QFileDialog, "getSaveFileName", lambda *args, **kargs: (fg_path, "")
    )
    qtbot.wait(100)

    assert not fg_path.exists(), "File/Screen Capture (setup): SVG already exists"
    ctrl_keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_P)
    assert fg_path.exists(), "File/Screen Capture: Could not create SVG"


def test_file_preferences(qtbot, qapp_cls_):
    menu = qapp_cls_.MainWindow.menus["file"]
    items = gather_menu_items(menu)

    def assert_and_close():
        assert qapp_cls_.activeWindow() != qapp_cls_.MainWindow
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)

    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(200, assert_and_close)
    qtbot.keyClick(menu, QtCore.Qt.Key_F)
    qtbot.wait(600)
    assert qapp_cls_.activeWindow() == qapp_cls_.MainWindow
    qtbot.wait(100)


def test_file_examples(qtbot, qapp_cls_):
    menu = qapp_cls_.MainWindow.menus["file"]
    items = gather_menu_items(menu)

    def assert_and_close():
        assert qapp_cls_.activeWindow() != qapp_cls_.MainWindow
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Escape)

    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(200, assert_and_close)
    qtbot.keyClick(menu, QtCore.Qt.Key_E)
    qtbot.wait(600)
    assert qapp_cls_.activeWindow() == qapp_cls_.MainWindow
    qtbot.wait(100)


def test_edit_actions(qtbot, qapp_cls_):
    pass


def test_edit_select_all(qtbot, qapp_cls_):
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_A, QtCore.Qt.ControlModifier)
    qtbot.wait(100)


def test_edit_cut_paste(qtbot, qapp_cls_):
    fg = qapp_cls_.MainWindow.currentFlowgraph

    qtbot.wait(100)
    var = find_blocks(fg, "variable")
    assert var is not None, "Edit/Cut and paste (setup): Could not find variable block"

    click_on(qtbot, qapp_cls_, var)
    ctrl_keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_X)
    qtbot.wait(100)

    var = find_blocks(fg, "variable")
    assert var is None, "Edit/Cut and paste: Could not cut variable block"

    ctrl_keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_V)
    qtbot.wait(100)

    var = find_blocks(fg, "variable")
    assert var is not None, "Edit/Cut and paste: Could not paste variable block"

    qtbot.wait(100)


def test_edit_copy_paste(qtbot, qapp_cls_):
    fg = qapp_cls_.MainWindow.currentFlowgraph

    qtbot.wait(100)
    var = find_blocks(fg, "variable")
    assert var is not None, "Edit/Copy and paste (setup): Could not find variable block"

    click_on(qtbot, qapp_cls_, var)
    ctrl_keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_C)
    qtbot.wait(100)
    ctrl_keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_V)

    vars = find_blocks(fg, "variable")
    assert isinstance(vars, list), "Edit/Copy and paste: Could not paste variable block"
    assert len(vars) == 2, "Edit/Copy and paste: Could not paste variable block"
    assert (
        vars[0].name != vars[1].name
    ), "Edit/Copy and paste: Newly pasted variable block's ID is the same as the original block's ID"

    delete_block(qtbot, qapp_cls_, vars[1])


def test_view_actions(qtbot, qapp_cls_):
    pass


def test_build_actions(qtbot, qapp_cls_):
    pass


def test_tools_actions(qtbot, qapp_cls_):
    pass


def test_tools_oot_browser(qtbot, qapp_cls_):
    menu = qapp_cls_.MainWindow.menus["tools"]
    items = gather_menu_items(menu)

    def assert_open():
        assert qapp_cls_.activeWindow() != qapp_cls_.MainWindow

    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_T, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, assert_open)
    qtbot.keyClick(menu, QtCore.Qt.Key_O)
    qtbot.wait(200)
    qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Escape)
    qtbot.wait(200)


def test_reports_actions(qtbot, qapp_cls_):
    pass


def test_help_windows(qtbot, qapp_cls_):
    def assert_and_close():
        assert qapp_cls_.activeWindow() != qapp_cls_.MainWindow
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)

    def test_help_window(menu_key):
        qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_H, QtCore.Qt.AltModifier)
        qtbot.wait(300)
        QtCore.QTimer.singleShot(200, assert_and_close)
        qtbot.keyClick(menu, menu_key)
        qtbot.wait(600)
        assert qapp_cls_.activeWindow() == qapp_cls_.MainWindow
        qtbot.wait(100)

    menu = qapp_cls_.MainWindow.menus["help"]
    qtbot.wait(100)

    for key in [
        QtCore.Qt.Key_H,
        QtCore.Qt.Key_T,
        QtCore.Qt.Key_K,
        QtCore.Qt.Key_G,
        QtCore.Qt.Key_A,
        QtCore.Qt.Key_Q,
    ]:
        test_help_window(key)


def test_file_new_close(qtbot, qapp_cls_, monkeypatch):
    win = qapp_cls_.MainWindow
    monkeypatch.setattr(
        QtWidgets.QMessageBox,
        "question",
        lambda *args: QtWidgets.QMessageBox.Discard,
    )
    qtbot.wait(100)

    menu_shortcut(qtbot, qapp_cls_, "file", QtCore.Qt.Key_F, QtCore.Qt.Key_N)
    assert win.tabWidget.count() == 2, "File/New"

    for i in range(3, 5):
        ctrl_keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_N)
        assert win.tabWidget.count() == i, "File/New"

    for i in range(1, 4):
        ctrl_keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_W)
        assert win.tabWidget.count() == 4 - i, "File/Close"


def test_generate(qtbot, qapp_cls_, monkeypatch, tmp_path):
    fg = qapp_cls_.MainWindow.currentFlowgraph
    view = qapp_cls_.MainWindow.currentView
    scaling = qapp_cls_.MainWindow.screen().devicePixelRatio()
    fg_path = tmp_path / "test_generate.grc"
    py_path = tmp_path / "default.py"
    monkeypatch.setattr(
        QtWidgets.QFileDialog, "getSaveFileName", lambda *args, **kargs: (fg_path, "")
    )

    qtbot.wait(100)
    for block in ["null sou", "null sin"]:
        add_block_from_query(qtbot, qapp_cls_, block)

    n_src = find_blocks(fg, "blocks_null_source")
    n_sink = find_blocks(fg, "blocks_null_sink")

    assert len(fg.connections) == 0

    start = scaling * global_pos(n_sink.gui, view)
    pag.moveTo(start.x(), start.y())
    pag.mouseDown()

    def drag():
        for i in range(20):
            pag.move(scaling * 10, 0)

    drag_t = threading.Thread(target=drag)
    drag_t.start()
    while drag_t.is_alive():
        qtbot.wait(50)
    pag.mouseUp()

    click_on(qtbot, qapp_cls_, n_src.sources[0])
    click_on(qtbot, qapp_cls_, n_sink.sinks[0])
    assert not fg_path.exists(), "File/Save (setup): .grc file already exists"
    assert not py_path.exists(), "File/Save (setup): Python file already exists"
    menu_shortcut(qtbot, qapp_cls_, "build", QtCore.Qt.Key_B, QtCore.Qt.Key_G)
    qtbot.wait(500)
    assert fg_path.exists(), "File/Save: Could not save .grc file"
    assert py_path.exists(), "File/Save: Could not save Python file"


def test_file_close_all(qtbot, qapp_cls_, monkeypatch):
    win = qapp_cls_.MainWindow
    monkeypatch.setattr(
        QtWidgets.QMessageBox,
        "question",
        lambda *args: QtWidgets.QMessageBox.Discard,
    )

    qtbot.wait(100)

    for i in range(1, 4):
        ctrl_keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_N)

    assert win.tabWidget.count() == 4, "File/Close All"
    menu_shortcut(qtbot, qapp_cls_, "file", QtCore.Qt.Key_F, QtCore.Qt.Key_L)
    assert win.tabWidget.count() == 1, "File/Close All"


def test_quit(qtbot, qapp_cls_, monkeypatch):
    monkeypatch.setattr(
        QtWidgets.QMessageBox,
        "question",
        lambda *args: QtWidgets.QMessageBox.Discard,
    )
    qapp_cls_.MainWindow.actions["exit"].trigger()
    assert True
    time.sleep(1)

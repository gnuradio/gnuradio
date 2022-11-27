import pytest
import gettext
import locale
import threading

from pytestqt.plugin import qapp

import time

import pyautogui as pag

import logging

from PyQt5 import QtTest, QtCore, Qt, QtGui
from os import path

from gnuradio import gr
from grc.gui_qt import properties
from grc.gui_qt.grc import Application
from grc.gui_qt.components.window import MainWindow
from grc.gui_qt.Platform import Platform

log = logging.getLogger('grc')

@pytest.fixture(scope="session")
def qapp_cls_():
    settings = properties.Properties([])

    ''' Translation Support '''
    # Try to get the current locale. Always add English
    lc, encoding = locale.getdefaultlocale()
    if lc:
        languages = [lc]
    languages += settings.DEFAULT_LANGUAGE
    log.debug("Using locale - %s" % str(languages))

    # Still run even if the english translation isn't found
    language = gettext.translation(settings.APP_NAME, settings.path.LANGUAGE, languages=languages,
                                   fallback=True)
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
        install_prefix=gr.prefix()
    )
    model.build_library()
    app = Application(settings, model)
    app.MainWindow.showMaximized()
    return app

def type_text(qtbot, app, keys):
    for key in keys:
        # Each sequence contains a single key.
        # That's why we use the first element
        keycode = QtGui.QKeySequence(key)[0]
        qtbot.keyClick(app.focusWidget(), keycode, QtCore.Qt.NoModifier)

def keystroke(qtbot, app, key):
    qtbot.keyClick(app.focusWidget(), key, QtCore.Qt.NoModifier)

def gather_menu_items(menu):
    ret = {}
    for act in menu.actions():
        ret[act.text()] = act
    return ret

def test_add_null_sink(qtbot, qapp_cls_):
    qtbot.wait(100)
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.ControlModifier)
    type_text(qtbot, qapp_cls_, "null sin")
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_Enter, QtCore.Qt.NoModifier)
    qtbot.wait(100)
    assert 'blocks_null_sink' in [block.key for block in qapp_cls_.MainWindow.currentFlowgraph.blocks]

def test_add_null_source(qtbot, qapp_cls_):
    qtbot.wait(100)
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.ControlModifier)
    type_text(qtbot, qapp_cls_, "null sourc")
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Enter)
    qtbot.wait(100)
    assert 'blocks_null_source' in [block.key for block in qapp_cls_.MainWindow.currentFlowgraph.blocks]

def test_add_throttle(qtbot, qapp_cls_):
    qtbot.wait(100)
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.ControlModifier)
    type_text(qtbot, qapp_cls_, "throttl")
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Enter)
    qtbot.wait(100)
    assert 'blocks_throttle' in [block.key for block in qapp_cls_.MainWindow.currentFlowgraph.blocks]

def test_open_properties(qtbot, qapp_cls_):
    qtbot.wait(100)
    qtbot.mouseDClick(
            qapp_cls_.MainWindow.currentView.viewport(), 
            QtCore.Qt.LeftButton, 
            pos=qapp_cls_.MainWindow.currentView.mapFromScene(qapp_cls_.MainWindow.currentFlowgraph.options_block.pos()+QtCore.QPoint(15,15)))
    qtbot.wait(100)
    assert qapp_cls_.MainWindow.currentFlowgraph.options_block.props_dialog.isVisible()

def test_change_id(qtbot, qapp_cls_):
    qtbot.mouseDClick(qapp_cls_.MainWindow.currentFlowgraph.options_block.props_dialog.edit_params[1],
            QtCore.Qt.LeftButton)
    type_text(qtbot, qapp_cls_, "changed")
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Enter)
    qtbot.wait(100)
    val = qapp_cls_.MainWindow.currentFlowgraph.options_block.params['title'].value
    assert val == "Not titled changed"

def test_rotate_block(qtbot, qapp_cls_):
    scaling = qapp_cls_.desktop().devicePixelRatio()
    
    block = qapp_cls_.MainWindow.currentFlowgraph.options_block
    click_pos = scaling*qapp_cls_.MainWindow.currentView.mapToGlobal(block.center().toPoint())
    pag.click(click_pos.x(), click_pos.y(), button="left")
    old_rotation = block.states['rotation']
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Left)
    qtbot.wait(100)
    new_rotation = block.states['rotation']
    assert new_rotation == old_rotation - 90
'''
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Right)
    qtbot.wait(500)
    new_rotation = block.states['rotation']
    assert new_rotation == old_rotation
'''

def test_disable_enable(qtbot, qapp_cls_):
    vp = qapp_cls_.MainWindow.currentView.viewport()
    scaling = qapp_cls_.desktop().devicePixelRatio()

    block = None
    for block_ in qapp_cls_.MainWindow.currentFlowgraph.blocks:
        if block_.key == 'variable':
            block = block_

    click_pos = scaling*qapp_cls_.MainWindow.currentView.mapToGlobal(block.center().toPoint())
    pag.click(click_pos.x(), click_pos.y(), button="left")
    assert block != None
    assert block.state == 'enabled'
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_D)
    qtbot.wait(100)
    assert block.state == 'disabled'
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_E)
    qtbot.wait(100)
    assert block.state == 'enabled'

def test_move_blocks(qtbot, qapp_cls_):
    fg = qapp_cls_.MainWindow.currentFlowgraph
    vp = qapp_cls_.MainWindow.currentView.viewport()
    scaling = qapp_cls_.desktop().devicePixelRatio()
    
    throttle = None
    n_src = None
    n_sink = None
    for block in fg.blocks:
        if block.key == 'blocks_throttle':
            throttle = block
        if block.key == 'blocks_null_source':
            n_src = block
        if block.key == 'blocks_null_sink':
            n_sink = block

    start = scaling*qapp_cls_.MainWindow.currentView.mapToGlobal(throttle.center().toPoint())
    pag.moveTo(start.x(), start.y())
    pag.mouseDown()
    def drag():
        for i in range(20):
            pag.move(0, 10)
    drag_t = threading.Thread(target=drag)
    drag_t.start()
    while drag_t.is_alive():
        qtbot.wait(100)
    pag.mouseUp()

    start = scaling*qapp_cls_.MainWindow.currentView.mapToGlobal(n_src.center().toPoint())
    pag.moveTo(start.x(), start.y())
    pag.mouseDown()
    def drag():
        for i in range(20):
            pag.move(-15, 0)
    drag_t = threading.Thread(target=drag)
    drag_t.start()
    while drag_t.is_alive():
        qtbot.wait(100)
    pag.mouseUp()

    assert True

def test_create_connection(qtbot, qapp_cls_):
    fg = qapp_cls_.MainWindow.currentFlowgraph
    vp = qapp_cls_.MainWindow.currentView.viewport()
    scaling = qapp_cls_.desktop().devicePixelRatio()

    throttle = None
    n_src = None
    n_sink = None
    for block in fg.blocks:
        if block.key == 'blocks_throttle':
            throttle = block
        if block.key == 'blocks_null_source':
            n_src = block
        if block.key == 'blocks_null_sink':
            n_sink = block
    
    assert len(fg.connections) == 0

    click_pos = scaling * qapp_cls_.MainWindow.currentView.mapToGlobal(n_src.mapToScene(n_src.sources[0].center()).toPoint())
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    click_pos = scaling * qapp_cls_.MainWindow.currentView.mapToGlobal(n_sink.mapToScene(n_sink.sinks[0].center()).toPoint())
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)

    qtbot.wait(50)
    assert len(fg.connections) > 0

def test_num_inputs(qtbot, qapp_cls_):
    fg = qapp_cls_.MainWindow.currentFlowgraph
    vp = qapp_cls_.MainWindow.currentView.viewport()
    scaling = qapp_cls_.desktop().devicePixelRatio()

    n_sink = None
    for block in fg.blocks:
        
        if block.key == 'blocks_null_sink':
            n_sink = block

    assert len(n_sink.sinks) == 1
    assert len(fg.connections) == 1
    click_pos = scaling * qapp_cls_.MainWindow.currentView.mapToGlobal(n_sink.center().toPoint())
    pag.doubleClick(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    qtbot.mouseDClick(n_sink.props_dialog.edit_params[2],
            QtCore.Qt.LeftButton)
    type_text(qtbot, qapp_cls_, "2")
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Enter)
    qtbot.wait(100)
    assert len(n_sink.sinks) == 2
    assert len(fg.connections) == 1
    click_pos = scaling * qapp_cls_.MainWindow.currentView.mapToGlobal(n_sink.center().toPoint())
    pag.doubleClick(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    qtbot.mouseDClick(n_sink.props_dialog.edit_params[2],
            QtCore.Qt.LeftButton)
    type_text(qtbot, qapp_cls_, "1")
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_Enter)
    qtbot.wait(100)
    assert len(n_sink.sinks) == 1
    assert len(fg.connections) == 1

def test_bypass(qtbot, qapp_cls_):
    scaling = qapp_cls_.desktop().devicePixelRatio()

    fg = qapp_cls_.MainWindow.currentFlowgraph
    vp = qapp_cls_.MainWindow.currentView.viewport()
    throttle = None
    n_src = None
    n_sink = None
    for block in fg.blocks:
        if block.key == 'blocks_throttle':
            throttle = block
        if block.key == 'blocks_null_source':
            n_src = block
        if block.key == 'blocks_null_sink':
            n_sink = block
    # Bypass the throttle block
    click_pos = scaling * qapp_cls_.MainWindow.currentView.mapToGlobal(throttle.center().toPoint())
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_B)
    qtbot.wait(100)
    assert throttle.state == 'bypassed'
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_E)
    assert throttle.state == 'enabled'
    # Try to bypass the null source, this shouldn't work
    click_pos = scaling * qapp_cls_.MainWindow.currentView.mapToGlobal(n_src.center().toPoint())
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_B)
    qtbot.wait(100)
    assert n_src.state == 'enabled'

def test_file_actions(qtbot, qapp_cls_):
    win = qapp_cls_.MainWindow
    menu = qapp_cls_.MainWindow.menus['file']
    items = gather_menu_items(menu)
    qtbot.wait(100)
    
    # New
    assert win.tabWidget.count() == 1, "File/New"
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    qtbot.keyClick(menu, QtCore.Qt.Key_N)
    qtbot.wait(100)
    assert win.tabWidget.count() == 2, "File/New"
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    qtbot.keyClick(menu, QtCore.Qt.Key_N)
    qtbot.wait(100)
    assert win.tabWidget.count() == 3, "File/New"
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    qtbot.keyClick(menu, QtCore.Qt.Key_N)
    qtbot.wait(100)
    assert win.tabWidget.count() == 4, "File/New"

    # Open
    # TODO

    # Close
    assert win.tabWidget.count() == 4, "File/Close"
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    qtbot.keyClick(menu, QtCore.Qt.Key_C)
    qtbot.wait(100)
    assert win.tabWidget.count() == 3, "File/Close"
    # Close All
    assert win.tabWidget.count() == 3, "File/Close All"
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    qtbot.keyClick(menu, QtCore.Qt.Key_L)
    qtbot.wait(100)
    assert win.tabWidget.count() == 1, "File/Close All"
    # Save
    # Save As
    # Screen Capture
    
    # Print
    # Exit

def test_edit_actions(qtbot, qapp_cls_):
    pass

def test_view_actions(qtbot, qapp_cls_):
    pass

def test_build_actions(qtbot, qapp_cls_):
    pass

def test_tools_actions(qtbot, qapp_cls_):
    pass

def test_reports_actions(qtbot, qapp_cls_):
    pass

def test_help_actions(qtbot, qapp_cls_):
    def close_modal_dialog():
        assert(qapp_cls_.activeWindow() != qapp_cls_.MainWindow)
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)

    menu = qapp_cls_.MainWindow.menus['help']
    items = gather_menu_items(menu)
    qtbot.wait(100)

    # Help
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)
    window_count = len(qapp_cls_.topLevelWindows())
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_H, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, close_modal_dialog)
    qtbot.keyClick(menu, QtCore.Qt.Key_H)
    qtbot.wait(100)
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)
    
    # TODO
    # Types
    # Parser Errors
    # Get Involved
    
    # About
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)
    window_count = len(qapp_cls_.topLevelWindows())
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_H, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, close_modal_dialog)
    qtbot.keyClick(menu, QtCore.Qt.Key_A)
    qtbot.wait(100)
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)
    
    # About Qt
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)
    window_count = len(qapp_cls_.topLevelWindows())
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_H, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, close_modal_dialog)
    qtbot.keyClick(menu, QtCore.Qt.Key_Q)
    qtbot.wait(100)
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)

def test_quit(qtbot, qapp_cls_):
    qapp_cls_.MainWindow.actions["exit"].trigger()
    assert True
    time.sleep(1)


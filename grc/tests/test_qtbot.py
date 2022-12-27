import pytest
import gettext
import locale
import threading

from pytestqt.plugin import qapp

import time

import pyautogui as pag

import logging

from PyQt5 import QtTest, QtCore, Qt, QtGui
from os import path, remove

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

def global_pos(block, view):
    scene_pos = block.mapToScene(block.boundingRect().center())
    global_pos = view.viewport().mapToGlobal(view.mapFromScene(scene_pos))
    return global_pos

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
    view = qapp_cls_.MainWindow.currentView
    scaling = qapp_cls_.desktop().devicePixelRatio()

    block = None
    for block_ in qapp_cls_.MainWindow.currentFlowgraph.blocks:
        if block_.key == 'variable':
            block = block_

    click_pos = scaling * global_pos(block, view)
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
    view = qapp_cls_.MainWindow.currentView
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

    start = scaling * global_pos(throttle, view)
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
    assert(scaling * global_pos(throttle, view) != start)

    start = scaling * global_pos(n_src, view)
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
    assert(scaling * global_pos(n_src, view) != start)

def test_create_connection(qtbot, qapp_cls_):
    fg = qapp_cls_.MainWindow.currentFlowgraph
    view = qapp_cls_.MainWindow.currentView
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

    click_pos = scaling * global_pos(n_src.sources[0], view)
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    click_pos = scaling * global_pos(n_sink.sinks[0], view)
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)

    qtbot.wait(50)
    assert len(fg.connections) > 0

def test_num_inputs(qtbot, qapp_cls_):
    fg = qapp_cls_.MainWindow.currentFlowgraph
    view = qapp_cls_.MainWindow.currentView
    scaling = qapp_cls_.desktop().devicePixelRatio()

    n_sink = None
    for block in fg.blocks:
        
        if block.key == 'blocks_null_sink':
            n_sink = block

    assert len(n_sink.sinks) == 1
    assert len(fg.connections) == 1
    click_pos = scaling * global_pos(n_sink, view)
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
    click_pos = scaling * global_pos(n_sink, view)
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
    view = qapp_cls_.MainWindow.currentView
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
    click_pos = scaling * global_pos(throttle, view)
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_B)
    qtbot.wait(100)
    assert throttle.state == 'bypassed'
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_E)
    assert throttle.state == 'enabled'
    # Try to bypass the null source, this shouldn't work
    click_pos = scaling * global_pos(n_src, view)
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    keystroke(qtbot, qapp_cls_, QtCore.Qt.Key_B)
    qtbot.wait(100)
    assert n_src.state == 'enabled'

def test_file_new(qtbot, qapp_cls_):
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

def test_file_close(qtbot, qapp_cls_):
    win = qapp_cls_.MainWindow
    menu = qapp_cls_.MainWindow.menus['file']
    items = gather_menu_items(menu)
    qtbot.wait(100)

    # Close
    assert win.tabWidget.count() == 4, "File/Close"
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    qtbot.keyClick(menu, QtCore.Qt.Key_C)
    qtbot.wait(100)
    assert win.tabWidget.count() == 3, "File/Close"

def test_file_close_all(qtbot, qapp_cls_):
    win = qapp_cls_.MainWindow
    menu = qapp_cls_.MainWindow.menus['file']
    items = gather_menu_items(menu)
    qtbot.wait(100)

    # Close All
    assert win.tabWidget.count() == 3, "File/Close All"
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    qtbot.keyClick(menu, QtCore.Qt.Key_L)
    qtbot.wait(100)
    assert win.tabWidget.count() == 1, "File/Close All"

def test_file_save_as(qtbot, qapp_cls_):
    win = qapp_cls_.MainWindow
    menu = qapp_cls_.MainWindow.menus['file']
    items = gather_menu_items(menu)
    qtbot.wait(100)
    
    def assert_and_close():
        assert(qapp_cls_.activeWindow() != qapp_cls_.MainWindow)
        type_text(qtbot, qapp_cls_, "test.grc")
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)
    
    assert (not path.isfile('test.grc')), "File/Save As (setup): File already exists"
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, assert_and_close)
    qtbot.keyClick(menu, QtCore.Qt.Key_A)
    qtbot.wait(200)
    assert (path.isfile('test.grc')), "File/Save As: Could not save file"
    remove('test.grc')
    assert (not path.isfile('test.grc')), "File/Save (teardown): Could not delete file"

def test_file_save(qtbot, qapp_cls_):
    win = qapp_cls_.MainWindow
    menu = qapp_cls_.MainWindow.menus['file']
    items = gather_menu_items(menu)
    qtbot.wait(100)
    
    assert (not path.isfile('test.grc')), "File/Save (setup): File already exists"
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_S, QtCore.Qt.ControlModifier)
    qtbot.wait(100)
    assert (path.isfile('test.grc')), "File/Save: Could not save file"
    remove('test.grc')
    assert (not path.isfile('test.grc')), "File/Save (teardown): Could not delete file"

def test_file_save_copy(qtbot, qapp_cls_):
    win = qapp_cls_.MainWindow
    menu = qapp_cls_.MainWindow.menus['file']
    items = gather_menu_items(menu)
    qtbot.wait(100)
    
    def assert_and_close():
        assert (qapp_cls_.activeWindow() != qapp_cls_.MainWindow), "File/Save Copy (setup): Could not create screen capture dialog"
        type_text(qtbot, qapp_cls_, "test_copy.grc")
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)

    assert (not path.isfile('test_copy.grc')), "File/Save Copy (setup): File already exists"
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, assert_and_close)
    qtbot.keyClick(menu, QtCore.Qt.Key_Y)
    qtbot.wait(200)
    assert path.isfile('test_copy.grc'), "File/Save Copy: Could not save file"
    remove('test_copy.grc')
    assert (not path.isfile('test_copy.grc')), "File/Save Copy (teardown): Could not delete file"

# TODO: File/Open
@pytest.mark.xfail()
def test_file_screen_capture_pdf(qtbot, qapp_cls_):
    def assert_and_close():
        assert (qapp_cls_.activeWindow() != qapp_cls_.MainWindow), "File/Screen Capture (setup): Could not create screen capture dialog"
        type_text(qtbot, qapp_cls_, "test.pdf")
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)

    assert (not path.isfile('test.pdf')), "File/Screen Capture (setup): PDF already exists"
    QtCore.QTimer.singleShot(100, assert_and_close)
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_P, QtCore.Qt.ControlModifier)
    qtbot.wait(200)
    assert path.isfile('test.pdf'), "File/Screen Capture: Could not create PDF"
    remove('test.pdf')
    assert (not path.isfile('test.pdf')), "File/Screen Capture (teardown): Could not delete PDF"
    

def test_file_screen_capture_png(qtbot, qapp_cls_):
    assert(not path.isfile('test.png'))

    def assert_and_close():
        assert(qapp_cls_.activeWindow() != qapp_cls_.MainWindow)
        type_text(qtbot, qapp_cls_, "test.png")
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)

    QtCore.QTimer.singleShot(100, assert_and_close)
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_P, QtCore.Qt.ControlModifier)
    qtbot.wait(200)
    assert path.isfile('test.png'), "File/Screen Capture: Could not create PNG"
    remove('test.png')
    assert(not path.isfile('test.png'))

def test_file_screen_capture_svg(qtbot, qapp_cls_):
    assert(not path.isfile('test.svg'))

    def assert_and_close():
        assert(qapp_cls_.activeWindow() != qapp_cls_.MainWindow)
        type_text(qtbot, qapp_cls_, "test.svg")
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)
    
    QtCore.QTimer.singleShot(100, assert_and_close)
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_P, QtCore.Qt.ControlModifier)
    qtbot.wait(200)
    assert path.isfile('test.svg'), "File/Screen Capture: Could not create SVG"
    remove('test.svg')
    assert(not path.isfile('test.svg'))

def test_edit_actions(qtbot, qapp_cls_):
    pass

def test_edit_cut_paste(qtbot, qapp_cls_):
    scaling = qapp_cls_.desktop().devicePixelRatio()

    fg = qapp_cls_.MainWindow.currentFlowgraph
    view = qapp_cls_.MainWindow.currentView
    block = None
    for block_ in qapp_cls_.MainWindow.currentFlowgraph.blocks:
        if block_.key == 'variable':
            block = block_

    assert block != None, "Edit/Cut and paste (setup): Could not find variable block"

    click_pos = scaling * global_pos(block, view)
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_X, QtCore.Qt.ControlModifier)
    qtbot.wait(100)

    block = None
    for block_ in qapp_cls_.MainWindow.currentFlowgraph.blocks:
        if block_.key == 'variable':
            block = block_
    assert block not in qapp_cls_.MainWindow.currentFlowgraph.blocks, "Edit/Cut and paste: Could not cut variable block"

    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_V, QtCore.Qt.ControlModifier)
    qtbot.wait(100)

    block = None
    for block_ in qapp_cls_.MainWindow.currentFlowgraph.blocks:
        if block_.key == 'variable':
            block = block_
    assert block in qapp_cls_.MainWindow.currentFlowgraph.blocks, "Edit/Cut and paste: Could not paste variable block"

    qtbot.wait(100)

def test_edit_copy_paste(qtbot, qapp_cls_):
    scaling = qapp_cls_.desktop().devicePixelRatio()

    fg = qapp_cls_.MainWindow.currentFlowgraph
    view = qapp_cls_.MainWindow.currentView
    block = None
    for block_ in qapp_cls_.MainWindow.currentFlowgraph.blocks:
        if block_.key == 'variable':
            block = block_

    assert block != None, "Edit/Copy and paste (setup): Could not find variable block"

    click_pos = scaling * global_pos(block, view)
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_C, QtCore.Qt.ControlModifier)
    qtbot.wait(100)
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_V, QtCore.Qt.ControlModifier)
    qtbot.wait(100)

    blocks = []
    for block_ in qapp_cls_.MainWindow.currentFlowgraph.blocks:
        if block_.key == 'variable':
            blocks.append(block_)
    assert len(blocks) == 2, "Edit/Copy and paste: Could not paste variable block"
    assert blocks[0].name != blocks[1].name, "Edit/Copy and paste: Newly pasted variable block's ID is the same as the original block's ID"

    click_pos = scaling * global_pos(blocks[1], view)
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(100)
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_Delete, QtCore.Qt.NoModifier)
    
    assert len(blocks) == 2, "Edit/Copy and paste (teardown): Could not delete new variable block"
    
def test_view_actions(qtbot, qapp_cls_):
    pass

def test_build_actions(qtbot, qapp_cls_):
    pass

def test_tools_actions(qtbot, qapp_cls_):
    pass

@pytest.mark.xfail()
def test_tools_oot_browser(qtbot, qapp_cls_):
    menu = qapp_cls_.MainWindow.menus['tools']
    items = gather_menu_items(menu)

    def assert_open():
        assert(qapp_cls_.activeWindow() != qapp_cls_.MainWindow)

    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_T, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, assert_open)
    qtbot.keyClick(menu, QtCore.Qt.Key_O)
    qtbot.wait(200)

def test_reports_actions(qtbot, qapp_cls_):
    pass

def test_help_help(qtbot, qapp_cls_):
    def assert_and_close():
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
    QtCore.QTimer.singleShot(100, assert_and_close)
    qtbot.keyClick(menu, QtCore.Qt.Key_H)
    qtbot.wait(200)
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)
    
def test_help_types(qtbot, qapp_cls_):
    def assert_and_close():
        assert(qapp_cls_.activeWindow() != qapp_cls_.MainWindow)
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)

    menu = qapp_cls_.MainWindow.menus['help']
    items = gather_menu_items(menu)
    qtbot.wait(100)

    # Types
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)
    window_count = len(qapp_cls_.topLevelWindows())
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_H, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, assert_and_close)
    qtbot.keyClick(menu, QtCore.Qt.Key_T)
    qtbot.wait(200)
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)

def test_help_keys(qtbot, qapp_cls_):
    def assert_and_close():
        assert(qapp_cls_.activeWindow() != qapp_cls_.MainWindow)
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)

    menu = qapp_cls_.MainWindow.menus['help']
    items = gather_menu_items(menu)
    qtbot.wait(100)

    # Keys
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)
    window_count = len(qapp_cls_.topLevelWindows())
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_H, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, assert_and_close)
    qtbot.keyClick(menu, QtCore.Qt.Key_K)
    qtbot.wait(200)
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)

    # TODO:
    # Parser Errors

def test_help_get_involved(qtbot, qapp_cls_):
    def assert_and_close():
        assert(qapp_cls_.activeWindow() != qapp_cls_.MainWindow)
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)

    menu = qapp_cls_.MainWindow.menus['help']
    items = gather_menu_items(menu)
    qtbot.wait(100)

    # Get Involved
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)
    window_count = len(qapp_cls_.topLevelWindows())
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_H, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, assert_and_close)
    qtbot.keyClick(menu, QtCore.Qt.Key_G)
    qtbot.wait(200)
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)

def test_help_about(qtbot, qapp_cls_):
    def assert_and_close():
        assert(qapp_cls_.activeWindow() != qapp_cls_.MainWindow)
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)

    menu = qapp_cls_.MainWindow.menus['help']
    items = gather_menu_items(menu)
    qtbot.wait(100)
    
    # About
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)
    window_count = len(qapp_cls_.topLevelWindows())
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_H, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, assert_and_close)
    qtbot.keyClick(menu, QtCore.Qt.Key_A)
    qtbot.wait(200)
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)

def test_help_about_qt(qtbot, qapp_cls_):
    def assert_and_close():
        assert(qapp_cls_.activeWindow() != qapp_cls_.MainWindow)
        qtbot.keyClick(qapp_cls_.activeWindow(), QtCore.Qt.Key_Enter)

    menu = qapp_cls_.MainWindow.menus['help']
    items = gather_menu_items(menu)
    qtbot.wait(100)
    
    # About Qt
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)
    window_count = len(qapp_cls_.topLevelWindows())
    qtbot.keyClick(qapp_cls_.focusWidget(), QtCore.Qt.Key_H, QtCore.Qt.AltModifier)
    qtbot.wait(100)
    QtCore.QTimer.singleShot(100, assert_and_close)
    qtbot.keyClick(menu, QtCore.Qt.Key_Q)
    qtbot.wait(200)
    assert(qapp_cls_.activeWindow() == qapp_cls_.MainWindow)

def test_quit(qtbot, qapp_cls_):
    qapp_cls_.MainWindow.actions["exit"].trigger()
    assert True
    time.sleep(1)


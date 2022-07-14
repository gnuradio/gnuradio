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
def qapp_cls():
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

def test_add_null_sink(qtbot, qapp_cls):
    qtbot.wait(100)
    qtbot.keyClick(qapp_cls.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.ControlModifier)
    type_text(qtbot, qapp_cls, "null sin")
    qtbot.keyClick(qapp_cls.focusWidget(), QtCore.Qt.Key_Enter, QtCore.Qt.NoModifier)
    qtbot.wait(100)
    assert 'blocks_null_sink' in [block.key for block in qapp_cls.MainWindow.currentFlowgraph.blocks]

def test_add_null_source(qtbot, qapp_cls):
    qtbot.wait(100)
    qtbot.keyClick(qapp_cls.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.ControlModifier)
    type_text(qtbot, qapp_cls, "null sourc")
    keystroke(qtbot, qapp_cls, QtCore.Qt.Key_Enter)
    qtbot.wait(100)
    assert 'blocks_null_source' in [block.key for block in qapp_cls.MainWindow.currentFlowgraph.blocks]

def test_add_throttle(qtbot, qapp_cls):
    qtbot.wait(100)
    qtbot.keyClick(qapp_cls.focusWidget(), QtCore.Qt.Key_F, QtCore.Qt.ControlModifier)
    type_text(qtbot, qapp_cls, "throttl")
    keystroke(qtbot, qapp_cls, QtCore.Qt.Key_Enter)
    qtbot.wait(100)
    assert 'blocks_throttle' in [block.key for block in qapp_cls.MainWindow.currentFlowgraph.blocks]

def test_open_properties(qtbot, qapp_cls):
    qtbot.wait(500)
    qtbot.mouseDClick(
            qapp_cls.MainWindow.currentView.viewport(), 
            QtCore.Qt.LeftButton, 
            pos=qapp_cls.MainWindow.currentView.mapFromScene(qapp_cls.MainWindow.currentFlowgraph.options_block.pos()+QtCore.QPoint(15,15)))
    qtbot.wait(500)
    assert qapp_cls.MainWindow.currentFlowgraph.options_block.props_dialog.isVisible()

def test_change_id(qtbot, qapp_cls):
    qtbot.mouseDClick(qapp_cls.MainWindow.currentFlowgraph.options_block.props_dialog.edit_params[1],
            QtCore.Qt.LeftButton)
    type_text(qtbot, qapp_cls, "changed")
    qtbot.wait(500)
    keystroke(qtbot, qapp_cls, QtCore.Qt.Key_Enter)
    qtbot.wait(500)
    val = qapp_cls.MainWindow.currentFlowgraph.options_block.params['title'].value
    assert val == "Not titled changed"

def test_rotate_block(qtbot, qapp_cls):
    block = qapp_cls.MainWindow.currentFlowgraph.options_block
    qtbot.mouseClick(
            qapp_cls.MainWindow.currentView.viewport(), 
            QtCore.Qt.LeftButton, 
            pos=qapp_cls.MainWindow.currentView.mapFromScene(block.pos()+QtCore.QPoint(15,15)))
    qtbot.wait(500)
    old_rotation = block.states['rotation']
    keystroke(qtbot, qapp_cls, QtCore.Qt.Key_Left)
    qtbot.wait(500)
    new_rotation = block.states['rotation']
    assert new_rotation == old_rotation - 90

def test_disable_enable(qtbot, qapp_cls):
    vp = qapp_cls.MainWindow.currentView.viewport()
    block = None
    for block_ in qapp_cls.MainWindow.currentFlowgraph.blocks:
        if block_.key == 'variable':
            block = block_
    qtbot.mouseClick(
            vp,
            QtCore.Qt.LeftButton, 
            pos=qapp_cls.MainWindow.currentView.mapFromScene(block.pos()+QtCore.QPoint(4,4)))
    assert block != None
    assert block.state == 'enabled'
    print(block.key)
    qtbot.wait(100)
    qtbot.mouseClick(
            vp,
            QtCore.Qt.LeftButton, 
            pos=qapp_cls.MainWindow.currentView.mapFromScene(block.pos()+QtCore.QPoint(4,4)))
    keystroke(qtbot, qapp_cls, QtCore.Qt.Key_D)
    qtbot.wait(100)
    assert block.state == 'disabled'
    keystroke(qtbot, qapp_cls, QtCore.Qt.Key_E)
    qtbot.wait(100)
    assert block.state == 'enabled'

def test_move_blocks(qtbot, qapp_cls):
    fg = qapp_cls.MainWindow.currentFlowgraph
    vp = qapp_cls.MainWindow.currentView.viewport()
    
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

    start = vp.mapToGlobal(throttle.pos().toPoint()+QtCore.QPoint(5,5))
    pag.moveTo(start.x(), start.y())
    pag.mouseDown()
    def drag():
        for i in range(20):
            pag.move(0, 2)
            time.sleep(0.0001)
    drag_t = threading.Thread(target=drag)
    drag_t.start()
    while drag_t.is_alive():
        qtbot.wait(100)
    pag.mouseUp()

    start = vp.mapToGlobal(n_src.pos().toPoint()+QtCore.QPoint(5,5))
    pag.moveTo(start.x(), start.y())
    pag.mouseDown()
    def drag():
        for i in range(20):
            pag.move(-8, 0)
            time.sleep(0.0001)
    drag_t = threading.Thread(target=drag)
    drag_t.start()
    while drag_t.is_alive():
        qtbot.wait(100)
    pag.mouseUp()

    start = vp.mapToGlobal(n_sink.pos().toPoint()+QtCore.QPoint(5,5))
    pag.moveTo(start.x(), start.y())
    pag.mouseDown()
    def drag():
        for i in range(20):
            pag.move(12, 0)
            time.sleep(0.0001)
    drag_t = threading.Thread(target=drag)
    drag_t.start()
    while drag_t.is_alive():
        qtbot.wait(100)
    pag.mouseUp()

    assert True

def test_create_connection(qtbot, qapp_cls):
    fg = qapp_cls.MainWindow.currentFlowgraph
    vp = qapp_cls.MainWindow.currentView.viewport()
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

    click_pos = qapp_cls.MainWindow.currentView.mapToGlobal(n_src.mapToScene(n_src.sources[0].pos()+QtCore.QPoint(4,4)).toPoint())
    pag.click(click_pos.x(), click_pos.y(), button="left")
    click_pos = qapp_cls.MainWindow.currentView.mapToGlobal(n_sink.mapToScene(n_sink.sinks[0].pos()+QtCore.QPoint(4,4)).toPoint())
    pag.click(click_pos.x(), click_pos.y(), button="left")

    qtbot.wait(50)
    assert len(fg.connections) > 0


def test_bypass(qtbot, qapp_cls):
    fg = qapp_cls.MainWindow.currentFlowgraph
    vp = qapp_cls.MainWindow.currentView.viewport()
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
    click_pos = qapp_cls.MainWindow.currentView.mapToGlobal((throttle.pos()+QtCore.QPoint(4,4)).toPoint())
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(50)
    keystroke(qtbot, qapp_cls, QtCore.Qt.Key_B)
    qtbot.wait(50)
    assert throttle.state == 'bypassed'
    qtbot.wait(50)
    keystroke(qtbot, qapp_cls, QtCore.Qt.Key_E)
    assert throttle.state == 'enabled'
    # Try to bypass the null source, this shouldn't work
    click_pos = qapp_cls.MainWindow.currentView.mapToGlobal((n_src.pos()+QtCore.QPoint(4,4)).toPoint())
    pag.click(click_pos.x(), click_pos.y(), button="left")
    qtbot.wait(50)
    keystroke(qtbot, qapp_cls, QtCore.Qt.Key_B)
    qtbot.wait(50)
    assert n_src.state == 'enabled'

def test_quit(qtbot, qapp_cls):
    qapp_cls.MainWindow.actions["exit"].trigger()
    assert True
    time.sleep(1)


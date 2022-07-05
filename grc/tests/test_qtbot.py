import pytest
import gettext
import locale
import threading

from pytestqt.plugin import qapp

import time

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

def test_open_properties(qtbot, qapp_cls):
    qtbot.wait(1000)
    qtbot.mouseDClick(
            qapp_cls.MainWindow.currentView.viewport(), 
            QtCore.Qt.LeftButton, 
            pos=qapp_cls.MainWindow.currentView.mapFromScene(qapp_cls.MainWindow.currentFlowgraph.options_block.pos()+QtCore.QPoint(15,15)))
    qtbot.wait(500)
    assert True

def test_change_id(qtbot, qapp_cls):
    qtbot.mouseDClick(qapp_cls.MainWindow.currentFlowgraph.options_block.props_dialog.edit_params[1],
            QtCore.Qt.LeftButton)
    type_text(qtbot, qapp_cls, "changed")
    qtbot.wait(500)
    keystroke(qtbot, qapp_cls, QtCore.Qt.Key_Enter)
    qtbot.wait(2000)
    val = qapp_cls.MainWindow.currentFlowgraph.options_block.params['title'].value
    assert val == "Not titled changed"

def test_rotate_block(qtbot, qapp_cls):
    block = qapp_cls.MainWindow.currentFlowgraph.options_block
    old_rotation = block.states['rotation']
    keystroke(qtbot, qapp_cls, QtCore.Qt.Key_Left)
    qtbot.wait(500)
    new_rotation = block.states['rotation']
    assert new_rotation == old_rotation - 90

def test_disable_enable(qtbot, qapp_cls):
    assert False

def test_create_connection(qtbot, qapp_cls):
    fg = qapp_cls.MainWindow.currentFlowgraph
    assert len(fg.connections) == 0
    # Insert code here
    assert len(fg.connections) > 0


def test_bypass(qtbot, qapp_cls):
    assert False

'''
# Looks like this won't work with pytest-qt, at least not this way
def test_move_block(qtbot, qapp_cls):
    block = qapp_cls.MainWindow.currentFlowgraph.options_block
    qtbot.wait(500)
    qtbot.mousePress(
            qapp_cls.MainWindow.currentView.viewport(), 
            QtCore.Qt.LeftButton, 
            pos=qapp_cls.MainWindow.currentView.mapFromScene(block.pos()+QtCore.QPoint(15,15)))
    qtbot.wait(500)
    qtbot.mouseMove(
            qapp_cls.MainWindow.currentView.viewport(), 
            pos=qapp_cls.MainWindow.currentView.mapFromScene(block.pos()+QtCore.QPoint(15,15)))
    qtbot.wait(500)
    qtbot.mouseMove(
            qapp_cls.MainWindow.currentView.viewport(), 
            pos=qapp_cls.MainWindow.currentView.mapFromScene(block.pos()+QtCore.QPoint(150,150)))
    qtbot.wait(500)
    qtbot.mousePress(
            qapp_cls.MainWindow.currentView.viewport(), 
            QtCore.Qt.LeftButton, 
            pos=qapp_cls.MainWindow.currentView.mapFromScene(block.pos()+QtCore.QPoint(200,200)))
    qtbot.wait(500)
    new_rotation = block.states['rotation']
    assert True
'''

def test_quit(qtbot, qapp_cls):
    qapp_cls.MainWindow.actions["exit"].trigger()
    qtbot.wait(1000)
    assert True


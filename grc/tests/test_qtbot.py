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

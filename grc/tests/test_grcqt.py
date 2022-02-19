# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
import pytest
import gettext
import locale

import logging

from PyQt5 import QtTest, QtCore
from os import path

from gnuradio import gr
from grc.gui_qt import properties
from grc.gui_qt.grc import Application
from grc.gui_qt.components.window import MainWindow
from grc.gui_qt.Platform import Platform

log = logging.getLogger('grc')


@pytest.fixture
def app():
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
    app.MainWindow.show()
    return app

def test_open_and_close(app):
    QtTest.QTest.qWait(2000)
    app.MainWindow.exit_triggered()
    assert True


# Copyright 2014-2022 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import, print_function

# Standard modules
import logging

# Third-party  modules
import six

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtGui import QStandardItemModel
from PyQt5.QtWebEngineWidgets import *

# Custom modules
from .canvas.block import Block
from .. import base

# Logging
log = logging.getLogger(__name__)

class WikiTab(QtWidgets.QDockWidget, base.Component):

    def __init__(self):
        QtWidgets.QDockWidget.__init__(self)
        base.Component.__init__(self)

        self.setObjectName('wiki_tab')
        self.setWindowTitle('Wiki')

        # TODO: Pull from preferences and revert to default if not found?
        self.setFloating(False)

        ### GUI Widgets

        # Create the layout widget
        container = QtWidgets.QWidget(self)
        container.setObjectName('wiki_tab::container')
        self._container = container

        layout = QtWidgets.QVBoxLayout(container)
        layout.setObjectName('block_library::layout')
        layout.setSpacing(0)
        layout.setContentsMargins(5, 0, 5, 5)
        self._text = QWebEngineView()
        layout.addWidget(self._text)
        self._layout = layout

        container.setLayout(layout)
        self.setWidget(container)

        # TODO: Move to the base controller and set actions as class attributes
        # Automatically create the actions, menus and toolbars.
        # Child controllers need to call the register functions to integrate into the mainwindow
        self.actions = {}
        self.menus = {}
        self.toolbars = {}
        self.createActions(self.actions)
        self.createMenus(self.actions, self.menus)
        self.createToolbars(self.actions, self.toolbars)
        self.connectSlots()

        # Register the dock widget through the AppController.
        # The AppController then tries to find a saved dock location from the preferences
        # before calling the MainWindow Controller to add the widget.
        self.app.registerDockWidget(self, location=self.settings.window.WIKI_TAB_DOCK_LOCATION)

    def setURL(self, url):
        self._text.load(url)
        self._text.show()

    ### Actions

    def createActions(self, actions):
        log.debug("Creating actions")

    def createMenus(self, actions, menus):
        log.debug("Creating menus")

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")

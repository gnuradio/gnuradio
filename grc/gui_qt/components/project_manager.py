# Copyright 2014-2020 Free Software Foundation, Inc.
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
import html
import logging
import textwrap

# Third-party  modules
import six

from PyQt5 import QtCore, QtGui, QtWidgets

# Custom modules
from .. import base

# Shortcuts
Action = QtWidgets.QAction
Menu = QtWidgets.QMenu
Toolbar = QtWidgets.QToolBar
Icons = QtGui.QIcon.fromTheme
Keys = QtGui.QKeySequence

# Logging
log = logging.getLogger(__name__)


class ProjectManager(QtWidgets.QDockWidget, base.Component):

    def __init__(self):
        QtWidgets.QDockWidget.__init__(self)
        base.Component.__init__(self)

        self.setObjectName('project_manager')
        self.setWindowTitle('Project Manager')

        self.resize(400, 300)
        self.setFloating(False)

        ### GUI Widgets

        container = QtWidgets.QWidget()
        container.setObjectName('project_manager::container')
        self._container = container

        layout = QtWidgets.QVBoxLayout(container)
        layout.setObjectName('project_manager::layout')
        layout.setSpacing(0)
        layout.setContentsMargins(5, 0, 5, 5)
        self._layout = layout

        tree = QtWidgets.QTreeWidget(container)
        tree.setDragEnabled(True)
        tree.setDragDropMode(QtWidgets.QAbstractItemView.DragOnly)
        tree.setColumnCount(1)
        tree.setObjectName("project_manager::tree")
        tree.header().setVisible(False)
        self._tree = tree

        # Add widgets to the component
        layout.addWidget(projects)
        container.setLayout(layout)
        self.setWidget(container)

        ### Translation support

        #self.setWindowTitle(_translate("", "Library", None))
        #library.headerItem().setText(0, _translate("", "Blocks", None))
        #QtCore.QMetaObject.connectSlotsByName(blockLibraryDock)
        self.retranslate()

        ### Setup actions

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
        self.app.registerDockWidget(self, location=self.settings.window.REPORTS_DOCK_LOCATION)

        # Register the menus
        #self.app.registerMenu(self.menus["reports"])

    def retranslate(self):
        #self.setWindowTitle(_translate("blockLibraryDock", "Library", None))
        #self.blockTree.headerItem().setText(0, _translate("blockLibraryDock", "Blocks", None))
        self.setWindowTitle("Project Manager")

from __future__ import absolute_import, print_function

# Standard modules
import logging

from qtpy import QtGui, QtWidgets

# Custom modules
from .. import base

# Logging
log = logging.getLogger(__name__)


class VariableEditor(QtWidgets.QDockWidget, base.Component):
    def __init__(self):
        super(VariableEditor, self).__init__()

        self.setObjectName('variable_editor')
        self.setWindowTitle('Variable Editor')

        ### GUI Widgets
        self._tree = QtWidgets.QTreeWidget()
        self._tree.setObjectName('variable_editor::tree_widget')
        self._tree.setHeaderLabels(["ID", "Value", ""])
        self.setWidget(self._tree)

        # TODO: Add non-hardcoded data
        imports = QtWidgets.QTreeWidgetItem(self._tree)
        imports.setText(0, "Imports")
        imports.setIcon(2, QtGui.QIcon.fromTheme("list-add"))
        variables = QtWidgets.QTreeWidgetItem(self._tree)
        variables.setText(0, "Variables")
        variables.setIcon(2, QtGui.QIcon.fromTheme("list-add"))
        samp_rate = QtWidgets.QTreeWidgetItem(variables)
        samp_rate.setText(0, "samp_rate")
        samp_rate.setText(1, "32000")
        self._tree.expandAll()

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
        self.app.registerDockWidget(self, location=self.settings.window.VARIABLE_EDITOR_DOCK_LOCATION)

    ### Actions

    def createActions(self, actions):
        log.debug("Creating actions")

    def createMenus(self, actions, menus):
        log.debug("Creating menus")

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")

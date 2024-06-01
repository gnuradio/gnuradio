from __future__ import absolute_import, print_function

# Standard modules
import logging
from enum import Enum

from qtpy import QtGui
from qtpy.QtWidgets import QMenu, QAction, QDockWidget, QTreeWidget, QTreeWidgetItem
from qtpy.QtCore import Slot, Signal, QPointF, Qt, QVariant

# Custom modules
from .. import base
from ...core.base import Element
from .canvas.flowgraph import FlowgraphScene
from .canvas import colors

# Logging
log = logging.getLogger(f"grc.application.{__name__}")


class VariableEditorAction(Enum):
    # Actions that are handled by the editor
    ADD_IMPORT = 0
    ADD_VARIABLE = 1
    OPEN_PROPERTIES = 2
    DELETE_BLOCK = 3
    DELETE_CONFIRM = 4
    ENABLE_BLOCK = 5
    DISABLE_BLOCK = 6


class VariableEditor(QDockWidget, base.Component):
    all_editor_actions = Signal([VariableEditorAction])

    def __init__(self):
        super(VariableEditor, self).__init__()

        self.qsettings = self.app.qsettings

        self.setObjectName('variable_editor')
        self.setWindowTitle('Variable Editor')

        self.right_click_menu = VariableEditorContextMenu(self)
        self.scene = None

        # GUI Widgets
        self._tree = QTreeWidget()
        self._model = self._tree.model()
        self._tree.setObjectName('variable_editor::tree_widget')
        self._tree.setHeaderLabels(["ID", "Value", ""])
        self.setWidget(self._tree)
        self.currently_rebuilding = True
        self._model.dataChanged.connect(self.handle_change)

        imports = QTreeWidgetItem(self._tree)
        imports.setText(0, "Imports")
        imports.setIcon(2, QtGui.QIcon.fromTheme("list-add"))
        variables = QTreeWidgetItem(self._tree)
        variables.setText(0, "Variables")
        variables.setIcon(2, QtGui.QIcon.fromTheme("list-add"))
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
        self.currently_rebuilding = False
        if not self.qsettings.value("appearance/display_variable_editor", True, type=bool):
            self.hide()

    # Actions

    def createActions(self, actions):
        log.debug("Creating actions")

    def createMenus(self, actions, menus):
        log.debug("Creating menus")

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")

    def contextMenuEvent(self, event):
        self.right_click_menu.exec_(self.mapToGlobal(event.pos()))

    def keyPressEvent(self, event):
        super(VariableEditor, self).keyPressEvent(event)

    def set_scene(self, scene: FlowgraphScene):
        self.scene = scene
        self.update_gui(self.scene.core.blocks)
        self._tree.resizeColumnToContents(0)
        self._tree.resizeColumnToContents(1)

    def handle_change(self, tl, br):  # TODO: Why are there two arguments?
        if self.currently_rebuilding:
            return

        c_block = self._tree.model().data(tl, role=Qt.UserRole)
        if not c_block:
            return

        new_text = self._tree.model().data(tl)
        c_block.old_data = c_block.export_data()
        if tl.column() == 0:  # The name (id) changed
            c_block.params['id'].set_value(new_text)
        else:  # column == 1, i.e. the value changed
            if c_block.is_import:
                c_block.params['imports'].set_value(new_text)
            else:
                c_block.params['value'].set_value(new_text)
        self.scene.blockPropsChange.emit(c_block)
        self._tree.resizeColumnToContents(0)
        self._tree.resizeColumnToContents(1)

    def _rebuild(self):
        # TODO: The way we update block params here seems suboptimal
        self.currently_rebuilding = True
        self._tree.clear()
        imports = QTreeWidgetItem(self._tree, 2)
        imports.setText(0, "Imports")
        imports.setForeground(0, self._tree.palette().color(self.palette().WindowText))
        imports.setIcon(2, QtGui.QIcon.fromTheme("list-add"))
        for block in self._imports:
            import_ = QTreeWidgetItem(imports, 0)
            import_.setText(0, block.name)
            import_.setData(0, Qt.UserRole, block)
            import_.setText(1, block.params['imports'].get_value())
            import_.setData(1, Qt.UserRole, block)
            import_.setIcon(2, QtGui.QIcon.fromTheme("list-remove"))
            if block.enabled:
                import_.setFlags(Qt.ItemIsSelectable | Qt.ItemIsEditable | Qt.ItemIsEnabled)
                import_.setForeground(0, self._tree.palette().color(self.palette().WindowText))
                import_.setForeground(1, self._tree.palette().color(self.palette().WindowText))
            else:
                import_.setFlags(Qt.ItemIsSelectable | Qt.ItemIsEnabled)
                import_.setForeground(0, self._tree.palette().color(self.palette().Disabled, self.palette().WindowText))
                import_.setForeground(1, self._tree.palette().color(self.palette().Disabled, self.palette().WindowText))

        variables = QTreeWidgetItem(self._tree, 2)
        variables.setText(0, "Variables")
        variables.setForeground(0, self._tree.palette().color(self.palette().WindowText))
        variables.setIcon(2, QtGui.QIcon.fromTheme("list-add"))
        for block in sorted(self._variables, key=lambda v: v.name):
            variable_ = QTreeWidgetItem(variables, 1)
            variable_.setText(0, block.name)
            variable_.setData(0, Qt.UserRole, block)
            if block.key == 'variable':
                variable_.setText(1, block.params['value'].get_value())
                variable_.setData(1, Qt.UserRole, block)
                variable_.setFlags(Qt.ItemIsSelectable | Qt.ItemIsEditable | Qt.ItemIsEnabled)
                if block.enabled:
                    variable_.setForeground(0, self._tree.palette().color(self.palette().WindowText))
                    variable_.setForeground(1, self._tree.palette().color(self.palette().WindowText))
                else:
                    variable_.setForeground(0, self._tree.palette().color(self.palette().Disabled, self.palette().WindowText))
                    variable_.setForeground(1, self._tree.palette().color(self.palette().Disabled, self.palette().WindowText))
            else:
                variable_.setFlags(Qt.ItemIsSelectable | Qt.ItemIsEnabled)
                if block.enabled:
                    variable_.setText(1, str(block.evaluate(block.value)))
                    variable_.setForeground(0, self._tree.palette().color(self.palette().WindowText))
                    variable_.setForeground(1, self._tree.palette().color(self.palette().WindowText))
                else:
                    variable_.setText(1, '<Open Properties>')
                    variable_.setForeground(0, self._tree.palette().color(self.palette().Disabled, self.palette().WindowText))
                    variable_.setForeground(1, self._tree.palette().color(self.palette().Disabled, self.palette().WindowText))
            variable_.setIcon(2, QtGui.QIcon.fromTheme("list-remove"))

        self.currently_rebuilding = False

    def update_gui(self, blocks):
        self._imports = [block for block in blocks if block.is_import]
        self._variables = [block for block in blocks if block.is_variable]
        self._rebuild()
        self._tree.expandAll()

    Slot(VariableEditorAction)

    def handle_action(self, action):
        log.debug(f"{action} triggered!")
        """
        Single handler for the different actions that can be triggered by the context menu,
        key presses or mouse clicks. Also triggers an update of the flow graph and editor.
        """
        if action == VariableEditorAction.ADD_IMPORT:
            self.all_editor_actions.emit(action)
            return
        elif action == VariableEditorAction.ADD_VARIABLE:
            self.all_editor_actions.emit(action)
            return
        elif action == VariableEditorAction.OPEN_PROPERTIES:
            # TODO: Disabled in GRC Gtk. Enable?
            self.all_editor_actions.emit(action)
            return

        if self._tree.currentItem().type() == 2:  # Import or Variable header line was selected
            return

        self.scene.clearSelection()
        if self._tree.currentItem().type() == 0:
            to_handle = self.scene.core.blocks.index(self._imports[self._tree.currentIndex().row()])
        else:
            to_handle = self.scene.core.blocks.index(self._variables[self._tree.currentIndex().row()])
        self.scene.core.blocks[to_handle].gui.setSelected(True)
        self.all_editor_actions.emit(action)


class VariableEditorContextMenu(QMenu):
    def __init__(self, var_edit: VariableEditor):
        super(QMenu, self).__init__()

        self.imports = QAction(_("variable_editor_add_import"), self)
        self.imports.triggered.connect(lambda: var_edit.handle_action(VariableEditorAction.ADD_IMPORT))
        self.addAction(self.imports)

        self.variables = QAction(_("variable_editor_add_variable"), self)
        self.variables.triggered.connect(lambda: var_edit.handle_action(VariableEditorAction.ADD_VARIABLE))
        self.addAction(self.variables)

        self.addSeparator()

        self.enable = QAction(_("variable_editor_enable"), self)
        self.enable.triggered.connect(lambda: var_edit.handle_action(VariableEditorAction.ENABLE_BLOCK))
        self.addAction(self.enable)

        self.disable = QAction(_("variable_editor_disable"), self)
        self.disable.triggered.connect(lambda: var_edit.handle_action(VariableEditorAction.DISABLE_BLOCK))
        self.addAction(self.disable)

        self.addSeparator()

        self.delete = QAction(_("variable_editor_delete"), self)
        self.delete.triggered.connect(lambda: var_edit.handle_action(VariableEditorAction.DELETE_BLOCK))
        self.addAction(self.delete)

        self.addSeparator()

        self.properties = QAction(_("variable_editor_properties"), self)
        self.properties.triggered.connect(lambda: var_edit.handle_action(VariableEditorAction.OPEN_PROPERTIES))
        self.addAction(self.properties)

    def update_enabled(self, selected, enabled=False):
        self.delete.setEnabled(selected)
        self.properties.setEnabled(selected)
        self.enable.setEnabled(selected and not enabled)
        self.disable.setEnabled(selected and enabled)

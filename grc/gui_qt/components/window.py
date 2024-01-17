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
import logging
import os
import sys
import subprocess
import cProfile, pstats


from typing import Union

from qtpy import QtCore, QtGui, QtWidgets
from qtpy.QtCore import Qt

# Custom modules
from .flowgraph_view import FlowgraphView
from .canvas.flowgraph import FlowgraphScene
from .example_browser import ExampleBrowser, ExampleBrowserDialog, Worker
from .executor import ExecFlowGraphThread
from .. import base, Constants, Utils
from .undoable_actions import (
    RotateAction,
    EnableAction,
    DisableAction,
    BypassAction,
    MoveAction,
    NewElementAction,
    DeleteElementAction,
    BlockPropsChangeAction,
    BussifyAction
)
from .preferences import PreferencesDialog
from .oot_browser import OOTBrowser
from .dialogs import ErrorsDialog
from ...core.base import Element

# Logging
log = logging.getLogger(f"grc.application.{__name__}")

# Shortcuts
Action = QtWidgets.QAction
Menu = QtWidgets.QMenu
Toolbar = QtWidgets.QToolBar
Icons = QtGui.QIcon.fromTheme
Keys = QtGui.QKeySequence
QStyle = QtWidgets.QStyle


class MainWindow(QtWidgets.QMainWindow, base.Component):
    def __init__(self):
        QtWidgets.QMainWindow.__init__(self)
        # base.Component.__init__(self)

        log.debug("Setting the main window")
        self.setObjectName("main")
        self.setWindowTitle(_("window-title"))
        self.setDockOptions(
            QtWidgets.QMainWindow.AllowNestedDocks |
            QtWidgets.QMainWindow.AllowTabbedDocks |
            QtWidgets.QMainWindow.AnimatedDocks
        )
        self.progress_bar = QtWidgets.QProgressBar()
        self.statusBar().addPermanentWidget(self.progress_bar)
        self.progress_bar.hide()

        # Setup the window icon
        icon = QtGui.QIcon(self.settings.path.ICON)
        log.debug("Setting window icon - ({0})".format(self.settings.path.ICON))
        self.setWindowIcon(icon)

        monitor = self.screen().availableGeometry()
        log.debug(
            "Setting window size - ({}, {})".format(monitor.width(), monitor.height())
        )
        self.resize(int(monitor.width() * 0.50), monitor.height())

        self.setCorner(Qt.BottomLeftCorner, Qt.LeftDockWidgetArea)

        self.menuBar().setNativeMenuBar(self.settings.window.NATIVE_MENUBAR)

        # TODO: Not sure about document mode
        # self.setDocumentMode(True)

        # Generate the rest of the window
        self.createStatusBar()

        self.profiler = cProfile.Profile()

        # actions['Quit.triggered.connect(self.close)
        # actions['Report.triggered.connect(self.reportDock.show)
        # QtCore.QMetaObject.connectSlotsByName(self)

        # Translation support

        # self.setWindowTitle(_translate("blockLibraryDock", "Library", None))
        # library.headerItem().setText(0, _translate("blockLibraryDock", "Blocks", None))
        # QtCore.QMetaObject.connectSlotsByName(blockLibraryDock)

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

        # Rest of the GUI widgets

        # Map some of the view's functions to the controller class
        self.registerDockWidget = self.addDockWidget
        self.registerMenu = self.addMenu
        self.registerToolBar = self.addToolBar

        # Do other initialization stuff. View should already be allocated and
        # actions dynamically connected to class functions. Also, the log
        # functionality should be also allocated
        log.debug("__init__")
        QtGui.QIcon.setThemeName("Papirus-Dark")

        # Add the menus from the view
        menus = self.menus
        self.registerMenu(menus["file"])
        self.registerMenu(menus["edit"])
        self.registerMenu(menus["view"])
        self.registerMenu(menus["build"])
        self.registerMenu(menus["tools"])
        self.registerMenu(menus["help"])

        toolbars = self.toolbars
        self.registerToolBar(toolbars["file"])
        self.registerToolBar(toolbars["edit"])
        self.registerToolBar(toolbars["run"])
        self.registerToolBar(toolbars["misc"])

        self.tabWidget = QtWidgets.QTabWidget()
        self.tabWidget.setTabsClosable(True)
        self.tabWidget.tabCloseRequested.connect(
            lambda index: self.close_triggered(index)
        )
        self.setCentralWidget(self.tabWidget)

        files_open = list(self.app.qsettings.value('window/files_open', []))
        if files_open:
            for file in files_open:
                self.open_triggered(file)
        else:
            self.new_triggered()

        self.clipboard = None
        self.undoView = None

        try:
            self.restoreGeometry(self.app.qsettings.value("window/geometry"))
            self.restoreState(self.app.qsettings.value("window/windowState"))
        except TypeError:
            log.warning("Could not restore window geometry and state.")

        self.examples_found = False
        self.ExampleBrowser = ExampleBrowser()
        self.ExampleBrowser.file_to_open.connect(self.open_example)
        self.OOTBrowser = OOTBrowser()

        self.threadpool = QtCore.QThreadPool()
        self.threadpool.setMaxThreadCount(1)
        ExampleFinder = Worker(self.ExampleBrowser.find_examples)
        ExampleFinder.signals.result.connect(self.populate_libraries_w_examples)
        ExampleFinder.signals.progress.connect(self.update_progress_bar)
        self.threadpool.start(ExampleFinder)

    """def show(self):
        log.debug("Showing main window")
        self.show()
    """

    def update_variable_editor(self, var_edit):
        var_edit.new_block.connect(self.var_edit_new_block)

    @QtCore.Slot(str)
    def var_edit_new_block(self, block_key):
        self.currentFlowgraphScene.add_block(block_key)

    @property
    def currentView(self):
        return self.tabWidget.currentWidget()

    @property
    def currentFlowgraphScene(self):
        return self.tabWidget.currentWidget().scene()

    @property
    def currentFlowgraph(self):
        return self.tabWidget.currentWidget().scene().core

    @QtCore.Slot(QtCore.QPointF)
    def registerMove(self, diff):
        self.currentFlowgraphScene.set_saved(False)
        self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.red)
        action = MoveAction(self.currentFlowgraphScene, diff)
        self.currentFlowgraphScene.undoStack.push(action)
        self.updateActions()

    @QtCore.Slot(Element)
    def registerNewElement(self, elem):
        self.currentFlowgraphScene.set_saved(False)
        self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.red)
        action = NewElementAction(self.currentFlowgraphScene, elem)
        self.currentFlowgraphScene.undoStack.push(action)
        self.updateActions()
        self.currentFlowgraphScene.update()

    @QtCore.Slot(Element)
    def registerDeleteElement(self, elem):
        self.currentFlowgraphScene.set_saved(False)
        self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.red)
        action = DeleteElementAction(self.currentFlowgraphScene, elem)
        self.currentFlowgraphScene.undoStack.push(action)
        self.updateActions()
        self.currentFlowgraphScene.update()

    @QtCore.Slot(Element)
    def registerBlockPropsChange(self, elem):
        self.currentFlowgraphScene.set_saved(False)
        self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.red)
        action = BlockPropsChangeAction(self.currentFlowgraphScene, elem)
        self.currentFlowgraphScene.undoStack.push(action)
        self.updateActions()
        self.currentFlowgraphScene.update()

    def createActions(self, actions):
        """
        Defines all actions for this view.
        Controller manages connecting signals to slots implemented in the controller
        """
        log.debug("Creating actions")

        # File Actions
        actions["new"] = Action(
            Icons("document-new"),
            _("new"),
            self,
            shortcut=Keys.New,
            statusTip=_("new-tooltip"),
        )

        actions["open"] = Action(
            Icons("document-open"),
            _("open"),
            self,
            shortcut=Keys.Open,
            statusTip=_("open-tooltip"),
        )

        actions["example_browser"] = Action(
            _("example_browser"),
            self,
            statusTip=_("example_browser-tooltip"),
        )

        actions["close"] = Action(
            Icons("window-close"),
            _("close"),
            self,
            shortcut=Keys.Close,
            statusTip=_("close-tooltip"),
        )

        actions["close_all"] = Action(
            Icons("window-close"),
            _("close_all"),
            self,
            statusTip=_("close_all-tooltip"),
        )
        actions["save"] = Action(
            Icons("document-save"),
            _("save"),
            self,
            shortcut=Keys.Save,
            statusTip=_("save-tooltip"),
        )

        actions["save_as"] = Action(
            Icons("document-save-as"),
            _("save_as"),
            self,
            shortcut=Keys.SaveAs,
            statusTip=_("save_as-tooltip"),
        )

        actions["save_copy"] = Action(_("save_copy"), self)

        actions["screen_capture"] = Action(
            Icons("camera-photo"),
            _("screen_capture"),
            self,
            shortcut=Keys.Print,
            statusTip=_("screen_capture-tooltip"),
        )

        actions["exit"] = Action(
            Icons("application-exit"),
            _("exit"),
            self,
            shortcut=Keys.Quit,
            statusTip=_("exit-tooltip"),
        )

        # Edit Actions
        actions["undo"] = Action(
            Icons("edit-undo"),
            _("undo"),
            self,
            shortcut=Keys.Undo,
            statusTip=_("undo-tooltip"),
        )

        actions["redo"] = Action(
            Icons("edit-redo"),
            _("redo"),
            self,
            shortcut=Keys.Redo,
            statusTip=_("redo-tooltip"),
        )

        actions["view_undo_stack"] = Action("View undo stack", self)

        actions["cut"] = Action(
            Icons("edit-cut"),
            _("cut"),
            self,
            shortcut=Keys.Cut,
            statusTip=_("cut-tooltip"),
        )

        actions["copy"] = Action(
            Icons("edit-copy"),
            _("copy"),
            self,
            shortcut=Keys.Copy,
            statusTip=_("copy-tooltip"),
        )

        actions["paste"] = Action(
            Icons("edit-paste"),
            _("paste"),
            self,
            shortcut=Keys.Paste,
            statusTip=_("paste-tooltip"),
        )

        actions["delete"] = Action(
            Icons("edit-delete"),
            _("delete"),
            self,
            shortcut=Keys.Delete,
            statusTip=_("delete-tooltip"),
        )

        actions["undo"].setEnabled(False)
        actions["redo"].setEnabled(False)
        actions["cut"].setEnabled(False)
        actions["copy"].setEnabled(False)
        actions["paste"].setEnabled(False)
        actions["delete"].setEnabled(False)

        actions["select_all"] = Action(
            Icons("edit-select_all"),
            _("select_all"),
            self,
            shortcut=Keys.SelectAll,
            statusTip=_("select_all-tooltip"),
        )

        actions["select_none"] = Action(
            _("Select None"), self, statusTip=_("select_none-tooltip")
        )

        actions["rotate_ccw"] = Action(
            Icons("object-rotate-left"),
            _("rotate_ccw"),
            self,
            shortcut=Keys.MoveToPreviousChar,
            statusTip=_("rotate_ccw-tooltip"),
        )

        actions["rotate_cw"] = Action(
            Icons("object-rotate-right"),
            _("rotate_cw"),
            self,
            shortcut=Keys.MoveToNextChar,
            statusTip=_("rotate_cw-tooltip"),
        )

        actions["rotate_cw"].setEnabled(False)
        actions["rotate_ccw"].setEnabled(False)

        actions["enable"] = Action(_("enable"), self, shortcut="E")
        actions["disable"] = Action(_("disable"), self, shortcut="D")
        actions["bypass"] = Action(_("bypass"), self, shortcut="B")

        # TODO
        actions["block_inc_type"] = Action(_("block_inc_type"), self)
        actions["block_dec_type"] = Action(_("block_dec_type"), self)

        actions["enable"].setEnabled(False)
        actions["disable"].setEnabled(False)
        actions["bypass"].setEnabled(False)

        # TODO
        actions["vertical_align_top"] = Action(_("vertical_align_top"), self)
        actions["vertical_align_middle"] = Action(_("vertical_align_middle"), self)
        actions["vertical_align_bottom"] = Action(_("vertical_align_bottom"), self)

        actions["vertical_align_top"].setEnabled(False)
        actions["vertical_align_middle"].setEnabled(False)
        actions["vertical_align_bottom"].setEnabled(False)

        actions["horizontal_align_left"] = Action(_("horizontal_align_left"), self)
        actions["horizontal_align_center"] = Action(_("horizontal_align_center"), self)
        actions["horizontal_align_right"] = Action(_("horizontal_align_right"), self)

        actions["horizontal_align_left"].setEnabled(False)
        actions["horizontal_align_center"].setEnabled(False)
        actions["horizontal_align_right"].setEnabled(False)

        actions["create_hier"] = Action(_("create_hier_block"), self)
        actions["open_hier"] = Action(_("open_hier_block"), self)
        actions["toggle_source_bus"] = Action(_("toggle_source_bus"), self)
        actions["toggle_sink_bus"] = Action(_("toggle_sink_bus"), self)

        actions["create_hier"].setEnabled(False)
        actions["open_hier"].setEnabled(False)
        actions["toggle_source_bus"].setEnabled(False)
        actions["toggle_sink_bus"].setEnabled(False)

        actions["properties"] = Action(
            Icons("document-properties"),
            _("flowgraph-properties"),
            self,
            statusTip=_("flowgraph-properties-tooltip"),
        )

        actions["properties"].setEnabled(False)

        # View Actions
        actions["zoom_in"] = Action(
            Icons("zoom-in"),
            _("Zoom &in"),
            self
        )
        actions["zoom_in"].setShortcuts([Keys.ZoomIn, "Ctrl+="])
        actions["zoom_out"] = Action(
            Icons("zoom-out"),
            _("Zoom &out"),
            self,
            shortcut=Keys.ZoomOut,
        )
        actions["zoom_original"] = Action(
            Icons("zoom-original"),
            _("O&riginal size"),
            self,
            shortcut="Ctrl+0",
        )

        actions["toggle_grid"] = Action(
            _("toggle_grid"), self, shortcut="G", statusTip=_("toggle_grid-tooltip")
        )

        actions["errors"] = Action(
            Icons("dialog-error"), _("errors"), self, statusTip=_("errors-tooltip")
        )

        actions["find"] = Action(
            Icons("edit-find"),
            _("find"),
            self,
            shortcut=Keys.Find,
            statusTip=_("find-tooltip"),
        )

        # Help Actions
        actions["about"] = Action(
            Icons("help-about"), _("about"), self, statusTip=_("about-tooltip")
        )

        actions["about_qt"] = Action(
            self.style().standardIcon(QStyle.SP_TitleBarMenuButton),
            _("about-qt"),
            self,
            statusTip=_("about-tooltip"),
        )

        actions["generate"] = Action(
            Icons("system-run"),
            _("process-generate"),
            self,
            shortcut="F5",
            statusTip=_("process-generate-tooltip"),
        )

        actions["execute"] = Action(
            Icons("media-playback-start"),
            _("process-execute"),
            self,
            shortcut="F6",
            statusTip=_("process-execute-tooltip"),
        )

        actions["kill"] = Action(
            Icons("process-stop"),
            _("process-kill"),
            self,
            shortcut="F7",
            statusTip=_("process-kill-tooltip"),
        )

        actions["help"] = Action(
            Icons("help-browser"),
            _("help"),
            self,
            shortcut=Keys.HelpContents,
            statusTip=_("help-tooltip"),
        )

        # Tools Actions

        actions["filter_design_tool"] = Action(_("&Filter Design Tool"), self)

        actions["module_browser"] = Action(_("&OOT Module Browser"), self)

        actions["start_profiler"] = Action(_("Start profiler"), self)
        actions["stop_profiler"] = Action(_("Stop profiler"), self)

        # Help Actions

        actions["types"] = Action(_("&Types"), self)

        actions["keys"] = Action(_("&Keys"), self)

        actions["get_involved"] = Action(_("&Get Involved"), self)

        actions["preferences"] = Action(
            Icons("preferences-system"),
            _("preferences"),
            self,
            statusTip=_("preferences-tooltip"),
        )

        actions["reload"] = Action(
            Icons("view-refresh"), _("reload"), self, statusTip=_("reload-tooltip")
        )

        # Disable some actions, by default
        actions["save"].setEnabled(True)
        actions["errors"].setEnabled(False)

    def updateDocTab(self):
        pass
        """
        doc_txt = self._app().DocumentationTab._text
        blocks = self.currentFlowgraphScene.selected_blocks()
        if len(blocks) == 1:
            #print(blocks[0].documentation)
            doc_string = blocks[0].documentation['']
            doc_txt.setText(doc_string)
        """

    def updateActions(self):
        """Update the available actions based on what is selected"""
        self.update_variable_editor(self.app.VariableEditor)

        blocks = self.currentFlowgraphScene.selected_blocks()
        conns = self.currentFlowgraphScene.selected_connections()
        undoStack = self.currentFlowgraphScene.undoStack
        canUndo = undoStack.canUndo()
        canRedo = undoStack.canRedo()
        valid_fg = self.currentFlowgraph.is_valid()
        saved_fg = self.currentFlowgraphScene.saved

        self.actions["save"].setEnabled(not saved_fg)

        self.actions["undo"].setEnabled(canUndo)
        self.actions["redo"].setEnabled(canRedo)
        self.actions["generate"].setEnabled(valid_fg)
        self.actions["execute"].setEnabled(valid_fg)
        self.actions["errors"].setEnabled(not valid_fg)
        self.actions["kill"].setEnabled(self.currentView.process_is_done())

        self.actions["cut"].setEnabled(False)
        self.actions["copy"].setEnabled(False)
        self.actions["paste"].setEnabled(False)
        self.actions["delete"].setEnabled(False)
        self.actions["rotate_cw"].setEnabled(False)
        self.actions["rotate_ccw"].setEnabled(False)
        self.actions["enable"].setEnabled(False)
        self.actions["disable"].setEnabled(False)
        self.actions["bypass"].setEnabled(False)
        self.actions["properties"].setEnabled(False)
        self.actions["create_hier"].setEnabled(False)
        self.actions["toggle_source_bus"].setEnabled(False)
        self.actions["toggle_sink_bus"].setEnabled(False)

        if self.clipboard:
            self.actions["paste"].setEnabled(True)

        if len(conns) > 0:
            self.actions["delete"].setEnabled(True)

        if len(blocks) > 0:
            self.actions["cut"].setEnabled(True)
            self.actions["copy"].setEnabled(True)
            self.actions["delete"].setEnabled(True)
            self.actions["rotate_cw"].setEnabled(True)
            self.actions["rotate_ccw"].setEnabled(True)
            self.actions["enable"].setEnabled(True)
            self.actions["disable"].setEnabled(True)
            self.actions["bypass"].setEnabled(True)
            self.actions["toggle_source_bus"].setEnabled(True)
            self.actions["toggle_sink_bus"].setEnabled(True)

            self.actions["vertical_align_top"].setEnabled(False)
            self.actions["vertical_align_middle"].setEnabled(False)
            self.actions["vertical_align_bottom"].setEnabled(False)

            self.actions["horizontal_align_left"].setEnabled(False)
            self.actions["horizontal_align_center"].setEnabled(False)
            self.actions["horizontal_align_right"].setEnabled(False)

            if len(blocks) == 1:
                self.actions["properties"].setEnabled(True)
                self.actions["create_hier"].setEnabled(
                    True
                )  # TODO: Other requirements for enabling this?

            if len(blocks) > 1:
                self.actions["vertical_align_top"].setEnabled(True)
                self.actions["vertical_align_middle"].setEnabled(True)
                self.actions["vertical_align_bottom"].setEnabled(True)

                self.actions["horizontal_align_left"].setEnabled(True)
                self.actions["horizontal_align_center"].setEnabled(True)
                self.actions["horizontal_align_right"].setEnabled(True)

            for block in blocks:
                if not block.core.can_bypass():
                    self.actions["bypass"].setEnabled(False)
                    break

    def createMenus(self, actions, menus):
        """Setup the main menubar for the application"""
        log.debug("Creating menus")

        # Global menu options
        self.menuBar().setNativeMenuBar(True)

        open_recent = Menu("Open recent")
        menus["open_recent"] = open_recent
        recent_files = None
        if recent_files:
            pass
        else:
            open_recent.setEnabled(False)
        # open_recent.addAction(actions["open"])
        # TODO: populate recent files

        # Setup the file menu
        file = Menu("&File")
        file.addAction(actions["new"])
        file.addAction(actions["open"])
        file.addMenu(open_recent)
        file.addAction(actions["example_browser"])
        file.addAction(actions["close"])
        file.addAction(actions["close_all"])
        file.addSeparator()
        file.addAction(actions["save"])
        file.addAction(actions["save_as"])
        file.addAction(actions["save_copy"])
        file.addSeparator()
        file.addAction(actions["screen_capture"])
        file.addSeparator()
        file.addAction(actions["preferences"])
        file.addSeparator()
        file.addAction(actions["exit"])
        menus["file"] = file

        # Setup the edit menu
        edit = Menu("&Edit")
        edit.addAction(actions["undo"])
        edit.addAction(actions["redo"])
        edit.addAction(actions["view_undo_stack"])
        edit.addSeparator()
        edit.addAction(actions["cut"])
        edit.addAction(actions["copy"])
        edit.addAction(actions["paste"])
        edit.addAction(actions["delete"])
        edit.addAction(actions["select_all"])
        edit.addAction(actions["select_none"])
        edit.addSeparator()
        edit.addAction(actions["rotate_ccw"])
        edit.addAction(actions["rotate_cw"])

        align = Menu("&Align")
        menus["align"] = align
        align.addAction(actions["vertical_align_top"])
        align.addAction(actions["vertical_align_middle"])
        align.addAction(actions["vertical_align_bottom"])
        align.addSeparator()
        align.addAction(actions["horizontal_align_left"])
        align.addAction(actions["horizontal_align_center"])
        align.addAction(actions["horizontal_align_right"])

        edit.addMenu(align)
        edit.addSeparator()
        edit.addAction(actions["enable"])
        edit.addAction(actions["disable"])
        edit.addAction(actions["bypass"])
        edit.addSeparator()

        more = Menu("&More")
        menus["more"] = more
        more.addAction(actions["create_hier"])
        more.addAction(actions["open_hier"])
        more.addAction(actions["toggle_source_bus"])
        more.addAction(actions["toggle_sink_bus"])

        edit.addMenu(more)
        edit.addAction(actions["properties"])
        menus["edit"] = edit

        # Setup submenu
        panels = Menu("&Panels")
        menus["panels"] = panels
        panels.setEnabled(False)

        toolbars = Menu("&Toolbars")
        menus["toolbars"] = toolbars
        toolbars.setEnabled(False)

        # Setup the view menu
        view = Menu("&View")
        view.addMenu(panels)
        view.addMenu(toolbars)
        view.addSeparator()
        view.addAction(actions["zoom_in"])
        view.addAction(actions["zoom_out"])
        view.addAction(actions["zoom_original"])
        view.addSeparator()
        view.addAction(actions["toggle_grid"])
        view.addAction(actions["find"])
        menus["view"] = view

        # Setup the build menu
        build = Menu("&Build")
        build.addAction(actions["errors"])
        build.addAction(actions["generate"])
        build.addAction(actions["execute"])
        build.addAction(actions["kill"])
        menus["build"] = build

        # Setup the tools menu
        tools = Menu("&Tools")
        tools.addAction(actions["filter_design_tool"])
        tools.addAction(actions["module_browser"])
        tools.addSeparator()
        tools.addAction(actions["start_profiler"])
        tools.addAction(actions["stop_profiler"])
        menus["tools"] = tools

        # Setup the help menu
        help = Menu("&Help")
        help.addAction(actions["help"])
        help.addAction(actions["types"])
        help.addAction(actions["keys"])
        help.addSeparator()
        help.addAction(actions["get_involved"])
        help.addAction(actions["about"])
        help.addAction(actions["about_qt"])
        menus["help"] = help

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")

        # Main toolbar
        file = Toolbar("File")
        file.setObjectName("_FileTb")
        file.addAction(actions["new"])
        file.addAction(actions["open"])
        file.addAction(actions["save"])
        file.addAction(actions["close"])
        toolbars["file"] = file

        # Edit toolbar
        edit = Toolbar("Edit")
        edit.setObjectName("_EditTb")
        edit.addAction(actions["undo"])
        edit.addAction(actions["redo"])
        edit.addSeparator()
        edit.addAction(actions["cut"])
        edit.addAction(actions["copy"])
        edit.addAction(actions["paste"])
        edit.addAction(actions["delete"])
        toolbars["edit"] = edit

        # Run Toolbar
        run = Toolbar("Run")
        run.setObjectName("_RunTb")
        run.addAction(actions["errors"])
        run.addAction(actions["generate"])
        run.addAction(actions["execute"])
        run.addAction(actions["kill"])
        toolbars["run"] = run

        # Misc Toolbar
        misc = Toolbar("Misc")
        misc.setObjectName("_MiscTb")
        misc.addAction(actions["reload"])
        toolbars["misc"] = misc

    def createStatusBar(self):
        log.debug("Creating status bar")
        self.statusBar().showMessage(_("ready-message"))

    def open(self):
        Open = QtWidgets.QFileDialog.getOpenFileName
        filename, filtr = Open(
            self,
            self.actions["open"].statusTip(),
            filter="Flow Graph Files (*.grc);;All files (*.*)",
        )
        return filename

    def save(self):
        Save = QtWidgets.QFileDialog.getSaveFileName
        filename, filtr = Save(
            self,
            self.actions["save"].statusTip(),
            filter="Flow Graph Files (*.grc);;All files (*.*)",
        )
        return filename

    # Overridden methods
    def addDockWidget(self, location, widget):
        """Adds a dock widget to the view."""
        # This overrides QT's addDockWidget so that a 'show' menu auto can automatically be
        # generated for this action.
        super().addDockWidget(location, widget)
        # This is the only instance where a controller holds a reference to a view it does not
        # actually control.
        name = widget.__class__.__name__
        log.debug("Generating show action item for widget: {0}".format(name))

        # Create the new action and wire it to the show/hide for the widget
        self.menus["panels"].addAction(widget.toggleViewAction())
        self.menus["panels"].setEnabled(True)

    def addToolBar(self, toolbar):
        """Adds a toolbar to the main window"""
        # This is also overridden so a show menu item can automatically be added
        super().addToolBar(toolbar)
        name = toolbar.windowTitle()
        log.debug("Generating show action item for toolbar: {0}".format(name))

        # Create the new action and wire it to the show/hide for the widget
        self.menus["toolbars"].addAction(toolbar.toggleViewAction())
        self.menus["toolbars"].setEnabled(True)

    def addMenu(self, menu):
        """Adds a menu to the main window"""
        help = self.menus["help"].menuAction()
        self.menuBar().insertMenu(help, menu)

    def populate_libraries_w_examples(self, example_tuple):
        examples, examples_w_block, designated_examples_w_block = example_tuple
        self.ExampleBrowser.populate(examples)
        self.app.BlockLibrary.populate_w_examples(examples_w_block, designated_examples_w_block)
        self.progress_bar.reset()
        self.progress_bar.hide()
        self.examples_found = True

    @QtCore.Slot(tuple)
    def update_progress_bar(self, progress_tuple):
        progress, msg = progress_tuple
        self.progress_bar.show()
        self.progress_bar.setValue(progress)
        self.progress_bar.setTextVisible(True)
        self.progress_bar.setFormat(msg)

    def connect_fg_signals(self, scene: FlowgraphScene):
        scene.selectionChanged.connect(self.updateActions)
        scene.selectionChanged.connect(self.updateDocTab)
        scene.itemMoved.connect(self.registerMove)
        scene.newElement.connect(self.registerNewElement)
        scene.deleteElement.connect(self.registerDeleteElement)
        scene.blockPropsChange.connect(self.registerBlockPropsChange)
        scene.blockPropsChange.connect(self.registerBlockPropsChange)

    # Action Handlers
    def new_triggered(self):
        log.debug("New")
        fg_view = FlowgraphView(self, self.platform)
        fg_view.centerOn(0, 0)
        initial_state = self.platform.parse_flow_graph("")
        fg_view.scene().import_data(initial_state)
        fg_view.scene().saved = False
        self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.red)
        self.connect_fg_signals(fg_view.scene())
        log.debug("Adding flowgraph view")

        self.tabWidget.addTab(fg_view, "Untitled")
        self.tabWidget.setCurrentIndex(self.tabWidget.count() - 1)

    def open_triggered(self, filename=None, save_allowed=True):
        log.debug("open")
        if not filename:
            filename = self.open()

        if filename:
            log.info("Opening flowgraph ({0})".format(filename))
            new_flowgraph = FlowgraphView(self, self.platform)
            initial_state = self.platform.parse_flow_graph(filename)
            self.tabWidget.addTab(new_flowgraph, os.path.basename(filename))
            self.tabWidget.setCurrentIndex(self.tabWidget.count() - 1)
            self.currentFlowgraphScene.import_data(initial_state)
            self.currentFlowgraphScene.filename = filename
            self.connect_fg_signals(self.currentFlowgraphScene)
            self.currentFlowgraphScene.saved = True
            self.currentFlowgraphScene.save_allowed = save_allowed

    def open_example(self, example_path):
        log.debug("open example")
        if example_path:
            self.open_triggered(example_path, False)

    def save_triggered(self):
        if not self.currentFlowgraphScene.save_allowed:
            self.save_as_triggered()
            return
        log.debug("save")
        filename = self.currentFlowgraphScene.filename

        if filename:
            try:
                self.platform.save_flow_graph(filename, self.currentFlowgraph)
            except IOError:
                log.error("Save failed")
                return

            log.info(f"Saved {filename}")
            self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.white)  # TODO: Not quite the right hue
            self.currentFlowgraphScene.set_saved(True)
        else:
            log.debug("Flowgraph does not have a filename")
            self.save_as_triggered()
        self.updateActions()

    def save_as_triggered(self):
        log.debug("Save As")
        filename, filtr = QtWidgets.QFileDialog.getSaveFileName(
            self,
            self.actions["save"].statusTip(),
            filter="Flow Graph Files (*.grc);;All files (*.*)",
        )
        if filename:
            self.currentFlowgraphScene.filename = filename
            try:
                self.platform.save_flow_graph(filename, self.currentFlowgraph)
            except IOError:
                log.error("Save (as) failed")
                return

            log.info(f"Saved (as) {filename}")
            self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.white)  # TODO: Not quite the right hue
            self.currentFlowgraphScene.set_saved(True)
            self.tabWidget.setTabText(self.tabWidget.currentIndex(), os.path.basename(filename))
        else:
            log.debug("Cancelled Save As action")
        self.updateActions()

    def save_copy_triggered(self):
        log.debug("Save Copy")
        filename, filtr = QtWidgets.QFileDialog.getSaveFileName(
            self,
            self.actions["save"].statusTip(),
            filter="Flow Graph Files (*.grc);;All files (*.*)",
        )
        if filename:
            try:
                self.platform.save_flow_graph(filename, self.currentFlowgraph)
            except IOError:
                log.error("Save (copy) failed")

            log.info(f"Saved (copy) {filename}")
        else:
            log.debug("Cancelled Save Copy action")

    def close_triggered(self, tab_index=None) -> Union[str, bool]:
        """
        Closes a tab.

        Parameters:
            tab_index: specifies which tab to close. If none, close the open tab

        Returns:
            the file path OR True if a tab was closed (False otherwise)
        """
        log.debug(f"Closing a tab (index {tab_index})")

        file_path = self.currentFlowgraphScene.filename
        if tab_index is None:
            tab_index = self.tabWidget.currentIndex()

        if self.currentFlowgraphScene.saved:
            self.tabWidget.removeTab(tab_index)
        else:
            message = None
            if file_path:
                message = f"Save changes to {os.path.basename(file_path)} before closing? Your changes will be lost otherwise."
            else:
                message = "This flowgraph has not been saved"  # TODO: Revise text

            response = QtWidgets.QMessageBox.question(
                None,
                "Save flowgraph?",
                message,
                QtWidgets.QMessageBox.Discard |
                QtWidgets.QMessageBox.Cancel |
                QtWidgets.QMessageBox.Save,
            )


            if response == QtWidgets.QMessageBox.Discard:
                file_path = self.currentFlowgraphScene.filename
                self.tabWidget.removeTab(tab_index)
            elif response == QtWidgets.QMessageBox.Save:
                self.save_triggered()
                if self.currentFlowgraphScene.saved:
                    file_path = self.currentFlowgraphScene.filename
                    self.tabWidget.removeTab(tab_index)
                else:
                    return False
            else:  # Cancel
                return False

        if self.tabWidget.count() == 0:  # No tabs left
            self.new_triggered()
            return True
        else:
            return file_path

    def close_all_triggered(self):
        log.debug("close")

        while self.tabWidget.count() > 1:
            self.close_triggered()
        # Close the final tab
        self.close_triggered()

    def print_triggered(self):
        log.debug("print")

    def screen_capture_triggered(self):
        log.debug("screen capture")
        # TODO: Should be user-set somehow
        background_transparent = True

        Save = QtWidgets.QFileDialog.getSaveFileName
        file_path, filtr = Save(
            self,
            self.actions["save"].statusTip(),
            filter="PDF files (*.pdf);;PNG files (*.png);;SVG files (*.svg)",
        )
        if file_path is not None:
            try:
                Utils.make_screenshot(
                    self.currentView, file_path, background_transparent
                )
            except ValueError:
                log.error("Failed to generate screenshot")

    def undo_triggered(self):
        log.debug("undo")
        self.currentFlowgraphScene.undoStack.undo()
        self.updateActions()

    def redo_triggered(self):
        log.debug("redo")
        self.currentFlowgraphScene.undoStack.redo()
        self.updateActions()

    def view_undo_stack_triggered(self):
        log.debug("view_undo_stack")
        self.undoView = QtWidgets.QUndoView(self.currentFlowgraphScene.undoStack)
        self.undoView.setWindowTitle("Undo stack")
        self.undoView.show()

    def cut_triggered(self):
        log.debug("cut")
        self.copy_triggered()
        self.currentFlowgraphScene.delete_selected()
        self.updateActions()

    def copy_triggered(self):
        log.debug("copy")
        self.clipboard = self.currentFlowgraphScene.copy_to_clipboard()
        self.updateActions()

    def paste_triggered(self):
        log.debug("paste")
        if self.clipboard:
            self.currentFlowgraphScene.paste_from_clipboard(self.clipboard)
            self.currentFlowgraphScene.update()
        else:
            log.debug("clipboard is empty")

    def delete_triggered(self):
        log.debug("delete")
        self.currentFlowgraphScene.set_saved(False)
        self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.red)
        action = DeleteElementAction(self.currentFlowgraphScene)
        self.currentFlowgraphScene.undoStack.push(action)
        self.updateActions()
        self.currentFlowgraphScene.update()

    def select_all_triggered(self):
        log.debug("select_all")
        self.currentFlowgraphScene.select_all()
        self.updateActions()

    def select_none_triggered(self):
        log.debug("select_none")
        self.currentFlowgraphScene.clearSelection()
        self.updateActions()

    def rotate_ccw_triggered(self):
        # Pass to Undo/Redo
        log.debug("rotate_ccw")
        self.currentFlowgraphScene.set_saved(False)
        self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.red)
        rotateCommand = RotateAction(self.currentFlowgraphScene, -90)
        self.currentFlowgraphScene.undoStack.push(rotateCommand)
        self.updateActions()
        self.currentFlowgraphScene.update()

    def rotate_cw_triggered(self):
        # Pass to Undo/Redo
        log.debug("rotate_cw")
        self.currentFlowgraphScene.set_saved(False)
        self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.red)
        rotateCommand = RotateAction(self.currentFlowgraphScene, 90)
        self.currentFlowgraphScene.undoStack.push(rotateCommand)
        self.updateActions()
        self.currentFlowgraphScene.update()

    def toggle_source_bus_triggered(self):
        log.debug("toggle_source_bus")
        self.currentFlowgraphScene.set_saved(False)
        self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.red)
        bussifyCommand = BussifyAction(self.currentFlowgraphScene, 'source')
        self.currentFlowgraphScene.undoStack.push(bussifyCommand)
        self.updateActions()
        self.currentFlowgraphScene.update()

    def toggle_sink_bus_triggered(self):
        log.debug("toggle_sink_bus")
        self.currentFlowgraphScene.set_saved(False)
        self.tabWidget.tabBar().setTabTextColor(self.tabWidget.currentIndex(), Qt.red)
        bussifyCommand = BussifyAction(self.currentFlowgraphScene, 'sink')
        self.currentFlowgraphScene.undoStack.push(bussifyCommand)
        self.updateActions()
        self.currentFlowgraphScene.update()

    def errors_triggered(self):
        log.debug("errors")
        err = ErrorsDialog(self.currentFlowgraph)
        err.exec()

    def module_browser_triggered(self):
        log.debug("oot browser")
        self.OOTBrowser.show()

    def zoom_in_triggered(self):
        log.debug("zoom in")
        self.currentView.zoom(1.1)

    def zoom_out_triggered(self):
        log.debug("zoom out")
        self.currentView.zoom(1.0 / 1.1)

    def zoom_original_triggered(self):
        log.debug("zoom to original size")
        self.currentView.zoomOriginal()

    def find_triggered(self):
        log.debug("find block")
        self._app().BlockLibrary._search_bar.setFocus()

    def get_involved_triggered(self):
        log.debug("get involved")
        ad = QtWidgets.QMessageBox()
        ad.setWindowTitle("Get Involved Instructions")
        ad.setText(
            """\
            <b>Welcome to the GNU Radio Community!</b><br/><br/>
            For more details on contributing to GNU Radio and getting engaged with our great community visit <a href="https://wiki.gnuradio.org/index.php/HowToGetInvolved">here</a>.<br/><br/>
            You can also join our <a href="https://chat.gnuradio.org/">Matrix chat server</a>, IRC Channel (#gnuradio) or contact through our <a href="https://lists.gnu.org/mailman/listinfo/discuss-gnuradio">mailing list (discuss-gnuradio)</a>.
        """
        )
        ad.exec()

    def about_triggered(self):
        log.debug("about")
        config = self.platform.config
        py_version = sys.version.split()[0]
        QtWidgets.QMessageBox.about(
            self, "About GNU Radio", f"GNU Radio {config.version} (Python {py_version})"
        )

    def about_qt_triggered(self):
        log.debug("about_qt")
        QtWidgets.QApplication.instance().aboutQt()

    def properties_triggered(self):
        log.debug("properties")
        if len(self.currentFlowgraphScene.selected_blocks()) != 1:
            log.warn("Opening Properties even though selected_blocks() != 1 ")
        self.currentFlowgraphScene.selected_blocks()[0].open_properties()

    def enable_triggered(self):
        log.debug("enable")
        all_enabled = True
        for block in self.currentFlowgraphScene.selected_blocks():
            if not block.core.state == "enabled":
                all_enabled = False
                break

        if not all_enabled:
            cmd = EnableAction(self.currentFlowgraphScene)
            self.currentFlowgraphScene.undoStack.push(cmd)

        self.currentFlowgraphScene.update()
        self.updateActions()

    def disable_triggered(self):
        log.debug("disable")
        all_disabled = True
        for g_block in self.currentFlowgraphScene.selected_blocks():
            if not g_block.core.state == "disabled":
                all_disabled = False
                break

        if not all_disabled:
            cmd = DisableAction(self.currentFlowgraphScene)
            self.currentFlowgraphScene.undoStack.push(cmd)

        self.currentFlowgraphScene.update()
        self.updateActions()

    def bypass_triggered(self):
        log.debug("bypass")
        all_bypassed = True
        for g_block in self.currentFlowgraphScene.selected_blocks():
            if not g_block.core.state == "bypassed":
                all_bypassed = False
                break

        if not all_bypassed:
            cmd = BypassAction(self.currentFlowgraphScene)
            self.currentFlowgraphScene.undoStack.push(cmd)

        self.currentFlowgraphScene.update()
        self.updateActions()

    def block_inc_type_triggered(self):
        log.debug("block_inc_type")

    def block_dec_type_triggered(self):
        log.debug("block_dec_type")

    def generate_triggered(self):
        log.debug("generate")
        if not self.currentFlowgraphScene.saved:
            self.save_triggered()
        if not self.currentFlowgraphScene.saved:  # The line above was cancelled
            log.error("Cannot generate a flowgraph without saving first")
            return

        filename = self.currentFlowgraphScene.filename
        generator = self.platform.Generator(
            self.currentFlowgraph, os.path.dirname(filename)
        )
        generator.write()
        self.currentView.generator = generator
        log.info(f"Generated {generator.file_path}")

    def execute_triggered(self):
        log.debug("execute")
        if self.currentView.process_is_done():
            self.generate_triggered()
            if self.currentView.generator:
                xterm = self.app.qsettings.value("grc/xterm_executable","")
                '''if self.config.xterm_missing() != xterm:
                    if not os.path.exists(xterm):
                        Dialogs.show_missing_xterm(main, xterm)
                    self.config.xterm_missing(xterm)'''
                if self.currentFlowgraphScene.saved and self.currentFlowgraphScene.filename:
                    # Save config before execution
                    #self.config.save()
                    ExecFlowGraphThread(
                        view=self.currentView,
                        flowgraph=self.currentFlowgraph,
                        xterm_executable=xterm,
                        callback=self.updateActions
                    )

    def kill_triggered(self):
        log.debug("kill")

    def show_help(parent):
        """Display basic usage tips."""
        message = """\
            <b>Usage Tips</b>
            \n\
            <u>Add block</u>: drag and drop or double click a block in the block
        selection window.
            <u>Rotate block</u>: Select a block, press left/right on the keyboard.
            <u>Change type</u>: Select a block, press up/down on the keyboard.
            <u>Edit parameters</u>: double click on a block in the flow graph.
            <u>Make connection</u>: click on the source port of one block, then
        click on the sink port of another block.
            <u>Remove connection</u>: select the connection and press delete, or
        drag the connection.
            \n\
            *Press Ctrl+K or see menu for Keyboard - Shortcuts
            \
        """

        ad = QtWidgets.QMessageBox()
        ad.setWindowTitle("Help")
        ad.setText(message)
        ad.exec()

    def types_triggered(self):
        log.debug("types")
        colors = [(name, color) for name, key, sizeof, color in Constants.CORE_TYPES]

        message = """
        <table>
        <tbody>
        """

        message += "\n".join(
            '<tr bgcolor="{color}"><td><tt>{name}</tt></td></tr>'
            "".format(color=color, name=name)
            for name, color in colors
        )
        message += "</tbody></table>"
        ad = QtWidgets.QMessageBox()
        ad.setWindowTitle("Stream Types")
        ad.setText(message)
        ad.exec()

    def keys_triggered(self):
        log.debug("keys")

        message = """\
            <b>Keyboard Shortcuts</b>
            <br><br>
            <u>Ctrl+N</u>: Create a new flowgraph.<br>
            <u>Ctrl+O</u>: Open an existing flowgraph.<br>
            <u>Ctrl+S</u>: Save the current flowgraph or save as for new.<br>
            <u>Ctrl+W</u>: Close the current flowgraph.<br>
            <u>Ctrl+Z</u>: Undo a change to the flowgraph.<br>
            <u>Ctrl+Y</u>: Redo a change to the flowgraph.<br>
            <u>Ctrl+A</u>: Selects all blocks and connections.<br>
            <u>Ctrl+P</u>: Screen Capture of the Flowgraph.<br>
            <u>Ctrl+Shift+P</u>: Save the console output to file.<br>
            <u>Ctrl+L</u>: Clear the console.<br>
            <u>Ctrl+E</u>: Show variable editor.<br>
            <u>Ctrl+F</u>: Search for a block by name.<br>
            <u>Ctrl+Q</u>: Quit.<br>
            <u>F1</u>    : Help menu.<br>
            <u>F5</u>    : Generate the Flowgraph.<br>
            <u>F6</u>    : Execute the Flowgraph.<br>
            <u>F7</u>    : Kill the Flowgraph.<br>
            <u>Ctrl+Shift+S</u>: Save as the current flowgraph.<br>
            <u>Ctrl+Shift+D</u>: Create a duplicate of current flow graph.<br>

            <u>Ctrl+X/C/V</u>: Edit-cut/copy/paste.<br>
            <u>Ctrl+D/B/R</u>: Toggle visibility of disabled blocks or
                    connections/block tree widget/console.<br>
            <u>Shift+T/M/B/L/C/R</u>: Vertical Align Top/Middle/Bottom and
                    Horizontal Align Left/Center/Right respectively of the
                    selected block.<br>
            <u>Ctrl+0</u>: Reset the zoom level<br>
            <u>Ctrl++/-</u>: Zoom in and out<br>
            \
            """

        ad = QtWidgets.QMessageBox()
        ad.setWindowTitle("Keyboard shortcuts")
        ad.setText(message)
        ad.exec()

    def preferences_triggered(self):
        log.debug("preferences")
        prefs_dialog = PreferencesDialog(self.app.qsettings)
        if prefs_dialog.exec_():  # User pressed Save
            prefs_dialog.save_all()
            self.currentFlowgraphScene.update()

    def example_browser_triggered(self, key_filter: Union[str, None] = None):
        log.debug("example-browser")
        if self.examples_found:
            self.ExampleBrowser.reset()
            ex_dialog = ExampleBrowserDialog(self.ExampleBrowser)
            if len(ex_dialog.browser.examples_dict) == 0:
                ad = QtWidgets.QMessageBox()
                ad.setWindowTitle("GRC: No examples found")
                ad.setText("GRC did not find any examples. Please ensure that the example path in grc.conf is correct.")
                ad.exec()
                return

            if isinstance(key_filter, str):
                if not ex_dialog.browser.filter_(key_filter):
                    ad = QtWidgets.QMessageBox()
                    ad.setWindowTitle("GRC: No examples")
                    ad.setText("There are no examples for this block.")
                    ad.exec()
                    return
            else:
                ex_dialog.browser.reset()

            ex_dialog.exec_()
        else:
            ad = QtWidgets.QMessageBox()
            ad.setWindowTitle("GRC still indexing examples")
            ad.setText("GRC is still indexing examples, please try again shortly.")
            ad.exec()

    def exit_triggered(self):
        log.debug("exit")

        files_open = []
        range_ = reversed(range(self.tabWidget.count()))
        for idx in range_:  # Close the rightmost first. It'll be the first element in files_open
            tab = self.tabWidget.widget(idx)
            file_path = self.currentFlowgraphScene.filename
            if file_path:
                files_open.append(file_path)
            closed = self.close_triggered()
            if closed == False:
                # We cancelled closing a tab. We don't want to close the application
                return

        # Write the leftmost tab to file first
        self.app.qsettings.setValue('window/files_open', reversed(files_open))
        self.app.qsettings.setValue('window/windowState', self.saveState())
        self.app.qsettings.setValue('window/geometry', self.saveGeometry())
        self.app.qsettings.sync()

        # TODO: Make sure all flowgraphs have been saved
        self.app.exit()

    def closeEvent(self, evt):
        log.debug("Close Event")
        self.exit_triggered()

    def help_triggered(self):
        log.debug("help")
        self.show_help()

    def report_triggered(self):
        log.debug("report")

    def library_triggered(self):
        log.debug("library_triggered")

    def library_toggled(self):
        log.debug("library_toggled")

    def filter_design_tool_triggered(self):
        log.debug("filter_design_tool")
        subprocess.Popen(
            "gr_filter_design",
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )

    def start_profiler_triggered(self):
        log.info("Starting profiler")
        self.profiler.enable()

    def stop_profiler_triggered(self):
        self.profiler.disable()
        log.info("Stopping profiler")
        stats = pstats.Stats(self.profiler)
        stats.dump_stats('stats.prof')


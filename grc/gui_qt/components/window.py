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

# Third-party  modules
import six

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.Qt import Qt
from PyQt5.QtGui import QStandardItemModel

# Custom modules
from . import FlowgraphView
from .. import base
from . import RotateCommand

# Logging
log = logging.getLogger(__name__)

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
        base.Component.__init__(self)

        log.debug("Setting the main window")
        self.setObjectName('main')
        self.setWindowTitle(_('window-title'))
        self.setDockOptions(QtWidgets.QMainWindow.AllowNestedDocks |
                            QtWidgets.QMainWindow.AllowTabbedDocks |
                            QtWidgets.QMainWindow.AnimatedDocks)

        # Setup the window icon
        icon = QtGui.QIcon(self.settings.path.ICON)
        log.debug("Setting window icon - ({0})".format(self.settings.path.ICON))
        self.setWindowIcon(icon)

        screen = QtWidgets.QDesktopWidget().availableGeometry()
        log.debug("Setting window size - ({}, {})".format(screen.width(), screen.height()))
        self.resize(int(screen.width() * 0.50), screen.height())

        self.setCorner(Qt.BottomLeftCorner, Qt.LeftDockWidgetArea)

        self.menuBar().setNativeMenuBar(self.settings.window.NATIVE_MENUBAR)

        # TODO: Not sure about document mode
        #self.setDocumentMode(True)

        # Generate the rest of the window
        self.createStatusBar()

        #actions['Quit.triggered.connect(self.close)
        #actions['Report.triggered.connect(self.reportDock.show)
        #QtCore.QMetaObject.connectSlotsByName(self)




        ### Translation support

        #self.setWindowTitle(_translate("blockLibraryDock", "Library", None))
        #library.headerItem().setText(0, _translate("blockLibraryDock", "Blocks", None))
        #QtCore.QMetaObject.connectSlotsByName(blockLibraryDock)

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


        ### Rest of the GUI widgets

        # Map some of the view's functions to the controller class
        self.registerDockWidget = self.addDockWidget
        self.registerMenu = self.addMenu
        self.registerToolBar = self.addToolBar

        # Do other initialization stuff. View should already be allocated and
        # actions dynamically connected to class functions. Also, the log
        # functionality should be also allocated
        log.debug("__init__")

        # Add the menus from the view
        menus = self.menus
        self.registerMenu(menus["file"])
        self.registerMenu(menus["edit"])
        self.registerMenu(menus["view"])
        self.registerMenu(menus["build"])
        self.registerMenu(menus["help"])

        toolbars = self.toolbars
        self.registerToolBar(toolbars["file"])
        self.registerToolBar(toolbars["edit"])
        self.registerToolBar(toolbars["run"])

        log.debug("Loading flowgraph model")
        self.fg_view = FlowgraphView(self)
        initial_state = self.platform.parse_flow_graph("")
        self.fg_view.flowgraph.import_data(initial_state)
        log.debug("Adding flowgraph view")
        self.tabWidget = QtWidgets.QTabWidget()
        self.tabWidget.setTabsClosable(True)
        #TODO: Don't close if the tab has not been saved
        self.tabWidget.tabCloseRequested.connect(lambda index: self.close_triggered(index))
        self.tabWidget.addTab(self.fg_view, "Untitled")
        self.setCentralWidget(self.tabWidget)
        self.currentFlowgraph.selectionChanged.connect(self.updateActions)
        #self.new_tab(self.flowgraph)

    '''def show(self):
        log.debug("Showing main window")
        self.show()
    '''

    @property
    def currentView(self):
        return self.tabWidget.currentWidget()

    @property
    def currentFlowgraph(self):
        return self.tabWidget.currentWidget().flowgraph

    def createActions(self, actions):
        '''
        Defines all actions for this view.
        Controller manages connecting signals to slots implemented in the controller
        '''
        log.debug("Creating actions")

        # File Actions
        actions['new'] = Action(Icons("document-new"), _("new"), self,
                                shortcut=Keys.New, statusTip=_("new-tooltip"))

        actions['open'] = Action(Icons("document-open"), _("open"), self,
                                 shortcut=Keys.Open, statusTip=_("open-tooltip"))

        actions['close'] = Action(Icons("window-close"), _("close"), self,
                                  shortcut=Keys.Close, statusTip=_("close-tooltip"))

        actions['close_all'] = Action(Icons("window-close"), _("close_all"), self,
                                      statusTip=_("close_all-tooltip"))
        actions['save'] = Action(Icons("document-save"), _("save"), self,
                                 shortcut=Keys.Save, statusTip=_("save-tooltip"))

        actions['save_as'] = Action(Icons("document-save-as"), _("save_as"), self,
                                    shortcut=Keys.SaveAs, statusTip=_("save_as-tooltip"))

        actions['print'] = Action(Icons('document-print'), _("print"), self,
                                  shortcut=Keys.Print, statusTip=_("print-tooltip"))

        actions['screen_capture'] = Action(Icons('camera-photo'), _("screen_capture"), self,
                                           statusTip=_("screen_capture-tooltip"))

        actions['exit'] = Action(Icons("application-exit"), _("exit"), self,
                                 shortcut=Keys.Quit, statusTip=_("exit-tooltip"))

        # Edit Actions
        actions['undo'] = Action(Icons('edit-undo'), _("undo"), self,
                                 shortcut=Keys.Undo, statusTip=_("undo-tooltip"))

        actions['redo'] = Action(Icons('edit-redo'), _("redo"), self,
                                 shortcut=Keys.Redo, statusTip=_("redo-tooltip"))

        actions['cut'] = Action(Icons('edit-cut'), _("cut"), self,
                                shortcut=Keys.Cut, statusTip=_("cut-tooltip"))

        actions['copy'] = Action(Icons('edit-copy'), _("copy"), self,
                                 shortcut=Keys.Copy, statusTip=_("copy-tooltip"))

        actions['paste'] = Action(Icons('edit-paste'), _("paste"), self,
                                  shortcut=Keys.Paste, statusTip=_("paste-tooltip"))

        actions['delete'] = Action(Icons('edit-delete'), _("delete"), self,
                                   shortcut=Keys.Delete, statusTip=_("delete-tooltip"))

        actions['select_all'] = Action(Icons('edit-select_all'), _("select_all"), self,
                                   shortcut=Keys.SelectAll, statusTip=_("select_all-tooltip"))

        actions['rotate_ccw'] = Action(Icons('object-rotate-left'), _("rotate_ccw"), self,
                                       shortcut=Keys.MoveToPreviousChar,
                                       statusTip=_("rotate_ccw-tooltip"))

        actions['rotate_cw'] = Action(Icons('object-rotate-right'), _("rotate_cw"), self,
                                      shortcut=Keys.MoveToNextChar,
                                      statusTip=_("rotate_cw-tooltip"))

        actions['enable'] = Action(_("enable"), self,
                                   shortcut="E")
        actions['disable'] = Action(_("disable"), self,
                                   shortcut="D")
        actions['bypass'] = Action(_("bypass"), self)

        actions['vertical_align_top'] = Action(_("vertical_align_top"), self)
        actions['vertical_align_middle'] = Action(_("vertical_align_middle"), self)
        actions['vertical_align_bottom'] = Action(_("vertical_align_bottom"), self)

        actions['horizontal_align_left'] = Action(_("horizontal_align_left"), self)
        actions['horizontal_align_center'] = Action(_("horizontal_align_center"), self)
        actions['horizontal_align_right'] = Action(_("horizontal_align_right"), self)

        actions['create_hier'] = Action(_("create_hier_block"), self)
        actions['open_hier'] = Action(_("open_hier_block"), self)
        actions['toggle_source_bus'] = Action(_("toggle_source_bus"), self)
        actions['toggle_sink_bus'] = Action(_("toggle_sink_bus"), self)

        actions['properties'] = Action(Icons('document-properties'), _("flowgraph-properties"),
                                       self, statusTip=_("flowgraph-properties-tooltip"))

        # View Actions
        actions['errors'] = Action(Icons('dialog-error'), _("errors"), self, shortcut='E',
                                   statusTip=_("errors-tooltip"))

        actions['find'] = Action(Icons('edit-find'), _("find"), self,
                                 shortcut=Keys.Find,
                                 statusTip=_("find-tooltip"))

        # Help Actions
        actions['about'] = Action(Icons('help-about'), _("about"), self,
                                  statusTip=_("about-tooltip"))

        actions['about_qt'] = Action(self.style().standardIcon(QStyle.SP_TitleBarMenuButton), _("about-qt"), self,
                                     statusTip=_("about-tooltip"))

        actions['generate'] = Action(Icons('system-run'), _("process-generate"), self,
                                     shortcut='F5', statusTip=_("process-generate-tooltip"))

        actions['execute'] = Action(Icons('media-playback-start'), _("process-execute"),
                                    self, shortcut='F6',
                                    statusTip=_("process-execute-tooltip"))

        actions['kill'] = Action(Icons('process-stop'), _("process-kill"), self,
                                 shortcut='F7', statusTip=_("process-kill-tooltip"))

        actions['help'] = Action(Icons('help-browser'), _("help"), self,
                                 shortcut=Keys.HelpContents, statusTip=_("help-tooltip"))

        actions['types'] = Action("Types", self)



        actions['preferences'] = Action(Icons('preferences-system'), _("preferences"), self,
                                        statusTip=_("preferences-tooltip"))

        # Disable some actions, by default
        actions['save'].setEnabled(False)
        actions['undo'].setEnabled(False)
        actions['redo'].setEnabled(False)
        actions['cut'].setEnabled(False)
        actions['copy'].setEnabled(False)
        actions['paste'].setEnabled(False)
        actions['delete'].setEnabled(False)
        actions['errors'].setEnabled(False)
        actions['rotate_cw'].setEnabled(False)
        actions['rotate_ccw'].setEnabled(False)
        actions['enable'].setEnabled(False)
        actions['disable'].setEnabled(False)
        actions['bypass'].setEnabled(False)


    def updateActions(self):
        ''' Update the available actions based on what is selected '''

        def there_are_blocks_in(selection):
            for element in selection:
                if element.is_block:
                    return True
            return False

        def there_are_connections_in(selection):
            for element in selection:
                if element.is_connection:
                    return True
            return False

        selected_elements = self.currentFlowgraph.selectedItems()
        undoStack = self.currentFlowgraph.undoStack
        canUndo = undoStack.canUndo()
        canRedo = undoStack.canRedo()

        self.actions['undo'].setEnabled(canUndo)
        self.actions['redo'].setEnabled(canRedo)
        self.actions['cut'].setEnabled(False)
        self.actions['copy'].setEnabled(False)
        self.actions['paste'].setEnabled(False)
        self.actions['delete'].setEnabled(False)
        self.actions['rotate_cw'].setEnabled(False)
        self.actions['rotate_ccw'].setEnabled(False)
        self.actions['enable'].setEnabled(False)
        self.actions['disable'].setEnabled(False)
        self.actions['bypass'].setEnabled(False)

        if there_are_connections_in(selected_elements):
            self.actions['delete'].setEnabled(True)

        if there_are_blocks_in(selected_elements):
            self.actions['cut'].setEnabled(True)
            self.actions['copy'].setEnabled(True)
            self.actions['paste'].setEnabled(True)
            self.actions['delete'].setEnabled(True)
            self.actions['rotate_cw'].setEnabled(True)
            self.actions['rotate_ccw'].setEnabled(True)
            self.actions['enable'].setEnabled(True)
            self.actions['disable'].setEnabled(True)

    def createMenus(self, actions, menus):
        ''' Setup the main menubar for the application '''
        log.debug("Creating menus")

        # Global menu options
        self.menuBar().setNativeMenuBar(True)

        # Setup the file menu
        file = Menu("&File")
        file.addAction(actions['new'])
        file.addAction(actions['open'])
        file.addAction(actions['close'])
        file.addAction(actions['close_all'])
        file.addSeparator()
        file.addAction(actions['save'])
        file.addAction(actions['save_as'])
        file.addSeparator()
        file.addAction(actions['screen_capture'])
        file.addAction(actions['print'])
        file.addSeparator()
        file.addAction(actions['exit'])
        menus['file'] = file

        # Setup the edit menu
        edit = Menu("&Edit")
        edit.addAction(actions['undo'])
        edit.addAction(actions['redo'])
        edit.addSeparator()
        edit.addAction(actions['cut'])
        edit.addAction(actions['copy'])
        edit.addAction(actions['paste'])
        edit.addAction(actions['delete'])
        edit.addAction(actions['select_all'])
        edit.addSeparator()
        edit.addAction(actions['rotate_ccw'])
        edit.addAction(actions['rotate_cw'])

        align = Menu("&Align")
        menus['align'] = align
        align.addAction(actions['vertical_align_top'])
        align.addAction(actions['vertical_align_middle'])
        align.addAction(actions['vertical_align_bottom'])
        align.addSeparator()
        align.addAction(actions['horizontal_align_left'])
        align.addAction(actions['horizontal_align_center'])
        align.addAction(actions['horizontal_align_right'])

        edit.addMenu(align)
        edit.addSeparator()
        edit.addAction(actions['enable'])
        edit.addAction(actions['disable'])
        edit.addAction(actions['bypass'])
        edit.addSeparator()

        more = Menu("&More")
        menus['more'] = more
        more.addAction(actions['create_hier'])
        more.addAction(actions['open_hier'])
        more.addAction(actions['toggle_source_bus'])
        more.addAction(actions['toggle_sink_bus'])

        edit.addMenu(more)
        edit.addAction(actions['properties'])
        menus['edit'] = edit

        # Setup submenu
        panels = Menu("&Panels")
        menus['panels'] = panels
        panels.setEnabled(False)

        toolbars = Menu("&Toolbars")
        menus['toolbars'] = toolbars
        toolbars.setEnabled(False)

        # Setup the view menu
        view = Menu("&View")
        view.addMenu(panels)
        view.addMenu(toolbars)
        view.addSeparator()
        view.addAction(actions['errors'])
        view.addAction(actions['find'])
        menus['view'] = view

        # Setup the build menu
        build = Menu("&Build")
        build.addAction(actions['generate'])
        build.addAction(actions['execute'])
        build.addAction(actions['kill'])
        menus['build'] = build

        # Setup the help menu
        help = Menu("&Help")
        help.addAction(actions['help'])
        help.addAction(actions['types'])
        help.addSeparator()
        help.addAction(actions['about'])
        help.addAction(actions['about_qt'])
        menus['help'] = help

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")

        # Main toolbar
        file = Toolbar("File")
        file.addAction(actions['new'])
        file.addAction(actions['open'])
        file.addAction(actions['save'])
        file.addAction(actions['close'])
        file.addAction(actions['print'])
        toolbars['file'] = file

        # Edit toolbar
        edit = Toolbar("Edit")
        edit.addAction(actions['undo'])
        edit.addAction(actions['redo'])
        edit.addSeparator()
        edit.addAction(actions['cut'])
        edit.addAction(actions['copy'])
        edit.addAction(actions['paste'])
        edit.addAction(actions['delete'])
        edit.addSeparator()
        edit.addAction(actions['rotate_ccw'])
        edit.addAction(actions['rotate_cw'])
        toolbars['edit'] = edit

        # Run Toolbar
        run = Toolbar('Run')
        run.addAction(actions['generate'])
        run.addAction(actions['execute'])
        run.addAction(actions['kill'])
        toolbars['run'] = run

    def createStatusBar(self):
        log.debug("Creating status bar")
        self.statusBar().showMessage(_("ready-message"))

    def new_tab(self, flowgraph):
        self.setCentralWidget(flowgraph)

    def open(self):
        Open = QtWidgets.QFileDialog.getOpenFileName
        filename, filtr = Open(self, self.actions['open'].statusTip(),
                               filter='Flow Graph Files (*.grc);;All files (*.*)')
        return filename

    def save(self):
        Save = QtWidgets.QFileDialog.getSaveFileName
        filename, filtr = Save(self, self.actions['save'].statusTip(),
                               filter='Flow Graph Files (*.grc);;All files (*.*)')
        return filename

    # Overridden methods
    def addDockWidget(self, location, widget):
        ''' Adds a dock widget to the view. '''
        # This overrides QT's addDockWidget so that a 'show' menu auto can automatically be
        # generated for this action.
        super().addDockWidget(location, widget)
        # This is the only instance where a controller holds a reference to a view it does not
        # actually control.
        name = widget.__class__.__name__
        log.debug("Generating show action item for widget: {0}".format(name))

        # Create the new action and wire it to the show/hide for the widget
        self.menus["panels"].addAction(widget.toggleViewAction())
        self.menus['panels'].setEnabled(True)

    def addToolBar(self, toolbar):
        ''' Adds a toolbar to the main window '''
        # This is also overridden so a show menu item can automatically be added
        super().addToolBar(toolbar)
        name = toolbar.windowTitle()
        log.debug("Generating show action item for toolbar: {0}".format(name))

        # Create the new action and wire it to the show/hide for the widget
        self.menus["toolbars"].addAction(toolbar.toggleViewAction())
        self.menus['toolbars'].setEnabled(True)

    def addMenu(self, menu):
        ''' Adds a menu to the main window '''
        help = self.menus["help"].menuAction()
        self.menuBar().insertMenu(help, menu)

    # Action Handlers
    def new_triggered(self):
        log.debug('new file')

    def open_triggered(self):
        log.debug('open')
        filename = self.open()

        if filename:
            log.info("Opening flowgraph ({0})".format(filename))
            new_flowgraph = FlowgraphView(self)
            initial_state = self.platform.parse_flow_graph(filename)
            self.tabWidget.addTab(new_flowgraph, os.path.basename(filename))
            self.tabWidget.setCurrentIndex(self.tabWidget.count() - 1)
            self.currentFlowgraph.import_data(initial_state)
            self.currentFlowgraph.selectionChanged.connect(self.updateActions)

    def save_triggered(self):
        log.debug('save')

    def save_as_triggered(self):
        log.debug('save as')
        filename = self.save()

        if filename:
            try:
                self.file_path = filename
                self.platform.save_flow_graph(filename, self.fg_view)
            except IOError:
                log.error('Save failed')

        log.info(filename)

    def close_triggered(self, tab_index=None):
        log.debug('close')
        if tab_index is None:
            self.tabWidget.removeTab(self.tabWidget.currentIndex())
        else:
            # TODO: Only if saved
            self.tabWidget.removeTab(tab_index)

    def close_all_triggered(self):
        log.debug('close')

    def print_triggered(self):
        log.debug('print')

    def screen_capture_triggered(self):
        log.debug('screen capture')

    def undo_triggered(self):
        log.debug('undo')
        self.currentFlowgraph.undoStack.undo()
        self.updateActions()

    def redo_triggered(self):
        log.debug('redo')
        self.currentFlowgraph.undoStack.redo()
        self.updateActions()

    def cut_triggered(self):
        log.debug('cut')

    def copy_triggered(self):
        log.debug('copy')

    def paste_triggered(self):
        log.debug('paste')

    def delete_triggered(self):
        log.debug('delete')
        self.currentFlowgraph.delete_selected()

    def rotate_ccw_triggered(self):
        log.debug('rotate_ccw')
        rotateCommand = RotateCommand(self.currentFlowgraph, -90)
        self.currentFlowgraph.undoStack.push(rotateCommand)
        self.updateActions()

    def rotate_cw_triggered(self):
        log.debug('rotate_cw')
        rotateCommand = RotateCommand(self.currentFlowgraph, 90)
        self.currentFlowgraph.undoStack.push(rotateCommand)
        self.updateActions()

    def errors_triggered(self):
        log.debug('errors')

    def find_triggered(self):
        log.debug('find block')
        self._app().BlockLibrary._search_bar.setFocus()

    def about_triggered(self):
        log.debug('about')
        self.about()

    def about_qt_triggered(self):
        log.debug('about_qt')
        QtWidgets.QApplication.instance().aboutQt()

    def properties_triggered(self):
        log.debug('properties')

    def enable_triggered(self):
        log.debug('enable')
        for block in self.currentFlowgraph.selected_blocks():
            block.state = 'enabled'
            block.create_shapes_and_labels()

    def disable_triggered(self):
        log.debug('disable')
        for block in self.currentFlowgraph.selected_blocks():
            block.state = 'disabled'
            block.create_shapes_and_labels()

    def execute_triggered(self):
        log.debug('execute')
        py_path = self.file_path[0:-3] + 'py'
        subprocess.Popen(f'/usr/bin/python {py_path}', shell=True)

    def generate_triggered(self):
        log.debug('generate')
        generator = self.platform.Generator(self.currentFlowgraph, os.path.dirname(self.file_path))
        generator.write()

    def types_triggered(self):
        log.debug('types')
        self.types()

    def preferences_triggered(self):
        log.debug('preferences')

    def exit_triggered(self):
        log.debug('exit')
        # TODO: Make sure all flowgraphs have been saved
        self.app.exit()

    def help_triggered(self):
        log.debug('help')
        self.help()

    def kill_triggered(self):
        log.debug('kill')

    def report_triggered(self):
        log.debug('report')

    def library_triggered(self):
        log.debug('library_triggered')

    def library_toggled(self):
        log.debug('library_toggled')

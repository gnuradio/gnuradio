import os
from PyQt5 import QtCore

# GRC imports
from .. import views, base


class MainWindow(base.Controller):
    """ GRC.Controllers.MainWindow - Controls main window view """

    def __init__(self):
        # Required function calls
        super().__init__()
        self.setView(views.MainWindow)

        # Map some of the view's functions to the controller class
        self.registerDockWidget = self.view.addDockWidget
        self.registerMenu = self.view.addMenu
        self.registerToolBar = self.view.addToolBar

        # Do other initialization stuff. View should already be allocated and
        # actions dynamically connected to class functions. Also, the self.log
        # functionality should be also allocated
        self.log.debug("__init__")

        # Add the menus from the view
        menus = self.view.menus
        self.registerMenu(menus["file"])
        self.registerMenu(menus["edit"])
        self.registerMenu(menus["view"])
        self.registerMenu(menus["build"])
        self.registerMenu(menus["help"])

        toolbars = self.view.toolbars
        self.registerToolBar(toolbars["file"])
        self.registerToolBar(toolbars["edit"])
        self.registerToolBar(toolbars["run"])

        self.log.debug("Loading flowgraph model")
        test_flowgraph = os.path.join(self.gp.path.INSTALL, 'companion/resources/data/rx_logo.grc')
        self.flowgraph = views.FlowGraph(self.view, test_flowgraph)
        self.log.debug("Adding flowgraph view")
        self.view.new_tab(self.flowgraph)

    def show(self):
        self.log.debug("Showing main window")
        self.view.show()

    # Action Handlers
    def new_triggered(self):
        self.log.debug('new file')

    def open_triggered(self):
        self.log.debug('open')
        filename = self.view.open()

        if filename:
            self.log.info("Opening flowgraph ({0})".format(filename))
            self.flowgraph = views.FlowGraph(self.view, filename)
            self.view.new_tab(self.flowgraph)

    def save_triggered(self):
        self.log.debug('save')

    def save_as_triggered(self):
        self.log.debug('save')

    def close_triggered(self):
        self.log.debug('close')

    def close_all_triggered(self):
        self.log.debug('close')

    def print_triggered(self):
        self.log.debug('print')

    def screen_capture_triggered(self):
        self.log.debug('screen capture')

    def undo_triggered(self):
        self.log.debug('undo')

    def redo_triggered(self):
        self.log.debug('redo')

    def cut_triggered(self):
        self.log.debug('cut')

    def copy_triggered(self):
        self.log.debug('copy')

    def paste_triggered(self):
        self.log.debug('paste')

    def delete_triggered(self):
        self.log.debug('delete')

    def rotate_ccw_triggered(self):
        self.log.debug('rotate_ccw')

    def rotate_cw_triggered(self):
        self.log.debug('rotate_cw')

    def errors_triggered(self):
        self.log.debug('errors')

    def find_triggered(self):
        self.log.debug('find block')

    def about_triggered(self):
        self.log.debug('about')
        self.view.about()

    def about_qt_triggered(self):
        self.log.debug('about_qt')
        self.view.aboutQt()

    def properties_triggered(self):
        self.log.debug('properties')

    def enable_triggered(self):
        self.log.debug('enable')

    def disable_triggered(self):
        self.log.debug('disable')

    def execute_triggered(self):
        self.log.debug('execute')

    def generate_triggered(self):
        self.log.debug('generate')

    def types_triggered(self):
        self.log.debug('types')
        self.view.types()

    def preferences_triggered(self):
        self.log.debug('preferences')

    def exit_triggered(self):
        self.log.debug('exit')

    def help_triggered(self):
        self.log.debug('help')
        self.view.help()

    def kill_triggered(self):
        self.log.debug('kill')

    def report_triggered(self):
        self.log.debug('report')

    def library_triggered(self):
        self.log.debug('library_triggered')

    def library_toggled(self):
        self.log.debug('library_toggled')

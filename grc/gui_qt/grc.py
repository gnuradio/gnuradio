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
import textwrap

# Third-party  modules
import six

from PyQt5 import QtCore, QtGui, QtWidgets

# Custom modules
from . import base
from . import components
from .helpers.profiling import StopWatch

# Logging
# Setup the logger to use a different name than the file name
log = logging.getLogger('grc.application')


class Application(QtWidgets.QApplication):
    '''
    This is the main QT application for GRC.
    It handles setting up the application components and actions and handles communication between different components in the system.
    '''

    def __init__(self, settings, platform):
        # Note. Logger must have the correct naming convention to share handlers
        log.debug("__init__")

        log.debug("Creating QApplication instance")
        QtWidgets.QApplication.__init__(self, settings.argv)

        # Save references to the global settings and gnuradio platform
        self.settings = settings
        self.platform = platform


        # Load the main view class and initialize QMainWindow
        log.debug("ARGV - {0}".format(settings.argv))
        log.debug("INSTALL_DIR - {0}".format(settings.path.INSTALL))

        # Global signals
        self.signals = {}

        # Setup the main application window
        log.debug("Creating main application window")
        stopwatch = StopWatch()
        self.MainWindow = components.MainWindow()
        stopwatch.lap('mainwindow')
        self.Console = components.Console()
        stopwatch.lap('console')
        self.BlockLibrary = components.BlockLibrary()
        stopwatch.lap('blocklibrary')
        self.DocumentationTab = components.DocumentationTab()
        stopwatch.lap('documentationtab')

        # Debug times
        log.debug("Loaded MainWindow controller - {:.4f}s".format(stopwatch.elapsed("mainwindow")))
        log.debug("Loaded Console component - {:.4f}s".format(stopwatch.elapsed("console")))
        log.debug("Loaded BlockLibrary component - {:.4}s".format(stopwatch.elapsed("blocklibrary")))
        log.debug("Loaded DocumentationTab component - {:.4}s".format(stopwatch.elapsed("documentationtab")))

        # Print Startup information once everything has loaded
        log.critical("TODO: Change welcome message.")

        welcome = '''
        linux; GNU C++ version 4.8.2; Boost_105400; UHD_003.007.002-94-ge56809a0

        <<< Welcome to GNU Radio Companion 3.7.6git-1-g01deede >>>

        Preferences file: /home/seth/.grc
        Block paths:
          /home/seth/Dev/gnuradio/target/share/gnuradio/grc/blocks
          /home/seth/.grc_gnuradio
        Loading: \"/home/seth/Dev/persistent-ew/gnuradio/target/flex_rx.grc\"
        '''

        config = platform.config
        paths="\n\t".join(platform.config.block_paths)
        welcome = f"<<< Welcome to {config.name} {config.version} >>>\n\n" \
                  f"Preferences file: {config.gui_prefs_file}\n" \
                  f"Block paths:\n\t{paths}\n"
        log.info(textwrap.dedent(welcome))


    # Global registration functions
    #  - Handles the majority of child controller interaciton

    def registerSignal(self, signal):
        pass

    def registerDockWidget(self, widget, location=0):
        ''' Allows child controllers to register a widget that can be docked in the main window '''
        # TODO: Setup the system to automatically add new "Show <View Name>" menu items when a new
        # dock widget is added.
        log.debug("Registering widget ({0}, {1})".format(widget.__class__.__name__, location))
        self.MainWindow.registerDockWidget(location, widget)

    def registerMenu(self, menu):
        ''' Allows child controllers to register an a menu rather than just a single action '''
        # TODO: Need to have several capabilities:
        #  - Menu's need the ability to have a priority for ordering
        #  - When registering, the controller needs to specific target menu
        #  - Automatically add seporators (unless it is the first or last items)
        #  - Have the ability to add it as a sub menu
        #  - MainWindow does not need to call register in the app controller. It can call directly
        #  - Possibly view sidebars and toolbars as submenu
        #  - Have the ability to create an entirely new menu
        log.debug("Registering menu ({0})".format(menu.title()))
        self.MainWindow.registerMenu(menu)

    def registerAction(self, action, menu):
        ''' Allows child controllers to register a global action shown in the main window '''
        pass

    def run(self):
        ''' Launches the main QT event loop '''
        # Show the main window after everything is initialized.
        self.MainWindow.show()
        return (self.exec_())

import logging
import textwrap

from PyQt5 import QtWidgets

# GRC Imports
from . import base            # Base classes
from . import controllers     # Internal controllers
from . import components

from .helpers.profiling import StopWatch

class Application(QtWidgets.QApplication):
    '''
    This is the main QT application for GRC.
    It handles setting up the application components and actions and handles communication between different components in the system.
    '''

    def __init__(self, settings, platform):
        # Note. Logger must have the correct naming convention to share handlers
        self.log = logging.getLogger("grc.app")
        self.log.debug("__init__")

        self.log.debug("Creating QApplication instance")
        QtWidgets.QApplication.__init__(self, settings.argv)

        # Save references to the global settings and gnuradio platform
        self.settings = settings
        self.platform = platform


        # Load the main view class and initialize QMainWindow
        self.log.debug("ARGV - {0}".format(settings.argv))
        self.log.debug("INSTALL_DIR - {0}".format(settings.path.INSTALL))

        # Global signals
        self.signals = {}

        # Setup the main application window
        self.log.debug("Creating main application window")
        stopwatch = StopWatch()
        self.MainWindow = controllers.MainWindow()
        stopwatch.lap('mainwindow')
        self.Reports = controllers.Reports()
        stopwatch.lap('reports')
        self.BlockLibrary = components.BlockLibrary()
        stopwatch.lap('blocklibrary')

        # Debug times
        self.log.debug("Loaded MainWindow controller - {:.4f}s".format(stopwatch.elapsed("mainwindow")))
        self.log.debug("Loaded Reports controller - {:.4f}s".format(stopwatch.elapsed("reports")))
        self.log.debug("Loaded BlockLibrary controller - {:.4}s".format(stopwatch.elapsed("blocklibrary")))

        # Print Startup information once everything has loaded
        self.log.critical("TODO: Change welcome message.")

        welcome = '''
        linux; GNU C++ version 4.8.2; Boost_105400; UHD_003.007.002-94-ge56809a0

        <<< Welcome to GNU Radio Companion 3.7.6git-1-g01deede >>>

        Preferences file: /home/seth/.grc
        Block paths:
          /home/seth/Dev/gnuradio/target/share/gnuradio/grc/blocks
          /home/seth/.grc_gnuradio
        Loading: \"/home/seth/Dev/persistent-ew/gnuradio/target/flex_rx.grc\"
        '''
        self.log.info(textwrap.dedent(welcome))


    # Global registration functions
    #  - Handles the majority of child controller interaciton

    def registerSignal(self, signal):
        pass

    def registerDockWidget(self, widget, location=0):
        ''' Allows child controllers to register a widget that can be docked in the main window '''
        # TODO: Setup the system to automatically add new "Show <View Name>" menu items when a new
        # dock widget is added.
        self.log.debug("Registering widget ({0}, {1})".format(widget.__class__.__name__, location))
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
        self.log.debug("Registering menu ({0})".format(menu.title()))
        self.MainWindow.registerMenu(menu)

    def registerAction(self, action, menu):
        ''' Allows child controllers to register a global action shown in the main window '''
        pass

    def run(self):
        ''' Launches the main QT event loop '''
        # Show the main window after everything is initialized.
        self.MainWindow.show()
        return (self.exec_())

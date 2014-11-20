import logging
import textwrap
from PyQt5 import QtWidgets

# GRC Imports
from . import base            # Base classes
from . import controllers     # Internal controllers


class AppController(object):
    """
    Main controller everything;
    handles setting up child controllers, views, and global actions.
    """

    def __init__(self, gp):
        # Note. Logger must have the correct naming convention to share
        # handlers
        self.log = logging.getLogger("grc.app")
        self.log.debug("__init__")

        # Save the global preferences
        self.gp = gp

        # Setup the base classes
        base.Controller.setAttributes(self, gp)
        base.View.setAttributes(gp)

        # Load the main view class and initialize QMainWindow
        self.log.debug("ARGV - {0}".format(gp.argv))
        self.log.debug("INSTALL_DIR - {0}".format(gp.path.INSTALL))

        # Global signals
        self.signals = {}

        self.log.debug("Creating QApplication instance")
        self._app = QtWidgets.QApplication(gp.argv)

        import time
        # Need to setup the slots for the QtAction
        self.log.debug("Loading MainWindow controller")
        start = time.time()
        self.MainWindow = controllers.MainWindow()
        self.log.debug ("Elapsed - " + "{0:0.1f}ms".format((time.time() - start) * 1000))

        self.log.debug("Loading Reports controller")
        self.Reports = controllers.Reports()
        self.log.debug ("Elapsed - " + "{0:0.1f}ms".format((time.time() - start) * 1000))

        self.log.debug("Loading BlockLibrary controller")
        self.BlockLibrary = controllers.BlockLibrary()
        self.log.debug("Elapsed - " + "{0:0.1f}ms".format((time.time() - start) * 1000))

        # Print Startup information once everything has loaded
        self.log.critical("TODO: Change welcome message.")

        welcome = """
        linux; GNU C++ version 4.8.2; Boost_105400; UHD_003.007.002-94-ge56809a0

        <<< Welcome to GNU Radio Companion 3.7.6git-1-g01deede >>>

        Preferences file: /home/seth/.grc
        Block paths:
          /home/seth/Dev/gnuradio/target/share/gnuradio/grc/blocks
          /home/seth/.grc_gnuradio
        Loading: \"/home/seth/Dev/persistent-ew/gnuradio/target/flex_rx.grc\"
        """
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
        return (self._app.exec_())

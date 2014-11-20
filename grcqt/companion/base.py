import abc
import logging
import weakref

# GRC imports
from . import helpers


class Controller(object):
    """ Abstract base class for all grc controllers. """
    __metaclass__ = abc.ABCMeta

    def __init__(self):
        """
        Initializes the controller base class.
        Sets up a default logger that can be overridden by the child class.
        """
        name = "grc.controller.{0}".format(self.__class__.__name__.lower())
        self.log = logging.getLogger(name)
        self.log.debug("Using default logger - {0}".format(name))

        self.view = None

    @classmethod
    def setAttributes(cls, app, gp):
        """
        Adds references for the app controller and global properties as
        attributes of the base class so that they are automatically inherited
        by the children.
        """
        # Need to use a weak references for the app controller due to circular references.
        # Otherwise the python gc will not correctly recover objects
        cls.appRef = weakref.ref(app)
        cls.gp = gp

    @property
    def app(self):
        """ Returns a referent for the weakref to the app controller"""
        return self.appRef()

    # Controller setup functions
    def setView(self, view):
        # Make sure the view is a subclass of the base view
        # Allocate the view (Should build actions and all)
        self.view = view()
        self.view.setController(self)

        # Dynamically build connection for the available signals
        self.log.debug("Connecting signals")
        self.connectSlots()

    # Base methods
    def setLogger(self, loggerName):
        """ Replaces the default logger """
        del self.log    # Get rid of the original logger since it is being replaced
        self.log = logging.getLogger(loggerName)
        self.log.debug("Using custom logger - {0}".format(loggerName))

    def connectSlots(self, useToggled=True, toggledHandler='_toggled',
                     triggeredHandler="_triggered"):
        '''
         Handles connecting signals from given actions to handlers
            self    - Calling class
            actions - Dictionary of a QAction and unique key

         Dynamically build the connections for the signals by finding the correct
         function to handle an action. Default behavior is to connect checkable actions to
         the 'toggled' signal and normal actions to the 'triggered' signal. If 'toggled' is
         not avaliable or useToggled is set to False then try to connect it to triggered.
         Both toggled and triggered are called for checkable items, so there is no need for
         both to be connected.

            void QAction::toggled ( bool checked ) [signal]
            void QAction::triggered ( bool checked = false ) [signal]
              - Checked is set for checkable actions

         Essentially the same as QMetaObject::connectSlotsByName, except the actions
          and slots can be separated into a view and controller class

        '''
        actions = self.view.actions
        for key in actions:
            if useToggled and actions[key].isCheckable():
                # Try to use toggled rather than triggered
                try:
                    handler = key + toggledHandler
                    actions[key].toggled.connect(getattr(self, handler))
                    self.log.debug("<{0}.toggled> connected to handler <{1}>".format(key, handler))
                    # Successful connection. Jump to the next action.
                    continue
                except:
                    # Default to the triggered handler
                    self.log.warning("Could not connect <{0}.toggled> to handler <{1}>".format(key,
                                     handler))

            # Try and bind the 'triggered' signal to a handler.
            try:
                handler = key + triggeredHandler
                actions[key].triggered.connect(getattr(self, handler))
                self.log.debug("<{0}.triggered> connected to handler <{1}>".format(key, handler))
            except:
                try:
                    self.log.warning("Handler not implemented for <{0}.triggered> in {1}".format(
                                     key, type(self).__name__))
                    actions[key].triggered.connect(getattr(self, 'notImplemented'))
                except:
                    # This should never happen
                    self.log.error("Class cannot handle <{0}.triggered>".format(key))

    def notImplemented(self):
        self.log.warning('Not implemented')


class View(object):
    """ Abstract base class for all grc views. """
    __metaclass__ = abc.ABCMeta

    def __init__(self):
        """
        Initializes the view base class.
        Sets up a default logger that can be overridden by a child and calls createActions
        to initialize the view's actions.
        """
        name = "grc.view.{0}".format(self.__class__.__name__.lower())
        self.log = logging.getLogger(name)
        self.log.debug("Using default logger - {0}".format(name))

        # Automatically create the actions, menus and toolbars.
        # Child controllers need to call the register functions to integrate into the mainwindow
        self.actions = {}
        self.menus = {}
        self.toolbars = {}
        self.createActions(self.actions)
        self.createMenus(self.actions, self.menus)
        self.createToolbars(self.actions, self.toolbars)

    # Required methods
    @abc.abstractmethod
    def createActions(self, actions):
        """ Add actions to the view. """
        raise NotImplementedError()

    @abc.abstractmethod
    def createMenus(self, actions, menus):
        """ Add actions to the view. """
        raise NotImplementedError()

    @abc.abstractmethod
    def createToolbars(self, actions, toolbars):
        """ Add actions to the view. """
        raise NotImplementedError()

    # Class methods
    @classmethod
    def setAttributes(cls, gp):
        """ Adds reference for the global properties as an attributes of the base class. """
        # Cannot set 'self.controller' because each view has a different corresponding controller.
        cls.gp = gp

    # Base methods
    def setLogger(self, loggerName):
        """ Replaces the default logger. """
        del self.log    # Get rid of the original logger since it is being replaced
        self.log = logging.getLogger(loggerName)
        self.log.debug("Using custom logger - {0}".format(loggerName))

    def setController(self, controller):
        """
        Creates a weak reference to the owning controller.
        Called by controller after the view has been initialized.
        """
        self.controller = weakref.ref(controller)

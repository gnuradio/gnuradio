# GRC imports
import cgi
import logging
from .. import views, base


class Reports(base.Controller):
    """ GRC.Controllers.Reports """

    def __init__(self):
        # Required function calls
        super().__init__()
        self.setView(views.Reports)

        # Do other initialization stuff. View should already be allocated and
        # actions dynamically connected to class functions. Also, the self.log
        # functionality should be also allocated
        self.log.debug("__init__")
        self.log.debug("Registering dock widget")
        # Register the dock widget through the AppController.
        # The AppController then tries to find a saved dock location from the preferences
        # before calling the MainWindow Controller to add the widget.
        self.app.registerDockWidget(self.view, location=self.gp.window.REPORTS_DOCK_LOCATION)

        # Register the menus
        self.app.registerMenu(self.view.menus["reports"])

        # Register a new handler for the root logger that outputs messages of
        #  INFO and HIGHER to the reports view
        handler = ReportsHandler(self.add_line)
        handler.setLevel(logging.INFO)

        # Need to add this handler to the parent of the controller's logger
        self.log.parent.addHandler(handler)
        self.handler = handler

        self.view.actions['show_level'].setChecked = True
        self.handler.show_level = True

    def add_line(self, line):
        # TODO: Support multiple columns for the HTML. DO better with the spacing
        #  and margins in the output

        self.view.text.append(line)

    # Handlers for the view actions
    def clear_triggered(self):
        self.view.text.clear()

    def save_triggered(self):
        pass

    def show_level_toggled(self, checked):
        self.handler.show_level = checked


class ReportsHandler(logging.Handler):  # Inherit from logging.Handler
    """ Writes out logs to the reporst window """

    def __init__(self, add_line, show_level=True, short_level=True):
        # run the regular Handler __init__
        logging.Handler.__init__(self)

        self.add_line = add_line  # Function for adding a line to the view
        self.show_level = show_level  # Dynamically show levels
        self.short_level = short_level  # Default to true, changed by properties

        self.formatLevelLength = self.formatLevelShort
        if not short_level:
            self.formatLevelLength = self.formatLevelLong

    def emit(self, record):
        # Just handle all formatting here
        if self.show_level:
            level = self.formatLevel(record.levelname)
            message = cgi.escape(record.msg)
            output = self.formatOutput()
            self.add_line(output.format(level, message))
        else:
            message = cgi.escape(record.msg)
            output = self.formatOutput()
            self.add_line(output.format(message))

    def formatLevel(self, levelname):
        output = "{0}{1}{2}"
        level = self.formatLevelLength(levelname)
        if levelname == "INFO":
            return output.format("<font color=\"Green\"><b>", level, "</b></font>")
        elif levelname == "WARNING":
            return output.format("<font color=\"Orange\"><b>", level, "</b></font>")
        elif levelname == "ERROR":
            return output.format("<font color=\"Red\"><b>", level, "</b></font>")
        elif levelname == "CRITICAL":
            return output.format("<font color=\"Red\"><b>", level, "</b></font>")
        else:
            return output.format("<font color=\"Blue\"><b>", level, "</b></font>")

    def formatLevelShort(self, levelname):
        return "[{0}]".format(levelname[0:1])

    def formatLevelLong(self, levelname):
        output = "{0:<10}"
        if levelname in ["DEBUG", "INFO", "WARNING"]:
            return output.format("[{0}]".format(levelname.capitalize()))
        else:
            return output.format("[{0}]".format(levelname.upper()))

    def formatOutput(self):
        ''' Returns the correct output format based on internal settings '''
        if self.show_level:
            if self.short_level:
                return "<tr><td width=\"25\">{0}</td><td><pre>{1}</pre></td></tr>"
            return "<tr><td width=\"75\">{0}</td><td><pre>{1}</pre></td></tr>"
        return "<tr><td><pre>{0}</pre></td></tr>"

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
import html
import logging
import textwrap

# Third-party  modules
import six

from PyQt5 import QtCore, QtGui, QtWidgets

# Custom modules
from .. import base

# Shortcuts
Action = QtWidgets.QAction
Menu = QtWidgets.QMenu
Toolbar = QtWidgets.QToolBar
Icons = QtGui.QIcon.fromTheme
Keys = QtGui.QKeySequence

# Logging
log = logging.getLogger(__name__)


HTML = '''
<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\"
\"http://www.w3.org/TR/REC-html40/strict.dtd\">
<html>
<head>
    <meta name=\"qrichtext\" content=\"1\" />
    <style type=\"text/css\">
        p, li {
            white-space: pre-wrap;
        }

        /* Not currently used because entries are wrapped with <pre> */
        body {
            /* font-family:  \'Ubuntu\'; */
            font-family:  \'MS Shell Dlg 2\';
            font-size:    10pt;
            font-weight:  400;
            font-style:   normal;
            font-color:   black;
            margin:       0px;
            text-indent:  0px;
            -qt-block-indent:   0;
        }

        pre {
            display: block;
            font-family: monospace;
            white-space: pre;
            font-color:  black;
            margin:      1em 0;
        }

    </style>
</head>
</html>
'''


class Console(QtWidgets.QDockWidget, base.Component):

    def __init__(self):
        QtWidgets.QDockWidget.__init__(self)
        base.Component.__init__(self)

        self.setObjectName('console')
        self.setWindowTitle('Console')

        ### GUI Widgets

        # Create the layout widget
        container = QtWidgets.QWidget(self)
        container.setObjectName('console::container')
        self._container = container

        layout = QtWidgets.QHBoxLayout(container)
        layout.setObjectName('console::layout')
        layout.setSpacing(0)
        layout.setContentsMargins(5, 0, 5, 5)
        self._layout = layout

        # Console output widget
        text = QtWidgets.QTextEdit(container)
        text.setObjectName('console::text')
        text.setUndoRedoEnabled(False)
        text.setReadOnly(True)
        text.setCursorWidth(0)
        text.setTextInteractionFlags(QtCore.Qt.TextSelectableByKeyboard | QtCore.Qt.TextSelectableByMouse)
        text.setHtml(textwrap.dedent(HTML))
        self._text = text

        # Add widgets to the component
        layout.addWidget(text)
        container.setLayout(layout)
        self.setWidget(container)

        ### Translation support

        #self.setWindowTitle(_translate("", "Library", None))
        #library.headerItem().setText(0, _translate("", "Blocks", None))
        #QtCore.QMetaObject.connectSlotsByName(blockLibraryDock)

        ### Setup actions

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
        self.app.registerDockWidget(self, location=self.settings.window.REPORTS_DOCK_LOCATION)

        # Register the menus
        self.app.registerMenu(self.menus["reports"])

        # Register a new handler for the root logger that outputs messages of
        #  INFO and HIGHER to the reports view
        handler = ReportsHandler(self.add_line)
        handler.setLevel(logging.DEBUG)

        # Need to add this handler to the parent of the controller's logger
        log.parent.addHandler(handler)
        self.handler = handler

        self.actions['show_level'].setChecked = True
        self.handler.show_level = True


    ### Actions

    def createActions(self, actions):
        ''' Defines all actions for this view. '''

        log.debug("Creating actions")

        # File Actions
        actions['save'] = Action(Icons("document-save"), _("save"), self,
                                statusTip=_("save-tooltip"))
        actions['clear'] = Action(Icons("document-close"), _("clear"), self,
                                statusTip=_("clear-tooltip"))
        actions['show_level'] = Action(_("show-level"), self, statusTip=_("show-level"), checkable=True, checked=True)
        actions['show_level'].setChecked = False  # Apparently, the checked named argument doesn't work

    def createMenus(self, actions, menus):
        ''' Setup the view's menus '''

        log.debug("Creating menus")

        reports = QtWidgets.QMenu("&Reports")
        reports.setObjectName("reports::menu")

        reports.addAction(actions["save"])
        reports.addAction(actions["clear"])
        reports.addAction(actions["show_level"])
        menus["reports"] = reports

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")




    def add_line(self, line):
        # TODO: Support multiple columns for the HTML. DO better with the spacing
        #  and margins in the output

        self._text.append(line)

    # Handlers for the view actions
    def clear_triggered(self):
        self._text.clear()

    def save_triggered(self):
        pass

    def show_level_toggled(self, checked):
        self.handler.show_level = checked


class ReportsHandler(logging.Handler):  # Inherit from logging.Handler
    ''' Writes out logs to the reporst window '''

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
            message = html.escape(record.msg)
            output = self.formatOutput()
            self.add_line(output.format(level, message))
        else:
            message = html.escape(record.msg)
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

#!/usr/bin/env python3

# Copyright 2009-2020 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

import argparse
import gettext
import locale
import logging
import logging.handlers
import os
import platform
import sys


VERSION_AND_DISCLAIMER_TEMPLATE = """\
GNU Radio Companion (%s) -
This program is part of GNU Radio.
GRC comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it.
"""

LOG_LEVELS = {
    'debug': logging.DEBUG,
    'info': logging.INFO,
    'warning': logging.WARNING,
    'error': logging.ERROR,
    'critical': logging.CRITICAL,
}

### Load GNU Radio
# Do this globally so it is available for both run_gtk() and run_qt()
try:
    from gnuradio import gr
except ImportError as ex:
    # Throw a new exception with more information
    print ("Cannot find GNU Radio! (Have you sourced the environment file?)", file=sys.stderr)

    # If this is a background session (not launched through a script), show a Tkinter error dialog.
    # Tkinter should already be installed default with Python, so this shouldn't add new dependencies
    if not sys.stdin.isatty():
        import tkinter
        from tkinter import messagebox
        # Hide the main window
        root = tkinter.Tk()
        root.withdraw()
        # Show the error dialog
        # TODO: Have a more helpful dialog here. Maybe a link to the wiki pages?
        messagebox.showerror("Cannot find GNU Radio", "Cannot find GNU Radio!")

    # Throw the new exception
    raise Exception("Cannot find GNU Radio!") from None


### Enable Logging
# Do this globally so it is available for both run_gtk() and run_qt()
# TODO: Advanced logging - https://docs.python.org/3/howto/logging-cookbook.html#formatting-styles
# Note: All other modules need to use the 'grc.<module>' convention
log = logging.getLogger('grc')
# Set the root log name
# Since other files are in the 'grc' module, they automatically get a child logger when using:
#   log = logging.getLogger(__name__)
# This log level should be set to DEBUG so the logger itself catches everything.
# The StreamHandler level can be set independently to choose what messages are sent to the console.
# The default console logging should be WARNING
log.setLevel(logging.DEBUG)


def run_gtk(args, log):
    ''' Runs the GTK version of GNU Radio Companion '''

    import gi
    gi.require_version('Gtk', '3.0')
    gi.require_version('PangoCairo', '1.0')
    from gi.repository import Gtk

    # Delay importing until the logging is setup
    from .gui.Platform import Platform
    from .gui.Application import Application

    # The platform is loaded differently between QT and GTK, so this is required both places
    log.debug("Loading platform")
    platform = Platform(
        version=gr.version(),
        version_parts=(gr.major_version(), gr.api_version(),
                       gr.minor_version()),
        prefs=gr.prefs(),
        install_prefix=gr.prefix()
    )
    platform.build_library()

    log.debug("Loading application")
    app = Application(args.flow_graphs, platform)
    log.debug("Running")
    sys.exit(app.run())


def run_qt(args, log):
    ''' Runs the Qt version of GNU Radio Companion '''

    import platform
    import locale
    import gettext

    from .gui_qt import grc
    from .gui_qt import helpers
    from .gui_qt import properties

    # Delay importing until the logging is setup
    from .gui_qt.Platform import Platform

    ''' Global Settings/Constants '''
    # Initialize a class with all of the default settings and properties
    # TODO: Move argv to separate argument parsing class that overrides default properties?
    # TODO: Split settings/constants into separate classes rather than a single properites class?
    settings = properties.Properties(sys.argv)

    ''' Translation Support '''
    # Try to get the current locale. Always add English
    lc, encoding = locale.getdefaultlocale()
    if lc:
        languages = [lc]
    languages += settings.DEFAULT_LANGUAGE
    log.debug("Using locale - %s" % str(languages))

    # Still run even if the english translation isn't found
    language = gettext.translation(settings.APP_NAME, settings.path.LANGUAGE, languages=languages,
                                   fallback=True)
    if type(language) == gettext.NullTranslations:
        log.error("Unable to find any translation")
        log.error("Default English translation missing")
    else:
        log.info("Using translation - %s" % language.info()["language"])
    # Still need to install null translation to let the system handle calls to _()
    language.install()


    ''' OS Platform '''
    # Figure out system specific properties and setup defaults.
    # Some properties can be overridden by preferences
    # Get the current OS
    if platform.system() == "Linux":
        log.debug("Detected Linux")
        settings.system.OS = "Linux"
        # Determine if Unity is running....
        try:
            #current_desktop = os.environ['DESKTOP_SESSION']
            current_desktop = os.environ['XDG_CURRENT_DESKTOP']
            log.debug("Desktop Session - %s" % current_desktop)
            if current_desktop == "Unity":
                log.debug("Detected GRC is running under unity")
                # Use the native menubar rather than leaving it in the window
                settings.window.NATIVE_MENUBAR = True
        except:
            log.warning("Unable to determine the Linux desktop system")

    elif platform.system() == "Darwin":
        log.debug("Detected Mac OS X")
        settings.system.OS = "OS X"
        # Setup Mac specific QT elements
        settings.window.NATIVE_MENUBAR = True
    elif platform.system() == "Windows":
        log.warning("Detected Windows")
        settings.system.OS = "Windows"
    else:
        log.warning("Unknown operating system")


    ''' Preferences '''
    # TODO: Move earlier? Need to load user preferences and override the default properties/settings

    # The platform is loaded differently between QT and GTK, so this is required both places
    log.debug("Loading platform")
    # TODO: Might be beneficial to rename Platform to avoid confusion with the builtin Python module
    # Possible names: internal, model?
    model = Platform(
        version=gr.version(),
        version_parts=(gr.major_version(), gr.api_version(), gr.minor_version()),
        prefs=gr.prefs(),
        install_prefix=gr.prefix()
    )
    model.build_library()

    # Launch GRC
    app = grc.Application(settings, model)
    sys.exit(app.run())


def main():

    ### Argument parsing
    parser = argparse.ArgumentParser(
        description=VERSION_AND_DISCLAIMER_TEMPLATE % gr.version())
    parser.add_argument('flow_graphs', nargs='*')

    # Custom Configurations
    # TODO: parser.add_argument('--config')

    # Logging support
    parser.add_argument('--log', choices=['debug', 'info', 'warning', 'error', 'critical'], default='info')
    parser.add_argument('--nowiki', action='store_true')
    # TODO: parser.add_argument('--log-output')

    # Graphics framework (QT or GTK)
    gui_group = parser.add_argument_group('Framework')
    gui_group_exclusive = gui_group.add_mutually_exclusive_group()
    gui_group_exclusive.add_argument("--qt", dest='framework', action='store_const', const='qt',
                                     help="GNU Radio Companion (QT)")
    gui_group_exclusive.add_argument("--gtk", dest='framework', action='store_const', const='gtk',
                                     help="GNU Radio Companion (GTK)")

    # Default options if not already set with add_argument()
    parser.set_defaults(framework='gtk')
    args = parser.parse_args()

    # Print the startup message
    py_version = sys.version.split()[0]
    log.info("Starting GNU Radio Companion {} (Python {})".format(gr.version(), py_version))

    # File logging
    log_file = os.path.expanduser('~') + "/.gnuradio/grc.log"
    try:
        fileHandler = logging.FileHandler(log_file)
        file_msg_format = '%(asctime)s [%(levelname)s] %(message)s'
        if args.log == 'debug':
            file_msg_format += ' (%(name)s:%(lineno)s)'
            fileHandler.setLevel(logging.DEBUG)
            log.info(f'Logging to {log_file} (DEBUG and higher)')
        else:
            fileHandler.setLevel(logging.INFO)
            log.info(f'Logging to {log_file} (INFO and higher)')
        file_formatter = logging.Formatter(file_msg_format)
        fileHandler.setFormatter(file_formatter)
        log.addHandler(fileHandler)
    except (PermissionError, FileNotFoundError) as e:
        log.error(f'Cannot write to {log_file} - {e}')


    ### GUI Framework
    if args.framework == 'qt':
        run_qt(args, log)
    else:
        run_gtk(args, log)

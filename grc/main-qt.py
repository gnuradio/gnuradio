#!/usr/bin/env python3

"""
Copyright 2014 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""


import os
import sys
import platform
import locale
import gettext
import logging
import logging.handlers
import argparse

from gui_qt import grc
from gui_qt import properties
from gui_qt import helpers

from core.platform import Platform


LEVELS = {'--debug': logging.DEBUG,
          '--info': logging.INFO,
          '--warning': logging.WARNING,
          '--error': logging.ERROR,
          '--critical': logging.CRITICAL}


VERSION_AND_DISCLAIMER_TEMPLATE = """\
GNU Radio Companion %s

This program is part of GNU Radio
GRC comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it.
"""


# Script for launching GRC. This should handle logging, translation, platform, and main QT setup.
if __name__ == "__main__":

    ''' Global Settings/Constants '''
    # Initialize a class with all of the default settings and properties
    # TODO: Move argv to separate argument parsing class that overrides default properties?
    # TODO: Split settings/constants into separate classes rather than a single properites class?
    settings = properties.Properties(sys.argv)


    ''' GNU Radio '''
    # Load gnuradio framework
    try:
        from gnuradio import gr
    except ImportError as e:
        print ('Unable to import GR framework')


    usage = 'usage: %prog [options] [saved flow graphs]'
    version = """
    GNU Radio Companion %s

    This program is part of GNU Radio
    GRC comes with ABSOLUTELY NO WARRANTY.
    This is free software,
    and you are welcome to redistribute it.
    """ % "3.7.2"  # gr.version()


    ''' Arguments and Environment Variables '''
    #parser = argparse.ArgumentParser(usage=usage#, version=version)
    #args = parser.parse_args()
    #parser = argparse.ArgumentParser(
    #description=VERSION_AND_DISCLAIMER_TEMPLATE % gr.version())
    #parser.add_argument('flow_graphs', nargs='*')
    #parser.add_argument('--log', choices=['debug', 'info', 'warning', 'error', 'critical'], default='warning')
    #args = parser.parse_args()


    ''' Logging Support '''
    # Initialize logging first so that it can be used immediately
    # Get logging level (Default to WARNING)
    if len(sys.argv) > 1:
        level_name = sys.argv[1]
        level = LEVELS.get(level_name, logging.WARNING)
    else:
        level = logging.INFO
    # Note: All children loggers must have the correct naming convention to share handlers
    #  i.e. 'grc.<module-name>'
    log = logging.getLogger('grc')

    # Still catch everything
    log.setLevel(logging.DEBUG)

    # Setup the console logging with the given level. Use a separate log handler
    #  for writing to the reports window.
    console = logging.StreamHandler()
    console.setLevel(level)

    # Add console formatting
    format = '[%(asctime)s - %(levelname)8s] --- %(message)s (%(filename)s:%(lineno)s)'
    date_format = '%I:%M'
    #formatter = logging.Formatter(format, datefmt = date_format)
    formatter = helpers.logging.ConsoleFormatter()
    console.setFormatter(formatter)
    log.addHandler(console)



    #msg_format = '[%(asctime)s - %(levelname)8s] --- %(message)s (%(filename)s:%(lineno)s)'
    msg_format = '[%(levelname)s] %(message)s (%(filename)s:%(lineno)s)'
    date_format = '%I:%M'
    formatter = logging.Formatter(msg_format, datefmt=date_format)

    #formatter = utils.log.ConsoleFormatter()
    console.setFormatter(formatter)
    log.addHandler(console)

    py_version = sys.version.split()[0]
    log.debug("Starting GNU Radio Companion ({})".format(py_version))


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
    # Some properties should be overriddable by preferences
    # Get the current OS
    if platform.system() == "Linux":
        log.debug("Detected Linux")
        settings.system.OS = "Linux"
        # Deteremine if Unity is running....
        try:
            #current_desktop = os.environ['DESKTOP_SESSION']
            current_desktop = os.environ['XDG_CURRENT_DESKTOP']
            log.debug("Desktop Session - %s" % current_desktop)
            if current_desktop == "Unity":
                log.debug("Detected GRC is running under unity")
                # Use the native menubar rather than leaving it in the window
                settings.window.NATIVE_MENUBAR = True
        except:
            log.error("Unable to determine the Linux desktop system")

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

    log.debug("Loading platform")
    # TODO: Might be beneficial to rename Platform to avoid confusion with the builtin Python module
    # Possible names: internal, model?
    model = Platform(
        version=gr.version(),
        version_parts=(gr.major_version(), gr.api_version(), gr.minor_version()),
        prefs=gr.prefs(),
        #install_prefix=gr.prefix()
    )
    model.build_library()

    # Launch GRC
    app = grc.Application(settings, model)
    sys.exit(app.run())

# Copyright 2009-2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

import argparse
import logging
import sys

import gi
gi.require_version('Gtk', '3.0')
gi.require_version('PangoCairo', '1.0')
from gi.repository import Gtk


VERSION_AND_DISCLAIMER_TEMPLATE = """\
GNU Radio Companion %s

This program is part of GNU Radio
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


def main():
    from gnuradio import gr
    parser = argparse.ArgumentParser(
        description=VERSION_AND_DISCLAIMER_TEMPLATE % gr.version())
    parser.add_argument('flow_graphs', nargs='*')
    parser.add_argument(
        '--log', choices=['debug', 'info', 'warning', 'error', 'critical'], default='warning')
    args = parser.parse_args()

    # Enable logging
    # Note: All other modules need to use the same '<prefix>.<module>' convention.
    # This should automatically be handled by logging.getLogger(__name__), but
    # the prefix can change depending on how grc is launched (either by calling
    # gnuradio-companion once installed or python -m grc). The prefix for main.py
    # should be the same as other modules, so use that dynamically.
    # Strip '.main' from the module name to get the prefix
    logger_prefix = __name__[0:__name__.rindex('.')]
    log = logging.getLogger(logger_prefix)

    # NOTE: This sets the log level to what was requested for the logger on the
    # command line, but this may not be the correct approach if multiple handlers
    # are intended to be used. The logger level shown here indicates all the log
    # messages that are captured and the handler levels indicate messages each
    # handler will output. A better approach may be resetting this to logging.DEBUG
    # to catch everything and making sure the handlers have the correct levels set.
    # This would be useful for a future GUI logging window that can filter messages
    # independently of the console output. In this case, this should be DEBUG.
    log.setLevel(LOG_LEVELS[args.log])

    # Console formatting
    console = logging.StreamHandler()
    console.setLevel(LOG_LEVELS[args.log])

    #msg_format = '[%(asctime)s - %(levelname)8s] --- %(message)s (%(filename)s:%(lineno)s)'
    msg_format = '[%(levelname)s] %(message)s (%(filename)s:%(lineno)s)'
    date_format = '%I:%M'
    formatter = logging.Formatter(msg_format, datefmt=date_format)

    #formatter = utils.log.ConsoleFormatter()
    console.setFormatter(formatter)
    log.addHandler(console)

    py_version = sys.version.split()[0]
    log.debug("Starting GNU Radio Companion ({})".format(py_version))

    # Delay importing until the logging is setup
    from .gui.Platform import Platform
    from .gui.Application import Application

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

#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Base CLI module """


import os
import sys
import logging
import functools
from importlib import import_module
from pkg_resources import iter_entry_points
from logging import Formatter, StreamHandler

import click
from click import ClickException
from click_plugins import with_plugins

from gnuradio import gr


class ModToolException(ClickException):
    """ Exception class for enhanced CLI interface """
    def show(self, file = None):
        """ displays the colored message """
        click.secho(f'ModToolException: {self.format_message()}', fg='red')


class CommandCLI(click.Group):
    """
    This is a derived class of the implemented click class
    which overrides some of the functional definitions for external
    plug-in support
    """
    cmd_folder = os.path.abspath(os.path.dirname(__file__))

    def list_commands(self, ctx):
        """
        Lists all the commands available in the modtool directory
        as well as the commands from external plug-ins.
        """
        cmds = []
        for filename in os.listdir(self.cmd_folder):
            if filename.endswith('.py') and not '_' in filename:
                cmds.append(filename[:-3])
        cmds.remove('base')
        cmds += self.commands
        return sorted(cmds)

    def get_command(self, ctx, cmd_name):
        """
        Returns a command object if it exists. The existing in-tree ModTool
        command is the priority over the same external plug-in command.
        """
        try:
            mod = import_module('gnuradio.modtool.cli.' + cmd_name)
        except ImportError as ex:
            logging.error(str(ex))
            return self.commands.get(cmd_name)
        return mod.cli


class ClickHandler(StreamHandler):
    """
    This is a derived class of implemented logging class
    StreamHandler which overrides some of its functional
    definitions to add colors to the stream output
    """
    def emit(self, record):
        """ Writes message to the stream """
        colormap = {
            'DEBUG': ('white', 'black'),
            'INFO': ('blue', None),
            'WARNING': ('yellow', None),
            'ERROR': ('red', None),
            'CRITICAL': ('white', 'red'),
        }
        try:
            msg = self.format(record)
            colors = colormap.get(record.levelname, (None, None))
            fgcolor = colors[0]
            bgcolor = colors[1]
            click.secho(msg, fg=fgcolor, bg=bgcolor)
            self.flush()
        except Exception:
            self.handleError(record)


def setup_cli_logger(logger):
    """ Sets up logger for CLI parsing """
    try:
        import colorama
        stream_handler = ClickHandler()
        logger.addHandler(stream_handler)
    except ImportError:
        stream_handler = logging.StreamHandler()
        logger.addHandler(stream_handler)
    finally:
        logger.setLevel(logging.INFO)


def cli_input(msg):
    """ Returns enhanced input """
    return input(click.style(msg, fg='cyan'))


def common_params(func):
    """ Common parameters for various modules"""
    @click.option('-d', '--directory', default='.',
                  help="Base directory of the module. Defaults to the cwd.")
    @click.option('--skip-lib', is_flag=True,
                  help="Don't do anything in the lib/ subdirectory.")
    @click.option('--skip-pybind', is_flag=True,
                  help="Don't do anything in the python/bindings/ subdirectory.")
    @click.option('--skip-python', is_flag=True,
                  help="Don't do anything in the python/ subdirectory.")
    @click.option('--skip-grc', is_flag=True,
                  help="Don't do anything in the grc/ subdirectory.")
    @click.option('--scm-mode', type=click.Choice(['yes', 'no', 'auto']),
                  default=gr.prefs().get_string('modtool', 'scm_mode', 'no'),
                  help="Use source control management [ yes | no | auto ]).")
    @click.option('-y', '--yes', is_flag=True,
                  help="Answer all questions with 'yes'. " +
                  "This can overwrite and delete your files, so be careful.")
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        """ Decorator that wraps common options """
        return func(*args, **kwargs)
    return wrapper


block_name = click.argument('blockname', nargs=1, required=False, metavar="BLOCK_NAME")


@with_plugins(iter_entry_points('gnuradio.modtool.cli.plugins'))
@click.command(cls=CommandCLI,
               epilog='Manipulate with GNU Radio modules source code tree. ' +
               'Call it without options to run specified command interactively')
def cli():
    """A tool for editing GNU Radio out-of-tree modules."""
    pass


def run(module):
    """Call the run function of the core modules."""
    try:
        module.run()
    except ModToolException as err:
        click.echo(err, file=sys.stderr)
        exit(1)

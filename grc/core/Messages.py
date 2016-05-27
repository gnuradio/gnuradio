# Copyright 2007, 2015 Free Software Foundation, Inc.
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


import traceback
import sys
import os

#  A list of functions that can receive a message.
MESSENGERS_LIST = list()
_indent = ''


def register_messenger(messenger):
    """
    Append the given messenger to the list of messengers.

    Args:
        messenger: a method that takes a string
    """
    MESSENGERS_LIST.append(messenger)


def set_indent(level=0):
    global _indent
    _indent = '    ' * level


def send(message):
    """
    Give the message to each of the messengers.

    Args:
        message: a message string
    """
    for messenger in MESSENGERS_LIST:
        messenger(_indent + message)

# register stdout by default
register_messenger(sys.stdout.write)


###########################################################################
# Special functions for specific program functionalities
###########################################################################
def send_init(platform):
    msg = "<<< Welcome to {config.name} {config.version} >>>\n\n" \
          "Block paths:\n\t{paths}\n"
    send(msg.format(
        config=platform.config,
        paths="\n\t".join(platform.config.block_paths))
    )


def send_xml_errors_if_any(xml_failures):
    if xml_failures:
        send('\nXML parser: Found {0} erroneous XML file{1} while loading the '
             'block tree (see "Help/Parser errors" for details)\n'.format(
                    len(xml_failures), 's' if len(xml_failures) > 1 else ''))


def send_start_load(file_path):
    send('\nLoading: "%s"\n' % file_path)


def send_error_msg_load(error):
    send('>>> Error: %s\n' % error)


def send_error_load(error):
    send_error_msg_load(error)
    traceback.print_exc()


def send_end_load():
    send('>>> Done\n')


def send_fail_load(error):
    send('Error: %s\n>>> Failure\n' % error)
    traceback.print_exc()


def send_start_gen(file_path):
    send('\nGenerating: %r\n' % file_path)


def send_auto_gen(file_path):
    send('>>> Generating: %r\n' % file_path)


def send_fail_gen(error):
    send('Generate Error: %s\n>>> Failure\n' % error)
    traceback.print_exc()


def send_start_exec(file_path):
    send('\nExecuting: %s\n' % file_path)


def send_verbose_exec(verbose):
    send(verbose)


def send_end_exec(code=0):
    send('\n>>> Done%s\n' % (" (return code %s)" % code if code else ""))


def send_fail_save(file_path):
    send('>>> Error: Cannot save: %s\n' % file_path)


def send_fail_connection():
    send('>>> Error: Cannot create connection.\n')


def send_fail_load_preferences(prefs_file_path):
    send('>>> Error: Cannot load preferences file: "%s"\n' % prefs_file_path)


def send_fail_save_preferences(prefs_file_path):
    send('>>> Error: Cannot save preferences file: "%s"\n' % prefs_file_path)


def send_warning(warning):
    send('>>> Warning: %s\n' % warning)

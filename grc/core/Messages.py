# Copyright 2007, 2015 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


import traceback
import sys

#  A list of functions that can receive a message.
MESSENGERS_LIST = list()
_indent = ''

# Global FlowGraph Error and the file that caused it
flowgraph_error = None
flowgraph_error_file = None


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
        paths="\n\t".join(platform.config.block_paths)))


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


def send_fail_connection(msg=''):
    send('>>> Error: Cannot create connection.\n' +
         ('\t{}\n'.format(msg) if msg else ''))


def send_fail_load_preferences(prefs_file_path):
    send('>>> Error: Cannot load preferences file: "%s"\n' % prefs_file_path)


def send_fail_save_preferences(prefs_file_path):
    send('>>> Error: Cannot save preferences file: "%s"\n' % prefs_file_path)


def send_warning(warning):
    send('>>> Warning: %s\n' % warning)


def send_flowgraph_error_report(flowgraph):
    """ verbose error report for flowgraphs """
    error_list = flowgraph.get_error_messages()
    if not error_list:
        return

    send('*' * 50 + '\n')
    summary_msg = '{} errors from flowgraph:\n'.format(len(error_list))
    send(summary_msg)
    for err in error_list:
        send(err)
    send('\n' + '*' * 50 + '\n')

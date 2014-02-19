"""
Copyright 2007 Free Software Foundation, Inc.
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

import traceback
import sys

## A list of functions that can receive a message.
MESSENGERS_LIST = list()

def register_messenger(messenger):
    """
    Append the given messenger to the list of messengers.
    
    Args:
        messenger: a method thats takes a string
    """
    MESSENGERS_LIST.append(messenger)

def send(message):
    """
    Give the message to each of the messengers.
    
    Args:
        message: a message string
    """
    for messenger in MESSENGERS_LIST: messenger(message)

#register stdout by default
register_messenger(sys.stdout.write)

###########################################################################
# Special functions for specific program functionalities
###########################################################################
def send_init(platform):
    send("""<<< Welcome to %s %s >>>\n"""%(platform.get_name(), platform.get_version()))

def send_page_switch(file_path):
    send('\nShowing: "%s"\n'%file_path)

################# functions for loading blocks ########################################
def send_xml_errors_if_any(xml_failures):
    if xml_failures:
        send('\nXML parser: Found {0} erroneous XML file{1} while loading the block tree '
             '(see "Help/Parser errors" for details)\n'.format(len(xml_failures), 's' if len(xml_failures) > 1 else ''))

################# functions for loading flow graphs ########################################
def send_start_load(file_path):
    send('\nLoading: "%s"'%file_path + '\n')

def send_error_load(error):
    send('>>> Error: %s\n'%error)
    traceback.print_exc()

def send_end_load():
    send('>>> Done\n')

def send_fail_load(error):
    send('Error: %s\n'%error)
    send('>>> Failure\n')
    traceback.print_exc()

################# functions for generating flow graphs  ########################################
def send_start_gen(file_path):
    send('\nGenerating: "%s"'%file_path + '\n')

def send_fail_gen(error):
    send('Generate Error: %s\n'%error)
    send('>>> Failure\n')
    traceback.print_exc()

################# functions for executing flow graphs   ########################################
def send_start_exec(file_path):
    send('\nExecuting: "%s"'%file_path + '\n')

def send_verbose_exec(verbose):
    send(verbose)

def send_end_exec():
    send('\n>>> Done\n')

################# functions for saving flow graphs  ########################################
def send_fail_save(file_path):
    send('>>> Error: Cannot save: %s\n'%file_path)

################# functions for connections ########################################
def send_fail_connection():
    send('>>> Error: Cannot create connection.\n')

################# functions for preferences ########################################
def send_fail_load_preferences(prefs_file_path):
    send('>>> Error: Cannot load preferences file: "%s"\n'%prefs_file_path)

def send_fail_save_preferences(prefs_file_path):
    send('>>> Error: Cannot save preferences file: "%s"\n'%prefs_file_path)

################# functions for warning ########################################
def send_warning(warning):
    send('>>> Warning: %s\n'%warning)

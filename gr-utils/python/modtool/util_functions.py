#
# Copyright 2013, 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#
""" Utility functions for gr_modtool """

import re
import sys
import readline

# None of these must depend on other modtool stuff!

def get_command_from_argv(possible_cmds):
    """ Read the requested command from argv. This can't be done with optparse,
    since the option parser isn't defined before the command is known, and
    optparse throws an error."""
    for arg in sys.argv:
        if arg[0] != "-" and arg in possible_cmds:
            return arg
    return None

def append_re_line_sequence(filename, linepattern, newline):
    """ Detects the re 'linepattern' in the file. After its last occurrence,
    paste 'newline'. If the pattern does not exist, append the new line
    to the file. Then, write. """
    oldfile = open(filename, 'r').read()
    lines = re.findall(linepattern, oldfile, flags=re.MULTILINE)
    if len(lines) == 0:
        open(filename, 'a').write(newline)
        return
    last_line = lines[-1]
    newfile = oldfile.replace(last_line, last_line + newline + '\n')
    open(filename, 'w').write(newfile)

def remove_pattern_from_file(filename, pattern):
    """ Remove all occurrences of a given pattern from a file. """
    oldfile = open(filename, 'r').read()
    pattern = re.compile(pattern, re.MULTILINE)
    open(filename, 'w').write(pattern.sub('', oldfile))

def str_to_fancyc_comment(text):
    """ Return a string as a C formatted comment. """
    l_lines = text.splitlines()
    outstr = "/* " + l_lines[0] + "\n"
    for line in l_lines[1:]:
        outstr += " * " + line + "\n"
    outstr += " */\n"
    return outstr

def str_to_python_comment(text):
    """ Return a string as a Python formatted comment. """
    return re.compile('^', re.MULTILINE).sub('# ', text)

def strip_default_values(string):
    """ Strip default values from a C++ argument list. """
    return re.sub(' *=[^,)]*', '', string)

def strip_arg_types(string):
    """"
    Strip the argument types from a list of arguments.
    Example: "int arg1, double arg2" -> "arg1, arg2"
    Note that some types have qualifiers, which also are part of
    the type, e.g. "const std::string &name" -> "name", or
    "const char *str" -> "str".
    """
    string = strip_default_values(string)
    return ", ".join(
                [part.strip().split(' ')[-1] for part in string.split(',')]
            ).translate(None, '*&')

def strip_arg_types_grc(string):
    """" Strip the argument types from a list of arguments for GRC make tag.
    Example: "int arg1, double arg2" -> "$arg1, $arg2" """
    if len(string) == 0:
        return ""
    else:
        string = strip_default_values(string)
        return ", ".join(['$' + part.strip().split(' ')[-1] for part in string.split(',')])

def get_modname():
    """ Grep the current module's name from gnuradio.project or CMakeLists.txt """
    modname_trans = {'howto-write-a-block': 'howto'}
    try:
        prfile = open('gnuradio.project', 'r').read()
        regexp = r'projectname\s*=\s*([a-zA-Z0-9-_]+)$'
        return re.search(regexp, prfile, flags=re.MULTILINE).group(1).strip()
    except IOError:
        pass
    # OK, there's no gnuradio.project. So, we need to guess.
    cmfile = open('CMakeLists.txt', 'r').read()
    regexp = r'(project\s*\(\s*|GR_REGISTER_COMPONENT\(")gr-(?P<modname>[a-zA-Z0-9-_]+)(\s*(CXX)?|" ENABLE)'
    try:
        modname = re.search(regexp, cmfile, flags=re.MULTILINE).group('modname').strip()
        if modname in modname_trans.keys():
            modname = modname_trans[modname]
        return modname
    except AttributeError:
        return None

def is_number(s):
    """ Return True if the string s contains a number. """
    try:
        float(s)
        return True
    except ValueError:
        return False

def xml_indent(elem, level=0):
    """ Adds indents to XML for pretty printing """
    i = "\n" + level*"    "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "    "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            xml_indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

def ask_yes_no(question, default):
    """ Asks a binary question. Returns True for yes, False for no.
    default is given as a boolean. """
    question += {True: ' [Y/n] ', False: ' [y/N] '}[default]
    if raw_input(question).lower() != {True: 'n', False: 'y'}[default]:
        return default
    else:
        return not default

class SequenceCompleter(object):
    """ A simple completer function wrapper to be used with readline, e.g.
    option_iterable = ("search", "seek", "destroy")
    readline.set_completer(SequenceCompleter(option_iterable).completefunc)

    Typical usage is with the `with` statement. Restores the previous completer
    at exit, thus nestable.
    """

    def __init__(self, sequence=None):
        self._seq = sequence or []
        self._tmp_matches = []

    def completefunc(self, text, state):
        if not text and state < len(self._seq):
            return self._seq[state]
        if not state:
            self._tmp_matches = filter(lambda candidate: candidate.startswith(text), self._seq)
        if state < len(self._tmp_matches):
            return self._tmp_matches[state]

    def __enter__(self):
        self._old_completer = readline.get_completer()
        readline.set_completer(self.completefunc)
        readline.parse_and_bind("tab: complete")

    def __exit__(self, exception_type, exception_value, traceback):
        readline.set_completer(self._old_completer)

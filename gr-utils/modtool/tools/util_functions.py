#
# Copyright 2013, 2018, 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Utility functions for gr_modtool """


import os
import re
try:
    import readline
    have_readline = True
except ImportError:
    have_readline = False

# None of these must depend on other modtool stuff!


def append_re_line_sequence(filename: str, linepattern: str, newline: str, closing_parentheses: str = ')') -> None:
    """
    Detects the re 'linepattern' in the file. After its last occurrence (and any other identical occurrences),
    paste 'newline'. If the pattern does not exist, append the new line
    to the file.

    If 'closing_parentheses' is not None, e.g. ")", remove it from end of the matched line, paste the 'newline',
    followed by a line containing the 'closing_parentheses'.

    Finally, write the modified file.
    """
    with open(filename, 'r') as f:
        oldfile = f.read()
    lines = re.findall(linepattern, oldfile, flags=re.MULTILINE)
    if len(lines) == 0:
        with open(filename, 'a') as f:
            f.write(newline)
        return
    last_line = lines[-1]
    if closing_parentheses is not None and last_line.rstrip().endswith(closing_parentheses):
        modified_last = f'\n{last_line.rstrip()[:-len(closing_parentheses)]}\n{closing_parentheses}\n'
    else:
        modified_last = last_line + newline + '\n'
    newfile = oldfile.replace(last_line, modified_last)
    with open(filename, 'w') as f:
        f.write(newfile)


def remove_pattern_from_file(filename, pattern):
    """ Remove all occurrences of a given pattern from a file. """
    with open(filename, 'r') as f:
        oldfile = f.read()
    pattern = re.compile(pattern, re.MULTILINE)
    with open(filename, 'w') as f:
        f.write(pattern.sub('', oldfile))


def str_to_fancyc_comment(text):
    """ Return a string as a C formatted comment. """
    l_lines = text.splitlines()
    if len(l_lines[0]) == 0:
        outstr = "/*\n"
    else:
        outstr = "/* " + l_lines[0] + "\n"
    for line in l_lines[1:]:
        if len(line) == 0:
            outstr += " *\n"
        else:
            outstr += " * " + line + "\n"
    outstr += " */\n"
    return outstr


def str_to_python_comment(text):
    """ Return a string as a Python formatted comment. """
    l_lines = text.splitlines()
    if len(l_lines[0]) == 0:
        outstr = "#\n"
    else:
        outstr = "# " + l_lines[0] + "\n"
    for line in l_lines[1:]:
        if len(line) == 0:
            outstr += "#\n"
        else:
            outstr += "# " + line + "\n"
    outstr += "#\n"
    return outstr


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
    ).replace('*', '').replace('&', '')


def strip_arg_types_grc(string):
    """" Strip the argument types from a list of arguments for GRC make tag.
    Example: "int arg1, double arg2" -> "$arg1, $arg2" """
    if len(string) == 0:
        return ""
    else:
        string = strip_default_values(string)
        return ", ".join(['${' + part.strip().split(' ')[-1] + '}' for part in string.split(',')])


def get_modname():
    """ Grep the current module's name from gnuradio.project or CMakeLists.txt """
    modname_trans = {'howto-write-a-block': 'howto'}
    try:
        with open('gnuradio.project', 'r') as f:
            prfile = f.read()
        regexp = r'projectname\s*=\s*([a-zA-Z0-9-_]+)$'
        return re.search(regexp, prfile, flags=re.MULTILINE).group(1).strip()
    except IOError:
        pass
    # OK, there's no gnuradio.project. So, we need to guess.
    with open('CMakeLists.txt', 'r') as f:
        cmfile = f.read()
    regexp = r'(project\s*\(\s*|GR_REGISTER_COMPONENT\(")gr-(?P<modname>[a-zA-Z0-9-_]+)(\s*(CXX)?|" ENABLE)'
    try:
        modname = re.search(regexp, cmfile, flags=re.MULTILINE).group(
            'modname').strip()
        if modname in list(modname_trans.keys()):
            modname = modname_trans[modname]
        return modname
    except AttributeError:
        return None


def get_block_names(pattern, modname):
    """ Return a list of cpp block names matches the regex pattern. """
    blocknames = []
    reg = re.compile(pattern)
    filename_re = re.compile(r'^(?!api)[a-zA-Z]\w+\.h$')
    for _, _, filenames in os.walk('include/'):
        for filename in filenames:
            if filename_re.search(filename) and reg.search(filename):
                blocknames.append(re.sub(r'.h$', '', filename))
    return blocknames


def is_number(s):
    """ Return True if the string s contains a number. """
    try:
        float(s)
        return True
    except ValueError:
        return False


def ask_yes_no(question, default):
    """ Asks a binary question. Returns True for yes, False for no.
    default is given as a boolean. """
    question += {True: ' [Y/n] ', False: ' [y/N] '}[default]
    if input(question).lower() != {True: 'n', False: 'y'}[default]:
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
            self._tmp_matches = [
                candidate for candidate in self._seq if candidate.startswith(text)]
        if state < len(self._tmp_matches):
            return self._tmp_matches[state]

    def __enter__(self):
        if have_readline:
            self._old_completer = readline.get_completer()
            readline.set_completer(self.completefunc)
            readline.parse_and_bind("tab: complete")

    def __exit__(self, exception_type, exception_value, traceback):
        if have_readline:
            readline.set_completer(self._old_completer)

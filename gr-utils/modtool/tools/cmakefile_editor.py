#
# Copyright 2013, 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Edit CMakeLists.txt files """


import re
import logging

logger = logging.getLogger(__name__)


class CMakeFileEditor(object):
    """A tool for editing CMakeLists.txt files. """

    def __init__(self, filename, separator='\n    ', indent='    '):
        self.filename = filename
        with open(filename, 'r') as f:
            self.cfile = f.read()
        self.separator = separator
        self.indent = indent

    def append_value(self, entry, value, to_ignore_start='', to_ignore_end=''):
        """ Add a value to an entry. """
        regexp = re.compile(fr'({entry}\({to_ignore_start}[^()]*?)\s*?(\s?{to_ignore_end})\)',
                            re.MULTILINE | re.IGNORECASE)
        substi = r'\1' + self.separator + value + r'\2)'
        (self.cfile, nsubs) = regexp.subn(substi, self.cfile, count=1)
        return nsubs

    def remove_value(self, entry, value, to_ignore_start='', to_ignore_end=''):
        """
        Remove a value from an entry.
        Example: You want to remove file.cc from this list() entry:
        list(SOURCES
            file.cc
            other_file.cc
        )

        Then run:
        >>> C.remove_value('list', 'file.cc', 'SOURCES')

        Returns the number of occurrences of entry in the current file
        that were removed.
        """
        # In the case of the example above, these are cases we need to catch:
        # - list(file.cc ...
        #   entry is right after the value parentheses, no whitespace. Can only happen
        #   when to_ignore_start is empty.
        # - list(... file.cc)
        #   Other entries come first, then entry is preceded by whitespace.
        # - list(SOURCES ... file.cc) # whitespace!
        #   When to_ignore_start is not empty, entry must always be preceded by whitespace.
        if len(to_ignore_start) == 0:
            regexp = r'^\s*({entry}\((?:[^()]*?\s+|)){value}\s*([^()]*{to_ignore_end}\s*\)){to_ignore_start}'
        else:
            regexp = r'^\s*({entry}\(\s*{to_ignore_start}[^()]*?\s+){value}\s*([^()]*{to_ignore_end}\s*\))'
        regexp = regexp.format(
            entry=entry,
            to_ignore_start=to_ignore_start,
            value=value,
            to_ignore_end=to_ignore_end,
        )
        regexp = re.compile(regexp, re.MULTILINE | re.IGNORECASE)
        (self.cfile, nsubs) = re.subn(regexp, r'\1\2', self.cfile, count=1)
        return nsubs

    def delete_entry(self, entry, value_pattern=''):
        """Remove an entry from the current buffer."""
        regexp = fr'{entry}\s*\([^()]*{value_pattern}[^()]*\)[^\n]*\n'
        regexp = re.compile(regexp, re.MULTILINE)
        (self.cfile, nsubs) = re.subn(regexp, '', self.cfile, count=1)
        return nsubs

    def write(self):
        """ Write the changes back to the file. """
        with open(self.filename, 'w') as f:
            f.write(self.cfile)

    def remove_double_newlines(self):
        """Simply clear double newlines from the file buffer."""
        self.cfile = re.compile(
            '\n\n\n+', re.MULTILINE).sub('\n\n', self.cfile)

    def find_filenames_match(self, regex):
        """ Find the filenames that match a certain regex
        on lines that aren't comments """
        filenames = []
        reg = re.compile(regex)
        fname_re = re.compile(r'[a-zA-Z]\w+\.\w{1,5}$')
        for line in self.cfile.splitlines():
            if len(line.strip()) == 0 or line.strip()[0] == '#':
                continue
            for word in re.split('[ /)(\t\n\r\f\v]', line):
                if fname_re.match(word) and reg.search(word):
                    filenames.append(word)
        return filenames

    def disable_file(self, fname):
        """ Comment out a file.
        Example:
        add_library(
            file1.cc
        )

        Here, file1.cc becomes #file1.cc with disable_file('file1.cc').
        """
        starts_line = False
        for line in self.cfile.splitlines():
            if len(line.strip()) == 0 or line.strip()[0] == '#':
                continue
            if re.search(r'\b' + fname + r'\b', line):
                if re.match(fname, line.lstrip()):
                    starts_line = True
                break
        comment_out_re = r'#\1' + '\n' + self.indent
        if not starts_line:
            comment_out_re = r'\n' + self.indent + comment_out_re
        (self.cfile, nsubs) = re.subn(
            r'(\b' + fname + r'\b)\s*', comment_out_re, self.cfile)
        if nsubs == 0:
            logger.warning(
                f"Warning: A replacement failed when commenting out {fname}. Check the CMakeFile.txt manually.")
        elif nsubs > 1:
            logger.warning(
                f"Warning: Replaced {fname} {nsubs} times (instead of once). Check the CMakeFile.txt manually.")

    def comment_out_lines(self, pattern, comment_str='#'):
        """ Comments out all lines that match with pattern """
        for line in self.cfile.splitlines():
            if re.search(pattern, line):
                self.cfile = self.cfile.replace(line, comment_str + line)

    def check_for_glob(self, globstr):
        """ Returns true if a glob as in globstr is found in the cmake file """
        str_ = globstr.replace('*', r'\*')
        glob_re = fr'GLOB\s[a-z_]+\s"{str_}"'
        return re.search(glob_re, self.cfile, flags=re.MULTILINE | re.IGNORECASE) is not None

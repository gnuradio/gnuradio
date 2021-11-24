#
# Copyright 2013, 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Edit C++ files for bindings """


import re
import logging

logger = logging.getLogger(__name__)


class CPPFileEditor(object):
    """A tool for editing CMakeLists.txt files. """

    def __init__(self, filename, separator='\n    ', indent='    '):
        self.filename = filename
        with open(filename, 'r') as f:
            self.cfile = f.read()
        self.separator = separator
        self.indent = indent

    # def append_value(self, entry, value, to_ignore_start='', to_ignore_end=''):
    #     """ Add a value to an entry. """
    #     regexp = re.compile(r'({}\({}[^()]*?)\s*?(\s?{})\)'.format(entry, to_ignore_start, to_ignore_end),
    #                         re.MULTILINE)
    #     substi = r'\1' + self.separator + value + r'\2)'
    #     (self.cfile, nsubs) = regexp.subn(substi, self.cfile, count=1)
    #     return nsubs

    def append_value(self, start_tag, end_tag, value):
        """ Add a value to an entry. """
        cfile_lines = self.cfile.splitlines()
        start_line_idx = [cfile_lines.index(
            s) for s in cfile_lines if start_tag in s][0]
        end_line_idx = [cfile_lines.index(s)
                        for s in cfile_lines if end_tag in s][0]

        self.cfile = '\n'.join(
            (cfile_lines[0:end_line_idx] + [self.indent + value] + cfile_lines[end_line_idx:]))
        return 1

    def remove_value(self, start_tag, end_tag, value):

        cfile_lines = self.cfile.splitlines()
        try:
            start_line_idx = [cfile_lines.index(
                s) for s in cfile_lines if start_tag in s][0]
            end_line_idx = [cfile_lines.index(
                s) for s in cfile_lines if end_tag in s][0]
        except:
            logger.warning("Could not find start or end tags in search")
            return 0

        try:
            lines_between_tags = cfile_lines[(start_line_idx + 1):end_line_idx]
            remove_index = [lines_between_tags.index(
                s) for s in cfile_lines if value in s][0]
            lines_between_tags.pop(remove_index)
        except:
            return 0

        self.cfile = '\n'.join((cfile_lines[0:(
            start_line_idx + 1)] + lines_between_tags + cfile_lines[end_line_idx:]))
        return 1

    def delete_entry(self, entry, value_pattern=''):
        """Remove an entry from the current buffer."""
        regexp = r'{}\s*\([^()]*{}[^()]*\)[^\n]*\n'.format(entry,
                                                           value_pattern)
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
                "Warning: A replacement failed when commenting out {}. Check the CMakeFile.txt manually.".format(fname))
        elif nsubs > 1:
            logger.warning(
                "Warning: Replaced {} {} times (instead of once). Check the CMakeFile.txt manually.".format(fname, nsubs))

    def comment_out_lines(self, pattern, comment_str='#'):
        """ Comments out all lines that match with pattern """
        for line in self.cfile.splitlines():
            if re.search(pattern, line):
                self.cfile = self.cfile.replace(line, comment_str + line)

    def check_for_glob(self, globstr):
        """ Returns true if a glob as in globstr is found in the cmake file """
        glob_re = r'GLOB\s[a-z_]+\s"{}"'.format(globstr.replace('*', r'\*'))
        return re.search(glob_re, self.cfile, flags=re.MULTILINE | re.IGNORECASE) is not None

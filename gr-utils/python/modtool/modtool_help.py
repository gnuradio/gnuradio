#
# Copyright 2013 Free Software Foundation, Inc.
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
""" The help module """

from gnuradio.modtool import *
from util_functions import get_command_from_argv
from templates import Templates


def print_class_descriptions():
    """ Go through all ModTool* classes and print their name,
        alias and description. """
    desclist = []
    for gvar in globals().values():
        try:
            if issubclass(gvar, ModTool) and not issubclass(gvar, ModToolHelp):
                desclist.append((gvar.name, ','.join(gvar.aliases), gvar.__doc__))
        except (TypeError, AttributeError):
            pass
    print 'Name      Aliases          Description'
    print '====================================================================='
    for description in desclist:
        print '%-8s  %-12s    %s' % description


class ModToolHelp(ModTool):
    """ Show some help. """
    name = 'help'
    aliases = ('h', '?')

    def __init__(self):
        ModTool.__init__(self)

    def setup(self, options, args):
        pass

    def run(self):
        cmd_dict = get_class_dict(globals().values())
        cmds = cmd_dict.keys()
        cmds.remove(self.name)
        for a in self.aliases:
            cmds.remove(a)
        help_requested_for = get_command_from_argv(cmds)
        if help_requested_for is None:
            print 'Usage:' + Templates['usage']
            print '\nList of possible commands:\n'
            print_class_descriptions()
            return
        cmd_dict[help_requested_for]().setup_parser().print_help()


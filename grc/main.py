# Copyright 2009-2016 Free Software Foundation, Inc.
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

import optparse

import gtk
from gnuradio import gr

from .gui.Platform import Platform
from .gui.ActionHandler import ActionHandler


VERSION_AND_DISCLAIMER_TEMPLATE = """\
GNU Radio Companion %s

This program is part of GNU Radio
GRC comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it.
"""


def main():
    parser = optparse.OptionParser(
        usage='usage: %prog [options] [saved flow graphs]',
        version=VERSION_AND_DISCLAIMER_TEMPLATE % gr.version())
    options, args = parser.parse_args()

    try:
        gtk.window_set_default_icon(gtk.IconTheme().load_icon('gnuradio-grc', 256, 0))
    except:
        pass

    platform = Platform(
        prefs_file=gr.prefs(),
        version=gr.version(),
        version_parts=(gr.major_version(), gr.api_version(), gr.minor_version()),
        install_prefix=gr.prefix()
    )
    ActionHandler(args, platform)
    gtk.main()


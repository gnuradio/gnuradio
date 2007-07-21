#
# Copyright 2005 Free Software Foundation, Inc.
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

import glob
import os.path

# Semi-hideous kludge to import everything in the gruimpl directory
# into the gnuradio.gru namespace.  This keeps us from having to remember 
# to manually update this file.

for p in __path__:
    filenames = glob.glob (os.path.join (p, "..", "gruimpl", "*.py"))
    for f in filenames:
        f = os.path.basename(f).lower()
        f = f[:-3]
        if f == '__init__':
            continue
        # print f
        exec "from gnuradio.gruimpl.%s import *" % (f,)

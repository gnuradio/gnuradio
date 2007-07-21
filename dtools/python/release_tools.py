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

import re
import os

def get_release(configure_ac_filename):
    f = open(configure_ac_filename,'r')
    contents = f.read()
    pat = re.compile('^AM_INIT_AUTOMAKE\((.*),(\d+)\.(\d+)(.*)\)', re.M)
    mo = pat.search(contents)
    if mo:
        return mo.groups()

def set_release(configure_ac_filename, new_release):
    f = open(configure_ac_filename,'r')
    contents = f.read()
    pat = re.compile('^AM_INIT_AUTOMAKE\(((.*),(\d+)\.(\d+)(.*))\)', re.M)
    repl = ''.join(('AM_INIT_AUTOMAKE(',
                    new_release[0],
                    ',',
                    new_release[1],
                    '.',
                    new_release[2],new_release[3],')'))
    new = pat.sub(repl, contents)
    return new

def incr_release(configure_ac_filename, final=False):
    cur = get_release(configure_ac_filename)
    if cur is None:
        raise ValueError, "%s: couldn't find version" % (configure_ac_filename,)
    new = set_release(configure_ac_filename, up_rev(cur, final))
    os.rename(configure_ac_filename, configure_ac_filename + '.bak')
    f = open(configure_ac_filename,'w')
    f.write(new)
    f.close()

def up_rev(current_rev, final=False):
    """
    X.Y -> X.Ycvs
    if final:
      X.Ycvs -> X.(Y+1)rc1
    else:
      X.Ycvs -> X.(Y+1)
    if final:
      X.YrcN -> X.Y
    else
      X.YrcN -> X.Yrc(N+1)
    """
    pkg, major, minor, suffix = current_rev

    if len(suffix) == 0:
        if final:
            raise RuntimeError, ("Can't go from current to final.  current = " + str(current_rev))
        return (pkg, major, minor, "cvs")
    if suffix == "cvs":
        if final:
            return (pkg, major, str(int(minor)+1), "")
        return (pkg, major, str(int(minor)+1), "rc1")
    elif suffix.startswith('rc'):
        if final:
            return (pkg, major, minor, "")
        return (pkg, major, minor, "rc" + (str(int(suffix[2:]) + 1)))

def make_tag(current_rev):
    pkg, major, minor, suffix = current_rev
    if suffix == "cvs":
        raise ValueError, "You tried to tag a X.Ycvs release..."
    return 'REL_%s_%s%s' % (major, minor, suffix)

#
# Copyright 2006,2009 Free Software Foundation, Inc.
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

import gnuradio_core as gsp
_prefs_base = gsp.gr_prefs


import ConfigParser
import os
import os.path
import sys


def _user_prefs_filename():
    return os.path.expanduser('~/.gnuradio/config.conf')
        
def _sys_prefs_dirname():
    return gsp.prefsdir()

def _bool(x):
    """
    Try to coerce obj to a True or False
    """
    if isinstance(x, bool):
        return x
    if isinstance(x, (float, int)):
        return bool(x)
    raise TypeError, x
        

class _prefs(_prefs_base):
    """
    Derive our 'real class' from the stubbed out base class that has support
    for SWIG directors.  This allows C++ code to magically and transparently
    invoke the methods in this python class.
    """
    def __init__(self):
	_prefs_base.__init__(self)
	self.cp = ConfigParser.RawConfigParser()
	self.__getattr__ = lambda self, name: getattr(self.cp, name)

    def _sys_prefs_filenames(self):
        dir = _sys_prefs_dirname()
        try:
            fnames = os.listdir(dir)
        except (IOError, OSError):
            return []
        fnames.sort()
        return [os.path.join(dir, f) for f in fnames]

    def _read_files(self):
        filenames = self._sys_prefs_filenames()
        filenames.append(_user_prefs_filename())
        #print "filenames: ", filenames
        self.cp.read(filenames)

    # ----------------------------------------------------------------
    # These methods override the C++ virtual methods of the same name
    # ----------------------------------------------------------------
    def has_section(self, section):
        return self.cp.has_section(section)

    def has_option(self, section, option):
        return self.cp.has_option(section, option)

    def get_string(self, section, option, default_val):
        try:
            return self.cp.get(section, option)
        except:
            return default_val

    def get_bool(self, section, option, default_val):
        try:
            return self.cp.getboolean(section, option)
        except:
            return default_val

    def get_long(self, section, option, default_val):
        try:
            return self.cp.getint(section, option)
        except:
            return default_val
        
    def get_double(self, section, option, default_val):
        try:
            return self.cp.getfloat(section, option)
        except:
            return default_val
    # ----------------------------------------------------------------
    #              End override of C++ virtual methods
    # ----------------------------------------------------------------


_prefs_db = _prefs()

# if GR_DONT_LOAD_PREFS is set, don't load them.
# (make check uses this to avoid interactions.)
if os.getenv("GR_DONT_LOAD_PREFS", None) is None:
    _prefs_db._read_files()
    

_prefs_base.set_singleton(_prefs_db)    # tell C++ what instance to use

def prefs():
    """
    Return the global preference data base
    """
    return _prefs_db

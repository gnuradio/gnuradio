#
# Copyright 2004,2006 Free Software Foundation, Inc.
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

"""
This is the 'generic' audio or soundcard interface.

The behavior of this module is controlled by the [audio] audio_module
configuration parameter.  If it is 'auto' we attempt to import modules
from the known_modules list, using the first one imported successfully.

If [audio] audio_module is not 'auto', we assume it's the name of
an audio module and attempt to import it.
"""

__all__ = ['source', 'sink']

from gnuradio import gr
import sys

source = None
sink = None


known_modules = (
    'audio_alsa', 'audio_oss', 'audio_osx', 'audio_jack', 'audio_portaudio', 'audio_windows')


def try_import(name):
    """
    Build a blob of code and try to execute it.
    If it succeeds we will have set the globals source and sink
    as side effects.

    returns True or False
    """
    global source, sink
    full_name = "gnuradio." + name
    code = """
import %s
source = %s.source
sink = %s.sink
""" % (full_name, full_name, full_name)
    try:
        exec code in globals()
        return True
    except ImportError:
        return False
    

def __init__ ():
    p = gr.prefs()		# get preferences (config file) object
    verbose = p.get_bool('audio', 'verbose', False)
    module = p.get_string('audio', 'audio_module', 'auto')

    if module == 'auto':        # search our list for the first one that we can import
        for m in known_modules:
            if try_import(m):
                if verbose: sys.stderr.write('audio: using %s\n' % (m,))
                return
        raise ImportError, 'Unable to locate an audio module.'

    else:                       # use the one the user specified
        if try_import(module):
            if verbose: sys.stderr.write('audio: using %s\n' % (module,))
        else:
            msg = 'Failed to import user-specified audio module %s' % (module,)
            if verbose: sys.stderr.write('audio: %s\n' % (msg,))
            raise ImportError, msg

__init__()

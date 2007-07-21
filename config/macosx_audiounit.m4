# Copyright 2006 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Boston, MA
# 02110-1301, USA.

AC_DEFUN([MACOSX_AUDIOUNIT],
[
    audiounitok=yes
    case "$host_os" in
    	darwin*);;
    	*) 
	   audiounitok=no
	   AC_MSG_RESULT([gr-audio-osx requires darwin or MacOS X.])
    esac

    AC_CHECK_HEADERS([AudioUnit/AudioUnit.h],[],
	  [audiounitok=no;AC_MSG_RESULT([gr-audio-osx requires AudioUnit/AudioUnit.h, which is available on MacOS X.])])

    AC_CHECK_HEADERS([AudioToolbox/AudioToolbox.h],[],
	  [audiounitok=no;AC_MSG_RESULT([gr-audio-osx requires AudioToolbox/AudioToolbox.h, which is available on MacOS X.])])    

    if test $audiounitok = yes; then
        ifelse([$1], , :, [$1])
    else
        ifelse([$2], , :, [$2])
    fi
])

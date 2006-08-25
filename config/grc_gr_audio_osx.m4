dnl Copyright 2001,2002,2003,2004,2005,2006 Free Software Foundation, Inc.
dnl 
dnl This file is part of GNU Radio
dnl 
dnl GNU Radio is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2, or (at your option)
dnl any later version.
dnl 
dnl GNU Radio is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with GNU Radio; see the file COPYING.  If not, write to
dnl the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
dnl Boston, MA 02111-1307, USA.

AC_DEFUN([GRC_GR_AUDIO_OSX],[
    GRC_ENABLE([gr-audio-osx])
    
    AC_CONFIG_FILES([ \
	gr-audio-osx/Makefile \
	gr-audio-osx/src/Makefile \
	gr-audio-osx/src/run_tests \
    ])
    
    passed=yes
    MACOSX_AUDIOUNIT([],
        [passed=no;AC_MSG_RESULT([gr-audio-osx requires AudioUnit, not found.])])

    GRC_BUILD_CONDITIONAL([gr-audio-osx],[
	dnl run_tests is created from run_tests.in.  Make it executable.
        AC_CONFIG_COMMANDS([run_tests_osx], [chmod +x gr-audio-osx/src/run_tests])
    ])
])

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

AC_DEFUN([GRC_GR_VIDEO_SDL],[
    AC_CONFIG_SRCDIR([gr-video-sdl/src/video_sdl.i])

    AC_CONFIG_FILES([ \
	gr-video-sdl/Makefile \
	gr-video-sdl/src/Makefile \
	gr-video-sdl/src/run_tests \
    ])
    
    succeeded=yes
    dnl Check for SDL
    SDL_VERSION=1.2.0
    AM_PATH_SDL($SDL_VERSION,[],[succeeded=no])

    if test $succeeded = yes; then
        #AM_CFLAGS="$AM_CFLAGS $SDL_CFLAGS"
        #AM_LDFLAGS="$AM_LDFLAGS $SDL_LIBS"
	AC_SUBST(SDL_CFLAGS)
	AC_SUBST(SDL_LIBS)

	dnl run_tests is created from run_tests.in.  Make it executable.
        AC_CONFIG_COMMANDS([run_tests_sdl], [chmod +x gr-video-sdl/src/run_tests])
        subdirs="$subdirs gr-video-sdl"
    else
	failed="$failed gr-video-sdl"
    fi
])

# Check for IPC System V shm support.             -*- Autoconf -*-

# Copyright 2003 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

AC_DEFUN([GR_SYSV_SHM],
[
	AC_LANG_SAVE
	AC_LANG_C

	AC_CHECK_HEADERS([sys/ipc.h sys/shm.h])

        save_LIBS="$LIBS"
	AC_SEARCH_LIBS(shmat, [cygipc ipc],
	  [ IPC_LIBS="$LIBS" ],
	  [ AC_MSG_WARN([SystemV IPC support not found. ]) ]
	)
        LIBS="$save_LIBS"

	AC_LANG_RESTORE
	AC_SUBST(IPC_LIBS)
])

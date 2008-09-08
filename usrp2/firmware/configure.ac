dnl
dnl Copyright 2007 Free Software Foundation, Inc.
dnl
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl

AC_INIT
AC_PREREQ(2.57)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([lib/u2_init.c])

AM_INIT_AUTOMAKE(u2f,0.0svn)

cross_compiling=yes

AC_PROG_CC([mb-gcc])
dnl AC_PROG_LD([mb-ld])

RANLIB=mb-ranlib
AC_SUBST(RANLIB)

dnl AC_PROG_RANLIB([mb-ranlib])

AC_CHECK_HEADERS(arpa/inet.h netinet/in.h byteswap.h)
AC_C_BIGENDIAN

AC_CONFIG_FILES([\
    Makefile \
    apps/Makefile \
    include/Makefile \
    lib/Makefile \
])

AC_OUTPUT

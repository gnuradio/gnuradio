dnl
dnl Copyright 2007,2008 Free Software Foundation, Inc.
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

AM_INIT_AUTOMAKE(usrp2-firmware,0.0svn)

dnl Component specific configuration
dnl The order of the GR_ macros determines the order of compilation
dnl For -any- checks on $enable_all_components
dnl use the following guidelines:
dnl   yes : --enable-all-components was specified, so error out if any
dnl         components do not pass configuration checks.
dnl   no  : --disable-all-components was specified, so try to build the
dnl         --enable'd components, and error out if any do not pass
dnl         configuration checks.
dnl   ""  : this option was not specified on the command line; try to
dnl         build all components that are not --with'd, but don't
dnl         error out if any component does not pass configuration checks.
dnl
dnl For each --enable-foo component, if that flag is not specified on
dnl the command line, the related variable $enable_foo will be set to
dnl $enable_all_components .

AC_ARG_ENABLE(
    [all-components],
    [  --enable-all-components Build all configurable components (default), or stop on failed dependencies]
)

#build_dirs="config"
build_dirs=
GRC_USRP2_STUB
GRC_USRP2_FIRMWARE


# Each component is now either to be built, was skipped, will be
# included from pre-installed libraries and includes, or failed
# dependencies.
AC_SUBST([build_dirs], [$build_dirs])
AC_SUBST([skipped_dirs], [$skipped_dirs])
AC_SUBST([with_dirs], [$with_dirs])

# fix for older autotools that don't define "abs_top_YYY" by default
AC_SUBST(abs_top_srcdir)
AC_SUBST(abs_top_builddir)

# 'with' variables - the pre-installed libraries, includes, and paths
# - must always come last in the lists, so they require special
# treatment.
AC_SUBST(with_INCLUDES)
AC_SUBST(with_SWIG_INCLUDES)
AC_SUBST(with_PYDIRPATH)
AC_SUBST(with_SWIGDIRPATH)
AC_SUBST(with_LIBDIRPATH)

AC_CONFIG_FILES([ \
    Makefile \
    config/Makefile \
])

AC_OUTPUT

#
# trim usrp2 out of dirs; we only use it as a controlling dependency
#
t=
for d in $build_dirs
do
    if test $d != usrp2; then
        if test -z "$t"; then
            t="$d"
        else
            t="$t $d"
        fi
    fi
done
build_dirs=$t

t=
for d in $skipped_dirs
do
    if test $d != usrp2; then
        if test -z "$t"; then
            t="$d"
        else
            t="$t $d"
        fi
    fi
done
skipped_dirs=$t

t=
for d in $with_dirs
do
    if test $d != usrp2; then
        if test -z "$t"; then
            t="$d"
        else
            t="$t $d"
        fi
    fi
done
with_dirs=$t

if test "$build_dirs" != ""; then
    echo
    echo "*********************************************************************"
    echo The following GNU Radio components have been successfully configured:
    echo 
    for dir in $build_dirs
    do
        echo $dir
    done
    echo
    echo You my now run the 'make' command to build these components.
    echo
fi

if test "$skipped_dirs" != ""; then
    echo "*********************************************************************"
    echo The following components were skipped either because you asked not
    echo to build them or they didn\'t pass configuration checks:
    echo
    for dir in $skipped_dirs
    do
        echo $dir
    done
    echo
    echo These components will not be built.
    echo
fi
if test "$with_dirs" != ""; then
    echo "*********************************************************************"
    echo The following components will be included from pre-installed
    echo libraries and includes:
    echo
    for dir in $with_dirs
    do
        echo $dir
    done
    echo
    echo These components will not be built.
    echo
fi

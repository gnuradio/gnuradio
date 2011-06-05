dnl Copyright 2006,2008,2009 Free Software Foundation, Inc.
dnl 
dnl This file is part of GNU Radio
dnl 
dnl GNU Radio is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3, or (at your option)
dnl any later version.
dnl 
dnl GNU Radio is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with GNU Radio; see the file COPYING.  If not, write to
dnl the Free Software Foundation, Inc., 51 Franklin Street,
dnl Boston, MA 02110-1301, USA.

dnl Create --enable-foo argument for named component, create variables as needed
dnl $1 is component name
AC_DEFUN([GRC_ENABLE], [
    _GRC_ENABLE($1,m4_bpatsubst($1,-,_))
])
dnl $2 is the '_'d component name
dnl on exit variable enable_$2 will be set to [yes|no];
dnl         passed will be [yes|no] (same as enable_$2)

AC_DEFUN([_GRC_ENABLE],[
    passed=yes
    AC_ARG_ENABLE([$1],
		  AC_HELP_STRING([--enable-$1],
				 [Stop if $1 fails configuration]),
		  [],[
        [enable_]$2=$enable_all_components
        if test x$enable_all_components = xno; then
            passed=no
        fi
    ])
])
dnl Component specific configuration
dnl The order of the GR_ macros determines the order of compilation
dnl For -any- checks on $enable_all_components
dnl use the following guildlines:
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

dnl Create --with-foo argument for named compoment, create variables as needed
dnl $1 is component name
dnl $2 is what to do on success
dnl $3 is the PKG_CONFIG name; if not given, then $1
AC_DEFUN([GRC_WITH], [
    if test [x]$3 = x; then
        pc_comp_name="$1"
    else
        pc_comp_name="$3"
    fi
    _GRC_WITH($1,[$2],${pc_comp_name},m4_bpatsubst($1,-,_))
])
dnl $3 is the pkg-config component name
dnl $4 is the '_'d component name
dnl on exit variable passed will be [yes|no|with]:
dnl   yes: if --enable-$1 and/or --enable-all-components was specified,
dnl        but --with was not;
dnl   with: if --with-$1 was specified, and passed checks;
dnl   no: all other conditions
AC_DEFUN([_GRC_WITH],[
    AC_ARG_WITH([$1],
	        AC_HELP_STRING([--with-$1@<:@=PATH@:>@],
	 		       [Use package $1 if installed in PATH (if specified) or PKG_CONFIG_PATH (if PATH not specified); stop if $1 not found]),
                [if test "x$withval" != "xyes"; then
		    [with_]$4[_val]=$withval
		    [with_]$4=yes
		 fi],
	        [])
    if test x$[with_]$4 = xyes; then
        if test x$[enable_]$4 = xyes; then
	    AC_MSG_ERROR([Component $1: Cannot use both --enable and --with])
        else
	    _GRC_WITH_PKG_CONFIG_CHECK($1,$3,$4)
	    ifelse([$2], , :, [$2])
        fi
    fi
])

dnl Use 'pkgconfig' to check for a package
dnl $1 is the --with component name
dnl $2 is the pkg-config component name, if provided; otherwise use $1 for this
dnl on success, resulting INCLUDES, INCLUDEDIR, LA, and LIBDIRPATH variables
dnl will be set; on failure, will exit with an error.
AC_DEFUN([GRC_WITH_PKG_CONFIG_CHECK], [
    if test [x]$2 = x; then
        pc_comp_name="$1"
    else
        pc_comp_name="$2"
    fi
    _GRC_WITH_PKG_CONFIG_CHECK($1,${pc_comp_name},m4_bpatsubst($1,-,_))
])
dnl $2 is the pkg-config component name
dnl $3 is the '_'d component name
AC_DEFUN([_GRC_WITH_PKG_CONFIG_CHECK],[
    dnl save PKG_CONFIG_PATH, restore at the end
    s_PKG_CONFIG_PATH=$PKG_CONFIG_PATH

    dnl create the PKG_CONFIG_PATH, via this component arg, if provided;
    dnl else use the environment PKG_CONFIG_PATH
    l_PKG_CONFIG_PATH=$[with_]$3[_val]
    if test "x$l_PKG_CONFIG_PATH" != "x"; then
        export PKG_CONFIG_PATH=$l_PKG_CONFIG_PATH

        dnl verify that the file exists; if not, no point in continuing
        if ! test -r ${l_PKG_CONFIG_PATH}/$2[.pc]; then
	    AC_MSG_ERROR([Component $1: PKGCONFIG cannot find info for $2, with provided PKG_CONFIG_PATH = @<:@ $l_PKG_CONFIG_PATH @:>@ .])
        fi
    fi

    dnl do the check; error out if not found
    PKG_CHECK_EXISTS($2, [passed=with; check1=yes], [
        check1=no
        dnl pkg-config returned an error; this might be that the .pc
	dnl file was not valid, or the Requires: were not met.
	dnl If the arg was provided and the input PKG_CONFIG_PATH , then try
	dnl again appending the whole PKG_CONFIG_PATH.
	if test "x$l_PKG_CONFIG_PATH" != "x"; then
            if test "x$s_PKG_CONFIG_PATH" != "x"; then
                export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${s_PKG_CONFIG_PATH}
	        PKG_CHECK_EXISTS($2, passed=with, passed=no)
            fi
        fi
        if test $passed != with; then
            AC_MSG_ERROR([Component $1: PKGCONFIG cannot find info for $2, with PKG_CONFIG_PATH = @<:@ $PKG_CONFIG_PATH @:>@ .])
        fi
        dnl pkg-config Requires are now met; save the new PKG_CONFIG_PATH
        s_PKG_CONFIG_PATH=$PKG_CONFIG_PATH
    ])

    dnl if PKG_CHECK_EXISTS returned, then this component's .pc file was
    dnl found in the provided 'arg' PKG_CONFIG_PATH;
    dnl retrieve various parameters
    $3[_INCLUDES]=`$PKG_CONFIG --cflags-only-I $2`
    $3[_LA]=`$PKG_CONFIG --libs $2`
    $3[_INCLUDEDIR]=`$PKG_CONFIG --variable=includedir $2`

    if test x$check1 = xyes; then
        dnl prepend the args PKG_CONFIG_PATH to the saved one, if the
	dnl saved version was not empty
        if test "x$s_PKG_CONFIG_PATH" != "x"; then
            export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${s_PKG_CONFIG_PATH}
        fi
    fi
])

dnl Check the $prefix versus the --with libdirpath for this component
dnl $1 is the prefix
dnl $2 is the --with component name
dnl $3 is the --with component library path
AC_DEFUN([GRC_PREFIX_LDFLAGS],[
    $2[_LIBDIRPATH]=$3
    dnl create LDFLAGS for this --with, if different from the provided $prefix
    if test [x]$1[/lib] != [x]$3; then
        $2[_LDFLAG]=[-L]$3
    else
        $2[_LDFLAG]=
    fi
])

dnl Check to make sure this dependency is fulfilled for this component
dnl $1 is the component's name
dnl $2 is the component dependency name
dnl On input and exit, $passed will be:
dnl   with : if --with passed muster
dnl   yes  : if --enable passed muster
dnl   no   : otherwise
dnl If trying --with, will error-out if any dependency was not --with'd
AC_DEFUN([GRC_CHECK_DEPENDENCY],[
dnl    f0=[enable_]m4_bpatsubst($1,-,_)
dnl    f1=[$enable_]m4_bpatsubst($1,-,_)
dnl    echo
dnl    echo "$1 : Checking Dependency $2"
dnl    echo "$1 : enable_all_components is '$enable_all_components'"
dnl    echo "$1 : $f0 is '$f1'"
dnl    echo "$1 : passed is '$passed'"
dnl    echo
    if test $passed != no; then
        if test $passed = yes; then
            dnl make sure this dependency was not skipped
            if test [x$]m4_bpatsubst($2,-,_)[_skipped] = xyes; then
                AC_MSG_RESULT([Component $1 requires $2, which is not being built or specified via pre-installed files.])
                passed=no
            fi
        else
            dnl make sure this dependency was --with'd only; not --enable'd
            if test [x$]m4_bpatsubst($2,-,_)[_with] = xno; then
                AC_MSG_ERROR([Component $1 requires $2 to be included as --with-$2@<:@=arg@:>@])
            fi
        fi
    fi
])

dnl Check to make sure GUILE is available
dnl $1 is the component name
AC_DEFUN([GRC_CHECK_GUILE],[
    if test x"$GUILE" = x; then
        AC_MSG_RESULT([Component $1 requires guile, which was not found.])
        passed=no
    fi
])

dnl Add the specified "with" list; clear the provided variable
dnl $1 is the component name
dnl $2 is the path list name suffix
dnl $3 is the separator (for paths, ":"; for includes " ")
AC_DEFUN([GRC_ADD_TO_LIST],[
    if test "x${$1[_]$2}" != "x"; then
        if test "x$[with_]$2" = "x"; then
            [with_]$2="${$1[_]$2}"
	else
	    [with_]$2="${$1[_]$2}"$3"$[with_]$2"
	fi
	$1[_]$2=
    fi
])

dnl Conditionally build named component.
dnl $1 is component name
dnl $2 is executed if configuration passes and build is desired
AC_DEFUN([GRC_BUILD_CONDITIONAL],[
    _GRC_BUILD_CONDITIONAL($1, $2, m4_bpatsubst($1,-,_))
])
dnl $3=m4_bpatsubst($1,-,_)
dnl Use $passed=no to indicate configuration failure;
dnl Use $passed=with to indicate the use of pre-installed libraries and headers;
dnl Any other value of $passed, including blank, assumes success;
dnl Defines $3_with=[yes|no] depending on if $passed=with or not (respectively)
dnl Defines $3_skipped=[yes|no] depending on if $passed=no or not (respectively)
AC_DEFUN([_GRC_BUILD_CONDITIONAL],[
    $3[_with]=no
    if test $passed = no; then
	if test x$[enable_]$3 = xyes; then
	    AC_MSG_ERROR([Component $1 has errors; stopping.])
	else
	    AC_MSG_RESULT([Not building component $1.])
	fi
    else
        if test $passed = with; then
	    with_dirs="$with_dirs $1"
	    GRC_ADD_TO_LIST($3, INCLUDES, " ")
	    GRC_ADD_TO_LIST($3, SWIG_INCLUDES, " ")
	    GRC_ADD_TO_LIST($3, PYDIRPATH, ":")
	    GRC_ADD_TO_LIST($3, SWIGDIRPATH, ":")
	    GRC_ADD_TO_LIST($3, LIBDIRPATH, ":")
	    GRC_ADD_TO_LIST($3, GUILE_LOAD_PATH, ":")
	    AC_MSG_RESULT([Component $1 will be included from a pre-installed library and includes.])
	    $3[_with]=yes
	else
	    $3[_LDFLAG]=
            if test x$[enable_]$3 != xno; then
	        ifelse([$2], , :, [$2])
	        build_dirs="$build_dirs $1"
	        AC_MSG_RESULT([Component $1 passed configuration checks; building.])
            else
	        passed=no
	        AC_MSG_RESULT([Component $1 passed configuration checks; but not building.])
	    fi
	fi
    fi
    if test $passed = no; then
        skipped_dirs="$skipped_dirs $1"
        $3[_skipped]=yes
    else
        $3[_skipped]=no
    fi
    AC_SUBST($3[_INCLUDES])
    AC_SUBST($3[_LA])
    AC_SUBST($3[_INCLUDEDIR])
    AC_SUBST($3[_LIBDIRPATH])
    AC_SUBST($3[_LDFLAG])
])

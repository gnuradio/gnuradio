# ===========================================================================
#                http://autoconf-archive.cryp.to/acx_blas.html
# ===========================================================================
#
# SYNOPSIS
#
#   ACX_CBLAS([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   This macro looks for a library that implements the CBLAS linear-algebra
#   interface (see http://www.netlib.org/blas/). On success, it sets the
#   CBLAS_LIBS output variable to hold the requisite library linkages.
#
#   To link with BLAS, you should link with:
#
#   	$CBLAS_LIBS $LIBS
#
#   Many libraries are searched for, from ATLAS to CXML to ESSL. The user
#   may also use --with-blas=<lib> in order to use some specific CBLAS
#   library <lib>. In order to link successfully, however, be aware that you
#   will probably need to use the same Fortran compiler (which can be set
#   via the F77 env. var.) as was used to compile the BLAS library.
#
#   ACTION-IF-FOUND is a list of shell commands to run if a CBLAS library is
#   found, and ACTION-IF-NOT-FOUND is a list of commands to run it if it is
#   not found. If ACTION-IF-FOUND is not specified, the default action will
#   define HAVE_CBLAS.
#
#   This macro requires autoconf 2.50 or later.
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Steven G. Johnson <stevenj@alum.mit.edu>
#   Copyright (c) 2008 Free Software Foundation, Inc.
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

AC_DEFUN([ACX_CBLAS], [
AC_PREREQ(2.50)
dnl AC_REQUIRE([AC_F77_LIBRARY_LDFLAGS])
AC_REQUIRE([AC_CANONICAL_HOST])
acx_cblas_ok=no

AC_ARG_WITH(cblas,
	[AC_HELP_STRING([--with-cblas=<lib>], [use CBLAS library <lib>])])
case $with_cblas in
	yes | "") ;;
	no) acx_cblas_ok=disable ;;
	-* | */* | *.a | *.so | *.so.* | *.o) CBLAS_LIBS="$with_cblas" ;;
	*) CBLAS_LIBS="-l$with_cblas" ;;
esac


acx_cblas_save_LIBS="$LIBS"
LIBS="$LIBS $FLIBS"

# First, check CBLAS_LIBS environment variable
if test $acx_cblas_ok = no; then
if test "x$CBLAS_LIBS" != x; then
	save_LIBS="$LIBS"; LIBS="$CBLAS_LIBS $LIBS"
	AC_MSG_CHECKING([for cblas_sgemm in $CBLAS_LIBS])
	AC_TRY_LINK_FUNC(cblas_sgemm, [acx_cblas_ok=yes], [CBLAS_LIBS=""])
	AC_MSG_RESULT($acx_cblas_ok)
	LIBS="$save_LIBS"
fi
fi

# CBLAS linked to by default?  (happens on some supercomputers)
if test $acx_cblas_ok = no; then
	save_LIBS="$LIBS"; LIBS="$LIBS"
	AC_CHECK_FUNC(cblas_sgemm, [acx_cblas_ok=yes])
	LIBS="$save_LIBS"
fi

# CBLAS in ATLAS library? (http://math-atlas.sourceforge.net/)
if test $acx_cblas_ok = no; then
	AC_CHECK_LIB(atlas, ATL_xerbla,
	    [AC_CHECK_LIB(cblas, cblas_dgemm,
	        [acx_cblas_ok=yes
		 CBLAS_LIBS="-lcblas -latlas"],
                [], [-latlas])],
	    [], [-latlas])
fi

# CBLAS in ATLAS library in /usr/lib64/atlas (if not cross-compiling)
if test $acx_cblas_ok = no && test "$cross_compiling" = no; then
	unset ac_cv_lib_atlas_ATL_xerbla
	unset ac_cv_lib_cblas_cblas_dgemm
	AC_CHECK_LIB(atlas, ATL_xerbla,
	    [AC_CHECK_LIB(cblas, cblas_dgemm,
	        [acx_cblas_ok=yes
		 CBLAS_LIBS="-L/usr/lib64/atlas -lcblas -latlas"],
                [], [-L/usr/lib64/atlas -latlas])],
	    [], [-L/usr/lib64/atlas -latlas])
fi

# CBLAS in ATLAS library in /usr/lib/atlas (if not cross-compiling)
if test $acx_cblas_ok = no && test "$cross_compiling" = no; then
	unset ac_cv_lib_atlas_ATL_xerbla
	unset ac_cv_lib_cblas_cblas_dgemm
	AC_CHECK_LIB(atlas, ATL_xerbla,
	    [AC_CHECK_LIB(cblas, cblas_dgemm,
	        [acx_cblas_ok=yes
		 CBLAS_LIBS="-L/usr/lib/atlas -lcblas -latlas"],
                [], [-L/usr/lib/atlas -latlas])],
	    [], [-L/usr/lib/atlas -latlas])
fi


# CBLAS in PhiPACK libraries? (requires generic BLAS lib, too)
if test $acx_cblas_ok = no; then
	AC_CHECK_LIB(cblas, cblas_sgemm,
		[AC_CHECK_LIB(dgemm, cblas_dgemm,
		[AC_CHECK_LIB(sgemm, cblas_sgemm,
			[acx_cblas_ok=yes; CBLAS_LIBS="-lsgemm -ldgemm -lblas"],
			[], [-lblas])],
			[], [-lblas])])
fi

# BLAS in Intel MKL library?
if test $acx_cblas_ok = no; then
	AC_CHECK_LIB(mkl, cblas_sgemm, [acx_cblas_ok=yes;CBLAS_LIBS="-lmkl"])
fi

# CBLAS in Apple vecLib library?
if test $acx_cblas_ok = no; then
	save_LIBS="$LIBS"; LIBS="-framework vecLib $LIBS"
	AC_CHECK_FUNC(cblas_sgemm, [acx_cblas_ok=yes;CBLAS_LIBS="-framework vecLib"])
	LIBS="$save_LIBS"
fi

# CBLAS in Alpha CXML library?
if test $acx_cblas_ok = no; then
	AC_CHECK_LIB(cxml, cblas_sgemm, [acx_cblas_ok=yes;CBLAS_LIBS="-lcxml"])
fi

# CBLAS in Alpha DXML library? (now called CXML, see above)
if test $acx_cblas_ok = no; then
	AC_CHECK_LIB(dxml, cblas_sgemm, [acx_cblas_ok=yes;CBLAS_LIBS="-ldxml"])
fi

# CBLAS in Sun Performance library?
if test $acx_cblas_ok = no; then
	if test "x$GCC" != xyes; then # only works with Sun CC
		AC_CHECK_LIB(sunmath, acosp,
			[AC_CHECK_LIB(sunperf, cblas_sgemm,
        			[CBLAS_LIBS="-xlic_lib=sunperf -lsunmath"
                                 acx_cblas_ok=yes],[],[-lsunmath])])
	fi
fi

# CBLAS in SCSL library?  (SGI/Cray Scientific Library)
if test $acx_cblas_ok = no; then
	AC_CHECK_LIB(scs, cblas_sgemm, [acx_cblas_ok=yes; CBLAS_LIBS="-lscs"])
fi

# CBLAS in SGIMATH library?
if test $acx_cblas_ok = no; then
	AC_CHECK_LIB(complib.sgimath, cblas_sgemm,
		     [acx_cblas_ok=yes; CBLAS_LIBS="-lcomplib.sgimath"])
fi

# CBLAS in IBM ESSL library? (requires generic CBLAS lib, too)
if test $acx_cblas_ok = no; then
	AC_CHECK_LIB(cblas, cblas_sgemm,
		[AC_CHECK_LIB(essl, cblas_sgemm,
			[acx_cblas_ok=yes; CBLAS_LIBS="-lessl -lblas"],
			[], [-lblas $FLIBS])])
fi

# Generic CBLAS library?
if test $acx_cblas_ok = no; then
	AC_CHECK_LIB(cblas, cblas_sgemm, [acx_cblas_ok=yes; CBLAS_LIBS="-lcblas"])
fi

# Generic BLAS library?
if test $acx_cblas_ok = no; then
	AC_CHECK_LIB(blas, cblas_sgemm, [acx_cblas_ok=yes; CBLAS_LIBS="-lblas"])
fi

# GSL generic CBLAS library?
if test $acx_cblas_ok = no; then
	AC_CHECK_LIB(gslcblas, cblas_sgemm, [acx_cblas_ok=yes; CBLAS_LIBS="-lgslcblas"])
fi

AC_SUBST(CBLAS_LIBS)

LIBS="$acx_cblas_save_LIBS"

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_cblas_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_CBLAS,1,[Define if you have a CBLAS library.]),[$1])
        :
else
        acx_cblas_ok=no
        $2
fi
])dnl ACX_CBLAS

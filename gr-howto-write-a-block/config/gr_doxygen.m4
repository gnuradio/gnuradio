dnl
dnl Copyright 2003,2005 Free Software Foundation, Inc.
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
dnl the Free Software Foundation, Inc., 51 Franklin Street,
dnl Boston, MA 02110-1301, USA.
dnl 

AC_DEFUN([GR_CHECK_DOXYGEN],[
  AC_ARG_ENABLE(doxygen, [  --enable-doxygen        enable documentation generation with doxygen (no)])
  AC_ARG_ENABLE(dot, [  --enable-dot            use 'dot' to generate graphs in doxygen (auto)])
  AC_ARG_ENABLE(html-docs, [  --enable-html-docs      enable HTML generation with doxygen (yes)], [], [ enable_html_docs=yes])
  AC_ARG_ENABLE(latex-docs, [  --enable-latex-docs     enable LaTeX doc generation with doxygen (no)], [], [ enable_latex_docs=no])

  if test "x$enable_doxygen" = xyes; then
        AC_PATH_PROG(DOXYGEN, doxygen, , $PATH)
        if test x$DOXYGEN = x; then
                if test "x$enable_doxygen" = xyes; then
                        AC_MSG_ERROR([could not find doxygen])
                fi
                enable_doc=no
		generate_docs=
        else
                enable_doc=yes
		generate_docs=docs
                AC_PATH_PROG(DOT, dot, , $PATH)
        fi
  else 
        enable_doc=no
  fi

  AM_CONDITIONAL(DOC, test x$enable_doc = xyes)

  if test x$DOT = x; then
        if test "x$enable_dot" = xyes; then
                AC_MSG_ERROR([could not find dot])
        fi
        enable_dot=no
  else
        enable_dot=yes
  fi
  AC_SUBST(enable_dot)
  AC_SUBST(enable_html_docs)
  AC_SUBST(enable_latex_docs)
  AC_SUBST(generate_docs)
])

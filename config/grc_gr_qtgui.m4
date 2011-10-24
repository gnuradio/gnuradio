dnl Copyright 2001,2002,2003,2004,2005,2006,2008,2011 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_GR_QTGUI],[
    GRC_ENABLE(gr-qtgui)

    dnl Don't do gr-qtgui if gnuradio-core skipped
    GRC_CHECK_DEPENDENCY(gr-qtgui, gnuradio-core)

    dnl If execution gets to here, $passed will be:
    dnl   with : if the --with code didn't error out
    dnl   yes  : if the --enable code passed muster and all dependencies are met
    dnl   no   : otherwise

    PYTHON_CHECK_MODULE([PyQt4.QtCore], [PyQt4 for Qt4], \
        [], [passed=no], \
        [PyQt4.QtCore.PYQT_VERSION >= 260000])

    PYTHON_CHECK_MODULE([PyQt4.Qwt5], [PyQwt5 for Qt4], \
       [], [passed=no], \
       [PyQt4.Qwt5.QWT_VERSION >= 327000])

# Check for: 
#	QtOpenGL
#	QtGui
#	QtCore
#	qwt 
#	qt4

# qt4-core, qt4-gui, qwt5-qt4, qwt5-qt4-dev, qt4-dev-tools

    if test $passed = yes; then
        dnl Check for package qt or qt-mt, set QT_CFLAGS and QT_LIBS
        PKG_CHECK_MODULES(QTCORE, QtCore >= 4.4, [],
	    [passed=no; AC_MSG_RESULT([gr-qtgui requires libQtCore >= 4.4.])])
        PKG_CHECK_MODULES(QTGUI, QtGui >= 4.4, [],
	    [passed=no; AC_MSG_RESULT([gr-qtgui requires libQtGui >= 4..])])
	
        dnl Fetch QWT variables
        GR_QWT([], [passed=no])

	dnl Export the include dirs and libraries (note: QTOPENGL_LIBS includes links
	dnl to QtCore and QtGui libraries)
	QT_INCLUDES="$QWT_CFLAGS $QTCORE_CFLAGS $QTGUI_CFLAGS"
	QT_LIBS="$QWT_LIBS"

        dnl Build an includes variable specifically for running qmake by extracting
        dnl all includes from the QWT, without the -I;
        dnl qmake appends the -I when processing the project file INCLUDEPATH
        for i in $QWT_CFLAGS; do
            QMAKE_INCLUDES="$QMAKE_INCLUDES ${i##-I}"
        done

	QT_MOC_EXEC=`pkg-config --variable=moc_location QtCore`
	QT_UIC_EXEC=`pkg-config --variable=uic_location QtCore`

        gr_qtgui_INCLUDES="\
-I\${abs_top_srcdir}/gr-qtgui/include \
-I\${abs_top_srcdir}/gr-qtgui/swig"
        gr_qtgui_LA="\${abs_top_builddir}/gr-qtgui/lib/libgnuradio-qtgui.la"
	gr_qtgui_LIBDIRPATH="\${abs_top_builddir}/gr-qtgui/lib:\${abs_top_builddir}/gr-qtgui/lib/.libs"
	gr_qtgui_SWIGDIRPATH="\${abs_top_builddir}/gr-digtial/lib/swig:\${abs_top_builddir}/gr-qtgui/swig/.libs:\${abs_top_srcdir}/gr-qtgui/swig"
	gr_qtgui_PYDIRPATH="\${abs_top_srcdir}/gr-qtgui/python"

        AC_SUBST(QMAKE_INCLUDES)
        AC_SUBST(QT_INCLUDES)
	AC_SUBST(QT_LIBS)
	AC_SUBST(QT_MOC_EXEC)
	AC_SUBST(QT_UIC_EXEC)
    fi

    AC_CONFIG_FILES([ \
        gr-qtgui/Makefile \
	gr-qtgui/gnuradio-qtgui.pc \
        gr-qtgui/apps/Makefile \
        gr-qtgui/examples/Makefile \
        gr-qtgui/grc/Makefile \
	gr-qtgui/include/Makefile \
        gr-qtgui/lib/Makefile \
        gr-qtgui/python/Makefile \
        gr-qtgui/python/run_tests \
        gr-qtgui/swig/Makefile \
	gr-qtgui/doc/Makefile \
    ])

    GRC_BUILD_CONDITIONAL(gr-qtgui,[
        dnl run_tests is created from run_tests.in.  Make it executable.
        AC_CONFIG_COMMANDS([run_tests_qtgui],
			   [chmod +x gr-qtgui/python/run_tests])
    ])
])

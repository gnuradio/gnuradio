AC_DEFUN([HUNTER_WX], [
	AC_PATH_PROG([WXCONFIG],[wx-config],[no])
	if test $WXCONFIG = no; then
		AC_MSG_ERROR(["wxWidgets is required, not found, stop."])
	fi

	WX_FLAGS=`$WXCONFIG --debug --cflags`
	WX_LIBS=`$WXCONFIG --debug --libs`
	AC_SUBST(WX_FLAGS)
	AC_SUBST(WX_LIBS)
])

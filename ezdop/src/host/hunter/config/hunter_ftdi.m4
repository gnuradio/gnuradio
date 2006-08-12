AC_DEFUN([HUNTER_FTDI],[
    AC_LANG_PUSH(C)
    
    AC_CHECK_HEADER([ftdi.h],[],[
	AC_MSG_ERROR(["Hunter requires ftdi.h, not found, stop."])]
    )
    
    save_LIBS="$LIBS"
    AC_SEARCH_LIBS(ftdi_init, [ftdi], [FTDI_LIBS=$LIBS],[
	AC_MSG_ERROR(["Hunter requires libftdi, not found, stop."])]
    )

    LIBS="$save_LIBS"
    AC_LANG_POP(C)

    AC_SUBST(FTDI_LIBS)
    AC_DEFINE([HAVE_LIBFTDI],[1],[Define to 1 if your system has libftdi.])
])

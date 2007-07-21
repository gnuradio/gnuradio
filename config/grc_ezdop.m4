dnl Copyright 2001,2002,2003,2004,2005,2006 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_EZDOP],[
    GRC_ENABLE([ezdop])

    AC_CONFIG_FILES([ \
	ezdop/Makefile \
	ezdop/ezdop.pc \
	ezdop/src/Makefile \
	ezdop/src/firmware/Makefile \
	ezdop/src/host/Makefile \
	ezdop/src/host/avrdude/Makefile \
	ezdop/src/host/ezdop/Makefile \
	ezdop/src/host/tests/Makefile \
    ])

    passed=yes

    # See ticket:96 in trac
    AM_PROG_CC_C_O

    # Firmware build requires Atmel AVR microcontroller port of GCC
    AC_PATH_PROG([AVRGCC], [avr-gcc -v], [no])
    if test x$AVRGCC = xno; then
    	passed=no
	AC_MSG_RESULT([ezdop requires avr-gcc, not found.])
    fi
    
    # ...and binutils
    AC_PATH_PROG([AVROBJCOPY], [avr-objcopy], [no])
    if test x$AVROBJCOPY = xno; then
    	passed=no
	AC_MSG_RESULT([ezdop requires avr binutils, not found.])
    fi
    
    # ...and standard library (test not working yet)
    #AC_CHECK_HEADERS([avr/io.h],[],
    #   [passed=no;AC_MSG_RESULT([ezdop requires avr glibc, not found.])])

    # Device access is via libftdi
    AC_LANG_PUSH(C)
    AC_CHECK_HEADERS([ftdi.h],[],
        [passed=no;AC_MSG_RESULT([ezdop requires ftdi.h, not found.])])
    save_LIBS="$LIBS"
    AC_SEARCH_LIBS([ftdi_init], [ftdi],[FTDI_LIBS="$LIBS"],
        [passed=no;AC_MSG_RESULT([ezdop requires libftdi, not found.])])
    LIBS="$save_LIBS"
    AC_LANG_POP

    GRC_BUILD_CONDITIONAL([ezdop],[
	EZDOP_INCLUDES='-I$(top_srcdir)/ezdop/src/host/ezdop/ -I$(top_srcdir)/ezdop/src/firmware/'
	EZDOP_LIBS='-lezdop'
	AC_SUBST(FTDI_LIBS)
	AC_SUBST(EZDOP_INCLUDES)
	AC_SUBST(EZDOP_LIBS)
	AC_SUBST(AVRGCC)
	AC_SUBST(AVROBJCOPY)
    ])
])

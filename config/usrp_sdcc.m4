# Check for sdcc support.             		-*- Autoconf -*-

# Copyright 2004 Free Software Foundation, Inc.

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
# Foundation, Inc., 51 Franklin Street, Boston, MA
# 02110-1301, USA.

AC_DEFUN([USRP_SDCC],
[
	sdccok=yes
	AC_CHECK_PROG(XCC, sdcc, sdcc -mmcs51 --no-xinit-opt,no)
	AC_CHECK_PROG(XAS, asx8051, asx8051 -plosgff,no)

	if test "$XCC" = "no" -o "$XAS" = "no" ; then
		AC_MSG_RESULT([USRP requires sdcc. sdcc not found. See http://sdcc.sf.net])
		sdccok=no
	else
		sdcc_version_min=$1

		sdcc_version=`sdcc --version 2>&1 | \
			sed  's/\(SDCC.* \)\([[0-9]]*\.[[0-9]]*\.[[0-9]]*\)\( .*$\)/\2/'`

	        AC_MSG_CHECKING([sdcc_version "$sdcc_version"])

		sdcc_major_version=`echo $sdcc_version | \
			sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
		sdcc_minor_version=`echo $sdcc_version | \
			sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
		sdcc_micro_version=`echo $sdcc_version | \
			sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

		sdcc_major_min=`echo $sdcc_version_min | \
			sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
		sdcc_minor_min=`echo $sdcc_version_min | \
			sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
		sdcc_micro_min=`echo $sdcc_version_min | \
			sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

		sdcc_version_proper=`expr \
			"$sdcc_major_version" \> "$sdcc_major_min" \| \
			"$sdcc_major_version" \= "$sdcc_major_min" \& \
	    		"$sdcc_minor_version" \> "$sdcc_minor_min" \| \
			"$sdcc_major_version" \= "$sdcc_major_min" \& \
			"$sdcc_minor_version" \= "$sdcc_minor_min" \& \
			"$sdcc_micro_version" \>= "$sdcc_micro_min" `

		if test "$sdcc_version_proper" = "1" ; then
			AC_MSG_RESULT([$sdcc_major_version.$sdcc_minor_version.$sdcc_micro_version])
		else
			sdccok=no
			AC_MSG_RESULT([USRP requires sdcc >= $sdcc_version_min. sdcc not found. See http://sdcc.sf.net"])
		fi

		AC_SUBST(XCC)
		AC_SUBST(XAS)
	fi

	if test $sdccok = yes; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$3], , :, [$3])
	fi
])

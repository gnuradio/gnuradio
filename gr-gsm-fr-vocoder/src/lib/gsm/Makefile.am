#
# Copyright 2005,2008 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

include $(top_srcdir)/Makefile.common

# Machine- or installation dependent flags you should configure to port

SASR	= -DSASR
######### Define SASR if >> is a signed arithmetic shift (-1 >> 1 == -1)

MULHACK = -DUSE_FLOAT_MUL
######### Define this if your host multiplies floats faster than integers,
######### e.g. on a SPARCstation.

FAST	= -DFAST
######### Define together with USE_FLOAT_MUL to enable the GSM library's
######### approximation option for incorrect, but good-enough results.

# LTP_CUT	= -DLTP_CUT
LTP_CUT	=
######### Define to enable the GSM library's long-term correlation 
######### approximation option---faster, but worse; works for
######### both integer and floating point multiplications.
######### This flag is still in the experimental stage.

OPTIONS = $(SASR) $(MULHACK) $(FAST) $(LTP_CUT)

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) -DNeedFunctionPrototypes=1 \
	$(OPTIONS) $(WITH_INCLUDES)

noinst_LTLIBRARIES = libgsm.la

libgsm_la_SOURCES = 	\
	add.c		\
	code.c		\
	debug.c		\
	decode.c	\
	gsm_create.c	\
	gsm_decode.c	\
	gsm_destroy.c	\
	gsm_encode.c	\
	gsm_explode.c	\
	gsm_implode.c	\
	gsm_option.c	\
	gsm_print.c	\
	long_term.c	\
	lpc.c		\
	preprocess.c	\
	rpe.c		\
	short_term.c	\
	table.c		

noinst_HEADERS =	\
	config.h	\
	gsm.h		\
	private.h	\
	proto.h		\
	unproto.h	

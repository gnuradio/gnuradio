/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <libguile.h>

#include "xyzzy.h"

/*
 * Load and run a waveform defined with define-waveform
 * usage: gr-run-waveform filename.wfd [args...]
 */
static const char *code =
"(set! %load-verbosely #t)				\n"
"(define primitive-load xyzzy-primitive-load)		\n"
"(define primitive-load-path xyzzy-primitive-load-path)	\n"
"(define search-path xyzzy-search-path)			\n"
"(define search-load-path xyzzy-search-load-path)	\n"
"\n"
"(primitive-load-path \"gnuradio/run-waveform\")	\n"
"\n"
"(define (main args)					\n"
"  (if (not (>= (length args) 2))			\n"
"      (let ((port (current-error-port)))		\n"
"	(display \"usage: \" port)			\n"
"	(display (car args) port)			\n"
"	(display \" filename.wfd [args...]\n\" port)	\n"
"	(exit 1)))					\n"
"  (apply run-waveform (cdr args)))			\n"
"\n"
"(main (command-line))					\n"
;


static void
inner_main (void *data, int argc, char **argv)
{
  if (!xyzzy_init(0))	// use compiled-in install path
    exit(1);

  scm_xyzzy_init();
  scm_c_eval_string(code);
}

int
main(int argc, char *argv[])
{
    setenv("GUILE_WARN_DEPRECATED", "no", 1);
    // setenv("LTDL_LIBRARY_PATH", "/home/eb/install/lib64", 1);

    scm_boot_guile (argc, argv, inner_main, 0);

    return 0; // never reached
}

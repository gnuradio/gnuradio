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
 * Test our embedded version of guile
 */

static bool
prepend_to_env(const char *name, const char *value)
{
  char *c_old = getenv(name);
  std::string new_val;
  if (c_old)
    new_val = std::string(value) + std::string(":") + std::string(c_old);
  else
    new_val = std::string(value);

  return setenv(name, new_val.c_str(), 1) == 0;
}

static const char *code = "\
;;(set! %load-verbosely #t)				\n\
							\n\
(save-module-excursion					\n\
 (lambda ()						\n\
   (set-current-module (resolve-module '(guile)))	\n\
   (set! primitive-load xyzzy-primitive-load)		\n\
   (set! primitive-load-path xyzzy-primitive-load-path)	\n\
   (set! search-path xyzzy-search-path)			\n\
   (set! %search-load-path %xyzzy-search-load-path)))	\n\
";

static void
inner_main (void *data, int argc, char **argv)
{
  // Load pseudo filesystem
  if (!xyzzy_init("./filesystem.dat"))
    exit(1);

  // Initialize our gsubrs
  scm_xyzzy_init();

  // Override standard code with our gsubrs
  scm_c_eval_string(code);

  // REPL
  scm_shell (argc, argv);
}

int
main(int argc, char *argv[])
{
    // kill warnings
    setenv("GUILE_WARN_DEPRECATED", "no", 1);

    // where to find our files and libraries
    prepend_to_env("GUILE_LOAD_PATH", "/-xyzzy-");
    prepend_to_env("LTDL_LIBRARY_PATH", LIBDIR);
    prepend_to_env("DYLD_LIBRARY_PATH", LIBDIR);

    scm_boot_guile (argc, argv, inner_main, 0);

    return 0; // never reached
}

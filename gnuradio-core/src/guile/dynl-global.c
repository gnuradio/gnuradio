/* -*- c -*- */
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This file is an attempt to work around a problem that appears on
 * certain Ubuntu (and perhaps other) systems.  On those systems
 * (10.04 is known to have the problem, while 10.10 and later work OK
 * without this kludge), we end up with a situation where exceptions
 * are not caught by the swig code, even though the swig generated
 * code "looks right" and "is right".  Details of the issue can be
 * found in swig bug 1863647,
 * http://sourceforge.net/tracker/index.php?func=detail&aid=1863647&group_id=1645&atid=101645
 *
 * We work around the problem by loading swig generated guile modules
 * using the equivalent of the dlopen's RTLD_GLOBAL flag.  This is
 * only possible on systems using libtool-2.*.  Those systems contain
 * the lt_dlavise_global function.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ltdl.h>
#include <libguile.h>

extern scm_t_bits scm_tc16_dynamic_obj;

#ifdef HAVE_LT_DLADVISE_GLOBAL
/*
 * Load shared module using the equivalent of the RTLD_GLOBAL flag
 */
static lt_dlhandle
dlopenext_global (const char *filename)
{
  lt_dlhandle handle = 0;
  lt_dladvise advise;

  if (!lt_dladvise_init (&advise)
      && !lt_dladvise_ext (&advise)
      && !lt_dladvise_global(&advise))
    handle = lt_dlopenadvise (filename, advise);

  lt_dladvise_destroy (&advise);
  return handle;
}

#else

/*
 * We don't have lt_dladvise_global.  Fall back to lt_dlopenext.
 */
static lt_dlhandle
dlopenext_global (const char *filename)
{
  return lt_dlopenext (filename);
}
#endif


static void *
sysdep_dynl_link_global (const char *fname, const char *subr)
{
  lt_dlhandle handle;
  handle = dlopenext_global (fname);
  if (NULL == handle)
    {
      SCM fn;
      SCM msg;

      fn = scm_from_locale_string (fname);
      msg = scm_from_locale_string (lt_dlerror ());
      scm_misc_error (subr, "file: ~S, message: ~S", scm_list_2 (fn, msg));
    }
  return (void *) handle;
}

SCM_DEFINE (scm_dynamic_link_global, "dynamic-link-global", 1, 0, 0, 
            (SCM filename),
	    "Find the shared object (shared library) denoted by\n"
	    "@var{filename} and link it into the running Guile\n"
	    "application.  The returned\n"
	    "scheme object is a ``handle'' for the library which can\n"
	    "be passed to @code{dynamic-func}, @code{dynamic-call} etc.\n\n"
	    "Searching for object files is system dependent.  Normally,\n"
	    "if @var{filename} does have an explicit directory it will\n"
	    "be searched for in locations\n"
	    "such as @file{/usr/lib} and @file{/usr/local/lib}.")
#define FUNC_NAME s_scm_dynamic_link_global
{
  void *handle;
  char *file;

  scm_dynwind_begin (0);
  file = scm_to_locale_string (filename);
  scm_dynwind_free (file);
  handle = sysdep_dynl_link_global (file, FUNC_NAME);
  scm_dynwind_end ();
  SCM_RETURN_NEWSMOB2 (scm_tc16_dynamic_obj, SCM_UNPACK (filename), handle);
}
#undef FUNC_NAME

void
scm_init_gnuradio_dynl_global_module(void)
{
  scm_c_define_gsubr (s_scm_dynamic_link_global, 1, 0, 0, (SCM (*)()) scm_dynamic_link_global);
}

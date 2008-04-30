/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_preferences.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


#ifdef MKDIR_TAKES_ONE_ARG
#define gr_mkdir(pathname, mode) mkdir(pathname)
#else
#define gr_mkdir(pathname, mode) mkdir((pathname), (mode))
#endif

/*
 * The simplest thing that could possibly work:
 *  the key is the filename; the value is the file contents.
 */

static const char *
pathname (const char *key)
{
  static char buf[200];
  snprintf (buf, sizeof (buf), "%s/.gnuradio/prefs/%s", getenv ("HOME"), key);
  return buf;
}

static void
ensure_dir_path ()
{
  char path[200];
  struct stat	statbuf;

  snprintf (path, sizeof (path), "%s/.gnuradio/prefs", getenv ("HOME"));
  if (stat (path, &statbuf) == 0 && S_ISDIR (statbuf.st_mode))
    return;

  // blindly try to make it 	// FIXME make this robust. C++ SUCKS!

  snprintf (path, sizeof (path), "%s/.gnuradio", getenv ("HOME"));
  gr_mkdir (path, 0750);
  snprintf (path, sizeof (path), "%s/.gnuradio/prefs", getenv ("HOME"));
  gr_mkdir (path, 0750);
}

const char *
gr_preferences::get (const char *key)
{
  static char buf[1024];

  FILE 	*fp = fopen (pathname (key), "r");
  if (fp == 0)
    return 0;

  memset (buf, 0, sizeof (buf));
  fread (buf, 1, sizeof (buf) - 1, fp);
  fclose (fp);
  return buf;
}

void 
gr_preferences::set (const char *key, const char *value)
{
  ensure_dir_path ();

  FILE *fp = fopen (pathname (key), "w");
  if (fp == 0){
    perror (pathname (key));
    return;
  }

  fwrite (value, 1, strlen (value), fp);
  fclose (fp);
};

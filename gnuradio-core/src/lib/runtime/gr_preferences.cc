/* -*- c++ -*- */
/*
 * Copyright 2003,2010,2011 Free Software Foundation, Inc.
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
#include <gr_sys_paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

/*
 * The simplest thing that could possibly work:
 *  the key is the filename; the value is the file contents.
 */

static const char *
pathname (const char *key)
{
  static fs::path path;
  path = fs::path(gr_appdata_path()) / ".gnuradio" / "prefs" / key;
  return path.string().c_str();
}

static void
ensure_dir_path ()
{
  fs::path path = fs::path(gr_appdata_path()) / ".gnuradio";
  if (!fs::is_directory(path)) fs::create_directory(path);

  path = path / "prefs";
  if (!fs::is_directory(path)) fs::create_directory(path);
}

const char *
gr_preferences::get (const char *key)
{
  static char buf[1024];

  FILE 	*fp = fopen (pathname (key), "r");
  if (fp == 0) {
    perror (pathname (key));
    return 0;
  }

  memset (buf, 0, sizeof (buf));
  size_t ret = fread (buf, 1, sizeof (buf) - 1, fp);
  if(ret == 0) {
    if(ferror(fp) != 0) {
      perror (pathname (key));
      fclose (fp);
      return 0;
    }
  }
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

  size_t ret = fwrite (value, 1, strlen (value), fp);
  if(ret == 0) {
    if(ferror(fp) != 0) {
      perror (pathname (key));
      fclose (fp);
      return;
    }
  }
  fclose (fp);
};

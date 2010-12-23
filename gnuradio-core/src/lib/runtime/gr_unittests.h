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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#ifdef MKDIR_TAKES_ONE_ARG
#define gr_mkdir(pathname, mode) mkdir(pathname)
#else
#define gr_mkdir(pathname, mode) mkdir((pathname), (mode))
#endif

/*
 * Mostly taken from gr_preferences.cc/h
 * The simplest thing that could possibly work:
 *  the key is the filename; the value is the file contents.
 */

static void
ensure_unittest_path (const char *path)
{
  struct stat statbuf;
  if (stat (path, &statbuf) == 0 && S_ISDIR (statbuf.st_mode))
    return;

  // blindly try to make it 	// FIXME make this robust. C++ SUCKS!
  gr_mkdir (path, 0750);
}

static void
get_unittest_path (const char *filename, char *fullpath, size_t pathsize)
{
  char path[200];
  snprintf (path, sizeof(path), "./.unittests");
  snprintf (fullpath, pathsize, "%s/%s", path, filename);

  ensure_unittest_path(path);
}


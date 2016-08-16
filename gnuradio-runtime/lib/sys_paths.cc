/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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

#include <gnuradio/sys_paths.h>
#include <cstdlib> //getenv
#include <cstdio>  //P_tmpdir (maybe)

#include <boost/filesystem/path.hpp>

namespace gr {

  const char *tmp_path()
  {
    const char *path;

    //first case, try TMP environment variable
    path = getenv("TMP");
    if(path)
      return path;

    //second case, try P_tmpdir when its defined
    #ifdef P_tmpdir
    if(P_tmpdir)
      return P_tmpdir;
    #endif /*P_tmpdir*/

    //fall-through case, nothing worked
    return "/tmp";
  }

  const char *appdata_path()
  {
    const char *path;

    //first case, try HOME environment variable (unix)
    path = getenv("HOME");
    if(path)
      return path;

    //second case, try APPDATA environment variable (windows)
    path = getenv("APPDATA");
    if(path)
      return path;

    //fall-through case, nothing worked
    return tmp_path();
  }

  std::string __userconf_path()
  {
    boost::filesystem::path p(appdata_path());
    p = p / ".gnuradio";
    return p.string();
  }

  const char *userconf_path()
  {
    static std::string p(__userconf_path());
    return p.c_str();
  }

}  /* namespace gr */

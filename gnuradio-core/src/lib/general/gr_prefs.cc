/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <gr_prefs.h>
#include <gr_sys_paths.h>
#include <gr_constants.h>
#include <algorithm>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
namespace fs = boost::filesystem;

/*
 * Stub implementations
 */

static gr_prefs	 s_default_singleton;
static gr_prefs  *s_singleton = &s_default_singleton;

gr_prefs *
gr_prefs::singleton()
{
  return s_singleton;
}

void
gr_prefs::set_singleton(gr_prefs *p)
{
  s_singleton = p;
}

gr_prefs::gr_prefs()
{
  _read_files();
}

gr_prefs::~gr_prefs()
{
  // nop
}

std::vector<std::string>
gr_prefs::_sys_prefs_filenames()
{
  std::vector<std::string> fnames;

  fs::path dir = gr_prefsdir();
  if(!fs::is_directory(dir))
    return fnames;

  fs::directory_iterator diritr(dir);
  while(diritr != fs::directory_iterator()) {
    fs::path p = *diritr++;
    fnames.push_back(p.string());
  }
  std::sort(fnames.begin(), fnames.end());

  // Find if there is a ~/.gnuradio/config file and add this to the
  // beginning of the file list to override any preferences in the
  // installed path config files.
  fs::path homedir = fs::path(gr_appdata_path());
  homedir = homedir/".gnuradio/config.conf";
  if(fs::exists(homedir)) {
    fnames.insert(fnames.begin(), homedir.string());
  }    

  return fnames;
}

void
gr_prefs::_read_files()
{
  std::vector<std::string> filenames = _sys_prefs_filenames();
  std::vector<std::string>::iterator sitr;
  char tmp[1024];
  for(sitr = filenames.begin(); sitr != filenames.end(); sitr++) {
    fs::ifstream fin(*sitr);
    while(!fin.eof()) {
      fin.getline(tmp, 1024);
      std::string t(tmp);
      // ignore empty lines or lines of just comments
      if((t.size() > 0) && (t[0] != '#')) {
	// remove any comments in the line
	size_t hash = t.find("#");

	// Use hash marks at the end of each segment as a delimiter
	d_configs += t.substr(0, hash) + '#';
      }
    }
    fin.close();
  }

  // Remove all whitespace
  d_configs.erase(std::remove_if(d_configs.begin(), d_configs.end(), ::isspace), d_configs.end());
}

bool
gr_prefs::has_section(const std::string &section)
{
  size_t t = d_configs.find("[" + section + "]#");
  return t != std::string::npos;
}

bool
gr_prefs::has_option(const std::string &section, const std::string &option)
{
  if(has_section(section)) {
    size_t sec = d_configs.find("[" + section + "]#");
    size_t opt = d_configs.find("#" + option + "=", sec);
    return opt != std::string::npos;
  }
  else {
    return false;
  }
}

const std::string
gr_prefs::get_string(const std::string &section, const std::string &option,
		     const std::string &default_val)
{
  std::stringstream envname;
  std::string secname=section, optname=option;

  std::transform(section.begin(), section.end(), secname.begin(), ::toupper);
  std::transform(option.begin(), option.end(), optname.begin(), ::toupper);
  envname << "GR_CONF_" << secname << "_" << optname;

  char *v = getenv(envname.str().c_str());
  if(v) {
    return std::string(v);
  }

  if(has_option(section, option)) {
    std::string optname = "#" + option + "=";
    size_t sec = d_configs.find("[" + section + "]#");
    size_t opt = d_configs.find(optname, sec);

    size_t start = opt + optname.size();
    size_t end = d_configs.find("#", start);
    size_t len = end - start;

    return d_configs.substr(start, len);
  }
  else {
    return default_val;
  }
}

bool
gr_prefs::get_bool(const std::string &section, const std::string &option, bool default_val)
{
  if(has_option(section, option)) {
    std::string str = get_string(section, option, "");
    if(str == "") {
      return default_val;
    }
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    if((str == "true") || (str == "on") || (str == "1"))
      return true;
    else if((str == "false") || (str == "off") || (str == "0"))
      return false;
    else
      return default_val;
  }
  else {
    return default_val;
  }
}

long
gr_prefs::get_long(const std::string &section, const std::string &option, long default_val)
{
  if(has_option(section, option)) {
    std::string str = get_string(section, option, "");
    if(str == "") {
      return default_val;
    }
    std::stringstream sstr(str);
    long n;
    sstr >> n;
    return n;
  }
  else {
    return default_val;
  }
}

double
gr_prefs::get_double(const std::string &section, const std::string &option, double default_val)
{
  if(has_option(section, option)) {
    std::string str = get_string(section, option, "");
    if(str == "") {
      return default_val;
    }
    std::stringstream sstr(str);
    double n;
    sstr >> n;
    return n;
  }
  else {
    return default_val;
  }
}


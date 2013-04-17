/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
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

#include <gnuradio/prefs.h>
#include <gnuradio/sys_paths.h>
#include <gnuradio/constants.h>
#include <algorithm>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
namespace fs = boost::filesystem;

namespace gr {

  /*
   * Stub implementations
   */
  static prefs s_default_singleton;
  static prefs *s_singleton = &s_default_singleton;

  prefs *
  prefs::singleton()
  {
    return s_singleton;
  }

  void
  prefs::set_singleton(prefs *p)
  {
    s_singleton = p;
  }

  prefs::prefs()
  {
    _read_files();
  }

  prefs::~prefs()
  {
    // nop
  }

  std::vector<std::string>
  prefs::_sys_prefs_filenames()
  {
    std::vector<std::string> fnames;

    fs::path dir = prefsdir();
    if(!fs::is_directory(dir))
      return fnames;

    fs::directory_iterator diritr(dir);
    while(diritr != fs::directory_iterator()) {
      fs::path p = *diritr++;
      if(p.extension() != ".swp")
        fnames.push_back(p.string());
    }
    std::sort(fnames.begin(), fnames.end());

    // Find if there is a ~/.gnuradio/config.conf file and add this to
    // the end of the file list to override any preferences in the
    // installed path config files.
    fs::path homedir = fs::path(gr::appdata_path());
    homedir = homedir/".gnuradio/config.conf";
    if(fs::exists(homedir)) {
      fnames.push_back(homedir.string());
    }

    return fnames;
  }

  void
  prefs::_read_files()
  {
    std::string config;

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
          config += t.substr(0, hash) + '#';
        }
      }
      fin.close();
    }

    // Remove all whitespace.
    config.erase(std::remove_if(config.begin(), config.end(),
                                ::isspace), config.end());

    // Convert the string into a map
    _convert_to_map(config);
  }

  void
  prefs::_convert_to_map(const std::string &conf)
  {
    // Convert the string into an map of maps
    // Map is structured as {section name: map of options}
    // And options map is simply: {option name: option value}
    std::string sub = conf;
    size_t sec_start = sub.find("[");
    while(sec_start != std::string::npos) {
      sub = sub.substr(sec_start);
    
      size_t sec_end = sub.find("]");
      if(sec_end == std::string::npos)
        throw std::runtime_error("Config file error: Mismatched section label.\n");
   
      std::string sec = sub.substr(1, sec_end-1);
      size_t next_sec_start = sub.find("[", sec_end);
      std::string subsec = sub.substr(sec_end+1, next_sec_start-sec_end-2);

      std::transform(sec.begin(), sec.end(), sec.begin(), ::tolower);

      std::map<std::string, std::string> options_map = d_config_map[sec];
      size_t next_opt = 0;
      size_t next_val = 0;
      next_opt = subsec.find("#");
      while(next_opt < subsec.size()-1) {
        next_val = subsec.find("=", next_opt);
        std::string option = subsec.substr(next_opt+1, next_val-next_opt-1);
      
        next_opt = subsec.find("#", next_val);
        std::string value = subsec.substr(next_val+1, next_opt-next_val-1);

        std::transform(option.begin(), option.end(), option.begin(), ::tolower);
        options_map[option] = value;
      }

      d_config_map[sec] = options_map;

      sec_start = sub.find("[", sec_end);
    }
  }

  std::string
  prefs::to_string()
  {
    config_map_itr sections;
    config_map_elem_itr options;
    std::stringstream s;

    for(sections = d_config_map.begin(); sections != d_config_map.end(); sections++) {
      s << "[" << sections->first << "]" << std::endl;
      for(options = sections->second.begin(); options != sections->second.end(); options++) {
        s << options->first << " = " << options->second << std::endl;
      }
      s << std::endl;
    }

    return s.str();
  }

  void
  prefs::save()
  {
    std::string conf = to_string();

    fs::path homedir = fs::path(gr::appdata_path());
    homedir = homedir/".gnuradio/config.conf";
    fs::ofstream fout(homedir);
    fout << conf;
    fout.close();
  }

  char *
  prefs::option_to_env(std::string section, std::string option)
  {
    std::stringstream envname;
    std::string secname=section, optname=option;

    std::transform(section.begin(), section.end(), secname.begin(), ::toupper);
    std::transform(option.begin(), option.end(), optname.begin(), ::toupper);
    envname << "GR_CONF_" << secname << "_" << optname;

    return getenv(envname.str().c_str());
  }

  bool
  prefs::has_section(const std::string &section)
  {
    std::string s = section;
    std::transform(section.begin(), section.end(), s.begin(), ::tolower);
    return d_config_map.count(s) > 0;
  }

  bool
  prefs::has_option(const std::string &section, const std::string &option)
  {
    if(option_to_env(section, option))
      return true;

    if(has_section(section)) {
      std::string s = section;
      std::transform(section.begin(), section.end(), s.begin(), ::tolower);

      std::string o = option;
      std::transform(option.begin(), option.end(), o.begin(), ::tolower);

      config_map_itr sec = d_config_map.find(s);
      return sec->second.count(o) > 0;
    }
    else {
      return false;
    }
  }

  const std::string
  prefs::get_string(const std::string &section, const std::string &option,
                    const std::string &default_val)
  {
    char *env = option_to_env(section, option);
    if(env)
      return std::string(env);

    if(has_option(section, option)) {
      std::string s = section;
      std::transform(section.begin(), section.end(), s.begin(), ::tolower);

      std::string o = option;
      std::transform(option.begin(), option.end(), o.begin(), ::tolower);

      config_map_itr sec = d_config_map.find(s);
      config_map_elem_itr opt = sec->second.find(o);
      return opt->second;
    }
    else {
      return default_val;
    }
  }

  void
  prefs::set_string(const std::string &section, const std::string &option,
                    const std::string &val)
  {
    std::string s = section;
    std::transform(section.begin(), section.end(), s.begin(), ::tolower);

    std::string o = option;
    std::transform(option.begin(), option.end(), o.begin(), ::tolower);

    std::map<std::string, std::string> opt_map = d_config_map[s];

    opt_map[o] = val;

    d_config_map[s] = opt_map;
  }

  bool
  prefs::get_bool(const std::string &section, const std::string &option,
                  bool default_val)
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

  void
  prefs::set_bool(const std::string &section, const std::string &option,
                  bool val)
  {
    std::string s = section;
    std::transform(section.begin(), section.end(), s.begin(), ::tolower);

    std::string o = option;
    std::transform(option.begin(), option.end(), o.begin(), ::tolower);

    std::map<std::string, std::string> opt_map = d_config_map[s];

    std::stringstream sstr;
    sstr << (val == true);
    opt_map[o] = sstr.str();

    d_config_map[s] = opt_map;
  }

  long
  prefs::get_long(const std::string &section, const std::string &option,
                  long default_val)
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

  void
  prefs::set_long(const std::string &section, const std::string &option,
                  long val)
  {
    std::string s = section;
    std::transform(section.begin(), section.end(), s.begin(), ::tolower);

    std::string o = option;
    std::transform(option.begin(), option.end(), o.begin(), ::tolower);

    std::map<std::string, std::string> opt_map = d_config_map[s];

    std::stringstream sstr;
    sstr << val;
    opt_map[o] = sstr.str();

    d_config_map[s] = opt_map;
  }

  double
  prefs::get_double(const std::string &section, const std::string &option,
                    double default_val)
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

  void
  prefs::set_double(const std::string &section, const std::string &option,
                    double val)
  {
    std::string s = section;
    std::transform(section.begin(), section.end(), s.begin(), ::tolower);

    std::string o = option;
    std::transform(option.begin(), option.end(), o.begin(), ::tolower);

    std::map<std::string, std::string> opt_map = d_config_map[s];

    std::stringstream sstr;
    sstr << val;
    opt_map[o] = sstr.str();

    d_config_map[s] = opt_map;
  }

} /* namespace gr */

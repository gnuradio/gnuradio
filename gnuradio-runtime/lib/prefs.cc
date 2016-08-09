/* -*- c++ -*- */
/*
 * Copyright 2006,2013,2015 Free Software Foundation, Inc.
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
#include <fstream>
#include <iostream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
namespace fs = boost::filesystem;
namespace po = boost::program_options;
typedef std::ifstream::char_type char_t;

namespace gr {

  prefs *
  prefs::singleton()
  {
    static prefs instance; // Guaranteed to be destroyed.
                            // Instantiated on first use.
    return &instance;
  }

  prefs::prefs()
  {
    _read_files(_sys_prefs_filenames());
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
      if(p.extension() == ".conf")
        fnames.push_back(p.string());
    }
    std::sort(fnames.begin(), fnames.end());

    // Find if there is a ~/.gnuradio/config.conf file and add this to
    // the end of the file list to override any preferences in the
    // installed path config files.
    fs::path userconf = fs::path(gr::userconf_path()) / "config.conf";
    if(fs::exists(userconf)) {
      fnames.push_back(userconf.string());
    }

    return fnames;
  }

  void
  prefs::_read_files(const std::vector<std::string> &filenames)
  {
    BOOST_FOREACH( std::string fname, filenames) {
      std::ifstream infile(fname.c_str());
      if(infile.good()) {
        try {
          po::basic_parsed_options<char_t> parsed = po::parse_config_file(infile, po::options_description(), true);
          BOOST_FOREACH(po::basic_option<char_t> o, (parsed.options) ){
            std::string okey = o.string_key;
            size_t pos = okey.find(".");
            std::string section, key;
            if(pos != std::string::npos) {
              section = okey.substr(0,pos);
              key = okey.substr(pos+1);
            } else {
              section = "default";
              key = okey;
            }
            std::transform(section.begin(), section.end(), section.begin(), ::tolower);
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            // value of a basic_option is always a std::vector<string>; we only allow single values, so:
            std::string value = o.value[0];
            d_config_map[section][key] = value;
          }
        } catch(std::exception e) {
          std::cerr << "WARNING: Config file '" << fname << "' failed to parse:" << std::endl;
          std::cerr << e.what() << std::endl;
          std::cerr << "Skipping it" << std::endl;
        }
      } else { // infile.good();
        std::cerr << "WARNING: Config file '" << fname << "' could not be opened for reading." << std::endl;
      }
    }
  }

  void
  prefs::add_config_file(const std::string &configfile)
  {
    std::vector<std::string> filenames;
    filenames.push_back(configfile);

    _read_files(filenames);
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
    fs::path userconf = fs::path(gr::userconf_path()) / "config.conf";
    fs::ofstream fout(userconf);
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

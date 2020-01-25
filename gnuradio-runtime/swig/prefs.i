/* -*- c++ -*- */
/*
 * Copyright 2006,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

class gr::prefs
{
public:
  static gr::prefs *singleton();

  void add_config_file(const std::string &configfile);

  virtual ~prefs();

  std::string to_string();

  void save();

  virtual bool has_section(const std::string &section);
  virtual bool has_option(const std::string &section, const std::string &option);

  virtual const std::string get_string(const std::string &section,
				       const std::string &option,
				       const std::string &default_val);
  virtual void set_string(const std::string &section,
                          const std::string &option,
                          const std::string &val);
  virtual bool get_bool(const std::string &section,
			const std::string &option,
			bool default_val);
  virtual void set_bool(const std::string &section,
			const std::string &option,
			bool val);
  virtual long get_long(const std::string &section,
			const std::string &option,
			long default_val);
  virtual void set_long(const std::string &section,
			const std::string &option,
			long val);
  virtual double get_double(const std::string &section,
			    const std::string &option,
			    double default_val);
  virtual void set_double(const std::string &section,
                          const std::string &option,
                          double val);
};

/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*******************************************************************************
* Copyright 2011 Johns Hopkins University Applied Physics Lab
* Author: Mark Plett
* Description:
*   SWIG interface generator file for gr_logger module.  gr_logger wraps log4cpp logging
*   for gnuradio.
*******************************************************************************/

%feature("autodoc", "1");		// generate python docstrings

%include "exception.i"
%import "gnuradio.i"			// the common stuff

%{
#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include <stdexcept>
%}

//-----------------------------------

#ifdef ENABLE_GR_LOG
#ifdef HAVE_LOG4CPP

%{
// The .h files
#include <gr_logger.h>
#include <log4cpp/Category.hh>
%}

namespace log4cpp{
class LoggerPtr {
public:
 ~LoggerPtr();
};
};
void logger_load_config(const std::string &config_filename);
void logger_set_level(log4cpp::LoggerPtr logger, const std::string &level);

%rename(log) gr_logger;

class gr_logger
{
private:
  
public:
  //gr_logger(std::string config_filename);
  gr_logger(std::string config_filename,int watchPeriodSec);
  void set_level(std::string name,std::string level);
  void get_level(std::string name,std::string &level);
  void add_console_appender(std::string name,std::string target,std::string pattern);
  void add_file_appender(std::string name,std::string filename,bool append,std::string patter);
  void add_rollingfile_appender(std::string name,std::string filename,size_t filesize,int bkup_indx,bool append,mode_t mode,std::string pattern);

  void notice(std::string name,std::string msg);
  void debug(std::string name,std::string msg);
  void info(std::string name,std::string msg);
  void warn(std::string name,std::string msg);
  void error(std::string name,std::string msg);
  void emerg(std::string name,std::string msg);
  void crit(std::string name,std::string msg);
  void errorIF(std::string name,bool cond,std::string msg);
  void gr_assert(std::string name,bool cond,std::string msg);

  static gr_logger_ptr getLogger(std::string name);

  void log_set_level(gr_logger_ptr logger,std::string level);
  void log_get_level(gr_logger_ptr logger,std::string &level);
  void log_add_console_appender(gr_logger_ptr logger,std::string target,std::string pattern);
  void log_add_file_appender(gr_logger_ptr logger,std::string filename,bool append,std::string pattern);
  void log_add_rollingfile_appender(gr_logger_ptr logger,std::string filename,size_t filesize,int bkup_index,bool append,mode_t mode,std::string pattern);

  void log_notice(gr_logger_ptr logger,std::string msg);
  void log_debug(gr_logger_ptr logger,std::string msg);
  void log_info(gr_logger_ptr logger,std::string msg);
  void log_warn(gr_logger_ptr logger,std::string msg);
  void log_error(gr_logger_ptr logger,std::string msg);
  void log_crit(gr_logger_ptr logger,std::string msg);
  void log_emerg(gr_logger_ptr logger,std::string msg);
  void log_errorIF(gr_logger_ptr logger,bool cond,std::string msg);
  void log_assert(gr_logger_ptr logger,bool cond,std::string msg);

  void get_logger_names(std::vector<std::string>& names);
  void reset_configuration();

};

#endif /* HAVE_LOG4CPP */
#endif /* ENABLE_GR_LOG */

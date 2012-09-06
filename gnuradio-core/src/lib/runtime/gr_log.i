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
*   SWIG interface generator file for gr_log module.  gr_log wraps log4cxx logging
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

%{
// The .h files
#include <gr_log.h>
#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
%}

namespace log4cxx{
class LoggerPtr {
public:
 ~LoggerPtr();
};
};
void logger_load_config(const std::string &config_filename);
void logger_set_level(log4cxx::LoggerPtr logger, const std::string &level);

%rename(log) gr_log;

class gr_log
{
private:
  
public:
  //gr_log(std::string config_filename);
  gr_log(std::string config_filename,int watchPeriodSec);
  void set_level(std::string name,std::string level);
  void get_level(std::string name,std::string &level);
  void trace(std::string name,std::string msg);
  void debug(std::string name,std::string msg);
  void info(std::string name,std::string msg);
  void warn(std::string name,std::string msg);
  void error(std::string name,std::string msg);
  void fatal(std::string name,std::string msg);
  void errorIF(std::string name,bool cond,std::string msg);
  void gr_assert(std::string name,bool cond,std::string msg);

  static log4cxx::LoggerPtr getLogger(std::string name);

  void set_log_level(log4cxx::LoggerPtr logger,std::string level);
  void get_log_level(log4cxx::LoggerPtr logger,std::string &level);
  void log_trace(log4cxx::LoggerPtr logger,std::string msg);
  void log_debug(log4cxx::LoggerPtr logger,std::string msg);
  void log_info(log4cxx::LoggerPtr logger,std::string msg);
  void log_warn(log4cxx::LoggerPtr logger,std::string msg);
  void log_error(log4cxx::LoggerPtr logger,std::string msg);
  void log_fatal(log4cxx::LoggerPtr logger,std::string msg);
  void log_errorIF(log4cxx::LoggerPtr logger,bool cond,std::string msg);
  void log_assert(log4cxx::LoggerPtr logger,bool cond,std::string msg);
};

#endif /* ENABLE_GR_LOG */

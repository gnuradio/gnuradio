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
*
* Copyright 2011 JHU APL
*
* SWIG interface generator file for gr_log module.  gr_log wraps log4cxx logging
* for gnuradio.
*
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

class LoggerPtr {
public:
 ~LoggerPtr();
};

void logger_load_config(const std::string &config_filename);
void logger_set_level(LoggerPtr logger, const std::string &level);

class gr_log
{
private:
  
public:
//   gr_log(std::string config_filename);
  gr_log(std::string config_filename,int watchPeriodSec);
  void set_level(std::string name,std::string level);
  void trace(std::string name,std::string msg);
  void debug(std::string name,std::string msg);
  void info(std::string name,std::string msg);
  void warn(std::string name,std::string msg);
  void error(std::string name,std::string msg);
  void fatal(std::string name,std::string msg);
  void errorIF(std::string name,bool cond,std::string msg);
  void gr_assert(std::string name,bool cond,std::string msg);

  static LoggerPtr getLogger(std::string name);

  void set_log_level(LoggerPtr logger,std::string level);
  void log_trace(LoggerPtr logger,std::string msg);
  void log_debug(LoggerPtr logger,std::string msg);
  void log_info(LoggerPtr logger,std::string msg);
  void log_warn(LoggerPtr logger,std::string msg);
  void log_error(LoggerPtr logger,std::string msg);
  void log_fatal(LoggerPtr logger,std::string msg);
  void log_errorIF(LoggerPtr logger,bool cond,std::string msg);
  void log_assert(LoggerPtr logger,bool cond,std::string msg);
};

#endif /* ENABLE_GR_LOG */

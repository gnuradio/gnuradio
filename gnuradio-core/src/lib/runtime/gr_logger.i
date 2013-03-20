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

%{
// The .h files
#include <gr_logger.h>
%}

%rename(logger) gr_logger;
%rename(logger_config) gr_logger_config;
%rename(logger_get_names) gr_logger_get_logger_names;
%rename(logger_reset_config) gr_logger_reset_config;


void gr_logger_config(const std::string config_filename,unsigned int watch_period = 0);
std::vector<std::string> gr_logger_get_logger_names(void);
void gr_logger_reset_config(void);

class gr_logger
{
 public:
  gr_logger(std::string logger_name);
  void set_level(std::string level){GR_LOG_SET_LEVEL(d_logger,level);}
  void get_level(std::string &level){GR_LOG_GET_LEVEL(d_logger,level);}
  void debug(std::string msg){GR_LOG_DEBUG(d_logger,msg);};
  void info(std::string msg){GR_LOG_INFO(d_logger,msg);};
  void notice(std::string msg){GR_LOG_NOTICE(d_logger,msg);};
  void warn(std::string msg){GR_LOG_WARN(d_logger,msg);};
  void error(std::string msg){GR_LOG_ERROR(d_logger,msg);};
  void crit(std::string msg){GR_LOG_CRIT(d_logger,msg);};
  void alert(std::string msg){GR_LOG_ALERT(d_logger,msg);};
  void fatal(std::string msg){GR_LOG_FATAL(d_logger,msg);};
  void emerg(std::string msg){GR_LOG_EMERG(d_logger,msg);};
  void errorIF(bool cond,std::string msg){GR_LOG_ERRORIF(d_logger,cond,msg);};
  void log_assert(bool cond,std::string msg){GR_LOG_ASSERT(d_logger,cond,msg);};
  void add_console_appender(std::string target,std::string pattern);
  void add_file_appender(std::string filename,bool append,std::string pattern);
  void add_rollingfile_appender(std::string filename,size_t filesize,int bkup_index,bool append,mode_t mode,std::string pattern);
};



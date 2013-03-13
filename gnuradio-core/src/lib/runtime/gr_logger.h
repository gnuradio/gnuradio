/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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

/*******************************************************************************
* Author: Mark Plett
* Description:
*   The gr_logger module wraps the log4cpp library for logging in gnuradio
*******************************************************************************/

#ifndef INCLUDED_GR_LOGGER_H
#define INCLUDED_GR_LOGGER_H

/*!
* \file gr_logger.h
* \ingroup logging
* \brief GNURADIO logging wrapper for log4cpp library (C++ port of log4j)
*
*/

#ifndef ENABLE_GR_LOG
#include "config.h"
//#define GR_LOG_CONFIG_H
#endif

#include <gr_core_api.h>
#include <assert.h>
#include <iostream>

//#ifndef ENABLE_GR_LOG
//#define ENABLE_GR_LOG 1
//#endif
//#ifndef HAVE_LOG4CPP
//#define HAVE_LOG4CPP 2
//#endif

#ifdef ENABLE_GR_LOG

// We have three configurations... first logging to stdout/stderr
#ifndef HAVE_LOG4CPP
//#warning GR logging Enabled and using std::cout

typedef std::string gr_logger_ptr;

#define GR_LOG_DECLARE_LOGPTR(logger)
#define GR_LOG_ASSIGN_LOGPTR(logger,name)
#define GR_CONFIG_LOGGER(config)
#define GR_CONFIG_AND_WATCH_LOGGER(config,period)
#define GR_LOG_GETLOGGER(logger, name) 
#define GR_SET_LEVEL(name, level)
#define GR_LOG_SET_LEVEL(logger, level) 
#define GR_GET_LEVEL(name, level)
#define GR_LOG_GET_LEVEL(logger, level)
#define GR_ADD_CONSOLE_APPENDER(logger,target,pattern)
#define GR_LOG_ADD_CONSOLE_APPENDER(logger,target,pattern)
#define GR_ADD_FILE_APPENDER(name,filename,append,pattern)
#define GR_LOG_ADD_FILE_APPENDER(logger,filename,append,pattern)
#define GR_ADD_ROLLINGFILE_APPENDER(name,filename,filesize,bkup_index,append,mode,pattern)
#define GR_LOG_ADD_ROLLINGFILE_APPENDER(logger,filename,filesize,bkup_index,append,mode,pattern)
#define GR_GET_LOGGER_NAMES(names)
#define GR_RESET_CONFIGURATION()
#define GR_DEBUG(name, msg) std::cout<<"DEBUG: "<<msg<<std::endl
#define GR_INFO(name, msg) std::cout<<"INFO: "<<msg<<std::endl
#define GR_NOTICE(name, msg) std::cout<<"NOTICE: "<<msg<<std::endl
#define GR_WARN(name, msg) std::cerr<<"WARN: "<<msg<<std::endl
#define GR_ERROR(name, msg) std::cerr<<"ERROR: "<<msg<<std::endl
#define GR_ALERT(name, msg) std::cerr<<"ERROR: "<<msg<<std::endl
#define GR_CRIT(name, msg) std::cerr<<"ERROR: "<<msg<<std::endl
#define GR_FATAL(name, msg) std::cerr<<"FATAL: "<<msg<<std::endl
#define GR_EMERG(name, msg) std::cerr<<"EMERG: "<<msg<<std::endl
#define GR_ERRORIF(name, cond, msg) if((cond)) std::cerr<<"ERROR: "<<msg<<std::endl
#define GR_ASSERT(name, cond, msg) if(!(cond)) std::cerr<<"ERROR: "<<msg<<std::endl; assert(cond)
#define GR_LOG_DEBUG(logger, msg) std::cout<<"DEBUG: "<<msg<<std::endl
#define GR_LOG_INFO(logger, msg) std::cout<<"INFO: "<<msg<<std::endl
#define GR_LOG_NOTICE(logger, msg) std::cout<<"NOTICE: "<<msg<<std::endl
#define GR_LOG_WARN(logger, msg) std::cerr<<"WARN: "<<msg<<std::endl
#define GR_LOG_ERROR(logger, msg) std::cerr<<"ERROR: "<<msg<<std::endl
#define GR_LOG_ALERT(logger, msg) std::cerr<<"ALERT: "<<msg<<std::endl
#define GR_LOG_CRIT(logger, msg) std::cerr<<"CRIT: "<<msg<<std::endl
#define GR_LOG_FATAL(logger, msg) std::cerr<<"FATAL: "<<msg<<std::endl
#define GR_LOG_EMERG(logger, msg) std::cerr<<"EMERG: "<<msg<<std::endl
#define GR_LOG_ERRORIF(logger, cond, msg) if((cond)) std::cerr<<"ERROR: "<<msg<<std::endl
#define GR_LOG_ASSERT(logger, cond, msg) std::cerr<<"ERROR: "<<msg<<std::endl; assert(cond)

#else /* HAVE_LOG4CPP */
// Second configuration...logging to log4cpp

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>

/*!
 * \brief GR_LOG macros
 * \ingroup logging
 *
 * These macros wrap the standard LOG4CPP_LEVEL macros.  The availablie macros
 * are:
 *  GR_LOG_DEBUG
 *  GR_LOG_INFO
 *  GR_LOG_WARN
 *  GR_LOG_TRACE
 *  GR_LOG_ERROR
 *  GR_LOG_ALERT
 *  GR_LOG_CRIT
 *  GR_LOG_FATAL
 *  GR_LOG_EMERG
 */
typedef log4cpp::Category* gr_logger_ptr;

/* Macros for Programmatic Configuration */
#define GR_LOG_DECLARE_LOGPTR(logger) \
  gr_logger_ptr logger;

#define GR_LOG_ASSIGN_LOGPTR(logger,name) \
  logger = gr_logger::getLogger(name);

#define GR_CONFIG_LOGGER(config)	\
  logger_load_config(config)

#define GR_CONFIG_AND_WATCH_LOGGER(config,period)	\
  logger_load_config_and_watch(config,period)

#define GR_LOG_GETLOGGER(logger, name) \
  gr_logger_ptr logger = gr_logger::getLogger(name);

#define GR_SET_LEVEL(name, level){ \
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  logger_set_level(logger,level);}

#define GR_LOG_SET_LEVEL(logger, level) \
  logger_set_level(logger, level);

#define GR_GET_LEVEL(name, level){ \
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  logger_get_level(logger,level);}

#define GR_LOG_GET_LEVEL(logger, level) \
  logger_get_level(logger,level);

#define GR_ADD_CONSOLE_APPENDER(name,target,pattern){\
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  logger_add_console_appender(logger,target,pattern);}

#define GR_LOG_ADD_CONSOLE_APPENDER(logger,target,pattern){\
  logger_add_console_appender(logger,target,pattern);}

#define GR_ADD_FILE_APPENDER(name,filename,append,pattern){\
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  logger_add_file_appender(logger,filename,append,pattern);}

#define GR_LOG_ADD_FILE_APPENDER(logger,filename,append,pattern){\
  logger_add_file_appender(logger,filename,append,pattern);}

#define GR_ADD_ROLLINGFILE_APPENDER(name,filename,filesize,bkup_index,append,mode,pattern){\
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  logger_add_rollingfile_appender(logger,filename,filesize,bkup_index,append,mode,pattern);}

#define GR_LOG_ADD_ROLLINGFILE_APPENDER(logger,filename,filesize,bkup_index,append,mode,pattern){\
  logger_add_rollingfile_appender(logger,filename,filesize,bkup_index,append,mode,pattern);}

#define GR_GET_LOGGER_NAMES(names){ \
  logger_get_logger_names(names);}

#define GR_RESET_CONFIGURATION(){ \
  logger_reset_config();}

/* Logger name referenced macros */
#define GR_DEBUG(name, msg) { \
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  *logger<< log4cpp::Priority::DEBUG << msg << log4cpp::eol;}

#define GR_INFO(name, msg) { \
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  *logger<< log4cpp::Priority::INFO << msg << log4cpp::eol;}

#define GR_NOTICE(name, msg) { \
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  *logger << log4cpp::Priority::NOTICE << msg;}

#define GR_WARN(name, msg) { \
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  *logger<< log4cpp::Priority::WARN << msg << log4cpp::eol;}

#define GR_ERROR(name, msg) { \
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  *logger<< log4cpp::Priority::ERROR << msg << log4cpp::eol;}

#define GR_CRIT(name, msg) { \
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  *logger<< log4cpp::Priority::CRIT << msg << log4cpp::eol;}

#define GR_ALERT(name, msg) { \
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  *logger<< log4cpp::Priority::ALERT << msg << log4cpp::eol;}

#define GR_FATAL(name, msg) { \
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  *logger<< log4cpp::Priority::FATAL << msg << log4cpp::eol;}

#define GR_EMERG(name, msg) { \
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  *logger<< log4cpp::Priority::EMERG << msg << log4cpp::eol;}

#define GR_ERRORIF(name, cond, msg) { \
if((cond)){\
  gr_logger_ptr logger = gr_logger::getLogger(name);\
  *logger<< log4cpp::Priority::ERROR << msg << log4cpp::eol;};\
};

#define GR_ASSERT(name, cond, msg) { \
if((!cond)){\
    gr_logger_ptr logger = gr_logger::getLogger(name);\
    *logger<< log4cpp::Priority::EMERG << msg << log4cpp::eol;};\
    assert(0);\
};

/* LoggerPtr Referenced Macros */
#define GR_LOG_DEBUG(logger, msg) { \
  *logger << log4cpp::Priority::DEBUG << msg << log4cpp::eol;}

#define GR_LOG_INFO(logger, msg) { \
  *logger << log4cpp::Priority::INFO << msg << log4cpp::eol;}

#define GR_LOG_NOTICE(logger, msg) { \
  *logger << log4cpp::Priority::NOTICE << msg << log4cpp::eol;}

#define GR_LOG_WARN(logger, msg) { \
  *logger << log4cpp::Priority::WARN << msg << log4cpp::eol;}

#define GR_LOG_ERROR(logger, msg) { \
  *logger << log4cpp::Priority::ERROR << msg << log4cpp::eol;}

#define GR_LOG_CRIT(logger, msg) { \
  *logger << log4cpp::Priority::CRIT << msg << log4cpp::eol;}

#define GR_LOG_ALERT(logger, msg) { \
  *logger << log4cpp::Priority::ALERT << msg << log4cpp::eol;}

#define GR_LOG_FATAL(logger, msg) { \
  *logger << log4cpp::Priority::FATAL << msg << log4cpp::eol;}

#define GR_LOG_EMERG(logger, msg) { \
  *logger << log4cpp::Priority::EMERG << msg << log4cpp::eol;}

#define GR_LOG_ERRORIF(logger,cond, msg) { \
if((!cond)){\
    *logger<< log4cpp::Priority::ERROR << msg << log4cpp::eol;};\
    assert(0);\
};

#define GR_LOG_ASSERT(logger, cond, msg) { \
if((!cond)){\
    *logger<< log4cpp::Priority::EMERG << msg << log4cpp::eol;};\
    assert(0);\
};

/*!
 * \brief Load logger's configuration file.
 *
 * Initialize the GNU Radio logger by loading the configuration file
 * \p config_filename.
 *
 * \param config_filename The configuration file. Set to "" for the
 *        basic logger that outputs to the console.
 */
GR_CORE_API void logger_load_config(const std::string &config_filename="");


GR_CORE_API void logger_load_config_and_watch(const std::string &config_filename,
                                              unsigned int watch_period);

GR_CORE_API void logger_reset_config(void);

/*!
 * \brief Set the logger's output level.
 *
 * Sets the level of the logger. This takes a string that is
 * translated to the standard levels and can be (case insensitive):
 *
 * \li off , notset
 * \li debug
 * \li info
 * \li notice
 * \li warn
 * \li error
 * \li crit
 * \li alert
 * \li fatal
 * \li emerg
 *
 * \param logger the logger to set the level of.
 * \param level  string to set the level to.
 */
GR_CORE_API void logger_set_level(gr_logger_ptr logger, const std::string &level);

/*!
 * \brief Set the logger's output level.
 *
 * Sets the level of the logger. This takes the actual Log4cpp::Priority
 * data type, which can be:
 *
 * \li log4cpp::Priority::NOTSET
 * \li log4cpp::Priority::DEBUG
 * \li log4cpp::Priority::INFO
 * \li log4cpp::Priority::NOTICE
 * \li log4cpp::Priority::WARN
 * \li log4cpp::Priority::ERROR
 * \li log4cpp::Priority::CRIT
 * \li log4cpp::Priority::ALERT
 * \li log4cpp::Priority::FATAL
 * \li log4cpp::Priority::EMERG
 *
 * \param logger the logger to set the level of.
 * \param level  new logger level of type Log4cpp::Priority
 */
void logger_set_level(gr_logger_ptr logger, log4cpp::Priority::Value level);


/*!
 * \brief Get the logger's output level.
 *
 * Gets the level of the logger. This returns a string that
 * corresponds to the standard levels and can be (case insensitive):
 *
 * \li notset
 * \li debug
 * \li info
 * \li notice
 * \li warn
 * \li error
 * \li crit
 * \li alert
 * \li fatal
 * \li emerg
 *
 * \param logger the logger to get the level of.
 * \param level  string to get the level into.
 */
GR_CORE_API void logger_get_level(gr_logger_ptr logger, std::string &level);

/*!
 * \brief Get the logger's output level.
 *
 * Gets the level of the logger. This returns the actual Log4cpp::Level
 * data type, which can be:
 *
 * \li log4cpp::Priority::NOTSET
 * \li log4cpp::Priority::DEBUG
 * \li log4cpp::Priority::INFO
 * \li log4cpp::Priority::NOTICE
 * \li log4cpp::Priority::WARN
 * \li log4cpp::Priority::ERROR
 * \li log4cpp::Priority::CRIT
 * \li log4cpp::Priority::ALERT
 * \li log4cpp::Priority::FATAL
 * \li log4cpp::Priority::EMERG
 *
 * \param logger the logger to get the level of.
 * \param level of the logger.
 */
GR_CORE_API void logger_get_level(gr_logger_ptr logger, log4cpp::Priority::Value &level);

GR_CORE_API void logger_add_console_appender(gr_logger_ptr logger,std::string target,std::string pattern);

GR_CORE_API void logger_add_file_appender(gr_logger_ptr logger,std::string filename,bool append,std::string pattern);

GR_CORE_API void logger_add_rollingfile_appender(gr_logger_ptr logger,std::string filename,
                    size_t filesize,int bkup_index,bool append,mode_t mode,std::string pattern);

GR_CORE_API void logger_get_logger_names(std::vector<std::string>& names);

/*!
 * \brief instantiate (configure) logger.
 * \ingroup logging
 *
 */
class gr_logger
{
 public:
  /*!
   * \brief contructor take log configuration file and configures loggers.
   */
  gr_logger(std::string config_filename)
  {
    // Load configuration file
    logger_load_config(config_filename);
  };

  /*!
   * \brief contructor take log configuration file and watchtime and configures
   */
  gr_logger(std::string config_filename, int watchPeriodSec)
  {
    // Load configuration file
    logger_load_config_and_watch(config_filename,watchPeriodSec);

  };

  static gr_logger_ptr getLogger(std::string name)
  {
      if(log4cpp::Category::exists(name)){
        gr_logger_ptr logger = &log4cpp::Category::getInstance(name);
        return logger;
      }
      else
      {
        gr_logger_ptr logger = &log4cpp::Category::getInstance(name);
        logger->setPriority(log4cpp::Priority::NOTSET);
        return logger;
      };
  };

  // Wrappers for logging macros
  /*! \brief inline function, wrapper to set the logger level */
  void set_level(std::string name,std::string level){GR_SET_LEVEL(name,level);}
  
  /*! \brief inline function, wrapper to get the logger level */
  void get_level(std::string name,std::string &level){GR_GET_LEVEL(name,level);}

  /*! \brief inline function, wrapper for DEBUG message */
  void debug(std::string name,std::string msg){GR_DEBUG(name,msg);};

  /*! \brief inline function, wrapper for INFO message */
  void info(std::string name,std::string msg){GR_INFO(name,msg);};

  /*! \brief inline function, wrapper for NOTICE message */
  void notice(std::string name,std::string msg){GR_NOTICE(name,msg);};

  /*! \brief inline function, wrapper for WARN message */
  void warn(std::string name,std::string msg){GR_WARN(name,msg);};

  /*! \brief inline function, wrapper for ERROR message */
  void error(std::string name,std::string msg){GR_ERROR(name,msg);};

  /*! \brief inline function, wrapper for CRIT message */
  void crit(std::string name,std::string msg){GR_CRIT(name,msg);};

  /*! \brief inline function, wrapper for ALERT message */
  void alert(std::string name,std::string msg){GR_ALERT(name,msg);};

  /*! \brief inline function, wrapper for FATAL message */
  void fatal(std::string name,std::string msg){GR_FATAL(name,msg);};

  /*! \brief inline function, wrapper for EMERG message */
  void emerg(std::string name,std::string msg){GR_EMERG(name,msg);};

  /*! \brief inline function, wrapper for LOG4CPP_ASSERT for conditional ERROR message */
  void errorIF(std::string name,bool cond,std::string msg){GR_ERRORIF(name,cond,msg);};

  /*! \brief inline function, wrapper for LOG4CPP_ASSERT for conditional ERROR message */
  void gr_assert(std::string name,bool cond,std::string msg){GR_ASSERT(name,cond,msg);};

  // Wrappers for Logger Pointer referenced functions
  /*! \brief inline function, wrapper to set the logger level */
  void set_log_level(gr_logger_ptr logger,std::string level){GR_LOG_SET_LEVEL(logger,level);}

  /*! \brief inline function, wrapper to get the logger level */
  void get_log_level(gr_logger_ptr logger,std::string &level){GR_LOG_GET_LEVEL(logger,level);}

  /*! \brief inline function, wrapper for LOG4CPP_DEBUG for DEBUG message */
  void log_debug(gr_logger_ptr logger,std::string msg){GR_LOG_DEBUG(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CPP_INFO for INFO message */
  void log_info(gr_logger_ptr logger,std::string msg){GR_LOG_INFO(logger,msg);};

  /*! \brief inline function, wrapper for NOTICE message */
  void log_notice(gr_logger_ptr logger,std::string msg){GR_LOG_NOTICE(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CPP_WARN for WARN message */
  void log_warn(gr_logger_ptr logger,std::string msg){GR_LOG_WARN(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CPP_ERROR for ERROR message */
  void log_error(gr_logger_ptr logger,std::string msg){GR_LOG_ERROR(logger,msg);};

  /*! \brief inline function, wrapper for NOTICE message */
  void log_crit(gr_logger_ptr logger,std::string msg){GR_LOG_CRIT(logger,msg);};

  /*! \brief inline function, wrapper for ALERT message */
  void log_alert(gr_logger_ptr logger,std::string msg){GR_LOG_ALERT(logger,msg);};

  /*! \brief inline function, wrapper for FATAL message */
  void log_fatal(gr_logger_ptr logger,std::string msg){GR_LOG_FATAL(logger,msg);};

  /*! \brief inline function, wrapper for EMERG message */
  void log_emerg(gr_logger_ptr logger,std::string msg){GR_LOG_EMERG(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CPP_ASSERT for conditional ERROR message */
  void log_errorIF(gr_logger_ptr logger,bool cond,std::string msg){GR_LOG_ERRORIF(logger,cond,msg);};

  /*! \brief inline function, wrapper for LOG4CPP_ASSERT for conditional ERROR message */
  void log_assert(gr_logger_ptr logger,bool cond,std::string msg){GR_LOG_ASSERT(logger,cond,msg);};
};


#endif /* HAVE_LOG4CPP */

// If Logger disable do nothing
#else /* ENABLE_GR_LOG */

typedef void* gr_logger_ptr;

#define GR_LOG_DECLARE_LOGPTR(logger)
#define GR_LOG_ASSIGN_LOGPTR(logger,name)
#define GR_CONFIG_LOGGER(config)
#define GR_CONFIG_AND_WATCH_LOGGER(config,period)
#define GR_LOG_GETLOGGER(logger, name) 
#define GR_SET_LEVEL(name, level)
#define GR_LOG_SET_LEVEL(logger, level) 
#define GR_GET_LEVEL(name, level)
#define GR_LOG_GET_LEVEL(logger, level)
#define GR_ADD_CONSOLE_APPENDER(logger,target,pattern)
#define GR_LOG_ADD_CONSOLE_APPENDER(logger,,target,pattern)
#define GR_ADD_FILE_APPENDER(name,filename,append,pattern)
#define GR_LOG_ADD_FILE_APPENDER(logger,filename,append,pattern)
#define GR_ADD_ROLLINGFILE_APPENDER(name,filename,filesize,bkup_index,append,mode,pattern)
#define GR_LOG_ADD_ROLLINGFILE_APPENDER(logger,filename,filesize,bkup_index,append,mode,pattern)
#define GR_GET_LOGGER_NAMES(names)
#define GR_RESET_CONFIGURATION()
#define GR_DEBUG(name, msg)
#define GR_INFO(name, msg)
#define GR_NOTICE(name, msg)
#define GR_WARN(name, msg)
#define GR_ERROR(name, msg)
#define GR_ALERT(name, msg)
#define GR_CRIT(name, msg)
#define GR_FATAL(name, msg)
#define GR_EMERG(name, msg)
#define GR_ERRORIF(name, cond, msg)
#define GR_ASSERT(name, cond, msg)
#define GR_LOG_DEBUG(logger, msg)
#define GR_LOG_INFO(logger, msg)
#define GR_LOG_NOTICE(logger, msg)
#define GR_LOG_WARN(logger, msg)
#define GR_LOG_ERROR(logger, msg)
#define GR_LOG_ALERT(logger, msg)
#define GR_LOG_CRIT(logger, msg)
#define GR_LOG_FATAL(logger, msg)
#define GR_LOG_EMERG(logger, msg)
#define GR_LOG_ERRORIF(logger, cond, msg)
#define GR_LOG_ASSERT(logger, cond, msg)

#endif /* ENABLE_GR_LOG */
#endif /* INCLUDED_GR_LOGGER_H */

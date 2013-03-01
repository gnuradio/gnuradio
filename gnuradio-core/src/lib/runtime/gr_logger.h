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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*******************************************************************************
* Copyright 2011 Johns Hopkins University Applied Physics Lab
* Author: Mark Plett
* Description:
*   The gr_logger module wraps the log4cxx library for logging in gnuradio
*******************************************************************************/

#ifndef INCLUDED_GR_LOGGER_H
#define INCLUDED_GR_LOGGER_H

/*!
* \file gr_logger.h
* \ingroup logging
* \brief GNURADIO logging wrapper for log4cxx library (C++ port of log4j)
*
*/

#include <gr_core_api.h>
#include <assert.h>
#include <iostream>

#ifdef ENABLE_GR_LOG

// We have three configurations... first logging to stdout/stderr
#ifndef HAVE_LOG4CXX
//#warning GR logging Enabled and using std::cout

typedef std::string gr_logger_ptr;

#define GR_LOG_DECLARE_LOGPTR(logger)
#define GR_LOG_ASSIGN_LOGPTR(logger,name)
#define GR_CONFIG_LOGGER(config)
#define GR_CONFIG_AND_WATCH_LOGGER(config)
#define GR_LOG_GETLOGGER(logger, name) 
#define GR_SET_LEVEL(name, level)
#define GR_LOG_SET_LEVEL(logger, level) 
#define GR_GET_LEVEL(name, level)
#define GR_LOG_GET_LEVEL(logger, level)
#define GR_ADD_CONSOLE_APPENDER(logger,layout)
#define GR_LOG_ADD_CONSOLE_APPENDER(logger,layout)
#define GR_ADD_FILE_APPENDER(logger,layout,filename,append)
#define GR_LOG_ADD_FILE_APPENDER(logger,layout,filename,append)
#define GR_ADD_ROLLINGFILE_APPENDER(logger,layout,filename,append,bkup_index,filesize)
#define GR_LOG_ADD_ROLLINGFILE_APPENDER(logger,layout,filename,append,bkup_index,filesize)
#define GR_GET_LOGGER_NAMES(names)
#define GR_RESET_CONFIGURATION()
#define GR_TRACE(name, msg) std::cout<<"TRACE:"<<msg<<std::endl
#define GR_DEBUG(name, msg) std::cout<<"DEBUG:"<<msg<<std::endl
#define GR_INFO(name, msg) std::cout<<"INFO:"<<msg<<std::endl
#define GR_WARN(name, msg) std::cerr<<"WARN:"<<msg<<std::endl
#define GR_ERROR(name, msg) std::cerr<<"ERROR:"<<msg<<std::endl
#define GR_FATAL(name, msg) std::cerr<<"FATAL:"<<msg<<std::endl
#define GR_ERRORIF(name, cond, msg) if((cond)) std::cerr<<"ERROR:"<<msg<<std::endl
#define GR_ASSERT(name, cond, msg) std::cerr<<"ERROR:"<<msg<<std::endl; assert(cond)
#define GR_LOG_SET_LEVEL(logger, level)
#define GR_LOG_GET_LEVEL(logger, level)
#define GR_LOG_TRACE(logger, msg) std::cout<<"TRACE:"<<msg<<std::endl
#define GR_LOG_DEBUG(logger, msg) std::cout<<"DEBUG:"<<msg<<std::endl
#define GR_LOG_INFO(logger, msg) std::cout<<"INFO:"<<msg<<std::endl
#define GR_LOG_WARN(logger, msg) std::cerr<<"WARN:"<<msg<<std::endl
#define GR_LOG_ERROR(logger, msg) std::cerr<<"ERROR:"<<msg<<std::endl
#define GR_LOG_FATAL(logger, msg) std::cerr<<"FATAL:"<<msg<<std::endl
#define GR_LOG_ERRORIF(logger, cond, msg) if((cond)) std::cerr<<"ERROR:"<<msg<<std::endl
#define GR_LOG_ASSERT(logger, cond, msg) std::cerr<<"ERROR:"<<msg<<std::endl; assert(cond)

#else /* HAVE_LOG4CXX */
// Second configuration...logging to log4cxx
//#warning GR logging Enabled and using LOG4CXX

#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>

/*!
 * \brief GR_LOG macros
 * \ingroup logging
 *
 * These macros wrap the standard LOG4CXX_LEVEL macros.  The availablie macros
 * are:
 *  GR_LOG_TRACE
 *  GR_LOG_DEBUG
 *  GR_LOG_INFO
 *  GR_LOG_WARN
 *  GR_LOG_ERROR
 *  GR_LOG_FATAL
 */

typedef log4cxx::LoggerPtr gr_logger_ptr;

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
  log4cxx::LoggerPtr logger = gr_logger::getLogger(name);

#define GR_SET_LEVEL(name, level){ \
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name); \
  logger_set_level(logger, level);}

#define GR_LOG_SET_LEVEL(logger, level) \
  logger_set_level(logger, level);

#define GR_GET_LEVEL(name, level){ \
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name); \
  logger_get_level(logger,level);}

#define GR_LOG_GET_LEVEL(logger, level) \
  logger_get_level(logger,level);

#define GR_ADD_CONSOLE_APPENDER(name,layout,terget){d      \
  gr_logger_ptr logger = log4cxx::Logger::getLogger(name); \
  logger_add_console_appender(logger,layout, target);}

#define GR_LOG_ADD_CONSOLE_APPENDER(logger,layout,target){      \
  logger_add_console_appender(logger,layout,target);}

#define GR_ADD_FILE_APPENDER(name,layout,filename,append){\
  gr_logger_ptr logger = log4cxx::Logger::getLogger(name); \
  logger_add_file_appender(logger,layout,filename,append);}

#define GR_LOG_ADD_FILE_APPENDER(logger,layout,filename,append){\
  logger_add_file_appender(logger,layout,filename,append);}

#define GR_ADD_ROLLINGFILE_APPENDER(name,layout,filename,append,bkup_index,filesize){\
  gr_logger_ptr logger = log4cxx::Logger::getLogger(name); \
  logger_add_rollingfile_appender(logger,layout,filename,append,bkup_index,filesize);}

#define GR_LOG_ADD_ROLLINGFILE_APPENDER(logger,layout,filename,append,bkup_index,filesize){\
  logger_add_rollingfile_appender(logger,layout,filename,append,bkup_index,filesize);}

#define GR_GET_LOGGER_NAMES(names){ \
  logger_get_logger_names(names);}

#define GR_RESET_CONFIGURATION(){ \
  logger_reset_config();}

/* Logger name referenced macros */
#define GR_TRACE(name, msg) { \
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name); \
  LOG4CXX_TRACE(logger, msg);}

#define GR_DEBUG(name, msg) { \
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name); \
  LOG4CXX_DEBUG(logger, msg);}

#define GR_INFO(name, msg) { \
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name); \
  LOG4CXX_INFO(logger, msg);}

#define GR_WARN(name, msg) { \
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name); \
  LOG4CXX_WARN(logger, msg);}

#define GR_ERROR(name, msg) { \
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name); \
  LOG4CXX_ERROR(logger, msg);}

#define GR_FATAL(name, msg) { \
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name); \
  LOG4CXX_FATAL(logger, msg);}

#define GR_ERRORIF(name, cond, msg) { \
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name); \
  LOG4CXX_ASSERT(logger, !(cond), msg);}

#define GR_ASSERT(name, cond, msg) { \
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name); \
  LOG4CXX_ASSERT(logger, (cond), msg); \
  assert((cond));}

/* LoggerPtr Referenced Macros */
#define GR_LOG_TRACE(logger, msg) { \
  LOG4CXX_TRACE(logger, msg);}

#define GR_LOG_DEBUG(logger, msg) { \
  LOG4CXX_DEBUG(logger, msg);}

#define GR_LOG_INFO(logger, msg) { \
  LOG4CXX_INFO(logger, msg);}

#define GR_LOG_WARN(logger, msg) { \
  LOG4CXX_WARN(logger, msg);}

#define GR_LOG_ERROR(logger, msg) { \
  LOG4CXX_ERROR(logger, msg);}

#define GR_LOG_FATAL(logger, msg) { \
  LOG4CXX_FATAL(logger, msg);}

#define GR_LOG_ERRORIF(logger,cond, msg) { \
  LOG4CXX_ASSERT(logger, !(cond), msg);}

#define GR_LOG_ASSERT(logger, cond, msg) { \
  LOG4CXX_ASSERT(logger, (cond), msg); \
  assert((cond));}

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
 * \li off
 * \li all
 * \li trace
 * \li debug
 * \li info
 * \li warn
 * \li error
 * \li fatal
 *
 * \param logger the logger to set the level of.
 * \param level  string to set the level to.
 */
GR_CORE_API void logger_set_level(gr_logger_ptr logger, const std::string &level);

/*!
 * \brief Set the logger's output level.
 *
 * Sets the level of the logger. This takes the actual Log4cxx::Level
 * data type, which can be:
 *
 * \li log4cxx::Level::getOff()
 * \li log4cxx::Level::getAll()
 * \li log4cxx::Level::getTrace()
 * \li log4cxx::Level::getDebug()
 * \li log4cxx::Level::getInfo()
 * \li log4cxx::Level::getWarn()
 * \li log4cxx::Level::getError()
 * \li log4cxx::Level::getFatal()
 *
 * \param logger the logger to set the level of.
 * \param level  new logger level of type Log4cxx::Level
 */
GR_CORE_API void logger_set_level(gr_logger_ptr logger, log4cxx::LevelPtr level);


/*!
 * \brief Get the logger's output level.
 *
 * Gets the level of the logger. This returns a string that
 * corresponds to the standard levels and can be (case insensitive):
 *
 * \li off
 * \li all
 * \li trace
 * \li debug
 * \li info
 * \li warn
 * \li error
 * \li fatal
 *
 * \param logger the logger to get the level of.
 * \param level  string to get the level into.
 */
GR_CORE_API void logger_get_level(gr_logger_ptr logger, std::string &level);

/*!
 * \brief Get the logger's output level.
 *
 * Gets the level of the logger. This returns the actual Log4cxx::Level
 * data type, which can be:
 *
 * \li log4cxx::Level::getOff()
 * \li log4cxx::Level::getAll()
 * \li log4cxx::Level::getTrace()
 * \li log4cxx::Level::getDebug()
 * \li log4cxx::Level::getInfo()
 * \li log4cxx::Level::getWarn()
 * \li log4cxx::Level::getError()
 * \li log4cxx::Level::getFatal()
 *
 * \param logger the logger to get the level of.
 */
GR_CORE_API void logger_get_level(gr_logger_ptr logger, log4cxx::LevelPtr level);


GR_CORE_API void logger_add_console_appender(gr_logger_ptr logger, std::string layout,
                                             std::string target);

GR_CORE_API void logger_add_file_appender(gr_logger_ptr logger, std::string layout,
                                          std::string filename, bool append);

GR_CORE_API void logger_add_rollingfile_appender(gr_logger_ptr logger, std::string layout,
                                                 std::string filename, bool append,
                                                 int bkup_index, std::string filesize);

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
    if(config_filename.find(".xml")!=std::string::npos) {
      log4cxx::xml::DOMConfigurator::configureAndWatch(config_filename, watchPeriodSec);
    }
    else {
      log4cxx::PropertyConfigurator::configureAndWatch(config_filename, watchPeriodSec);
    }
  };

  static log4cxx::LoggerPtr getLogger(std::string name)
  {
    if(log4cxx::LogManager::exists(name)) {
      log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name);
      return logger;
    }
    else {
      log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name);
      logger->setLevel(log4cxx::Level::getOff());
      return logger;
    };
  };

  // Wrappers for logging macros
  /*! \brief inline function, wrapper to set the logger level */
  void set_level(std::string name,std::string level){GR_SET_LEVEL(name,level);}
  
  /*! \brief inline function, wrapper to get the logger level */
  void get_level(std::string name,std::string &level){GR_GET_LEVEL(name,level);}

  /*! \brief inline function, wrapper for LOG4CXX_TRACE for TRACE message */
  void trace(std::string name,std::string msg){GR_TRACE(name,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_DEBUG for DEBUG message */
  void debug(std::string name,std::string msg){GR_DEBUG(name,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_INFO for INFO message */
  void info(std::string name,std::string msg){GR_INFO(name,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_WARN for WARN message */
  void warn(std::string name,std::string msg){GR_WARN(name,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_ERROR for ERROR message */
  void error(std::string name,std::string msg){GR_ERROR(name,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_FATAL for FATAL message */
  void fatal(std::string name,std::string msg){GR_FATAL(name,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_ASSERT for conditional ERROR message */
  void errorIF(std::string name,bool cond,std::string msg){GR_ERRORIF(name,cond,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_ASSERT for conditional ERROR message */
  void gr_assert(std::string name,bool cond,std::string msg){GR_ASSERT(name,cond,msg);};

  // Wrappers for Logger Pointer referenced functions
  /*! \brief inline function, wrapper to set the logger level */
  void set_log_level(log4cxx::LoggerPtr logger,std::string level){GR_LOG_SET_LEVEL(logger,level);}

  /*! \brief inline function, wrapper to get the logger level */
  void get_log_level(log4cxx::LoggerPtr logger,std::string &level){GR_LOG_GET_LEVEL(logger,level);}

  /*! \brief inline function, wrapper for LOG4CXX_TRACE for TRACE message */
  void log_trace(log4cxx::LoggerPtr logger,std::string msg){GR_LOG_TRACE(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_DEBUG for DEBUG message */
  void log_debug(log4cxx::LoggerPtr logger,std::string msg){GR_LOG_DEBUG(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_INFO for INFO message */
  void log_info(log4cxx::LoggerPtr logger,std::string msg){GR_LOG_INFO(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_WARN for WARN message */
  void log_warn(log4cxx::LoggerPtr logger,std::string msg){GR_LOG_WARN(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_ERROR for ERROR message */
  void log_error(log4cxx::LoggerPtr logger,std::string msg){GR_LOG_ERROR(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_FATAL for FATAL message */
  void log_fatal(log4cxx::LoggerPtr logger,std::string msg){GR_LOG_FATAL(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_ASSERT for conditional ERROR message */
  void log_errorIF(log4cxx::LoggerPtr logger,bool cond,std::string msg){GR_LOG_ERRORIF(logger,cond,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_ASSERT for conditional ERROR message */
  void log_assert(log4cxx::LoggerPtr logger,bool cond,std::string msg){GR_LOG_ASSERT(logger,cond,msg);};
};


#endif /* HAVE_LOG4CXX */

// If Logger disable do nothing
#else /* ENABLE_GR_LOG */

typedef void* gr_logger_ptr;

#define GR_LOG_DECLARE_LOGPTR(logger)
#define GR_LOG_ASSIGN_LOGPTR(logger,name)
#define GR_CONFIG_LOGGER(config)
#define GR_CONFIG_AND_WATCH_LOGGER(config)
#define GR_LOG_GETLOGGER(logger, name) 
#define GR_SET_LEVEL(name, level)
#define GR_LOG_SET_LEVEL(logger, level) 
#define GR_GET_LEVEL(name, level)
#define GR_LOG_GET_LEVEL(logger, level)
#define GR_ADD_CONSOLE_APPENDER(logger,layout)
#define GR_LOG_ADD_CONSOLE_APPENDER(logger,layout)
#define GR_ADD_FILE_APPENDER(logger,layout,filename,append)
#define GR_LOG_ADD_FILE_APPENDER(logger,layout)
#define GR_ADD_ROLLINGFILE_APPENDER(logger,layout,filename,append,bkup_index,filesize)
#define GR_LOG_ADD_ROLLINGFILE_APPENDER(logger,layout)
#define GR_GET_LOGGER_NAMES(names)
#define GR_RESET_CONFIGURATION()
#define GR_TRACE(name, msg)
#define GR_DEBUG(name, msg)
#define GR_INFO(name, msg)
#define GR_WARN(name, msg) 
#define GR_ERROR(name, msg) 
#define GR_FATAL(name, msg) 
#define GR_ERRORIF(name, cond, msg)
#define GR_ASSERT(name, cond, msg)
#define GR_LOG_SET_LEVEL(logger, level)
#define GR_LOG_GET_LEVEL(logger, level)
#define GR_LOG_TRACE(logger, msg)
#define GR_LOG_DEBUG(logger, msg)
#define GR_LOG_INFO(logger, msg) 
#define GR_LOG_WARN(logger, msg) 
#define GR_LOG_ERROR(logger, msg) 
#define GR_LOG_FATAL(logger, msg) 
#define GR_LOG_ERRORIF(logger, cond, msg)
#define GR_LOG_ASSERT(logger, cond, msg)

#endif /* ENABLE_GR_LOG */
#endif /* INCLUDED_GR_LOGGER_H */

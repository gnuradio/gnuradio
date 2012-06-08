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

/************************************************
* Copyright 2011 JHU APL
* Description:
*   The gr_log module wraps the log4cxx library for logging in gnuradio
*
**************************************************/

#ifndef INCLUDED_GR_LOG_H
#define INCLUDED_GR_LOG_H

/*!
* \file gr_log.h
* \ingroup logging
* \brief GNURADIO logging wrapper for log4cxx library (C++ port of log4j)
*
*/

#ifdef ENABLE_GR_LOG

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <assert.h>

#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/propertyconfigurator.h>

using namespace log4cxx;
using namespace log4cxx::xml;
using namespace log4cxx::helpers;

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

#define GR_CONFIG_LOGGER(config)	\
  logger_load_config(config)

#define GR_LOG_GETLOGGER(logger, name) \
  LoggerPtr logger = gr_log::getLogger(name);

/* Logger name referenced macros */
#define GR_TRACE(name, msg) { \
  LoggerPtr logger = Logger::getLogger(name); \
  LOG4CXX_TRACE(logger, msg);}

#define GR_DEBUG(name, msg) { \
  LoggerPtr logger = Logger::getLogger(name); \
  LOG4CXX_DEBUG(logger, msg);}

#define GR_INFO(name, msg) { \
  LoggerPtr logger = Logger::getLogger(name); \
  LOG4CXX_INFO(logger, msg);}

#define GR_WARN(name, msg) { \
  LoggerPtr logger = Logger::getLogger(name); \
  LOG4CXX_WARN(logger, msg);}

#define GR_ERROR(name, msg) { \
  LoggerPtr logger = Logger::getLogger(name); \
  LOG4CXX_ERROR(logger, msg);}

#define GR_FATAL(name, msg) { \
  LoggerPtr logger = Logger::getLogger(name); \
  LOG4CXX_FATAL(logger, msg);}

#define GR_ERRORIF(name, cond, msg) { \
  LoggerPtr logger = Logger::getLogger(name); \
  LOG4CXX_ASSERT(logger, !(cond), msg);}

#define GR_ASSERT(name, cond, msg) { \
  LoggerPtr logger = Logger::getLogger(name); \
  LOG4CXX_ASSERT(logger, cond, msg); \
  assert(cond);}

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
  LOG4CXX_ASSERT(logger, cond, msg); \
  assert(cond);}


// Load configuration file
void
logger_load_config(std::string config_filename)
{
  if(config_filename.find(".xml") != std::string::npos) {
    DOMConfigurator::configure(config_filename);
  }
  else {
    PropertyConfigurator::configure(config_filename);
  }
}

/*!
 * \brief instantiate (configure) logger.
 * \ingroup logging
 *
 */
class gr_log
{
 public:
  /*!
   * \brief contructor take log configuration file and configures loggers.
   */
  gr_log(std::string config_filename)
  {
    // Load configuration file
    logger_load_config(config_filename);
  };

  /*!
   * \brief contructor take log configuration file and watchtime and configures
   */
  gr_log(std::string config_filename, int watchPeriodSec)
  {
    // Load configuration file
    if(config_filename.find(".xml")!=std::string::npos) {
      DOMConfigurator::configureAndWatch(config_filename, watchPeriodSec);
    }
    else {
      PropertyConfigurator::configureAndWatch(config_filename, watchPeriodSec);
    }
  };

  static LoggerPtr getLogger(std::string name)
  {
    if(LogManager::exists(name)) {
      LoggerPtr logger = Logger::getLogger(name);
      return logger;
    }
    else {
      LoggerPtr logger = Logger::getLogger(name);
      logger->setLevel(log4cxx::Level::getOff());
      return logger;
    }
  };

  // Wrappers for logging macros
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

  /*! \brief inline function, wrapper for LOG4CXX_TRACE for TRACE message */
  void log_trace(LoggerPtr logger,std::string msg){GR_LOG_TRACE(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_DEBUG for DEBUG message */
  void log_debug(LoggerPtr logger,std::string msg){GR_LOG_DEBUG(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_INFO for INFO message */
  void log_info(LoggerPtr logger,std::string msg){GR_LOG_INFO(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_WARN for WARN message */
  void log_warn(LoggerPtr logger,std::string msg){GR_LOG_WARN(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_ERROR for ERROR message */
  void log_error(LoggerPtr logger,std::string msg){GR_LOG_ERROR(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_FATAL for FATAL message */
  void log_fatal(LoggerPtr logger,std::string msg){GR_LOG_FATAL(logger,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_ASSERT for conditional ERROR message */
  void log_errorIF(LoggerPtr logger,bool cond,std::string msg){GR_LOG_ERRORIF(logger,cond,msg);};

  /*! \brief inline function, wrapper for LOG4CXX_ASSERT for conditional ERROR message */
  void log_assert(LoggerPtr logger,bool cond,std::string msg){GR_LOG_ASSERT(logger,cond,msg);};
};


//If ENABLE_GR_LOG not set then clear all logging macros
#else

#define GR_CONFIG_LOGGER(config)
#define GR_LOG_GETLOGGER(logger, name) 
#define GR_TRACE(name, msg)
#define GR_DEBUG(name, msg)
#define GR_INFO(name, msg)
#define GR_WARN(name, msg) 
#define GR_ERROR(name, msg) 
#define GR_FATAL(name, msg) 
#define GR_ERRORIF(name, cond, msg)
#define GR_ASSERT(name, cond, msg)
#define GR_LOG_TRACE(logger, msg)
#define GR_LOG_DEBUG(logger, msg)
#define GR_LOG_INFO(logger, msg) 
#define GR_LOG_WARN(logger, msg) 
#define GR_LOG_ERROR(logger, msg) 
#define GR_LOG_FATAL(logger, msg) 
#define GR_LOG_ERRORIF(logger, cond, msg)
#define GR_LOG_ASSERT(logger, cond, msg)

#endif /* ENABLE_GR_LOG */
#endif /* INCLUDED_GR_LOG_H */

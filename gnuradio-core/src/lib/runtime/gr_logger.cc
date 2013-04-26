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
* Author: Mark Plett 
* Description:
*   The gr_log module wraps the log4cpp library for logging in gnuradio.
*******************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h" 
#endif

#include <gr_logger.h>
#include <stdexcept>
#include <algorithm>

#ifdef ENABLE_GR_LOG
#ifdef HAVE_LOG4CPP 

bool gr_logger_configured(false);

void
logger_load_config(const std::string &config_filename)
{
  if(!gr_logger_configured){
    gr_logger_configured = true;
    if(config_filename.size() != 0) {
       try
       {
         log4cpp::PropertyConfigurator::configure(config_filename);
       }
       catch( log4cpp::ConfigureFailure &e )
       {
         std::cout << "Logger config failed :" << e.what() << std::endl;
       }
    };
  };
}

void
logger_load_config_and_watch(const std::string &config_filename,
                             unsigned int watch_period)
{
// NOTE:: NEEDS CODE TO WATCH FILE HERE
  if(!gr_logger_configured){
    gr_logger_configured = true;
    if(config_filename.size() != 0) {
       try
       {
         log4cpp::PropertyConfigurator::configure(config_filename);
       }
       catch( log4cpp::ConfigureFailure &e )
       {
         std::cout << "Logger config failed :" << e.what() << std::endl;
       }
    };
  };
}

void 
logger_reset_config(void){
  std::vector<log4cpp::Category*> *loggers = log4cpp::Category::getCurrentCategories();
  std::vector<log4cpp::Category*>::iterator logger = loggers->begin();
// We can't destroy categories but we can neuter them by removing all appenders.
  for (;logger!=loggers->end();logger++){
    (*logger)->removeAllAppenders();
  };
}

void
logger_set_level(gr_logger_ptr logger, const std::string &level)
{
  std::string nocase = level;
  std::transform(level.begin(), level.end(), nocase.begin(), ::tolower);

  if(nocase == "off" || nocase == "notset")
    logger_set_level(logger, log4cpp::Priority::NOTSET);
  else if(nocase == "debug")
    logger_set_level(logger, log4cpp::Priority::DEBUG);
  else if(nocase == "info")
    logger_set_level(logger, log4cpp::Priority::INFO);
  else if(nocase == "notice")
    logger_set_level(logger, log4cpp::Priority::NOTICE);
  else if(nocase == "warn")
    logger_set_level(logger, log4cpp::Priority::WARN);
  else if(nocase == "error")
    logger_set_level(logger, log4cpp::Priority::ERROR);
  else if(nocase == "crit")
    logger_set_level(logger, log4cpp::Priority::CRIT);
  else if(nocase == "alert")
    logger_set_level(logger, log4cpp::Priority::ALERT);
 else if(nocase=="fatal")
    logger_set_level(logger, log4cpp::Priority::FATAL);
  else if(nocase == "all" || nocase == "emerg")
    logger_set_level(logger, log4cpp::Priority::EMERG);
  else
    throw std::runtime_error("logger_set_level: Bad level type.\n");
}

void
logger_set_level(gr_logger_ptr logger, log4cpp::Priority::Value level)
{
  logger->setPriority(level);
}

void 
logger_get_level(gr_logger_ptr logger, std::string &level)
{
  log4cpp::Priority::Value levelPtr = logger->getPriority();
  if(levelPtr == log4cpp::Priority::NOTSET) level = "noset";
  if(levelPtr == log4cpp::Priority::DEBUG) level = "debug";
  if(levelPtr == log4cpp::Priority::INFO) level = "info";
  if(levelPtr == log4cpp::Priority::NOTICE) level = "notice";
  if(levelPtr == log4cpp::Priority::WARN) level = "warn";
  if(levelPtr == log4cpp::Priority::ERROR) level = "error";
  if(levelPtr == log4cpp::Priority::CRIT) level = "crit";
  if(levelPtr == log4cpp::Priority::ALERT) level = "alert";
  if(levelPtr == log4cpp::Priority::FATAL) level = "fatal";
  if(levelPtr == log4cpp::Priority::EMERG) level = "emerg";
};

void 
logger_get_level(gr_logger_ptr logger,log4cpp::Priority::Value level)
{
  level = logger->getPriority();
}

void 
logger_add_console_appender(gr_logger_ptr logger,std::string target,std::string pattern)
{

  log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
  log4cpp::Appender* app;
  if(target=="cout")
    app = new log4cpp::OstreamAppender("ConsoleAppender::",&std::cout);
  else
	  app = new log4cpp::OstreamAppender("ConsoleAppender::",&std::cerr);

  layout->setConversionPattern(pattern);
	app->setLayout(layout);
  logger->setAppender(app);

}

void
logger_add_file_appender(gr_logger_ptr logger,std::string filename,bool append,std::string pattern)
{

  log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	log4cpp::Appender* app = new 
              log4cpp::FileAppender("FileAppender::"+filename,
              filename);
  layout->setConversionPattern(pattern);
	app->setLayout(layout);
  logger->setAppender(app);

}

void 
logger_add_rollingfile_appender(gr_logger_ptr logger,std::string filename,
            size_t filesize,int bkup_index,bool append,mode_t mode,std::string pattern)
{
  log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
  log4cpp::Appender* app = new 
              log4cpp::RollingFileAppender("RollFileAppender::"+filename,filename,filesize,bkup_index,append,mode);
  layout->setConversionPattern(pattern);
  app->setLayout(layout);
  logger->setAppender(app);
}

void
logger_get_logger_names(std::vector<std::string>& names){
  std::vector<log4cpp::Category*> *loggers = log4cpp::Category::getCurrentCategories();
  std::vector<log4cpp::Category*>::iterator logger = loggers->begin();

  names.clear();
  for (;logger!=loggers->end();logger++){
    names.push_back((*logger)->getName());
  };
  
}

#endif /* HAVE_LOG4CPP */
#endif /* ENABLE_GR_LOGGER */

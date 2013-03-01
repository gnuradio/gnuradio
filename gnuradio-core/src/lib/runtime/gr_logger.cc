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
*   The gr_log module wraps the log4cxx library for logging in gnuradio.
*******************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h" 
#endif

#include <gr_logger.h>
#include <stdexcept>
#include <algorithm>

#ifdef ENABLE_GR_LOG
#ifdef HAVE_LOG4CXX 

bool gr_logger_configured(false);

void
logger_load_config(const std::string &config_filename)
{
  if(!gr_logger_configured) {
    gr_logger_configured = true;
    if(config_filename.size() > 0) {
      if(config_filename.find(".xml") != std::string::npos) {
        log4cxx::xml::DOMConfigurator::configure(config_filename);
      }
      else {
        log4cxx::PropertyConfigurator::configure(config_filename);
      }
    }
  }
}

void
logger_load_config_and_watch(const std::string &config_filename,
                             unsigned int watch_period)
{
  if(!gr_logger_configured) {
    gr_logger_configured = true;
    if(config_filename.size() > 0) {
      if(config_filename.find(".xml") != std::string::npos) {
        log4cxx::xml::DOMConfigurator::configureAndWatch(config_filename, watch_period);
      }
      else {
        log4cxx::PropertyConfigurator::configureAndWatch(config_filename, watch_period);
      }
    }
  }
}

void 
logger_reset_config(void)
{
  log4cxx::LogManager::resetConfiguration();
  gr_logger_configured=false;
}

void
logger_set_level(gr_logger_ptr logger, const std::string &level)
{
  std::string nocase = level;
  std::transform(level.begin(), level.end(), nocase.begin(), ::tolower);
  
  if(nocase == "off")
    logger_set_level(logger, log4cxx::Level::getOff());
  else if(nocase == "all")
    logger_set_level(logger, log4cxx::Level::getAll());
  else if(nocase == "trace")
    logger_set_level(logger, log4cxx::Level::getTrace());
  else if(nocase == "debug")
    logger_set_level(logger, log4cxx::Level::getDebug());
  else if(nocase == "info")
    logger_set_level(logger, log4cxx::Level::getInfo());
  else if(nocase == "warn")
    logger_set_level(logger, log4cxx::Level::getWarn());
  else if(nocase == "error")
    logger_set_level(logger, log4cxx::Level::getError());
  else if(nocase == "fatal")
    logger_set_level(logger, log4cxx::Level::getFatal());
  else
    throw std::runtime_error("logger_set_level: Bad level type.\n");
}

void
logger_set_level(gr_logger_ptr logger, log4cxx::LevelPtr level)
{
  logger->setLevel(level);
}

void 
logger_get_level(gr_logger_ptr logger, std::string &level)
{
  log4cxx::LevelPtr levelPtr = logger->getLevel();
  if(levelPtr == log4cxx::Level::getOff()) level = "off";
  if(levelPtr == log4cxx::Level::getAll()) level = "all";
  if(levelPtr == log4cxx::Level::getTrace()) level = "trace";
  if(levelPtr == log4cxx::Level::getDebug()) level = "debug";
  if(levelPtr == log4cxx::Level::getInfo()) level = "info";
  if(levelPtr == log4cxx::Level::getWarn()) level = "warn";
  if(levelPtr == log4cxx::Level::getError()) level = "error";
  if(levelPtr == log4cxx::Level::getFatal()) level = "fatal";
};

void 
logger_get_level(gr_logger_ptr logger, log4cxx::LevelPtr level)
{
  level = logger->getLevel();
}

void 
logger_add_console_appender(gr_logger_ptr logger, std::string layout,
                            std::string target)
{
  log4cxx::PatternLayout *playout =
    new log4cxx::PatternLayout(layout);
  log4cxx::ConsoleAppender *appender =
    new log4cxx::ConsoleAppender(log4cxx::LayoutPtr(playout), target);
  log4cxx::helpers::Pool p;
  appender->activateOptions(p);
  logger->addAppender(appender);
}

void
logger_add_file_appender(gr_logger_ptr logger, std::string layout,
                         std::string filename, bool append)
{
  log4cxx::PatternLayout *playout =
    new log4cxx::PatternLayout(layout);
  log4cxx::FileAppender *appender =
    new log4cxx::FileAppender(log4cxx::LayoutPtr(playout), filename, append);
  log4cxx::helpers::Pool p;
  appender->activateOptions(p);
  logger->addAppender(appender);
}

void 
logger_add_rollingfile_appender(gr_logger_ptr logger, std::string layout,
                                std::string filename, bool append,
                                int bkup_index, std::string filesize)
{
  log4cxx::PatternLayout *playout =
    new log4cxx::PatternLayout(layout);
  log4cxx::RollingFileAppender *appender = 
    new log4cxx::RollingFileAppender(log4cxx::LayoutPtr(playout), filename, append);
  appender->setMaxBackupIndex(bkup_index);
  appender->setMaxFileSize(filesize);
  log4cxx::helpers::Pool p; 
  appender->activateOptions(p);
  logger->addAppender(appender);
}

void
logger_get_logger_names(std::vector<std::string>& names)
{
  log4cxx::LoggerList list = log4cxx::LogManager::getCurrentLoggers();
  log4cxx::LoggerList::iterator logger = list.begin();
  names.clear();
  for(; logger != list.end(); logger++) {
    names.push_back((*logger)->getName());
  }
}

#endif /* HAVE_LOG4CXX */
#endif /* ENABLE_GR_LOGGER */

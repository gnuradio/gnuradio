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

#include <gnuradio/logger.h>
#include <stdexcept>
#include <algorithm>


#ifdef ENABLE_GR_LOG
#ifdef HAVE_LOG4CPP

namespace gr {

  bool logger_config::logger_configured(false);

  /************************ BEGIN LOG4CPP HELPERS ***********************/
  /* Logger config class.  This is a singleton that controls how
   * log4cpp is configured If watch_period>0 a thread is started to
   * watch teh config file for changes.
   */

  // Getters of logger_config
  logger_config&
  logger_config::get_instance(void)
  {
    static logger_config instance;
    return instance;
  }

  std::string
  logger_config::get_filename()
  {
    logger_config& in=get_instance();
    return in.filename;
  }

  unsigned int
  logger_config::get_watch_period()
  {
    logger_config& in=get_instance();
    return in.watch_period;
  }

  // Method to watch config file for changes
  void
  logger_config::watch_file(std::string filename, unsigned int watch_period)
  {
    std::time_t last_write(boost::filesystem::last_write_time(filename));
    std::time_t current_time(0);
    while(true) {
      try {
        current_time = boost::filesystem::last_write_time(filename);
        if(current_time>last_write) {
          //std::cout<<"GNURadio Reloading logger configuration:"<<filename<<std::endl;
          last_write = current_time;
          // Should we wipe out all old configuration or just add the
          //     new?  Just adding...  logger_reset_config();
          logger_configured = logger_load_config(filename);
        }
        boost::this_thread::sleep(boost::posix_time::time_duration(0,0,watch_period,0));
      }
      catch(const boost::thread_interrupted&) {
        std::cout<<"GNURadio leaving logger config file watch."<<std::endl;
        break;
      }
    }
  }

  // Method to load the confifuration.  It only loads if the filename
  // or watch has changed
  void
  logger_config::load_config(std::string filename,unsigned int watch_period)
  {
    logger_config& instance = get_instance();
    // Only reconfigure if filename or watch has changed
    if(!logger_configured) {
      instance.filename = filename;
      instance.watch_period = watch_period;
      // Stop any file watching thread
      if(instance.watch_thread!=NULL)
        stop_watch();
      // Load configuration
      //std::cout<<"GNURadio Loading logger configuration:"<<instance.filename<<std::endl;
      logger_configured = logger_load_config(instance.filename);
      // Start watch if required
      if(instance.watch_period>0) {
        instance.watch_thread = new boost::thread(watch_file, instance.filename,
                                                  instance.watch_period);
      }
    }
  }

  // Method to stop the watcher thread
  void
  logger_config::stop_watch()
  {
    logger_config& instance = get_instance();
    if(instance.watch_thread) {
      instance.watch_thread->interrupt();
      instance.watch_thread->join();
      delete(instance.watch_thread);
      instance.watch_thread=NULL;
    }
  }

  // Method to reset logger configuration
  void
  logger_config::reset_config(void)
  {
    logger_config& instance = get_instance();
    stop_watch();
    std::vector<log4cpp::Category*> *loggers = log4cpp::Category::getCurrentCategories();
    std::vector<log4cpp::Category*>::iterator logger = loggers->begin();
    // We can't destroy categories but we can neuter them by removing all appenders.
    for(;logger!=loggers->end();logger++) {
      (*logger)->removeAllAppenders();
    }
    instance.filename = std::string("");
    instance.watch_period = 0;
    logger_configured = false;
  }

  /***************** Functions to call log4cpp methods *************************/

  logger_ptr
  logger_get_logger(std::string name)
  {
    if(log4cpp::Category::exists(name)) {
      logger_ptr logger = &log4cpp::Category::getInstance(name);
      return logger;
    }
    else {
      logger_ptr logger = &log4cpp::Category::getInstance(name);
      logger->setPriority(log4cpp::Priority::NOTSET);
      return logger;
    }
  }

  bool
  logger_load_config(const std::string &config_filename)
  {
    if(config_filename.size() != 0) {
      try {
        log4cpp::PropertyConfigurator::configure(config_filename);
        return true;
      }
      catch(log4cpp::ConfigureFailure &e) {
        std::cerr << "Logger config failed :" << e.what() << std::endl;
      }
    }
    return false;
  }

  void
  logger_set_level(logger_ptr logger, const std::string &level)
  {
    std::string nocase = level;
    std::transform(level.begin(), level.end(), nocase.begin(), ::tolower);

    if(nocase == "off" || nocase == "notset")
      logger_set_level(logger, log4cpp::Priority::NOTSET);
    else if(nocase == "all" || nocase == "debug")
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
    else if(nocase == "emerg")
      logger_set_level(logger, log4cpp::Priority::EMERG);
    else
      throw std::runtime_error("logger_set_level: Bad level type.\n");
  }

  void
  logger_set_level(logger_ptr logger, log4cpp::Priority::Value level)
  {
    logger->setPriority(level);
  }

  void
  logger_get_level(logger_ptr logger, std::string &level)
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
  }

  void
  logger_get_level(logger_ptr logger,log4cpp::Priority::Value level)
  {
    level = logger->getPriority();
  }

  void
  logger_add_console_appender(logger_ptr logger, std::string target, std::string pattern)
  {
    log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
    log4cpp::Appender* app;
    if(target=="stdout")
      app = new log4cpp::OstreamAppender("ConsoleAppender::",&std::cout);
    else
      app = new log4cpp::OstreamAppender("ConsoleAppender::",&std::cerr);

    layout->setConversionPattern(pattern);
    app->setLayout(layout);
    logger->setAppender(app);
  }

  void
  logger_add_file_appender(logger_ptr logger, std::string filename,
                           bool append, std::string pattern)
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
  logger_add_rollingfile_appender(logger_ptr logger, std::string filename,
                                  size_t filesize, int bkup_index, bool append,
                                  mode_t mode, std::string pattern)
  {
    log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
    log4cpp::Appender* app = new
      log4cpp::RollingFileAppender("RollFileAppender::" + filename, filename,
                                   filesize, bkup_index, append, mode);
    layout->setConversionPattern(pattern);
    app->setLayout(layout);
    logger->setAppender(app);
  }

  std::vector<std::string>
  logger_get_logger_names(void)
  {
    std::vector<std::string> names;
    std::vector<log4cpp::Category*> *loggers = log4cpp::Category::getCurrentCategories();
    std::vector<log4cpp::Category*>::iterator logger = loggers->begin();

    for(;logger!=loggers->end();logger++) {
      names.push_back((*logger)->getName());
    }
    return names;
  }

} /* namespace gr */

#endif /* HAVE_LOG4CPP */

/****** Start Methods to provide Python the capabilities of the macros ********/
void
gr_logger_config(const std::string config_filename, unsigned int watch_period)
{
  GR_CONFIG_AND_WATCH_LOGGER(config_filename, watch_period);
}

std::vector<std::string>
gr_logger_get_logger_names(void)
{
  std::vector<std::string> names;
  GR_GET_LOGGER_NAMES(names);
  return names;
}

void
gr_logger_reset_config(void)
{
  GR_RESET_CONFIGURATION();
}

// Remaining capability provided by gr::logger class in gnuradio/logger.h

#else /* ENABLE_GR_LOGGER */

/****** Start Methods to provide Python the capabilities of the macros ********/
void
gr_logger_config(const std::string config_filename, unsigned int watch_period)
{
  //NOP
}

std::vector<std::string>
gr_logger_get_logger_names(void)
{
  return std::vector<std::string>(1, "");
}

void
gr_logger_reset_config(void)
{
  //NOP
}

#endif /* ENABLE_GR_LOGGER */

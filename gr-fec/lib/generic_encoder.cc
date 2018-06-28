/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/prefs.h>
#include <stdio.h>

namespace gr {
  namespace fec {

    generic_encoder::generic_encoder(std::string name)
    {
      d_name = name;
      my_id = base_unique_id++;

#ifdef ENABLE_GR_LOG
#ifdef HAVE_LOG4CPP
      prefs *p = prefs::singleton();
      std::string config_file = p->get_string("LOG", "log_config", "");
      std::string log_level = p->get_string("LOG", "log_level", "off");
      std::string log_file = p->get_string("LOG", "log_file", "");

      GR_CONFIG_LOGGER(config_file);

      GR_LOG_GETLOGGER(LOG, "gr_log." + alias());
      GR_LOG_SET_LEVEL(LOG, log_level);
      if(log_file.size() > 0) {
        if(log_file == "stdout") {
          GR_LOG_SET_CONSOLE_APPENDER(LOG, "stdout","gr::log :%p: %c{1} - %m%n");
        }
        else if(log_file == "stderr") {
          GR_LOG_SET_CONSOLE_APPENDER(LOG, "stderr","gr::log :%p: %c{1} - %m%n");
        }
        else {
          GR_LOG_SET_FILE_APPENDER(LOG, log_file , true,"%r :%p: %c{1} - %m%n");
        }
      }
      d_logger = LOG;

#endif /* HAVE_LOG4CPP */
#else /* ENABLE_GR_LOG */
      d_logger = NULL;
#endif /* ENABLE_GR_LOG */
    }

    generic_encoder::~generic_encoder()
    {
    }

    const char*
    generic_encoder::get_input_conversion()
    {
      return "none";
    }

    const char*
    generic_encoder::get_output_conversion()
    {
      return "none";
    }

    int generic_encoder::base_unique_id = 1;
    int
    generic_encoder::unique_id()
    {
      return my_id;
    }

    /*******************************************************
     * Static functions
     ******************************************************/
    int
    get_encoder_output_size(generic_encoder::sptr my_encoder)
    {
      return my_encoder->get_output_size();
    }

    int
    get_encoder_input_size(generic_encoder::sptr my_encoder)
    {
      return my_encoder->get_input_size();
    }

    const char*
    get_encoder_input_conversion(generic_encoder::sptr my_encoder)
    {
      return my_encoder->get_input_conversion();
    }

    const char*
    get_encoder_output_conversion(generic_encoder::sptr my_encoder)
    {
      return my_encoder->get_output_conversion();
    }

  } /* namespace fec */
} /* namespace gr */

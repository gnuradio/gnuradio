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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "message_strobe_random_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <iostream>

namespace gr {
  namespace blocks {

    message_strobe_random::sptr
    message_strobe_random::make(pmt::pmt_t msg, message_strobe_random_distribution_t dist, float mean_ms, float std_ms)
    {
      return gnuradio::get_initial_sptr
        (new message_strobe_random_impl(msg, dist, mean_ms, std_ms));
    }

    message_strobe_random_impl::message_strobe_random_impl(pmt::pmt_t msg, message_strobe_random_distribution_t dist, float mean_ms, float std_ms)
      : block("message_strobe_random",
                 io_signature::make(0, 0, 0),
                 io_signature::make(0, 0, 0)),
        d_finished(false),
        d_mean_ms(mean_ms),
        d_std_ms(std_ms),
        d_dist(dist),
        d_msg(msg),
        d_rng()
    {
      // allocate RNGs
      update_dist();
      
      // set up ports
      message_port_register_out(pmt::mp("strobe"));
      d_thread = boost::shared_ptr<boost::thread>
        (new boost::thread(boost::bind(&message_strobe_random_impl::run, this)));

      message_port_register_in(pmt::mp("set_msg"));
      set_msg_handler(pmt::mp("set_msg"),
                      boost::bind(&message_strobe_random_impl::set_msg, this, _1));
    }

    float message_strobe_random_impl::next_delay(){
        switch(d_dist){
            case STROBE_POISSON:
                //return d_variate_poisson->operator()();
                return d_variate_poisson->operator()();
            case STROBE_GAUSSIAN:
                return d_variate_normal->operator()();
            case STROBE_UNIFORM:
                return d_variate_uniform->operator()();
            default:
                throw std::runtime_error("message_strobe_random_impl::d_distribution is very unhappy with you");
        }
    }

    void message_strobe_random_impl::update_dist(){
        boost::poisson_distribution<> pd(d_mean_ms);
        d_variate_poisson = boost::shared_ptr< boost::variate_generator<boost::mt19937, boost::poisson_distribution<> > > (
                new boost::variate_generator <boost::mt19937, boost::poisson_distribution<> >(d_rng,pd) );
        
        boost::normal_distribution<> nd(d_mean_ms, d_std_ms);
        d_variate_normal = boost::shared_ptr< boost::variate_generator<boost::mt19937, boost::normal_distribution<> > > (
                new boost::variate_generator <boost::mt19937, boost::normal_distribution<> >(d_rng,nd) );

        boost::uniform_real<> ud(d_mean_ms-d_std_ms, d_mean_ms+d_std_ms);
        d_variate_uniform = boost::shared_ptr< boost::variate_generator<boost::mt19937, boost::uniform_real<> > > (
                new boost::variate_generator <boost::mt19937, boost::uniform_real<> >(d_rng,ud) );
    }


    message_strobe_random_impl::~message_strobe_random_impl()
    {
      d_finished = true;
      d_thread->interrupt();
      d_thread->join();
    }

    void message_strobe_random_impl::run()
    {
      while(!d_finished) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(std::max(0.0f,next_delay()))); 
        if(d_finished) {
          return;
        }

        message_port_pub(pmt::mp("strobe"), d_msg);
      }
    }

  } /* namespace blocks */
} /* namespace gr */

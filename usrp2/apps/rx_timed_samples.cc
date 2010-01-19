/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp2/usrp2.h>
#include <usrp2/rx_nop_handler.h>
#include <gruel/realtime.h>
#include <sys/time.h>
#include <iostream>
#include <stdexcept>
#include <boost/format.hpp>
#include <cstring>
#include <boost/lexical_cast.hpp>

/***********************************************************************
 * This app exercises the start streaming functionality of the usrp2:
 *   1) Set the usrp2 time to the current host system time
 *   2) Call start streaming with a time in the near future
 *   3) Print the timestamps on the received packets
 **********************************************************************/

// ------------------------------------------------------------------------

class rx_handler : public vrt::rx_packet_handler
{
public:

  rx_handler(size_t max_samples)
  {
      d_num_samples = 0;
      d_max_samples = max_samples;
  }

  ~rx_handler(){}

  bool //print the timestamps, increment the number of samples
  operator()(const uint32_t *items, size_t nitems, const vrt::expanded_header *vrt_header)
  {
    std::cout << boost::format("Got packet: Secs %d, Ticks %d\n")
        % vrt_header->integer_secs % vrt_header->fractional_secs;
    d_num_samples += nitems;
    return true;
  }

  bool done(void){
    return d_num_samples >= d_max_samples;
  }

private:
  size_t d_max_samples, d_num_samples;
};

// ------------------------------------------------------------------------

static void
usage(const char *progname)
{
  const char *p = strrchr(progname, '/');	// drop leading directory path
  if (p)
    p++;

  if (strncmp(p, "lt-", 3) == 0)		// drop lt- libtool prefix
    p += 3;
  
  std::cerr << boost::format("Usage: %s [options]\n\n") % p;
  std::cerr << boost::format("Options:\n");
  std::cerr << boost::format("  -h                   show this message and exit\n");
  std::cerr << boost::format("  -e ETH_INTERFACE     specify ethernet interface [default=eth0]\n");
  std::cerr << boost::format("  -m MAC_ADDR          mac address of USRP2 HH:HH [default=first one found]\n");
  std::cerr << boost::format("  -N NSAMPLES          specify number of samples to receive [default=1000]\n");
  std::cerr << boost::format("  -s SECONDS           specify number of seconds in the future to receive [default=3]\n");
}

int
main(int argc, char **argv)
{
  // options and their defaults
  const char *interface = "eth0";
  const char *mac_addr_str = "";
  size_t nsamples = 1000;
  size_t nseconds = 3;

  int ch;

  while ((ch = getopt(argc, argv, "he:m:N:s:")) != -1){
    switch (ch){

    case 'e':
      interface = optarg;
      break;
      
    case 'm':
      mac_addr_str = optarg;
      break;

    case 'N':
      nsamples = boost::lexical_cast<size_t>(optarg);
      break;

    case 's':
      nseconds = boost::lexical_cast<size_t>(optarg);
      break;

    case 'h':
    default:
      usage(argv[0]);
      exit(1);
    }
  }

  std::cout << std::endl << std::endl;

  //create a new handler to print info about packets
  rx_handler my_handler(nsamples);

  //create a new usrp2 object, set some properties
  usrp2::usrp2::sptr u2 = usrp2::usrp2::make(interface, mac_addr_str);
  u2->set_rx_decim(16);

  //set the current host time to the usrp2
  struct timeval curr_time;
  gettimeofday(&curr_time, NULL);
  uint32_t curr_secs = curr_time.tv_sec;
  long clk_rate; u2->fpga_master_clock_freq(&clk_rate);
  uint32_t curr_ticks = (uint64_t)curr_time.tv_usec*clk_rate/1000000ul;
  std::cout << boost::format("Current time: Secs %u, Ticks %u\n") % curr_secs % curr_ticks;
  u2->set_time(usrp2::time_spec_t(curr_secs, curr_ticks));

  //begin streaming n seconds in the future
  std::cout << boost::format("Begin streaming %u seconds in the future...\n\n") % nseconds;
  u2->start_rx_streaming(0, usrp2::time_spec_t(curr_secs+nseconds, curr_ticks));
  while(not my_handler.done()){
    u2->rx_samples(&my_handler);
  }

  //done, stop streaming
  u2->stop_rx_streaming();

  std::cout << std::endl << std::endl;
  return 0;
}

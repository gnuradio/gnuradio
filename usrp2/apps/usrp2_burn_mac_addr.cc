/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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
#include <iostream>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <stdexcept>


static volatile bool signaled = false;

static void 
sig_handler(int sig)
{
  signaled = true;
}

static void
install_sig_handler(int signum,
		    void (*new_handler)(int))
{
  struct sigaction new_action;
  memset (&new_action, 0, sizeof (new_action));

  new_action.sa_handler = new_handler;
  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = 0;

  if (sigaction (signum, &new_action, 0) < 0){
    perror ("sigaction (install new)");
    throw std::runtime_error ("sigaction");
  }
}


static void
usage(const char *progname)
{
  fprintf(stderr, "usage: %s [-e ethN] [-m old_mac_addr] new_mac_addr\n",
	  progname);
  fprintf(stderr, "  old_mac_addr defaults to 00:50:c2:85:3f:ff\n");
  fprintf(stderr, "  new_mac_address must be HH:HH or HH:HH:HH:HH:HH:HH\n");
}

int
main(int argc, char **argv)
{
  int ch;
  const char *interface = "eth0";
  const char *old_mac_addr = "00:50:c2:85:3f:ff";
  const char *new_mac_addr = 0;

  while ((ch = getopt(argc, argv, "he:m:")) != EOF){
    switch (ch){
    case 'e':
      interface = optarg;
      break;
      
    case 'm':
      old_mac_addr = optarg;
      break;
      
    case 'h':
    default:
      usage(argv[0]);
      exit(1);
    }
  }

  if (argc - optind != 1){
    usage(argv[0]);
    exit(1);
  }

  new_mac_addr = argv[optind];
  
  install_sig_handler(SIGINT, sig_handler);

  usrp2::usrp2::sptr u2;

  try {
    usrp2::props p(usrp2::USRP_TYPE_ETH);
    p.eth_args.ifc = interface;
    p.eth_args.mac_addr = old_mac_addr;
    u2 = usrp2::usrp2::make(p);
  }
  catch (std::exception const &e){
    std::cerr << e.what() << std::endl;
    return 1;
  }

  if (!u2->burn_mac_addr(usrp2::u2_mac_addr(new_mac_addr))){
    std::cerr << "Failed to burn mac address: "
	      << new_mac_addr << std::endl;
    return 1;
  }

  u2.reset();	// close

  // wait 250 ms
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 250000000;
  nanosleep(&ts, 0);

  try {
    usrp2::props p(usrp2::USRP_TYPE_ETH);
    p.eth_args.ifc = interface;
    p.eth_args.mac_addr = new_mac_addr;
    u2 = usrp2::usrp2::make(p);
  }
  catch (std::exception const &e){
    std::cerr << "Failed to connect to USRP2 using new addr: "
	      << new_mac_addr << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

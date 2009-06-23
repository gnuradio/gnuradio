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
#include <cstdio>
#include <getopt.h>

static void
usage(const char *progname)
{
  fprintf(stderr, "usage: %s [-e ethN]\n",
	  progname);
}

int
main(int argc, char **argv)
{
  int ch;
  const char *interface = "eth0";

  while ((ch = getopt(argc, argv, "he:")) != EOF){
    switch (ch){
    case 'e':
      interface = optarg;
      break;
      
    case 'h':
    default:
      usage(argv[0]);
      exit(1);
    }
  }

  if (argc - optind != 0){
    usage(argv[0]);
    exit(1);
  }
  
  usrp2::props_vector_t r = usrp2::find(interface);

  for (size_t i = 0; i < r.size(); i++){
    std::cout << r[i] << std::endl;
  }

  if (r.size() == 0){
    std::cerr << "No USRP2 found.\n";
    return 1;
  }

  return 0;
}

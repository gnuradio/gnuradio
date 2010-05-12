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
#include "config.h"
#endif
#include <usrp2/usrp2.h>
#include <usrp2/strtod_si.h>
#include <iostream>
#include <cstdio>
#include <complex>
#include <getopt.h>
#include <gruel/realtime.h>
#include <signal.h>
#include <string.h>
#include <stdexcept>


typedef std::complex<float> fcomplex;

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


static const char *
prettify_progname(const char *progname)		// that's probably almost a word ;)
{
  const char *p = strrchr(progname, '/');	// drop leading directory path
  if (p)
    p++;

  if (strncmp(p, "lt-", 3) == 0)		// drop lt- libtool prefix
    p += 3;

  return p;
}

static void
usage(const char *progname)
{
  fprintf(stderr, "Usage: %s [options]\n\n", prettify_progname(progname));
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -h                   show this message and exit\n");
  fprintf(stderr, "  -e ETH_INTERFACE     specify ethernet interface [default=eth0]\n");
  fprintf(stderr, "  -m MAC_ADDR          mac address of USRP2 HH:HH [default=first one found]\n");
  fprintf(stderr, "  -I INPUT_FILE        set input filename [default=stdin]\n");
  fprintf(stderr, "  -r                   repeat.  When EOF of input file is reached, seek to beginning\n");
  fprintf(stderr, "  -f FREQ              set frequency to FREQ [default=0]\n");
  fprintf(stderr, "  -i INTERP            set interpolation rate to INTERP [default=32]\n");
  fprintf(stderr, "  -g gain              set tx gain\n");
  fprintf(stderr, "  -S SCALE             fpga scaling factor for I & Q [default=256]\n");
}

#define GAIN_NOT_SET (-1000)
#define	MAX_SAMPLES (371)

int
main(int argc, char **argv)
{
  const char *interface = "eth0";
  const char *input_filename = 0;
  bool repeat = false;
  const char *mac_addr_str = "";
  double freq = 0;
  int32_t interp = 32;
  int32_t samples_per_frame = MAX_SAMPLES;
  int32_t scale = -1;
  double gain = GAIN_NOT_SET;
  
  int    ch;
  double tmp;


  while ((ch = getopt(argc, argv, "he:m:I:rf:i:S:F:g:")) != EOF){
    switch (ch){

    case 'e':
      interface = optarg;
      break;
      
    case 'm':
      mac_addr_str = optarg;
#if 0
      if (!usrp2_basic::parse_mac_addr(optarg, &mac_addr)){
	std::cerr << "invalid mac addr: " << optarg << std::endl;
	usage(argv[0]);
	return 1;
      }
#endif
      break;

    case 'I':
      input_filename = optarg;
      break;
      
    case 'r':
      repeat = true;
      break;
      
    case 'f':
      if (!strtod_si(optarg, &freq)){
	std::cerr << "invalid number: " << optarg << std::endl;
	usage(argv[0]);
	return 1;
      }
      break;

    case 'F':
      samples_per_frame = strtol(optarg, 0, 0);
      break;

    case 'i':
      interp = strtol(optarg, 0, 0);
      break;

    case 'g':
      gain = strtod(optarg, 0);
      break;

    case 'S':
      if (!strtod_si(optarg, &tmp)){
	std::cerr << "invalid number: " << optarg << std::endl;
	usage(argv[0]);
	return 1;
      }
      scale = static_cast<int32_t>(tmp);
      break;
      
    case 'h':
    default:
      usage(argv[0]);
      return 1;
    }
  }

  
  if (argc - optind != 0){
    usage(argv[0]);
    return 1;
  }
  
  if (samples_per_frame < 9 || samples_per_frame > MAX_SAMPLES){
    std::cerr << prettify_progname(argv[0])
	      << ": samples_per_frame is out of range.  "
	      << "Must be in [9, " << MAX_SAMPLES << "].\n";
    usage(argv[0]);
    return 1;
  }


  FILE *fp = 0;
  if (input_filename == 0)
    fp = stdin;
  else {
    fp = fopen(input_filename, "rb");
    if (fp == 0){
      perror(input_filename);
      return 1;
    }
  }

  install_sig_handler(SIGINT, sig_handler);


  gruel::rt_status_t rt = gruel::enable_realtime_scheduling();
  if (rt != gruel::RT_OK)
    std::cerr << "Failed to enable realtime scheduling" << std::endl;


  usrp2::usrp2::sptr u2 = usrp2::usrp2::make(interface, mac_addr_str);
  
  if (gain != GAIN_NOT_SET){
    if (!u2->set_tx_gain(gain)){
      std::cerr << "set_tx_gain failed\n";
      return 1;
    }
  }

  usrp2::tune_result tr;
  if (!u2->set_tx_center_freq(freq, &tr)){
    fprintf(stderr, "set_tx_center_freq(%g) failed\n", freq);
    return 1;
  }

  printf("Daughterboard configuration:\n");
  printf("  baseband_freq=%f\n", tr.baseband_freq);
  printf("       duc_freq=%f\n", tr.dxc_freq);
  printf("  residual_freq=%f\n", tr.residual_freq);
  printf("       inverted=%s\n\n", tr.spectrum_inverted ? "yes" : "no");

  if (!u2->set_tx_interp(interp)){
    fprintf(stderr, "set_tx_interp(%d) failed\n", interp);
    return 1;
  }

  if (scale != -1){
    if (!u2->set_tx_scale_iq(scale, scale)){
      std::cerr << "set_tx_scale_iq failed\n";
      return 1;
    }
  }

  usrp2::tx_metadata	md;
  md.timestamp = -1;
  md.start_of_burst = 1;
  md.send_now = 1;

  while (!signaled){

    std::complex<int16_t> samples[MAX_SAMPLES];

    int r = fread(samples, sizeof(uint32_t), samples_per_frame, fp);

    // fprintf(stderr, "fread -> %d\n", r);
    
    if (r == 0){
      if (!repeat)
	break;
      if (fseek(fp, 0, SEEK_SET) == -1)
	break;
    }

    // FIXME if r < 9, pad to 9 for minimum packet size constraint

    if (!u2->tx_16sc(0, samples, r, &md)){
      fprintf(stderr, "tx_complex_int16 failed\n");
      break;
    }
  }

  return 0;
}

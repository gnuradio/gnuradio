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
#include <usrp2/strtod_si.h>
#include <usrp2/copiers.h>
#include <usrp2/rx_nop_handler.h>
#include <gruel/realtime.h>
#include <sys/time.h>
#include <iostream>
#include <cstdio>
#include <string.h>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <signal.h>

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

// ------------------------------------------------------------------------

// FIXME make this a template

class file_writer_16sc : public usrp2::rx_nop_handler
{
  FILE	       *d_fp;
  std::string	d_filename;
  
public:

  file_writer_16sc(const std::string &filename, uint64_t max_samples)
    : usrp2::rx_nop_handler(max_samples), d_filename(filename)
  {
    d_fp = fopen(filename.c_str(), "wb");
    if (d_fp == 0){
      perror(filename.c_str());
      throw std::invalid_argument(filename);
    }
  }

  ~file_writer_16sc();

  bool 
  operator()(const uint32_t *items, size_t nitems, const usrp2::rx_metadata *metadata)
  {
    bool ok = rx_nop_handler::operator()(items, nitems, metadata);

    size_t host_nitems = nitems;
    std::complex<int16_t> host_items[host_nitems];

    usrp2::copy_u2_16sc_to_host_16sc(nitems, items, host_items);

    size_t n = 0;
    while (n < host_nitems){
      size_t r = fwrite(&host_items[n], sizeof(host_items[0]), host_nitems - n, d_fp);
      n += r;
      if (r == 0){	// out of space?
        d_err = true;
	perror(d_filename.c_str());
	ok = false;
	break;
      }
    }

    return ok;
  }
};

file_writer_16sc::~file_writer_16sc()
{
  fclose(d_fp);
}

// ------------------------------------------------------------------------

class file_writer_32fc : public usrp2::rx_nop_handler
{
  FILE	       *d_fp;
  std::string	d_filename;
  
public:

  file_writer_32fc(const std::string &filename, uint64_t max_samples)
    : usrp2::rx_nop_handler(max_samples), d_filename(filename)
  {
    d_fp = fopen(filename.c_str(), "wb");
    if (d_fp == 0){
      perror(filename.c_str());
      throw std::invalid_argument(filename);
    }
  }

  ~file_writer_32fc();

  bool 
  operator()(const uint32_t *items, size_t nitems, const usrp2::rx_metadata *metadata)
  {
    bool ok = rx_nop_handler::operator()(items, nitems, metadata);

    size_t host_nitems = nitems;
    std::complex<float> host_items[host_nitems];

    usrp2::copy_u2_16sc_to_host_32fc(nitems, items, host_items);

    size_t n = 0;
    while (n < host_nitems){
      size_t r = fwrite(&host_items[n], sizeof(host_items[0]), host_nitems - n, d_fp);
      n += r;
      if (r == 0){	// out of space?
        d_err = true;
	perror(d_filename.c_str());
	ok = false;
	break;
      }
    }

    return ok;
  }
};

file_writer_32fc::~file_writer_32fc()
{
  fclose(d_fp);
}

// ------------------------------------------------------------------------

static void
usage(const char *progname)
{
  const char *p = strrchr(progname, '/');	// drop leading directory path
  if (p)
    p++;

  if (strncmp(p, "lt-", 3) == 0)		// drop lt- libtool prefix
    p += 3;
  
  fprintf(stderr, "Usage: %s [options]\n\n", p);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -h                   show this message and exit\n");
  fprintf(stderr, "  -e ETH_INTERFACE     specify ethernet interface [default=eth0]\n");
  fprintf(stderr, "  -m MAC_ADDR          mac address of USRP2 HH:HH [default=first one found]\n");
  fprintf(stderr, "  -f FREQUENCY         specify receive center frequency in Hz [default=0.0]\n");
  fprintf(stderr, "  -d DECIM             specify receive decimation rate [default=5]\n");
  fprintf(stderr, "  -g GAIN              specify receive daughterboard gain [default=0]\n");
  fprintf(stderr, "  -N NSAMPLES          specify number of samples to receive [default=infinite]\n");
  fprintf(stderr, "  -o OUTPUT_FILENAME   specify file to receive samples [default=none]\n");
  fprintf(stderr, "  -s                   write complex<short> [default=complex<float>]\n");
  fprintf(stderr, "  -v                   verbose output\n");
}

int
main(int argc, char **argv)
{
  // options and their defaults
  const char *interface = "eth0";
  const char *mac_addr_str = "";
  double rx_freq = 0.0;
  int rx_decim = 5;
  double rx_gain = 0.0;
  uint64_t nsamples = 0;
  bool output_shorts = false;
  char *output_filename = 0;
  bool verbose = false;

  int ch;

  while ((ch = getopt(argc, argv, "he:m:f:d:g:N:o:sv")) != EOF){
    double tmp;
    switch (ch){

    case 'e':
      interface = optarg;
      break;
      
    case 'm':
      mac_addr_str = optarg;
      break;

    case 'f':
      if (!strtod_si(optarg, &rx_freq)) {
        std::cerr << "invalid number: " << optarg << std::endl;
	usage(argv[0]);
	exit(1);
      }
      break;

    case 'g':
      if (!strtod_si(optarg, &rx_gain)) {
        std::cerr << "invalid number: " << optarg << std::endl;
	usage(argv[0]);
	exit(1);
      }
      break;

    case 'd':
      rx_decim = strtol(optarg, 0, 0);
      if (rx_decim < 4 or rx_decim > 512) { 
        std::cerr << "invalid decimation rate: " << optarg << std::endl;
	usage(argv[0]);
	exit(1);
      }
      break;

    case 'N':
      if (!strtod_si(optarg, &tmp)) {
        std::cerr << "invalid number: " << optarg << std::endl;
	usage(argv[0]);
	exit(1);
      }
      nsamples = static_cast<uint64_t>(tmp);
      break;
      
    case 's':
      output_shorts = true;
      break;

    case 'o':
      output_filename = optarg;
      break;
      
    case 'v':
      verbose = true;
      break;

    case 'h':
    default:
      usage(argv[0]);
      exit(1);
    }
  }


  install_sig_handler(SIGINT, sig_handler);

  usrp2::rx_nop_handler::sptr handler;

  if (output_filename){
    if (output_shorts)
      handler = usrp2::rx_nop_handler::sptr(new file_writer_16sc(output_filename, nsamples));
    else
      handler = usrp2::rx_nop_handler::sptr(new file_writer_32fc(output_filename, nsamples));
  }
  else
    handler = usrp2::rx_nop_handler::sptr(new usrp2::rx_nop_handler(nsamples));

  gruel::rt_status_t rt = gruel::enable_realtime_scheduling();
  if (rt != gruel::RT_OK)
    std::cerr << "Failed to enable realtime scheduling" << std::endl;

  usrp2::usrp2::sptr u2 = usrp2::usrp2::make(interface, mac_addr_str);
  
  // FIXME in case it was left running...
  if (!u2->stop_rx_streaming()){
    fprintf(stderr, "stop_rx_streaming failed\n");
  }

  if (!u2->set_rx_gain(rx_gain)){
    fprintf(stderr, "set_rx_gain(%f) failed\n", rx_gain);
    exit(1);
  }

  usrp2::tune_result tr;
  if (!u2->set_rx_center_freq(rx_freq, &tr)){
    fprintf(stderr, "set_rx_center_freq(%g) failed\n", rx_freq);
    exit(1);
  }

  if (verbose){
    printf("USRP2 MAC address: %s\n\n", u2->mac_addr().c_str());
    printf("Daughterboard configuration:\n");
    printf("  baseband_freq=%f\n", tr.baseband_freq);
    printf("       ddc_freq=%f\n", tr.dxc_freq);
    printf("  residual_freq=%f\n", tr.residual_freq);
    printf("       inverted=%s\n\n", tr.spectrum_inverted ? "yes" : "no");
  }
  
  if (!u2->set_rx_decim(rx_decim)) {
    fprintf(stderr, "set_rx_decim(%d) failed\n", rx_decim);
    exit(1);
  }

  if (verbose)
    printf("USRP2 using decimation rate of %d\n", rx_decim);
    
  if (!u2->start_rx_streaming(0)){
    fprintf(stderr, "start_rx_streaming failed\n");
    exit(1);
  }

  if (verbose) {
    if (nsamples > 0)
        printf("Receiving %zd samples\n\n", nsamples);
    else
        printf("Receiving infinite samples\n\n");
  }
  
  struct timeval start, end;
  gettimeofday(&start, 0);

  while (!signaled && 
         !handler->has_errored_p() && 
         !handler->has_finished_p()) {
    bool ok = u2->rx_samples(0, handler.get());
    if (!ok){
      fprintf(stderr, "u2->rx_samples failed\n");
      return 1;
    }
  }

  gettimeofday(&end, 0);
  long n_usecs = end.tv_usec-start.tv_usec;
  long n_secs = end.tv_sec-start.tv_sec;
  double elapsed = (double)n_secs + (double)n_usecs*1e-6;
  double mbs = handler->nsamples()*sizeof(uint32_t)/elapsed/1e6;
  double pps = handler->nframes()/elapsed;
  
  u2->stop_rx_streaming();

  if (verbose){
    printf("\nCopy handler called %li times.\n", handler->nframes());
    printf("Copy handler called with %li bytes.\n\n", handler->nsamples()*sizeof(uint32_t));
    printf("Elapsed time was %5.3f seconds.\n", elapsed);
    printf("Packet rate was %1.0f pkts/sec.\n", pps);
    printf("Approximate throughput was %5.2f MB/sec.\n", mbs);
    printf("Total instances of overruns was %d.\n", u2->rx_overruns());
    printf("Total missing frames was %d.\n", u2->rx_missing());
  }

  return 0;
}

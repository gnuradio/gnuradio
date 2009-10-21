/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <vrt/quadradio.h>
#include <vrt/rx.h>
#include <vrt/copiers.h>

#include <errno.h>
#include <iostream>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <gruel/realtime.h>
#include <complex>

#define	MIN_IP_LOCAL_PORT	32768
#define	MAX_IP_LOCAL_PORT	61000

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

class rx_nop_handler : public vrt::rx_packet_handler
{
private:
  uint64_t	d_max_samples;
  uint64_t	d_max_quantum;
  uint64_t	d_nsamples;
  uint64_t	d_npackets;
  int		d_last_pkt_cnt;
  uint64_t	d_nwrong_pkt_cnt;

protected:
  bool		d_err;

public:

  // Shared pointer to an instance of this class
  typedef boost::shared_ptr<rx_nop_handler> sptr;

  /*!
   * Constructor
   *
   * \param max_samples  Maximum number of samples to copy. Use zero for no maximum.
   * \param max_quantum  Maximum number of samples required to accept in one call.
   *                     Use 0 to indicate no maximum.
   */
  rx_nop_handler(uint64_t max_samples, uint64_t max_quantum=0)
    : d_max_samples(max_samples), d_max_quantum(max_quantum),
      d_nsamples(0), d_npackets(0), 
      d_last_pkt_cnt(0xf), d_nwrong_pkt_cnt(0),
      d_err(false){}

    
    ~rx_nop_handler();
  
  bool operator()(const uint32_t *payload,
		  size_t n32_bit_words,
		  const vrt::expanded_header *hdr);

  /*!
   * \brief Returns number of packets this copier was called with
   */
  uint64_t npackets() const { return d_npackets; }

  /*!
   * \brief Returns actual number of samples copied
   */
  uint64_t nsamples() const { return d_nsamples; }

  /*!
   * \brief Returns maximum number of samples that will be copied
   */
  uint64_t max_samples() const { return d_max_samples; }

  /*!
   * Returns true if an error has occurred. Derived classes must set d_err to true
   * when an error occurs in the () operator
   */
  bool has_errored_p() const { return d_err; }

  /*!
   * \brief Returns true if this instance has reached the maximum number of samples
   */
  bool has_finished_p() const 
  { return d_max_samples == 0 ? false : d_nsamples >= d_max_samples-d_max_quantum; }
      
  uint64_t nwrong_pkt_cnt() const { return d_nwrong_pkt_cnt; }


};


rx_nop_handler::~rx_nop_handler()
{
}

bool
rx_nop_handler::operator()(const uint32_t *payload,
			   size_t n32_bit_words,
			   const vrt::expanded_header *hdr)
{
  if (d_npackets != 0 && hdr->pkt_cnt() != ((d_last_pkt_cnt + 1) & 0xf)){
    d_nwrong_pkt_cnt++;
    fprintf(stderr, "bad cnt (pkt %lld)\n", d_npackets);
  }
  d_last_pkt_cnt = hdr->pkt_cnt();

  d_nsamples += n32_bit_words;
  d_npackets++;

  return !has_finished_p();
}

// ------------------------------------------------------------------------

class file_writer_16sc : public rx_nop_handler
{
  FILE	       *d_fp;
  std::string	d_filename;
  
public:

  file_writer_16sc(const std::string &filename, uint64_t max_samples)
    : rx_nop_handler(max_samples), d_filename(filename)
  {
    d_fp = fopen(filename.c_str(), "wb");
    if (d_fp == 0){
      perror(filename.c_str());
      throw std::invalid_argument(filename);
    }
  }

  ~file_writer_16sc();

  bool 
  operator()(const uint32_t *items, size_t nitems, const vrt::expanded_header *hdr)
  {
    bool ok = rx_nop_handler::operator()(items, nitems, hdr);

    size_t host_nitems = nitems;
    std::complex<int16_t> host_items[host_nitems];

    vrt::copy_net_16sc_to_host_16sc(nitems, items, host_items);

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

class file_writer_32fc : public rx_nop_handler
{
  FILE	       *d_fp;
  std::string	d_filename;
  
public:

  file_writer_32fc(const std::string &filename, uint64_t max_samples)
    : rx_nop_handler(max_samples), d_filename(filename)
  {
    d_fp = fopen(filename.c_str(), "wb");
    if (d_fp == 0){
      perror(filename.c_str());
      throw std::invalid_argument(filename);
    }
  }

  ~file_writer_32fc();

  bool 
  operator()(const uint32_t *items, size_t nitems, const vrt::expanded_header *hdr)
  {
    bool ok = rx_nop_handler::operator()(items, nitems, hdr);

    size_t host_nitems = nitems;
    std::complex<float> host_items[host_nitems];

    vrt::copy_net_16sc_to_host_32fc(nitems, items, host_items);

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
//fprintf(stderr, "  -e ETH_INTERFACE     specify ethernet interface [default=eth0]\n");
//fprintf(stderr, "  -m MAC_ADDR          mac address of USRP2 HH:HH [default=first one found]\n");
//fprintf(stderr, "  -f FREQUENCY         specify receive center frequency in Hz [default=0.0]\n");
//fprintf(stderr, "  -d DECIM             specify receive decimation rate [default=5]\n");
//fprintf(stderr, "  -g GAIN              specify receive daughterboard gain [default=0]\n");
  fprintf(stderr, "  -N NSAMPLES          specify number of samples to receive [default=infinite]\n");
  fprintf(stderr, "  -o OUTPUT_FILENAME   specify file to receive samples [default=none]\n");
  fprintf(stderr, "  -s                   write complex<short> [default=complex<float>]\n");
  fprintf(stderr, "  -S samples_per_pkt   specify # of samples per pkt [default=maximum]\n");
//fprintf(stderr, "  -v                   verbose output\n");
}


int
main(int argc, char **argv)
{
  const char *quad_radio_ip = "192.168.123.123";
  size_t rx_bufsize = 62.5e6;	    // sizeof memory mapped network buffer
  int samples_per_pkt = 0;	    // use default
  uint64_t nsamples = 0;
  char *output_filename = 0;
  bool output_shorts = false;
  int t;

  int ch;

  while ((ch = getopt(argc, argv, "hN:o:sS:")) != EOF){
    switch (ch){
    case 'N':
      nsamples = (uint64_t) strtod(optarg, 0);
      break;

    case 'o':
      output_filename = optarg;
      break;

    case 's':
      output_shorts = true;
      break;

    case 'S':
      errno = 0;
      t = strtol(optarg, 0, 0);
      if (errno != 0){
	usage(argv[0]);
	exit(1);
      }
      samples_per_pkt = t;
      break;

    case 'h':
    default:
      usage(argv[0]);
      exit(1);
    }
  }


  install_sig_handler(SIGINT, sig_handler);

  gruel::rt_status_t rt = gruel::enable_realtime_scheduling();
  if (rt != gruel::RT_OK)
    std::cerr << "Failed to enable realtime scheduling" << std::endl;

    
  vrt::quadradio::sptr qr;
  try {
    qr = vrt::quadradio::sptr(new vrt::quadradio(quad_radio_ip, rx_bufsize));
  }
  catch (...){
    std::cerr << "Failed to create vrt::quadradio\n";
    return 1;
  }


  rx_nop_handler::sptr handler;
  if (output_filename){
    if (output_shorts)
      handler = rx_nop_handler::sptr(new file_writer_16sc(output_filename, nsamples));
    else
      handler = rx_nop_handler::sptr(new file_writer_32fc(output_filename, nsamples));
  }
  else
    handler = rx_nop_handler::sptr(new rx_nop_handler(nsamples));


  printf("samples_per_pkt = %d\n", samples_per_pkt);

  if (!qr->start_streaming(0, samples_per_pkt)){
    fprintf(stderr, "failed to send_rx_command\n");
    return 1;
  }

  // start receiving packets

  while(1
	&& !signaled
	&& !handler->has_errored_p()
	&& !handler->has_finished_p()){
    bool ok = qr->vrt_rx()->rx_packets(handler.get());
    if (!ok){
      fprintf(stderr, "vrt->rx_packets failed\n");
      break;
    }
  }

  qr->stop_streaming(0);

  printf("%llu packets received, %llu bad pkt_cnt field values, %llu samples\n",
	 handler->npackets(), handler->nwrong_pkt_cnt(), handler->nsamples());

  //sleep(1);

  return 0;
}

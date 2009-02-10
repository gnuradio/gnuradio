/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
#include "usrp2_basic.h"
#include <iostream>
#include <complex>
#include <getopt.h>
#include <string.h>
#include "strtod_si.h"
#include <signal.h>
#include <stdexcept>
#include "gri_if_stats.h"
#include <gr_realtime.h>


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


/*
 * Vectorize me!
 */
void
convert_samples_to_complex(size_t nsamples,
			   uint32_t *i_samples,
			   fcomplex *c_samples)
{
  uint32_t *p = i_samples;
  for (size_t i = 0; i < nsamples; i++){
    int16_t  si = ((int16_t) (p[i] >> 16));
    int16_t  sq = ((int16_t) (p[i] & 0xffff));
    c_samples[i] = fcomplex((float) si, (float) sq);
  }
}


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
  fprintf(stderr, "  -o OUTPUT_FILE       set output filename [default=NONE]\n");
  fprintf(stderr, "  -f FREQ              set frequency to FREQ [default=0]\n");
  fprintf(stderr, "  -d DECIM             set decimation rate to DECIM [default=32]\n");
  fprintf(stderr, "  -N NSAMPLES          total number of samples to receive [default=2.5e6]\n");
  fprintf(stderr, "  -F SAMPLES_PER_FRAME number of samples in each frame [default=371]\n");
  fprintf(stderr, "  -S SCALE             fpga scaling factor for I & Q [default=1024]\n");
  fprintf(stderr, "  -M DONT_LOCK|LOCK_TO_SMA|LOCK_TO_MIMO   specify MIMO clock source\n");
  fprintf(stderr, "  -P                   provide clock to MIMO connector\n");
}

struct pkt_info {
  int		d_nsamples;
  int		d_timestamp;
  unsigned int	d_seqno;

  pkt_info(int nsamples, int timestamp, int seqno)
    : d_nsamples(nsamples),
      d_timestamp(timestamp),
      d_seqno(seqno) {}
};

int
main(int argc, char **argv)
{

  // options and their defaults
  const char *interface = "eth0";
  const char *mac_addr_str = 0;
  const char *output_filename = 0;
  double freq = 0;
  int32_t decim = 32;
  int32_t nsamples = static_cast<int32_t>(2.5e6);
  int32_t samples_per_frame = 371;
  int32_t scale = 1024;
  int	  mimo_config = MC_WE_DONT_LOCK;
  bool	  provide_clock = false;

  int    ch;
  double tmp;
  u2_mac_addr_t mac_addr;

  setvbuf(stdout, 0, _IOFBF, 64 * 1024); // make stdout fully buffered

  while ((ch = getopt(argc, argv, "he:m:o:f:d:N:F:S:M:P")) != EOF){
    switch (ch){

    case 'e':
      interface = optarg;
      break;
      
    case 'm':
      mac_addr_str = optarg;
      if (!usrp2_basic::parse_mac_addr(optarg, &mac_addr)){
	std::cerr << "invalid mac addr: " << optarg << std::endl;
	usage(argv[0]);
	exit(1);
      }
      break;

    case 'o':
      output_filename = optarg;
      break;
      
    case 'f':
      if (!strtod_si(optarg, &freq)){
	std::cerr << "invalid number: " << optarg << std::endl;
	usage(argv[0]);
	exit(1);
      }
      break;

    case 'N':
      if (!strtod_si(optarg, &tmp)){
	std::cerr << "invalid number: " << optarg << std::endl;
	usage(argv[0]);
	exit(1);
      }
      nsamples = static_cast<int32_t>(tmp);
      break;

    case 'F':
      samples_per_frame = strtol(optarg, 0, 0);
      break;

    case 'd':
      decim = strtol(optarg, 0, 0);
      break;

    case 'S':
      if (!strtod_si(optarg, &tmp)){
	std::cerr << "invalid number: " << optarg << std::endl;
	usage(argv[0]);
	exit(1);
      }
      scale = static_cast<int32_t>(tmp);
      break;
      
    case 'M':
      if (strcmp(optarg, "DONT_LOCK") == 0)
	mimo_config = MC_WE_DONT_LOCK;
      else if (strcmp(optarg, "LOCK_TO_SMA") == 0)
	mimo_config = MC_WE_LOCK_TO_SMA;
      else if (strcmp(optarg, "LOCK_TO_MIMO") == 0)
	mimo_config = MC_WE_LOCK_TO_MIMO;
      else {
	usage(argv[0]);
	exit(1);
      }
      break;

    case 'P':
      provide_clock = true;
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

  FILE *of = 0;
  if (output_filename)
    of = fopen(output_filename, "wb");

  usrp2_basic *u2 = new usrp2_basic();

  if (!u2->open(interface)){
    std::cerr << "couldn't open " << interface << std::endl;
    return 0;
  }


  install_sig_handler(SIGINT, sig_handler);
  if (1){
    install_sig_handler(SIGALRM, sig_handler);
    alarm(5);
  }

  
  std::vector<op_id_reply_t> r = u2->find_usrps();

  for (size_t i = 0; i < r.size(); i++){
    std::cout << r[i] << std::endl;
  }

  if (r.size() == 0){
    std::cerr << "No USRP2 found.\n";
    return 1;
  }

  u2_mac_addr_t which = r[0].addr;	// pick the first one


  gr_rt_status_t rt = gr_enable_realtime_scheduling();
  if (rt != RT_OK)
    std::cerr << "failed to enable realtime scheduling\n";

  if (provide_clock)
    mimo_config |= MC_PROVIDE_CLK_TO_MIMO;

  u2->config_mimo(which, mimo_confg);
  

  gri_if_stats start, stop;
  gri_get_if_stats(interface, &start);

  if (!u2->start_rx(which, freq, decim, nsamples, samples_per_frame, scale, scale)){
    std::cerr << "start_rx failed\n";
    return 1;
  }


  std::vector<pkt_info> history;
  history.reserve(64*1024);		// preallocate 64K entries

  
  long total_samples_recvd = 0;

  while (!signaled && total_samples_recvd < nsamples){
    u2_eth_samples_t	pkt;
    // fcomplex		c_samples[U2_MAX_SAMPLES];
    
    // read samples
    int n = u2->read_samples(which, &pkt);
    if (n <= 0)
      break;
    
    total_samples_recvd += n;

    history.push_back(pkt_info(n, u2p_timestamp(&pkt.hdrs.fixed), pkt.hdrs.thdr.seqno));

    // convert_samples_to_complex(n, pkt.samples, c_samples);
    // size_t r = fwrite(c_samples, sizeof(fcomplex), n, of);

    if (of){
      fwrite(pkt.samples, sizeof(uint32_t), n, of);
      fflush(of);
    }
  }


  gri_get_if_stats(interface, &stop);

  if (!u2->stop_rx(which)){
    std::cerr << "stop_rx failed\n";
    return 1;
  }


  long expected_rx_packets =
    (nsamples + samples_per_frame - 1)/samples_per_frame;

  long expected_rx_bytes   =
    expected_rx_packets * sizeof(u2_eth_packet_t) + nsamples * 4;


  long total_pkts_recvd = 0;
  total_samples_recvd = 0;

  int nbad_seqno = 0;

  for (unsigned i = 0; i < history.size(); i++){
    total_pkts_recvd++;
    total_samples_recvd += history[i].d_nsamples;

    bool bad_seqno = history[i].d_seqno != (i & 0xff);
    if (bad_seqno)
      nbad_seqno++;
    
    printf("%3d  %8d  %8ld  %8ld  %3d %s\n",
	   history[i].d_nsamples,
	   history[i].d_timestamp,
	   total_pkts_recvd, total_samples_recvd,
	   history[i].d_seqno,
	   bad_seqno ? "BAD SEQNO" : ""
	   );
  }

  if (nbad_seqno == 0)
    printf("\nAll sequence numbers are correct\n");
  else
    printf("\n%d sequence numbers were INCORRECT\n", nbad_seqno);
    

  printf("\nUser space statistics:\n");
  printf("  rx_samples:  %8ld",     total_samples_recvd);
  printf("   expected  %8d  %s\n",
	 nsamples,
	 nsamples - total_samples_recvd == 0 ? "OK" : "NOT OK");
  
  printf("  rx_packets:  %8ld",     total_pkts_recvd);
  printf("   expected  %8ld  %s\n",
	 expected_rx_packets,
	 expected_rx_packets - total_pkts_recvd == 0 ? "OK" : "NOT OK");
  

  fflush(stdout);

  printf("\nKernel interface statistics:\n");

  long long delta;
  delta = stop.rx_bytes - start.rx_bytes;
  printf("  rx_bytes:    %8Ld",     delta);
  printf("   expected  %8ld  %s\n",
	 expected_rx_bytes,
	 expected_rx_bytes - delta == 0 ? "OK" : "NOT OK");

  delta = stop.rx_packets - start.rx_packets;
  printf("  rx_packets:  %8Ld",     delta);
  printf("   expected  %8ld  %s\n",
	 expected_rx_packets,
	 expected_rx_packets - delta == 0 ? "OK" : "NOT OK");

  printf("  rx_errs:     %8Ld\n",   stop.rx_errs - start.rx_errs);
  printf("  rx_drop:     %8Ld\n",   stop.rx_drop - start.rx_drop);
  printf("  tx_bytes:    %8Ld\n",   stop.tx_bytes - start.tx_bytes);
  printf("  tx_packets:  %8Ld\n",   stop.tx_packets - start.tx_packets);
  printf("  tx_errs:     %8Ld\n",   stop.tx_errs - start.tx_errs);
  printf("  tx_drop:     %8Ld\n",   stop.tx_drop - start.tx_drop);


  return 0;
}

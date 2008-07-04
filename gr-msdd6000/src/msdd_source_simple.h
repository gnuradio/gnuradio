#ifndef INCLUDED_MSDD_SOURCE_SIMPLE_H
#define INCLUDED_MSDD_SOURCE_SIMPLE_H

#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <gr_sync_block.h>
#include <msdd6000.h>


class msdd_source_simple;
typedef boost::shared_ptr<msdd_source_simple> msdd_source_simple_sptr;


// public shared_ptr constructor

msdd_source_simple_sptr msdd_make_source_simple ( const char *src, unsigned short port_src);



class msdd_source_simple : public gr_sync_block {
 private:
  friend msdd_source_simple_sptr
  msdd_make_source_simple ( const char *src, unsigned short port_src);

  MSDD6000* rcv;
  int d_lastseq;

 protected:
  msdd_source_simple (const char *src, unsigned short port_src);

 public:
  ~msdd_source_simple ();
  int ninput_bytes_reqd_for_noutput_items(int out);
  bool stop();
  bool start();

  bool set_decim_rate(unsigned int);
  bool set_rx_freq(int,double);
  bool set_pga(int,double);

  int work(int, gr_vector_const_void_star&, gr_vector_void_star&);
  
  long adc_freq();
  int decim_rate();
  std::vector<int> gain_range();
  std::vector<float> freq_range();
};

#endif /* INCLUDED_MSDD_SOURCE_C_H */

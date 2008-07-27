#ifndef MSDD6000_H
#define MSDD6000_H

#include <boost/scoped_ptr.hpp>

class MSDD6000 {
  class detail;

  //! holds objects with system dependent types
  boost::scoped_ptr<detail>	d_detail;  

public:

  enum state {
    STATE_STOPPED, STATE_STARTED,
  };

  MSDD6000(char* ip_addr);
  ~MSDD6000();

  void set_decim(int decim_pow2);
  void set_fc(int center_mhz, int offset_hz);	
  void set_ddc_gain(int gain);
  void set_rf_attn(int attn);

  void set_output(int mode, void* arg);

  void start();
  void stop();
	
  void send_request(float,float,float,float,float);	
  int read(char*, int);

  int d_decim;
  int d_fc_mhz;
  int d_offset_hz;
  int d_rf_attn;
  int d_ddc_gain;
  int d_sock;
  state d_state;

};


#endif

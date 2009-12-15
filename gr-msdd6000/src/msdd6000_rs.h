#ifndef MSDD_RS__RS_6000_H
#define MSDD_RS__RS_6000_H

#include <boost/scoped_ptr.hpp>

class MSDD6000_RS {
  class detail;

  //! holds objects with system dependent types
  boost::scoped_ptr<detail>	d_detail;  

public:

  enum state {
    STATE_STOPPED, STATE_STARTED
  };

  MSDD6000_RS(char* ip_addr);
  ~MSDD6000_RS();

  /* set functions -- sets digitizer parameters */
 
 // void set_output(int mode, void* arg);

  void set_rf_attn(int attn);
  void set_ddc_gain(int gain);
  void set_fc(int center_mhz, int offset_hz);
  void set_ddc_samp_rate(float sample_rate_khz);
  void set_ddc_bw(float bw_khz);

  void start();
  void stop();

  /* function starts the flow of data from the digitizer */
  int start_data();
  /* function stops the flow of data from the digitizer */
  int stop_data();
  
  /* query functions -- queries digitizer 'actual' parameters */
  float pull_ddc_samp_rate();
  float pull_ddc_bw();
  float pull_rx_freq();
  int   pull_ddc_gain();
  int   pull_rf_atten();
	
  void send_request(float,float,float,float,float,float,float);	
  int read(char*, int);

  int parse_control(char*, int);

private:
  // parameters for a receiver object.
  int   d_fc_mhz;
  int   d_offset_hz;
  int   d_rf_attn;
  int   d_ddc_gain;
  float d_ddc_sample_rate_khz;
  float d_ddc_bw_khz;
  int   d_start;
  int   d_sock;
  state d_state;

};


#endif

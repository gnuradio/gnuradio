#ifndef MSDD6000_H
#define MSDD6000_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/socket.h>

#define DEBUG(A)	printf("=debug=> %s\n", A)

#define STATE_STOPPED	0
#define STATE_STARTED	1

class MSDD6000 {
	public:
		MSDD6000(char* addr);
		
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

//		in_addr d_adx;	
		in_addr d_myadx;	

		struct sockaddr_in d_sockaddr;
		struct sockaddr_in d_mysockaddr;
	
		int d_sock;
		int d_state;
};






#endif

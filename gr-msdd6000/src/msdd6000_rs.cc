/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <msdd6000_rs.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#define DEBUG(A)	printf("=debug=> %s\n", A)

static void 
optimize_socket(int socket);

/*
 * Holds types that need autoconf help.  They're here and not in the .h file because
 * here we've got access to config.h
 */
class MSDD6000_RS::detail {
public:
  struct sockaddr_in d_sockaddr;
};


MSDD6000_RS::MSDD6000_RS(char* addr)
  : d_detail(new MSDD6000_RS::detail())
{
	d_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	
	optimize_socket(d_sock);
	
	
	// set up remote sockaddr
//	int s = inet_aton(addr, &d_adx); 
	d_detail->d_sockaddr.sin_family = AF_INET;
	d_detail->d_sockaddr.sin_port = htons(10000);
	int s = inet_aton(addr, &d_detail->d_sockaddr.sin_addr);
	
	// set up local sockaddr
	struct in_addr d_myadx;
	struct sockaddr_in d_mysockaddr;
	short int port = 10010;
	d_myadx.s_addr = INADDR_ANY;
	d_mysockaddr.sin_family = AF_INET;
	d_mysockaddr.sin_port = htons(port);
	memcpy(&d_mysockaddr.sin_addr.s_addr, &d_myadx.s_addr, sizeof(in_addr));
	//d_sockaddr.sin_addr = INADDR_ANY;
	s = bind(d_sock, (const sockaddr*) &d_mysockaddr, sizeof(d_mysockaddr));
	
	// set default values
	//d_decim = 2;
	d_ddc_gain 	= 2;
	d_rf_attn 	= 0;
	d_fc_mhz  	= 3500;
	d_offset_hz = 0;
        d_ddc_gain  = 0;
        d_ddc_sample_rate_khz = 25600;
        d_ddc_bw_khz          = 25600;
        d_start               = 0;
	d_state = STATE_STOPPED;
}

MSDD6000_RS::~MSDD6000_RS()
{
	//printf("MSDD6000_RS::Destructing\n");
	close(d_sock);
}


static void
optimize_socket(int socket){
#define BANDWIDTH	1000000000/8
#define DELAY		0.5
	int ret;

	int sock_buf_size = static_cast<int>(2*BANDWIDTH*DELAY);
	char textbuf[512];
	snprintf(textbuf, sizeof(textbuf), "%d", sock_buf_size);
	printf("sock_buf_size = %d\n", sock_buf_size);
	
	ret = setsockopt(socket, SOL_SOCKET, SO_SNDBUF,
			 &sock_buf_size, sizeof(sock_buf_size));

	ret = setsockopt(socket, SOL_SOCKET, SO_RCVBUF,
			 &sock_buf_size, sizeof(sock_buf_size));
	
	int uid = getuid();
	if(uid!=0){
		printf(" ****** COULD NOT OPTIMIZE SYSTEM NETWORK PARAMETERS BECAUSE YOU ARE NOT RUNNING AS ROOT *******\n ****** YOUR MSDD6000_RS RECIEVER PERFORMANCE IS GOING TO BE TERRIBLE *******\n");
		return;
	}


	// SET UP SOME SYSTEM WIDE TCP SOCKET PARAMETERS
	// FIXME seems like kind of a big hammer.  Are you sure you need this?
	FILE* fd = fopen("/proc/sys/net/core/netdev_max_backlog", "w");
	if (fd){
	  fwrite("10000", 1, strlen("10000"), fd);
	  fclose(fd);
	}

	fd = fopen("/proc/sys/net/core/rmem_max", "w");
	if (fd){
	  fwrite(textbuf, 1, strlen(textbuf), fd);
	  fclose(fd);
	}

	fd = fopen("/proc/sys/net/core/wmem_max", "w");
	if (fd){
	  fwrite(textbuf, 1, strlen(textbuf), fd);
	  fclose(fd);
	}

	// just incase these were rejected before because of max sizes...

	ret = setsockopt( socket, SOL_SOCKET, SO_SNDBUF,
                   (char *)&sock_buf_size, sizeof(sock_buf_size) );

	ret = setsockopt( socket, SOL_SOCKET, SO_RCVBUF,
                   (char *)&sock_buf_size, sizeof(sock_buf_size) );
	
}


//void MSDD6000_RS::set_decim(int decim_pow2){
//	DEBUG("SETTING NEW DECIM");
//	d_decim = decim_pow2;
//
//	if(d_state==STATE_STARTED)
//		send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_decim, d_offset_hz);
//}

void MSDD6000_RS::set_rf_attn(int attn){
	DEBUG("SETTING NEW RF ATTN");
	d_rf_attn = attn;
	if(d_state==STATE_STARTED)
		send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_offset_hz, d_ddc_sample_rate_khz, d_ddc_bw_khz, d_start);
}

void MSDD6000_RS::set_ddc_gain(int gain){
	DEBUG("SETTING NEW DDC GAIN");
	d_ddc_gain = gain;
	if(d_state==STATE_STARTED)
		send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_offset_hz, d_ddc_sample_rate_khz, d_ddc_bw_khz, d_start);
}

void MSDD6000_RS::set_fc(int center_mhz, int offset_hz){
	DEBUG("SETTING NEW FC");
	d_fc_mhz = center_mhz;
	d_offset_hz = offset_hz;
	
	send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_offset_hz, d_ddc_sample_rate_khz, d_ddc_bw_khz, d_start);
//	if(d_state==STATE_STARTED)
//		send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_decim, d_offset_hz);
//		send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_ddc_sample_rate_khz, d_ddc_bw_khz, d_start);
//		
}

void MSDD6000_RS::set_ddc_samp_rate(float sample_rate_khz){
	DEBUG("SETTING NEW SAMPLE RATE");
	d_ddc_sample_rate_khz = sample_rate_khz;
	send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_offset_hz, d_ddc_sample_rate_khz, d_ddc_bw_khz, d_start);		
}

void MSDD6000_RS::set_ddc_bw(float bw_khz){
	DEBUG("SETTING NEW DDC BW");
	d_ddc_bw_khz = bw_khz;
	send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_offset_hz, d_ddc_sample_rate_khz, d_ddc_bw_khz, d_start);		
}

void MSDD6000_RS::start(){
    send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_offset_hz, d_ddc_sample_rate_khz, d_ddc_bw_khz, d_start);       
    return;
}

void MSDD6000_RS::stop(){
	// new request with 0 decim tells it to halt
	stop_data();
}


int MSDD6000_RS::start_data(){
	d_start = 1;
	send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_offset_hz, d_ddc_sample_rate_khz, d_ddc_bw_khz, d_start);	
	d_state = STATE_STARTED;
    return 0;
	}


int MSDD6000_RS::stop_data(){
	// new request with 0 decim tells it to halt
	d_start = 0;
	send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_offset_hz, d_ddc_sample_rate_khz, d_ddc_bw_khz, d_start);	
	d_state = STATE_STOPPED;
    return 0;
	}
	
/* Query functions */
float MSDD6000_RS::pull_ddc_samp_rate(){
  return d_ddc_sample_rate_khz;
}
float MSDD6000_RS::pull_ddc_bw(){
	return d_ddc_bw_khz;
}

float MSDD6000_RS::pull_rx_freq(){
	return d_fc_mhz;
}
int   MSDD6000_RS::pull_ddc_gain(){
	return d_ddc_gain;
}

int   MSDD6000_RS::pull_rf_atten(){
	return d_rf_attn;
}


void MSDD6000_RS::send_request(float freq_mhz, float rf_attn, float ddc_gain, float ddc_offset_hz, float ddc_samp_rate_khz, float ddc_input_bw_khz, float ddc_start){
	static char buff[512];
	// Send MSDD6000_RS control frame.
	sprintf(buff, "%f %f %f %f %f %f %f\n",freq_mhz, rf_attn, ddc_gain, ddc_offset_hz, ddc_samp_rate_khz, ddc_input_bw_khz, ddc_start); //ddc_dec, ddc_offset_hz);
	printf("sending: %s\n", buff);
        int flags = 0;
	sendto( d_sock, buff, strlen(buff)+1, flags,
		(const sockaddr*)&(d_detail->d_sockaddr), sizeof(d_detail->d_sockaddr));
	}


int  MSDD6000_RS::read(char* buf, int size){
	int flags = 0;
	return recv(d_sock, buf, size, flags);
	}
	
int MSDD6000_RS::parse_control(char* buf, int size){
    //packet_len = sprintf(&txbuff[6], "%f %f %f %f %f %f %f",downsamp,ddc_dec_rate,ddc_step_int,ddc_step_frac,ddc_samp_rate_khz,ddc_input_bw_khz,ddc_start);    

    float downsamp;
    float ddc_dec_rate;
    float ddc_step_int;
    float ddc_step_frac;
    float ddc_samp_rate_khz;
    float ddc_input_bw_khz;
    float ddc_start;

    sscanf(&buf[6],"%f %f %f %f %f %f %f",&downsamp,&ddc_dec_rate,&ddc_step_int,&ddc_step_frac,&ddc_samp_rate_khz,&ddc_input_bw_khz,&ddc_start);
  
    // pull off sample rate  
    d_ddc_sample_rate_khz = ddc_samp_rate_khz;
    printf("Sample Rate %f\n",d_ddc_sample_rate_khz);
    // pull off bw
    d_ddc_bw_khz  =  ddc_input_bw_khz;
    printf("BW %f\n", d_ddc_bw_khz);
    return 0;
}



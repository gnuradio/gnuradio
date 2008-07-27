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
#include <msdd6000.h>

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
class MSDD6000::detail {
public:
  struct sockaddr_in d_sockaddr;
};


MSDD6000::MSDD6000(char* addr)
  : d_detail(new MSDD6000::detail())
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
	d_decim = 2;
	d_ddc_gain = 2;
	d_rf_attn = 0;
	d_state = STATE_STOPPED;
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
		printf(" ****** COULD NOT OPTIMIZE SYSTEM NETWORK PARAMETERS BECAUSE YOU ARE NOT RUNNING AS ROOT *******\n ****** YOUR MSDD6000 RECIEVER PERFORMANCE IS GOING TO BE TERRIBLE *******\n");
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


void MSDD6000::set_decim(int decim_pow2){
	DEBUG("SETTING NEW DECIM");
	d_decim = decim_pow2;

	if(d_state==STATE_STARTED)
		send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_decim, d_offset_hz);
}

void MSDD6000::set_rf_attn(int attn){
	DEBUG("SETTING NEW RF ATTN");
	d_rf_attn = attn;
	if(d_state==STATE_STARTED)
		send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_decim, d_offset_hz);
}

void MSDD6000::set_ddc_gain(int gain){
	DEBUG("SETTING NEW DDC GAIN");
	d_ddc_gain = gain;
	if(d_state==STATE_STARTED)
		send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_decim, d_offset_hz);
}

void MSDD6000::set_fc(int center_mhz, int offset_hz){
	DEBUG("SETTING NEW FC");
	d_fc_mhz = center_mhz;
	d_offset_hz = offset_hz;
	
	if(d_state==STATE_STARTED)
		send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_decim, d_offset_hz);
}


void MSDD6000::start(){
	send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, d_decim, d_offset_hz);
	d_state = STATE_STARTED;
	}


void MSDD6000::stop(){
	// new request with 0 decim tells it to halt
	send_request(d_fc_mhz, d_rf_attn, d_ddc_gain, 0, d_offset_hz);
	d_state = STATE_STOPPED;
	}


void MSDD6000::send_request(float freq_mhz, float rf_attn, float ddc_gain, float ddc_dec, float ddc_offset_hz){
	static char buff[512];
	sprintf(buff, "%f %f %f %f %f\n",freq_mhz, rf_attn, ddc_gain, ddc_dec, ddc_offset_hz);
	printf("sending: %s\n", buff);
        int flags = 0;
	sendto( d_sock, buff, strlen(buff)+1, flags,
		(const sockaddr*)&(d_detail->d_sockaddr), sizeof(d_detail->d_sockaddr));
	}


int  MSDD6000::read(char* buf, int size){
	int flags = 0;
	return recv(d_sock, buf, size, flags);
	}



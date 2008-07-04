#include "msdd6000.h"

#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

void optimize_socket(int socket);

MSDD6000::MSDD6000(char* addr){
	d_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	
	optimize_socket(d_sock);
	
	
	// set up remote sockaddr
//	int s = inet_aton(addr, &d_adx); 
	d_sockaddr.sin_family = AF_INET;
	d_sockaddr.sin_port = htons(10000);
	int s = inet_aton(addr, &d_sockaddr.sin_addr);
	
	// set up local sockaddr
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


void optimize_socket(int socket){
#define BANDWIDTH	1000000000/8
#define DELAY		0.5
	int ret;

	int sock_buf_size = 2*BANDWIDTH*DELAY;
	char textbuf[512];
	sprintf(textbuf, "%d", sock_buf_size);
	printf("sock_buf_size = %d\n", sock_buf_size);
	
	ret = setsockopt( socket, SOL_SOCKET, SO_SNDBUF,
                   (char *)&sock_buf_size, sizeof(sock_buf_size) );

	ret = setsockopt( socket, SOL_SOCKET, SO_RCVBUF,
                   (char *)&sock_buf_size, sizeof(sock_buf_size) );
	
	int uid = getuid();
	if(uid!=0){
		printf(" ****** COULD NOT OPTIMIZE SYSTEM NETWORK PARAMETERS BECAUSE YOU ARE NOT RUNNING AS ROOT *******\n ****** YOUR MSDD6000 RECIEVER PERFORMANCE IS GOING TO BE TERRIBLE *******\n");
		return;
	}


	// SET UP SOME SYSTEM WIDE TCP SOCKET PARAMETERS
	FILE* fd = fopen("/proc/sys/net/core/netdev_max_backlog", "w");
	fwrite("10000", 1, strlen("10000"), fd);
	fclose(fd);

	fd = fopen("/proc/sys/net/core/rmem_max", "w");
	fwrite(textbuf, 1, strlen(textbuf), fd);
	fclose(fd);

	fd = fopen("/proc/sys/net/core/wmem_max", "w");
	fwrite(textbuf, 1, strlen(textbuf), fd);
	fclose(fd);

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
	sendto( d_sock, buff, strlen(buff)+1, flags, (const sockaddr*)&d_sockaddr, sizeof(d_sockaddr));
	}


int  MSDD6000::read(char* buf, int size){
	int flags = 0;
	return recv(d_sock, buf, size, flags);
	}



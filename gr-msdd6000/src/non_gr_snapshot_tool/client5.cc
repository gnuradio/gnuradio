#include "msdd6000.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>


#define MSDD_HOST	"192.168.1.200"
int decim = 2;
float fc = 3500000000.0;
int gain = 1;

//long target_buf_size = 256*1024*1024; // 2 gig output when converted to floats
long target_buf_size = 1024*1024;
#define CHUNK_SIZE	(366*2)*sizeof(short)+6
#define OUTPUT_FILE	"CAPTURE_FILE.DAT"

char* buffer;
long int buffer_size;
sem_t buf_sem;
sem_t lock;

long int in_ptr;
long int out_ptr;
char** argvg;


int main(int argc, char* argv[]){
		
	int prio = getpriority(PRIO_PROCESS, getpid());
	printf("prio = %d\n", prio);

	// renice to -20
	setpriority(PRIO_PROCESS, getpid(), -20);

	prio = getpriority(PRIO_PROCESS, getpid());
	printf("new prio = %d\n", prio);

	
	argvg = argv;

	// instantiate our reciever instance
	MSDD6000 rcv((char*)MSDD_HOST);
	
	// set up desired rcv parameters
	
	int tune_mhz = long(fc)/1000000;
	int tune_hz = long(fc)%1000000;
	printf("mhz = %d   hz = %d\n", tune_mhz, tune_hz);
	
	rcv.set_decim(decim);
	rcv.set_fc(tune_mhz, tune_hz); // tune frequency in mhz, and ddc fine tuning in hz
	rcv.set_ddc_gain(gain); // careful, too much gain will cause bit-clipping (this simply chooses which 16 bits to map in 0=high order)
	rcv.set_rf_attn(10);  // adjusted variable attenuator in front of adc (0-32dB i think)
	
	// send start command
	rcv.start();
	
	// allocate our recieve buffer

	buffer_size = ((long)CHUNK_SIZE) * (target_buf_size/CHUNK_SIZE);

	printf("Allocating Intermediate Buffer.  %f MB\n", ((float)buffer_size)/(1024.0*1024));
	buffer = (char*)malloc(buffer_size);
	in_ptr = out_ptr = 0;
	printf("malloc returns %x.\n", buffer);
	
	while(in_ptr + CHUNK_SIZE < buffer_size){
		rcv.read( &buffer[in_ptr], CHUNK_SIZE );
		in_ptr+=CHUNK_SIZE;
		}
	
	printf("done.\n");
		
	int lastseq = -1;
	int discont = 0;

	float fbuf[366*2];

	FILE* fd = fopen(OUTPUT_FILE, "w");

	for(long long i=0;i<buffer_size;i += CHUNK_SIZE){
		int seq = *((int*) &buffer[i + 2] );
		if(seq == 0){
//			printf( "seq = %d, lastseq = %d at i = %d\n", seq, lastseq, i);
		}
		if(lastseq == -1){
			if(seq==0){
//				printf("found start... starting...\n");
				lastseq = seq;
			}
		} else {
			if(seq != lastseq+366){
//				printf("seq gap, %d -> %d\n", lastseq, seq);
				if(seq==0){
//					printf("stopping at i=%d\n", i);
					break;
					} else {
//					printf("this is a bad discontinuity!!! :{\n");
					discont++;
					}
			} else {
				// this is good data, and we are started
				for(int j = 0; j<366*2; j++){
					long ptr = i+6+j*2;
//					printf("%x %x \n", buffer[ptr], buffer[ptr+1]);
					short int sample_data = (*(( signed short*) &buffer[ptr]));
//					printf("%d\n", sample_data);
					fbuf[j] = (float) sample_data;
//					printf("%f\n", fbuf[j]);
					
				}	
				fwrite(fbuf, sizeof(float), 366*2, fd);
			}
			lastseq = seq;
		}
	}

	fclose(fd);
	
	printf("total discontinuities = %d\n", discont);
	
}


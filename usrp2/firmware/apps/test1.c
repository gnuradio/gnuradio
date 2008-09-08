#include "u2_init.h"
#include "memory_map.h"
#include "spi.h"
#include "hal_io.h"
#include "buffer_pool.h"
#include "pic.h"
#include "nonstdio.h"

// Globals
#define EMPTY 0
#define FILLING 1
#define FULL 2
#define EMPTYING 3

#define PORT 2    // ethernet = 2, serdes = 0
int dsp_rx_buf, dsp_tx_buf, serdes_rx_buf, serdes_tx_buf;
int dsp_rx_idle, dsp_tx_idle, serdes_rx_idle, serdes_tx_idle;

int buffer_state[4];


void double_buffering(int port);

//
// We register this in the secondary interrupt vector.
// It's called on buffer manager interrupts
//
void
buffer_irq_handler(unsigned irq)
{
  double_buffering(PORT);
}

int
main(void)
{
  int i;

  u2_init();

  // Control LEDs
  output_regs->leds = 0x02;

  // Turn on ADCs
  output_regs->adc_ctrl = 0x0A;

  // Set up TX Chain
  dsp_tx_regs->freq = 0;
  dsp_tx_regs->scale_iq = (1 << 16) | 1;
  dsp_tx_regs->interp_rate = 8;

  // Set up RX Chain
  dsp_rx_regs->freq = 0;
  dsp_rx_regs->scale_iq = (1 << 16) | 1;
  dsp_rx_regs->decim_rate = 8;

  // Set up buffer control, using only 4 for now
  for(i=0;i<4;i++) 
    buffer_state[i] = EMPTY;

  // Set up DSP RX
  buffer_state[0] = FILLING;
  serdes_tx_idle = 1;
  bp_receive_to_buf(0, 1, 1, 10, 509);  // DSP_RX to buffer 0, use 500 lines

  //dsp_rx_regs->run_rx = 1;           // Start DSP_RX
  putstr("Done DSP RX setup\n");

  // Set up serdes RX
  buffer_state[2] = FILLING;
  dsp_tx_idle = 1;
  bp_receive_to_buf(2, PORT, 1, 5, 504);

  while (buffer_pool_status->status == 0)  // wait for completion of DSP RX
    ;

  putstr("Done DSP TX setup\n");
  //dsp_tx_regs->run_tx = 1;

  // register interrupt handler
  pic_register_handler(IRQ_BUFFER, buffer_irq_handler);

  while (1)
    ;

  hal_finish();
  return 1;
}

void 
double_buffering(int port) {
  unsigned int localstatus = buffer_pool_status->status;

  if(localstatus & BPS_DONE_0) {
    bp_clear_buf(0);
    if(buffer_state[0] == FILLING) {
      buffer_state[0] = FULL;
      if(buffer_state[1] == EMPTY) {
	bp_receive_to_buf(1, 1, 1, 10, 509);  // DSP_RX to buffer 1, use 500 lines
	buffer_state[1] = FILLING;
      }
      else
	dsp_rx_idle = 1;
      if(serdes_tx_idle) {
	serdes_tx_idle = 0;
	bp_send_from_buf(0, port, 1, 10, 509);  // SERDES_TX from buffer 0
	buffer_state[0] = EMPTYING;
      }
    }
    else {  // buffer was emptying
      buffer_state[0] = EMPTY;
      if(dsp_rx_idle) {
	dsp_rx_idle = 0;
	bp_receive_to_buf(0, 1, 1, 10, 509);  // DSP_RX to buffer 0, use 500 lines
	buffer_state[0] = FILLING;
      }
      if(buffer_state[1] == FULL) {
	bp_send_from_buf(1, port, 1, 10, 509);  // SERDES_TX from buffer 1
	buffer_state[1] = EMPTYING;
      }
      else
	serdes_tx_idle = 1;
    }
    putstr("Int Proc'ed 0\n");
  }
  if(localstatus & BPS_DONE_1) {
    bp_clear_buf(1);
    if(buffer_state[1] == FILLING) {
      buffer_state[1] = FULL;
      if(buffer_state[0] == EMPTY) {
	bp_receive_to_buf(0, 1, 1, 10, 509);  // DSP_RX to buffer 1, use 500 lines
	buffer_state[0] = FILLING;
      }
      else
	dsp_rx_idle = 1;
      if(serdes_tx_idle) {
	serdes_tx_idle = 0;
	bp_send_from_buf(1, port, 1, 10, 509);  // SERDES_TX from buffer 1
	buffer_state[1] = EMPTYING;
      }
    }
    else {  // buffer was emptying
      buffer_state[1] = EMPTY;
      if(dsp_rx_idle) {
	dsp_rx_idle = 0;
	bp_receive_to_buf(1, 1, 1, 10, 509);  // DSP_RX to buffer 1, use 500 lines
	buffer_state[1] = FILLING;
      }
      if(buffer_state[0] == FULL) {
	bp_send_from_buf(0, port, 1, 10, 509);  // SERDES_TX from buffer 0
	buffer_state[0] = EMPTYING;
      }
      else
	serdes_tx_idle = 1;
    }
  putstr("Int Proc'ed 1\n");
  }
  if(localstatus & BPS_DONE_2) {
    bp_clear_buf(2);
    if(buffer_state[2] == FILLING) {
      buffer_state[2] = FULL;
      if(buffer_state[3] == EMPTY) {
	bp_receive_to_buf(3, port, 1, 5, 504);  // SERDES_RX to buffer 3, use 500 lines
	buffer_state[3] = FILLING;
      }
      else
	serdes_rx_idle = 1;
      if(dsp_tx_idle) {
	dsp_tx_idle = 0;
	bp_send_from_buf(2, 1, 1, 5, 504);  // DSP_TX from buffer 2
	buffer_state[2] = EMPTYING;
      }
    }
    else {  // buffer was emptying
      buffer_state[2] = EMPTY;
      if(serdes_rx_idle) {
	serdes_rx_idle = 0;
	bp_receive_to_buf(2, port, 1, 5, 504);  // SERDES_RX to buffer 2
	buffer_state[2] = FILLING;
      }
      if(buffer_state[3] == FULL) {
	bp_send_from_buf(3, 1, 1, 5, 504);  // DSP_TX from buffer 3
	buffer_state[3] = EMPTYING;
      }
      else
	dsp_tx_idle = 1;
    }
  putstr("Int Proc'ed 2\n");
  }
  if(localstatus & BPS_DONE_3) {
    bp_clear_buf(3);
    if(buffer_state[3] == FILLING) {
      buffer_state[3] = FULL;
      if(buffer_state[2] == EMPTY) {
	bp_receive_to_buf(2, port, 1, 5, 504);  // SERDES_RX to buffer 2, use 500 lines
	buffer_state[2] = FILLING;
      }
      else
	serdes_rx_idle = 1;
      if(dsp_tx_idle) {
	dsp_tx_idle = 0;
	bp_send_from_buf(3, 1, 1, 5, 504);  // DSP_TX from buffer 3
	buffer_state[3] = EMPTYING;
      }
    }
    else {  // buffer was emptying
      buffer_state[3] = EMPTY;
      if(serdes_rx_idle) {
	serdes_rx_idle = 0;
	bp_receive_to_buf(3, port, 1, 5, 504);  // SERDES_RX to buffer 3
	buffer_state[3] = FILLING;
      }
      if(buffer_state[2] == FULL) {
	bp_send_from_buf(2, 1, 1, 5, 504);  // DSP_TX from buffer 2
	buffer_state[2] = EMPTYING;
      }
      else
	dsp_tx_idle = 1;
    }
  putstr("Int Proc'ed 3\n");
  }
}

// Spare Code

#if 0  
  // Set up LSDAC
  int i = 0;
  while(1) {
    int command = (3 << 19) | (0 << 16) |  (i & 0xffff);
    spi_transact(SPI_TXONLY, SPI_SS_TX_DAC, command, 24, 1); // negate TX phase
    i++;
  }
#endif

#if 0  
  // Write to buffer 0
  int *buf = (int *)(BUFFER_BASE + BUFFER_0);
  puthex_nl((int)buf);

  for(i=0;i<BUFFER_SIZE;i++)
    buf[i] = i;

  putstr("Filled buffer 0\n");

  // Write to buffer 1
  buf = (int *)(BUFFER_BASE + BUFFER_1);
  puthex_nl((int)buf);
  for(i=0;i<BUFFER_SIZE;i++)
    buf[i] =  i + ((i^0xFFFF) << 16);

  putstr("Filled buffer 1\n");

#endif

#if 0
  // rx SERDES into buffer #2  (buf,port,step,fl,ll)
  bp_receive_to_buf(2, 0, 1, 10, 300);
  putstr("SERDES RX buffer setup\n");

  // send SERDES from buffer #0 (buf,port,step,fl,ll)
  bp_send_from_buf(0, 0, 1, 20, 200);
  putstr("SERDES TX buffer setup\n");

#endif

#if 0
  // send to DACs from buffer #1
  bp_send_from_buf(1 /*buf#*/, 1 /*port*/, 1 /*step*/, 20 /*fl*/, 250 /*ll*/);
  putstr("DAC Buffer setup\n");
#endif

#if 0
  //putstr("ENTER INT\n");
  for(i=0;i<8;i++)
    if(*status & (1<<i)) {
      //putstr("Clearing buf ");
      puthex_nl(i);
      bp_clear_buf(i);
    }
  //putstr("EXIT INT\n");
#endif

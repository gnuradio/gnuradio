#include "u2_init.h"
#include "memory_map.h"
#include "spi.h"
#include "hal_io.h"
#include "buffer_pool.h"
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

static void
wait_until_status_nonzero(void)
{
  while (buffer_pool_status->status == 0)
    ;
}

int
main(void)
{
  int i;
  
  u2_init();
  
  dsp_tx_regs->freq = 0;
  dsp_tx_regs->scale_iq = (1 << 16) | 1;
  dsp_tx_regs->interp_rate = 8;

  // Write data to be sent into the first buffer
  volatile unsigned int *buffer0 = buffer_ram(0);
  volatile unsigned int *buffer1 = buffer_ram(1);
  

  putstr("Starting to fill in RAM\n");
  for(i=0;i<512;i++) 
    buffer0[i] = i;
  putstr("Filled in RAM\n");
  
  buffer0[0] = 7; // start and end of buffer, send immediately
  buffer0[1] = 0x0000;  // start time
  bp_clear_buf(0);
  bp_send_from_buf(0, 1, 1, 0, 9);
  while (buffer_pool_status->status == 0)
    ;
  while(timer_regs -> time < 0x6000)
    {}

  buffer0[0] = 3; // start and end of buffer
  buffer0[1] = 0x8000;  // start time
  bp_clear_buf(0);
  bp_send_from_buf(0, 1, 1, 0, 9);
  while (buffer_pool_status->status == 0)
    ;
  while(timer_regs -> time < 0x8400)
    {}

  buffer0[0] = 3; // start and end of buffer
  buffer0[1] = 0x8800;  // start time
  bp_clear_buf(0);
  bp_send_from_buf(0, 1, 1, 0, 9);
  while (buffer_pool_status->status == 0)
    ;
  while(timer_regs -> time < 0x9000)
    {}

  buffer0[0] = 0x2;  // not last
  buffer0[1] = 0x9100;  // start time
  bp_clear_buf(0);
  bp_send_from_buf(0, 1, 1, 0, 9);
  while (buffer_pool_status->status == 0)
    ;
  buffer0[0] = 0x1;  // last
  buffer0[1] = 0x0000;  // start time
  bp_clear_buf(0);
  bp_send_from_buf(0, 1, 1, 0, 9);
  while (buffer_pool_status->status == 0)
    ;

  
  buffer0[0] = 0x3;  // first and last
  buffer0[1] = 0x8000;  // Time in the past
  bp_clear_buf(0);
  bp_send_from_buf(0, 1, 1, 0, 9);
  while (buffer_pool_status->status == 0)
    ;

  /*
  buffer0[0] = 0x2; // not last
  buffer0[1] = 0x9600;  // start time
  bp_clear_buf(0);
  bp_send_from_buf(0, 1, 1, 0, 9);
  while (buffer_pool_status->status == 0)
    ;
  */

  while(timer_regs -> time < 0xa000)
    {}

  putstr("Done\n");

  while(1)
    {}
  hal_finish();
  
  // Send a bunch, let them pile up in FIFO
  bp_send_from_buf(0, 2, 1, 21, 80);    wait_until_status_nonzero();
  bp_clear_buf(0);
  putstr("First add'l TX done\n");
  bp_send_from_buf(0, 2, 1, 81, 288);   wait_until_status_nonzero();
  bp_clear_buf(0);
  bp_send_from_buf(0, 2, 1, 289, 292);  wait_until_status_nonzero();
  bp_clear_buf(0);
  bp_send_from_buf(0, 2, 1, 293, 326);  wait_until_status_nonzero();
  bp_clear_buf(0);
  bp_send_from_buf(0, 2, 1, 327, 399);  wait_until_status_nonzero();
  bp_clear_buf(0);
  bp_send_from_buf(0, 2, 1, 400, 511);  wait_until_status_nonzero();
  bp_clear_buf(0);
  putstr("All add'l TX done\n");
  
  bp_receive_to_buf(1, 2, 1, 21, 80);   wait_until_status_nonzero();
  bp_clear_buf(1);
  putstr("First add'l RX done\n");
  bp_receive_to_buf(1, 2, 1, 81, 288);  wait_until_status_nonzero();
  bp_clear_buf(1);
  bp_receive_to_buf(1, 2, 1, 289, 292); wait_until_status_nonzero();
  bp_clear_buf(1);
  bp_receive_to_buf(1, 2, 1, 293, 326); wait_until_status_nonzero();
  bp_clear_buf(1);
  bp_receive_to_buf(1, 2, 1, 327, 399); wait_until_status_nonzero();
  bp_clear_buf(1);
  bp_receive_to_buf(1, 2, 1, 400, 511); wait_until_status_nonzero();
  bp_clear_buf(1);
  putstr("All add'l RX done\n");

  for(i=0;i<512;i++)
    if(buffer0[i] != buffer1[i]) {
      putstr("ERROR at location: ");
      puthex_nl(i);
      putstr("Value sent: ");
      puthex_nl(buffer0[i]);
      putstr("Value rcvd: ");
      puthex_nl(buffer1[i]);
      //break;
    }
  
  putstr("Done Testing\n");
  
  hal_finish();
  return 1;
}

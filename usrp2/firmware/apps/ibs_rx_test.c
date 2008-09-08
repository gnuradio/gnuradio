#include "u2_init.h"
#include "memory_map.h"
#include "spi.h"
#include "hal_io.h"
#include "buffer_pool.h"
#include "nonstdio.h"

#define PORT 2    // ethernet = 2, serdes = 0
int dsp_rx_buf, dsp_tx_buf, serdes_rx_buf, serdes_tx_buf;
int dsp_rx_idle, dsp_tx_idle, serdes_rx_idle, serdes_tx_idle;

int buffer_state[4];

static void __attribute__((unused))
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

  output_regs->adc_ctrl = 0x0A;

  dsp_rx_regs->freq = 0;
  dsp_rx_regs->scale_iq = (1 << 16) | 1;
  dsp_rx_regs->decim_rate = 8;

  volatile unsigned int *buffer0 = buffer_ram(0);
  volatile unsigned int *buffer1 = buffer_ram(1);
  volatile unsigned int *buffer2 = buffer_ram(2);
  
  putstr("Starting RX\n");
  bp_clear_buf(0);
  bp_receive_to_buf(0, 1, 1, 0, 99);

  dsp_rx_regs->rx_command = (50 << 9) | 100;   // Numlines, lines per frame
  dsp_rx_regs->rx_time = 0x2000;

  dsp_rx_regs->rx_command = (137 << 9) | 50;   // Numlines, lines per frame
  dsp_rx_regs->rx_time = 0x2200;

  while (buffer_pool_status->status == 0)
    ;
  bp_clear_buf(0);
  bp_clear_buf(1);
  bp_receive_to_buf(1, 1, 1, 0, 99);
  while (buffer_pool_status->status == 0)
    ;
  bp_clear_buf(2);
  bp_receive_to_buf(2, 1, 1, 0, 99);
  while (buffer_pool_status->status == 0)
    ;
  
  for(i=0;i<100;i++) {
    puthex(i);
    putstr("   ");
    puthex_nl(buffer0[i]);
  }
  for(i=0;i<60;i++) {
    puthex(i);
    putstr("   ");
    puthex_nl(buffer1[i]);
  }
  for(i=0;i<60;i++) {
    puthex(i);
    putstr("   ");
    puthex_nl(buffer2[i]);
  }
  //while(timer_regs -> time < 0x6000)
  //  {}

  putstr("Done\n");
  hal_finish();
  
  return 1;
}

#if 0
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
#endif

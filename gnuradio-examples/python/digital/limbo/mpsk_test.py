#!/usr/bin/env python

from gnuradio import gr, blks
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import random
import struct
from mpsk_pkt import *
import cmath

class my_graph(gr.flow_graph):

    def __init__(self, rx_callback, spb, excess_bw, SNR, freq_error, arity):
        gr.flow_graph.__init__(self)

        fg = self

        # Tuning Parameters
        gain_mu = 0.05/spb

        # transmitter
        self.packet_transmitter = \
          mpsk_mod_pkts(fg, spb=spb, excess_bw=excess_bw, diff=True, arity=arity)

        # ----------------------------------------------------------------
        #                       Channel model
        # ----------------------------------------------------------------

        awgn = gr.noise_source_c(gr.GR_GAUSSIAN, pow(10.0,-SNR/20.0))
        add = gr.add_cc()            # add some noise
        fg.connect(awgn, (add,1))
        radians = 17*pi/180
        phase_rotate = gr.multiply_const_cc(cmath.exp(radians * 1j))

        # ----------------------------------------------------------------

        # channel filter
        rx_filt_taps = gr.firdes.low_pass(1,spb,0.8,0.1,gr.firdes.WIN_HANN)
        #rx_filt_taps = (1,)
        rx_filt = gr.fir_filter_ccf(1,rx_filt_taps)

        # receiver
        self.packet_receiver = \
            mpsk_demod_pkts(fg, callback=rx_callback,
                            excess_bw=excess_bw, arity=arity, diff=True,
                            costas_alpha=.005, gain_mu=gain_mu, spb=spb)
        
        fg.connect (self.packet_transmitter, (add,0))
        fg.connect(add, phase_rotate, rx_filt, self.packet_receiver)


class stats(object):
    def __init__(self):
        self.npkts = 0
        self.nright = 0
        
def main():
    st = stats()
    
    def send_pkt(payload='', eof=False):
        fg.packet_transmitter.send_pkt(payload, eof)

    def rx_callback(ok, payload):
        st.npkts += 1
        if ok:
            st.nright += 1
        if len(payload) <= 16:
            print "ok = %5r  payload = '%s'  %d/%d" % (ok, payload, st.nright, st.npkts)
        else:
            (pktno,) = struct.unpack('!H', payload[0:2])
            print "ok = %5r  pktno = %4d  len(payload) = %4d  %d/%d" % (ok, pktno, len(payload),
                                                                        st.nright, st.npkts)


    parser = OptionParser (option_class=eng_option)
    parser.add_option("-M", "--megabytes", type="eng_float", default=1,
                      help="set megabytes to transmit [default=%default]")
    parser.add_option("-s", "--size", type="eng_float", default=1500,
                      help="set packet size [default=%default]")
    parser.add_option("","--spb", type=int, default=4,
                      help="set samples per baud to SPB [default=%default]")
    parser.add_option("", "--excess-bw", type="eng_float", default=0.4,
                      help="set excess bandwidth for RRC filter [default=%default]")
    parser.add_option("", "--snr", type="eng_float", default=40,
                      help="set SNR in dB for simulation [default=%default]")
    parser.add_option("", "--m-arity", type=int, default=4,
                      help="PSK arity [default=%default]")
    parser.add_option("", "--freq-error", type="eng_float", default=0,
                      help="set frequency error for simulation [default=%default]")

    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    pkt_size = int(options.size)

    fg = my_graph(rx_callback, options.spb, options.excess_bw, options.snr,
                  options.freq_error, options.m_arity)
    fg.start()

    nbytes = int(1e6 * options.megabytes)
    n = 0
    pktno = 0

    send_pkt('Hello World')

    # generate and send packets
    while n < nbytes:
        send_pkt(struct.pack('!H', pktno) + (pkt_size - 2) * chr(pktno & 0xff))
        n += pkt_size
        pktno += 1

    send_pkt('Goodbye World')
    send_pkt(eof=True)             # tell modulator we're not sending any more pkts

    fg.wait()


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass

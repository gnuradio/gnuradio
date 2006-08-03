#!/usr/bin/env python

from gnuradio import gr, blks
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import random
import struct

#import os
#print os.getpid()
#raw_input('Attach and press enter')


class my_graph(gr.flow_graph):

    def __init__(self, rx_callback, spb, bt, SNR, freq_error):
        gr.flow_graph.__init__(self)

        fg = self

        # Tuning Parameters
        gain_mu = 0.002*spb
        omega = spb*(1+freq_error)

        # transmitter
        self.packet_transmitter = blks.gmsk2_mod_pkts(fg, spb=spb, bt=bt)

        # add some noise
        add = gr.add_cc()
        noise = gr.noise_source_c(gr.GR_GAUSSIAN, pow(10.0,-SNR/20.0))

        # channel filter
        rx_filt_taps = gr.firdes.low_pass(1,spb,0.8,0.1,gr.firdes.WIN_HANN)
        rx_filt = gr.fir_filter_ccf(1,rx_filt_taps)

        # receiver
        self.packet_receiver = blks.gmsk2_demod_pkts(fg, callback=rx_callback,
                                                     spb=spb, gain_mu=gain_mu,
                                                     freq_error=freq_error, omega=omega)

        fg.connect (self.packet_transmitter, (add,0))
        fg.connect (noise, (add,1))
        fg.connect(add, rx_filt, self.packet_receiver)


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
    parser.add_option("", "--bt", type="eng_float", default=0.3,
                      help="set bandwidth time product for Gaussian filter [default=%default]")
    parser.add_option("", "--snr", type="eng_float", default=20,
                      help="set SNR in dB for simulation [default=%default]")
    parser.add_option("", "--freq-error", type="eng_float", default=0,
                      help="set frequency error for simulation [default=%default]")
    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    pkt_size = int(options.size)

    fg = my_graph(rx_callback, options.spb, options.bt, options.snr, options.freq_error)
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

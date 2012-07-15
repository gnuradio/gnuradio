#!/usr/bin/env python
"""
BER simulation for QPSK signals, compare to theoretical values.
Change the N_BITS value to simulate more bits per Eb/N0 value,
thus allowing to check for lower BER values.

Lower values will work faster, higher values will use a lot of RAM.
Also, this app isn't highly optimized--the flow graph is completely
reinstantiated for every Eb/N0 value.
Of course, expect the maximum value for BER to be one order of
magnitude below what you chose for N_BITS.
"""


import math
import numpy
from scipy.special import erfc
import pylab
from gnuradio import gr, digital

# Best to choose powers of 10
N_BITS = 1e7
RAND_SEED = 42

def berawgn(EbN0):
    """ Calculates theoretical bit error rate in AWGN (for BPSK and given Eb/N0) """
    return 0.5 * erfc(math.sqrt(10**(float(EbN0)/10)))

class BitErrors(gr.hier_block2):
    """ Two inputs: true and received bits. We compare them and
    add up the number of incorrect bits. Because integrate_ff()
    can only add up a certain number of values, the output is
    not a scalar, but a sequence of values, the sum of which is
    the BER. """
    def __init__(self, bits_per_byte):
        gr.hier_block2.__init__(self, "BitErrors",
                gr.io_signature(2, 2, gr.sizeof_char),
                gr.io_signature(1, 1, gr.sizeof_int))

        # Bit comparison
        comp = gr.xor_bb()
        intdump_decim = 100000
        if N_BITS < intdump_decim:
            intdump_decim = int(N_BITS)
        self.connect(self,
                     comp,
                     gr.unpack_k_bits_bb(bits_per_byte),
                     gr.uchar_to_float(),
                     gr.integrate_ff(intdump_decim),
                     gr.multiply_const_ff(1.0/N_BITS),
                     self)
        self.connect((self, 1), (comp, 1))

class BERAWGNSimu(gr.top_block):
    " This contains the simulation flow graph "
    def __init__(self, EbN0):
        gr.top_block.__init__(self)
        self.const = digital.qpsk_constellation()
        # Source is N_BITS bits, non-repeated
        data = map(int, numpy.random.randint(0, self.const.arity(), N_BITS/self.const.bits_per_symbol()))
        src   = gr.vector_source_b(data, False)
        mod   = gr.chunks_to_symbols_bc((self.const.points()), 1)
        add   = gr.add_vcc()
        noise = gr.noise_source_c(gr.GR_GAUSSIAN,
                                  self.EbN0_to_noise_voltage(EbN0),
                                  RAND_SEED)
        demod = digital.constellation_decoder_cb(self.const.base())
        ber   = BitErrors(self.const.bits_per_symbol())
        self.sink  = gr.vector_sink_f()
        self.connect(src, mod, add, demod, ber, self.sink)
        self.connect(noise, (add, 1))
        self.connect(src, (ber, 1))

    def EbN0_to_noise_voltage(self, EbN0):
        """ Converts Eb/N0 to a single-sided noise voltage (assuming unit symbol power) """
        return 1.0 / math.sqrt(2.0 * self.const.bits_per_symbol() * 10**(float(EbN0)/10))


def simulate_ber(EbN0):
    """ All the work's done here: create flow graph, run, read out BER """
    print "Eb/N0 = %d dB" % EbN0
    fg = BERAWGNSimu(EbN0)
    fg.run()
    return numpy.sum(fg.sink.data())

if __name__ == "__main__":
    EbN0_min = 0
    EbN0_max = 15
    EbN0_range = range(EbN0_min, EbN0_max+1)
    ber_theory = [berawgn(x)      for x in EbN0_range]
    print "Simulating..."
    ber_simu   = [simulate_ber(x) for x in EbN0_range]

    f = pylab.figure()
    s = f.add_subplot(1,1,1)
    s.semilogy(EbN0_range, ber_theory, 'g-.', label="Theoretical")
    s.semilogy(EbN0_range, ber_simu, 'b-o', label="Simulated")
    s.set_title('BER Simulation')
    s.set_xlabel('Eb/N0 (dB)')
    s.set_ylabel('BER')
    s.legend()
    s.grid()
    pylab.show()


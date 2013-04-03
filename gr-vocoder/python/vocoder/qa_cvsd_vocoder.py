#!/usr/bin/env python
#
# Copyright 2007,2010,2011,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest, vocoder, blocks, filter
from gnuradio.vocoder import cvsd

class test_cvsd_vocoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown (self):
        self.tb = None

    def test001_module_load (self):
        raw_enc = vocoder.cvsd_encode_sb()
        raw_dec = vocoder.cvsd_decode_bs()
        hb_enc = cvsd.cvsd_encode_fb()
        hb_dec = cvsd.cvsd_decode_bf()


    """ Disable for now
    def test01(self):
        sample_rate = 8000
        scale_factor = 32000

        expected_data = (6.9670547250243192e-21, -2.4088578356895596e-05,
                         -5.1261918997624889e-05, 7.2410854045301676e-05,
                         8.444241393590346e-05, -1.2537107068055775e-05,
                         0.00024186755763366818, -0.00060463894624263048,
                         0.00064864184241741896, 0.010409165173768997,
                         0.0087582804262638092, 0.017965050414204597,
                         0.010722399689257145, 0.006602009292691946,
                         0.02213001623749733, 0.0079685859382152557,
                         0.033707316964864731, 0.027021972462534904,
                         0.0086071854457259178, 0.0081678871065378189,
                         0.039343506097793579, 0.030671956017613411,
                         0.029626710340380669, 0.020126519724726677,
                         0.023636780679225922, 0.0064640454947948456,
                         -0.0038861562497913837, 0.0021134600974619389,
                         -0.0088051930069923401, -0.00023228264763019979,
                         -0.033737499266862869, -0.033141419291496277,
                         -0.037145044654607773, -0.0080892946571111679,
                         -0.077117636799812317, -0.078382067382335663,
                         -0.055503919720649719, -0.019355267286300659,
                         -0.022441385313868523, -0.073706060647964478,
                         -0.054677654057741165, -0.047119375318288803,
                         -0.044418536126613617, -0.036084383726119995,
                         -0.0206278245896101, -0.031200021505355835,
                         -0.0004070434661116451, 0.0006594572332687676,
                         -0.016584658995270729, 0.07387717068195343,
                         -0.0063191778026521206, 0.051200628280639648,
                         -0.029480356723070145, 0.05176771804690361,
                         0.038578659296035767, 0.026550088077783585,
                         0.067103870213031769, 0.001888439292088151,
                         0.28141644597053528, 0.49543789029121399,
                         0.6626054048538208, 0.79180729389190674,
                         0.89210402965545654, 0.96999943256378174,
                         1.0261462926864624, 1.0267977714538574,
                         1.0251555442810059, 1.0265737771987915,
                         1.0278496742248535, 1.0208886861801147,
                         1.0325057506561279, 0.91415292024612427,
                         0.83941859006881714, 0.67373806238174438,
                         0.51683622598648071, 0.38949671387672424,
                         0.16016888618469238, 0.049505095928907394,
                         -0.16699212789535522, -0.26886492967605591,
                         -0.49256673455238342, -0.59178370237350464,
                         -0.73317724466323853, -0.78922677040100098,
                         -0.88782668113708496, -0.96708977222442627,
                         -0.96490746736526489, -0.94962418079376221,
                         -0.94716215133666992, -0.93755108118057251,
                         -0.84852480888366699, -0.80485564470291138,
                         -0.69762390851974487, -0.58398681879043579,
                         -0.45891636610031128, -0.29681697487831116,
                         -0.16035343706607819, 0.014823081903159618,
                         0.16282452642917633, 0.33802291750907898)

        # WARNING: not importing analog in this QA code.
        # If we enable this, we can probably just create a sin with numpy.
        src = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE, 200, 1, 0)
        head = blocks.head(gr.sizeof_float, 100)
        src_scale = blocks.multiply_const_ff(scale_factor)

        interp = filter.rational_resampler_fff(8, 1)
        f2s = blocks.float_to_short()

        enc = vocoder.cvsd_vocoder.encode_sb()
        dec = vocoder.cvsd_vocoder.decode_bs()

        s2f = blocks.short_to_float()
        decim = filter.rational_resampler_fff(1, 8)

        sink_scale = blocks.multiply_const_ff(1.0/scale_factor)
        sink = blocks.vector_sink_f()

        self.tb.connect(src, src_scale, interp, f2s, enc)
        self.tb.connect(enc, dec, s2f, decim, sink_scale, head, sink)
        self.tb.run()
	print sink.data()

        self.assertFloatTuplesAlmostEqual (expected_data, sink.data(), 5)
    """

if __name__ == '__main__':
    gr_unittest.run(test_cvsd_vocoder, "test_cvsd_vocoder.xml")

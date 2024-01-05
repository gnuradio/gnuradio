#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, analog, blocks


class test_agc(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_sets(self):
        agc = analog.agc_cc(1e-3, 1, 1)

        agc.set_rate(1)
        agc.set_reference(1.1)
        agc.set_gain(1.1)
        agc.set_max_gain(100)

        self.assertAlmostEqual(agc.rate(), 1)
        self.assertAlmostEqual(agc.reference(), 1.1)
        self.assertAlmostEqual(agc.gain(), 1.1)
        self.assertAlmostEqual(agc.max_gain(), 100)

    def test_001(self):
        ''' Test the complex AGC loop (single rate input) '''
        tb = self.tb

        expected_result = (
            (100 + 0j),
            (72.89209747314453 + 52.9592170715332j),
            (25.089027404785156 + 77.2160873413086j),
            (-22.611034393310547 + 69.58960723876953j),
            (-53.35764694213867 + 38.766597747802734j),
            (-59.4586067199707 - 2.7399494229030097e-06j),
            (-43.3734245300293 - 31.51263999938965j),
            (-14.941386222839355 - 45.984867095947266j),
            (13.478157997131348 - 41.48149490356445j),
            (31.838510513305664 - 23.13202476501465j),
            (35.51927947998047 + 3.3255341804760974e-06j),
            (25.94291114807129 + 18.848634719848633j),
            (8.949296951293945 + 27.543113708496094j),
            (-8.085277557373047 + 24.883914947509766j),
            (-19.13165283203125 + 13.899954795837402j),
            (-21.383323669433594 - 2.987417019539862e-06j),
            (-15.65035343170166 - 11.370650291442871j),
            (-5.4110236167907715 - 16.653427124023438j),
            (4.900828838348389 - 15.083191871643066j),
            (11.62836742401123 - 8.448498725891113j),
            (13.036169052124023 + 2.4410530841123546e-06j),
            (9.572690963745117 + 6.954970359802246j),
            (3.3217051029205322 + 10.223164558410645j),
            (-3.0204410552978516 + 9.295955657958984j),
            (-7.197745323181152 + 5.229465007781982j),
            (-8.107251167297363 - 1.8916969111160142e-06j),
            (-5.983887195587158 - 4.347550392150879j),
            (-2.087981939315796 - 6.426152229309082j),
            (1.9100888967514038 - 5.87864351272583j),
            (4.581503391265869 - 3.3286550045013428j),
            (5.196768760681152 + 1.4596606661143596e-06j),
            (3.864729881286621 + 2.807892322540283j),
            (1.359479308128357 + 4.184051513671875j),
            (-1.2544355392456055 + 3.8607518672943115j),
            (-3.036635398864746 + 2.2062432765960693j),
            (-3.4781548976898193 - 1.137218077928992e-06j),
            (-2.613386869430542 - 1.8987380266189575j),
            (-0.9293051958084106 - 2.8601105213165283j),
            (0.8672783374786377 - 2.6692051887512207j),
            (2.1244049072265625 - 1.5434693098068237j),
            (2.463329315185547 + 9.225283861269418e-07j),
            (1.8744803667068481 + 1.3618910312652588j),
            (0.6752913594245911 + 2.078335762023926j),
            (-0.6386655569076538 + 1.9656078815460205j),
            (-1.5857415199279785 + 1.1521075963974j),
            (-1.864084243774414 - 7.840082503207668e-07j),
            (-1.438162922859192 - 1.0448874235153198j),
            (-0.5252984762191772 - 1.6167048215866089j),
            (0.5036717653274536 - 1.5501397848129272j),
            (1.2676655054092407 - 0.9210119843482971j))

        sampling_freq = 100
        src1 = analog.sig_source_c(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100.0)
        dst1 = blocks.vector_sink_c()
        head = blocks.head(gr.sizeof_gr_complex, int(5 * sampling_freq * 0.10))

        agc = analog.agc_cc(1e-3, 1, 1)

        tb.connect(src1, head)
        tb.connect(head, agc)
        tb.connect(agc, dst1)

        tb.run()
        dst_data = dst1.data()
        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 4)

    def test_002_sets(self):
        agc = analog.agc_ff(1e-3, 1, 1)

        agc.set_rate(1)
        agc.set_reference(1.1)
        agc.set_gain(1.1)
        agc.set_max_gain(100)

        self.assertAlmostEqual(agc.rate(), 1)
        self.assertAlmostEqual(agc.reference(), 1.1)
        self.assertAlmostEqual(agc.gain(), 1.1)
        self.assertAlmostEqual(agc.max_gain(), 100)

    def test_002(self):
        ''' Test the floating point AGC loop (single rate input) '''
        tb = self.tb

        expected_result = (
            0.0,
            58.83704376220703,
            89.69985961914062,
            81.26403045654297,
            45.50606918334961,
            -3.3625440210016677e-06,
            -42.9488639831543,
            -65.50326538085938,
            -59.368656158447266,
            -33.26097869873047,
            4.995997642254224e-06,
            31.423521041870117,
            47.950958251953125,
            43.48566436767578,
            24.37834358215332,
            -5.4677821026416495e-06,
            -23.06298828125,
            -35.21844482421875,
            -31.964082717895508,
            -17.93484115600586,
            5.396469077822985e-06,
            16.998228073120117,
            25.982229232788086,
            23.60628318786621,
            13.260700225830078,
            -4.97806149724056e-06,
            -12.598825454711914,
            -19.282241821289062,
            -17.543500900268555,
            -9.870061874389648,
            4.467380676942412e-06,
            9.407480239868164,
            14.422045707702637,
            13.14553451538086,
            7.410478591918945,
            -3.91256025977782e-06,
            -7.092466354370117,
            -10.896439552307129,
            -9.955231666564941,
            -5.62628698348999,
            3.411524403418298e-06,
            5.413146018981934,
            8.338950157165527,
            7.640974521636963,
            4.332027435302734,
            -2.95963241114805e-06,
            -4.19495964050293,
            -6.483736991882324,
            -5.962202072143555,
            -3.3931667804718018)

        sampling_freq = 100
        src1 = analog.sig_source_f(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100.0)
        dst1 = blocks.vector_sink_f()
        head = blocks.head(gr.sizeof_float, int(5 * sampling_freq * 0.10))

        agc = analog.agc_ff(1e-3, 1, 1)

        tb.connect(src1, head)
        tb.connect(head, agc)
        tb.connect(agc, dst1)

        tb.run()
        dst_data = dst1.data()
        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 4)

    def test_003_sets(self):
        agc = analog.agc2_cc(1e-3, 1e-1, 1, 1)

        agc.set_attack_rate(1)
        agc.set_decay_rate(2)
        agc.set_reference(1.1)
        agc.set_gain(1.1)
        agc.set_max_gain(100)

        self.assertAlmostEqual(agc.attack_rate(), 1)
        self.assertAlmostEqual(agc.decay_rate(), 2)
        self.assertAlmostEqual(agc.reference(), 1.1)
        self.assertAlmostEqual(agc.gain(), 1.1)
        self.assertAlmostEqual(agc.max_gain(), 100)

    def test_003(self):
        ''' Test the complex AGC loop (attack and decay rate inputs) '''
        tb = self.tb

        expected_result = \
            ((100 + 0j),
             (0.8090173602104187 + 0.5877856016159058j),
             (0.3090175688266754 + 0.9510582685470581j),
             (-0.309017539024353 + 0.9510582089424133j),
             (-0.8090170621871948 + 0.5877852439880371j),
             (-1.000004529953003 - 4.608183701293456e-08j),
             (-0.8090165853500366 - 0.587785005569458j),
             (-0.3090173006057739 - 0.9510576725006104j),
             (0.3090173900127411 - 0.951057493686676j),
             (0.8090166449546814 - 0.5877848863601685j),
             (1.0000040531158447 + 9.362654651567937e-08j),
             (0.809016227722168 + 0.5877848267555237j),
             (0.3090171217918396 + 0.9510573148727417j),
             (-0.3090173006057739 + 0.9510571360588074j),
             (-0.8090163469314575 + 0.5877846479415894j),
             (-1.000003695487976 - 1.39708305368913e-07j),
             (-0.8090159296989441 - 0.5877846479415894j),
             (-0.30901697278022766 - 0.951056957244873j),
             (0.30901727080345154 - 0.9510568976402283j),
             (0.809016227722168 - 0.5877844095230103j),
             (1.000003457069397 + 1.87252979344521e-07j),
             (0.809015691280365 + 0.5877845287322998j),
             (0.3090168535709381 + 0.9510567784309387j),
             (-0.30901727080345154 + 0.951056718826294j),
             (-0.8090161085128784 + 0.5877842903137207j),
             (-1.0000033378601074 - 2.3333473109232727e-07j),
             (-0.8090156316757202 - 0.5877845287322998j),
             (-0.3090168237686157 - 0.9510566592216492j),
             (0.3090173006057739 - 0.9510565400123596j),
             (0.8090160489082336 - 0.5877842307090759j),
             (1.0000032186508179 + 2.8087941927879e-07j),
             (0.8090155124664307 + 0.5877845287322998j),
             (0.30901676416397095 + 0.9510567784309387j),
             (-0.3090173006057739 + 0.9510565400123596j),
             (-0.8090160489082336 + 0.5877841711044312j),
             (-1.0000033378601074 - 3.2696124208086985e-07j),
             (-0.8090155124664307 - 0.5877845883369446j),
             (-0.30901673436164856 - 0.9510567784309387j),
             (0.3090173602104187 - 0.9510565400123596j),
             (0.8090160489082336 - 0.5877841114997864j),
             (1.0000033378601074 + 3.745059302673326e-07j),
             (0.8090154528617859 + 0.5877846479415894j),
             (0.3090166747570038 + 0.9510567784309387j),
             (-0.3090174198150635 + 0.9510565400123596j),
             (-0.8090161681175232 + 0.5877841114997864j),
             (-1.0000032186508179 - 4.2058766780428414e-07j),
             (-0.8090154528617859 - 0.5877846479415894j),
             (-0.309016615152359 - 0.9510567784309387j),
             (0.30901747941970825 - 0.9510564804077148j),
             (0.8090161681175232 - 0.5877840518951416j))

        sampling_freq = 100
        src1 = analog.sig_source_c(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100)
        dst1 = blocks.vector_sink_c()
        head = blocks.head(gr.sizeof_gr_complex, int(5 * sampling_freq * 0.10))

        agc = analog.agc2_cc(1e-2, 1e-3, 1, 1)

        tb.connect(src1, head)
        tb.connect(head, agc)
        tb.connect(agc, dst1)

        tb.run()
        dst_data = dst1.data()
        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 4)

    def test_004_sets(self):
        agc = analog.agc2_ff(1e-3, 1e-1, 1, 1)

        agc.set_attack_rate(1)
        agc.set_decay_rate(2)
        agc.set_reference(1.1)
        agc.set_gain(1.1)
        agc.set_max_gain(100)

        self.assertAlmostEqual(agc.attack_rate(), 1)
        self.assertAlmostEqual(agc.decay_rate(), 2)
        self.assertAlmostEqual(agc.reference(), 1.1)
        self.assertAlmostEqual(agc.gain(), 1.1)
        self.assertAlmostEqual(agc.max_gain(), 100)

    def test_004(self):
        ''' Test the floating point AGC loop (attack and decay rate inputs) '''
        tb = self.tb

        expected_result = \
            (0.0,
             58.83704376220703,
             40.194339752197266,
             2.9184224605560303,
             0.6760660409927368,
             -6.79303795436681e-08,
             -1.4542515277862549,
             -1.9210143089294434,
             -1.0450801849365234,
             -0.6193966865539551,
             1.3429632872430375e-07,
             1.4308913946151733,
             1.9054334163665771,
             1.044317603111267,
             0.619373619556427,
             -2.003930177352231e-07,
             -1.4308818578720093,
             -1.905427098274231,
             -1.0443172454833984,
             -0.6193735599517822,
             2.6858961632569844e-07,
             1.4308820962905884,
             1.9054267406463623,
             1.0443172454833984,
             0.6193734407424927,
             -3.3468785431978176e-07,
             -1.4308820962905884,
             -1.9054267406463623,
             -1.0443171262741089,
             -0.6193735599517822,
             4.0288449554282124e-07,
             1.430882215499878,
             1.905427098274231,
             1.0443170070648193,
             0.6193734407424927,
             -4.689827903803234e-07,
             -1.430882453918457,
             -1.9054268598556519,
             -1.0443170070648193,
             -0.6193733811378479,
             5.371793463382346e-07,
             1.4308825731277466,
             1.9054265022277832,
             1.0443170070648193,
             0.6193733811378479,
             -6.032776127540274e-07,
             -1.4308825731277466,
             -1.9054265022277832,
             -1.0443168878555298,
             -0.6193733811378479)

        sampling_freq = 100
        src1 = analog.sig_source_f(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100)
        dst1 = blocks.vector_sink_f()
        head = blocks.head(gr.sizeof_float, int(5 * sampling_freq * 0.10))

        agc = analog.agc2_ff(1e-2, 1e-3, 1, 1)

        tb.connect(src1, head)
        tb.connect(head, agc)
        tb.connect(agc, dst1)

        tb.run()
        dst_data = dst1.data()
        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 4)

    def test_005(self):
        ''' Test the complex AGC loop (attack and decay rate inputs) '''
        tb = self.tb

        expected_result = \
            ((100 + 0j),
             (0.8090173602104187 + 0.5877856016159058j),
             (0.3090175688266754 + 0.9510582685470581j),
             (-0.309017539024353 + 0.9510582089424133j),
             (-0.8090170621871948 + 0.5877852439880371j),
             (-1.000004529953003 - 4.608183701293456e-08j),
             (-0.8090165853500366 - 0.587785005569458j),
             (-0.3090173006057739 - 0.9510576725006104j),
             (0.3090173900127411 - 0.951057493686676j),
             (0.8090166449546814 - 0.5877848863601685j),
             (1.0000040531158447 + 9.362654651567937e-08j),
             (0.809016227722168 + 0.5877848267555237j),
             (0.3090171217918396 + 0.9510573148727417j),
             (-0.3090173006057739 + 0.9510571360588074j),
             (-0.8090163469314575 + 0.5877846479415894j),
             (-1.000003695487976 - 1.39708305368913e-07j),
             (-0.8090159296989441 - 0.5877846479415894j),
             (-0.30901697278022766 - 0.951056957244873j),
             (0.30901727080345154 - 0.9510568976402283j),
             (0.809016227722168 - 0.5877844095230103j),
             (1.000003457069397 + 1.87252979344521e-07j),
             (0.809015691280365 + 0.5877845287322998j),
             (0.3090168535709381 + 0.9510567784309387j),
             (-0.30901727080345154 + 0.951056718826294j),
             (-0.8090161085128784 + 0.5877842903137207j),
             (-1.0000033378601074 - 2.3333473109232727e-07j),
             (-0.8090156316757202 - 0.5877845287322998j),
             (-0.3090168237686157 - 0.9510566592216492j),
             (0.3090173006057739 - 0.9510565400123596j),
             (0.8090160489082336 - 0.5877842307090759j),
             (1.0000032186508179 + 2.8087941927879e-07j),
             (0.8090155124664307 + 0.5877845287322998j),
             (0.30901676416397095 + 0.9510567784309387j),
             (-0.3090173006057739 + 0.9510565400123596j),
             (-0.8090160489082336 + 0.5877841711044312j),
             (-1.0000033378601074 - 3.2696124208086985e-07j),
             (-0.8090155124664307 - 0.5877845883369446j),
             (-0.30901673436164856 - 0.9510567784309387j),
             (0.3090173602104187 - 0.9510565400123596j),
             (0.8090160489082336 - 0.5877841114997864j),
             (1.0000033378601074 + 3.745059302673326e-07j),
             (0.8090154528617859 + 0.5877846479415894j),
             (0.3090166747570038 + 0.9510567784309387j),
             (-0.3090174198150635 + 0.9510565400123596j),
             (-0.8090161681175232 + 0.5877841114997864j),
             (-1.0000032186508179 - 4.2058766780428414e-07j),
             (-0.8090154528617859 - 0.5877846479415894j),
             (-0.309016615152359 - 0.9510567784309387j),
             (0.30901747941970825 - 0.9510564804077148j),
             (0.8090161681175232 - 0.5877840518951416j))

        sampling_freq = 100
        src1 = analog.sig_source_c(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100)
        dst1 = blocks.vector_sink_c()
        head = blocks.head(gr.sizeof_gr_complex, int(5 * sampling_freq * 0.10))

        agc = analog.agc2_cc(1e-2, 1e-3, 1, 1)

        tb.connect(src1, head)
        tb.connect(head, agc)
        tb.connect(agc, dst1)

        tb.run()
        dst_data = dst1.data()
        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 4)

    def test_006_000_agc3_setters(self):
        agc = analog.agc3_cc(1e-3, 1e-1, 1)

        agc.set_attack_rate(1)
        agc.set_decay_rate(2)
        agc.set_reference(1.1)
        agc.set_gain(1.1)

        self.assertAlmostEqual(agc.attack_rate(), 1)
        self.assertAlmostEqual(agc.decay_rate(), 2)
        self.assertAlmostEqual(agc.reference(), 1.1)
        self.assertAlmostEqual(agc.gain(), 1.1)

    def test_006_001_agc3(self, stride=1):
        ''' Test the complex AGC loop agc3 (attack and decay rate inputs) '''
        tb = self.tb

        sampling_freq = 100
        # N must by a multiple of the volk_alignment of the system for this test to work.
        # For a machine with 512-bit registers, that would be 8 complex-floats.
        N = int(8 * sampling_freq)
        src1 = analog.sig_source_c(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100)
        dst1 = blocks.vector_sink_c()
        head = blocks.head(gr.sizeof_gr_complex, N)

        ref = 1
        # attack_rate, decay_rate, reference, initial gain, stride
        agc = analog.agc3_cc(1e-2, 1e-3, ref, ref, stride)

        tb.connect(src1, agc)
        tb.connect(agc, head)
        tb.connect(head, dst1)

        tb.run()
        dst_data = dst1.data()
        self.assertEqual(len(dst_data), N, "unexpected data length")
        result = [abs(x) for x in dst_data]
        for idx, x in enumerate(result):
            self.assertAlmostEqual(x, ref, 4,
                                   f"failed at pos {idx} (stride = {stride})")

    def test_006_002_agc3_striding(self):
        for spacing in (2, 4, 5, 17, 233):
            self.test_006_001_agc3(stride=spacing)

    def test_006_003_agc3_setter_errors(self):
        default_src = analog.agc3_cc()
        settables = ("attack_rate", "decay_rate", "reference", "gain",
                     "max_gain")
        troublemakers = (getattr(default_src, f"set_{prop}")
                         for prop in settables)
        for troublemaker in troublemakers:
            with self.assertRaises(ValueError):
                troublemaker(-3.0)

    def test_006_004_agc3(self, stride=1):
        '''This test is performed employing an AM signal.
        AGC should throw out the recovered carrier without any effect
        of modulation in order to pass the test'''
        tb = self.tb
        samp_rate = 1e4
        carrier_freq = 200
        envelope_freq = 5
        N = samp_rate / envelope_freq  # period
        ref = 1.0
        m = 0.99

        mul = blocks.multiply_cc(1)
        carrier_src = analog.sig_source_c(
            samp_rate, analog.GR_COS_WAVE, carrier_freq, 1.0, 0.0, 0)
        envelope_src = analog.sig_source_c(
            samp_rate, analog.GR_COS_WAVE, envelope_freq, m, 1, 0)
        agc3 = analog.agc3_cc((0.8), (0.7), ref, 1.0, stride)
        skip = blocks.skiphead(gr.sizeof_gr_complex, int(N))
        head = blocks.head(gr.sizeof_gr_complex, int(N))
        dst = blocks.vector_sink_c()

        tb.connect((carrier_src, 0), (mul, 0))
        tb.connect((envelope_src, 0), (mul, 1))
        tb.connect((mul, 0), (agc3, 0))
        tb.connect((agc3, 0), (skip, 0))
        tb.connect((skip, 0), (head, 0))
        tb.connect((head, 0), (dst, 0))

        tb.run()
        dst_data = dst.data()
        self.assertEqual(len(dst_data), N, "unexpected data length")
        result = [abs(x) for x in dst_data]
        for idx, x in enumerate(result):
            self.assertAlmostEqual(x, ref, None,
                                   f"failed at pos {idx} (stride = {stride})", 0.1)

    def test_007_agc3_constructor_arguments(self):
        attack_rate = 1.1e-3
        decay_rate = 1.2e-4
        reference = 1.3
        gain = 1.4
        iir_update_decim = 2
        max_gain = 1.5e4

        agc3 = analog.agc3_cc(attack_rate, decay_rate, reference, gain, iir_update_decim, max_gain)

        self.assertAlmostEqual(agc3.attack_rate(), attack_rate)
        self.assertAlmostEqual(agc3.decay_rate(), decay_rate)
        self.assertAlmostEqual(agc3.reference(), reference)
        self.assertAlmostEqual(agc3.gain(), gain)
        self.assertAlmostEqual(agc3.max_gain(), max_gain)

    def test_100(self):
        ''' Test complex feedforward agc with constant input '''

        length = 8
        gain = 2

        input_data = 8 * (0.0,) + 24 * (1.0,) + 24 * (0.0,)
        expected_result = (8 + length - 1) * (0.0,) + 24 * (gain * 1.0,) + (0,)

        src = blocks.vector_source_c(input_data)
        agc = analog.feedforward_agc_cc(8, 2.0)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, agc, dst)

        self.tb.run()
        dst_data = dst.data()[0:len(expected_result)]

        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 4)


if __name__ == '__main__':
    gr_unittest.run(test_agc)

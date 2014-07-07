#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2012,2013 Free Software Foundation, Inc.
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
            (100.000244140625+7.2191943445432116e-07j),
            (72.892257690429688+52.959323883056641j),
            (25.089065551757812+77.216217041015625j),
            (-22.611061096191406+69.589706420898438j),
            (-53.357715606689453+38.766635894775391j),
            (-59.458671569824219+3.4792964243024471e-07j),
            (-43.373462677001953-31.512666702270508j),
            (-14.94139289855957-45.984889984130859j),
            (13.478158950805664-41.48150634765625j),
            (31.838506698608398-23.132022857666016j),
            (35.519271850585938-3.1176801940091536e-07j),
            (25.942903518676758+18.848621368408203j),
            (8.9492912292480469+27.5430908203125j),
            (-8.0852642059326172+24.883890151977539j),
            (-19.131628036499023+13.899936676025391j),
            (-21.383295059204102+3.1281737733479531e-07j),
            (-15.650330543518066-11.370632171630859j),
            (-5.4110145568847656-16.65339469909668j),
            (4.9008159637451172-15.083160400390625j),
            (11.628337860107422-8.4484796524047852j),
            (13.036135673522949-2.288476110834381e-07j),
            (9.5726661682128906+6.954948902130127j),
            (3.3216962814331055+10.223132133483887j),
            (-3.0204284191131592+9.2959251403808594j),
            (-7.1977195739746094+5.2294478416442871j),
            (-8.1072216033935547+1.8976157889483147e-07j),
            (-5.9838657379150391-4.3475332260131836j),
            (-2.0879747867584229-6.4261269569396973j),
            (1.9100792407989502-5.8786196708679199j),
            (4.5814824104309082-3.3286411762237549j),
            (5.1967458724975586-1.3684227440080576e-07j),
            (3.8647139072418213+2.8078789710998535j),
            (1.3594740629196167+4.1840314865112305j),
            (-1.2544282674789429+3.8607344627380371j),
            (-3.0366206169128418+2.2062335014343262j),
            (-3.4781389236450195+1.1194014604143376e-07j),
            (-2.6133756637573242-1.8987287282943726j),
            (-0.9293016791343689-2.8600969314575195j),
            (0.86727333068847656-2.6691930294036865j),
            (2.1243946552276611-1.5434627532958984j),
            (2.4633183479309082-8.6486437567145913e-08j),
            (1.8744727373123169+1.3618841171264648j),
            (0.67528903484344482+2.0783262252807617j),
            (-0.63866174221038818+1.965599536895752j),
            (-1.5857341289520264+1.152103066444397j),
            (-1.8640764951705933+7.6355092915036948e-08j),
            (-1.4381576776504517-1.0448826551437378j),
            (-0.52529704570770264-1.6166983842849731j),
            (0.50366902351379395-1.5501341819763184j),
            (1.26766037940979-0.92100900411605835j))

        sampling_freq = 100
        src1 = analog.sig_source_c(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100.0)
        dst1 = blocks.vector_sink_c()
        head = blocks.head(gr.sizeof_gr_complex, int (5*sampling_freq * 0.10))

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
            7.2191943445432116e-07,
            58.837181091308594,
            89.700050354003906,
            81.264183044433594,
            45.506141662597656,
            4.269894304798072e-07,
            -42.948936462402344,
            -65.50335693359375,
            -59.368724822998047,
            -33.261005401611328,
            -4.683740257860336e-07,
            31.423542022705078,
            47.950984954833984,
            43.485683441162109,
            24.378345489501953,
            5.7254135299444897e-07,
            -23.062990188598633,
            -35.218441009521484,
            -31.964075088500977,
            -17.934831619262695,
            -5.0591745548445033e-07,
            16.998210906982422,
            25.982204437255859,
            23.606258392333984,
            13.260685920715332,
            4.9936483037527069e-07,
            -12.59880542755127,
            -19.28221321105957,
            -17.54347038269043,
            -9.8700437545776367,
            -4.188150626305287e-07,
            9.4074573516845703,
            14.422011375427246,
            13.145503044128418,
            7.41046142578125,
            3.8512698097292741e-07,
            -7.0924453735351562,
            -10.896408081054688,
            -9.9552040100097656,
            -5.6262712478637695,
            -3.1982864356905338e-07,
            5.4131259918212891,
            8.3389215469360352,
            7.6409502029418945,
            4.3320145606994629,
            2.882407841298118e-07,
            -4.194943904876709,
            -6.4837145805358887,
            -5.9621825218200684,
            -3.3931560516357422)

        sampling_freq = 100
        src1 = analog.sig_source_f(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100.0)
        dst1 = blocks.vector_sink_f ()
        head = blocks.head (gr.sizeof_float, int (5*sampling_freq * 0.10))

        agc = analog.agc_ff(1e-3, 1, 1)

        tb.connect (src1, head)
        tb.connect (head, agc)
        tb.connect (agc, dst1)

        tb.run ()
        dst_data = dst1.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, dst_data, 4)

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
                        ((100.000244140625+7.2191943445432116e-07j),
                         (0.80881959199905396+0.58764183521270752j),
                         (0.30894950032234192+0.95084899663925171j),
                         (-0.30895623564720154+0.95086973905563354j),
                         (-0.80887287855148315+0.58768033981323242j),
                         (-0.99984413385391235+5.850709250410091e-09j),
                         (-0.80889981985092163-0.58770018815994263j),
                         (-0.30897706747055054-0.95093393325805664j),
                         (0.30898112058639526-0.95094609260559082j),
                         (0.80893135070800781-0.58772283792495728j),
                         (0.99990922212600708-8.7766354184282136e-09j),
                         (0.80894720554351807+0.58773452043533325j),
                         (0.30899339914321899+0.95098406076431274j),
                         (-0.30899572372436523+0.95099133253097534j),
                         (-0.80896598100662231+0.58774799108505249j),
                         (-0.99994778633117676+1.4628290578855285e-08j),
                         (-0.80897533893585205-0.58775502443313599j),
                         (-0.30900305509567261-0.95101380348205566j),
                         (0.30900448560714722-0.95101797580718994j),
                         (0.80898630619049072-0.58776277303695679j),
                         (0.99997037649154663-1.7554345532744264e-08j),
                         (0.80899184942245483+0.58776694536209106j),
                         (0.30900871753692627+0.95103120803833008j),
                         (-0.30900952219963074+0.95103377103805542j),
                         (-0.8089984655380249+0.58777159452438354j),
                         (-0.99998390674591064+2.3406109050938539e-08j),
                         (-0.809001624584198-0.58777409791946411j),
                         (-0.30901208519935608-0.95104163885116577j),
                         (0.30901262164115906-0.95104306936264038j),
                         (0.80900543928146362-0.587776780128479j),
                         (0.99999171495437622-2.6332081404234486e-08j),
                         (0.80900734663009644+0.58777821063995361j),
                         (0.30901408195495605+0.95104765892028809j),
                         (-0.30901429057121277+0.95104855298995972j),
                         (-0.80900967121124268+0.58777981996536255j),
                         (-0.99999648332595825+3.2183805842578295e-08j),
                         (-0.80901080369949341-0.58778077363967896j),
                         (-0.30901527404785156-0.95105135440826416j),
                         (0.30901545286178589-0.95105189085006714j),
                         (0.80901217460632324-0.58778166770935059j),
                         (0.99999916553497314-3.5109700036173308e-08j),
                         (0.809012770652771+0.58778214454650879j),
                         (0.30901595950126648+0.9510534405708313j),
                         (-0.30901598930358887+0.95105385780334473j),
                         (-0.80901366472244263+0.58778274059295654j),
                         (-1.0000008344650269+4.0961388947380328e-08j),
                         (-0.8090139627456665-0.58778303861618042j),
                         (-0.30901634693145752-0.95105475187301636j),
                         (0.30901640653610229-0.95105493068695068j),
                         (0.80901449918746948-0.5877833366394043j))

        sampling_freq = 100
        src1 = analog.sig_source_c(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100)
        dst1 = blocks.vector_sink_c()
        head = blocks.head(gr.sizeof_gr_complex, int(5*sampling_freq * 0.10))

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
            (7.2191943445432116e-07,
             58.837181091308594,
             40.194305419921875,
             2.9183335304260254,
             0.67606079578399658,
             8.6260438791896377e-09,
             -1.4542514085769653,
             -1.9210131168365479,
             -1.0450780391693115,
             -0.61939650774002075,
             -1.2590258613442984e-08,
             1.4308931827545166,
             1.9054338932037354,
             1.0443156957626343,
             0.61937344074249268,
             2.0983527804219193e-08,
             -1.4308838844299316,
             -1.9054274559020996,
             -1.0443152189254761,
             -0.61937344074249268,
             -2.5180233009791664e-08,
             1.4308837652206421,
             1.9054274559020996,
             1.0443154573440552,
             0.61937344074249268,
             3.3573645197293445e-08,
             -1.4308838844299316,
             -1.9054274559020996,
             -1.0443152189254761,
             -0.61937350034713745,
             -3.7770352179222755e-08,
             1.4308837652206421,
             1.9054274559020996,
             1.0443154573440552,
             0.61937350034713745,
             4.6163762590367696e-08,
             -1.4308838844299316,
             -1.9054274559020996,
             -1.0443153381347656,
             -0.61937344074249268,
             -5.0360466019583328e-08,
             1.4308837652206421,
             1.9054274559020996,
             1.0443155765533447,
             0.61937344074249268,
             5.8753879983441948e-08,
             -1.4308837652206421,
             -1.9054274559020996,
             -1.0443153381347656,
             -0.61937344074249268)

        sampling_freq = 100
        src1 = analog.sig_source_f(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100)
        dst1 = blocks.vector_sink_f()
        head = blocks.head(gr.sizeof_float, int(5*sampling_freq * 0.10))

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
                        ((100.000244140625+7.2191943445432116e-07j),
                         (0.80881959199905396+0.58764183521270752j),
                         (0.30894950032234192+0.95084899663925171j),
                         (-0.30895623564720154+0.95086973905563354j),
                         (-0.80887287855148315+0.58768033981323242j),
                         (-0.99984413385391235+5.850709250410091e-09j),
                         (-0.80889981985092163-0.58770018815994263j),
                         (-0.30897706747055054-0.95093393325805664j),
                         (0.30898112058639526-0.95094609260559082j),
                         (0.80893135070800781-0.58772283792495728j),
                         (0.99990922212600708-8.7766354184282136e-09j),
                         (0.80894720554351807+0.58773452043533325j),
                         (0.30899339914321899+0.95098406076431274j),
                         (-0.30899572372436523+0.95099133253097534j),
                         (-0.80896598100662231+0.58774799108505249j),
                         (-0.99994778633117676+1.4628290578855285e-08j),
                         (-0.80897533893585205-0.58775502443313599j),
                         (-0.30900305509567261-0.95101380348205566j),
                         (0.30900448560714722-0.95101797580718994j),
                         (0.80898630619049072-0.58776277303695679j),
                         (0.99997037649154663-1.7554345532744264e-08j),
                         (0.80899184942245483+0.58776694536209106j),
                         (0.30900871753692627+0.95103120803833008j),
                         (-0.30900952219963074+0.95103377103805542j),
                         (-0.8089984655380249+0.58777159452438354j),
                         (-0.99998390674591064+2.3406109050938539e-08j),
                         (-0.809001624584198-0.58777409791946411j),
                         (-0.30901208519935608-0.95104163885116577j),
                         (0.30901262164115906-0.95104306936264038j),
                         (0.80900543928146362-0.587776780128479j),
                         (0.99999171495437622-2.6332081404234486e-08j),
                         (0.80900734663009644+0.58777821063995361j),
                         (0.30901408195495605+0.95104765892028809j),
                         (-0.30901429057121277+0.95104855298995972j),
                         (-0.80900967121124268+0.58777981996536255j),
                         (-0.99999648332595825+3.2183805842578295e-08j),
                         (-0.80901080369949341-0.58778077363967896j),
                         (-0.30901527404785156-0.95105135440826416j),
                         (0.30901545286178589-0.95105189085006714j),
                         (0.80901217460632324-0.58778166770935059j),
                         (0.99999916553497314-3.5109700036173308e-08j),
                         (0.809012770652771+0.58778214454650879j),
                         (0.30901595950126648+0.9510534405708313j),
                         (-0.30901598930358887+0.95105385780334473j),
                         (-0.80901366472244263+0.58778274059295654j),
                         (-1.0000008344650269+4.0961388947380328e-08j),
                         (-0.8090139627456665-0.58778303861618042j),
                         (-0.30901634693145752-0.95105475187301636j),
                         (0.30901640653610229-0.95105493068695068j),
                         (0.80901449918746948-0.5877833366394043j))

        sampling_freq = 100
        src1 = analog.sig_source_c(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100)
        dst1 = blocks.vector_sink_c()
        head = blocks.head(gr.sizeof_gr_complex, int(5*sampling_freq * 0.10))

        agc = analog.agc2_cc(1e-2, 1e-3, 1, 1)

        tb.connect(src1, head)
        tb.connect(head, agc)
        tb.connect(agc, dst1)

        tb.run()
        dst_data = dst1.data()
        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 4)

    def test_006_sets(self):
        agc = analog.agc3_cc(1e-3, 1e-1, 1)

	agc.set_attack_rate(1)
	agc.set_decay_rate(2)
	agc.set_reference(1.1)
	agc.set_gain(1.1)

        self.assertAlmostEqual(agc.attack_rate(), 1)
        self.assertAlmostEqual(agc.decay_rate(), 2)
        self.assertAlmostEqual(agc.reference(), 1.1)
        self.assertAlmostEqual(agc.gain(), 1.1)

    def test_006(self):
        ''' Test the complex AGC loop (attack and decay rate inputs) '''
        tb = self.tb

        sampling_freq = 100
        N = int(5*sampling_freq)
        src1 = analog.sig_source_c(sampling_freq, analog.GR_SIN_WAVE,
                                   sampling_freq * 0.10, 100)
        dst1 = blocks.vector_sink_c()
        head = blocks.head(gr.sizeof_gr_complex, N)

        ref = 1
        agc = analog.agc3_cc(1e-2, 1e-3, ref)

        tb.connect(src1, head)
        tb.connect(head, agc)
        tb.connect(agc, dst1)

        tb.run()
        dst_data = dst1.data()
        M = 100
        result = map(lambda x: abs(x), dst_data[N-M:])
        self.assertFloatTuplesAlmostEqual(result, M*[ref,], 4)

    def test_100(self):
        ''' Test complex feedforward agc with constant input '''

        length = 8
        gain = 2

        input_data = 8*(0.0,) + 24*(1.0,) + 24*(0.0,)
        expected_result = (8+length-1)*(0.0,) + 24*(gain*1.0,) + (0,)

        src = blocks.vector_source_c(input_data)
        agc = analog.feedforward_agc_cc(8, 2.0)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, agc, dst)

        self.tb.run()
        dst_data = dst.data()[0:len(expected_result)]

        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 4)


if __name__ == '__main__':
    gr_unittest.run(test_agc, "test_agc.xml")

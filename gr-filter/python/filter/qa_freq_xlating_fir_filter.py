#!/usr/bin/env python
#
# Copyright 2008,2010,2012 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

from gnuradio import gr, gr_unittest
import filter_swig as filter
import cmath, math

class test_freq_xlating_filter(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()

    def tearDown(self):
        self.tb = None

    def generate_ccf_source(self):
        self.fs = fs = 1
        self.fc = fc = 0.3
        self.bw = bw = 0.1
        self.taps = filter.firdes.low_pass(1, fs, bw, bw/4)
        times = xrange(100)
        self.src_data = map(lambda t: cmath.exp(-2j*cmath.pi*fc/fs*(t/100.0)), times)

    def generate_ccc_source(self):
        self.fs = fs = 1
        self.fc = fc = 0.3
        self.bw = bw = 0.1
        self.taps = filter.firdes.complex_band_pass(1, fs, -bw/2, bw/2, bw/4)
        times = xrange(100)
        self.src_data = map(lambda t: cmath.exp(-2j*cmath.pi*fc/fs*(t/100.0)), times)

    def generate_fcf_source(self):
        self.fs = fs = 1
        self.fc = fc = 0.3
        self.bw = bw = 0.1
        self.taps = filter.firdes.low_pass(1, fs, bw, bw/4)
        times = xrange(100)
        self.src_data = map(lambda t: math.sin(2*cmath.pi*fc/fs*(t/100.0)), times)

    def generate_fcc_source(self):
        self.fs = fs = 1
        self.fc = fc = 0.3
        self.bw = bw = 0.1
        self.taps = filter.firdes.complex_band_pass(1, fs, -bw/2, bw/2, bw/4)
        times = xrange(100)
        self.src_data = map(lambda t: math.sin(2*cmath.pi*fc/fs*(t/100.0)), times)

    def generate_scf_source(self):
        self.fs = fs = 1
        self.fc = fc = 0.3
        self.bw = bw = 0.1
        self.taps = filter.firdes.low_pass(1, fs, bw, bw/4)
        times = xrange(100)
        self.src_data = map(lambda t: int(100*math.sin(2*cmath.pi*fc/fs*(t/100.0))), times)

    def generate_scc_source(self):
        self.fs = fs = 1
        self.fc = fc = 0.3
        self.bw = bw = 0.1
        self.taps = filter.firdes.complex_band_pass(1, fs, -bw/2, bw/2, bw/4)
        times = xrange(100)
        self.src_data = map(lambda t: int(100*math.sin(2*cmath.pi*fc/fs*(t/100.0))), times)


    def test_fir_filter_ccf_001(self):
        self.generate_ccf_source()
        expected_data = ((0.001697700354270637+0.004312471952289343j),
                         (0.003520616563037038-0.003014103975147009j),
                         (0.004252811893820763-0.008337559178471565j),
                         (0.0030743128154426813-0.010262271389365196j),
                         (0.0007344777695834637-0.007861139252781868j),
                         (-0.0011067686136811972-0.0028924935031682253j),
                         (-0.002371778478845954+0.0019914964213967323j),
                         (-0.003023319412022829+0.005717850290238857j),
                         (-0.0021738125942647457+0.007211698684841394j),
                         (-0.0004628606839105487+0.005501383915543556j),
                         (0.0007428556564264+0.0019867848604917526j),
                         (0.001634795218706131-0.0013514887541532516j),
                         (0.002205110155045986-0.00402155052870512j),
                         (0.0015480631263926625-0.005179159343242645j),
                         (0.00026722141774371266-0.003887997241690755j),
                         (-0.0004911854630336165-0.0013578246580436826j),
                         (-0.0011226939968764782+0.0009080552263185382j),
                         (-0.0016229727771133184+0.0028335191309452057j),
                         (-0.0010890064295381308+0.0037298379465937614j),
                         (-0.00012392725329846144+0.0027196139562875032j))
        src = gr.vector_source_c(self.src_data)
        op  = filter.freq_xlating_fir_filter_ccf(1, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

    def test_fir_filter_ccf_002(self):
        self.generate_ccf_source()
        expected_data = ((6.419439159799367e-05-0.0006292851758189499j),
                         (-0.00037074743886478245+0.0013245552545413375j),
                          (0.0006853155209682882-0.0023769831750541925j),
                          (-0.001427714480087161+0.002608160488307476j),
                          (0.0015907397028058767-0.000811046629678458j),
                          (-0.0004226673918310553-0.0024389736354351044j),
                          (-0.0013841050677001476+0.006231029983609915j),
                          (0.0035029184073209763-0.009738259017467499j),
                          (-0.005924836732447147+0.010320881381630898j),
                          (0.006831614300608635-0.003950652200728655j),
                          (-0.0021247887052595615-0.015604906715452671j),
                          (-0.04283163696527481+0.09995654970407486j),
                          (-0.01391829177737236+0.07924056798219681j),
                          (0.010886997915804386-0.02463012933731079j),
                          (-0.0056075905449688435+0.004998659715056419j),
                          (0.0016976913902908564+0.004312459379434586j),
                          (0.0007344821933656931-0.007861112244427204j),
                          (-0.002173811662942171+0.007211671676486731j),
                          (0.0022051059640944004-0.00402153329923749j),
                          (-0.0011226903880015016+0.0009080505697056651j))
        src = gr.vector_source_c(self.src_data)
        op  = filter.freq_xlating_fir_filter_ccf(4, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

    def test_fir_filter_ccc_001(self):
        self.generate_ccc_source()
        expected_data = ((0.0036842757836-0.0114002721384j),
                         (0.00324621866457-0.0108166672289j),
                         (0.00206564785913-0.00923090614378j),
                         (0.00109899020754-0.00656201224774j),
                         (0.000506619049702-0.00402844604105j),
                         (-0.000523390364833-0.00166808743961j),
                         (-0.00140534969978+0.00103991874494j),
                         (-0.00154365820345+0.00315759982914j),
                         (-0.00180402118713+0.00427215453237j),
                         (-0.00216706306674+0.00524478312582j),
                         (-0.00178848754149+0.0057489364408j),
                         (-0.00129876169376+0.00512680830434j),
                         (-0.00122803379782+0.00427244976163j),
                         (-0.000722666736692+0.00351428100839j),
                         (5.53092104383e-05+0.00207865727134j),
                         (0.000227351076319+0.000517217209563j),
                         (0.000414477253798-0.000383921898901j),
                         (0.000998671515845-0.00135387131013j),
                         (0.00104933069088-0.00243046949618j),
                         (0.000765930046327-0.0026717747096j))
        src = gr.vector_source_c(self.src_data)
        op  = filter.freq_xlating_fir_filter_ccc(1, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

    def test_fir_filter_ccc_002(self):
        self.generate_ccc_source()
        expected_data = ((-0.000650451984257+0.00120697380044j),
                         (-9.59713361226e-05+0.00102412770502j),
                         (0.000958710326813-0.00145424995571j),
                         (0.000901343999431-0.00290832063183j),
                         (-0.000822560978122+0.000296717538731j),
                         (-0.00211223773658+0.00519825471565j),
                         (-0.00037001183955+0.00358242215589j),
                         (0.00327983591706-0.00616005761549j),
                         (0.00356886954978-0.0117237549275j),
                         (-0.00328874029219+0.00182871113066j),
                         (-0.0139285130426+0.0320657044649j),
                         (-0.0198133718222+0.0562113076448j),
                         (-0.0157803222537+0.0530290603638j),
                         (-0.00550725404173+0.0255754813552j),
                         (0.00252919178456-0.00232240976766j),
                         (0.00368427345529-0.0114002330229j),
                         (0.000506620621309-0.00402843113989j),
                         (-0.00180401885882+0.00427213776857j),
                         (-0.00122803344857+0.00427243299782j),
                         (0.000414476031438-0.000383919978049j))
        src = gr.vector_source_c(self.src_data)
        op  = filter.freq_xlating_fir_filter_ccc(4, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

    def test_fir_filter_fcf_001(self):
        self.generate_fcf_source()
        expected_data = ((0.000247188087087-0.000157509770361j),
                         (-0.000155229790835-0.000246197130764j),
                         (-0.000264906557277+0.000174603672349j),
                         (6.99016964063e-05+0.000174961372977j),
                         (-5.48477692064e-05-0.0001131295503j),
                         (-0.000237467131228+0.000118011368613j),
                         (0.000136614587973+0.000229531884543j),
                         (0.000229347482673-0.000166581812664j),
                         (-0.000106010869786-0.000150042149471j),
                         (2.92293734674e-05+0.000142060467624j),
                         (0.000228707227507-9.30760797928e-05j),
                         (-0.000124306126963-0.000216641055886j),
                         (-0.000204823678359+0.00016052465071j),
                         (0.00012825592421+0.000133123627165j),
                         (-1.18284006021e-05-0.000159015646204j),
                         (-0.000219973371713+7.5438656495e-05j),
                         (0.000114713984658+0.000205190401175j),
                         (0.000185727752978-0.000154630601173j),
                         (-0.000141745767905-0.000120098840853j),
                         (-3.9850056055e-07+0.000168364742422j))
        src = gr.vector_source_f(self.src_data)
        op  = filter.freq_xlating_fir_filter_fcf(1, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

    def test_fir_filter_fcf_002(self):
        self.generate_fcf_source()
        expected_data = ((7.3052920925e-05-3.91741014028e-06j),
                         (3.11913172482e-05-0.000109872074972j),
                         (-0.000128646017401-3.49857727997e-05j),
                         (-5.49546712136e-05+8.96326746442e-05j),
                         (5.14321582159e-05+9.64698920143e-05j),
                         (0.000120189361041+2.4231892894e-05j),
                         (0.000100405508419-0.000223224604269j),
                         (-0.000274751859251-2.33274622587e-05j),
                         (1.52600114234e-06+0.000133301247843j),
                         (3.77224641852e-05+5.29596509296e-05j),
                         (-3.60160379387e-06+0.000247975171078j),
                         (0.00113093166146-0.000663110695314j),
                         (0.00059568521101-0.00099650840275j),
                         (-0.000475480686873+0.000250602373853j),
                         (0.000191397906747+0.000271986238658j),
                         (0.000247183139436-0.000157510468853j),
                         (-5.48357638763e-05-0.000113135029096j),
                         (-0.00010601492977-0.00015005269961j),
                         (-0.000204817260965+0.000160534662427j),
                         (0.000114742244477+0.000205190313864j))
        src = gr.vector_source_f(self.src_data)
        op  = filter.freq_xlating_fir_filter_fcf(4, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

    def test_fir_filter_fcc_001(self):
        self.generate_fcc_source()
        expected_data = ((-0.000337088305969+7.46004516259e-05j),
                         (-5.63409266761e-05+0.000301144464174j),
                         (9.16960561881e-05-2.89259278361e-05j),
                         (-0.000231866899412-6.25764005235e-05j),
                         (-0.000116414688819+0.000258557556663j),
                         (0.000206079319469+5.05045172758e-05j),
                         (-3.85114690289e-05-0.00019276549574j),
                         (-0.000146380873048+0.000112079876999j),
                         (0.000215423395275+0.000116706331028j),
                         (0.000136050162837-0.000232611957472j),
                         (-0.000155499437824-5.41604022146e-05j),
                         (0.000106907449663+0.00016310159117j),
                         (0.000224392410018-0.000156331108883j),
                         (-0.000131131906528-0.000172063446371j),
                         (-5.92393880652e-05+0.00016801241145j),
                         (0.000214921761653-5.32235890205e-06j),
                         (-5.96960526309e-05-0.000211164733628j),
                         (-0.000193948610104+0.000113364716526j),
                         (0.000134820176754+0.000142527525895j),
                         (4.74465123261e-05-0.000175131688593j))
        src = gr.vector_source_f(self.src_data)
        op  = filter.freq_xlating_fir_filter_fcc(1, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

    def test_fir_filter_fcc_002(self):
        self.generate_fcc_source()
        expected_data = ((-6.94218761055e-05-9.90489479591e-06j),
                         (-2.56973435171e-05+8.05932795629e-05j),
                         (7.01698663761e-05+7.36373840482e-05j),
                         (7.57163215894e-05-4.65324592369e-05j),
                         (-3.01657128148e-05-0.000122838056996j),
                         (-9.53659764491e-05-3.73945695173e-05j),
                         (-2.33501577895e-05+0.000109135726234j),
                         (6.65136758471e-05+0.000125709688291j),
                         (3.08501912514e-05-9.16842873266e-06j),
                         (-2.64703612629e-05-0.000135892929393j),
                         (0.000136643866426-0.000162003751029j),
                         (0.000501801609062-0.000185820827028j),
                         (0.000694551155902-0.000299874518532j),
                         (0.000424396857852-0.00038379128091j),
                         (-9.1786707344e-05-0.000242479465669j),
                         (-0.000337087287335+7.45999423089e-05j),
                         (-0.000116414521472+0.000258556567132j),
                         (0.000215422536712+0.000116706112749j),
                         (0.000224391726078-0.000156330308528j),
                         (-5.96956087975e-05-0.000211163976928j))
        src = gr.vector_source_f(self.src_data)
        op  = filter.freq_xlating_fir_filter_fcc(4, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

    def test_fir_filter_scf_001(self):
        self.generate_scf_source()
        expected_data = ((-0.0330070219934+0.101965591311j),
                         (-0.0484239049256+0.0872343629599j),
                         (-0.0214109234512+0.102555386722j),
                         (0.0484058149159+0.0557125210762j),
                         (0.0574690811336-0.0350844524801j),
                         (0.0365394353867-0.0802438184619j),
                         (0.0453781634569-0.130992040038j),
                         (0.00801951438189-0.214278846979j),
                         (-0.0770946145058-0.230616629124j),
                         (-0.105601429939-0.190731987357j),
                         (-0.105361394584-0.177761554718j),
                         (-0.131518915296-0.136102750897j),
                         (-0.103761836886-0.0382263250649j),
                         (-0.0167790111154+0.0152206514031j),
                         (0.0277570039034+0.0300403907895j),
                         (0.056065287441+0.0806603953242j),
                         (0.118084669113+0.104863211513j),
                         (0.128281414509+0.0677760615945j),
                         (0.0748447552323+0.0619902014732j),
                         (0.0512856245041+0.0775099247694j))
        src = gr.vector_source_s(self.src_data)
        op  = filter.freq_xlating_fir_filter_scf(1, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

    def test_fir_filter_scf_002(self):
        self.generate_scf_source()
        expected_data = ((0.00824625696987-1.50158575707e-05j),
                         (0.0020101047121-0.0116540221497j),
                         (-0.0126378880814-0.00259830290452j),
                         (-0.00363933457993+0.00837504956871j),
                         (0.00107023562305+0.00915473792702j),
                         (0.0169738996774+0.00422182958573j),
                         (0.00630031805485-0.025423232466j),
                         (-0.0283014029264+0.00104465708137j),
                         (0.00890890974551+0.0115978596732j),
                         (-0.0142687577754+0.00306978379376j),
                         (0.02845691517+0.0331163145602j),
                         (0.0538152232766-0.0908300876617j),
                         (-0.0843691527843-0.0956566259265j),
                         (0.0476895272732+0.0747984498739j),
                         (0.0898786485195+0.082478672266j),
                         (-0.0330070182681+0.101965606213j),
                         (0.0574697069824-0.0350842289627j),
                         (-0.0770940706134-0.230615705252j),
                         (-0.103762261569-0.0382265634835j),
                         (0.11808334291+0.104863762856j))
        src = gr.vector_source_s(self.src_data)
        op  = filter.freq_xlating_fir_filter_scf(4, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

    def test_fir_filter_scc_001(self):
        self.generate_scc_source()
        expected_data = ((-0.00775564694777+0.0437113791704j),
                         (0.0108830630779+0.0433648750186j),
                         (0.015553932637-0.0133284125477j),
                         (-0.0264905355871-0.0403266139328j),
                         (-0.0243480335921-0.03030154109j),
                         (-0.000327925226884-0.069333948195j),
                         (-0.0306392069906-0.107313856483j),
                         (-0.0452371090651-0.0854917764664j),
                         (-0.0108894333243-0.0875641107559j),
                         (-0.0182112380862-0.118961036205j),
                         (-0.0447825863957-0.0922874584794j),
                         (-0.0147479763255-0.0572904124856j),
                         (0.00204290449619-0.0721436738968j),
                         (-0.027713002637-0.0548989400268j),
                         (-0.0149045493454-0.00210141134448j),
                         (0.0176361314952-0.00149522523861j),
                         (-0.00527482619509-0.00698099425063j),
                         (-0.0151527002454+0.036265052855j),
                         (0.0199296213686+0.0452499426901j),
                         (0.0122985243797+0.0143278446048j))
        src = gr.vector_source_s(self.src_data)
        op  = filter.freq_xlating_fir_filter_scc(1, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

    def test_fir_filter_scc_002(self):
        self.generate_scc_source()
        expected_data = ((-0.0080680437386-0.00158522999845j),
                         (-0.0037795654498+0.00733159901574j),
                         (0.00842926371843+0.00777021236718j),
                         (0.0112090632319-0.00249325321056j),
                         (-0.0027476802934-0.0115710813552j),
                         (-0.0158688724041-0.00715934624895j),
                         (-0.00802888441831+0.00620818417519j),
                         (0.0131985172629+0.0149175003171j),
                         (0.0190298333764+0.00956719089299j),
                         (-0.00112380902283-0.00936658866704j),
                         (-0.0204226914793-0.0333464704454j),
                         (-0.00695514678955-0.0437445007265j),
                         (0.0314490310848-0.0207983348519j),
                         (0.0529675260186+0.0302227605134j),
                         (0.0317338332534+0.0667510479689j),
                         (-0.00775565672666+0.0437112376094j),
                         (-0.024347923696-0.0303014591336j),
                         (-0.0108893103898-0.0875638127327j),
                         (0.00204296782613-0.0721434056759j),
                         (-0.00527479872108-0.00698097236454j))
        src = gr.vector_source_s(self.src_data)
        op  = filter.freq_xlating_fir_filter_scc(4, self.taps, self.fc, self.fs)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data[-20:], 5)

if __name__ == '__main__':
    gr_unittest.run(test_freq_xlating_filter, "test_freq_xlating_filter.xml")


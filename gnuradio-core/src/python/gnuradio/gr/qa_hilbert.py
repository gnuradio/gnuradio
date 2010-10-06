#!/usr/bin/env python
#
# Copyright 2004,2007,2010 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest
import math

class test_hilbert (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_hilbert (self):
        tb = self.tb
        ntaps = 51
        sampling_freq = 100

        expected_result = (                             -1.4678005338941702e-11j, 
                                                        -0.0011950774351134896j, 
                                                        -0.0019336787518113852j, 
                                                        -0.0034673355985432863j, 
                                                        -0.0036765895783901215j, 
                                                        -0.004916108213365078j, 
                                                        -0.0042778430506587029j, 
                                                        -0.006028641015291214j, 
                                                        -0.005476709920912981j, 
                                                        -0.0092810001224279404j, 
                                                        -0.0095402700826525688j, 
                                                        -0.016060983762145042j, 
                                                        -0.016446959227323532j, 
                                                        -0.02523401565849781j, 
                                                        -0.024382550269365311j, 
                                                        -0.035477779805660248j, 
                                                        -0.033021725714206696j, 
                                                        -0.048487484455108643j, 
                                                        -0.04543270543217659j, 
                                                        -0.069477587938308716j, 
                                                        -0.066984444856643677j, 
                                                        -0.10703597217798233j, 
                                                        -0.10620346665382385j, 
                                                        -0.1852707713842392j, 
                                                        -0.19357112050056458j, 
                            (7.2191945754696007e-09     -0.50004088878631592j), 
                            (0.58778399229049683        -0.6155126690864563j), 
                            (0.95105588436126709        -0.12377222627401352j), 
                            (0.95105588436126709        +0.41524654626846313j), 
                            (0.5877838134765625         +0.91611981391906738j), 
                            (5.8516356205018383e-09     +1.0670661926269531j), 
                            (-0.5877840518951416        +0.87856143712997437j), 
                            (-0.95105588436126709       +0.35447561740875244j), 
                            (-0.95105588436126709       -0.26055556535720825j), 
                            (-0.5877838134765625        -0.77606213092803955j), 
                            (-8.7774534307527574e-09    -0.96460390090942383j), 
                            (0.58778399229049683        -0.78470128774642944j), 
                            (0.95105588436126709        -0.28380891680717468j), 
                            (0.95105588436126709        +0.32548999786376953j), 
                            (0.5877838134765625         +0.82514488697052002j), 
                            (1.4629089051254596e-08     +1.0096219778060913j), 
                            (-0.5877840518951416        +0.81836479902267456j), 
                            (-0.95105588436126709       +0.31451958417892456j), 
                            (-0.95105588436126709       -0.3030143678188324j), 
                            (-0.5877838134765625        -0.80480599403381348j), 
                            (-1.7554906861505515e-08    -0.99516552686691284j), 
                            (0.58778399229049683        -0.80540722608566284j), 
                            (0.95105582475662231        -0.30557557940483093j), 
                            (0.95105588436126709        +0.31097668409347534j), 
                            (0.5877838134765625         +0.81027895212173462j), 
                            (2.3406542482007353e-08     +1.0000816583633423j), 
                            (-0.5877840518951416        +0.80908381938934326j), 
                            (-0.95105588436126709       +0.30904293060302734j), 
                            (-0.95105588436126709       -0.30904296040534973j), 
                            (-0.5877838134765625        -0.80908387899398804j), 
                            (-2.6332360292258272e-08    -1.0000815391540527j), 
                            (0.58778399229049683        -0.80908381938934326j), 
                            (0.95105582475662231        -0.30904299020767212j), 
                            (0.95105588436126709        +0.30904293060302734j), 
                            (0.5877838134765625         +0.80908381938934326j), 
                            (3.218399768911695e-08      +1.0000815391540527j))
        

        src1 = gr.sig_source_f (sampling_freq, gr.GR_SIN_WAVE,
                                sampling_freq * 0.10, 1.0)

        head = gr.head (gr.sizeof_float, int (ntaps + sampling_freq * 0.10))
        hilb = gr.hilbert_fc (ntaps)
        dst1 = gr.vector_sink_c ()
        tb.connect (src1, head)
        tb.connect (head, hilb)
        tb.connect (hilb, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertComplexTuplesAlmostEqual (expected_result, dst_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_hilbert, "test_hilbert.xml")

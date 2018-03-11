#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: IQ Imbalance Generator
# Author: mettus
# Generated: Thu Aug  1 12:08:07 2013
##################################################

from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
import math

class iqbal_gen(gr.hier_block2):

    def __init__(self, magnitude=0, phase=0, mode=0):
        '''
        This block implements the single branch IQ imbalance
        transmitter and receiver models.

        Developed from source (2014):
        "In-Phase and Quadrature Imbalance:
          Modeling, Estimation, and Compensation"

        TX Impairment:

                                  {R}--|Multiply: 10**(mag/20)|--+--|Multiply: cos(pi*degree/180)|--X1
        Input ---|Complex2Float|---|                             +--|Multiply: sin(pi*degree/180)|--X2
                                  {I}--|  Adder  |
                                   X2--|   (+)   |--X3

                          X1--{R}--| Float 2 |--- Output
                          X3--{I}--| Complex |

        RX Impairment:

                                  {R}--|Multiply: cos(pi*degree/180)|-------|       |
        Input ---|Complex2Float|---|                                        | Adder |--X1
                                  {I}--+--|Multiply: sin(pi*degree/180)|----|  (+)  |
                                       |
                                       +--X2

                        X1--|Multply: 10**(mag/20)|--{R}--| Float 2 |--- Output
                        X2---------------------------{I}--| Complex |

        (ASCII ART monospace viewing)
        '''
        gr.hier_block2.__init__(
            self, "IQ Imbalance Generator",
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
        )

        ##################################################
        # Parameters
        ##################################################
        self.magnitude = magnitude
        self.phase = phase
        self.mode = mode

        ##################################################
        # Blocks
        ##################################################
        # Same blocks for Transmitter and Receiver
        self.mag = blocks.multiply_const_vff((math.pow(10,magnitude/20.0), ))
        self.sin_phase = blocks.multiply_const_vff((math.sin(phase*math.pi/180.0), ))
        self.cos_phase = blocks.multiply_const_vff((math.cos(phase*math.pi/180.0), ))
        self.f2c = blocks.float_to_complex(1)
        self.c2f = blocks.complex_to_float(1)
        self.adder = blocks.add_vff(1)

        ##################################################
        # Connections
        ##################################################
        if(self.mode):
            # Receiver Mode
            self.connect((self, 0), (self.c2f, 0))
            self.connect((self.c2f, 0), (self.cos_phase, 0))
            self.connect((self.cos_phase, 0), (self.adder, 0))
            self.connect((self.c2f, 1), (self.sin_phase, 0))
            self.connect((self.sin_phase, 0), (self.adder, 1))
            self.connect((self.adder, 0), (self.mag, 0))
            self.connect((self.mag, 0), (self.f2c, 0))
            self.connect((self.c2f, 1), (self.f2c, 1))
            self.connect((self.f2c, 0), (self, 0))
        else:
            # Transmitter Mode
            self.connect((self, 0), (self.c2f, 0))
            self.connect((self.c2f, 0), (self.mag, 0))
            self.connect((self.mag, 0), (self.cos_phase, 0))
            self.connect((self.cos_phase, 0), (self.f2c, 0))
            self.connect((self.mag, 0), (self.sin_phase, 0))
            self.connect((self.sin_phase, 0), (self.adder, 0))
            self.connect((self.c2f, 1), (self.adder, 1))
            self.connect((self.adder, 0), (self.f2c, 1))
            self.connect((self.f2c, 0), (self, 0))


    # QT sink close method reimplementation

    def get_magnitude(self):
        return self.magnitude

    def set_magnitude(self, magnitude):
        self.magnitude = magnitude
        self.mag.set_k((math.pow(10,self.magnitude/20.0), ))

    def get_phase(self):
        return self.phase

    def set_phase(self, phase):
        self.phase = phase
        self.sin_phase.set_k((math.sin(self.phase*math.pi/180.0), ))
        self.cos_phase.set_k((math.cos(self.phase*math.pi/180.0), ))



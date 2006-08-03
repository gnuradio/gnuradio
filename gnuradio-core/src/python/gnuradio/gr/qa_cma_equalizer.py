#!/usr/bin/env python

from gnuradio import gr, gr_unittest

class test_cma_equalizer_fir(gr_unittest.TestCase):

    def setUp(self):
    	self.fg = gr.flow_graph()

    def tearDown(self):
    	self.fg = None
    	
    def transform(self, src_data):
	SRC = gr.vector_source_c(src_data, False)
	EQU = gr.cma_equalizer_cc(4, 1.0, .001)
	DST = gr.vector_sink_c()
	self.fg.connect(SRC, EQU, DST)
	self.fg.run()
	return DST.data()

    def test_001_identity(self):
    	# Constant modulus signal so no adjustments
	src_data      = (1+0j, 0+1j, -1+0j, 0-1j)*1000
	expected_data = src_data
	result = self.transform(src_data)
	self.assertComplexTuplesAlmostEqual(expected_data, result)

if __name__ == "__main__":
    gr_unittest.main()
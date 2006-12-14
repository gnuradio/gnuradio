#!/usr/bin/env python

from gnuradio import gr, gr_unittest

class test_runtime(gr_unittest.TestCase):

    def setUp(self):
	pass
	
    def tearDown(self):
    	pass

    def test_001_run(self):
    	hblock = gr.hier_block2("test_block", 
				gr.io_signature(0,0,0), 
				gr.io_signature(0,0,0))
    	runtime = gr.runtime(hblock)
    	runtime.run()

    def test_002_run_twice(self):
    	hblock = gr.hier_block2("test_block", 
				gr.io_signature(0,0,0), 
				gr.io_signature(0,0,0))
    	runtime = gr.runtime(hblock)
    	runtime.run()
    	self.assertRaises(RuntimeError, lambda: runtime.run())
        
if __name__ == "__main__":
    gr_unittest.main()

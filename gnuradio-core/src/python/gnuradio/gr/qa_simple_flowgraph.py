#!/usr/bin/env python

from gnuradio import gr, gr_unittest

class test_simple_flowgraph(gr_unittest.TestCase):

    def setUp(self):
	pass
	
    def tearDown(self):
	pass
			
    def test_001_define_component(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src", gr.null_source(gr.sizeof_int))
	sfg.define_component("dst", gr.null_sink(gr.sizeof_int))

    def test_002_define_component_name_in_use(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src", gr.null_source(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: sfg.define_component("src", gr.null_sink(gr.sizeof_int)))
    
    def test_003_connect(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src", gr.null_source(gr.sizeof_int))
	sfg.define_component("dst", gr.null_sink(gr.sizeof_int))
	sfg.connect("src", 0, "dst", 0)

    def test_004connect_unknown_src(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("dst", gr.null_sink(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: sfg.connect("src", 0, "dst", 0))
    
    def test_005_connect_unknown_dst(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src", gr.null_source(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: sfg.connect("src", 0, "dst", 0))

    def test_006_connect_invalid_src_port_neg(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src", gr.null_source(gr.sizeof_int))
	sfg.define_component("dst", gr.null_sink(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: sfg.connect("src", -1, "dst", 0))

    def test_007_connect_invalid_src_port_exceeds(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src", gr.null_source(gr.sizeof_int))
	sfg.define_component("dst", gr.null_sink(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: sfg.connect("src", 1, "dst", 0))

    def test_008_connect_invalid_dst_port_neg(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src", gr.null_source(gr.sizeof_int))
	sfg.define_component("dst", gr.null_sink(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: sfg.connect("src", 0, "dst", -1))

    def test_009_connect_invalid_dst_port_exceeds(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src", gr.null_source(gr.sizeof_int))
	sfg.define_component("dst", gr.null_sink(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: sfg.connect("src", 0, "dst", 1))

    def test_010_connect_invalid_dst_port_in_use(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src1", gr.null_source(gr.sizeof_int))
	sfg.define_component("src2", gr.null_source(gr.sizeof_int))
	sfg.define_component("dst", gr.null_sink(gr.sizeof_int))
	sfg.connect("src1", 0, "dst", 0)
	self.assertRaises(ValueError, 
	    lambda: sfg.connect("src2", 0, "dst", 0))
    
    def test_011_connect_one_src_two_dst(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src", gr.null_source(gr.sizeof_int))
	sfg.define_component("dst1", gr.null_sink(gr.sizeof_int))
	sfg.define_component("dst2", gr.null_sink(gr.sizeof_int))
	sfg.connect("src", 0, "dst1", 0)
	sfg.connect("src", 0, "dst2", 0)

    def test_012_connect_type_mismatch(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src", gr.null_source(gr.sizeof_int))
	sfg.define_component("dst", gr.null_sink(gr.sizeof_char))
	self.assertRaises(ValueError,
	    lambda: sfg.connect("src", 0, "dst", 0))

    def test_013_validate(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src", gr.null_source(gr.sizeof_int))
	sfg.define_component("dst1", gr.null_sink(gr.sizeof_int))
	sfg.define_component("dst2", gr.null_sink(gr.sizeof_int))
	sfg.connect("src", 0, "dst1", 0)
	sfg.connect("src", 0, "dst2", 0)
	sfg.validate()
	
    def test_014_validate(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src1", gr.null_source (gr.sizeof_int))
	sfg.define_component("nop1", gr.nop (gr.sizeof_int))
	sfg.define_component("dst1", gr.null_sink (gr.sizeof_int))
	sfg.define_component("dst2", gr.null_sink (gr.sizeof_int))
	sfg.connect("src1", 0, "nop1", 0)
	sfg.connect("src1", 0, "nop1", 1)
	sfg.connect("nop1", 0, "dst1", 0)
	sfg.connect("nop1", 1, "dst2", 0)
	sfg.validate ()
										       
    def test_015_validate(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src1", gr.null_source (gr.sizeof_int))
	sfg.define_component("nop1", gr.nop (gr.sizeof_int))
	sfg.define_component("dst1", gr.null_sink (gr.sizeof_int))
	sfg.define_component("dst2", gr.null_sink (gr.sizeof_int))
	sfg.connect("src1", 0, "nop1", 0)
	sfg.connect("src1", 0, "nop1", 2)
	sfg.connect("nop1", 0, "dst1", 0)
	sfg.connect("nop1", 1, "dst2", 0)
	self.assertRaises(RuntimeError,
	    lambda: sfg.validate ())
    									       
    def test_016_validate(self):
	sfg = gr.simple_flowgraph()
	sfg.define_component("src1", gr.null_source (gr.sizeof_int))
	sfg.define_component("nop1", gr.nop (gr.sizeof_int))
	sfg.define_component("dst1", gr.null_sink (gr.sizeof_int))
	sfg.define_component("dst2", gr.null_sink (gr.sizeof_int))
	sfg.connect("src1", 0, "nop1", 0)
	sfg.connect("src1", 0, "nop1", 1)
	sfg.connect("nop1", 0, "dst1", 0)
	sfg.connect("nop1", 2, "dst2", 0)
	self.assertRaises(RuntimeError,
	    lambda: sfg.validate ())
										       
if __name__ == "__main__":
    gr_unittest.main()

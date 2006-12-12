#!/usr/bin/env python

from gnuradio import gr, gr_unittest

class test_hier_block2(gr_unittest.TestCase):

    def setUp(self):
	pass
	
    def tearDown(self):
    	pass
	
    def test_001_make(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	self.assertEqual("test_block", hblock.name())
	self.assertEqual(1, hblock.input_signature().max_streams())
	self.assertEqual(1, hblock.output_signature().min_streams())
	self.assertEqual(1, hblock.output_signature().max_streams())
	self.assertEqual(gr.sizeof_int, hblock.output_signature().sizeof_stream_item(0))

    def test_002_define_component(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("src", gr.null_source(gr.sizeof_int))
	hblock.define_component("dst", gr.null_sink(gr.sizeof_int))

    def test_003_define_component_reserved_input(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: hblock.define_component("self", gr.nop(gr.sizeof_int)))

    def test_004_define_component_name_in_use(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("src", gr.null_source(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: hblock.define_component("src", gr.null_sink(gr.sizeof_int)))

    def test_006_connect_internal(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	hblock.define_component("nop2", gr.nop(gr.sizeof_int))
	hblock.connect("nop1", 0, "nop2", 0)

    def test_007_connect_input(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	hblock.connect("self", 0, "nop1", 0)

    def test_008_connect_output(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	hblock.connect("nop1", 0, "self", 0)

    def test_009_connect_unknown_src(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	hblock.define_component("nop1", nop1)
	self.assertRaises(ValueError, 
	    lambda: hblock.connect("nop2", 0, "self", 0))
    
    def test_010_connect_unknown_dst(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: hblock.connect("self", 0, "nop2", 0))

    def test_011_connect_invalid_src_port_neg(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: hblock.connect("self", -1, "nop1", 0))

    def test_012_connect_invalid_src_port_exceeds(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: hblock.connect("self", 1, "nop1", 0))
    
    def test_013_connect_invalid_dst_port_neg(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: hblock.connect("self", -1, "nop1", 0))

    def test_014_connect_invalid_dst_port_exceeds(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	self.assertRaises(ValueError, 
	    lambda: hblock.connect("self", 1, "nop1", 0))

    def test_015_connect_dst_port_in_use(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	nop1 = gr.nop(gr.sizeof_int)
	hblock.define_component("nop1", nop1)
	hblock.connect("nop1", 0, "self", 0);
	self.assertRaises(ValueError, 
	    lambda: hblock.connect("nop1", 0, "self", 0))
    
    def test_016_connect_one_src_two_dst(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("src", gr.null_source(gr.sizeof_int))
	hblock.define_component("dst1", gr.null_sink(gr.sizeof_int))
	hblock.define_component("dst2", gr.null_sink(gr.sizeof_int))
	hblock.connect("src", 0, "dst1", 0)
	hblock.connect("src", 0, "dst2", 0)

    def test_017_connect_type_mismatch(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(1,1,gr.sizeof_int), 
				gr.io_signature(1,1,gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_char))
	self.assertRaises(ValueError, 
	    lambda: hblock.connect("nop1", 0, "self", 0))

    def test_018_check_topology(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(0,0,gr.sizeof_int), 
				gr.io_signature(0,0,gr.sizeof_int))
	hblock.check_topology(0, 0);
    """
    def test_019_validate(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(0,0,gr.sizeof_int), 
				gr.io_signature(0,0,gr.sizeof_int))
	runtime = gr.runtime(hblock)
	runtime.validate()
    
    def test_020_validate_1(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(0,0,gr.sizeof_int), 
				gr.io_signature(0,0,gr.sizeof_int))
	hblock.define_component("src", gr.null_source(gr.sizeof_int))
	hblock.define_component("dst1", gr.null_sink(gr.sizeof_int))
	hblock.define_component("dst2", gr.null_sink(gr.sizeof_int))
	hblock.connect("src", 0, "dst1", 0)
	hblock.connect("src", 0, "dst2", 0)			
	runtime = gr.runtime(hblock)
	runtime.validate()

    def test_021_validate_2(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(0,0,gr.sizeof_int), 
				gr.io_signature(0,0,gr.sizeof_int))
	hblock.define_component("src1", gr.null_source(gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	hblock.define_component("dst1", gr.null_sink(gr.sizeof_int))
	hblock.define_component("dst2", gr.null_sink(gr.sizeof_int))
	hblock.connect("src1", 0, "nop1", 0)
	hblock.connect("src1", 0, "nop1", 1)
	hblock.connect("nop1", 0, "dst1", 0)
	hblock.connect("nop1", 1, "dst2", 0)
	runtime = gr.runtime(hblock)
	runtime.validate()
        
    def test_022_validate_3(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(0,0,gr.sizeof_int), 
				gr.io_signature(0,0,gr.sizeof_int))
	hblock.define_component("src1", gr.null_source(gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	hblock.define_component("dst1", gr.null_sink(gr.sizeof_int))
	hblock.define_component("dst2", gr.null_sink(gr.sizeof_int))
	hblock.connect("src1", 0, "nop1", 0)
	hblock.connect("src1", 0, "nop1", 2)
	hblock.connect("nop1", 0, "dst1", 0)
	hblock.connect("nop1", 1, "dst2", 0)
	runtime = gr.runtime(hblock)
	self.assertRaises(RuntimeError,
	    lambda: runtime.validate())
        
    def test_023_validate_4(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(0,0,gr.sizeof_int), 
				gr.io_signature(0,0,gr.sizeof_int))
	hblock.define_component("src1", gr.null_source(gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	hblock.define_component("dst1", gr.null_sink(gr.sizeof_int))
	hblock.define_component("dst2", gr.null_sink(gr.sizeof_int))
	hblock.connect("src1", 0, "nop1", 0)
	hblock.connect("src1", 0, "nop1", 1)
	hblock.connect("nop1", 0, "dst1", 0)
	hblock.connect("nop1", 2, "dst2", 0)
	runtime = gr.runtime(hblock)
	self.assertRaises(RuntimeError,
	    lambda: runtime.validate())
        
    def test_024_validate_5(self):
	hblock = gr.hier_block2("test_block", 
				gr.io_signature(0,0,gr.sizeof_int), 
				gr.io_signature(0,0,gr.sizeof_int))
	hblock.define_component("src1", gr.null_source(gr.sizeof_int))
	hblock.define_component("nop1", gr.nop(gr.sizeof_int))
	hblock.define_component("dst1", gr.null_sink(gr.sizeof_int))
	hblock.define_component("dst2", gr.null_sink(gr.sizeof_int))
	hblock.connect("src1", 0, "nop1", 0)
	hblock.connect("src1", 0, "nop1", 1)
	hblock.connect("nop1", 0, "dst1", 0)
	hblock.connect("nop1", 1, "dst2", 0)
	runtime = gr.runtime(hblock)
	runtime.validate()
	# Pending implementation of disconnect
	# hblock.disconnect("src1", 0, "nop1", 1)
	# runtime.validate()	
	# self.assertRaises(ValueError,
	#     lambda: hblock.disconnect("src1", 0, "nop1", 1))
	"""
	        
if __name__ == "__main__":
    gr_unittest.main()

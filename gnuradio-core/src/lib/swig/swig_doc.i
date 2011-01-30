%feature("docstring") gr_py_msg_queue__insert_tail "Insert message at tail of queue.

Block if queue if full.

Params: (msg)"

%feature("docstring") gr_py_msg_queue__delete_head "Delete message from head of queue and return it. Block if no message is available.

Params: (NONE)"

%feature("docstring") atsc_bit_timing_loop "ATSC BitTimingLoop3

This class accepts a single real input and produces two outputs, the raw symbol (float) and the tag (atsc_syminfo)."

%feature("docstring") atsc_bit_timing_loop::atsc_bit_timing_loop "

Params: (NONE)"

%feature("docstring") atsc_bit_timing_loop::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_bit_timing_loop::reset "

Params: (NONE)"

%feature("docstring") atsc_bit_timing_loop::~atsc_bit_timing_loop "

Params: (NONE)"

%feature("docstring") atsc_bit_timing_loop::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") atsc_bit_timing_loop::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") atsc_bit_timing_loop::set_mu "

Params: (a_mu)"

%feature("docstring") atsc_bit_timing_loop::set_no_update "

Params: (a_no_update)"

%feature("docstring") atsc_bit_timing_loop::set_loop_filter_tap "

Params: (tap)"

%feature("docstring") atsc_bit_timing_loop::set_timing_rate "

Params: (rate)"

%feature("docstring") atsc_make_bit_timing_loop "Creates a bit_timing_loop block.

ATSC BitTimingLoop3

This class accepts a single real input and produces two outputs, the raw symbol (float) and the tag (atsc_syminfo).

Params: (NONE)"

%feature("docstring") atsc_deinterleaver "Deinterleave RS encoded ATSC data ( atsc_mpeg_packet_rs_encoded --> atsc_mpeg_packet_rs_encoded)

input: atsc_mpeg_packet_rs_encoded; output: atsc_mpeg_packet_rs_encoded."

%feature("docstring") atsc_deinterleaver::atsc_deinterleaver "

Params: (NONE)"

%feature("docstring") atsc_deinterleaver::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_deinterleaver::reset "

Params: (NONE)"

%feature("docstring") atsc_make_deinterleaver "Creates a deinterleaver block.

Deinterleave RS encoded ATSC data ( atsc_mpeg_packet_rs_encoded --> atsc_mpeg_packet_rs_encoded)

input: atsc_mpeg_packet_rs_encoded; output: atsc_mpeg_packet_rs_encoded.

Params: (NONE)"

%feature("docstring") atsc_depad "depad mpeg ts packets from 256 byte atsc_mpeg_packet to 188 byte char

input: atsc_mpeg_packet; output: unsigned char"

%feature("docstring") atsc_depad::atsc_depad "

Params: (NONE)"

%feature("docstring") atsc_depad::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_depad::reset "

Params: (NONE)"

%feature("docstring") atsc_make_depad "Creates a depad block.

depad mpeg ts packets from 256 byte atsc_mpeg_packet to 188 byte char

input: atsc_mpeg_packet; output: unsigned char

Params: (NONE)"

%feature("docstring") atsc_derandomizer "\"dewhiten\" incoming mpeg transport stream packets

input: atsc_mpeg_packet_no_sync; output: atsc_mpeg_packet;"

%feature("docstring") atsc_derandomizer::atsc_derandomizer "

Params: (NONE)"

%feature("docstring") atsc_derandomizer::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_derandomizer::reset "

Params: (NONE)"

%feature("docstring") atsc_make_derandomizer "Creates a derandomizer block.

\"dewhiten\" incoming mpeg transport stream packets

input: atsc_mpeg_packet_no_sync; output: atsc_mpeg_packet;

Params: (NONE)"

%feature("docstring") atsc_ds_to_softds "Debug glue routine (atsc_data_segment --> atsc_soft_data_segment)

input: atsc_data_segment; output: atsc_soft_data_segment."

%feature("docstring") atsc_ds_to_softds::atsc_ds_to_softds "

Params: (NONE)"

%feature("docstring") atsc_ds_to_softds::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_ds_to_softds::map_to_soft_symbols "

Params: (out, in)"

%feature("docstring") atsc_ds_to_softds::reset "

Params: (NONE)"

%feature("docstring") atsc_make_ds_to_softds "Creates a ds_to_softds block.

Debug glue routine (atsc_data_segment --> atsc_soft_data_segment)

input: atsc_data_segment; output: atsc_soft_data_segment.

Params: (NONE)"

%feature("docstring") atsc_equalizer "ATSC equalizer (float,syminfo --> float,syminfo)

first inputs are data samples, second inputs are tags. first outputs are equalized data samples, second outputs are tags."

%feature("docstring") atsc_equalizer::atsc_equalizer "

Params: (NONE)"

%feature("docstring") atsc_equalizer::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") atsc_equalizer::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_equalizer::reset "

Params: (NONE)"

%feature("docstring") atsc_equalizer::~atsc_equalizer "

Params: (NONE)"

%feature("docstring") atsc_make_equalizer "Creates a equalizer block.

ATSC equalizer (float,syminfo --> float,syminfo)

first inputs are data samples, second inputs are tags. first outputs are equalized data samples, second outputs are tags.

Params: (NONE)"

%feature("docstring") atsc_field_sync_demux "ATSC Field Sync Demux.

This class accepts 1 stream of floats (data), and 1 stream of tags (syminfo). * It outputs one stream of atsc_soft_data_segment packets"

%feature("docstring") atsc_field_sync_demux::atsc_field_sync_demux "

Params: (NONE)"

%feature("docstring") atsc_field_sync_demux::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") atsc_field_sync_demux::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") atsc_field_sync_demux::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_field_sync_demux::reset "

Params: (NONE)"

%feature("docstring") atsc_make_field_sync_demux "Creates a field_sync_demux block.

ATSC Field Sync Demux.

This class accepts 1 stream of floats (data), and 1 stream of tags (syminfo). * It outputs one stream of atsc_soft_data_segment packets

Params: (NONE)"

%feature("docstring") atsc_field_sync_mux "Insert ATSC Field Syncs as required (atsc_data_segment --> atsc_data_segment)

input: atsc_data_segment; output: atsc_data_segment."

%feature("docstring") atsc_field_sync_mux::atsc_field_sync_mux "

Params: (NONE)"

%feature("docstring") atsc_field_sync_mux::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") atsc_field_sync_mux::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_field_sync_mux::reset "

Params: (NONE)"

%feature("docstring") atsc_make_field_sync_mux "Creates a field_sync_mux block.

Insert ATSC Field Syncs as required (atsc_data_segment --> atsc_data_segment)

input: atsc_data_segment; output: atsc_data_segment.

Params: (NONE)"

%feature("docstring") atsc_fpll "ATSC FPLL (2nd Version)

A/D --> GrFIRfilterFFF ----> GrAtscFPLL ---->

We use GrFIRfilterFFF to bandpass filter the signal of interest.

This class accepts a single real input and produces a single real output"

%feature("docstring") atsc_fpll::atsc_fpll "

Params: (NONE)"

%feature("docstring") atsc_fpll::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_fpll::reset "

Params: (NONE)"

%feature("docstring") atsc_fpll::initialize "

Params: (NONE)"

%feature("docstring") atsc_make_fpll "Creates a fpll block.

ATSC FPLL (2nd Version)

A/D --> GrFIRfilterFFF ----> GrAtscFPLL ---->

We use GrFIRfilterFFF to bandpass filter the signal of interest.

This class accepts a single real input and produces a single real output

Params: (NONE)"

%feature("docstring") atsc_fs_checker "ATSC field sync checker (float,syminfo --> float,syminfo)

first output is delayed version of input. second output is set of tags, one-for-one with first output."

%feature("docstring") atsc_fs_checker::atsc_fs_checker "

Params: (NONE)"

%feature("docstring") atsc_fs_checker::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_fs_checker::reset "

Params: (NONE)"

%feature("docstring") atsc_fs_checker::~atsc_fs_checker "

Params: (NONE)"

%feature("docstring") atsc_make_fs_checker "Creates a fs_checker block.

ATSC field sync checker (float,syminfo --> float,syminfo)

first output is delayed version of input. second output is set of tags, one-for-one with first output.

Params: (NONE)"

%feature("docstring") atsc_interleaver "Interleave RS encoded ATSC data ( atsc_mpeg_packet_rs_encoded --> atsc_mpeg_packet_rs_encoded)*

input: atsc_mpeg_packet_rs_encoded; output: atsc_mpeg_packet_rs_encoded."

%feature("docstring") atsc_interleaver::atsc_interleaver "

Params: (NONE)"

%feature("docstring") atsc_interleaver::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_interleaver::reset "

Params: (NONE)"

%feature("docstring") atsc_make_interleaver "Creates a interleaver block.

Interleave RS encoded ATSC data ( atsc_mpeg_packet_rs_encoded --> atsc_mpeg_packet_rs_encoded)*

input: atsc_mpeg_packet_rs_encoded; output: atsc_mpeg_packet_rs_encoded.

Params: (NONE)"

%feature("docstring") atsc_pad "pad mpeg ts packets from 188 byte char to to 256 byte atsc_mpeg_packet

input: unsigned char; output: atsc_mpeg_packet"

%feature("docstring") atsc_pad::atsc_pad "

Params: (NONE)"

%feature("docstring") atsc_pad::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") atsc_pad::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_pad::reset "

Params: (NONE)"

%feature("docstring") atsc_make_pad "Creates a pad block.

pad mpeg ts packets from 188 byte char to to 256 byte atsc_mpeg_packet

input: unsigned char; output: atsc_mpeg_packet

Params: (NONE)"

%feature("docstring") atsc_randomizer "\"Whiten\" incoming mpeg transport stream packets

input: atsc_mpeg_packet; output: atsc_mpeg_packet_no_sync"

%feature("docstring") atsc_randomizer::atsc_randomizer "

Params: (NONE)"

%feature("docstring") atsc_randomizer::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_randomizer::reset "

Params: (NONE)"

%feature("docstring") atsc_make_randomizer "Creates a randomizer block.

\"Whiten\" incoming mpeg transport stream packets

input: atsc_mpeg_packet; output: atsc_mpeg_packet_no_sync

Params: (NONE)"

%feature("docstring") atsc_rs_decoder "Reed-Solomon decoder for ATSC

input: atsc_mpeg_packet_rs_encoded; output: atsc_mpeg_packet_no_sync."

%feature("docstring") atsc_rs_decoder::atsc_rs_decoder "

Params: (NONE)"

%feature("docstring") atsc_rs_decoder::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_rs_decoder::reset "

Params: (NONE)"

%feature("docstring") atsc_make_rs_decoder "Creates a rs_decoder block.

Reed-Solomon decoder for ATSC

input: atsc_mpeg_packet_rs_encoded; output: atsc_mpeg_packet_no_sync.

Params: (NONE)"

%feature("docstring") atsc_rs_encoder "Reed-Solomon encoder for ATSC

input: atsc_mpeg_packet_no_sync; output: atsc_mpeg_packet_rs_encoded."

%feature("docstring") atsc_rs_encoder::atsc_rs_encoder "

Params: (NONE)"

%feature("docstring") atsc_rs_encoder::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_rs_encoder::reset "

Params: (NONE)"

%feature("docstring") atsc_make_rs_encoder "Creates a rs_encoder block.

Reed-Solomon encoder for ATSC

input: atsc_mpeg_packet_no_sync; output: atsc_mpeg_packet_rs_encoded.

Params: (NONE)"

%feature("docstring") atsc_trellis_encoder "ATSC 12-way interleaved trellis encoder (atsc_mpeg_packet_rs_encoded --> atsc_data_segment)

input: atsc_mpeg_packet_rs_encoded; output: atsc_data_segment."

%feature("docstring") atsc_trellis_encoder::atsc_trellis_encoder "

Params: (NONE)"

%feature("docstring") atsc_trellis_encoder::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_trellis_encoder::reset "

Params: (NONE)"

%feature("docstring") atsc_make_trellis_encoder "Creates a trellis_encoder block.

ATSC 12-way interleaved trellis encoder (atsc_mpeg_packet_rs_encoded --> atsc_data_segment)

input: atsc_mpeg_packet_rs_encoded; output: atsc_data_segment.

Params: (NONE)"

%feature("docstring") atsc_viterbi_decoder "ATSC 12-way interleaved viterbi decoder (atsc_soft_data_segment --> atsc_mpeg_packet_rs_encoded)

input: atsc_soft_data_segment; output: atsc_mpeg_packet_rs_encoded."

%feature("docstring") atsc_viterbi_decoder::atsc_viterbi_decoder "

Params: (NONE)"

%feature("docstring") atsc_viterbi_decoder::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") atsc_viterbi_decoder::reset "

Params: (NONE)"

%feature("docstring") atsc_make_viterbi_decoder "Creates a viterbi_decoder block.

ATSC 12-way interleaved viterbi decoder (atsc_soft_data_segment --> atsc_mpeg_packet_rs_encoded)

input: atsc_soft_data_segment; output: atsc_mpeg_packet_rs_encoded.

Params: (NONE)"

%feature("docstring") comedi_sink_s "sink using COMEDI

The sink has one input stream of signed short integers.

Input samples must be in the range [-32768,32767]."

%feature("docstring") comedi_sink_s::output_error_msg "

Params: (msg, err)"

%feature("docstring") comedi_sink_s::bail "

Params: (msg, err)"

%feature("docstring") comedi_sink_s::comedi_sink_s "

Params: (sampling_freq, device_name)"

%feature("docstring") comedi_sink_s::~comedi_sink_s "

Params: (NONE)"

%feature("docstring") comedi_sink_s::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") comedi_sink_s::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") comedi_make_sink_s "Creates a sink_s block.

sink using COMEDI

The sink has one input stream of signed short integers.

Input samples must be in the range [-32768,32767].

Params: (sampling_freq, device_name)"

%feature("docstring") comedi_source_s "source using COMEDI

The source has one to many input stream of signed short integers.

Output samples will be in the range [-32768,32767]."

%feature("docstring") comedi_source_s::output_error_msg "

Params: (msg, err)"

%feature("docstring") comedi_source_s::bail "

Params: (msg, err)"

%feature("docstring") comedi_source_s::comedi_source_s "

Params: (sampling_freq, device_name)"

%feature("docstring") comedi_source_s::~comedi_source_s "

Params: (NONE)"

%feature("docstring") comedi_source_s::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") comedi_source_s::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") comedi_make_source_s "Creates a source_s block.

source using COMEDI

The source has one to many input stream of signed short integers.

Output samples will be in the range [-32768,32767].

Params: (sampling_freq, device_name)"

%feature("docstring") gcell_fft_vcc "Compute forward or reverse FFT. complex vector in / complex vector out.

Concrete class that uses gcell to offload FFT to SPEs."

%feature("docstring") gcell_fft_vcc::gcell_fft_vcc "

Params: (fft_size, forward, window, shift)"

%feature("docstring") gcell_fft_vcc::~gcell_fft_vcc "

Params: (NONE)"

%feature("docstring") gcell_fft_vcc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gcell_make_fft_vcc "Creates a fft_vcc block.

Compute forward or reverse FFT. complex vector in / complex vector out.

Concrete class that uses gcell to offload FFT to SPEs.

Params: (fft_size, forward, window, shift)"

%feature("docstring") gr_add_cc "output = sum (input_0, input_1, ...)

Add across all input streams."

%feature("docstring") gr_add_cc::gr_add_cc "

Params: (vlen)"

%feature("docstring") gr_add_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_cc "Creates a add_cc block.

output = sum (input_0, input_1, ...)

Add across all input streams.

Params: (vlen)"

%feature("docstring") gr_add_const_cc "output = input + constant"

%feature("docstring") gr_add_const_cc::gr_add_const_cc "

Params: (k)"

%feature("docstring") gr_add_const_cc::k "

Params: (NONE)"

%feature("docstring") gr_add_const_cc::set_k "

Params: (k)"

%feature("docstring") gr_add_const_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_const_cc "Creates a add_const_cc block.

output = input + constant

Params: (k)"

%feature("docstring") gr_add_const_ff "output = input + constant"

%feature("docstring") gr_add_const_ff::gr_add_const_ff "

Params: (k)"

%feature("docstring") gr_add_const_ff::k "

Params: (NONE)"

%feature("docstring") gr_add_const_ff::set_k "

Params: (k)"

%feature("docstring") gr_add_const_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_const_ff "Creates a add_const_ff block.

output = input + constant

Params: (k)"

%feature("docstring") gr_add_const_ii "output = input + constant"

%feature("docstring") gr_add_const_ii::gr_add_const_ii "

Params: (k)"

%feature("docstring") gr_add_const_ii::k "

Params: (NONE)"

%feature("docstring") gr_add_const_ii::set_k "

Params: (k)"

%feature("docstring") gr_add_const_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_const_ii "Creates a add_const_ii block.

output = input + constant

Params: (k)"

%feature("docstring") gr_add_const_sf "output = input + constant"

%feature("docstring") gr_add_const_sf::gr_add_const_sf "

Params: (k)"

%feature("docstring") gr_add_const_sf::k "

Params: (NONE)"

%feature("docstring") gr_add_const_sf::set_k "

Params: (k)"

%feature("docstring") gr_add_const_sf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_const_sf "Creates a add_const_sf block.

output = input + constant

Params: (k)"

%feature("docstring") gr_add_const_ss "output = input + constant"

%feature("docstring") gr_add_const_ss::gr_add_const_ss "

Params: (k)"

%feature("docstring") gr_add_const_ss::k "

Params: (NONE)"

%feature("docstring") gr_add_const_ss::set_k "

Params: (k)"

%feature("docstring") gr_add_const_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_const_ss "Creates a add_const_ss block.

output = input + constant

Params: (k)"

%feature("docstring") gr_add_const_vcc "output vector = input vector + constant vector"

%feature("docstring") gr_add_const_vcc::gr_add_const_vcc "

Params: (k)"

%feature("docstring") gr_add_const_vcc::k "

Params: (NONE)"

%feature("docstring") gr_add_const_vcc::set_k "

Params: (k)"

%feature("docstring") gr_add_const_vcc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_const_vcc "Creates a add_const_vcc block.

output vector = input vector + constant vector

Params: (k)"

%feature("docstring") gr_add_const_vff "output vector = input vector + constant vector"

%feature("docstring") gr_add_const_vff::gr_add_const_vff "

Params: (k)"

%feature("docstring") gr_add_const_vff::k "

Params: (NONE)"

%feature("docstring") gr_add_const_vff::set_k "

Params: (k)"

%feature("docstring") gr_add_const_vff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_const_vff "Creates a add_const_vff block.

output vector = input vector + constant vector

Params: (k)"

%feature("docstring") gr_add_const_vii "output vector = input vector + constant vector"

%feature("docstring") gr_add_const_vii::gr_add_const_vii "

Params: (k)"

%feature("docstring") gr_add_const_vii::k "

Params: (NONE)"

%feature("docstring") gr_add_const_vii::set_k "

Params: (k)"

%feature("docstring") gr_add_const_vii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_const_vii "Creates a add_const_vii block.

output vector = input vector + constant vector

Params: (k)"

%feature("docstring") gr_add_const_vss "output vector = input vector + constant vector"

%feature("docstring") gr_add_const_vss::gr_add_const_vss "

Params: (k)"

%feature("docstring") gr_add_const_vss::k "

Params: (NONE)"

%feature("docstring") gr_add_const_vss::set_k "

Params: (k)"

%feature("docstring") gr_add_const_vss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_const_vss "Creates a add_const_vss block.

output vector = input vector + constant vector

Params: (k)"

%feature("docstring") gr_add_ff "output = sum (input_0, input_1, ...)

Add across all input streams."

%feature("docstring") gr_add_ff::gr_add_ff "

Params: (vlen)"

%feature("docstring") gr_add_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_ff "Creates a add_ff block.

output = sum (input_0, input_1, ...)

Add across all input streams.

Params: (vlen)"

%feature("docstring") gr_add_ii "output = sum (input_0, input_1, ...)

Add across all input streams."

%feature("docstring") gr_add_ii::gr_add_ii "

Params: (vlen)"

%feature("docstring") gr_add_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_ii "Creates a add_ii block.

output = sum (input_0, input_1, ...)

Add across all input streams.

Params: (vlen)"

%feature("docstring") gr_add_ss "output = sum (input_0, input_1, ...)

Add across all input streams."

%feature("docstring") gr_add_ss::gr_add_ss "

Params: (vlen)"

%feature("docstring") gr_add_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_add_ss "Creates a add_ss block.

output = sum (input_0, input_1, ...)

Add across all input streams.

Params: (vlen)"

%feature("docstring") gr_additive_scrambler_bb "Scramble an input stream using an LFSR. This block works on the LSB only of the input data stream, i.e., on an \"unpacked binary\" stream, and produces the same format on its output.


The scrambler works by XORing the incoming bit stream by the output of the LFSR. Optionally, after 'count' bits have been processed, the shift register is reset to the seed value. This allows processing fixed length vectors of samples."

%feature("docstring") gr_additive_scrambler_bb::gr_additive_scrambler_bb "

Params: (mask, seed, len, count)"

%feature("docstring") gr_additive_scrambler_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_additive_scrambler_bb "Creates a additive_scrambler_bb block.

Scramble an input stream using an LFSR. This block works on the LSB only of the input data stream, i.e., on an \"unpacked binary\" stream, and produces the same format on its output.


The scrambler works by XORing the incoming bit stream by the output of the LFSR. Optionally, after 'count' bits have been processed, the shift register is reset to the seed value. This allows processing fixed length vectors of samples.

Params: (mask, seed, len, count)"

%feature("docstring") gr_agc2_cc "high performance Automatic Gain Control class

For Power the absolute value of the complex number is used."

%feature("docstring") gr_agc2_cc::gr_agc2_cc "

Params: (attack_rate, decay_rate, reference, gain, max_gain)"

%feature("docstring") gr_agc2_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_agc2_cc "Creates a agc2_cc block.

high performance Automatic Gain Control class

For Power the absolute value of the complex number is used.

Params: (attack_rate, decay_rate, reference, gain, max_gain)"

%feature("docstring") gr_agc2_ff "high performance Automatic Gain Control class

Power is approximated by absolute value"

%feature("docstring") gr_agc2_ff::gr_agc2_ff "

Params: (attack_rate, decay_rate, reference, gain, max_gain)"

%feature("docstring") gr_agc2_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_agc2_ff "Creates a agc2_ff block.

high performance Automatic Gain Control class

Power is approximated by absolute value

Params: (attack_rate, decay_rate, reference, gain, max_gain)"

%feature("docstring") gr_agc_cc "high performance Automatic Gain Control class

For Power the absolute value of the complex number is used."

%feature("docstring") gr_agc_cc::gr_agc_cc "

Params: (rate, reference, gain, max_gain)"

%feature("docstring") gr_agc_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_agc_cc "Creates a agc_cc block.

high performance Automatic Gain Control class

For Power the absolute value of the complex number is used.

Params: (rate, reference, gain, max_gain)"

%feature("docstring") gr_agc_ff "high performance Automatic Gain Control class

Power is approximated by absolute value"

%feature("docstring") gr_agc_ff::gr_agc_ff "

Params: (rate, reference, gain, max_gain)"

%feature("docstring") gr_agc_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_agc_ff "Creates a agc_ff block.

high performance Automatic Gain Control class

Power is approximated by absolute value

Params: (rate, reference, gain, max_gain)"



%feature("docstring") gr_align_on_samplenumbers_ss::gr_align_on_samplenumbers_ss "

Params: (nchan, align_interval)"

%feature("docstring") gr_align_on_samplenumbers_ss::~gr_align_on_samplenumbers_ss "

Params: (NONE)"

%feature("docstring") gr_align_on_samplenumbers_ss::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_align_on_samplenumbers_ss::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_align_on_samplenumbers_ss::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_align_on_samplenumbers_ss "Creates a align_on_samplenumbers_ss block.



Params: (nchan, align_interval)"

%feature("docstring") gr_and_bb "output = input_0 & input_1 & , ... & input_N)

bitwise boolean and across all input streams."

%feature("docstring") gr_and_bb::gr_and_bb "

Params: (NONE)"

%feature("docstring") gr_and_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_and_bb "Creates a and_bb block.

output = input_0 & input_1 & , ... & input_N)

bitwise boolean and across all input streams.

Params: (NONE)"

%feature("docstring") gr_and_const_bb "output_N = input_N & value

bitwise boolean and of const to the data stream."

%feature("docstring") gr_and_const_bb::gr_and_const_bb "

Params: (k)"

%feature("docstring") gr_and_const_bb::k "

Params: (NONE)"

%feature("docstring") gr_and_const_bb::set_k "

Params: (k)"

%feature("docstring") gr_and_const_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_and_const_bb "Creates a and_const_bb block.

output_N = input_N & value

bitwise boolean and of const to the data stream.

Params: (k)"

%feature("docstring") gr_and_const_ii "output_N = input_N & value

bitwise boolean and of const to the data stream."

%feature("docstring") gr_and_const_ii::gr_and_const_ii "

Params: (k)"

%feature("docstring") gr_and_const_ii::k "

Params: (NONE)"

%feature("docstring") gr_and_const_ii::set_k "

Params: (k)"

%feature("docstring") gr_and_const_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_and_const_ii "Creates a and_const_ii block.

output_N = input_N & value

bitwise boolean and of const to the data stream.

Params: (k)"

%feature("docstring") gr_and_const_ss "output_N = input_N & value

bitwise boolean and of const to the data stream."

%feature("docstring") gr_and_const_ss::gr_and_const_ss "

Params: (k)"

%feature("docstring") gr_and_const_ss::k "

Params: (NONE)"

%feature("docstring") gr_and_const_ss::set_k "

Params: (k)"

%feature("docstring") gr_and_const_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_and_const_ss "Creates a and_const_ss block.

output_N = input_N & value

bitwise boolean and of const to the data stream.

Params: (k)"

%feature("docstring") gr_and_ii "output = input_0 & input_1 & , ... & input_N)

bitwise boolean and across all input streams."

%feature("docstring") gr_and_ii::gr_and_ii "

Params: (NONE)"

%feature("docstring") gr_and_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_and_ii "Creates a and_ii block.

output = input_0 & input_1 & , ... & input_N)

bitwise boolean and across all input streams.

Params: (NONE)"

%feature("docstring") gr_and_ss "output = input_0 & input_1 & , ... & input_N)

bitwise boolean and across all input streams."

%feature("docstring") gr_and_ss::gr_and_ss "

Params: (NONE)"

%feature("docstring") gr_and_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_and_ss "Creates a and_ss block.

output = input_0 & input_1 & , ... & input_N)

bitwise boolean and across all input streams.

Params: (NONE)"

%feature("docstring") gr_annotator_1to1 "1-to-1 stream annotator testing block. FOR TESTING PURPOSES ONLY.

This block creates tags to be sent downstream every 10,000 items it sees. The tags contain the name and ID of the instantiated block, use \"seq\" as a key, and have a counter that increments by 1 for every tag produced that is used as the tag's value. The tags are propagated using the 1-to-1 policy.

It also stores a copy of all tags it sees flow past it. These tags can be recalled externally with the data() member.

This block is only meant for testing and showing how to use the tags."

%feature("docstring") gr_annotator_1to1::~gr_annotator_1to1 "

Params: (NONE)"

%feature("docstring") gr_annotator_1to1::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_annotator_1to1::data "

Params: (NONE)"

%feature("docstring") gr_annotator_1to1::gr_annotator_1to1 "

Params: (when, sizeof_stream_item)"

%feature("docstring") gr_make_annotator_1to1 "Creates a annotator_1to1 block.

1-to-1 stream annotator testing block. FOR TESTING PURPOSES ONLY.

This block creates tags to be sent downstream every 10,000 items it sees. The tags contain the name and ID of the instantiated block, use \"seq\" as a key, and have a counter that increments by 1 for every tag produced that is used as the tag's value. The tags are propagated using the 1-to-1 policy.

It also stores a copy of all tags it sees flow past it. These tags can be recalled externally with the data() member.

This block is only meant for testing and showing how to use the tags.

Params: (when, sizeof_stream_item)"

%feature("docstring") gr_annotator_alltoall "All-to-all stream annotator testing block. FOR TESTING PURPOSES ONLY.

This block creates tags to be sent downstream every 10,000 items it sees. The tags contain the name and ID of the instantiated block, use \"seq\" as a key, and have a counter that increments by 1 for every tag produced that is used as the tag's value. The tags are propagated using the all-to-all policy.

It also stores a copy of all tags it sees flow past it. These tags can be recalled externally with the data() member.

This block is only meant for testing and showing how to use the tags."

%feature("docstring") gr_annotator_alltoall::~gr_annotator_alltoall "

Params: (NONE)"

%feature("docstring") gr_annotator_alltoall::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_annotator_alltoall::data "

Params: (NONE)"

%feature("docstring") gr_annotator_alltoall::gr_annotator_alltoall "

Params: (when, sizeof_stream_item)"

%feature("docstring") gr_make_annotator_alltoall "Creates a annotator_alltoall block.

All-to-all stream annotator testing block. FOR TESTING PURPOSES ONLY.

This block creates tags to be sent downstream every 10,000 items it sees. The tags contain the name and ID of the instantiated block, use \"seq\" as a key, and have a counter that increments by 1 for every tag produced that is used as the tag's value. The tags are propagated using the all-to-all policy.

It also stores a copy of all tags it sees flow past it. These tags can be recalled externally with the data() member.

This block is only meant for testing and showing how to use the tags.

Params: (when, sizeof_stream_item)"



%feature("docstring") gr_argmax_fs::gr_argmax_fs "

Params: (vlen)"

%feature("docstring") gr_argmax_fs::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_argmax_fs "Creates a argmax_fs block.



Params: (vlen)"



%feature("docstring") gr_argmax_is::gr_argmax_is "

Params: (vlen)"

%feature("docstring") gr_argmax_is::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_argmax_is "Creates a argmax_is block.



Params: (vlen)"



%feature("docstring") gr_argmax_ss::gr_argmax_ss "

Params: (vlen)"

%feature("docstring") gr_argmax_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_argmax_ss "Creates a argmax_ss block.



Params: (vlen)"

%feature("docstring") gr_bin_statistics_f "control scanning and record frequency domain statistics"

%feature("docstring") gr_bin_statistics_f::gr_bin_statistics_f "

Params: (vlen, msgq, tune, tune_delay, dwell_delay)"

%feature("docstring") gr_bin_statistics_f::enter_init "

Params: (NONE)"

%feature("docstring") gr_bin_statistics_f::enter_tune_delay "

Params: (NONE)"

%feature("docstring") gr_bin_statistics_f::enter_dwell_delay "

Params: (NONE)"

%feature("docstring") gr_bin_statistics_f::leave_dwell_delay "

Params: (NONE)"

%feature("docstring") gr_bin_statistics_f::vlen "

Params: (NONE)"

%feature("docstring") gr_bin_statistics_f::center_freq "

Params: (NONE)"

%feature("docstring") gr_bin_statistics_f::msgq "

Params: (NONE)"

%feature("docstring") gr_bin_statistics_f::reset_stats "

Params: (NONE)"

%feature("docstring") gr_bin_statistics_f::accrue_stats "

Params: (input)"

%feature("docstring") gr_bin_statistics_f::send_stats "

Params: (NONE)"

%feature("docstring") gr_bin_statistics_f::~gr_bin_statistics_f "

Params: (NONE)"

%feature("docstring") gr_bin_statistics_f::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_bin_statistics_f "Creates a bin_statistics_f block.

control scanning and record frequency domain statistics

Params: (vlen, msgq, tune, tune_delay, dwell_delay)"

%feature("docstring") gr_binary_slicer_fb "slice float binary symbol outputting 1 bit output

x < 0 --> 0 x >= 0 --> 1"

%feature("docstring") gr_binary_slicer_fb::gr_binary_slicer_fb "

Params: (NONE)"

%feature("docstring") gr_binary_slicer_fb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_binary_slicer_fb "Creates a binary_slicer_fb block.

slice float binary symbol outputting 1 bit output

x < 0 --> 0 x >= 0 --> 1

Params: (NONE)"

%feature("docstring") gr_block_detail "Implementation details to support the signal processing abstraction

This class contains implementation detail that should be \"out of sight\" of almost all users of GNU Radio. This decoupling also means that we can make changes to the guts without having to recompile everything."

%feature("docstring") gr_block_detail::~gr_block_detail "

Params: (NONE)"

%feature("docstring") gr_block_detail::ninputs "

Params: (NONE)"

%feature("docstring") gr_block_detail::noutputs "

Params: (NONE)"

%feature("docstring") gr_block_detail::sink_p "

Params: (NONE)"

%feature("docstring") gr_block_detail::source_p "

Params: (NONE)"

%feature("docstring") gr_block_detail::set_done "

Params: (done)"

%feature("docstring") gr_block_detail::done "

Params: (NONE)"

%feature("docstring") gr_block_detail::set_input "

Params: (which, reader)"

%feature("docstring") gr_block_detail::input "

Params: (which)"

%feature("docstring") gr_block_detail::set_output "

Params: (which, buffer)"

%feature("docstring") gr_block_detail::output "

Params: (which)"

%feature("docstring") gr_block_detail::consume "Tell the scheduler  of input stream  were consumed.

Params: (which_input, how_many_items)"

%feature("docstring") gr_block_detail::consume_each "Tell the scheduler  were consumed on each input stream.

Params: (how_many_items)"

%feature("docstring") gr_block_detail::produce "Tell the scheduler  were produced on output stream .

Params: (which_output, how_many_items)"

%feature("docstring") gr_block_detail::produce_each "Tell the scheduler  were produced on each output stream.

Params: (how_many_items)"

%feature("docstring") gr_block_detail::_post "Accept msg, place in queue, arrange for thread to be awakened if it's not already.

Params: (msg)"

%feature("docstring") gr_block_detail::nitems_read "

Params: (which_input)"

%feature("docstring") gr_block_detail::nitems_written "

Params: (which_output)"

%feature("docstring") gr_block_detail::add_item_tag "Adds a new tag to the given output stream.

This takes the input parameters and builds a PMT tuple from it. It then calls gr_buffer::add_item_tag(pmt::pmt_t t), which appends the tag onto its deque.

Params: (which_output, abs_offset, key, value, srcid)"

%feature("docstring") gr_block_detail::get_tags_in_range "Given a [start,end), returns a vector of all tags in the range.

Pass-through function to gr_buffer_reader to get a vector of tags in given range. Range of counts is from start to end-1.

Tags are tuples of: (item count, source id, key, value)

Params: (v, which_input, abs_start, abs_end)"

%feature("docstring") gr_block_detail::gr_block_detail "

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_block_detail "Creates a block_detail block.

Implementation details to support the signal processing abstraction

This class contains implementation detail that should be \"out of sight\" of almost all users of GNU Radio. This decoupling also means that we can make changes to the guts without having to recompile everything.

Params: (ninputs, noutputs)"

%feature("docstring") gr_buffer "Single writer, multiple reader fifo."

%feature("docstring") gr_buffer::~gr_buffer "

Params: (NONE)"

%feature("docstring") gr_buffer::space_available "return number of items worth of space available for writing

Params: (NONE)"

%feature("docstring") gr_buffer::bufsize "return size of this buffer in items

Params: (NONE)"

%feature("docstring") gr_buffer::write_pointer "return pointer to write buffer.

The return value points at space that can hold at least space_available() items.

Params: (NONE)"

%feature("docstring") gr_buffer::update_write_pointer "tell buffer that we wrote  into it

Params: (nitems)"

%feature("docstring") gr_buffer::set_done "

Params: (done)"

%feature("docstring") gr_buffer::done "

Params: (NONE)"

%feature("docstring") gr_buffer::link "Return the block that writes to this buffer.

Params: (NONE)"

%feature("docstring") gr_buffer::nreaders "

Params: (NONE)"

%feature("docstring") gr_buffer::reader "

Params: (index)"

%feature("docstring") gr_buffer::mutex "

Params: (NONE)"

%feature("docstring") gr_buffer::nitems_written "

Params: (NONE)"

%feature("docstring") gr_buffer::add_item_tag "Adds a new tag to the buffer.

Params: (tag)"

%feature("docstring") gr_buffer::prune_tags "Removes all tags before  from buffer.

Params: (max_time)"

%feature("docstring") gr_buffer::get_tags_begin "

Params: (NONE)"

%feature("docstring") gr_buffer::get_tags_end "

Params: (NONE)"

%feature("docstring") gr_buffer::index_add "

Params: (a, b)"

%feature("docstring") gr_buffer::index_sub "

Params: (a, b)"

%feature("docstring") gr_buffer::allocate_buffer "

Params: (nitems, sizeof_item)"

%feature("docstring") gr_buffer::gr_buffer "constructor is private. Use gr_make_buffer to create instances.

Allocate a buffer that holds at least  of size .


The total size of the buffer will be rounded up to a system dependent boundary. This is typically the system page size, but under MS windows is 64KB.

Params: (nitems, sizeof_item, link)"

%feature("docstring") gr_buffer::drop_reader "disassociate  from this buffer

Params: (reader)"

%feature("docstring") gr_make_buffer "Creates a buffer block.

Single writer, multiple reader fifo.

Params: (nitems, sizeof_item, link)"

%feature("docstring") gr_burst_tagger "output[i] = input[i]"

%feature("docstring") gr_burst_tagger::gr_burst_tagger "

Params: (itemsize)"

%feature("docstring") gr_burst_tagger::~gr_burst_tagger "

Params: (NONE)"

%feature("docstring") gr_burst_tagger::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_burst_tagger "Creates a burst_tagger block.

output[i] = input[i]

Params: (itemsize)"

%feature("docstring") gr_bytes_to_syms "Convert stream of bytes to stream of +/- 1 symbols

input: stream of bytes; output: stream of float.

This block is deprecated.

The combination of gr_packed_to_unpacked_bb followed by gr_chunks_to_symbols_bf or gr_chunks_to_symbols_bc handles the general case of mapping from a stream of bytes into arbitrary float or complex symbols."

%feature("docstring") gr_bytes_to_syms::gr_bytes_to_syms "

Params: (NONE)"

%feature("docstring") gr_bytes_to_syms::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_bytes_to_syms "Creates a bytes_to_syms block.

Convert stream of bytes to stream of +/- 1 symbols

input: stream of bytes; output: stream of float.

This block is deprecated.

The combination of gr_packed_to_unpacked_bb followed by gr_chunks_to_symbols_bf or gr_chunks_to_symbols_bc handles the general case of mapping from a stream of bytes into arbitrary float or complex symbols.

Params: (NONE)"

%feature("docstring") gr_channel_model "channel simulator"

%feature("docstring") gr_channel_model::gr_channel_model "

Params: (noise_voltage, frequency_offset, epsilon, taps, noise_seed)"

%feature("docstring") gr_channel_model::set_noise_voltage "

Params: (noise_voltage)"

%feature("docstring") gr_channel_model::set_frequency_offset "

Params: (frequency_offset)"

%feature("docstring") gr_channel_model::set_taps "

Params: (taps)"

%feature("docstring") gr_channel_model::set_timing_offset "

Params: (epsilon)"

%feature("docstring") gr_make_channel_model "Creates a channel_model block.

channel simulator

Params: (noise_voltage, frequency_offset, epsilon, taps, noise_seed)"

%feature("docstring") gr_char_to_float "Convert stream of chars to a stream of float."

%feature("docstring") gr_char_to_float::gr_char_to_float "

Params: (NONE)"

%feature("docstring") gr_char_to_float::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_char_to_float "Creates a char_to_float block.

Convert stream of chars to a stream of float.

Params: (NONE)"

%feature("docstring") gr_check_counting_s "sink that checks if its input stream consists of a counting sequence.

This sink is typically used to test the USRP \"Counting Mode\" or \"Counting mode 32 bit\"."

%feature("docstring") gr_check_counting_s::gr_check_counting_s "

Params: (do_32bit)"

%feature("docstring") gr_check_counting_s::enter_SEARCHING "

Params: (NONE)"

%feature("docstring") gr_check_counting_s::enter_LOCKED "

Params: (NONE)"

%feature("docstring") gr_check_counting_s::right "

Params: (NONE)"

%feature("docstring") gr_check_counting_s::wrong "

Params: (NONE)"

%feature("docstring") gr_check_counting_s::right_three_times "

Params: (NONE)"

%feature("docstring") gr_check_counting_s::wrong_three_times "

Params: (NONE)"

%feature("docstring") gr_check_counting_s::log_error "

Params: (expected, actual)"

%feature("docstring") gr_check_counting_s::log_error_32bit "

Params: (expected, actual)"

%feature("docstring") gr_check_counting_s::check_32bit "

Params: (noutput_items, in)"

%feature("docstring") gr_check_counting_s::check_16bit "

Params: (noutput_items, in)"

%feature("docstring") gr_check_counting_s::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_check_counting_s "Creates a check_counting_s block.

sink that checks if its input stream consists of a counting sequence.

This sink is typically used to test the USRP \"Counting Mode\" or \"Counting mode 32 bit\".

Params: (do_32bit)"

%feature("docstring") gr_check_lfsr_32k_s "sink that checks if its input stream consists of a lfsr_32k sequence.

This sink is typically used along with gr_lfsr_32k_source_s to test the USRP using its digital loopback mode."

%feature("docstring") gr_check_lfsr_32k_s::gr_check_lfsr_32k_s "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::enter_SEARCHING "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::enter_MATCH0 "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::enter_MATCH1 "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::enter_MATCH2 "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::enter_LOCKED "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::right "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::wrong "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::right_three_times "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::wrong_three_times "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::log_error "

Params: (expected, actual)"

%feature("docstring") gr_check_lfsr_32k_s::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_check_lfsr_32k_s::ntotal "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::nright "

Params: (NONE)"

%feature("docstring") gr_check_lfsr_32k_s::runlength "

Params: (NONE)"

%feature("docstring") gr_make_check_lfsr_32k_s "Creates a check_lfsr_32k_s block.

sink that checks if its input stream consists of a lfsr_32k sequence.

This sink is typically used along with gr_lfsr_32k_source_s to test the USRP using its digital loopback mode.

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_bc "Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of unsigned char; output: stream of gr_complex.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_chunks_to_symbols_bc::gr_chunks_to_symbols_bc "

Params: (symbol_table, D)"

%feature("docstring") gr_chunks_to_symbols_bc::D "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_bc::symbol_table "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_bc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_chunks_to_symbols_bc::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_chunks_to_symbols_bc "Creates a chunks_to_symbols_bc block.

Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of unsigned char; output: stream of gr_complex.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (symbol_table, D)"

%feature("docstring") gr_chunks_to_symbols_bf "Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of unsigned char; output: stream of float.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_chunks_to_symbols_bf::gr_chunks_to_symbols_bf "

Params: (symbol_table, D)"

%feature("docstring") gr_chunks_to_symbols_bf::D "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_bf::symbol_table "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_bf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_chunks_to_symbols_bf::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_chunks_to_symbols_bf "Creates a chunks_to_symbols_bf block.

Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of unsigned char; output: stream of float.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (symbol_table, D)"

%feature("docstring") gr_chunks_to_symbols_ic "Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of int; output: stream of gr_complex.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_chunks_to_symbols_ic::gr_chunks_to_symbols_ic "

Params: (symbol_table, D)"

%feature("docstring") gr_chunks_to_symbols_ic::D "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_ic::symbol_table "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_ic::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_chunks_to_symbols_ic::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_chunks_to_symbols_ic "Creates a chunks_to_symbols_ic block.

Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of int; output: stream of gr_complex.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (symbol_table, D)"

%feature("docstring") gr_chunks_to_symbols_if "Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of int; output: stream of float.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_chunks_to_symbols_if::gr_chunks_to_symbols_if "

Params: (symbol_table, D)"

%feature("docstring") gr_chunks_to_symbols_if::D "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_if::symbol_table "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_if::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_chunks_to_symbols_if::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_chunks_to_symbols_if "Creates a chunks_to_symbols_if block.

Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of int; output: stream of float.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (symbol_table, D)"

%feature("docstring") gr_chunks_to_symbols_sc "Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of short; output: stream of gr_complex.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_chunks_to_symbols_sc::gr_chunks_to_symbols_sc "

Params: (symbol_table, D)"

%feature("docstring") gr_chunks_to_symbols_sc::D "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_sc::symbol_table "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_sc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_chunks_to_symbols_sc::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_chunks_to_symbols_sc "Creates a chunks_to_symbols_sc block.

Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of short; output: stream of gr_complex.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (symbol_table, D)"

%feature("docstring") gr_chunks_to_symbols_sf "Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of short; output: stream of float.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_chunks_to_symbols_sf::gr_chunks_to_symbols_sf "

Params: (symbol_table, D)"

%feature("docstring") gr_chunks_to_symbols_sf::D "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_sf::symbol_table "

Params: (NONE)"

%feature("docstring") gr_chunks_to_symbols_sf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_chunks_to_symbols_sf::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_chunks_to_symbols_sf "Creates a chunks_to_symbols_sf block.

Map a stream of symbol indexes (unpacked bytes or shorts) to stream of float or complex onstellation points.in  dimensions ( = 1 by default)

input: stream of short; output: stream of float.

out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_XY handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (symbol_table, D)"

%feature("docstring") gr_clock_recovery_mm_cc "Mueller and M?ller (M&M) based clock recovery block with complex input, complex output.

This implements the Mueller and M?ller (M&M) discrete-time error-tracking synchronizer. The complex version here is based on: Modified Mueller and Muller clock recovery circuit Based: G. R. Danesfahani, T.G. Jeans, \"Optimisation of modified Mueller and Muller 
    algorithm,\" Electronics Letters, Vol. 31, no. 13, 22 June 1995, pp. 1032 - 1033."

%feature("docstring") gr_clock_recovery_mm_cc::~gr_clock_recovery_mm_cc "

Params: (NONE)"

%feature("docstring") gr_clock_recovery_mm_cc::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_clock_recovery_mm_cc::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_clock_recovery_mm_cc::mu "

Params: (NONE)"

%feature("docstring") gr_clock_recovery_mm_cc::omega "

Params: (NONE)"

%feature("docstring") gr_clock_recovery_mm_cc::gain_mu "

Params: (NONE)"

%feature("docstring") gr_clock_recovery_mm_cc::gain_omega "

Params: (NONE)"

%feature("docstring") gr_clock_recovery_mm_cc::set_verbose "

Params: (verbose)"

%feature("docstring") gr_clock_recovery_mm_cc::set_gain_mu "

Params: (gain_mu)"

%feature("docstring") gr_clock_recovery_mm_cc::set_gain_omega "

Params: (gain_omega)"

%feature("docstring") gr_clock_recovery_mm_cc::set_mu "

Params: (mu)"

%feature("docstring") gr_clock_recovery_mm_cc::set_omega "

Params: (omega)"

%feature("docstring") gr_clock_recovery_mm_cc::gr_clock_recovery_mm_cc "

Params: (omega, gain_omega, mu, gain_mu, omega_relative_limi)"

%feature("docstring") gr_clock_recovery_mm_cc::slicer_0deg "

Params: (sample)"

%feature("docstring") gr_clock_recovery_mm_cc::slicer_45deg "

Params: (sample)"

%feature("docstring") gr_make_clock_recovery_mm_cc "Creates a clock_recovery_mm_cc block.

Mueller and M?ller (M&M) based clock recovery block with complex input, complex output.

This implements the Mueller and M?ller (M&M) discrete-time error-tracking synchronizer. The complex version here is based on: Modified Mueller and Muller clock recovery circuit Based: G. R. Danesfahani, T.G. Jeans, \"Optimisation of modified Mueller and Muller 
    algorithm,\" Electronics Letters, Vol. 31, no. 13, 22 June 1995, pp. 1032 - 1033.

Params: (omega, gain_omega, mu, gain_mu, omega_relative_limi)"

%feature("docstring") gr_clock_recovery_mm_ff "Mueller and M?ller (M&M) based clock recovery block with float input, float output.

This implements the Mueller and M?ller (M&M) discrete-time error-tracking synchronizer.

See \"Digital Communication Receivers: Synchronization, Channel
 Estimation and Signal Processing\" by Heinrich Meyr, Marc Moeneclaey, & Stefan Fechtel. ISBN 0-471-50275-8."

%feature("docstring") gr_clock_recovery_mm_ff::~gr_clock_recovery_mm_ff "

Params: (NONE)"

%feature("docstring") gr_clock_recovery_mm_ff::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_clock_recovery_mm_ff::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_clock_recovery_mm_ff::mu "

Params: (NONE)"

%feature("docstring") gr_clock_recovery_mm_ff::omega "

Params: (NONE)"

%feature("docstring") gr_clock_recovery_mm_ff::gain_mu "

Params: (NONE)"

%feature("docstring") gr_clock_recovery_mm_ff::gain_omega "

Params: (NONE)"

%feature("docstring") gr_clock_recovery_mm_ff::set_gain_mu "

Params: (gain_mu)"

%feature("docstring") gr_clock_recovery_mm_ff::set_gain_omega "

Params: (gain_omega)"

%feature("docstring") gr_clock_recovery_mm_ff::set_mu "

Params: (mu)"

%feature("docstring") gr_clock_recovery_mm_ff::set_omega "

Params: (omega)"

%feature("docstring") gr_clock_recovery_mm_ff::gr_clock_recovery_mm_ff "

Params: (omega, gain_omega, mu, gain_mu, omega_relative_limit)"

%feature("docstring") gr_make_clock_recovery_mm_ff "Creates a clock_recovery_mm_ff block.

Mueller and M?ller (M&M) based clock recovery block with float input, float output.

This implements the Mueller and M?ller (M&M) discrete-time error-tracking synchronizer.

See \"Digital Communication Receivers: Synchronization, Channel
 Estimation and Signal Processing\" by Heinrich Meyr, Marc Moeneclaey, & Stefan Fechtel. ISBN 0-471-50275-8.

Params: (omega, gain_omega, mu, gain_mu, omega_relative_limit)"

%feature("docstring") gr_cma_equalizer_cc "Implements constant modulus adaptive filter on complex stream."

%feature("docstring") gr_cma_equalizer_cc::gr_cma_equalizer_cc "

Params: (num_taps, modulus, mu)"

%feature("docstring") gr_cma_equalizer_cc::error "

Params: (out)"

%feature("docstring") gr_cma_equalizer_cc::update_tap "

Params: (tap, in)"

%feature("docstring") gr_make_cma_equalizer_cc "Creates a cma_equalizer_cc block.

Implements constant modulus adaptive filter on complex stream.

Params: (num_taps, modulus, mu)"

%feature("docstring") gr_complex_to_arg "complex in, angle out (float)"

%feature("docstring") gr_complex_to_arg::gr_complex_to_arg "

Params: (vlen)"

%feature("docstring") gr_complex_to_arg::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_complex_to_arg "Creates a complex_to_arg block.

complex in, angle out (float)

Params: (vlen)"

%feature("docstring") gr_complex_to_float "convert a stream of gr_complex to 1 or 2 streams of float"

%feature("docstring") gr_complex_to_float::gr_complex_to_float "

Params: (vlen)"

%feature("docstring") gr_complex_to_float::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_complex_to_float "Creates a complex_to_float block.

convert a stream of gr_complex to 1 or 2 streams of float

Params: (vlen)"

%feature("docstring") gr_complex_to_imag "complex in, imaginary out (float)"

%feature("docstring") gr_complex_to_imag::gr_complex_to_imag "

Params: (vlen)"

%feature("docstring") gr_complex_to_imag::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_complex_to_imag "Creates a complex_to_imag block.

complex in, imaginary out (float)

Params: (vlen)"

%feature("docstring") gr_complex_to_interleaved_short "Convert stream of complex to a stream of interleaved shorts."

%feature("docstring") gr_complex_to_interleaved_short::gr_complex_to_interleaved_short "

Params: (NONE)"

%feature("docstring") gr_complex_to_interleaved_short::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_complex_to_interleaved_short "Creates a complex_to_interleaved_short block.

Convert stream of complex to a stream of interleaved shorts.

Params: (NONE)"

%feature("docstring") gr_complex_to_mag "complex in, magnitude out (float)"

%feature("docstring") gr_complex_to_mag::gr_complex_to_mag "

Params: (vlen)"

%feature("docstring") gr_complex_to_mag::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_complex_to_mag "Creates a complex_to_mag block.

complex in, magnitude out (float)

Params: (vlen)"

%feature("docstring") gr_complex_to_mag_squared "complex in, magnitude squared out (float)"

%feature("docstring") gr_complex_to_mag_squared::gr_complex_to_mag_squared "

Params: (vlen)"

%feature("docstring") gr_complex_to_mag_squared::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_complex_to_mag_squared "Creates a complex_to_mag_squared block.

complex in, magnitude squared out (float)

Params: (vlen)"

%feature("docstring") gr_complex_to_real "complex in, real out (float)"

%feature("docstring") gr_complex_to_real::gr_complex_to_real "

Params: (vlen)"

%feature("docstring") gr_complex_to_real::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_complex_to_real "Creates a complex_to_real block.

complex in, real out (float)

Params: (vlen)"

%feature("docstring") gr_conjugate_cc "output = complex conjugate of input"

%feature("docstring") gr_conjugate_cc::gr_conjugate_cc "

Params: (NONE)"

%feature("docstring") gr_conjugate_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_conjugate_cc "Creates a conjugate_cc block.

output = complex conjugate of input

Params: (NONE)"



%feature("docstring") gr_constellation::gr_constellation "

Params: (constellation)"

%feature("docstring") gr_constellation::points "Returns the set of points in this constellation.

Params: (NONE)"

%feature("docstring") gr_constellation::decision_maker "Returns the constellation point that matches best. Also calculates the phase error.

Params: (sample)"

%feature("docstring") gr_constellation::bits_per_symbol "

Params: (NONE)"

%feature("docstring") gr_constellation::base "

Params: (NONE)"

%feature("docstring") gr_make_constellation "Creates a constellation block.



Params: (constellation)"

%feature("docstring") gr_constellation_decoder_cb "Constellation Decoder."

%feature("docstring") gr_constellation_decoder_cb::gr_constellation_decoder_cb "

Params: (sym_position, sym_value_out)"

%feature("docstring") gr_constellation_decoder_cb::set_constellation "

Params: (sym_position, sym_value_out)"

%feature("docstring") gr_constellation_decoder_cb::~gr_constellation_decoder_cb "

Params: (NONE)"

%feature("docstring") gr_constellation_decoder_cb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_constellation_decoder_cb "Creates a constellation_decoder_cb block.

Constellation Decoder.

Params: (sym_position, sym_value_out)"



%feature("docstring") gr_constellation_psk::gr_constellation_psk "

Params: (constellation, n_sectors)"

%feature("docstring") gr_constellation_psk::get_sector "

Params: (sample)"

%feature("docstring") gr_constellation_psk::calc_sector_value "

Params: (sector)"

%feature("docstring") gr_make_constellation_psk "Creates a constellation_psk block.



Params: (constellation, n_sectors)"

%feature("docstring") gr_constellation_receiver_cb "This block takes care of receiving generic modulated signals through phase, frequency, and symbol synchronization.

This block takes care of receiving generic modulated signals through phase, frequency, and symbol synchronization. It performs carrier frequency and phase locking as well as symbol timing recovery.

The phase and frequency synchronization are based on a Costas loop that finds the error of the incoming signal point compared to its nearest constellation point. The frequency and phase of the NCO are updated according to this error.

The symbol synchronization is done using a modified Mueller and Muller circuit from the paper:

G. R. Danesfahani, T.G. Jeans, \"Optimisation of modified Mueller and Muller 
    algorithm,\" Electronics Letters, Vol. 31, no. 13, 22 June 1995, pp. 1032 - 1033.

This circuit interpolates the downconverted sample (using the NCO developed by the Costas loop) every mu samples, then it finds the sampling error based on this and the past symbols and the decision made on the samples. Like the phase error detector, there are optimized decision algorithms for BPSK and QPKS, but 8PSK uses another brute force computation against all possible symbols. The modifications to the M&M used here reduce self-noise."

%feature("docstring") gr_constellation_receiver_cb::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_constellation_receiver_cb::alpha "(CL) Returns the value for alpha (the phase gain term)

Params: (NONE)"

%feature("docstring") gr_constellation_receiver_cb::beta "(CL) Returns the value of beta (the frequency gain term)

Params: (NONE)"

%feature("docstring") gr_constellation_receiver_cb::freq "(CL) Returns the current value of the frequency of the NCO in the Costas loop

Params: (NONE)"

%feature("docstring") gr_constellation_receiver_cb::phase "(CL) Returns the current value of the phase of the NCO in the Costal loop

Params: (NONE)"

%feature("docstring") gr_constellation_receiver_cb::set_alpha "(CL) Sets the value for alpha (the phase gain term)

Params: (alpha)"

%feature("docstring") gr_constellation_receiver_cb::set_beta "(CL) Setss the value of beta (the frequency gain term)

Params: (beta)"

%feature("docstring") gr_constellation_receiver_cb::set_freq "(CL) Sets the current value of the frequency of the NCO in the Costas loop

Params: (freq)"

%feature("docstring") gr_constellation_receiver_cb::set_phase "(CL) Setss the current value of the phase of the NCO in the Costal loop

Params: (phase)"

%feature("docstring") gr_constellation_receiver_cb::gr_constellation_receiver_cb "Constructor to synchronize incoming M-PSK symbols.

The constructor also chooses which phase detector and decision maker to use in the work loop based on the value of M.

Params: (constellation, alpha, beta, fmin, fmax)"

%feature("docstring") gr_constellation_receiver_cb::phase_error_tracking "

Params: (phase_error)"

%feature("docstring") gr_constellation_receiver_cb::__attribute__ "delay line plus some length for overflow protection

Params: ()"

%feature("docstring") gr_make_constellation_receiver_cb "Creates a constellation_receiver_cb block.

This block takes care of receiving generic modulated signals through phase, frequency, and symbol synchronization.

This block takes care of receiving generic modulated signals through phase, frequency, and symbol synchronization. It performs carrier frequency and phase locking as well as symbol timing recovery.

The phase and frequency synchronization are based on a Costas loop that finds the error of the incoming signal point compared to its nearest constellation point. The frequency and phase of the NCO are updated according to this error.

The symbol synchronization is done using a modified Mueller and Muller circuit from the paper:

G. R. Danesfahani, T.G. Jeans, \"Optimisation of modified Mueller and Muller 
    algorithm,\" Electronics Letters, Vol. 31, no. 13, 22 June 1995, pp. 1032 - 1033.

This circuit interpolates the downconverted sample (using the NCO developed by the Costas loop) every mu samples, then it finds the sampling error based on this and the past symbols and the decision made on the samples. Like the phase error detector, there are optimized decision algorithms for BPSK and QPKS, but 8PSK uses another brute force computation against all possible symbols. The modifications to the M&M used here reduce self-noise.

Params: (constellation, alpha, beta, fmin, fmax)"



%feature("docstring") gr_constellation_rect::gr_constellation_rect "

Params: (constellation, real_sectors, imag_sectors, width_real_sectors, width_imag_sectors)"

%feature("docstring") gr_constellation_rect::get_sector "

Params: (sample)"

%feature("docstring") gr_constellation_rect::calc_sector_value "

Params: (sector)"

%feature("docstring") gr_make_constellation_rect "Creates a constellation_rect block.



Params: (constellation, real_sectors, imag_sectors, width_real_sectors, width_imag_sectors)"

%feature("docstring") gr_copy "output[i] = input[i]

When enabled (default), this block copies its input to its output. When disabled, this block drops its input on the floor."

%feature("docstring") gr_copy::gr_copy "

Params: (itemsize)"

%feature("docstring") gr_copy::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_copy::set_enabled "

Params: (enable)"

%feature("docstring") gr_copy::enabled "

Params: (NONE)"

%feature("docstring") gr_copy::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_copy "Creates a copy block.

output[i] = input[i]

When enabled (default), this block copies its input to its output. When disabled, this block drops its input on the floor.

Params: (itemsize)"

%feature("docstring") gr_correlate_access_code_bb "Examine input for specified access code, one bit at a time.

input: stream of bits, 1 bit per input byte (data in LSB) output: stream of bits, 2 bits per output byte (data in LSB, flag in next higher bit).

Each output byte contains two valid bits, the data bit, and the flag bit. The LSB (bit 0) is the data bit, and is the original input data, delayed 64 bits. Bit 1 is the flag bit and is 1 if the corresponding data bit is the first data bit following the access code. Otherwise the flag bit is 0."

%feature("docstring") gr_correlate_access_code_bb::gr_correlate_access_code_bb "

Params: (access_code, threshold)"

%feature("docstring") gr_correlate_access_code_bb::~gr_correlate_access_code_bb "

Params: (NONE)"

%feature("docstring") gr_correlate_access_code_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_correlate_access_code_bb::set_access_code "

Params: (access_code)"

%feature("docstring") gr_make_correlate_access_code_bb "Creates a correlate_access_code_bb block.

Examine input for specified access code, one bit at a time.

input: stream of bits, 1 bit per input byte (data in LSB) output: stream of bits, 2 bits per output byte (data in LSB, flag in next higher bit).

Each output byte contains two valid bits, the data bit, and the flag bit. The LSB (bit 0) is the data bit, and is the original input data, delayed 64 bits. Bit 1 is the flag bit and is 1 if the corresponding data bit is the first data bit following the access code. Otherwise the flag bit is 0.

Params: (access_code, threshold)"

%feature("docstring") gr_costas_loop_cc "Carrier tracking PLL for QPSK

input: complex; output: complex 
The Costas loop can have two output streams: stream 1 is the baseband I and Q; stream 2 is the normalized frequency of the loop.

must be 2 or 4."

%feature("docstring") gr_costas_loop_cc::gr_costas_loop_cc "

Params: (alpha, beta, max_freq, min_freq, order)"

%feature("docstring") gr_costas_loop_cc::phase_detector_4 "the phase detector circuit for fourth-order loops

Params: (sample)"

%feature("docstring") gr_costas_loop_cc::phase_detector_2 "the phase detector circuit for second-order loops

Params: (sample)"

%feature("docstring") gr_costas_loop_cc::set_alpha "set the first order gain

Params: (alpha)"

%feature("docstring") gr_costas_loop_cc::alpha "get the first order gain

Params: (NONE)"

%feature("docstring") gr_costas_loop_cc::set_beta "set the second order gain

Params: (beta)"

%feature("docstring") gr_costas_loop_cc::beta "get the second order gain

Params: (NONE)"

%feature("docstring") gr_costas_loop_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_costas_loop_cc::freq "returns the current NCO frequency in radians/sample

Params: (NONE)"

%feature("docstring") gr_make_costas_loop_cc "Creates a costas_loop_cc block.

Carrier tracking PLL for QPSK

input: complex; output: complex 
The Costas loop can have two output streams: stream 1 is the baseband I and Q; stream 2 is the normalized frequency of the loop.

must be 2 or 4.

Params: (alpha, beta, max_freq, min_freq, order)"

%feature("docstring") gr_cpfsk_bc "Perform continuous phase 2-level frequency shift keying modulation on an input stream of unpacked bits."

%feature("docstring") gr_cpfsk_bc::gr_cpfsk_bc "

Params: (k, ampl, samples_per_sym)"

%feature("docstring") gr_cpfsk_bc::~gr_cpfsk_bc "

Params: (NONE)"

%feature("docstring") gr_cpfsk_bc::set_amplitude "

Params: (amplitude)"

%feature("docstring") gr_cpfsk_bc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_cpfsk_bc "Creates a cpfsk_bc block.

Perform continuous phase 2-level frequency shift keying modulation on an input stream of unpacked bits.

Params: (k, ampl, samples_per_sym)"

%feature("docstring") gr_ctcss_squelch_ff "gate or zero output if ctcss tone not present"

%feature("docstring") gr_ctcss_squelch_ff::gr_ctcss_squelch_ff "

Params: (rate, freq, level, len, ramp, gate)"

%feature("docstring") gr_ctcss_squelch_ff::find_tone "

Params: (freq)"

%feature("docstring") gr_ctcss_squelch_ff::update_state "

Params: (in)"

%feature("docstring") gr_ctcss_squelch_ff::mute "

Params: (NONE)"

%feature("docstring") gr_ctcss_squelch_ff::squelch_range "

Params: (NONE)"

%feature("docstring") gr_ctcss_squelch_ff::level "

Params: (NONE)"

%feature("docstring") gr_ctcss_squelch_ff::set_level "

Params: (level)"

%feature("docstring") gr_ctcss_squelch_ff::len "

Params: (NONE)"

%feature("docstring") gr_make_ctcss_squelch_ff "Creates a ctcss_squelch_ff block.

gate or zero output if ctcss tone not present

Params: (rate, freq, level, len, ramp, gate)"

%feature("docstring") gr_decode_ccsds_27_fb "A rate 1/2, k=7 convolutional decoder for the CCSDS standard

This block performs soft-decision convolutional decoding using the Viterbi algorithm.

The input is a stream of (possibly noise corrupted) floating point values nominally spanning [-1.0, 1.0], representing the encoded channel symbols 0 (-1.0) and 1 (1.0), with erased symbols at 0.0.

The output is MSB first packed bytes of decoded values.

As a rate 1/2 code, there will be one output byte for every 16 input symbols.

This block is designed for continuous data streaming, not packetized data. The first 32 bits out will be zeroes, with the output delayed four bytes from the corresponding inputs."

%feature("docstring") gr_decode_ccsds_27_fb::gr_decode_ccsds_27_fb "

Params: (NONE)"

%feature("docstring") gr_decode_ccsds_27_fb::~gr_decode_ccsds_27_fb "

Params: (NONE)"

%feature("docstring") gr_decode_ccsds_27_fb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_decode_ccsds_27_fb "Creates a decode_ccsds_27_fb block.

A rate 1/2, k=7 convolutional decoder for the CCSDS standard

This block performs soft-decision convolutional decoding using the Viterbi algorithm.

The input is a stream of (possibly noise corrupted) floating point values nominally spanning [-1.0, 1.0], representing the encoded channel symbols 0 (-1.0) and 1 (1.0), with erased symbols at 0.0.

The output is MSB first packed bytes of decoded values.

As a rate 1/2 code, there will be one output byte for every 16 input symbols.

This block is designed for continuous data streaming, not packetized data. The first 32 bits out will be zeroes, with the output delayed four bytes from the corresponding inputs.

Params: (NONE)"

%feature("docstring") gr_deinterleave "deinterleave a single input into N outputs"

%feature("docstring") gr_deinterleave::gr_deinterleave "

Params: (itemsize)"

%feature("docstring") gr_deinterleave::~gr_deinterleave "

Params: (NONE)"

%feature("docstring") gr_deinterleave::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_deinterleave::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_deinterleave "Creates a deinterleave block.

deinterleave a single input into N outputs

Params: (itemsize)"

%feature("docstring") gr_delay "delay the input by a certain number of samples"

%feature("docstring") gr_delay::gr_delay "

Params: (itemsize, delay)"

%feature("docstring") gr_delay::delay "

Params: (NONE)"

%feature("docstring") gr_delay::set_delay "

Params: (delay)"

%feature("docstring") gr_delay::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_delay "Creates a delay block.

delay the input by a certain number of samples

Params: (itemsize, delay)"

%feature("docstring") gr_descrambler_bb "Descramble an input stream using an LFSR. This block works on the LSB only of the input data stream, i.e., on an \"unpacked binary\" stream, and produces the same format on its output."

%feature("docstring") gr_descrambler_bb::gr_descrambler_bb "

Params: (mask, seed, len)"

%feature("docstring") gr_descrambler_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_descrambler_bb "Creates a descrambler_bb block.

Descramble an input stream using an LFSR. This block works on the LSB only of the input data stream, i.e., on an \"unpacked binary\" stream, and produces the same format on its output.

Params: (mask, seed, len)"

%feature("docstring") gr_diff_decoder_bb "y[0] = (x[0] - x[-1]) % M

Differential decoder"

%feature("docstring") gr_diff_decoder_bb::gr_diff_decoder_bb "

Params: (modulus)"

%feature("docstring") gr_diff_decoder_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_diff_decoder_bb "Creates a diff_decoder_bb block.

y[0] = (x[0] - x[-1]) % M

Differential decoder

Params: (modulus)"

%feature("docstring") gr_diff_encoder_bb "y[0] = (x[0] + y[-1]) % M

Differential encoder"

%feature("docstring") gr_diff_encoder_bb::gr_diff_encoder_bb "

Params: (modulus)"

%feature("docstring") gr_diff_encoder_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_diff_encoder_bb "Creates a diff_encoder_bb block.

y[0] = (x[0] + y[-1]) % M

Differential encoder

Params: (modulus)"



%feature("docstring") gr_diff_phasor_cc::gr_diff_phasor_cc "

Params: (NONE)"

%feature("docstring") gr_diff_phasor_cc::~gr_diff_phasor_cc "

Params: (NONE)"

%feature("docstring") gr_diff_phasor_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_diff_phasor_cc "Creates a diff_phasor_cc block.



Params: (NONE)"

%feature("docstring") gr_dispatcher "invoke callbacks based on select."

%feature("docstring") gr_dispatcher::gr_dispatcher "

Params: (NONE)"

%feature("docstring") gr_dispatcher::~gr_dispatcher "

Params: (NONE)"

%feature("docstring") gr_dispatcher::add_handler "

Params: (handler)"

%feature("docstring") gr_dispatcher::del_handler "

Params: (handler)"

%feature("docstring") gr_dispatcher::loop "Event dispatching loop.

Enter a polling loop that only terminates after all gr_select_handlers have been removed.  sets the timeout parameter to the select() call, measured in seconds.

Params: (timeout)"

%feature("docstring") gr_make_dispatcher "Creates a dispatcher block.

invoke callbacks based on select.

Params: (NONE)"

%feature("docstring") gr_divide_cc "output = input_0 / input_1 / input_x ...)

Divide across all input streams."

%feature("docstring") gr_divide_cc::gr_divide_cc "

Params: (vlen)"

%feature("docstring") gr_divide_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_divide_cc "Creates a divide_cc block.

output = input_0 / input_1 / input_x ...)

Divide across all input streams.

Params: (vlen)"

%feature("docstring") gr_divide_ff "output = input_0 / input_1 / input_x ...)

Divide across all input streams."

%feature("docstring") gr_divide_ff::gr_divide_ff "

Params: (vlen)"

%feature("docstring") gr_divide_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_divide_ff "Creates a divide_ff block.

output = input_0 / input_1 / input_x ...)

Divide across all input streams.

Params: (vlen)"

%feature("docstring") gr_divide_ii "output = input_0 / input_1 / input_x ...)

Divide across all input streams."

%feature("docstring") gr_divide_ii::gr_divide_ii "

Params: (vlen)"

%feature("docstring") gr_divide_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_divide_ii "Creates a divide_ii block.

output = input_0 / input_1 / input_x ...)

Divide across all input streams.

Params: (vlen)"

%feature("docstring") gr_divide_ss "output = input_0 / input_1 / input_x ...)

Divide across all input streams."

%feature("docstring") gr_divide_ss::gr_divide_ss "

Params: (vlen)"

%feature("docstring") gr_divide_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_divide_ss "Creates a divide_ss block.

output = input_0 / input_1 / input_x ...)

Divide across all input streams.

Params: (vlen)"

%feature("docstring") gr_dpll_bb "Detect the peak of a signal

If a peak is detected, this block outputs a 1, or it outputs 0's."

%feature("docstring") gr_dpll_bb::gr_dpll_bb "

Params: (period, gain)"

%feature("docstring") gr_dpll_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_dpll_bb "Creates a dpll_bb block.

Detect the peak of a signal

If a peak is detected, this block outputs a 1, or it outputs 0's.

Params: (period, gain)"

%feature("docstring") gr_encode_ccsds_27_bb "A rate 1/2, k=7 convolutional encoder for the CCSDS standard

This block performs convolutional encoding using the CCSDS standard polynomial (\"Voyager\").

The input is an MSB first packed stream of bits.

The output is a stream of symbols 0 or 1 representing the encoded data.

As a rate 1/2 code, there will be 16 output symbols for every input byte.

This block is designed for continuous data streaming, not packetized data. There is no provision to \"flush\" the encoder."

%feature("docstring") gr_encode_ccsds_27_bb::gr_encode_ccsds_27_bb "

Params: (NONE)"

%feature("docstring") gr_encode_ccsds_27_bb::~gr_encode_ccsds_27_bb "

Params: (NONE)"

%feature("docstring") gr_encode_ccsds_27_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_encode_ccsds_27_bb "Creates a encode_ccsds_27_bb block.

A rate 1/2, k=7 convolutional encoder for the CCSDS standard

This block performs convolutional encoding using the CCSDS standard polynomial (\"Voyager\").

The input is an MSB first packed stream of bits.

The output is a stream of symbols 0 or 1 representing the encoded data.

As a rate 1/2 code, there will be 16 output symbols for every input byte.

This block is designed for continuous data streaming, not packetized data. There is no provision to \"flush\" the encoder.

Params: (NONE)"

%feature("docstring") gr_fake_channel_decoder_pp "remove fake padding from packet

input: stream of byte vectors; output: stream of byte vectors"

%feature("docstring") gr_fake_channel_decoder_pp::gr_fake_channel_decoder_pp "

Params: (input_vlen, output_vlen)"

%feature("docstring") gr_fake_channel_decoder_pp::~gr_fake_channel_decoder_pp "

Params: (NONE)"

%feature("docstring") gr_fake_channel_decoder_pp::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fake_channel_decoder_pp "Creates a fake_channel_decoder_pp block.

remove fake padding from packet

input: stream of byte vectors; output: stream of byte vectors

Params: (input_vlen, output_vlen)"

%feature("docstring") gr_fake_channel_encoder_pp "pad packet with alternating 1,0 pattern.

input: stream of byte vectors; output: stream of byte vectors"

%feature("docstring") gr_fake_channel_encoder_pp::gr_fake_channel_encoder_pp "

Params: (input_vlen, output_vlen)"

%feature("docstring") gr_fake_channel_encoder_pp::~gr_fake_channel_encoder_pp "

Params: (NONE)"

%feature("docstring") gr_fake_channel_encoder_pp::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fake_channel_encoder_pp "Creates a fake_channel_encoder_pp block.

pad packet with alternating 1,0 pattern.

input: stream of byte vectors; output: stream of byte vectors

Params: (input_vlen, output_vlen)"

%feature("docstring") gr_feedforward_agc_cc "Non-causal AGC which computes required gain based on max absolute value over nsamples."

%feature("docstring") gr_feedforward_agc_cc::gr_feedforward_agc_cc "

Params: (nsamples, reference)"

%feature("docstring") gr_feedforward_agc_cc::~gr_feedforward_agc_cc "

Params: (NONE)"

%feature("docstring") gr_feedforward_agc_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_feedforward_agc_cc "Creates a feedforward_agc_cc block.

Non-causal AGC which computes required gain based on max absolute value over nsamples.

Params: (nsamples, reference)"

%feature("docstring") gr_fft_filter_ccc "Fast FFT filter with gr_complex input, gr_complex output and gr_complex taps."

%feature("docstring") gr_fft_filter_ccc::gr_fft_filter_ccc "Construct a FFT filter with the given taps

Params: (decimation, taps)"

%feature("docstring") gr_fft_filter_ccc::~gr_fft_filter_ccc "

Params: (NONE)"

%feature("docstring") gr_fft_filter_ccc::set_taps "

Params: (taps)"

%feature("docstring") gr_fft_filter_ccc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fft_filter_ccc "Creates a fft_filter_ccc block.

Fast FFT filter with gr_complex input, gr_complex output and gr_complex taps.

Params: (decimation, taps)"

%feature("docstring") gr_fft_filter_fff "Fast FFT filter with float input, float output and float taps."

%feature("docstring") gr_fft_filter_fff::gr_fft_filter_fff "Construct a FFT filter with the given taps

Params: (decimation, taps)"

%feature("docstring") gr_fft_filter_fff::~gr_fft_filter_fff "

Params: (NONE)"

%feature("docstring") gr_fft_filter_fff::set_taps "

Params: (taps)"

%feature("docstring") gr_fft_filter_fff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fft_filter_fff "Creates a fft_filter_fff block.

Fast FFT filter with float input, float output and float taps.

Params: (decimation, taps)"

%feature("docstring") gr_fft_vcc "Compute forward or reverse FFT. complex vector in / complex vector out.

Abstract base class."

%feature("docstring") gr_fft_vcc::gr_fft_vcc "

Params: (name, fft_size, forward, window, shift)"

%feature("docstring") gr_fft_vcc::~gr_fft_vcc "

Params: (NONE)"

%feature("docstring") gr_fft_vcc::set_window "

Params: (window)"

%feature("docstring") gr_make_fft_vcc "Creates a fft_vcc block.

Compute forward or reverse FFT. complex vector in / complex vector out.

Abstract base class.

Params: (name, fft_size, forward, window, shift)"

%feature("docstring") gr_fft_vcc_fftw "Compute forward or reverse FFT. complex vector in / complex vector out.

Concrete class that uses FFTW."

%feature("docstring") gr_fft_vcc_fftw::gr_fft_vcc_fftw "

Params: (fft_size, forward, window, shift)"

%feature("docstring") gr_fft_vcc_fftw::~gr_fft_vcc_fftw "

Params: (NONE)"

%feature("docstring") gr_fft_vcc_fftw::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fft_vcc_fftw "Creates a fft_vcc_fftw block.

Compute forward or reverse FFT. complex vector in / complex vector out.

Concrete class that uses FFTW.

Params: (fft_size, forward, window, shift)"

%feature("docstring") gr_fft_vfc "Compute forward FFT. float vector in / complex vector out."

%feature("docstring") gr_fft_vfc::gr_fft_vfc "

Params: (fft_size, forward, window)"

%feature("docstring") gr_fft_vfc::~gr_fft_vfc "

Params: (NONE)"

%feature("docstring") gr_fft_vfc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_fft_vfc::set_window "

Params: (window)"

%feature("docstring") gr_make_fft_vfc "Creates a fft_vfc block.

Compute forward FFT. float vector in / complex vector out.

Params: (fft_size, forward, window)"

%feature("docstring") gr_file_descriptor_sink "Write stream to file descriptor."

%feature("docstring") gr_file_descriptor_sink::gr_file_descriptor_sink "

Params: (itemsize, fd)"

%feature("docstring") gr_file_descriptor_sink::~gr_file_descriptor_sink "

Params: (NONE)"

%feature("docstring") gr_file_descriptor_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_file_descriptor_sink "Creates a file_descriptor_sink block.

Write stream to file descriptor.

Params: (itemsize, fd)"

%feature("docstring") gr_file_descriptor_source "Read stream from file descriptor."

%feature("docstring") gr_file_descriptor_source::gr_file_descriptor_source "

Params: (itemsize, fd, repeat)"

%feature("docstring") gr_file_descriptor_source::read_items "

Params: (buf, nitems)"

%feature("docstring") gr_file_descriptor_source::handle_residue "

Params: (buf, nbytes_read)"

%feature("docstring") gr_file_descriptor_source::flush_residue "

Params: (NONE)"

%feature("docstring") gr_file_descriptor_source::~gr_file_descriptor_source "

Params: (NONE)"

%feature("docstring") gr_file_descriptor_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_file_descriptor_source "Creates a file_descriptor_source block.

Read stream from file descriptor.

Params: (itemsize, fd, repeat)"

%feature("docstring") gr_file_sink "Write stream to file."

%feature("docstring") gr_file_sink::gr_file_sink "

Params: (itemsize, filename)"

%feature("docstring") gr_file_sink::~gr_file_sink "

Params: (NONE)"

%feature("docstring") gr_file_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_file_sink "Creates a file_sink block.

Write stream to file.

Params: (itemsize, filename)"

%feature("docstring") gr_file_source "Read stream from file."

%feature("docstring") gr_file_source::gr_file_source "

Params: (itemsize, filename, repeat)"

%feature("docstring") gr_file_source::~gr_file_source "

Params: (NONE)"

%feature("docstring") gr_file_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_file_source::seek "seek file to  relative to

Params: (seek_point, whence)"

%feature("docstring") gr_make_file_source "Creates a file_source block.

Read stream from file.

Params: (itemsize, filename, repeat)"

%feature("docstring") gr_filter_delay_fc "Filter-Delay Combination Block.

The block takes one or two float stream and outputs a complex stream. If only one float stream is input, the real output is a delayed version of this input and the imaginary output is the filtered output. If two floats are connected to the input, then the real output is the delayed version of the first input, and the imaginary output is the filtered output. The delay in the real path accounts for the group delay introduced by the filter in the imaginary path. The filter taps needs to be calculated before initializing this block."

%feature("docstring") gr_filter_delay_fc::~gr_filter_delay_fc "

Params: (NONE)"

%feature("docstring") gr_filter_delay_fc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_filter_delay_fc::gr_filter_delay_fc "

Params: (taps)"

%feature("docstring") gr_make_filter_delay_fc "Creates a filter_delay_fc block.

Filter-Delay Combination Block.

The block takes one or two float stream and outputs a complex stream. If only one float stream is input, the real output is a delayed version of this input and the imaginary output is the filtered output. If two floats are connected to the input, then the real output is the delayed version of the first input, and the imaginary output is the filtered output. The delay in the real path accounts for the group delay introduced by the filter in the imaginary path. The filter taps needs to be calculated before initializing this block.

Params: (taps)"

%feature("docstring") gr_fir_filter_ccc "FIR filter with gr_complex input, gr_complex output and gr_complex taps."

%feature("docstring") gr_fir_filter_ccc::gr_fir_filter_ccc "Construct a FIR filter with the given taps

Params: (decimation, taps)"

%feature("docstring") gr_fir_filter_ccc::~gr_fir_filter_ccc "

Params: (NONE)"

%feature("docstring") gr_fir_filter_ccc::set_taps "

Params: (taps)"

%feature("docstring") gr_fir_filter_ccc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fir_filter_ccc "Creates a fir_filter_ccc block.

FIR filter with gr_complex input, gr_complex output and gr_complex taps.

Params: (decimation, taps)"

%feature("docstring") gr_fir_filter_ccf "FIR filter with gr_complex input, gr_complex output and float taps."

%feature("docstring") gr_fir_filter_ccf::gr_fir_filter_ccf "Construct a FIR filter with the given taps

Params: (decimation, taps)"

%feature("docstring") gr_fir_filter_ccf::~gr_fir_filter_ccf "

Params: (NONE)"

%feature("docstring") gr_fir_filter_ccf::set_taps "

Params: (taps)"

%feature("docstring") gr_fir_filter_ccf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fir_filter_ccf "Creates a fir_filter_ccf block.

FIR filter with gr_complex input, gr_complex output and float taps.

Params: (decimation, taps)"

%feature("docstring") gr_fir_filter_fcc "FIR filter with float input, gr_complex output and gr_complex taps."

%feature("docstring") gr_fir_filter_fcc::gr_fir_filter_fcc "Construct a FIR filter with the given taps

Params: (decimation, taps)"

%feature("docstring") gr_fir_filter_fcc::~gr_fir_filter_fcc "

Params: (NONE)"

%feature("docstring") gr_fir_filter_fcc::set_taps "

Params: (taps)"

%feature("docstring") gr_fir_filter_fcc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fir_filter_fcc "Creates a fir_filter_fcc block.

FIR filter with float input, gr_complex output and gr_complex taps.

Params: (decimation, taps)"

%feature("docstring") gr_fir_filter_fff "FIR filter with float input, float output and float taps."

%feature("docstring") gr_fir_filter_fff::gr_fir_filter_fff "Construct a FIR filter with the given taps

Params: (decimation, taps)"

%feature("docstring") gr_fir_filter_fff::~gr_fir_filter_fff "

Params: (NONE)"

%feature("docstring") gr_fir_filter_fff::set_taps "

Params: (taps)"

%feature("docstring") gr_fir_filter_fff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fir_filter_fff "Creates a fir_filter_fff block.

FIR filter with float input, float output and float taps.

Params: (decimation, taps)"

%feature("docstring") gr_fir_filter_fsf "FIR filter with float input, short output and float taps."

%feature("docstring") gr_fir_filter_fsf::gr_fir_filter_fsf "Construct a FIR filter with the given taps

Params: (decimation, taps)"

%feature("docstring") gr_fir_filter_fsf::~gr_fir_filter_fsf "

Params: (NONE)"

%feature("docstring") gr_fir_filter_fsf::set_taps "

Params: (taps)"

%feature("docstring") gr_fir_filter_fsf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fir_filter_fsf "Creates a fir_filter_fsf block.

FIR filter with float input, short output and float taps.

Params: (decimation, taps)"

%feature("docstring") gr_fir_filter_scc "FIR filter with short input, gr_complex output and gr_complex taps."

%feature("docstring") gr_fir_filter_scc::gr_fir_filter_scc "Construct a FIR filter with the given taps

Params: (decimation, taps)"

%feature("docstring") gr_fir_filter_scc::~gr_fir_filter_scc "

Params: (NONE)"

%feature("docstring") gr_fir_filter_scc::set_taps "

Params: (taps)"

%feature("docstring") gr_fir_filter_scc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fir_filter_scc "Creates a fir_filter_scc block.

FIR filter with short input, gr_complex output and gr_complex taps.

Params: (decimation, taps)"

%feature("docstring") gr_flat_flowgraph "Class specializing gr_flat_flowgraph that has all nodes as gr_blocks, with no hierarchy."

%feature("docstring") gr_flat_flowgraph::~gr_flat_flowgraph "

Params: (NONE)"

%feature("docstring") gr_flat_flowgraph::setup_connections "

Params: (NONE)"

%feature("docstring") gr_flat_flowgraph::merge_connections "

Params: (sfg)"

%feature("docstring") gr_flat_flowgraph::dump "

Params: (NONE)"

%feature("docstring") gr_flat_flowgraph::make_block_vector "Make a vector of gr_block from a vector of gr_basic_block

Params: (blocks)"

%feature("docstring") gr_flat_flowgraph::gr_flat_flowgraph "

Params: (NONE)"

%feature("docstring") gr_flat_flowgraph::allocate_block_detail "

Params: (block)"

%feature("docstring") gr_flat_flowgraph::allocate_buffer "

Params: (block, port)"

%feature("docstring") gr_flat_flowgraph::connect_block_inputs "

Params: (block)"

%feature("docstring") gr_make_flat_flowgraph "Creates a flat_flowgraph block.

Class specializing gr_flat_flowgraph that has all nodes as gr_blocks, with no hierarchy.

Params: (NONE)"

%feature("docstring") gr_fll_band_edge_cc "Frequency Lock Loop using band-edge filters.

The frequency lock loop derives a band-edge filter that covers the upper and lower bandwidths of a digitally-modulated signal. The bandwidth range is determined by the excess bandwidth (e.g., rolloff factor) of the modulated signal. The placement in frequency of the band-edges is determined by the oversampling ratio (number of samples per symbol) and the excess bandwidth. The size of the filters should be fairly large so as to average over a number of symbols.

The FLL works by filtering the upper and lower band edges into x_u(t) and x_l(t), respectively. These are combined to form cc(t) = x_u(t) + x_l(t) and ss(t) = x_u(t) - x_l(t). Combining these to form the signal e(t) = Re{cc(t) \times ss(t)^*} (where ^* is the complex conjugate) provides an error signal at the DC term that is directly proportional to the carrier frequency. We then make a second-order loop using the error signal that is the running average of e(t).

In theory, the band-edge filter is the derivative of the matched filter in frequency, (H_be(f) = \frac{H(f)}{df}. In practice, this comes down to a quarter sine wave at the point of the matched filter's rolloff (if it's a raised-cosine, the derivative of a cosine is a sine). Extend this sine by another quarter wave to make a half wave around the band-edges is equivalent in time to the sum of two sinc functions. The baseband filter fot the band edges is therefore derived from this sum of sincs. The band edge filters are then just the baseband signal modulated to the correct place in frequency. All of these calculations are done in the 'design_filter' function.

Note: We use FIR filters here because the filters have to have a flat phase response over the entire frequency range to allow their comparisons to be valid."

%feature("docstring") gr_fll_band_edge_cc::gr_fll_band_edge_cc "Build the FLL

Params: (samps_per_sym, rolloff, filter_size, alpha, beta)"

%feature("docstring") gr_fll_band_edge_cc::~gr_fll_band_edge_cc "

Params: (NONE)"

%feature("docstring") gr_fll_band_edge_cc::design_filter "Design the band-edge filter based on the number of samples per symbol, filter rolloff factor, and the filter size

Params: (samps_per_sym, rolloff, filter_size)"

%feature("docstring") gr_fll_band_edge_cc::set_alpha "Set the alpha gainvalue

Params: (alpha)"

%feature("docstring") gr_fll_band_edge_cc::set_beta "Set the beta gain value

Params: (beta)"

%feature("docstring") gr_fll_band_edge_cc::print_taps "Print the taps to screen.

Params: (NONE)"

%feature("docstring") gr_fll_band_edge_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fll_band_edge_cc "Creates a fll_band_edge_cc block.

Frequency Lock Loop using band-edge filters.

The frequency lock loop derives a band-edge filter that covers the upper and lower bandwidths of a digitally-modulated signal. The bandwidth range is determined by the excess bandwidth (e.g., rolloff factor) of the modulated signal. The placement in frequency of the band-edges is determined by the oversampling ratio (number of samples per symbol) and the excess bandwidth. The size of the filters should be fairly large so as to average over a number of symbols.

The FLL works by filtering the upper and lower band edges into x_u(t) and x_l(t), respectively. These are combined to form cc(t) = x_u(t) + x_l(t) and ss(t) = x_u(t) - x_l(t). Combining these to form the signal e(t) = Re{cc(t) \times ss(t)^*} (where ^* is the complex conjugate) provides an error signal at the DC term that is directly proportional to the carrier frequency. We then make a second-order loop using the error signal that is the running average of e(t).

In theory, the band-edge filter is the derivative of the matched filter in frequency, (H_be(f) = \frac{H(f)}{df}. In practice, this comes down to a quarter sine wave at the point of the matched filter's rolloff (if it's a raised-cosine, the derivative of a cosine is a sine). Extend this sine by another quarter wave to make a half wave around the band-edges is equivalent in time to the sum of two sinc functions. The baseband filter fot the band edges is therefore derived from this sum of sincs. The band edge filters are then just the baseband signal modulated to the correct place in frequency. All of these calculations are done in the 'design_filter' function.

Note: We use FIR filters here because the filters have to have a flat phase response over the entire frequency range to allow their comparisons to be valid.

Params: (samps_per_sym, rolloff, filter_size, alpha, beta)"

%feature("docstring") gr_float_to_char "Convert stream of float to a stream of char."

%feature("docstring") gr_float_to_char::gr_float_to_char "

Params: (NONE)"

%feature("docstring") gr_float_to_char::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_float_to_char "Creates a float_to_char block.

Convert stream of float to a stream of char.

Params: (NONE)"

%feature("docstring") gr_float_to_complex "Convert 1 or 2 streams of float to a stream of gr_complex."

%feature("docstring") gr_float_to_complex::gr_float_to_complex "

Params: (vlen)"

%feature("docstring") gr_float_to_complex::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_float_to_complex "Creates a float_to_complex block.

Convert 1 or 2 streams of float to a stream of gr_complex.

Params: (vlen)"

%feature("docstring") gr_float_to_short "Convert stream of float to a stream of short."

%feature("docstring") gr_float_to_short::gr_float_to_short "

Params: (NONE)"

%feature("docstring") gr_float_to_short::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_float_to_short "Creates a float_to_short block.

Convert stream of float to a stream of short.

Params: (NONE)"

%feature("docstring") gr_float_to_uchar "Convert stream of float to a stream of unsigned char."

%feature("docstring") gr_float_to_uchar::gr_float_to_uchar "

Params: (NONE)"

%feature("docstring") gr_float_to_uchar::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_float_to_uchar "Creates a float_to_uchar block.

Convert stream of float to a stream of unsigned char.

Params: (NONE)"

%feature("docstring") gr_flowgraph "Class representing a directed, acyclic graph of basic blocks."

%feature("docstring") gr_flowgraph::~gr_flowgraph "

Params: (NONE)"

%feature("docstring") gr_flowgraph::connect "

Params: (src, dst)"

%feature("docstring") gr_flowgraph::disconnect "

Params: (src, dst)"

%feature("docstring") gr_flowgraph::validate "

Params: (NONE)"

%feature("docstring") gr_flowgraph::clear "

Params: (NONE)"

%feature("docstring") gr_flowgraph::edges "

Params: (NONE)"

%feature("docstring") gr_flowgraph::calc_used_blocks "

Params: (NONE)"

%feature("docstring") gr_flowgraph::topological_sort "

Params: (blocks)"

%feature("docstring") gr_flowgraph::partition "

Params: (NONE)"

%feature("docstring") gr_flowgraph::gr_flowgraph "

Params: (NONE)"

%feature("docstring") gr_flowgraph::calc_used_ports "

Params: (block, check_inputs)"

%feature("docstring") gr_flowgraph::calc_downstream_blocks "

Params: (block, port)"

%feature("docstring") gr_flowgraph::calc_upstream_edges "

Params: (block)"

%feature("docstring") gr_flowgraph::has_block_p "

Params: (block)"

%feature("docstring") gr_flowgraph::calc_upstream_edge "

Params: (block, port)"

%feature("docstring") gr_flowgraph::check_valid_port "

Params: (sig, port)"

%feature("docstring") gr_flowgraph::check_dst_not_used "

Params: (dst)"

%feature("docstring") gr_flowgraph::check_type_match "

Params: (src, dst)"

%feature("docstring") gr_flowgraph::calc_connections "

Params: (block, check_inputs)"

%feature("docstring") gr_flowgraph::check_contiguity "

Params: (block, used_ports, check_inputs)"

%feature("docstring") gr_flowgraph::calc_reachable_blocks "

Params: (block, blocks)"

%feature("docstring") gr_flowgraph::reachable_dfs_visit "

Params: (block, blocks)"

%feature("docstring") gr_flowgraph::calc_adjacent_blocks "

Params: (block, blocks)"

%feature("docstring") gr_flowgraph::sort_sources_first "

Params: (blocks)"

%feature("docstring") gr_flowgraph::source_p "

Params: (block)"

%feature("docstring") gr_flowgraph::topological_dfs_visit "

Params: (block, output)"

%feature("docstring") gr_make_flowgraph "Creates a flowgraph block.

Class representing a directed, acyclic graph of basic blocks.

Params: (NONE)"

%feature("docstring") gr_fmdet_cf "Implements an IQ slope detector.

input: stream of complex; output: stream of floats

This implements a limiting slope detector. The limiter is in the normalization by the magnitude of the sample"

%feature("docstring") gr_fmdet_cf::gr_fmdet_cf "

Params: (samplerate, freq_low, freq_high, scl)"

%feature("docstring") gr_fmdet_cf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_fmdet_cf "Creates a fmdet_cf block.

Implements an IQ slope detector.

input: stream of complex; output: stream of floats

This implements a limiting slope detector. The limiter is in the normalization by the magnitude of the sample

Params: (samplerate, freq_low, freq_high, scl)"

%feature("docstring") gr_fractional_interpolator_cc "Interpolating mmse filter with gr_complex input, gr_complex output."

%feature("docstring") gr_fractional_interpolator_cc::~gr_fractional_interpolator_cc "

Params: (NONE)"

%feature("docstring") gr_fractional_interpolator_cc::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_fractional_interpolator_cc::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_fractional_interpolator_cc::mu "

Params: (NONE)"

%feature("docstring") gr_fractional_interpolator_cc::interp_ratio "

Params: (NONE)"

%feature("docstring") gr_fractional_interpolator_cc::set_mu "

Params: (mu)"

%feature("docstring") gr_fractional_interpolator_cc::set_interp_ratio "

Params: (interp_ratio)"

%feature("docstring") gr_fractional_interpolator_cc::gr_fractional_interpolator_cc "

Params: (phase_shift, interp_ratio)"

%feature("docstring") gr_make_fractional_interpolator_cc "Creates a fractional_interpolator_cc block.

Interpolating mmse filter with gr_complex input, gr_complex output.

Params: (phase_shift, interp_ratio)"

%feature("docstring") gr_fractional_interpolator_ff "Interpolating mmse filter with float input, float output."

%feature("docstring") gr_fractional_interpolator_ff::~gr_fractional_interpolator_ff "

Params: (NONE)"

%feature("docstring") gr_fractional_interpolator_ff::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_fractional_interpolator_ff::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_fractional_interpolator_ff::mu "

Params: (NONE)"

%feature("docstring") gr_fractional_interpolator_ff::interp_ratio "

Params: (NONE)"

%feature("docstring") gr_fractional_interpolator_ff::set_mu "

Params: (mu)"

%feature("docstring") gr_fractional_interpolator_ff::set_interp_ratio "

Params: (interp_ratio)"

%feature("docstring") gr_fractional_interpolator_ff::gr_fractional_interpolator_ff "

Params: (phase_shift, interp_ratio)"

%feature("docstring") gr_make_fractional_interpolator_ff "Creates a fractional_interpolator_ff block.

Interpolating mmse filter with float input, float output.

Params: (phase_shift, interp_ratio)"

%feature("docstring") gr_framer_sink_1 "Given a stream of bits and access_code flags, assemble packets.

input: stream of bytes from gr_correlate_access_code_bb output: none. Pushes assembled packet into target queue.

The framer expects a fixed length header of 2 16-bit shorts containing the payload length, followed by the payload. If the 2 16-bit shorts are not identical, this packet is ignored. Better algs are welcome.

The input data consists of bytes that have two bits used. Bit 0, the LSB, contains the data bit. Bit 1 if set, indicates that the corresponding bit is the the first bit of the packet. That is, this bit is the first one after the access code."

%feature("docstring") gr_framer_sink_1::gr_framer_sink_1 "

Params: (target_queue)"

%feature("docstring") gr_framer_sink_1::enter_search "

Params: (NONE)"

%feature("docstring") gr_framer_sink_1::enter_have_sync "

Params: (NONE)"

%feature("docstring") gr_framer_sink_1::enter_have_header "

Params: (payload_len, whitener_offset)"

%feature("docstring") gr_framer_sink_1::header_ok "

Params: (NONE)"

%feature("docstring") gr_framer_sink_1::header_payload "

Params: (len, offset)"

%feature("docstring") gr_framer_sink_1::~gr_framer_sink_1 "

Params: (NONE)"

%feature("docstring") gr_framer_sink_1::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_framer_sink_1 "Creates a framer_sink_1 block.

Given a stream of bits and access_code flags, assemble packets.

input: stream of bytes from gr_correlate_access_code_bb output: none. Pushes assembled packet into target queue.

The framer expects a fixed length header of 2 16-bit shorts containing the payload length, followed by the payload. If the 2 16-bit shorts are not identical, this packet is ignored. Better algs are welcome.

The input data consists of bytes that have two bits used. Bit 0, the LSB, contains the data bit. Bit 1 if set, indicates that the corresponding bit is the the first bit of the packet. That is, this bit is the first one after the access code.

Params: (target_queue)"

%feature("docstring") gr_freq_xlating_fir_filter_ccc "FIR filter combined with frequency translation with gr_complex input, gr_complex output and gr_complex taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored."

%feature("docstring") gr_freq_xlating_fir_filter_ccc::~gr_freq_xlating_fir_filter_ccc "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_ccc::set_center_freq "

Params: (center_freq)"

%feature("docstring") gr_freq_xlating_fir_filter_ccc::set_taps "

Params: (taps)"

%feature("docstring") gr_freq_xlating_fir_filter_ccc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_freq_xlating_fir_filter_ccc::build_composite_fir "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_ccc::gr_freq_xlating_fir_filter_ccc "Construct a FIR filter with the given taps and a composite frequency translation that shifts center_freq down to zero Hz. The frequency translation logically comes before the filtering operation.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_make_freq_xlating_fir_filter_ccc "Creates a freq_xlating_fir_filter_ccc block.

FIR filter combined with frequency translation with gr_complex input, gr_complex output and gr_complex taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_freq_xlating_fir_filter_ccf "FIR filter combined with frequency translation with gr_complex input, gr_complex output and float taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored."

%feature("docstring") gr_freq_xlating_fir_filter_ccf::~gr_freq_xlating_fir_filter_ccf "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_ccf::set_center_freq "

Params: (center_freq)"

%feature("docstring") gr_freq_xlating_fir_filter_ccf::set_taps "

Params: (taps)"

%feature("docstring") gr_freq_xlating_fir_filter_ccf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_freq_xlating_fir_filter_ccf::build_composite_fir "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_ccf::gr_freq_xlating_fir_filter_ccf "Construct a FIR filter with the given taps and a composite frequency translation that shifts center_freq down to zero Hz. The frequency translation logically comes before the filtering operation.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_make_freq_xlating_fir_filter_ccf "Creates a freq_xlating_fir_filter_ccf block.

FIR filter combined with frequency translation with gr_complex input, gr_complex output and float taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_freq_xlating_fir_filter_fcc "FIR filter combined with frequency translation with float input, gr_complex output and gr_complex taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored."

%feature("docstring") gr_freq_xlating_fir_filter_fcc::~gr_freq_xlating_fir_filter_fcc "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_fcc::set_center_freq "

Params: (center_freq)"

%feature("docstring") gr_freq_xlating_fir_filter_fcc::set_taps "

Params: (taps)"

%feature("docstring") gr_freq_xlating_fir_filter_fcc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_freq_xlating_fir_filter_fcc::build_composite_fir "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_fcc::gr_freq_xlating_fir_filter_fcc "Construct a FIR filter with the given taps and a composite frequency translation that shifts center_freq down to zero Hz. The frequency translation logically comes before the filtering operation.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_make_freq_xlating_fir_filter_fcc "Creates a freq_xlating_fir_filter_fcc block.

FIR filter combined with frequency translation with float input, gr_complex output and gr_complex taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_freq_xlating_fir_filter_fcf "FIR filter combined with frequency translation with float input, gr_complex output and float taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored."

%feature("docstring") gr_freq_xlating_fir_filter_fcf::~gr_freq_xlating_fir_filter_fcf "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_fcf::set_center_freq "

Params: (center_freq)"

%feature("docstring") gr_freq_xlating_fir_filter_fcf::set_taps "

Params: (taps)"

%feature("docstring") gr_freq_xlating_fir_filter_fcf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_freq_xlating_fir_filter_fcf::build_composite_fir "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_fcf::gr_freq_xlating_fir_filter_fcf "Construct a FIR filter with the given taps and a composite frequency translation that shifts center_freq down to zero Hz. The frequency translation logically comes before the filtering operation.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_make_freq_xlating_fir_filter_fcf "Creates a freq_xlating_fir_filter_fcf block.

FIR filter combined with frequency translation with float input, gr_complex output and float taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_freq_xlating_fir_filter_scc "FIR filter combined with frequency translation with short input, gr_complex output and gr_complex taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored."

%feature("docstring") gr_freq_xlating_fir_filter_scc::~gr_freq_xlating_fir_filter_scc "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_scc::set_center_freq "

Params: (center_freq)"

%feature("docstring") gr_freq_xlating_fir_filter_scc::set_taps "

Params: (taps)"

%feature("docstring") gr_freq_xlating_fir_filter_scc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_freq_xlating_fir_filter_scc::build_composite_fir "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_scc::gr_freq_xlating_fir_filter_scc "Construct a FIR filter with the given taps and a composite frequency translation that shifts center_freq down to zero Hz. The frequency translation logically comes before the filtering operation.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_make_freq_xlating_fir_filter_scc "Creates a freq_xlating_fir_filter_scc block.

FIR filter combined with frequency translation with short input, gr_complex output and gr_complex taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_freq_xlating_fir_filter_scf "FIR filter combined with frequency translation with short input, gr_complex output and float taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored."

%feature("docstring") gr_freq_xlating_fir_filter_scf::~gr_freq_xlating_fir_filter_scf "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_scf::set_center_freq "

Params: (center_freq)"

%feature("docstring") gr_freq_xlating_fir_filter_scf::set_taps "

Params: (taps)"

%feature("docstring") gr_freq_xlating_fir_filter_scf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_freq_xlating_fir_filter_scf::build_composite_fir "

Params: (NONE)"

%feature("docstring") gr_freq_xlating_fir_filter_scf::gr_freq_xlating_fir_filter_scf "Construct a FIR filter with the given taps and a composite frequency translation that shifts center_freq down to zero Hz. The frequency translation logically comes before the filtering operation.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_make_freq_xlating_fir_filter_scf "Creates a freq_xlating_fir_filter_scf block.

FIR filter combined with frequency translation with short input, gr_complex output and float taps

This class efficiently combines a frequency translation (typically \"down conversion\") with a FIR filter (typically low-pass) and decimation. It is ideally suited for a \"channel selection filter\" and can be efficiently used to select and decimate a narrow band signal out of wide bandwidth input.

Uses a single input array to produce a single output array. Additional inputs and/or outputs are ignored.

Params: (decimation, taps, center_freq, sampling_freq)"

%feature("docstring") gr_frequency_modulator_fc "Frequency modulator block

float input; complex baseband output."

%feature("docstring") gr_frequency_modulator_fc::gr_frequency_modulator_fc "

Params: (sensitivity)"

%feature("docstring") gr_frequency_modulator_fc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_frequency_modulator_fc "Creates a frequency_modulator_fc block.

Frequency modulator block

float input; complex baseband output.

Params: (sensitivity)"

%feature("docstring") gr_glfsr_source_b "Galois LFSR pseudo-random source."

%feature("docstring") gr_glfsr_source_b::gr_glfsr_source_b "

Params: (degree, repeat, mask, seed)"

%feature("docstring") gr_glfsr_source_b::~gr_glfsr_source_b "

Params: (NONE)"

%feature("docstring") gr_glfsr_source_b::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_glfsr_source_b::period "

Params: (NONE)"

%feature("docstring") gr_glfsr_source_b::mask "

Params: (NONE)"

%feature("docstring") gr_make_glfsr_source_b "Creates a glfsr_source_b block.

Galois LFSR pseudo-random source.

Params: (degree, repeat, mask, seed)"

%feature("docstring") gr_glfsr_source_f "Galois LFSR pseudo-random source generating float outputs -1.0 - 1.0."

%feature("docstring") gr_glfsr_source_f::gr_glfsr_source_f "

Params: (degree, repeat, mask, seed)"

%feature("docstring") gr_glfsr_source_f::~gr_glfsr_source_f "

Params: (NONE)"

%feature("docstring") gr_glfsr_source_f::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_glfsr_source_f::period "

Params: (NONE)"

%feature("docstring") gr_glfsr_source_f::mask "

Params: (NONE)"

%feature("docstring") gr_make_glfsr_source_f "Creates a glfsr_source_f block.

Galois LFSR pseudo-random source generating float outputs -1.0 - 1.0.

Params: (degree, repeat, mask, seed)"

%feature("docstring") gr_goertzel_fc "Goertzel single-bin DFT calculation."

%feature("docstring") gr_goertzel_fc::gr_goertzel_fc "

Params: (rate, len, freq)"

%feature("docstring") gr_goertzel_fc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_goertzel_fc::set_freq "

Params: (freq)"

%feature("docstring") gr_goertzel_fc::set_rate "

Params: (rate)"

%feature("docstring") gr_make_goertzel_fc "Creates a goertzel_fc block.

Goertzel single-bin DFT calculation.

Params: (rate, len, freq)"

%feature("docstring") gr_head "copies the first N items to the output then signals done

Useful for building test cases"

%feature("docstring") gr_head::gr_head "

Params: (sizeof_stream_item, nitems)"

%feature("docstring") gr_head::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_head::reset "

Params: (NONE)"

%feature("docstring") gr_make_head "Creates a head block.

copies the first N items to the output then signals done

Useful for building test cases

Params: (sizeof_stream_item, nitems)"

%feature("docstring") gr_hier_block2 "Hierarchical container class for gr_block's and gr_hier_block2's."

%feature("docstring") gr_hier_block2::gr_hier_block2 "

Params: (name, input_signature, output_signature)"

%feature("docstring") gr_hier_block2::~gr_hier_block2 "

Params: (NONE)"

%feature("docstring") gr_hier_block2::self "Return an object, representing the current block, which can be passed to connect.

The returned object may only be used as an argument to connect or disconnect. Any other use of self() results in unspecified (erroneous) behavior.

Params: (NONE)"

%feature("docstring") gr_hier_block2::connect "Add a stand-alone (possibly hierarchical) block to internal graph.

This adds a gr-block or hierarchical block to the internal graph without wiring it to anything else.

Params: (block)"

%feature("docstring") gr_hier_block2::disconnect "Remove a gr-block or hierarchical block from the internal flowgraph.

This removes a gr-block or hierarchical block from the internal flowgraph, disconnecting it from other blocks as needed.

Params: (block)"

%feature("docstring") gr_hier_block2::disconnect_all "Disconnect all connections in the internal flowgraph.

This call removes all output port to input port connections in the internal flowgraph.

Params: (NONE)"

%feature("docstring") gr_hier_block2::lock "Lock a flowgraph in preparation for reconfiguration. When an equal number of calls to lock() and unlock() have occurred, the flowgraph will be reconfigured.

N.B. lock() and unlock() may not be called from a flowgraph thread (E.g., gr_block::work method) or deadlock will occur when reconfiguration happens.

Params: (NONE)"

%feature("docstring") gr_hier_block2::unlock "Unlock a flowgraph in preparation for reconfiguration. When an equal number of calls to lock() and unlock() have occurred, the flowgraph will be reconfigured.

N.B. lock() and unlock() may not be called from a flowgraph thread (E.g., gr_block::work method) or deadlock will occur when reconfiguration happens.

Params: (NONE)"

%feature("docstring") gr_hier_block2::flatten "

Params: (NONE)"

%feature("docstring") gr_hier_block2::to_hier_block2 "

Params: (NONE)"

%feature("docstring") gr_make_hier_block2 "Creates a hier_block2 block.

Hierarchical container class for gr_block's and gr_hier_block2's.

Params: (name, input_signature, output_signature)"

%feature("docstring") gr_hilbert_fc "Hilbert transformer.

real output is input appropriately delayed. imaginary output is hilbert filtered (90 degree phase shift) version of input."

%feature("docstring") gr_hilbert_fc::~gr_hilbert_fc "

Params: (NONE)"

%feature("docstring") gr_hilbert_fc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_hilbert_fc::gr_hilbert_fc "

Params: (ntaps)"

%feature("docstring") gr_make_hilbert_fc "Creates a hilbert_fc block.

Hilbert transformer.

real output is input appropriately delayed. imaginary output is hilbert filtered (90 degree phase shift) version of input.

Params: (ntaps)"

%feature("docstring") gr_histo_sink_f "Histogram module."

%feature("docstring") gr_histo_sink_f::gr_histo_sink_f "

Params: (msgq)"

%feature("docstring") gr_histo_sink_f::send_frame "

Params: ()"

%feature("docstring") gr_histo_sink_f::clear "

Params: ()"

%feature("docstring") gr_histo_sink_f::~gr_histo_sink_f "

Params: ()"

%feature("docstring") gr_histo_sink_f::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_histo_sink_f::get_frame_size "

Params: ()"

%feature("docstring") gr_histo_sink_f::get_num_bins "

Params: ()"

%feature("docstring") gr_histo_sink_f::set_frame_size "

Params: (frame_size)"

%feature("docstring") gr_histo_sink_f::set_num_bins "

Params: (num_bins)"

%feature("docstring") gr_make_histo_sink_f "Creates a histo_sink_f block.

Histogram module.

Params: (msgq)"

%feature("docstring") gr_iir_filter_ffd "IIR filter with float input, float output and double taps

This filter uses the Direct Form I implementation, where  contains the feed-forward taps, and  the feedback ones.

The input and output satisfy a difference equation of the form



with the corresponding rational system function



Note that some texts define the system function with a + in the denominator. If you're using that convention, you'll need to negate the feedback taps."

%feature("docstring") gr_iir_filter_ffd::gr_iir_filter_ffd "Construct an IIR filter with the given taps

Params: (fftaps, fbtaps)"

%feature("docstring") gr_iir_filter_ffd::~gr_iir_filter_ffd "

Params: (NONE)"

%feature("docstring") gr_iir_filter_ffd::set_taps "

Params: (fftaps, fbtaps)"

%feature("docstring") gr_iir_filter_ffd::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_iir_filter_ffd "Creates a iir_filter_ffd block.

IIR filter with float input, float output and double taps

This filter uses the Direct Form I implementation, where  contains the feed-forward taps, and  the feedback ones.

The input and output satisfy a difference equation of the form



with the corresponding rational system function



Note that some texts define the system function with a + in the denominator. If you're using that convention, you'll need to negate the feedback taps.

Params: (fftaps, fbtaps)"

%feature("docstring") gr_integrate_cc "output = sum(input[0]...input[n])

Integrate successive samples in input stream and decimate"

%feature("docstring") gr_integrate_cc::gr_integrate_cc "

Params: (decim)"

%feature("docstring") gr_integrate_cc::~gr_integrate_cc "

Params: (NONE)"

%feature("docstring") gr_integrate_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_integrate_cc "Creates a integrate_cc block.

output = sum(input[0]...input[n])

Integrate successive samples in input stream and decimate

Params: (decim)"

%feature("docstring") gr_integrate_ff "output = sum(input[0]...input[n])

Integrate successive samples in input stream and decimate"

%feature("docstring") gr_integrate_ff::gr_integrate_ff "

Params: (decim)"

%feature("docstring") gr_integrate_ff::~gr_integrate_ff "

Params: (NONE)"

%feature("docstring") gr_integrate_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_integrate_ff "Creates a integrate_ff block.

output = sum(input[0]...input[n])

Integrate successive samples in input stream and decimate

Params: (decim)"

%feature("docstring") gr_integrate_ii "output = sum(input[0]...input[n])

Integrate successive samples in input stream and decimate"

%feature("docstring") gr_integrate_ii::gr_integrate_ii "

Params: (decim)"

%feature("docstring") gr_integrate_ii::~gr_integrate_ii "

Params: (NONE)"

%feature("docstring") gr_integrate_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_integrate_ii "Creates a integrate_ii block.

output = sum(input[0]...input[n])

Integrate successive samples in input stream and decimate

Params: (decim)"

%feature("docstring") gr_integrate_ss "output = sum(input[0]...input[n])

Integrate successive samples in input stream and decimate"

%feature("docstring") gr_integrate_ss::gr_integrate_ss "

Params: (decim)"

%feature("docstring") gr_integrate_ss::~gr_integrate_ss "

Params: (NONE)"

%feature("docstring") gr_integrate_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_integrate_ss "Creates a integrate_ss block.

output = sum(input[0]...input[n])

Integrate successive samples in input stream and decimate

Params: (decim)"

%feature("docstring") gr_interleave "interleave N inputs to a single output"

%feature("docstring") gr_interleave::gr_interleave "

Params: (itemsize)"

%feature("docstring") gr_interleave::~gr_interleave "

Params: (NONE)"

%feature("docstring") gr_interleave::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_interleave::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_interleave "Creates a interleave block.

interleave N inputs to a single output

Params: (itemsize)"

%feature("docstring") gr_interleaved_short_to_complex "Convert stream of interleaved shorts to a stream of complex."

%feature("docstring") gr_interleaved_short_to_complex::gr_interleaved_short_to_complex "

Params: (NONE)"

%feature("docstring") gr_interleaved_short_to_complex::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_interleaved_short_to_complex "Creates a interleaved_short_to_complex block.

Convert stream of interleaved shorts to a stream of complex.

Params: (NONE)"

%feature("docstring") gr_interp_fir_filter_ccc "Interpolating FIR filter with gr_complex input, gr_complex output and gr_complex taps."

%feature("docstring") gr_interp_fir_filter_ccc::gr_interp_fir_filter_ccc "Construct a FIR filter with the given taps

Params: (interpolation, taps)"

%feature("docstring") gr_interp_fir_filter_ccc::install_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_ccc::~gr_interp_fir_filter_ccc "

Params: (NONE)"

%feature("docstring") gr_interp_fir_filter_ccc::set_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_ccc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_interp_fir_filter_ccc "Creates a interp_fir_filter_ccc block.

Interpolating FIR filter with gr_complex input, gr_complex output and gr_complex taps.

Params: (interpolation, taps)"

%feature("docstring") gr_interp_fir_filter_ccf "Interpolating FIR filter with gr_complex input, gr_complex output and float taps."

%feature("docstring") gr_interp_fir_filter_ccf::gr_interp_fir_filter_ccf "Construct a FIR filter with the given taps

Params: (interpolation, taps)"

%feature("docstring") gr_interp_fir_filter_ccf::install_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_ccf::~gr_interp_fir_filter_ccf "

Params: (NONE)"

%feature("docstring") gr_interp_fir_filter_ccf::set_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_ccf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_interp_fir_filter_ccf "Creates a interp_fir_filter_ccf block.

Interpolating FIR filter with gr_complex input, gr_complex output and float taps.

Params: (interpolation, taps)"

%feature("docstring") gr_interp_fir_filter_fcc "Interpolating FIR filter with float input, gr_complex output and gr_complex taps."

%feature("docstring") gr_interp_fir_filter_fcc::gr_interp_fir_filter_fcc "Construct a FIR filter with the given taps

Params: (interpolation, taps)"

%feature("docstring") gr_interp_fir_filter_fcc::install_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_fcc::~gr_interp_fir_filter_fcc "

Params: (NONE)"

%feature("docstring") gr_interp_fir_filter_fcc::set_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_fcc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_interp_fir_filter_fcc "Creates a interp_fir_filter_fcc block.

Interpolating FIR filter with float input, gr_complex output and gr_complex taps.

Params: (interpolation, taps)"

%feature("docstring") gr_interp_fir_filter_fff "Interpolating FIR filter with float input, float output and float taps."

%feature("docstring") gr_interp_fir_filter_fff::gr_interp_fir_filter_fff "Construct a FIR filter with the given taps

Params: (interpolation, taps)"

%feature("docstring") gr_interp_fir_filter_fff::install_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_fff::~gr_interp_fir_filter_fff "

Params: (NONE)"

%feature("docstring") gr_interp_fir_filter_fff::set_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_fff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_interp_fir_filter_fff "Creates a interp_fir_filter_fff block.

Interpolating FIR filter with float input, float output and float taps.

Params: (interpolation, taps)"

%feature("docstring") gr_interp_fir_filter_fsf "Interpolating FIR filter with float input, short output and float taps."

%feature("docstring") gr_interp_fir_filter_fsf::gr_interp_fir_filter_fsf "Construct a FIR filter with the given taps

Params: (interpolation, taps)"

%feature("docstring") gr_interp_fir_filter_fsf::install_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_fsf::~gr_interp_fir_filter_fsf "

Params: (NONE)"

%feature("docstring") gr_interp_fir_filter_fsf::set_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_fsf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_interp_fir_filter_fsf "Creates a interp_fir_filter_fsf block.

Interpolating FIR filter with float input, short output and float taps.

Params: (interpolation, taps)"

%feature("docstring") gr_interp_fir_filter_scc "Interpolating FIR filter with short input, gr_complex output and gr_complex taps."

%feature("docstring") gr_interp_fir_filter_scc::gr_interp_fir_filter_scc "Construct a FIR filter with the given taps

Params: (interpolation, taps)"

%feature("docstring") gr_interp_fir_filter_scc::install_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_scc::~gr_interp_fir_filter_scc "

Params: (NONE)"

%feature("docstring") gr_interp_fir_filter_scc::set_taps "

Params: (taps)"

%feature("docstring") gr_interp_fir_filter_scc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_interp_fir_filter_scc "Creates a interp_fir_filter_scc block.

Interpolating FIR filter with short input, gr_complex output and gr_complex taps.

Params: (interpolation, taps)"



%feature("docstring") gr_iqcomp_cc::gr_iqcomp_cc "

Params: (mu)"

%feature("docstring") gr_iqcomp_cc::mu "

Params: (NONE)"

%feature("docstring") gr_iqcomp_cc::set_mu "

Params: (mu)"

%feature("docstring") gr_iqcomp_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_iqcomp_cc "Creates a iqcomp_cc block.



Params: (mu)"

%feature("docstring") gr_keep_one_in_n "decimate a stream, keeping one item out of every n."

%feature("docstring") gr_keep_one_in_n::gr_keep_one_in_n "

Params: (item_size, n)"

%feature("docstring") gr_keep_one_in_n::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_keep_one_in_n::set_n "

Params: (n)"

%feature("docstring") gr_make_keep_one_in_n "Creates a keep_one_in_n block.

decimate a stream, keeping one item out of every n.

Params: (item_size, n)"

%feature("docstring") gr_kludge_copy "output[i] = input[i]

This is a short term kludge to work around a problem with the hierarchical block impl."

%feature("docstring") gr_kludge_copy::gr_kludge_copy "

Params: (itemsize)"

%feature("docstring") gr_kludge_copy::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_kludge_copy::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_kludge_copy "Creates a kludge_copy block.

output[i] = input[i]

This is a short term kludge to work around a problem with the hierarchical block impl.

Params: (itemsize)"

%feature("docstring") gr_lfsr_32k_source_s "LFSR pseudo-random source with period of 2^15 bits (2^11 shorts)

This source is typically used along with gr_check_lfsr_32k_s to test the USRP using its digital loopback mode."

%feature("docstring") gr_lfsr_32k_source_s::gr_lfsr_32k_source_s "

Params: (NONE)"

%feature("docstring") gr_lfsr_32k_source_s::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_lfsr_32k_source_s "Creates a lfsr_32k_source_s block.

LFSR pseudo-random source with period of 2^15 bits (2^11 shorts)

This source is typically used along with gr_check_lfsr_32k_s to test the USRP using its digital loopback mode.

Params: (NONE)"

%feature("docstring") gr_lms_dfe_cc "Least-Mean-Square Decision Feedback Equalizer (complex in/out)."

%feature("docstring") gr_lms_dfe_cc::gr_lms_dfe_cc "

Params: (lambda_ff, lambda_fb, num_fftaps, num_fbtaps)"

%feature("docstring") gr_lms_dfe_cc::slicer_0deg "

Params: (baud)"

%feature("docstring") gr_lms_dfe_cc::slicer_45deg "

Params: (baud)"

%feature("docstring") gr_lms_dfe_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_lms_dfe_cc "Creates a lms_dfe_cc block.

Least-Mean-Square Decision Feedback Equalizer (complex in/out).

Params: (lambda_ff, lambda_fb, num_fftaps, num_fbtaps)"

%feature("docstring") gr_lms_dfe_ff "Least-Mean-Square Decision Feedback Equalizer (float in/out)."

%feature("docstring") gr_lms_dfe_ff::gr_lms_dfe_ff "

Params: (lambda_ff, lambda_fb, num_fftaps, num_fbtaps)"

%feature("docstring") gr_lms_dfe_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_lms_dfe_ff "Creates a lms_dfe_ff block.

Least-Mean-Square Decision Feedback Equalizer (float in/out).

Params: (lambda_ff, lambda_fb, num_fftaps, num_fbtaps)"

%feature("docstring") gr_map_bb "output[i] = map[input[i]]"

%feature("docstring") gr_map_bb::gr_map_bb "

Params: (map)"

%feature("docstring") gr_map_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_map_bb "Creates a map_bb block.

output[i] = map[input[i]]

Params: (map)"



%feature("docstring") gr_max_ff::gr_max_ff "

Params: (vlen)"

%feature("docstring") gr_max_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_max_ff "Creates a max_ff block.



Params: (vlen)"



%feature("docstring") gr_max_ii::gr_max_ii "

Params: (vlen)"

%feature("docstring") gr_max_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_max_ii "Creates a max_ii block.



Params: (vlen)"



%feature("docstring") gr_max_ss::gr_max_ss "

Params: (vlen)"

%feature("docstring") gr_max_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_max_ss "Creates a max_ss block.



Params: (vlen)"

%feature("docstring") gr_message "Message class.

The ideas and method names for adjustable message length were lifted from the click modular router \"Packet\" class."

%feature("docstring") gr_message::gr_message "

Params: (type, arg1, arg2, length)"

%feature("docstring") gr_message::buf_data "

Params: (NONE)"

%feature("docstring") gr_message::buf_len "

Params: (NONE)"

%feature("docstring") gr_message::~gr_message "

Params: (NONE)"

%feature("docstring") gr_message::type "

Params: (NONE)"

%feature("docstring") gr_message::arg1 "

Params: (NONE)"

%feature("docstring") gr_message::arg2 "

Params: (NONE)"

%feature("docstring") gr_message::set_type "

Params: (type)"

%feature("docstring") gr_message::set_arg1 "

Params: (arg1)"

%feature("docstring") gr_message::set_arg2 "

Params: (arg2)"

%feature("docstring") gr_message::msg "

Params: (NONE)"

%feature("docstring") gr_message::length "

Params: (NONE)"

%feature("docstring") gr_message::to_string "

Params: (NONE)"

%feature("docstring") gr_make_message "Creates a message block.

Message class.

The ideas and method names for adjustable message length were lifted from the click modular router \"Packet\" class.

Params: (type, arg1, arg2, length)"

%feature("docstring") gr_message_sink "Gather received items into messages and insert into msgq."

%feature("docstring") gr_message_sink::gr_message_sink "

Params: (itemsize, msgq, dont_block)"

%feature("docstring") gr_message_sink::~gr_message_sink "

Params: (NONE)"

%feature("docstring") gr_message_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_message_sink "Creates a message_sink block.

Gather received items into messages and insert into msgq.

Params: (itemsize, msgq, dont_block)"

%feature("docstring") gr_message_source "Turn received messages into a stream."

%feature("docstring") gr_message_source::gr_message_source "

Params: (itemsize, msgq_limit)"

%feature("docstring") gr_message_source::~gr_message_source "

Params: (NONE)"

%feature("docstring") gr_message_source::msgq "

Params: (NONE)"

%feature("docstring") gr_message_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_message_source "Creates a message_source block.

Turn received messages into a stream.

Params: (itemsize, msgq_limit)"

%feature("docstring") gr_moving_average_cc "output is the moving sum of the last N samples, scaled by the scale factor

max_iter limits how long we go without flushing the accumulator This is necessary to avoid numerical instability for float and complex."

%feature("docstring") gr_moving_average_cc::gr_moving_average_cc "

Params: (length, scale, max_iter)"

%feature("docstring") gr_moving_average_cc::~gr_moving_average_cc "

Params: (NONE)"

%feature("docstring") gr_moving_average_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_moving_average_cc::length "

Params: (NONE)"

%feature("docstring") gr_moving_average_cc::scale "

Params: (NONE)"

%feature("docstring") gr_moving_average_cc::set_length_and_scale "

Params: (length, scale)"

%feature("docstring") gr_make_moving_average_cc "Creates a moving_average_cc block.

output is the moving sum of the last N samples, scaled by the scale factor

max_iter limits how long we go without flushing the accumulator This is necessary to avoid numerical instability for float and complex.

Params: (length, scale, max_iter)"

%feature("docstring") gr_moving_average_ff "output is the moving sum of the last N samples, scaled by the scale factor

max_iter limits how long we go without flushing the accumulator This is necessary to avoid numerical instability for float and complex."

%feature("docstring") gr_moving_average_ff::gr_moving_average_ff "

Params: (length, scale, max_iter)"

%feature("docstring") gr_moving_average_ff::~gr_moving_average_ff "

Params: (NONE)"

%feature("docstring") gr_moving_average_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_moving_average_ff::length "

Params: (NONE)"

%feature("docstring") gr_moving_average_ff::scale "

Params: (NONE)"

%feature("docstring") gr_moving_average_ff::set_length_and_scale "

Params: (length, scale)"

%feature("docstring") gr_make_moving_average_ff "Creates a moving_average_ff block.

output is the moving sum of the last N samples, scaled by the scale factor

max_iter limits how long we go without flushing the accumulator This is necessary to avoid numerical instability for float and complex.

Params: (length, scale, max_iter)"

%feature("docstring") gr_moving_average_ii "output is the moving sum of the last N samples, scaled by the scale factor

max_iter limits how long we go without flushing the accumulator This is necessary to avoid numerical instability for float and complex."

%feature("docstring") gr_moving_average_ii::gr_moving_average_ii "

Params: (length, scale, max_iter)"

%feature("docstring") gr_moving_average_ii::~gr_moving_average_ii "

Params: (NONE)"

%feature("docstring") gr_moving_average_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_moving_average_ii::length "

Params: (NONE)"

%feature("docstring") gr_moving_average_ii::scale "

Params: (NONE)"

%feature("docstring") gr_moving_average_ii::set_length_and_scale "

Params: (length, scale)"

%feature("docstring") gr_make_moving_average_ii "Creates a moving_average_ii block.

output is the moving sum of the last N samples, scaled by the scale factor

max_iter limits how long we go without flushing the accumulator This is necessary to avoid numerical instability for float and complex.

Params: (length, scale, max_iter)"

%feature("docstring") gr_moving_average_ss "output is the moving sum of the last N samples, scaled by the scale factor

max_iter limits how long we go without flushing the accumulator This is necessary to avoid numerical instability for float and complex."

%feature("docstring") gr_moving_average_ss::gr_moving_average_ss "

Params: (length, scale, max_iter)"

%feature("docstring") gr_moving_average_ss::~gr_moving_average_ss "

Params: (NONE)"

%feature("docstring") gr_moving_average_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_moving_average_ss::length "

Params: (NONE)"

%feature("docstring") gr_moving_average_ss::scale "

Params: (NONE)"

%feature("docstring") gr_moving_average_ss::set_length_and_scale "

Params: (length, scale)"

%feature("docstring") gr_make_moving_average_ss "Creates a moving_average_ss block.

output is the moving sum of the last N samples, scaled by the scale factor

max_iter limits how long we go without flushing the accumulator This is necessary to avoid numerical instability for float and complex.

Params: (length, scale, max_iter)"

%feature("docstring") gr_mpsk_receiver_cc "This block takes care of receiving M-PSK modulated signals through phase, frequency, and symbol synchronization.

This block takes care of receiving M-PSK modulated signals through phase, frequency, and symbol synchronization. It performs carrier frequency and phase locking as well as symbol timing recovery. It works with (D)BPSK, (D)QPSK, and (D)8PSK as tested currently. It should also work for OQPSK and PI/4 DQPSK.

The phase and frequency synchronization are based on a Costas loop that finds the error of the incoming signal point compared to its nearest constellation point. The frequency and phase of the NCO are updated according to this error. There are optimized phase error detectors for BPSK and QPSK, but 8PSK is done using a brute-force computation of the constellation points to find the minimum.

The symbol synchronization is done using a modified Mueller and Muller circuit from the paper:

G. R. Danesfahani, T.G. Jeans, \"Optimisation of modified Mueller and Muller 
    algorithm,\" Electronics Letters, Vol. 31, no. 13, 22 June 1995, pp. 1032 - 1033.

This circuit interpolates the downconverted sample (using the NCO developed by the Costas loop) every mu samples, then it finds the sampling error based on this and the past symbols and the decision made on the samples. Like the phase error detector, there are optimized decision algorithms for BPSK and QPKS, but 8PSK uses another brute force computation against all possible symbols. The modifications to the M&M used here reduce self-noise."

%feature("docstring") gr_mpsk_receiver_cc::~gr_mpsk_receiver_cc "

Params: (NONE)"

%feature("docstring") gr_mpsk_receiver_cc::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_mpsk_receiver_cc::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_mpsk_receiver_cc::mu "(M&M) Returns current value of mu

Params: (NONE)"

%feature("docstring") gr_mpsk_receiver_cc::omega "(M&M) Returns current value of omega

Params: (NONE)"

%feature("docstring") gr_mpsk_receiver_cc::gain_mu "(M&M) Returns mu gain factor

Params: (NONE)"

%feature("docstring") gr_mpsk_receiver_cc::gain_omega "(M&M) Returns omega gain factor

Params: (NONE)"

%feature("docstring") gr_mpsk_receiver_cc::set_mu "(M&M) Sets value of mu

Params: (mu)"

%feature("docstring") gr_mpsk_receiver_cc::set_omega "(M&M) Sets value of omega and its min and max values

Params: (omega)"

%feature("docstring") gr_mpsk_receiver_cc::set_gain_mu "(M&M) Sets value for mu gain factor

Params: (gain_mu)"

%feature("docstring") gr_mpsk_receiver_cc::set_gain_omega "(M&M) Sets value for omega gain factor

Params: (gain_omega)"

%feature("docstring") gr_mpsk_receiver_cc::alpha "(CL) Returns the value for alpha (the phase gain term)

Params: (NONE)"

%feature("docstring") gr_mpsk_receiver_cc::beta "(CL) Returns the value of beta (the frequency gain term)

Params: (NONE)"

%feature("docstring") gr_mpsk_receiver_cc::freq "(CL) Returns the current value of the frequency of the NCO in the Costas loop

Params: (NONE)"

%feature("docstring") gr_mpsk_receiver_cc::phase "(CL) Returns the current value of the phase of the NCO in the Costal loop

Params: (NONE)"

%feature("docstring") gr_mpsk_receiver_cc::set_alpha "(CL) Sets the value for alpha (the phase gain term)

Params: (alpha)"

%feature("docstring") gr_mpsk_receiver_cc::set_beta "(CL) Setss the value of beta (the frequency gain term)

Params: (beta)"

%feature("docstring") gr_mpsk_receiver_cc::set_freq "(CL) Sets the current value of the frequency of the NCO in the Costas loop

Params: (freq)"

%feature("docstring") gr_mpsk_receiver_cc::set_phase "(CL) Setss the current value of the phase of the NCO in the Costal loop

Params: (phase)"

%feature("docstring") gr_mpsk_receiver_cc::gr_mpsk_receiver_cc "Constructor to synchronize incoming M-PSK symbols.

The constructor also chooses which phase detector and decision maker to use in the work loop based on the value of M.

Params: (M, theta, alpha, beta, fmin, fmax, mu, gain_mu, omega, gain_omega, omega_rel)"

%feature("docstring") gr_mpsk_receiver_cc::make_constellation "

Params: (NONE)"

%feature("docstring") gr_mpsk_receiver_cc::mm_sampler "

Params: (symbol)"

%feature("docstring") gr_mpsk_receiver_cc::mm_error_tracking "

Params: (sample)"

%feature("docstring") gr_mpsk_receiver_cc::phase_error_tracking "

Params: (sample)"

%feature("docstring") gr_mpsk_receiver_cc::phase_error_detector_generic "Phase error detector for MPSK modulations.

This function determines the phase error for any MPSK signal by creating a set of PSK constellation points and doing a brute-force search to see which point minimizes the Euclidean distance. This point is then used to derotate the sample to the real-axis and a atan (using the fast approximation function) to determine the phase difference between the incoming sample and the real constellation point

This should be cleaned up and made more efficient.

Params: (sample)"

%feature("docstring") gr_mpsk_receiver_cc::phase_error_detector_bpsk "Phase error detector for BPSK modulation.

This function determines the phase error using a simple BPSK phase error detector by multiplying the real and imaginary (the error signal) components together. As the imaginary part goes to 0, so does this error.

Params: (sample)"

%feature("docstring") gr_mpsk_receiver_cc::phase_error_detector_qpsk "Phase error detector for QPSK modulation.

This function determines the phase error using the limiter approach in a standard 4th order Costas loop

Params: (sample)"

%feature("docstring") gr_mpsk_receiver_cc::decision_generic "Decision maker for a generic MPSK constellation.

This decision maker is a generic implementation that does a brute-force search for the constellation point that minimizes the error between it and the incoming signal.

Params: (sample)"

%feature("docstring") gr_mpsk_receiver_cc::decision_bpsk "Decision maker for BPSK constellation.

This decision maker is a simple slicer function that makes a decision on the symbol based on its placement on the real axis of greater than 0 or less than 0; the quadrature component is always 0.

Params: (sample)"

%feature("docstring") gr_mpsk_receiver_cc::decision_qpsk "Decision maker for QPSK constellation.

This decision maker is a simple slicer function that makes a decision on the symbol based on its placement versus both axes and returns which quadrant the symbol is in.

Params: (sample)"

%feature("docstring") gr_mpsk_receiver_cc::__attribute__ "delay line plus some length for overflow protection

Params: ()"

%feature("docstring") gr_make_mpsk_receiver_cc "Creates a mpsk_receiver_cc block.

This block takes care of receiving M-PSK modulated signals through phase, frequency, and symbol synchronization.

This block takes care of receiving M-PSK modulated signals through phase, frequency, and symbol synchronization. It performs carrier frequency and phase locking as well as symbol timing recovery. It works with (D)BPSK, (D)QPSK, and (D)8PSK as tested currently. It should also work for OQPSK and PI/4 DQPSK.

The phase and frequency synchronization are based on a Costas loop that finds the error of the incoming signal point compared to its nearest constellation point. The frequency and phase of the NCO are updated according to this error. There are optimized phase error detectors for BPSK and QPSK, but 8PSK is done using a brute-force computation of the constellation points to find the minimum.

The symbol synchronization is done using a modified Mueller and Muller circuit from the paper:

G. R. Danesfahani, T.G. Jeans, \"Optimisation of modified Mueller and Muller 
    algorithm,\" Electronics Letters, Vol. 31, no. 13, 22 June 1995, pp. 1032 - 1033.

This circuit interpolates the downconverted sample (using the NCO developed by the Costas loop) every mu samples, then it finds the sampling error based on this and the past symbols and the decision made on the samples. Like the phase error detector, there are optimized decision algorithms for BPSK and QPKS, but 8PSK uses another brute force computation against all possible symbols. The modifications to the M&M used here reduce self-noise.

Params: (M, theta, alpha, beta, fmin, fmax, mu, gain_mu, omega, gain_omega, omega_rel)"

%feature("docstring") gr_multiply_cc "output = prod (input_0, input_1, ...)

Multiply across all input streams."

%feature("docstring") gr_multiply_cc::gr_multiply_cc "

Params: (vlen)"

%feature("docstring") gr_multiply_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_cc "Creates a multiply_cc block.

output = prod (input_0, input_1, ...)

Multiply across all input streams.

Params: (vlen)"

%feature("docstring") gr_multiply_const_cc "output = input * constant"

%feature("docstring") gr_multiply_const_cc::gr_multiply_const_cc "

Params: (k)"

%feature("docstring") gr_multiply_const_cc::k "

Params: (NONE)"

%feature("docstring") gr_multiply_const_cc::set_k "

Params: (k)"

%feature("docstring") gr_multiply_const_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_const_cc "Creates a multiply_const_cc block.

output = input * constant

Params: (k)"

%feature("docstring") gr_multiply_const_ff "output = input * constant"

%feature("docstring") gr_multiply_const_ff::gr_multiply_const_ff "

Params: (k)"

%feature("docstring") gr_multiply_const_ff::k "

Params: (NONE)"

%feature("docstring") gr_multiply_const_ff::set_k "

Params: (k)"

%feature("docstring") gr_multiply_const_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_const_ff "Creates a multiply_const_ff block.

output = input * constant

Params: (k)"

%feature("docstring") gr_multiply_const_ii "output = input * constant"

%feature("docstring") gr_multiply_const_ii::gr_multiply_const_ii "

Params: (k)"

%feature("docstring") gr_multiply_const_ii::k "

Params: (NONE)"

%feature("docstring") gr_multiply_const_ii::set_k "

Params: (k)"

%feature("docstring") gr_multiply_const_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_const_ii "Creates a multiply_const_ii block.

output = input * constant

Params: (k)"

%feature("docstring") gr_multiply_const_ss "output = input * constant"

%feature("docstring") gr_multiply_const_ss::gr_multiply_const_ss "

Params: (k)"

%feature("docstring") gr_multiply_const_ss::k "

Params: (NONE)"

%feature("docstring") gr_multiply_const_ss::set_k "

Params: (k)"

%feature("docstring") gr_multiply_const_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_const_ss "Creates a multiply_const_ss block.

output = input * constant

Params: (k)"

%feature("docstring") gr_multiply_const_vcc "output vector = input vector * constant vector (element-wise)"

%feature("docstring") gr_multiply_const_vcc::gr_multiply_const_vcc "

Params: (k)"

%feature("docstring") gr_multiply_const_vcc::k "

Params: (NONE)"

%feature("docstring") gr_multiply_const_vcc::set_k "

Params: (k)"

%feature("docstring") gr_multiply_const_vcc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_const_vcc "Creates a multiply_const_vcc block.

output vector = input vector * constant vector (element-wise)

Params: (k)"

%feature("docstring") gr_multiply_const_vff "output vector = input vector * constant vector (element-wise)"

%feature("docstring") gr_multiply_const_vff::gr_multiply_const_vff "

Params: (k)"

%feature("docstring") gr_multiply_const_vff::k "

Params: (NONE)"

%feature("docstring") gr_multiply_const_vff::set_k "

Params: (k)"

%feature("docstring") gr_multiply_const_vff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_const_vff "Creates a multiply_const_vff block.

output vector = input vector * constant vector (element-wise)

Params: (k)"

%feature("docstring") gr_multiply_const_vii "output vector = input vector * constant vector (element-wise)"

%feature("docstring") gr_multiply_const_vii::gr_multiply_const_vii "

Params: (k)"

%feature("docstring") gr_multiply_const_vii::k "

Params: (NONE)"

%feature("docstring") gr_multiply_const_vii::set_k "

Params: (k)"

%feature("docstring") gr_multiply_const_vii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_const_vii "Creates a multiply_const_vii block.

output vector = input vector * constant vector (element-wise)

Params: (k)"

%feature("docstring") gr_multiply_const_vss "output vector = input vector * constant vector (element-wise)"

%feature("docstring") gr_multiply_const_vss::gr_multiply_const_vss "

Params: (k)"

%feature("docstring") gr_multiply_const_vss::k "

Params: (NONE)"

%feature("docstring") gr_multiply_const_vss::set_k "

Params: (k)"

%feature("docstring") gr_multiply_const_vss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_const_vss "Creates a multiply_const_vss block.

output vector = input vector * constant vector (element-wise)

Params: (k)"

%feature("docstring") gr_multiply_ff "output = prod (input_0, input_1, ...)

Multiply across all input streams."

%feature("docstring") gr_multiply_ff::gr_multiply_ff "

Params: (vlen)"

%feature("docstring") gr_multiply_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_ff "Creates a multiply_ff block.

output = prod (input_0, input_1, ...)

Multiply across all input streams.

Params: (vlen)"

%feature("docstring") gr_multiply_ii "output = prod (input_0, input_1, ...)

Multiply across all input streams."

%feature("docstring") gr_multiply_ii::gr_multiply_ii "

Params: (vlen)"

%feature("docstring") gr_multiply_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_ii "Creates a multiply_ii block.

output = prod (input_0, input_1, ...)

Multiply across all input streams.

Params: (vlen)"

%feature("docstring") gr_multiply_ss "output = prod (input_0, input_1, ...)

Multiply across all input streams."

%feature("docstring") gr_multiply_ss::gr_multiply_ss "

Params: (vlen)"

%feature("docstring") gr_multiply_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_multiply_ss "Creates a multiply_ss block.

output = prod (input_0, input_1, ...)

Multiply across all input streams.

Params: (vlen)"

%feature("docstring") gr_mute_cc "output = input or zero if muted."

%feature("docstring") gr_mute_cc::gr_mute_cc "

Params: (mute)"

%feature("docstring") gr_mute_cc::mute "

Params: (NONE)"

%feature("docstring") gr_mute_cc::set_mute "

Params: (mute)"

%feature("docstring") gr_mute_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_mute_cc "Creates a mute_cc block.

output = input or zero if muted.

Params: (mute)"

%feature("docstring") gr_mute_ff "output = input or zero if muted."

%feature("docstring") gr_mute_ff::gr_mute_ff "

Params: (mute)"

%feature("docstring") gr_mute_ff::mute "

Params: (NONE)"

%feature("docstring") gr_mute_ff::set_mute "

Params: (mute)"

%feature("docstring") gr_mute_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_mute_ff "Creates a mute_ff block.

output = input or zero if muted.

Params: (mute)"

%feature("docstring") gr_mute_ii "output = input or zero if muted."

%feature("docstring") gr_mute_ii::gr_mute_ii "

Params: (mute)"

%feature("docstring") gr_mute_ii::mute "

Params: (NONE)"

%feature("docstring") gr_mute_ii::set_mute "

Params: (mute)"

%feature("docstring") gr_mute_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_mute_ii "Creates a mute_ii block.

output = input or zero if muted.

Params: (mute)"

%feature("docstring") gr_mute_ss "output = input or zero if muted."

%feature("docstring") gr_mute_ss::gr_mute_ss "

Params: (mute)"

%feature("docstring") gr_mute_ss::mute "

Params: (NONE)"

%feature("docstring") gr_mute_ss::set_mute "

Params: (mute)"

%feature("docstring") gr_mute_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_mute_ss "Creates a mute_ss block.

output = input or zero if muted.

Params: (mute)"

%feature("docstring") gr_nlog10_ff "output = n*log10(input) + k"

%feature("docstring") gr_nlog10_ff::gr_nlog10_ff "

Params: (n, vlen, k)"

%feature("docstring") gr_nlog10_ff::~gr_nlog10_ff "

Params: (NONE)"

%feature("docstring") gr_nlog10_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_nlog10_ff "Creates a nlog10_ff block.

output = n*log10(input) + k

Params: (n, vlen, k)"

%feature("docstring") gr_noise_source_c "random number source"

%feature("docstring") gr_noise_source_c::gr_noise_source_c "

Params: (type, ampl, seed)"

%feature("docstring") gr_noise_source_c::set_type "

Params: (type)"

%feature("docstring") gr_noise_source_c::set_amplitude "

Params: (ampl)"

%feature("docstring") gr_noise_source_c::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_noise_source_c "Creates a noise_source_c block.

random number source

Params: (type, ampl, seed)"

%feature("docstring") gr_noise_source_f "random number source"

%feature("docstring") gr_noise_source_f::gr_noise_source_f "

Params: (type, ampl, seed)"

%feature("docstring") gr_noise_source_f::set_type "

Params: (type)"

%feature("docstring") gr_noise_source_f::set_amplitude "

Params: (ampl)"

%feature("docstring") gr_noise_source_f::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_noise_source_f "Creates a noise_source_f block.

random number source

Params: (type, ampl, seed)"

%feature("docstring") gr_noise_source_i "random number source"

%feature("docstring") gr_noise_source_i::gr_noise_source_i "

Params: (type, ampl, seed)"

%feature("docstring") gr_noise_source_i::set_type "

Params: (type)"

%feature("docstring") gr_noise_source_i::set_amplitude "

Params: (ampl)"

%feature("docstring") gr_noise_source_i::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_noise_source_i "Creates a noise_source_i block.

random number source

Params: (type, ampl, seed)"

%feature("docstring") gr_noise_source_s "random number source"

%feature("docstring") gr_noise_source_s::gr_noise_source_s "

Params: (type, ampl, seed)"

%feature("docstring") gr_noise_source_s::set_type "

Params: (type)"

%feature("docstring") gr_noise_source_s::set_amplitude "

Params: (ampl)"

%feature("docstring") gr_noise_source_s::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_noise_source_s "Creates a noise_source_s block.

random number source

Params: (type, ampl, seed)"

%feature("docstring") gr_nonrecursive_convolutional_encoder_bb "Peforms nonrecursive convolutional encoding. ???

More detailed description goes here."

%feature("docstring") gr_nonrecursive_convolutional_encoder_bb::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_nonrecursive_convolutional_encoder_bb::gr_nonrecursive_convolutional_encoder_bb "Constructor to perform nonrecursive convolutional encoding.

Params: (generator_coeffs)"

%feature("docstring") gr_nonrecursive_convolutional_encoder_bb::generate_partab "

Params: (NONE)"

%feature("docstring") gr_make_nonrecursive_convolutional_encoder_bb "Creates a nonrecursive_convolutional_encoder_bb block.

Peforms nonrecursive convolutional encoding. ???

More detailed description goes here.

Params: (generator_coeffs)"

%feature("docstring") gr_nop "Does nothing. Used for testing only."

%feature("docstring") gr_nop::gr_nop "

Params: (sizeof_stream_item)"

%feature("docstring") gr_nop::count_received_msgs "

Params: (msg)"

%feature("docstring") gr_nop::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_nop::nmsgs_received "

Params: (NONE)"

%feature("docstring") gr_make_nop "Creates a nop block.

Does nothing. Used for testing only.

Params: (sizeof_stream_item)"

%feature("docstring") gr_not_bb "output = ~input_0

bitwise boolean not across input stream."

%feature("docstring") gr_not_bb::gr_not_bb "

Params: (NONE)"

%feature("docstring") gr_not_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_not_bb "Creates a not_bb block.

output = ~input_0

bitwise boolean not across input stream.

Params: (NONE)"

%feature("docstring") gr_not_ii "output = ~input_0

bitwise boolean not across input stream."

%feature("docstring") gr_not_ii::gr_not_ii "

Params: (NONE)"

%feature("docstring") gr_not_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_not_ii "Creates a not_ii block.

output = ~input_0

bitwise boolean not across input stream.

Params: (NONE)"

%feature("docstring") gr_not_ss "output = ~input_0

bitwise boolean not across input stream."

%feature("docstring") gr_not_ss::gr_not_ss "

Params: (NONE)"

%feature("docstring") gr_not_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_not_ss "Creates a not_ss block.

output = ~input_0

bitwise boolean not across input stream.

Params: (NONE)"

%feature("docstring") gr_null_sink "Bit bucket."

%feature("docstring") gr_null_sink::gr_null_sink "

Params: (sizeof_stream_item)"

%feature("docstring") gr_null_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_null_sink "Creates a null_sink block.

Bit bucket.

Params: (sizeof_stream_item)"

%feature("docstring") gr_null_source "A source of zeros."

%feature("docstring") gr_null_source::gr_null_source "

Params: (sizeof_stream_item)"

%feature("docstring") gr_null_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_null_source "Creates a null_source block.

A source of zeros.

Params: (sizeof_stream_item)"

%feature("docstring") gr_ofdm_bpsk_demapper "take a vector of complex constellation points in from an FFT and demodulate to a stream of bits. Simple BPSK version."

%feature("docstring") gr_ofdm_bpsk_demapper::gr_ofdm_bpsk_demapper "

Params: (occupied_carriers)"

%feature("docstring") gr_ofdm_bpsk_demapper::slicer "

Params: (x)"

%feature("docstring") gr_ofdm_bpsk_demapper::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_ofdm_bpsk_demapper::~gr_ofdm_bpsk_demapper "

Params: ()"

%feature("docstring") gr_ofdm_bpsk_demapper::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_ofdm_bpsk_demapper "Creates a ofdm_bpsk_demapper block.

take a vector of complex constellation points in from an FFT and demodulate to a stream of bits. Simple BPSK version.

Params: (occupied_carriers)"

%feature("docstring") gr_ofdm_cyclic_prefixer "adds a cyclic prefix vector to an input size long ofdm symbol(vector) and converts vector to a stream output_size long."

%feature("docstring") gr_ofdm_cyclic_prefixer::gr_ofdm_cyclic_prefixer "

Params: (input_size, output_size)"

%feature("docstring") gr_ofdm_cyclic_prefixer::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_ofdm_cyclic_prefixer "Creates a ofdm_cyclic_prefixer block.

adds a cyclic prefix vector to an input size long ofdm symbol(vector) and converts vector to a stream output_size long.

Params: (input_size, output_size)"

%feature("docstring") gr_ofdm_demapper_vcb "take a stream of vectors in from an FFT and demodulate to a stream of bits. Abstract class must be subclassed with specific mapping."

%feature("docstring") gr_ofdm_demapper_vcb::gr_ofdm_demapper_vcb "

Params: (bits_per_symbol, vlen)"

%feature("docstring") gr_ofdm_demapper_vcb::~gr_ofdm_demapper_vcb "

Params: ()"

%feature("docstring") gr_make_ofdm_demapper_vcb "Creates a ofdm_demapper_vcb block.

take a stream of vectors in from an FFT and demodulate to a stream of bits. Abstract class must be subclassed with specific mapping.

Params: (bits_per_symbol, vlen)"

%feature("docstring") gr_ofdm_frame_acquisition "take a vector of complex constellation points in from an FFT and performs a correlation and equalization.

This block takes the output of an FFT of a received OFDM symbol and finds the start of a frame based on two known symbols. It also looks at the surrounding bins in the FFT output for the correlation in case there is a large frequency shift in the data. This block assumes that the fine frequency shift has already been corrected and that the samples fall in the middle of one FFT bin.

It then uses one of those known symbols to estimate the channel response over all subcarriers and does a simple 1-tap equalization on all subcarriers. This corrects for the phase and amplitude distortion caused by the channel."

%feature("docstring") gr_ofdm_frame_acquisition::gr_ofdm_frame_acquisition "

Params: (occupied_carriers, fft_length, cplen, known_symbol, max_fft_shift_len)"

%feature("docstring") gr_ofdm_frame_acquisition::slicer "

Params: (x)"

%feature("docstring") gr_ofdm_frame_acquisition::correlate "

Params: (symbol, zeros_on_left)"

%feature("docstring") gr_ofdm_frame_acquisition::calculate_equalizer "

Params: (symbol, zeros_on_left)"

%feature("docstring") gr_ofdm_frame_acquisition::coarse_freq_comp "

Params: (freq_delta, count)"

%feature("docstring") gr_ofdm_frame_acquisition::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_ofdm_frame_acquisition::snr "Return an estimate of the SNR of the channel.

Params: (NONE)"

%feature("docstring") gr_ofdm_frame_acquisition::~gr_ofdm_frame_acquisition "

Params: ()"

%feature("docstring") gr_ofdm_frame_acquisition::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_ofdm_frame_acquisition "Creates a ofdm_frame_acquisition block.

take a vector of complex constellation points in from an FFT and performs a correlation and equalization.

This block takes the output of an FFT of a received OFDM symbol and finds the start of a frame based on two known symbols. It also looks at the surrounding bins in the FFT output for the correlation in case there is a large frequency shift in the data. This block assumes that the fine frequency shift has already been corrected and that the samples fall in the middle of one FFT bin.

It then uses one of those known symbols to estimate the channel response over all subcarriers and does a simple 1-tap equalization on all subcarriers. This corrects for the phase and amplitude distortion caused by the channel.

Params: (occupied_carriers, fft_length, cplen, known_symbol, max_fft_shift_len)"

%feature("docstring") gr_ofdm_frame_sink "Takes an OFDM symbol in, demaps it into bits of 0's and 1's, packs them into packets, and sends to to a message queue sink.

NOTE: The mod input parameter simply chooses a pre-defined demapper/slicer. Eventually, we want to be able to pass in a reference to an object to do the demapping and slicing for a given modulation type."

%feature("docstring") gr_ofdm_frame_sink::gr_ofdm_frame_sink "

Params: (sym_position, sym_value_out, target_queue, occupied_tones, phase_gain, freq_gain)"

%feature("docstring") gr_ofdm_frame_sink::enter_search "

Params: (NONE)"

%feature("docstring") gr_ofdm_frame_sink::enter_have_sync "

Params: (NONE)"

%feature("docstring") gr_ofdm_frame_sink::enter_have_header "

Params: (NONE)"

%feature("docstring") gr_ofdm_frame_sink::header_ok "

Params: (NONE)"

%feature("docstring") gr_ofdm_frame_sink::slicer "

Params: (x)"

%feature("docstring") gr_ofdm_frame_sink::demapper "

Params: (in, out)"

%feature("docstring") gr_ofdm_frame_sink::set_sym_value_out "

Params: (sym_position, sym_value_out)"

%feature("docstring") gr_ofdm_frame_sink::~gr_ofdm_frame_sink "

Params: (NONE)"

%feature("docstring") gr_ofdm_frame_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_ofdm_frame_sink "Creates a ofdm_frame_sink block.

Takes an OFDM symbol in, demaps it into bits of 0's and 1's, packs them into packets, and sends to to a message queue sink.

NOTE: The mod input parameter simply chooses a pre-defined demapper/slicer. Eventually, we want to be able to pass in a reference to an object to do the demapping and slicing for a given modulation type.

Params: (sym_position, sym_value_out, target_queue, occupied_tones, phase_gain, freq_gain)"

%feature("docstring") gr_ofdm_insert_preamble "insert \"pre-modulated\" preamble symbols before each payload."

%feature("docstring") gr_ofdm_insert_preamble::gr_ofdm_insert_preamble "

Params: (fft_length, preamble)"

%feature("docstring") gr_ofdm_insert_preamble::enter_idle "

Params: (NONE)"

%feature("docstring") gr_ofdm_insert_preamble::enter_preamble "

Params: (NONE)"

%feature("docstring") gr_ofdm_insert_preamble::enter_first_payload "

Params: (NONE)"

%feature("docstring") gr_ofdm_insert_preamble::enter_payload "

Params: (NONE)"

%feature("docstring") gr_ofdm_insert_preamble::~gr_ofdm_insert_preamble "

Params: (NONE)"

%feature("docstring") gr_ofdm_insert_preamble::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_ofdm_insert_preamble "Creates a ofdm_insert_preamble block.

insert \"pre-modulated\" preamble symbols before each payload.

Params: (fft_length, preamble)"

%feature("docstring") gr_ofdm_mapper_bcv "take a stream of bytes in and map to a vector of complex constellation points suitable for IFFT input to be used in an ofdm modulator. Abstract class must be subclassed with specific mapping."

%feature("docstring") gr_ofdm_mapper_bcv::gr_ofdm_mapper_bcv "

Params: (constellation, msgq_limit, occupied_carriers, fft_length)"

%feature("docstring") gr_ofdm_mapper_bcv::randsym "

Params: (NONE)"

%feature("docstring") gr_ofdm_mapper_bcv::~gr_ofdm_mapper_bcv "

Params: ()"

%feature("docstring") gr_ofdm_mapper_bcv::msgq "

Params: (NONE)"

%feature("docstring") gr_ofdm_mapper_bcv::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_ofdm_mapper_bcv "Creates a ofdm_mapper_bcv block.

take a stream of bytes in and map to a vector of complex constellation points suitable for IFFT input to be used in an ofdm modulator. Abstract class must be subclassed with specific mapping.

Params: (constellation, msgq_limit, occupied_carriers, fft_length)"

%feature("docstring") gr_ofdm_sampler "does the rest of the OFDM stuff"

%feature("docstring") gr_ofdm_sampler::gr_ofdm_sampler "

Params: (fft_length, symbol_length, timeout)"

%feature("docstring") gr_ofdm_sampler::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_ofdm_sampler::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_ofdm_sampler "Creates a ofdm_sampler block.

does the rest of the OFDM stuff

Params: (fft_length, symbol_length, timeout)"

%feature("docstring") gr_or_bb "output = input_0 | input_1 | , ... | input_N)

bitwise boolean or across all input streams."

%feature("docstring") gr_or_bb::gr_or_bb "

Params: (NONE)"

%feature("docstring") gr_or_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_or_bb "Creates a or_bb block.

output = input_0 | input_1 | , ... | input_N)

bitwise boolean or across all input streams.

Params: (NONE)"

%feature("docstring") gr_or_ii "output = input_0 | input_1 | , ... | input_N)

bitwise boolean or across all input streams."

%feature("docstring") gr_or_ii::gr_or_ii "

Params: (NONE)"

%feature("docstring") gr_or_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_or_ii "Creates a or_ii block.

output = input_0 | input_1 | , ... | input_N)

bitwise boolean or across all input streams.

Params: (NONE)"

%feature("docstring") gr_or_ss "output = input_0 | input_1 | , ... | input_N)

bitwise boolean or across all input streams."

%feature("docstring") gr_or_ss::gr_or_ss "

Params: (NONE)"

%feature("docstring") gr_or_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_or_ss "Creates a or_ss block.

output = input_0 | input_1 | , ... | input_N)

bitwise boolean or across all input streams.

Params: (NONE)"

%feature("docstring") gr_oscope_sink_f "Building block for python oscilloscope module.

Accepts multiple float streams."

%feature("docstring") gr_oscope_sink_f::gr_oscope_sink_f "

Params: (sampling_rate, msgq)"

%feature("docstring") gr_oscope_sink_f::~gr_oscope_sink_f "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_f::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_oscope_sink_f::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_oscope_sink_f "Creates a oscope_sink_f block.

Building block for python oscilloscope module.

Accepts multiple float streams.

Params: (sampling_rate, msgq)"

%feature("docstring") gr_pa_2x2_phase_combiner "pa_2x2 phase combiner

Anntenas are arranged like this:

2 3 0 1

dx and dy are lambda/2."

%feature("docstring") gr_pa_2x2_phase_combiner::gr_pa_2x2_phase_combiner "

Params: (NONE)"

%feature("docstring") gr_pa_2x2_phase_combiner::theta "

Params: (NONE)"

%feature("docstring") gr_pa_2x2_phase_combiner::set_theta "

Params: (theta)"

%feature("docstring") gr_pa_2x2_phase_combiner::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_pa_2x2_phase_combiner "Creates a pa_2x2_phase_combiner block.

pa_2x2 phase combiner

Anntenas are arranged like this:

2 3 0 1

dx and dy are lambda/2.

Params: (NONE)"

%feature("docstring") gr_packed_to_unpacked_bb "Convert a stream of packed bytes or shorts to stream of unpacked bytes or shorts.

input: stream of unsigned char; output: stream of unsigned char.

This is the inverse of gr_unpacked_to_packed_XX.

The bits in the bytes or shorts input stream are grouped into chunks of  bits and each resulting chunk is written right- justified to the output stream of bytes or shorts. All b or 16 bits of the each input bytes or short are processed. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX_ followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_packed_to_unpacked_bb::gr_packed_to_unpacked_bb "

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_packed_to_unpacked_bb::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_packed_to_unpacked_bb::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_packed_to_unpacked_bb::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_packed_to_unpacked_bb "Creates a packed_to_unpacked_bb block.

Convert a stream of packed bytes or shorts to stream of unpacked bytes or shorts.

input: stream of unsigned char; output: stream of unsigned char.

This is the inverse of gr_unpacked_to_packed_XX.

The bits in the bytes or shorts input stream are grouped into chunks of  bits and each resulting chunk is written right- justified to the output stream of bytes or shorts. All b or 16 bits of the each input bytes or short are processed. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX_ followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_packed_to_unpacked_ii "Convert a stream of packed bytes or shorts to stream of unpacked bytes or shorts.

input: stream of int; output: stream of int.

This is the inverse of gr_unpacked_to_packed_XX.

The bits in the bytes or shorts input stream are grouped into chunks of  bits and each resulting chunk is written right- justified to the output stream of bytes or shorts. All b or 16 bits of the each input bytes or short are processed. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX_ followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_packed_to_unpacked_ii::gr_packed_to_unpacked_ii "

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_packed_to_unpacked_ii::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_packed_to_unpacked_ii::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_packed_to_unpacked_ii::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_packed_to_unpacked_ii "Creates a packed_to_unpacked_ii block.

Convert a stream of packed bytes or shorts to stream of unpacked bytes or shorts.

input: stream of int; output: stream of int.

This is the inverse of gr_unpacked_to_packed_XX.

The bits in the bytes or shorts input stream are grouped into chunks of  bits and each resulting chunk is written right- justified to the output stream of bytes or shorts. All b or 16 bits of the each input bytes or short are processed. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX_ followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_packed_to_unpacked_ss "Convert a stream of packed bytes or shorts to stream of unpacked bytes or shorts.

input: stream of short; output: stream of short.

This is the inverse of gr_unpacked_to_packed_XX.

The bits in the bytes or shorts input stream are grouped into chunks of  bits and each resulting chunk is written right- justified to the output stream of bytes or shorts. All b or 16 bits of the each input bytes or short are processed. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX_ followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_packed_to_unpacked_ss::gr_packed_to_unpacked_ss "

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_packed_to_unpacked_ss::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_packed_to_unpacked_ss::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_packed_to_unpacked_ss::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_packed_to_unpacked_ss "Creates a packed_to_unpacked_ss block.

Convert a stream of packed bytes or shorts to stream of unpacked bytes or shorts.

input: stream of short; output: stream of short.

This is the inverse of gr_unpacked_to_packed_XX.

The bits in the bytes or shorts input stream are grouped into chunks of  bits and each resulting chunk is written right- justified to the output stream of bytes or shorts. All b or 16 bits of the each input bytes or short are processed. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX_ followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_packet_sink "process received bits looking for packet sync, header, and process bits into packet"

%feature("docstring") gr_packet_sink::gr_packet_sink "

Params: (sync_vector, target_queue, threshold)"

%feature("docstring") gr_packet_sink::enter_search "

Params: (NONE)"

%feature("docstring") gr_packet_sink::enter_have_sync "

Params: (NONE)"

%feature("docstring") gr_packet_sink::enter_have_header "

Params: (payload_len)"

%feature("docstring") gr_packet_sink::slice "

Params: (x)"

%feature("docstring") gr_packet_sink::header_ok "

Params: (NONE)"

%feature("docstring") gr_packet_sink::header_payload_len "

Params: (NONE)"

%feature("docstring") gr_packet_sink::~gr_packet_sink "

Params: (NONE)"

%feature("docstring") gr_packet_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_packet_sink::carrier_sensed "return true if we detect carrier

Params: (NONE)"

%feature("docstring") gr_make_packet_sink "Creates a packet_sink block.

process received bits looking for packet sync, header, and process bits into packet

Params: (sync_vector, target_queue, threshold)"

%feature("docstring") gr_peak_detector2_fb "Detect the peak of a signal

If a peak is detected, this block outputs a 1, or it outputs 0's. A separate debug output may be connected, to view the internal EWMA described below."

%feature("docstring") gr_peak_detector2_fb::gr_peak_detector2_fb "

Params: (threshold_factor_rise, look_ahead, alpha)"

%feature("docstring") gr_peak_detector2_fb::set_threshold_factor_rise "Set the threshold factor value for the rise time.

Params: (thr)"

%feature("docstring") gr_peak_detector2_fb::set_look_ahead "Set the look-ahead factor.

Params: (look)"

%feature("docstring") gr_peak_detector2_fb::set_alpha "Set the running average alpha.

Params: (alpha)"

%feature("docstring") gr_peak_detector2_fb::threshold_factor_rise "Get the threshold factor value for the rise time.

Params: (NONE)"

%feature("docstring") gr_peak_detector2_fb::look_ahead "Get the look-ahead factor value.

Params: (NONE)"

%feature("docstring") gr_peak_detector2_fb::alpha "Get the alpha value of the running average.

Params: (NONE)"

%feature("docstring") gr_peak_detector2_fb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_peak_detector2_fb "Creates a peak_detector2_fb block.

Detect the peak of a signal

If a peak is detected, this block outputs a 1, or it outputs 0's. A separate debug output may be connected, to view the internal EWMA described below.

Params: (threshold_factor_rise, look_ahead, alpha)"

%feature("docstring") gr_peak_detector_fb "Detect the peak of a signal

If a peak is detected, this block outputs a 1, or it outputs 0's."

%feature("docstring") gr_peak_detector_fb::gr_peak_detector_fb "

Params: (threshold_factor_rise, threshold_factor_fall, look_ahead, alpha)"

%feature("docstring") gr_peak_detector_fb::set_threshold_factor_rise "Set the threshold factor value for the rise time.

Params: (thr)"

%feature("docstring") gr_peak_detector_fb::set_threshold_factor_fall "Set the threshold factor value for the fall time.

Params: (thr)"

%feature("docstring") gr_peak_detector_fb::set_look_ahead "Set the look-ahead factor.

Params: (look)"

%feature("docstring") gr_peak_detector_fb::set_alpha "Set the running average alpha.

Params: (alpha)"

%feature("docstring") gr_peak_detector_fb::threshold_factor_rise "Get the threshold factor value for the rise time.

Params: (NONE)"

%feature("docstring") gr_peak_detector_fb::threshold_factor_fall "Get the threshold factor value for the fall time.

Params: (NONE)"

%feature("docstring") gr_peak_detector_fb::look_ahead "Get the look-ahead factor value.

Params: (NONE)"

%feature("docstring") gr_peak_detector_fb::alpha "Get the alpha value of the running average.

Params: (NONE)"

%feature("docstring") gr_peak_detector_fb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_peak_detector_fb "Creates a peak_detector_fb block.

Detect the peak of a signal

If a peak is detected, this block outputs a 1, or it outputs 0's.

Params: (threshold_factor_rise, threshold_factor_fall, look_ahead, alpha)"

%feature("docstring") gr_peak_detector_ib "Detect the peak of a signal

If a peak is detected, this block outputs a 1, or it outputs 0's."

%feature("docstring") gr_peak_detector_ib::gr_peak_detector_ib "

Params: (threshold_factor_rise, threshold_factor_fall, look_ahead, alpha)"

%feature("docstring") gr_peak_detector_ib::set_threshold_factor_rise "Set the threshold factor value for the rise time.

Params: (thr)"

%feature("docstring") gr_peak_detector_ib::set_threshold_factor_fall "Set the threshold factor value for the fall time.

Params: (thr)"

%feature("docstring") gr_peak_detector_ib::set_look_ahead "Set the look-ahead factor.

Params: (look)"

%feature("docstring") gr_peak_detector_ib::set_alpha "Set the running average alpha.

Params: (alpha)"

%feature("docstring") gr_peak_detector_ib::threshold_factor_rise "Get the threshold factor value for the rise time.

Params: (NONE)"

%feature("docstring") gr_peak_detector_ib::threshold_factor_fall "Get the threshold factor value for the fall time.

Params: (NONE)"

%feature("docstring") gr_peak_detector_ib::look_ahead "Get the look-ahead factor value.

Params: (NONE)"

%feature("docstring") gr_peak_detector_ib::alpha "Get the alpha value of the running average.

Params: (NONE)"

%feature("docstring") gr_peak_detector_ib::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_peak_detector_ib "Creates a peak_detector_ib block.

Detect the peak of a signal

If a peak is detected, this block outputs a 1, or it outputs 0's.

Params: (threshold_factor_rise, threshold_factor_fall, look_ahead, alpha)"

%feature("docstring") gr_peak_detector_sb "Detect the peak of a signal

If a peak is detected, this block outputs a 1, or it outputs 0's."

%feature("docstring") gr_peak_detector_sb::gr_peak_detector_sb "

Params: (threshold_factor_rise, threshold_factor_fall, look_ahead, alpha)"

%feature("docstring") gr_peak_detector_sb::set_threshold_factor_rise "Set the threshold factor value for the rise time.

Params: (thr)"

%feature("docstring") gr_peak_detector_sb::set_threshold_factor_fall "Set the threshold factor value for the fall time.

Params: (thr)"

%feature("docstring") gr_peak_detector_sb::set_look_ahead "Set the look-ahead factor.

Params: (look)"

%feature("docstring") gr_peak_detector_sb::set_alpha "Set the running average alpha.

Params: (alpha)"

%feature("docstring") gr_peak_detector_sb::threshold_factor_rise "Get the threshold factor value for the rise time.

Params: (NONE)"

%feature("docstring") gr_peak_detector_sb::threshold_factor_fall "Get the threshold factor value for the fall time.

Params: (NONE)"

%feature("docstring") gr_peak_detector_sb::look_ahead "Get the look-ahead factor value.

Params: (NONE)"

%feature("docstring") gr_peak_detector_sb::alpha "Get the alpha value of the running average.

Params: (NONE)"

%feature("docstring") gr_peak_detector_sb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_peak_detector_sb "Creates a peak_detector_sb block.

Detect the peak of a signal

If a peak is detected, this block outputs a 1, or it outputs 0's.

Params: (threshold_factor_rise, threshold_factor_fall, look_ahead, alpha)"

%feature("docstring") gr_pfb_arb_resampler_ccf "Polyphase filterbank arbitrary resampler with gr_complex input, gr_complex output and float taps.

This block takes in a signal stream and performs arbitrary resampling. The resampling rate can be any real number . The resampling is done by constructing  filters where  is the interpolation rate. We then calculate  where .

Using  and , we can perform rational resampling where  is a rational number close to the input rate  where we have  filters and we cycle through them as a polyphase filterbank with a stride of  so that .

To get the arbitrary rate, we want to interpolate between two points. For each value out, we take an output from the current filter, , and the next filter  and then linearly interpolate between the two based on the real resampling rate we want.

The linear interpolation only provides us with an approximation to the real sampling rate specified. The error is a quantization error between the two filters we used as our interpolation points. To this end, the number of filters, , used determines the quantization error; the larger , the smaller the noise. You can design for a specified noise floor by setting the filter size (parameters ). The size defaults to 32 filters, which is about as good as most implementations need.

The trick with designing this filter is in how to specify the taps of the prototype filter. Like the PFB interpolator, the taps are specified using the interpolated filter rate. In this case, that rate is the input sample rate multiplied by the number of filters in the filterbank, which is also the interpolation rate. All other values should be relative to this rate.

For example, for a 32-filter arbitrary resampler and using the GNU Radio's firdes utility to build the filter, we build a low-pass filter with a sampling rate of , a 3-dB bandwidth of  and a transition bandwidth of . We can also specify the out-of-band attenuation to use, , and the filter window function (a Blackman-harris window in this case). The first input is the gain of the filter, which we specify here as the interpolation rate ().



The theory behind this block can be found in Chapter 7.5 of the following book."

%feature("docstring") gr_pfb_arb_resampler_ccf::gr_pfb_arb_resampler_ccf "Build the polyphase filterbank arbitray resampler.

Params: (rate, taps, filter_size)"

%feature("docstring") gr_pfb_arb_resampler_ccf::create_diff_taps "

Params: (newtaps, difftaps)"

%feature("docstring") gr_pfb_arb_resampler_ccf::create_taps "Resets the filterbank's filter taps with the new prototype filter

Params: (newtaps, ourtaps, ourfilter)"

%feature("docstring") gr_pfb_arb_resampler_ccf::~gr_pfb_arb_resampler_ccf "

Params: (NONE)"

%feature("docstring") gr_pfb_arb_resampler_ccf::print_taps "Print all of the filterbank taps to screen.

Params: (NONE)"

%feature("docstring") gr_pfb_arb_resampler_ccf::set_rate "

Params: (rate)"

%feature("docstring") gr_pfb_arb_resampler_ccf::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_pfb_arb_resampler_ccf "Creates a pfb_arb_resampler_ccf block.

Polyphase filterbank arbitrary resampler with gr_complex input, gr_complex output and float taps.

This block takes in a signal stream and performs arbitrary resampling. The resampling rate can be any real number . The resampling is done by constructing  filters where  is the interpolation rate. We then calculate  where .

Using  and , we can perform rational resampling where  is a rational number close to the input rate  where we have  filters and we cycle through them as a polyphase filterbank with a stride of  so that .

To get the arbitrary rate, we want to interpolate between two points. For each value out, we take an output from the current filter, , and the next filter  and then linearly interpolate between the two based on the real resampling rate we want.

The linear interpolation only provides us with an approximation to the real sampling rate specified. The error is a quantization error between the two filters we used as our interpolation points. To this end, the number of filters, , used determines the quantization error; the larger , the smaller the noise. You can design for a specified noise floor by setting the filter size (parameters ). The size defaults to 32 filters, which is about as good as most implementations need.

The trick with designing this filter is in how to specify the taps of the prototype filter. Like the PFB interpolator, the taps are specified using the interpolated filter rate. In this case, that rate is the input sample rate multiplied by the number of filters in the filterbank, which is also the interpolation rate. All other values should be relative to this rate.

For example, for a 32-filter arbitrary resampler and using the GNU Radio's firdes utility to build the filter, we build a low-pass filter with a sampling rate of , a 3-dB bandwidth of  and a transition bandwidth of . We can also specify the out-of-band attenuation to use, , and the filter window function (a Blackman-harris window in this case). The first input is the gain of the filter, which we specify here as the interpolation rate ().



The theory behind this block can be found in Chapter 7.5 of the following book.

Params: (rate, taps, filter_size)"

%feature("docstring") gr_pfb_channelizer_ccf "Polyphase filterbank channelizer with gr_complex input, gr_complex output and float taps.

This block takes in complex inputs and channelizes it to  channels of equal bandwidth. Each of the resulting channels is decimated to the new rate that is the input sampling rate  divided by the number of channels, .

The PFB channelizer code takes the taps generated above and builds a set of filters. The set contains  number of filters and each filter contains ceil(taps.size()/decim) number of taps. Each tap from the filter prototype is sequentially inserted into the next filter. When all of the input taps are used, the remaining filters in the filterbank are filled out with 0's to make sure each filter has the same number of taps.

Each filter operates using the gr_fir filter classs of GNU Radio, which takes the input stream at  and performs the inner product calculation to  where  is the number of filter taps. To efficiently handle this in the GNU Radio structure, each filter input must come from its own input stream. So the channelizer must be provided with  streams where the input stream has been deinterleaved. This is most easily done using the gr_stream_to_streams block.

The output is then produced as a vector, where index  in the vector is the next sample from the th channel. This is most easily handled by sending the output to a gr_vector_to_streams block to handle the conversion and passing  streams out.

The input and output formatting is done using a hier_block2 called pfb_channelizer_ccf. This can take in a single stream and outputs  streams based on the behavior described above.

The filter's taps should be based on the input sampling rate.

For example, using the GNU Radio's firdes utility to building filters, we build a low-pass filter with a sampling rate of , a 3-dB bandwidth of  and a transition bandwidth of . We can also specify the out-of-band attenuation to use, , and the filter window function (a Blackman-harris window in this case). The first input is the gain of the filter, which we specify here as unity.



The filter output can also be overs ampled. The over sampling rate is the ratio of the the actual output sampling rate to the normal output sampling rate. It must be rationally related to the number of channels as N/i for i in [1,N], which gives an outputsample rate of [fs/N, fs] where fs is the input sample rate and N is the number of channels.

For example, for 6 channels with fs = 6000 Hz, the normal rate is 6000/6 = 1000 Hz. Allowable oversampling rates are 6/6, 6/5, 6/4, 6/3, 6/2, and 6/1 where the output sample rate of a 6/1 oversample ratio is 6000 Hz, or 6 times the normal 1000 Hz. A rate of 6/5 = 1.2, so the output rate would be 1200 Hz.

The theory behind this block can be found in Chapter 6 of the following book."

%feature("docstring") gr_pfb_channelizer_ccf::gr_pfb_channelizer_ccf "Build the polyphase filterbank decimator.

Params: (numchans, taps, oversample_rate)"

%feature("docstring") gr_pfb_channelizer_ccf::~gr_pfb_channelizer_ccf "

Params: (NONE)"

%feature("docstring") gr_pfb_channelizer_ccf::set_taps "Resets the filterbank's filter taps with the new prototype filter

Params: (taps)"

%feature("docstring") gr_pfb_channelizer_ccf::print_taps "Print all of the filterbank taps to screen.

Params: (NONE)"

%feature("docstring") gr_pfb_channelizer_ccf::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_pfb_channelizer_ccf "Creates a pfb_channelizer_ccf block.

Polyphase filterbank channelizer with gr_complex input, gr_complex output and float taps.

This block takes in complex inputs and channelizes it to  channels of equal bandwidth. Each of the resulting channels is decimated to the new rate that is the input sampling rate  divided by the number of channels, .

The PFB channelizer code takes the taps generated above and builds a set of filters. The set contains  number of filters and each filter contains ceil(taps.size()/decim) number of taps. Each tap from the filter prototype is sequentially inserted into the next filter. When all of the input taps are used, the remaining filters in the filterbank are filled out with 0's to make sure each filter has the same number of taps.

Each filter operates using the gr_fir filter classs of GNU Radio, which takes the input stream at  and performs the inner product calculation to  where  is the number of filter taps. To efficiently handle this in the GNU Radio structure, each filter input must come from its own input stream. So the channelizer must be provided with  streams where the input stream has been deinterleaved. This is most easily done using the gr_stream_to_streams block.

The output is then produced as a vector, where index  in the vector is the next sample from the th channel. This is most easily handled by sending the output to a gr_vector_to_streams block to handle the conversion and passing  streams out.

The input and output formatting is done using a hier_block2 called pfb_channelizer_ccf. This can take in a single stream and outputs  streams based on the behavior described above.

The filter's taps should be based on the input sampling rate.

For example, using the GNU Radio's firdes utility to building filters, we build a low-pass filter with a sampling rate of , a 3-dB bandwidth of  and a transition bandwidth of . We can also specify the out-of-band attenuation to use, , and the filter window function (a Blackman-harris window in this case). The first input is the gain of the filter, which we specify here as unity.



The filter output can also be overs ampled. The over sampling rate is the ratio of the the actual output sampling rate to the normal output sampling rate. It must be rationally related to the number of channels as N/i for i in [1,N], which gives an outputsample rate of [fs/N, fs] where fs is the input sample rate and N is the number of channels.

For example, for 6 channels with fs = 6000 Hz, the normal rate is 6000/6 = 1000 Hz. Allowable oversampling rates are 6/6, 6/5, 6/4, 6/3, 6/2, and 6/1 where the output sample rate of a 6/1 oversample ratio is 6000 Hz, or 6 times the normal 1000 Hz. A rate of 6/5 = 1.2, so the output rate would be 1200 Hz.

The theory behind this block can be found in Chapter 6 of the following book.

Params: (numchans, taps, oversample_rate)"

%feature("docstring") gr_pfb_clock_sync_ccf "Timing synchronizer using polyphase filterbanks.

This block performs timing synchronization for PAM signals by minimizing the derivative of the filtered signal, which in turn maximizes the SNR and minimizes ISI.

This approach works by setting up two filterbanks; one filterbank contains the signal's pulse shaping matched filter (such as a root raised cosine filter), where each branch of the filterbank contains a different phase of the filter. The second filterbank contains the derivatives of the filters in the first filterbank. Thinking of this in the time domain, the first filterbank contains filters that have a sinc shape to them. We want to align the output signal to be sampled at exactly the peak of the sinc shape. The derivative of the sinc contains a zero at the maximum point of the sinc (sinc(0) = 1, sinc(0)' = 0). Furthermore, the region around the zero point is relatively linear. We make use of this fact to generate the error signal.

If the signal out of the derivative filters is d_i[n] for the ith filter, and the output of the matched filter is x_i[n], we calculate the error as: e[n] = (Re{x_i[n]} * Re{d_i[n]} + Im{x_i[n]} * Im{d_i[n]}) / 2.0 This equation averages the error in the real and imaginary parts. There are two reasons we multiply by the signal itself. First, if the symbol could be positive or negative going, but we want the error term to always tell us to go in the same direction depending on which side of the zero point we are on. The sign of x_i[n] adjusts the error term to do this. Second, the magnitude of x_i[n] scales the error term depending on the symbol's amplitude, so larger signals give us a stronger error term because we have more confidence in that symbol's value. Using the magnitude of x_i[n] instead of just the sign is especially good for signals with low SNR.

The error signal, e[n], gives us a value proportional to how far away from the zero point we are in the derivative signal. We want to drive this value to zero, so we set up a second order loop. We have two variables for this loop; d_k is the filter number in the filterbank we are on and d_rate is the rate which we travel through the filters in the steady state. That is, due to the natural clock differences between the transmitter and receiver, d_rate represents that difference and would traverse the filter phase paths to keep the receiver locked. Thinking of this as a second-order PLL, the d_rate is the frequency and d_k is the phase. So we update d_rate and d_k using the standard loop equations based on two error signals, d_alpha and d_beta. We have these two values set based on each other for a critically damped system, so in the block constructor, we just ask for \"gain,\" which is d_alpha while d_beta is equal to (gain^2)/4.

The clock sync block needs to know the number of samples per symbol (sps), because it only returns a single point representing the symbol. The sps can be any positive real number and does not need to be an integer. The filter taps must also be specified. The taps are generated by first conceiving of the prototype filter that would be the signal's matched filter. Then interpolate this by the number of filters in the filterbank. These are then distributed among all of the filters. So if the prototype filter was to have 45 taps in it, then each path of the filterbank will also have 45 taps. This is easily done by building the filter with the sample rate multiplied by the number of filters to use.

The number of filters can also be set and defaults to 32. With 32 filters, you get a good enough resolution in the phase to produce very small, almost unnoticeable, ISI. Going to 64 filters can reduce this more, but after that there is very little gained for the extra complexity.

The initial phase is another settable parameter and refers to the filter path the algorithm initially looks at (i.e., d_k starts at init_phase). This value defaults to zero, but it might be useful to start at a different phase offset, such as the mid- point of the filters.

The final parameter is the max_rate_devitation, which defaults to 1.5. This is how far we allow d_rate to swing, positive or negative, from 0. Constraining the rate can help keep the algorithm from walking too far away to lock during times when there is no signal."

%feature("docstring") gr_pfb_clock_sync_ccf::gr_pfb_clock_sync_ccf "Build the polyphase filterbank timing synchronizer.

Params: (sps, gain, taps, filter_size, init_phase, max_rate_deviation)"

%feature("docstring") gr_pfb_clock_sync_ccf::create_diff_taps "

Params: (newtaps, difftaps)"

%feature("docstring") gr_pfb_clock_sync_ccf::~gr_pfb_clock_sync_ccf "

Params: (NONE)"

%feature("docstring") gr_pfb_clock_sync_ccf::set_taps "Resets the filterbank's filter taps with the new prototype filter

Params: (taps, ourtaps, ourfilter)"

%feature("docstring") gr_pfb_clock_sync_ccf::channel_taps "Returns the taps of the matched filter

Params: (channel)"

%feature("docstring") gr_pfb_clock_sync_ccf::diff_channel_taps "Returns the taps in the derivative filter

Params: (channel)"

%feature("docstring") gr_pfb_clock_sync_ccf::print_taps "Print all of the filterbank taps to screen.

Params: (NONE)"

%feature("docstring") gr_pfb_clock_sync_ccf::print_diff_taps "Print all of the filterbank taps of the derivative filter to screen.

Params: (NONE)"

%feature("docstring") gr_pfb_clock_sync_ccf::set_alpha "Set the gain value alpha for the control loop

Params: (alpha)"

%feature("docstring") gr_pfb_clock_sync_ccf::set_beta "Set the gain value beta for the control loop

Params: (beta)"

%feature("docstring") gr_pfb_clock_sync_ccf::set_max_rate_deviation "Set the maximum deviation from 0 d_rate can have

Params: (m)"

%feature("docstring") gr_pfb_clock_sync_ccf::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_pfb_clock_sync_ccf::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_pfb_clock_sync_ccf "Creates a pfb_clock_sync_ccf block.

Timing synchronizer using polyphase filterbanks.

This block performs timing synchronization for PAM signals by minimizing the derivative of the filtered signal, which in turn maximizes the SNR and minimizes ISI.

This approach works by setting up two filterbanks; one filterbank contains the signal's pulse shaping matched filter (such as a root raised cosine filter), where each branch of the filterbank contains a different phase of the filter. The second filterbank contains the derivatives of the filters in the first filterbank. Thinking of this in the time domain, the first filterbank contains filters that have a sinc shape to them. We want to align the output signal to be sampled at exactly the peak of the sinc shape. The derivative of the sinc contains a zero at the maximum point of the sinc (sinc(0) = 1, sinc(0)' = 0). Furthermore, the region around the zero point is relatively linear. We make use of this fact to generate the error signal.

If the signal out of the derivative filters is d_i[n] for the ith filter, and the output of the matched filter is x_i[n], we calculate the error as: e[n] = (Re{x_i[n]} * Re{d_i[n]} + Im{x_i[n]} * Im{d_i[n]}) / 2.0 This equation averages the error in the real and imaginary parts. There are two reasons we multiply by the signal itself. First, if the symbol could be positive or negative going, but we want the error term to always tell us to go in the same direction depending on which side of the zero point we are on. The sign of x_i[n] adjusts the error term to do this. Second, the magnitude of x_i[n] scales the error term depending on the symbol's amplitude, so larger signals give us a stronger error term because we have more confidence in that symbol's value. Using the magnitude of x_i[n] instead of just the sign is especially good for signals with low SNR.

The error signal, e[n], gives us a value proportional to how far away from the zero point we are in the derivative signal. We want to drive this value to zero, so we set up a second order loop. We have two variables for this loop; d_k is the filter number in the filterbank we are on and d_rate is the rate which we travel through the filters in the steady state. That is, due to the natural clock differences between the transmitter and receiver, d_rate represents that difference and would traverse the filter phase paths to keep the receiver locked. Thinking of this as a second-order PLL, the d_rate is the frequency and d_k is the phase. So we update d_rate and d_k using the standard loop equations based on two error signals, d_alpha and d_beta. We have these two values set based on each other for a critically damped system, so in the block constructor, we just ask for \"gain,\" which is d_alpha while d_beta is equal to (gain^2)/4.

The clock sync block needs to know the number of samples per symbol (sps), because it only returns a single point representing the symbol. The sps can be any positive real number and does not need to be an integer. The filter taps must also be specified. The taps are generated by first conceiving of the prototype filter that would be the signal's matched filter. Then interpolate this by the number of filters in the filterbank. These are then distributed among all of the filters. So if the prototype filter was to have 45 taps in it, then each path of the filterbank will also have 45 taps. This is easily done by building the filter with the sample rate multiplied by the number of filters to use.

The number of filters can also be set and defaults to 32. With 32 filters, you get a good enough resolution in the phase to produce very small, almost unnoticeable, ISI. Going to 64 filters can reduce this more, but after that there is very little gained for the extra complexity.

The initial phase is another settable parameter and refers to the filter path the algorithm initially looks at (i.e., d_k starts at init_phase). This value defaults to zero, but it might be useful to start at a different phase offset, such as the mid- point of the filters.

The final parameter is the max_rate_devitation, which defaults to 1.5. This is how far we allow d_rate to swing, positive or negative, from 0. Constraining the rate can help keep the algorithm from walking too far away to lock during times when there is no signal.

Params: (sps, gain, taps, filter_size, init_phase, max_rate_deviation)"

%feature("docstring") gr_pfb_clock_sync_fff "Timing synchronizer using polyphase filterbanks.

This block performs timing synchronization for PAM signals by minimizing the derivative of the filtered signal, which in turn maximizes the SNR and minimizes ISI.

This approach works by setting up two filterbanks; one filterbanke contains the signal's pulse shaping matched filter (such as a root raised cosine filter), where each branch of the filterbank contains a different phase of the filter. The second filterbank contains the derivatives of the filters in the first filterbank. Thinking of this in the time domain, the first filterbank contains filters that have a sinc shape to them. We want to align the output signal to be sampled at exactly the peak of the sinc shape. The derivative of the sinc contains a zero at the maximum point of the sinc (sinc(0) = 1, sinc(0)' = 0). Furthermore, the region around the zero point is relatively linear. We make use of this fact to generate the error signal.

If the signal out of the derivative filters is d_i[n] for the ith filter, and the output of the matched filter is x_i[n], we calculate the error as: e[n] = (Re{x_i[n]} * Re{d_i[n]} + Im{x_i[n]} * Im{d_i[n]}) / 2.0 This equation averages the error in the real and imaginary parts. There are two reasons we multiply by the signal itself. First, if the symbol could be positive or negative going, but we want the error term to always tell us to go in the same direction depending on which side of the zero point we are on. The sign of x_i[n] adjusts the error term to do this. Second, the magnitude of x_i[n] scales the error term depending on the symbol's amplitude, so larger signals give us a stronger error term because we have more confidence in that symbol's value. Using the magnitude of x_i[n] instead of just the sign is especially good for signals with low SNR.

The error signal, e[n], gives us a value proportional to how far away from the zero point we are in the derivative signal. We want to drive this value to zero, so we set up a second order loop. We have two variables for this loop; d_k is the filter number in the filterbank we are on and d_rate is the rate which we travel through the filters in the steady state. That is, due to the natural clock differences between the transmitter and receiver, d_rate represents that difference and would traverse the filter phase paths to keep the receiver locked. Thinking of this as a second-order PLL, the d_rate is the frequency and d_k is the phase. So we update d_rate and d_k using the standard loop equations based on two error signals, d_alpha and d_beta. We have these two values set based on each other for a critically damped system, so in the block constructor, we just ask for \"gain,\" which is d_alpha while d_beta is equal to (gain^2)/4.

The clock sync block needs to know the number of samples per second (sps), because it only returns a single point representing the sample. The sps can be any positive real number and does not need to be an integer. The filter taps must also be specified. The taps are generated by first conceiving of the prototype filter that would be the signal's matched filter. Then interpolate this by the number of filters in the filterbank. These are then distributed among all of the filters. So if the prototype filter was to have 45 taps in it, then each path of the filterbank will also have 45 taps. This is easily done by building the filter with the sample rate multiplied by the number of filters to use.

The number of filters can also be set and defaults to 32. With 32 filters, you get a good enough resolution in the phase to produce very small, almost unnoticeable, ISI. Going to 64 filters can reduce this more, but after that there is very little gained for the extra complexity.

The initial phase is another settable parameter and refers to the filter path the algorithm initially looks at (i.e., d_k starts at init_phase). This value defaults to zero, but it might be useful to start at a different phase offset, such as the mid- point of the filters.

The final parameter is the max_rate_devitation, which defaults to 1.5. This is how far we allow d_rate to swing, positive or negative, from 0. Constraining the rate can help keep the algorithm from walking too far away to lock during times when there is no signal."

%feature("docstring") gr_pfb_clock_sync_fff::gr_pfb_clock_sync_fff "Build the polyphase filterbank timing synchronizer.

Params: (sps, gain, taps, filter_size, init_phase, max_rate_deviation)"

%feature("docstring") gr_pfb_clock_sync_fff::create_diff_taps "

Params: (newtaps, difftaps)"

%feature("docstring") gr_pfb_clock_sync_fff::~gr_pfb_clock_sync_fff "

Params: (NONE)"

%feature("docstring") gr_pfb_clock_sync_fff::set_taps "Resets the filterbank's filter taps with the new prototype filter

Params: (taps, ourtaps, ourfilter)"

%feature("docstring") gr_pfb_clock_sync_fff::channel_taps "Returns the taps of the matched filter

Params: (channel)"

%feature("docstring") gr_pfb_clock_sync_fff::diff_channel_taps "Returns the taps in the derivative filter

Params: (channel)"

%feature("docstring") gr_pfb_clock_sync_fff::print_taps "Print all of the filterbank taps to screen.

Params: (NONE)"

%feature("docstring") gr_pfb_clock_sync_fff::print_diff_taps "Print all of the filterbank taps of the derivative filter to screen.

Params: (NONE)"

%feature("docstring") gr_pfb_clock_sync_fff::set_alpha "Set the gain value alpha for the control loop

Params: (alpha)"

%feature("docstring") gr_pfb_clock_sync_fff::set_beta "Set the gain value beta for the control loop

Params: (beta)"

%feature("docstring") gr_pfb_clock_sync_fff::set_max_rate_deviation "Set the maximum deviation from 0 d_rate can have

Params: (m)"

%feature("docstring") gr_pfb_clock_sync_fff::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_pfb_clock_sync_fff::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_pfb_clock_sync_fff "Creates a pfb_clock_sync_fff block.

Timing synchronizer using polyphase filterbanks.

This block performs timing synchronization for PAM signals by minimizing the derivative of the filtered signal, which in turn maximizes the SNR and minimizes ISI.

This approach works by setting up two filterbanks; one filterbanke contains the signal's pulse shaping matched filter (such as a root raised cosine filter), where each branch of the filterbank contains a different phase of the filter. The second filterbank contains the derivatives of the filters in the first filterbank. Thinking of this in the time domain, the first filterbank contains filters that have a sinc shape to them. We want to align the output signal to be sampled at exactly the peak of the sinc shape. The derivative of the sinc contains a zero at the maximum point of the sinc (sinc(0) = 1, sinc(0)' = 0). Furthermore, the region around the zero point is relatively linear. We make use of this fact to generate the error signal.

If the signal out of the derivative filters is d_i[n] for the ith filter, and the output of the matched filter is x_i[n], we calculate the error as: e[n] = (Re{x_i[n]} * Re{d_i[n]} + Im{x_i[n]} * Im{d_i[n]}) / 2.0 This equation averages the error in the real and imaginary parts. There are two reasons we multiply by the signal itself. First, if the symbol could be positive or negative going, but we want the error term to always tell us to go in the same direction depending on which side of the zero point we are on. The sign of x_i[n] adjusts the error term to do this. Second, the magnitude of x_i[n] scales the error term depending on the symbol's amplitude, so larger signals give us a stronger error term because we have more confidence in that symbol's value. Using the magnitude of x_i[n] instead of just the sign is especially good for signals with low SNR.

The error signal, e[n], gives us a value proportional to how far away from the zero point we are in the derivative signal. We want to drive this value to zero, so we set up a second order loop. We have two variables for this loop; d_k is the filter number in the filterbank we are on and d_rate is the rate which we travel through the filters in the steady state. That is, due to the natural clock differences between the transmitter and receiver, d_rate represents that difference and would traverse the filter phase paths to keep the receiver locked. Thinking of this as a second-order PLL, the d_rate is the frequency and d_k is the phase. So we update d_rate and d_k using the standard loop equations based on two error signals, d_alpha and d_beta. We have these two values set based on each other for a critically damped system, so in the block constructor, we just ask for \"gain,\" which is d_alpha while d_beta is equal to (gain^2)/4.

The clock sync block needs to know the number of samples per second (sps), because it only returns a single point representing the sample. The sps can be any positive real number and does not need to be an integer. The filter taps must also be specified. The taps are generated by first conceiving of the prototype filter that would be the signal's matched filter. Then interpolate this by the number of filters in the filterbank. These are then distributed among all of the filters. So if the prototype filter was to have 45 taps in it, then each path of the filterbank will also have 45 taps. This is easily done by building the filter with the sample rate multiplied by the number of filters to use.

The number of filters can also be set and defaults to 32. With 32 filters, you get a good enough resolution in the phase to produce very small, almost unnoticeable, ISI. Going to 64 filters can reduce this more, but after that there is very little gained for the extra complexity.

The initial phase is another settable parameter and refers to the filter path the algorithm initially looks at (i.e., d_k starts at init_phase). This value defaults to zero, but it might be useful to start at a different phase offset, such as the mid- point of the filters.

The final parameter is the max_rate_devitation, which defaults to 1.5. This is how far we allow d_rate to swing, positive or negative, from 0. Constraining the rate can help keep the algorithm from walking too far away to lock during times when there is no signal.

Params: (sps, gain, taps, filter_size, init_phase, max_rate_deviation)"

%feature("docstring") gr_pfb_decimator_ccf "Polyphase filterbank bandpass decimator with gr_complex input, gr_complex output and float taps.

This block takes in a signal stream and performs interger down- sampling (decimation) with a polyphase filterbank. The first input is the integer specifying how much to decimate by. The second input is a vector (Python list) of floating-point taps of the prototype filter. The third input specifies the channel to extract. By default, the zeroth channel is used, which is the baseband channel (first Nyquist zone).

The  parameter specifies which channel to use since this class is capable of bandpass decimation. Given a complex input stream at a sampling rate of  and a decimation rate of , the input frequency domain is split into  channels that represent the Nyquist zones. Using the polyphase filterbank, we can select any one of these channels to decimate.

The output signal will be the basebanded and decimated signal from that channel. This concept is very similar to the PFB channelizer (see gr_pfb_channelizer_ccf) where only a single channel is extracted at a time.

The filter's taps should be based on the sampling rate before decimation.

For example, using the GNU Radio's firdes utility to building filters, we build a low-pass filter with a sampling rate of , a 3-dB bandwidth of  and a transition bandwidth of . We can also specify the out-of-band attenuation to use, , and the filter window function (a Blackman-harris window in this case). The first input is the gain of the filter, which we specify here as unity.



The PFB decimator code takes the taps generated above and builds a set of filters. The set contains  number of filters and each filter contains ceil(taps.size()/decim) number of taps. Each tap from the filter prototype is sequentially inserted into the next filter. When all of the input taps are used, the remaining filters in the filterbank are filled out with 0's to make sure each filter has the same number of taps.

The theory behind this block can be found in Chapter 6 of the following book."

%feature("docstring") gr_pfb_decimator_ccf::gr_pfb_decimator_ccf "Build the polyphase filterbank decimator.

Params: (decim, taps, channel)"

%feature("docstring") gr_pfb_decimator_ccf::~gr_pfb_decimator_ccf "

Params: (NONE)"

%feature("docstring") gr_pfb_decimator_ccf::set_taps "Resets the filterbank's filter taps with the new prototype filter

Params: (taps)"

%feature("docstring") gr_pfb_decimator_ccf::print_taps "Print all of the filterbank taps to screen.

Params: (NONE)"

%feature("docstring") gr_pfb_decimator_ccf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_pfb_decimator_ccf "Creates a pfb_decimator_ccf block.

Polyphase filterbank bandpass decimator with gr_complex input, gr_complex output and float taps.

This block takes in a signal stream and performs interger down- sampling (decimation) with a polyphase filterbank. The first input is the integer specifying how much to decimate by. The second input is a vector (Python list) of floating-point taps of the prototype filter. The third input specifies the channel to extract. By default, the zeroth channel is used, which is the baseband channel (first Nyquist zone).

The  parameter specifies which channel to use since this class is capable of bandpass decimation. Given a complex input stream at a sampling rate of  and a decimation rate of , the input frequency domain is split into  channels that represent the Nyquist zones. Using the polyphase filterbank, we can select any one of these channels to decimate.

The output signal will be the basebanded and decimated signal from that channel. This concept is very similar to the PFB channelizer (see gr_pfb_channelizer_ccf) where only a single channel is extracted at a time.

The filter's taps should be based on the sampling rate before decimation.

For example, using the GNU Radio's firdes utility to building filters, we build a low-pass filter with a sampling rate of , a 3-dB bandwidth of  and a transition bandwidth of . We can also specify the out-of-band attenuation to use, , and the filter window function (a Blackman-harris window in this case). The first input is the gain of the filter, which we specify here as unity.



The PFB decimator code takes the taps generated above and builds a set of filters. The set contains  number of filters and each filter contains ceil(taps.size()/decim) number of taps. Each tap from the filter prototype is sequentially inserted into the next filter. When all of the input taps are used, the remaining filters in the filterbank are filled out with 0's to make sure each filter has the same number of taps.

The theory behind this block can be found in Chapter 6 of the following book.

Params: (decim, taps, channel)"

%feature("docstring") gr_pfb_interpolator_ccf "Polyphase filterbank interpolator with gr_complex input, gr_complex output and float taps.

This block takes in a signal stream and performs interger up- sampling (interpolation) with a polyphase filterbank. The first input is the integer specifying how much to interpolate by. The second input is a vector (Python list) of floating-point taps of the prototype filter.

The filter's taps should be based on the interpolation rate specified. That is, the bandwidth specified is relative to the bandwidth after interpolation.

For example, using the GNU Radio's firdes utility to building filters, we build a low-pass filter with a sampling rate of , a 3-dB bandwidth of  and a transition bandwidth of . We can also specify the out-of-band attenuation to use, ATT, and the filter window function (a Blackman-harris window in this case). The first input is the gain, which is also specified as the interpolation rate so that the output levels are the same as the input (this creates an overall increase in power).



The PFB interpolator code takes the taps generated above and builds a set of filters. The set contains  number of filters and each filter contains ceil(taps.size()/interp) number of taps. Each tap from the filter prototype is sequentially inserted into the next filter. When all of the input taps are used, the remaining filters in the filterbank are filled out with 0's to make sure each filter has the same number of taps.

The theory behind this block can be found in Chapter 7.1 of the following book."

%feature("docstring") gr_pfb_interpolator_ccf::gr_pfb_interpolator_ccf "Construct a Polyphase filterbank interpolator

Params: (interp, taps)"

%feature("docstring") gr_pfb_interpolator_ccf::~gr_pfb_interpolator_ccf "

Params: (NONE)"

%feature("docstring") gr_pfb_interpolator_ccf::set_taps "Resets the filterbank's filter taps with the new prototype filter

Params: (taps)"

%feature("docstring") gr_pfb_interpolator_ccf::print_taps "Print all of the filterbank taps to screen.

Params: (NONE)"

%feature("docstring") gr_pfb_interpolator_ccf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_pfb_interpolator_ccf "Creates a pfb_interpolator_ccf block.

Polyphase filterbank interpolator with gr_complex input, gr_complex output and float taps.

This block takes in a signal stream and performs interger up- sampling (interpolation) with a polyphase filterbank. The first input is the integer specifying how much to interpolate by. The second input is a vector (Python list) of floating-point taps of the prototype filter.

The filter's taps should be based on the interpolation rate specified. That is, the bandwidth specified is relative to the bandwidth after interpolation.

For example, using the GNU Radio's firdes utility to building filters, we build a low-pass filter with a sampling rate of , a 3-dB bandwidth of  and a transition bandwidth of . We can also specify the out-of-band attenuation to use, ATT, and the filter window function (a Blackman-harris window in this case). The first input is the gain, which is also specified as the interpolation rate so that the output levels are the same as the input (this creates an overall increase in power).



The PFB interpolator code takes the taps generated above and builds a set of filters. The set contains  number of filters and each filter contains ceil(taps.size()/interp) number of taps. Each tap from the filter prototype is sequentially inserted into the next filter. When all of the input taps are used, the remaining filters in the filterbank are filled out with 0's to make sure each filter has the same number of taps.

The theory behind this block can be found in Chapter 7.1 of the following book.

Params: (interp, taps)"

%feature("docstring") gr_pfb_synthesis_filterbank_ccf "Polyphase synthesis filterbank with gr_complex input, gr_complex output and float taps."

%feature("docstring") gr_pfb_synthesis_filterbank_ccf::gr_pfb_synthesis_filterbank_ccf "Build the polyphase synthesis filterbank.

Params: (numchans, taps)"

%feature("docstring") gr_pfb_synthesis_filterbank_ccf::~gr_pfb_synthesis_filterbank_ccf "

Params: (NONE)"

%feature("docstring") gr_pfb_synthesis_filterbank_ccf::set_taps "Resets the filterbank's filter taps with the new prototype filter

Params: (taps)"

%feature("docstring") gr_pfb_synthesis_filterbank_ccf::print_taps "Print all of the filterbank taps to screen.

Params: (NONE)"

%feature("docstring") gr_pfb_synthesis_filterbank_ccf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_pfb_synthesis_filterbank_ccf "Creates a pfb_synthesis_filterbank_ccf block.

Polyphase synthesis filterbank with gr_complex input, gr_complex output and float taps.

Params: (numchans, taps)"

%feature("docstring") gr_phase_modulator_fc "Phase modulator block

output=complex(cos(in*sensitivity),sin(in*sensitivity))."

%feature("docstring") gr_phase_modulator_fc::gr_phase_modulator_fc "

Params: (sensitivity)"

%feature("docstring") gr_phase_modulator_fc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_phase_modulator_fc "Creates a phase_modulator_fc block.

Phase modulator block

output=complex(cos(in*sensitivity),sin(in*sensitivity)).

Params: (sensitivity)"

%feature("docstring") gr_pll_carriertracking_cc "Implements a PLL which locks to the input frequency and outputs the input signal mixed with that carrier.

input: stream of complex; output: stream of complex.

This PLL locks onto a [possibly noisy] reference carrier on the input and outputs that signal, downconverted to DC

All settings max_freq and min_freq are in terms of radians per sample, NOT HERTZ. Alpha is the phase gain (first order, units of radians per radian) and beta is the frequency gain (second order, units of radians per sample per radian)"

%feature("docstring") gr_pll_carriertracking_cc::gr_pll_carriertracking_cc "

Params: (alpha, beta, max_freq, min_freq)"

%feature("docstring") gr_pll_carriertracking_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_pll_carriertracking_cc::mod_2pi "

Params: (in)"

%feature("docstring") gr_pll_carriertracking_cc::phase_detector "

Params: (sample, ref_phase)"

%feature("docstring") gr_pll_carriertracking_cc::lock_detector "

Params: ()"

%feature("docstring") gr_pll_carriertracking_cc::squelch_enable "

Params: ()"

%feature("docstring") gr_pll_carriertracking_cc::set_lock_threshold "

Params: ()"

%feature("docstring") gr_make_pll_carriertracking_cc "Creates a pll_carriertracking_cc block.

Implements a PLL which locks to the input frequency and outputs the input signal mixed with that carrier.

input: stream of complex; output: stream of complex.

This PLL locks onto a [possibly noisy] reference carrier on the input and outputs that signal, downconverted to DC

All settings max_freq and min_freq are in terms of radians per sample, NOT HERTZ. Alpha is the phase gain (first order, units of radians per radian) and beta is the frequency gain (second order, units of radians per sample per radian)

Params: (alpha, beta, max_freq, min_freq)"

%feature("docstring") gr_pll_freqdet_cf "Implements a PLL which locks to the input frequency and outputs an estimate of that frequency. Useful for FM Demod.

input: stream of complex; output: stream of floats.

This PLL locks onto a [possibly noisy] reference carrier on the input and outputs an estimate of that frequency in radians per sample. All settings max_freq and min_freq are in terms of radians per sample, NOT HERTZ. Alpha is the phase gain (first order, units of radians per radian) and beta is the frequency gain (second order, units of radians per sample per radian)"

%feature("docstring") gr_pll_freqdet_cf::gr_pll_freqdet_cf "

Params: (alpha, beta, max_freq, min_freq)"

%feature("docstring") gr_pll_freqdet_cf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_pll_freqdet_cf::mod_2pi "

Params: (in)"

%feature("docstring") gr_pll_freqdet_cf::phase_detector "

Params: (sample, ref_phase)"

%feature("docstring") gr_make_pll_freqdet_cf "Creates a pll_freqdet_cf block.

Implements a PLL which locks to the input frequency and outputs an estimate of that frequency. Useful for FM Demod.

input: stream of complex; output: stream of floats.

This PLL locks onto a [possibly noisy] reference carrier on the input and outputs an estimate of that frequency in radians per sample. All settings max_freq and min_freq are in terms of radians per sample, NOT HERTZ. Alpha is the phase gain (first order, units of radians per radian) and beta is the frequency gain (second order, units of radians per sample per radian)

Params: (alpha, beta, max_freq, min_freq)"

%feature("docstring") gr_pll_refout_cc "Implements a PLL which locks to the input frequency and outputs a carrier

input: stream of complex; output: stream of complex.

This PLL locks onto a [possibly noisy] reference carrier on the input and outputs a clean version which is phase and frequency aligned to it.

All settings max_freq and min_freq are in terms of radians per sample, NOT HERTZ. Alpha is the phase gain (first order, units of radians per radian) and beta is the frequency gain (second order, units of radians per sample per radian)"

%feature("docstring") gr_pll_refout_cc::gr_pll_refout_cc "

Params: (alpha, beta, max_freq, min_freq)"

%feature("docstring") gr_pll_refout_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_pll_refout_cc::mod_2pi "

Params: (in)"

%feature("docstring") gr_pll_refout_cc::phase_detector "

Params: (sample, ref_phase)"

%feature("docstring") gr_make_pll_refout_cc "Creates a pll_refout_cc block.

Implements a PLL which locks to the input frequency and outputs a carrier

input: stream of complex; output: stream of complex.

This PLL locks onto a [possibly noisy] reference carrier on the input and outputs a clean version which is phase and frequency aligned to it.

All settings max_freq and min_freq are in terms of radians per sample, NOT HERTZ. Alpha is the phase gain (first order, units of radians per radian) and beta is the frequency gain (second order, units of radians per sample per radian)

Params: (alpha, beta, max_freq, min_freq)"

%feature("docstring") gr_pn_correlator_cc "PN code sequential search correlator.

Receives complex baseband signal, outputs complex correlation against reference PN code, one sample per PN code period"

%feature("docstring") gr_pn_correlator_cc::gr_pn_correlator_cc "

Params: (degree, mask, seed)"

%feature("docstring") gr_pn_correlator_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_pn_correlator_cc::~gr_pn_correlator_cc "

Params: (NONE)"

%feature("docstring") gr_make_pn_correlator_cc "Creates a pn_correlator_cc block.

PN code sequential search correlator.

Receives complex baseband signal, outputs complex correlation against reference PN code, one sample per PN code period

Params: (degree, mask, seed)"

%feature("docstring") gr_probe_avg_mag_sqrd_c "compute avg magnitude squared.

input: gr_complex

Compute a running average of the magnitude squared of the the input. The level and indication as to whether the level exceeds threshold can be retrieved with the level and unmuted accessors."

%feature("docstring") gr_probe_avg_mag_sqrd_c::gr_probe_avg_mag_sqrd_c "

Params: (threshold_db, alpha)"

%feature("docstring") gr_probe_avg_mag_sqrd_c::~gr_probe_avg_mag_sqrd_c "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_c::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_probe_avg_mag_sqrd_c::unmuted "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_c::level "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_c::threshold "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_c::set_alpha "

Params: (alpha)"

%feature("docstring") gr_probe_avg_mag_sqrd_c::set_threshold "

Params: (decibels)"

%feature("docstring") gr_make_probe_avg_mag_sqrd_c "Creates a probe_avg_mag_sqrd_c block.

compute avg magnitude squared.

input: gr_complex

Compute a running average of the magnitude squared of the the input. The level and indication as to whether the level exceeds threshold can be retrieved with the level and unmuted accessors.

Params: (threshold_db, alpha)"

%feature("docstring") gr_probe_avg_mag_sqrd_cf "compute avg magnitude squared.

input: gr_complex output: gr_float

Compute a running average of the magnitude squared of the the input. The level and indication as to whether the level exceeds threshold can be retrieved with the level and unmuted accessors."

%feature("docstring") gr_probe_avg_mag_sqrd_cf::gr_probe_avg_mag_sqrd_cf "

Params: (threshold_db, alpha)"

%feature("docstring") gr_probe_avg_mag_sqrd_cf::~gr_probe_avg_mag_sqrd_cf "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_cf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_probe_avg_mag_sqrd_cf::unmuted "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_cf::level "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_cf::threshold "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_cf::set_alpha "

Params: (alpha)"

%feature("docstring") gr_probe_avg_mag_sqrd_cf::set_threshold "

Params: (decibels)"

%feature("docstring") gr_make_probe_avg_mag_sqrd_cf "Creates a probe_avg_mag_sqrd_cf block.

compute avg magnitude squared.

input: gr_complex output: gr_float

Compute a running average of the magnitude squared of the the input. The level and indication as to whether the level exceeds threshold can be retrieved with the level and unmuted accessors.

Params: (threshold_db, alpha)"

%feature("docstring") gr_probe_avg_mag_sqrd_f "compute avg magnitude squared.

input: float

Compute a running average of the magnitude squared of the the input. The level and indication as to whether the level exceeds threshold can be retrieved with the level and unmuted accessors."

%feature("docstring") gr_probe_avg_mag_sqrd_f::gr_probe_avg_mag_sqrd_f "

Params: (threshold_db, alpha)"

%feature("docstring") gr_probe_avg_mag_sqrd_f::~gr_probe_avg_mag_sqrd_f "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_f::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_probe_avg_mag_sqrd_f::unmuted "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_f::level "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_f::threshold "

Params: (NONE)"

%feature("docstring") gr_probe_avg_mag_sqrd_f::set_alpha "

Params: (alpha)"

%feature("docstring") gr_probe_avg_mag_sqrd_f::set_threshold "

Params: (decibels)"

%feature("docstring") gr_make_probe_avg_mag_sqrd_f "Creates a probe_avg_mag_sqrd_f block.

compute avg magnitude squared.

input: float

Compute a running average of the magnitude squared of the the input. The level and indication as to whether the level exceeds threshold can be retrieved with the level and unmuted accessors.

Params: (threshold_db, alpha)"

%feature("docstring") gr_probe_density_b "This block maintains a running average of the input stream and makes it available as an accessor function. The input stream is type unsigned char.

If you send this block a stream of unpacked bytes, it will tell you what the bit density is."

%feature("docstring") gr_probe_density_b::gr_probe_density_b "

Params: (alpha)"

%feature("docstring") gr_probe_density_b::~gr_probe_density_b "

Params: (NONE)"

%feature("docstring") gr_probe_density_b::density "Returns the current density value.

Params: (NONE)"

%feature("docstring") gr_probe_density_b::set_alpha "Set the average filter constant.

Params: (alpha)"

%feature("docstring") gr_probe_density_b::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_probe_density_b "Creates a probe_density_b block.

This block maintains a running average of the input stream and makes it available as an accessor function. The input stream is type unsigned char.

If you send this block a stream of unpacked bytes, it will tell you what the bit density is.

Params: (alpha)"

%feature("docstring") gr_probe_mpsk_snr_c "Compute the estimate SNR of an MPSK signal using the Squared Signal to Noise Variance (SNV) technique.

This technique assumes an AWGN channel.


Compute the running average of the signal mean and noise variance. The estimated signal mean, noise variance, and SNR are available via accessors.

This SNR estimator is inaccurate below about 7dB SNR."

%feature("docstring") gr_probe_mpsk_snr_c::gr_probe_mpsk_snr_c "

Params: (alpha)"

%feature("docstring") gr_probe_mpsk_snr_c::~gr_probe_mpsk_snr_c "

Params: (NONE)"

%feature("docstring") gr_probe_mpsk_snr_c::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_probe_mpsk_snr_c::signal_mean "

Params: (NONE)"

%feature("docstring") gr_probe_mpsk_snr_c::noise_variance "

Params: (NONE)"

%feature("docstring") gr_probe_mpsk_snr_c::snr "

Params: (NONE)"

%feature("docstring") gr_probe_mpsk_snr_c::set_alpha "

Params: (alpha)"

%feature("docstring") gr_make_probe_mpsk_snr_c "Creates a probe_mpsk_snr_c block.

Compute the estimate SNR of an MPSK signal using the Squared Signal to Noise Variance (SNV) technique.

This technique assumes an AWGN channel.


Compute the running average of the signal mean and noise variance. The estimated signal mean, noise variance, and SNR are available via accessors.

This SNR estimator is inaccurate below about 7dB SNR.

Params: (alpha)"

%feature("docstring") gr_probe_signal_f "Sink that allows a sample to be grabbed from Python."

%feature("docstring") gr_probe_signal_f::gr_probe_signal_f "

Params: (NONE)"

%feature("docstring") gr_probe_signal_f::~gr_probe_signal_f "

Params: (NONE)"

%feature("docstring") gr_probe_signal_f::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_probe_signal_f::level "

Params: (NONE)"

%feature("docstring") gr_make_probe_signal_f "Creates a probe_signal_f block.

Sink that allows a sample to be grabbed from Python.

Params: (NONE)"

%feature("docstring") gr_pwr_squelch_cc "gate or zero output when input power below threshold"

%feature("docstring") gr_pwr_squelch_cc::gr_pwr_squelch_cc "

Params: (db, alpha, ramp, gate)"

%feature("docstring") gr_pwr_squelch_cc::update_state "

Params: (in)"

%feature("docstring") gr_pwr_squelch_cc::mute "

Params: (NONE)"

%feature("docstring") gr_pwr_squelch_cc::squelch_range "

Params: (NONE)"

%feature("docstring") gr_pwr_squelch_cc::threshold "

Params: (NONE)"

%feature("docstring") gr_pwr_squelch_cc::set_threshold "

Params: (db)"

%feature("docstring") gr_pwr_squelch_cc::set_alpha "

Params: (alpha)"

%feature("docstring") gr_make_pwr_squelch_cc "Creates a pwr_squelch_cc block.

gate or zero output when input power below threshold

Params: (db, alpha, ramp, gate)"

%feature("docstring") gr_pwr_squelch_ff "gate or zero output when input power below threshold"

%feature("docstring") gr_pwr_squelch_ff::gr_pwr_squelch_ff "

Params: (db, alpha, ramp, gate)"

%feature("docstring") gr_pwr_squelch_ff::update_state "

Params: (in)"

%feature("docstring") gr_pwr_squelch_ff::mute "

Params: (NONE)"

%feature("docstring") gr_pwr_squelch_ff::squelch_range "

Params: (NONE)"

%feature("docstring") gr_pwr_squelch_ff::threshold "

Params: (NONE)"

%feature("docstring") gr_pwr_squelch_ff::set_threshold "

Params: (db)"

%feature("docstring") gr_pwr_squelch_ff::set_alpha "

Params: (alpha)"

%feature("docstring") gr_make_pwr_squelch_ff "Creates a pwr_squelch_ff block.

gate or zero output when input power below threshold

Params: (db, alpha, ramp, gate)"

%feature("docstring") gr_quadrature_demod_cf "quadrature demodulator: complex in, float out

This can be used to demod FM, FSK, GMSK, etc. The input is complex baseband."

%feature("docstring") gr_quadrature_demod_cf::gr_quadrature_demod_cf "

Params: (gain)"

%feature("docstring") gr_quadrature_demod_cf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_quadrature_demod_cf "Creates a quadrature_demod_cf block.

quadrature demodulator: complex in, float out

This can be used to demod FM, FSK, GMSK, etc. The input is complex baseband.

Params: (gain)"



%feature("docstring") gr_rail_ff::gr_rail_ff "

Params: (lo, hi)"

%feature("docstring") gr_rail_ff::lo "

Params: (NONE)"

%feature("docstring") gr_rail_ff::set_lo "

Params: (lo)"

%feature("docstring") gr_rail_ff::hi "

Params: (NONE)"

%feature("docstring") gr_rail_ff::set_hi "

Params: (hi)"

%feature("docstring") gr_rail_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_rail_ff "Creates a rail_ff block.



Params: (lo, hi)"

%feature("docstring") gr_rational_resampler_base_ccc "Rational Resampling Polyphase FIR filter with gr_complex input, gr_complex output and gr_complex taps."

%feature("docstring") gr_rational_resampler_base_ccc::gr_rational_resampler_base_ccc "Construct a FIR filter with the given taps

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_rational_resampler_base_ccc::install_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_ccc::~gr_rational_resampler_base_ccc "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_ccc::history "Assume block computes y_i = f(x_i, x_i-1, x_i-2, x_i-3...) History is the number of x_i's that are examined to produce one y_i. This comes in handy for FIR filters, where we use history to ensure that our input contains the appropriate \"history\" for the filter. History should be equal to the number of filter taps.

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_ccc::set_history "

Params: (history)"

%feature("docstring") gr_rational_resampler_base_ccc::interpolation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_ccc::decimation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_ccc::set_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_ccc::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_rational_resampler_base_ccc::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_rational_resampler_base_ccc "Creates a rational_resampler_base_ccc block.

Rational Resampling Polyphase FIR filter with gr_complex input, gr_complex output and gr_complex taps.

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_rational_resampler_base_ccf "Rational Resampling Polyphase FIR filter with gr_complex input, gr_complex output and float taps."

%feature("docstring") gr_rational_resampler_base_ccf::gr_rational_resampler_base_ccf "Construct a FIR filter with the given taps

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_rational_resampler_base_ccf::install_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_ccf::~gr_rational_resampler_base_ccf "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_ccf::history "Assume block computes y_i = f(x_i, x_i-1, x_i-2, x_i-3...) History is the number of x_i's that are examined to produce one y_i. This comes in handy for FIR filters, where we use history to ensure that our input contains the appropriate \"history\" for the filter. History should be equal to the number of filter taps.

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_ccf::set_history "

Params: (history)"

%feature("docstring") gr_rational_resampler_base_ccf::interpolation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_ccf::decimation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_ccf::set_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_ccf::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_rational_resampler_base_ccf::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_rational_resampler_base_ccf "Creates a rational_resampler_base_ccf block.

Rational Resampling Polyphase FIR filter with gr_complex input, gr_complex output and float taps.

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_rational_resampler_base_fcc "Rational Resampling Polyphase FIR filter with float input, gr_complex output and gr_complex taps."

%feature("docstring") gr_rational_resampler_base_fcc::gr_rational_resampler_base_fcc "Construct a FIR filter with the given taps

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_rational_resampler_base_fcc::install_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_fcc::~gr_rational_resampler_base_fcc "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fcc::history "Assume block computes y_i = f(x_i, x_i-1, x_i-2, x_i-3...) History is the number of x_i's that are examined to produce one y_i. This comes in handy for FIR filters, where we use history to ensure that our input contains the appropriate \"history\" for the filter. History should be equal to the number of filter taps.

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fcc::set_history "

Params: (history)"

%feature("docstring") gr_rational_resampler_base_fcc::interpolation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fcc::decimation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fcc::set_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_fcc::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_rational_resampler_base_fcc::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_rational_resampler_base_fcc "Creates a rational_resampler_base_fcc block.

Rational Resampling Polyphase FIR filter with float input, gr_complex output and gr_complex taps.

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_rational_resampler_base_fff "Rational Resampling Polyphase FIR filter with float input, float output and float taps."

%feature("docstring") gr_rational_resampler_base_fff::gr_rational_resampler_base_fff "Construct a FIR filter with the given taps

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_rational_resampler_base_fff::install_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_fff::~gr_rational_resampler_base_fff "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fff::history "Assume block computes y_i = f(x_i, x_i-1, x_i-2, x_i-3...) History is the number of x_i's that are examined to produce one y_i. This comes in handy for FIR filters, where we use history to ensure that our input contains the appropriate \"history\" for the filter. History should be equal to the number of filter taps.

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fff::set_history "

Params: (history)"

%feature("docstring") gr_rational_resampler_base_fff::interpolation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fff::decimation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fff::set_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_fff::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_rational_resampler_base_fff::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_rational_resampler_base_fff "Creates a rational_resampler_base_fff block.

Rational Resampling Polyphase FIR filter with float input, float output and float taps.

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_rational_resampler_base_fsf "Rational Resampling Polyphase FIR filter with float input, short output and float taps."

%feature("docstring") gr_rational_resampler_base_fsf::gr_rational_resampler_base_fsf "Construct a FIR filter with the given taps

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_rational_resampler_base_fsf::install_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_fsf::~gr_rational_resampler_base_fsf "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fsf::history "Assume block computes y_i = f(x_i, x_i-1, x_i-2, x_i-3...) History is the number of x_i's that are examined to produce one y_i. This comes in handy for FIR filters, where we use history to ensure that our input contains the appropriate \"history\" for the filter. History should be equal to the number of filter taps.

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fsf::set_history "

Params: (history)"

%feature("docstring") gr_rational_resampler_base_fsf::interpolation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fsf::decimation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_fsf::set_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_fsf::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_rational_resampler_base_fsf::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_rational_resampler_base_fsf "Creates a rational_resampler_base_fsf block.

Rational Resampling Polyphase FIR filter with float input, short output and float taps.

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_rational_resampler_base_scc "Rational Resampling Polyphase FIR filter with short input, gr_complex output and gr_complex taps."

%feature("docstring") gr_rational_resampler_base_scc::gr_rational_resampler_base_scc "Construct a FIR filter with the given taps

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_rational_resampler_base_scc::install_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_scc::~gr_rational_resampler_base_scc "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_scc::history "Assume block computes y_i = f(x_i, x_i-1, x_i-2, x_i-3...) History is the number of x_i's that are examined to produce one y_i. This comes in handy for FIR filters, where we use history to ensure that our input contains the appropriate \"history\" for the filter. History should be equal to the number of filter taps.

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_scc::set_history "

Params: (history)"

%feature("docstring") gr_rational_resampler_base_scc::interpolation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_scc::decimation "

Params: (NONE)"

%feature("docstring") gr_rational_resampler_base_scc::set_taps "

Params: (taps)"

%feature("docstring") gr_rational_resampler_base_scc::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_rational_resampler_base_scc::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_rational_resampler_base_scc "Creates a rational_resampler_base_scc block.

Rational Resampling Polyphase FIR filter with short input, gr_complex output and gr_complex taps.

Params: (interpolation, decimation, taps)"

%feature("docstring") gr_regenerate_bb "Detect the peak of a signal and repeat every period samples

If a peak is detected, this block outputs a 1 repeated every period samples until reset by detection of another 1 on the input or stopped after max_regen regenerations have occurred.

Note that if max_regen=(-1)/ULONG_MAX then the regeneration will run forever."

%feature("docstring") gr_regenerate_bb::gr_regenerate_bb "

Params: (period, max_regen)"

%feature("docstring") gr_regenerate_bb::set_max_regen "Reset the maximum regeneration count; this will reset the current regen.

Params: (regen)"

%feature("docstring") gr_regenerate_bb::set_period "Reset the period of regenerations; this will reset the current regen.

Params: (period)"

%feature("docstring") gr_regenerate_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_regenerate_bb "Creates a regenerate_bb block.

Detect the peak of a signal and repeat every period samples

If a peak is detected, this block outputs a 1 repeated every period samples until reset by detection of another 1 on the input or stopped after max_regen regenerations have occurred.

Note that if max_regen=(-1)/ULONG_MAX then the regeneration will run forever.

Params: (period, max_regen)"

%feature("docstring") gr_repeat "Repeat a sample 'interp' times in output stream."

%feature("docstring") gr_repeat::gr_repeat "

Params: (itemsize, interp)"

%feature("docstring") gr_repeat::~gr_repeat "

Params: (NONE)"

%feature("docstring") gr_repeat::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_repeat "Creates a repeat block.

Repeat a sample 'interp' times in output stream.

Params: (itemsize, interp)"

%feature("docstring") gr_rms_cf "RMS average power."

%feature("docstring") gr_rms_cf::gr_rms_cf "

Params: (alpha)"

%feature("docstring") gr_rms_cf::~gr_rms_cf "

Params: (NONE)"

%feature("docstring") gr_rms_cf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_rms_cf::unmuted "

Params: (NONE)"

%feature("docstring") gr_rms_cf::set_alpha "

Params: (alpha)"

%feature("docstring") gr_make_rms_cf "Creates a rms_cf block.

RMS average power.

Params: (alpha)"

%feature("docstring") gr_rms_ff "RMS average power."

%feature("docstring") gr_rms_ff::gr_rms_ff "

Params: (alpha)"

%feature("docstring") gr_rms_ff::~gr_rms_ff "

Params: (NONE)"

%feature("docstring") gr_rms_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_rms_ff::unmuted "

Params: (NONE)"

%feature("docstring") gr_rms_ff::set_alpha "

Params: (alpha)"

%feature("docstring") gr_make_rms_ff "Creates a rms_ff block.

RMS average power.

Params: (alpha)"

%feature("docstring") gr_sample_and_hold_bb "sample and hold circuit

Samples the data stream (input stream 0) and holds the value if the control signal is 1 (intput stream 1)."

%feature("docstring") gr_sample_and_hold_bb::gr_sample_and_hold_bb "

Params: (NONE)"

%feature("docstring") gr_sample_and_hold_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_sample_and_hold_bb "Creates a sample_and_hold_bb block.

sample and hold circuit

Samples the data stream (input stream 0) and holds the value if the control signal is 1 (intput stream 1).

Params: (NONE)"

%feature("docstring") gr_sample_and_hold_ff "sample and hold circuit

Samples the data stream (input stream 0) and holds the value if the control signal is 1 (intput stream 1)."

%feature("docstring") gr_sample_and_hold_ff::gr_sample_and_hold_ff "

Params: (NONE)"

%feature("docstring") gr_sample_and_hold_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_sample_and_hold_ff "Creates a sample_and_hold_ff block.

sample and hold circuit

Samples the data stream (input stream 0) and holds the value if the control signal is 1 (intput stream 1).

Params: (NONE)"

%feature("docstring") gr_sample_and_hold_ii "sample and hold circuit

Samples the data stream (input stream 0) and holds the value if the control signal is 1 (intput stream 1)."

%feature("docstring") gr_sample_and_hold_ii::gr_sample_and_hold_ii "

Params: (NONE)"

%feature("docstring") gr_sample_and_hold_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_sample_and_hold_ii "Creates a sample_and_hold_ii block.

sample and hold circuit

Samples the data stream (input stream 0) and holds the value if the control signal is 1 (intput stream 1).

Params: (NONE)"

%feature("docstring") gr_sample_and_hold_ss "sample and hold circuit

Samples the data stream (input stream 0) and holds the value if the control signal is 1 (intput stream 1)."

%feature("docstring") gr_sample_and_hold_ss::gr_sample_and_hold_ss "

Params: (NONE)"

%feature("docstring") gr_sample_and_hold_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_sample_and_hold_ss "Creates a sample_and_hold_ss block.

sample and hold circuit

Samples the data stream (input stream 0) and holds the value if the control signal is 1 (intput stream 1).

Params: (NONE)"

%feature("docstring") gr_scrambler_bb "Scramble an input stream using an LFSR. This block works on the LSB only of the input data stream, i.e., on an \"unpacked binary\" stream, and produces the same format on its output."

%feature("docstring") gr_scrambler_bb::gr_scrambler_bb "

Params: (mask, seed, len)"

%feature("docstring") gr_scrambler_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_scrambler_bb "Creates a scrambler_bb block.

Scramble an input stream using an LFSR. This block works on the LSB only of the input data stream, i.e., on an \"unpacked binary\" stream, and produces the same format on its output.

Params: (mask, seed, len)"

%feature("docstring") gr_short_to_float "Convert stream of short to a stream of float."

%feature("docstring") gr_short_to_float::gr_short_to_float "

Params: (NONE)"

%feature("docstring") gr_short_to_float::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_short_to_float "Creates a short_to_float block.

Convert stream of short to a stream of float.

Params: (NONE)"

%feature("docstring") gr_sig_source_c "signal generator with gr_complex output."

%feature("docstring") gr_sig_source_c::gr_sig_source_c "

Params: (sampling_freq, waveform, wave_freq, ampl, offset)"

%feature("docstring") gr_sig_source_c::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_sig_source_c::sampling_freq "

Params: (NONE)"

%feature("docstring") gr_sig_source_c::waveform "

Params: (NONE)"

%feature("docstring") gr_sig_source_c::frequency "

Params: (NONE)"

%feature("docstring") gr_sig_source_c::amplitude "

Params: (NONE)"

%feature("docstring") gr_sig_source_c::offset "

Params: (NONE)"

%feature("docstring") gr_sig_source_c::set_sampling_freq "

Params: (sampling_freq)"

%feature("docstring") gr_sig_source_c::set_waveform "

Params: (waveform)"

%feature("docstring") gr_sig_source_c::set_frequency "

Params: (frequency)"

%feature("docstring") gr_sig_source_c::set_amplitude "

Params: (ampl)"

%feature("docstring") gr_sig_source_c::set_offset "

Params: (offset)"

%feature("docstring") gr_make_sig_source_c "Creates a sig_source_c block.

signal generator with gr_complex output.

Params: (sampling_freq, waveform, wave_freq, ampl, offset)"

%feature("docstring") gr_sig_source_f "signal generator with float output."

%feature("docstring") gr_sig_source_f::gr_sig_source_f "

Params: (sampling_freq, waveform, wave_freq, ampl, offset)"

%feature("docstring") gr_sig_source_f::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_sig_source_f::sampling_freq "

Params: (NONE)"

%feature("docstring") gr_sig_source_f::waveform "

Params: (NONE)"

%feature("docstring") gr_sig_source_f::frequency "

Params: (NONE)"

%feature("docstring") gr_sig_source_f::amplitude "

Params: (NONE)"

%feature("docstring") gr_sig_source_f::offset "

Params: (NONE)"

%feature("docstring") gr_sig_source_f::set_sampling_freq "

Params: (sampling_freq)"

%feature("docstring") gr_sig_source_f::set_waveform "

Params: (waveform)"

%feature("docstring") gr_sig_source_f::set_frequency "

Params: (frequency)"

%feature("docstring") gr_sig_source_f::set_amplitude "

Params: (ampl)"

%feature("docstring") gr_sig_source_f::set_offset "

Params: (offset)"

%feature("docstring") gr_make_sig_source_f "Creates a sig_source_f block.

signal generator with float output.

Params: (sampling_freq, waveform, wave_freq, ampl, offset)"

%feature("docstring") gr_sig_source_i "signal generator with int output."

%feature("docstring") gr_sig_source_i::gr_sig_source_i "

Params: (sampling_freq, waveform, wave_freq, ampl, offset)"

%feature("docstring") gr_sig_source_i::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_sig_source_i::sampling_freq "

Params: (NONE)"

%feature("docstring") gr_sig_source_i::waveform "

Params: (NONE)"

%feature("docstring") gr_sig_source_i::frequency "

Params: (NONE)"

%feature("docstring") gr_sig_source_i::amplitude "

Params: (NONE)"

%feature("docstring") gr_sig_source_i::offset "

Params: (NONE)"

%feature("docstring") gr_sig_source_i::set_sampling_freq "

Params: (sampling_freq)"

%feature("docstring") gr_sig_source_i::set_waveform "

Params: (waveform)"

%feature("docstring") gr_sig_source_i::set_frequency "

Params: (frequency)"

%feature("docstring") gr_sig_source_i::set_amplitude "

Params: (ampl)"

%feature("docstring") gr_sig_source_i::set_offset "

Params: (offset)"

%feature("docstring") gr_make_sig_source_i "Creates a sig_source_i block.

signal generator with int output.

Params: (sampling_freq, waveform, wave_freq, ampl, offset)"

%feature("docstring") gr_sig_source_s "signal generator with short output."

%feature("docstring") gr_sig_source_s::gr_sig_source_s "

Params: (sampling_freq, waveform, wave_freq, ampl, offset)"

%feature("docstring") gr_sig_source_s::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_sig_source_s::sampling_freq "

Params: (NONE)"

%feature("docstring") gr_sig_source_s::waveform "

Params: (NONE)"

%feature("docstring") gr_sig_source_s::frequency "

Params: (NONE)"

%feature("docstring") gr_sig_source_s::amplitude "

Params: (NONE)"

%feature("docstring") gr_sig_source_s::offset "

Params: (NONE)"

%feature("docstring") gr_sig_source_s::set_sampling_freq "

Params: (sampling_freq)"

%feature("docstring") gr_sig_source_s::set_waveform "

Params: (waveform)"

%feature("docstring") gr_sig_source_s::set_frequency "

Params: (frequency)"

%feature("docstring") gr_sig_source_s::set_amplitude "

Params: (ampl)"

%feature("docstring") gr_sig_source_s::set_offset "

Params: (offset)"

%feature("docstring") gr_make_sig_source_s "Creates a sig_source_s block.

signal generator with short output.

Params: (sampling_freq, waveform, wave_freq, ampl, offset)"

%feature("docstring") gr_simple_correlator "inverse of gr_simple_framer (more or less)"

%feature("docstring") gr_simple_correlator::gr_simple_correlator "

Params: (payload_bytesize)"

%feature("docstring") gr_simple_correlator::slice "

Params: (x)"

%feature("docstring") gr_simple_correlator::update_avg "

Params: (x)"

%feature("docstring") gr_simple_correlator::enter_locked "

Params: (NONE)"

%feature("docstring") gr_simple_correlator::enter_under_threshold "

Params: (NONE)"

%feature("docstring") gr_simple_correlator::enter_looking "

Params: (NONE)"

%feature("docstring") gr_simple_correlator::add_index "

Params: (a, b)"

%feature("docstring") gr_simple_correlator::sub_index "

Params: (a, b)"

%feature("docstring") gr_simple_correlator::~gr_simple_correlator "

Params: (NONE)"

%feature("docstring") gr_simple_correlator::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_simple_correlator "Creates a simple_correlator block.

inverse of gr_simple_framer (more or less)

Params: (payload_bytesize)"

%feature("docstring") gr_simple_correlator_bb "Inverse of simple_framer. Takes a stream of bits from gr_correlate_access_code_bb and returns a stream of items of size payload_bytesize.

input: stream of bits from gr_correlate_access_code_bb output: steam of items of size payload_bytesize.

The framer expects a fixed length header of 1 byte giving the packet number.

The input data consists of bytes that have two bits used. Bit 0, the LSB, contains the data bit. Bit 1 if set, indicates that the corresponding bit is the the first bit of the packet. That is, this bit is the first one after the access code."

%feature("docstring") gr_simple_correlator_bb::gr_simple_correlator_bb "

Params: (target_queue)"

%feature("docstring") gr_simple_correlator_bb::enter_search "

Params: (NONE)"

%feature("docstring") gr_simple_correlator_bb::enter_have_sync "

Params: (NONE)"

%feature("docstring") gr_simple_correlator_bb::enter_have_header "

Params: (seq)"

%feature("docstring") gr_simple_correlator_bb::header_ok "

Params: (NONE)"

%feature("docstring") gr_simple_correlator_bb::header_payload "

Params: (seq)"

%feature("docstring") gr_simple_correlator_bb::~gr_simple_correlator_bb "

Params: (NONE)"

%feature("docstring") gr_simple_correlator_bb::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_simple_correlator_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_simple_correlator_bb "Creates a simple_correlator_bb block.

Inverse of simple_framer. Takes a stream of bits from gr_correlate_access_code_bb and returns a stream of items of size payload_bytesize.

input: stream of bits from gr_correlate_access_code_bb output: steam of items of size payload_bytesize.

The framer expects a fixed length header of 1 byte giving the packet number.

The input data consists of bytes that have two bits used. Bit 0, the LSB, contains the data bit. Bit 1 if set, indicates that the corresponding bit is the the first bit of the packet. That is, this bit is the first one after the access code.

Params: (target_queue)"

%feature("docstring") gr_simple_framer "add sync field, seq number and command field to payload"

%feature("docstring") gr_simple_framer::gr_simple_framer "

Params: (payload_bytesize)"

%feature("docstring") gr_simple_framer::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_simple_framer::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_simple_framer "Creates a simple_framer block.

add sync field, seq number and command field to payload

Params: (payload_bytesize)"

%feature("docstring") gr_simple_squelch_cc "simple squelch block based on average signal power and threshold in dB."

%feature("docstring") gr_simple_squelch_cc::gr_simple_squelch_cc "

Params: (threshold_db, alpha)"

%feature("docstring") gr_simple_squelch_cc::~gr_simple_squelch_cc "

Params: (NONE)"

%feature("docstring") gr_simple_squelch_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_simple_squelch_cc::unmuted "

Params: (NONE)"

%feature("docstring") gr_simple_squelch_cc::set_alpha "

Params: (alpha)"

%feature("docstring") gr_simple_squelch_cc::set_threshold "

Params: (decibels)"

%feature("docstring") gr_simple_squelch_cc::threshold "

Params: (NONE)"

%feature("docstring") gr_simple_squelch_cc::squelch_range "

Params: (NONE)"

%feature("docstring") gr_make_simple_squelch_cc "Creates a simple_squelch_cc block.

simple squelch block based on average signal power and threshold in dB.

Params: (threshold_db, alpha)"

%feature("docstring") gr_single_pole_iir_filter_cc "single pole IIR filter with complex input, complex output

The input and output satisfy a difference equation of the form

with the corresponding rational system function



Note that some texts define the system function with a + in the denominator. If you're using that convention, you'll need to negate the feedback tap."

%feature("docstring") gr_single_pole_iir_filter_cc::gr_single_pole_iir_filter_cc "

Params: (alpha, vlen)"

%feature("docstring") gr_single_pole_iir_filter_cc::~gr_single_pole_iir_filter_cc "

Params: (NONE)"

%feature("docstring") gr_single_pole_iir_filter_cc::set_taps "

Params: (alpha)"

%feature("docstring") gr_single_pole_iir_filter_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_single_pole_iir_filter_cc "Creates a single_pole_iir_filter_cc block.

single pole IIR filter with complex input, complex output

The input and output satisfy a difference equation of the form

with the corresponding rational system function



Note that some texts define the system function with a + in the denominator. If you're using that convention, you'll need to negate the feedback tap.

Params: (alpha, vlen)"

%feature("docstring") gr_single_pole_iir_filter_ff "single pole IIR filter with float input, float output

The input and output satisfy a difference equation of the form

with the corresponding rational system function



Note that some texts define the system function with a + in the denominator. If you're using that convention, you'll need to negate the feedback tap."

%feature("docstring") gr_single_pole_iir_filter_ff::gr_single_pole_iir_filter_ff "

Params: (alpha, vlen)"

%feature("docstring") gr_single_pole_iir_filter_ff::~gr_single_pole_iir_filter_ff "

Params: (NONE)"

%feature("docstring") gr_single_pole_iir_filter_ff::set_taps "

Params: (alpha)"

%feature("docstring") gr_single_pole_iir_filter_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_single_pole_iir_filter_ff "Creates a single_pole_iir_filter_ff block.

single pole IIR filter with float input, float output

The input and output satisfy a difference equation of the form

with the corresponding rational system function



Note that some texts define the system function with a + in the denominator. If you're using that convention, you'll need to negate the feedback tap.

Params: (alpha, vlen)"

%feature("docstring") gr_single_threaded_scheduler "Simple scheduler for stream computations."

%feature("docstring") gr_single_threaded_scheduler::~gr_single_threaded_scheduler "

Params: (NONE)"

%feature("docstring") gr_single_threaded_scheduler::run "

Params: (NONE)"

%feature("docstring") gr_single_threaded_scheduler::stop "

Params: (NONE)"

%feature("docstring") gr_single_threaded_scheduler::gr_single_threaded_scheduler "

Params: (blocks)"

%feature("docstring") gr_single_threaded_scheduler::main_loop "

Params: (NONE)"

%feature("docstring") gr_make_single_threaded_scheduler "Creates a single_threaded_scheduler block.

Simple scheduler for stream computations.

Params: (blocks)"

%feature("docstring") gr_skiphead "skips the first N items, from then on copies items to the output

Useful for building test cases and sources which have metadata or junk at the start"

%feature("docstring") gr_skiphead::gr_skiphead "

Params: (itemsize, nitems_to_skip)"

%feature("docstring") gr_skiphead::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_skiphead "Creates a skiphead block.

skips the first N items, from then on copies items to the output

Useful for building test cases and sources which have metadata or junk at the start

Params: (itemsize, nitems_to_skip)"



%feature("docstring") gr_squash_ff::gr_squash_ff "

Params: (igrid, ogrid)"

%feature("docstring") gr_squash_ff::~gr_squash_ff "

Params: (NONE)"

%feature("docstring") gr_squash_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_squash_ff "Creates a squash_ff block.



Params: (igrid, ogrid)"

%feature("docstring") gr_stream_mux "Stream muxing block to multiplex many streams into one with a specified format.

Muxes N streams together producing an output stream that contains N0 items from the first stream, N1 items from the second, etc. and repeats:

[N0, N1, N2, ..., Nm, N0, N1, ...]"

%feature("docstring") gr_stream_mux::gr_stream_mux "

Params: (itemsize, lengths)"

%feature("docstring") gr_stream_mux::increment_stream "

Params: (NONE)"

%feature("docstring") gr_stream_mux::~gr_stream_mux "

Params: ()"

%feature("docstring") gr_stream_mux::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_stream_mux::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_make_stream_mux "Creates a stream_mux block.

Stream muxing block to multiplex many streams into one with a specified format.

Muxes N streams together producing an output stream that contains N0 items from the first stream, N1 items from the second, etc. and repeats:

[N0, N1, N2, ..., Nm, N0, N1, ...]

Params: (itemsize, lengths)"

%feature("docstring") gr_stream_to_streams "convert a stream of items into a N streams of items

Converts a stream of N items into N streams of 1 item. Repeat ad infinitum."

%feature("docstring") gr_stream_to_streams::gr_stream_to_streams "

Params: (item_size, nstreams)"

%feature("docstring") gr_stream_to_streams::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_stream_to_streams "Creates a stream_to_streams block.

convert a stream of items into a N streams of items

Converts a stream of N items into N streams of 1 item. Repeat ad infinitum.

Params: (item_size, nstreams)"

%feature("docstring") gr_stream_to_vector "convert a stream of items into a stream of blocks containing nitems_per_block"

%feature("docstring") gr_stream_to_vector::gr_stream_to_vector "

Params: (item_size, nitems_per_block)"

%feature("docstring") gr_stream_to_vector::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_stream_to_vector "Creates a stream_to_vector block.

convert a stream of items into a stream of blocks containing nitems_per_block

Params: (item_size, nitems_per_block)"

%feature("docstring") gr_streams_to_stream "Convert N streams of 1 item into a 1 stream of N items

Convert N streams of 1 item into 1 stream of N items. Repeat ad infinitum."

%feature("docstring") gr_streams_to_stream::gr_streams_to_stream "

Params: (item_size, nstreams)"

%feature("docstring") gr_streams_to_stream::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_streams_to_stream "Creates a streams_to_stream block.

Convert N streams of 1 item into a 1 stream of N items

Convert N streams of 1 item into 1 stream of N items. Repeat ad infinitum.

Params: (item_size, nstreams)"

%feature("docstring") gr_streams_to_vector "convert N streams of items to 1 stream of vector length N"

%feature("docstring") gr_streams_to_vector::gr_streams_to_vector "

Params: (item_size, nstreams)"

%feature("docstring") gr_streams_to_vector::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_streams_to_vector "Creates a streams_to_vector block.

convert N streams of items to 1 stream of vector length N

Params: (item_size, nstreams)"



%feature("docstring") gr_stretch_ff::gr_stretch_ff "

Params: (lo, vlen)"

%feature("docstring") gr_stretch_ff::lo "

Params: (NONE)"

%feature("docstring") gr_stretch_ff::set_lo "

Params: (lo)"

%feature("docstring") gr_stretch_ff::vlen "

Params: (NONE)"

%feature("docstring") gr_stretch_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_stretch_ff "Creates a stretch_ff block.



Params: (lo, vlen)"

%feature("docstring") gr_sub_cc "output = input_0 - input_1 - ...)

Subtract across all input streams."

%feature("docstring") gr_sub_cc::gr_sub_cc "

Params: (vlen)"

%feature("docstring") gr_sub_cc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_sub_cc "Creates a sub_cc block.

output = input_0 - input_1 - ...)

Subtract across all input streams.

Params: (vlen)"

%feature("docstring") gr_sub_ff "output = input_0 - input_1 - ...)

Subtract across all input streams."

%feature("docstring") gr_sub_ff::gr_sub_ff "

Params: (vlen)"

%feature("docstring") gr_sub_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_sub_ff "Creates a sub_ff block.

output = input_0 - input_1 - ...)

Subtract across all input streams.

Params: (vlen)"

%feature("docstring") gr_sub_ii "output = input_0 - input_1 - ...)

Subtract across all input streams."

%feature("docstring") gr_sub_ii::gr_sub_ii "

Params: (vlen)"

%feature("docstring") gr_sub_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_sub_ii "Creates a sub_ii block.

output = input_0 - input_1 - ...)

Subtract across all input streams.

Params: (vlen)"

%feature("docstring") gr_sub_ss "output = input_0 - input_1 - ...)

Subtract across all input streams."

%feature("docstring") gr_sub_ss::gr_sub_ss "

Params: (vlen)"

%feature("docstring") gr_sub_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_sub_ss "Creates a sub_ss block.

output = input_0 - input_1 - ...)

Subtract across all input streams.

Params: (vlen)"

%feature("docstring") gr_tagged_file_sink "Write stream to file descriptor."

%feature("docstring") gr_tagged_file_sink::gr_tagged_file_sink "

Params: (itemsize, samp_rate)"

%feature("docstring") gr_tagged_file_sink::~gr_tagged_file_sink "

Params: (NONE)"

%feature("docstring") gr_tagged_file_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_tagged_file_sink "Creates a tagged_file_sink block.

Write stream to file descriptor.

Params: (itemsize, samp_rate)"

%feature("docstring") gr_test "Test class for testing runtime system (setting up buffers and such.)

This block does not do any usefull actual data processing. It just exposes setting all standard block parameters using the contructor or public methods.

This block can be usefull when testing the runtime system. You can force this block to have a large history, decimation factor and/or large output_multiple. The runtime system should detect this and create large enough buffers all through the signal chain."

%feature("docstring") gr_test::~gr_test "

Params: (NONE)"

%feature("docstring") gr_test::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_test::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_test::set_check_topology "Force check topology to return true or false.

Params: (check_topology)"

%feature("docstring") gr_test::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_test::fixed_rate_ninput_to_noutput "Given ninput samples, return number of output samples that will be produced. N.B. this is only defined if fixed_rate returns true. Generally speaking, you don't need to override this.

Params: (ninput)"

%feature("docstring") gr_test::fixed_rate_noutput_to_ninput "Given noutput samples, return number of input samples required to produce noutput. N.B. this is only defined if fixed_rate returns true.

Params: (noutput)"

%feature("docstring") gr_test::set_fixed_rate_public "Set if fixed rate should return true. N.B. This is normally a private method but we make it available here as public.

Params: (fixed_rate)"

%feature("docstring") gr_test::set_consume_type "Set the consume pattern.

Params: (cons_type)"

%feature("docstring") gr_test::set_consume_limit "Set the consume limit.

Params: (limit)"

%feature("docstring") gr_test::set_produce_type "Set the produce pattern.

Params: (prod_type)"

%feature("docstring") gr_test::set_produce_limit "Set the produce limit.

Params: (limit)"

%feature("docstring") gr_test::gr_test "

Params: (name, min_inputs, max_inputs, sizeof_input_item, min_outputs, max_outputs, sizeof_output_item, history, output_multiple, relative_rate, fixed_rate, cons_type, prod_type)"

%feature("docstring") gr_make_test "Creates a test block.

Test class for testing runtime system (setting up buffers and such.)

This block does not do any usefull actual data processing. It just exposes setting all standard block parameters using the contructor or public methods.

This block can be usefull when testing the runtime system. You can force this block to have a large history, decimation factor and/or large output_multiple. The runtime system should detect this and create large enough buffers all through the signal chain.

Params: (name, min_inputs, max_inputs, sizeof_input_item, min_outputs, max_outputs, sizeof_output_item, history, output_multiple, relative_rate, fixed_rate, cons_type, prod_type)"

%feature("docstring") gr_threshold_ff "Please fix my documentation."

%feature("docstring") gr_threshold_ff::gr_threshold_ff "

Params: (lo, hi, initial_state)"

%feature("docstring") gr_threshold_ff::lo "

Params: (NONE)"

%feature("docstring") gr_threshold_ff::set_lo "

Params: (lo)"

%feature("docstring") gr_threshold_ff::hi "

Params: (NONE)"

%feature("docstring") gr_threshold_ff::set_hi "

Params: (hi)"

%feature("docstring") gr_threshold_ff::last_state "

Params: (NONE)"

%feature("docstring") gr_threshold_ff::set_last_state "

Params: (last_state)"

%feature("docstring") gr_threshold_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_threshold_ff "Creates a threshold_ff block.

Please fix my documentation.

Params: (lo, hi, initial_state)"

%feature("docstring") gr_throttle "throttle flow of samples such that the average rate does not exceed samples_per_sec.

input: one stream of itemsize; output: one stream of itemsize

N.B. this should only be used in GUI apps where there is no other rate limiting block. It is not intended nor effective at precisely controlling the rate of samples. That should be controlled by a source or sink tied to sample clock. E.g., a USRP or audio card."

%feature("docstring") gr_throttle::gr_throttle "

Params: (itemsize, samples_per_sec)"

%feature("docstring") gr_throttle::~gr_throttle "

Params: (NONE)"

%feature("docstring") gr_throttle::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_throttle "Creates a throttle block.

throttle flow of samples such that the average rate does not exceed samples_per_sec.

input: one stream of itemsize; output: one stream of itemsize

N.B. this should only be used in GUI apps where there is no other rate limiting block. It is not intended nor effective at precisely controlling the rate of samples. That should be controlled by a source or sink tied to sample clock. E.g., a USRP or audio card.

Params: (itemsize, samples_per_sec)"

%feature("docstring") gr_timer "implement timeouts"

%feature("docstring") gr_timer::gr_timer "

Params: ()"

%feature("docstring") gr_timer::~gr_timer "

Params: (NONE)"

%feature("docstring") gr_timer::schedule_at "schedule timer to fire at abs_when

Params: (abs_when)"

%feature("docstring") gr_timer::schedule_after "schedule timer to fire rel_when seconds from now.

Params: (rel_when)"

%feature("docstring") gr_timer::schedule_periodic "schedule a periodic timeout.

Params: (abs_when, period)"

%feature("docstring") gr_timer::unschedule "cancel timer

Params: (NONE)"

%feature("docstring") gr_timer::now "return absolute current time (seconds since the epoc).

Params: (NONE)"

%feature("docstring") gr_make_timer "Creates a timer block.

implement timeouts

Params: ()"

%feature("docstring") gr_top_block "Top-level hierarchical block representing a flowgraph."

%feature("docstring") gr_top_block::gr_top_block "

Params: (name)"

%feature("docstring") gr_top_block::~gr_top_block "

Params: (NONE)"

%feature("docstring") gr_top_block::run "The simple interface to running a flowgraph.

Calls start() then wait(). Used to run a flowgraph that will stop on its own, or when another thread will call stop().

Params: (NONE)"

%feature("docstring") gr_top_block::start "Start the contained flowgraph. Creates one or more threads to execute the flow graph. Returns to the caller once the threads are created. Calling start() on a top_block that is already started IS an error.

Params: (NONE)"

%feature("docstring") gr_top_block::stop "Stop the running flowgraph. Notifies each thread created by the scheduler to shutdown, then returns to caller. Calling stop() on a top_block that is already stopped IS NOT an error.

Params: (NONE)"

%feature("docstring") gr_top_block::wait "Wait for a flowgraph to complete. Flowgraphs complete when either (1) all blocks indicate that they are done (typically only when using gr.file_source, or gr.head, or (2) after stop() has been called to request shutdown. Calling wait on a top_block that is not running IS NOT an error (wait returns w/o blocking).

Params: (NONE)"

%feature("docstring") gr_top_block::lock "Lock a flowgraph in preparation for reconfiguration. When an equal number of calls to lock() and unlock() have occurred, the flowgraph will be reconfigured.

N.B. lock() and unlock() may not be called from a flowgraph thread (E.g., gr_block::work method) or deadlock will occur when reconfiguration happens.

Params: (NONE)"

%feature("docstring") gr_top_block::unlock "Unlock a flowgraph in preparation for reconfiguration. When an equal number of calls to lock() and unlock() have occurred, the flowgraph will be reconfigured.

N.B. lock() and unlock() may not be called from a flowgraph thread (E.g., gr_block::work method) or deadlock will occur when reconfiguration happens.

Params: (NONE)"

%feature("docstring") gr_top_block::dump "Displays flattened flowgraph edges and block connectivity

Params: (NONE)"

%feature("docstring") gr_top_block::to_top_block "

Params: (NONE)"

%feature("docstring") gr_make_top_block "Creates a top_block block.

Top-level hierarchical block representing a flowgraph.

Params: (name)"

%feature("docstring") gr_uchar_to_float "Convert stream of unsigned chars to a stream of float."

%feature("docstring") gr_uchar_to_float::gr_uchar_to_float "

Params: (NONE)"

%feature("docstring") gr_uchar_to_float::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_uchar_to_float "Creates a uchar_to_float block.

Convert stream of unsigned chars to a stream of float.

Params: (NONE)"

%feature("docstring") gr_udp_sink "Write stream to an UDP socket."

%feature("docstring") gr_udp_sink::gr_udp_sink "UDP Sink Constructor.

Params: (itemsize, host, port, payload_size, eof)"

%feature("docstring") gr_udp_sink::~gr_udp_sink "

Params: (NONE)"

%feature("docstring") gr_udp_sink::payload_size "return the PAYLOAD_SIZE of the socket

Params: (NONE)"

%feature("docstring") gr_udp_sink::connect "Change the connection to a new destination.

Calls disconnect() to terminate any current connection first.

Params: (host, port)"

%feature("docstring") gr_udp_sink::disconnect "Send zero-length packet (if eof is requested) then stop sending.

Zero-byte packets can be interpreted as EOF by gr_udp_source. Note that disconnect occurs automatically when the sink is destroyed, but not when its top_block stops.

Params: (NONE)"

%feature("docstring") gr_udp_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_udp_sink "Creates a udp_sink block.

Write stream to an UDP socket.

Params: (itemsize, host, port, payload_size, eof)"

%feature("docstring") gr_udp_source "Read stream from an UDP socket."

%feature("docstring") gr_udp_source::gr_udp_source "UDP Source Constructor.

Params: (itemsize, host, port, payload_size, eof, wait)"

%feature("docstring") gr_udp_source::~gr_udp_source "

Params: (NONE)"

%feature("docstring") gr_udp_source::payload_size "return the PAYLOAD_SIZE of the socket

Params: (NONE)"

%feature("docstring") gr_udp_source::get_port "return the port number of the socket

Params: (NONE)"

%feature("docstring") gr_udp_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_udp_source "Creates a udp_source block.

Read stream from an UDP socket.

Params: (itemsize, host, port, payload_size, eof, wait)"

%feature("docstring") gr_unpack_k_bits_bb "Converts a byte with k relevent bits to k output bytes with 1 bit in the LSB."

%feature("docstring") gr_unpack_k_bits_bb::gr_unpack_k_bits_bb "

Params: (k)"

%feature("docstring") gr_unpack_k_bits_bb::~gr_unpack_k_bits_bb "

Params: (NONE)"

%feature("docstring") gr_unpack_k_bits_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_unpack_k_bits_bb "Creates a unpack_k_bits_bb block.

Converts a byte with k relevent bits to k output bytes with 1 bit in the LSB.

Params: (k)"

%feature("docstring") gr_unpacked_to_packed_bb "Convert a stream of unpacked bytes or shorts into a stream of packed bytes or shorts.

input: stream of unsigned char; output: stream of unsigned char.

This is the inverse of gr_packed_to_unpacked_XX.

The low  bits are extracted from each input byte or short. These bits are then packed densely into the output bytes or shorts, such that all 8 or 16 bits of the output bytes or shorts are filled with valid input bits. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_unpacked_to_packed_bb::gr_unpacked_to_packed_bb "

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_unpacked_to_packed_bb::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_unpacked_to_packed_bb::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_unpacked_to_packed_bb::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_unpacked_to_packed_bb "Creates a unpacked_to_packed_bb block.

Convert a stream of unpacked bytes or shorts into a stream of packed bytes or shorts.

input: stream of unsigned char; output: stream of unsigned char.

This is the inverse of gr_packed_to_unpacked_XX.

The low  bits are extracted from each input byte or short. These bits are then packed densely into the output bytes or shorts, such that all 8 or 16 bits of the output bytes or shorts are filled with valid input bits. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_unpacked_to_packed_ii "Convert a stream of unpacked bytes or shorts into a stream of packed bytes or shorts.

input: stream of int; output: stream of int.

This is the inverse of gr_packed_to_unpacked_XX.

The low  bits are extracted from each input byte or short. These bits are then packed densely into the output bytes or shorts, such that all 8 or 16 bits of the output bytes or shorts are filled with valid input bits. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_unpacked_to_packed_ii::gr_unpacked_to_packed_ii "

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_unpacked_to_packed_ii::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_unpacked_to_packed_ii::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_unpacked_to_packed_ii::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_unpacked_to_packed_ii "Creates a unpacked_to_packed_ii block.

Convert a stream of unpacked bytes or shorts into a stream of packed bytes or shorts.

input: stream of int; output: stream of int.

This is the inverse of gr_packed_to_unpacked_XX.

The low  bits are extracted from each input byte or short. These bits are then packed densely into the output bytes or shorts, such that all 8 or 16 bits of the output bytes or shorts are filled with valid input bits. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_unpacked_to_packed_ss "Convert a stream of unpacked bytes or shorts into a stream of packed bytes or shorts.

input: stream of short; output: stream of short.

This is the inverse of gr_packed_to_unpacked_XX.

The low  bits are extracted from each input byte or short. These bits are then packed densely into the output bytes or shorts, such that all 8 or 16 bits of the output bytes or shorts are filled with valid input bits. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols."

%feature("docstring") gr_unpacked_to_packed_ss::gr_unpacked_to_packed_ss "

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_unpacked_to_packed_ss::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_unpacked_to_packed_ss::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_unpacked_to_packed_ss::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_make_unpacked_to_packed_ss "Creates a unpacked_to_packed_ss block.

Convert a stream of unpacked bytes or shorts into a stream of packed bytes or shorts.

input: stream of short; output: stream of short.

This is the inverse of gr_packed_to_unpacked_XX.

The low  bits are extracted from each input byte or short. These bits are then packed densely into the output bytes or shorts, such that all 8 or 16 bits of the output bytes or shorts are filled with valid input bits. The right thing is done if bits_per_chunk is not a power of two.

The combination of gr_packed_to_unpacked_XX followed by gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the general case of mapping from a stream of bytes or shorts into arbitrary float or complex symbols.

Params: (bits_per_chunk, endianness)"

%feature("docstring") gr_vco_f "VCO - Voltage controlled oscillator

input: float stream of control voltages; output: float oscillator output."

%feature("docstring") gr_vco_f::gr_vco_f "VCO - Voltage controlled oscillator.

Params: (sampling_rate, sensitivity, amplitude)"

%feature("docstring") gr_vco_f::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_vco_f "Creates a vco_f block.

VCO - Voltage controlled oscillator

input: float stream of control voltages; output: float oscillator output.

Params: (sampling_rate, sensitivity, amplitude)"

%feature("docstring") gr_vector_sink_b "unsigned char sink that writes to a vector"

%feature("docstring") gr_vector_sink_b::gr_vector_sink_b "

Params: (vlen)"

%feature("docstring") gr_vector_sink_b::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_vector_sink_b::reset "

Params: (NONE)"

%feature("docstring") gr_vector_sink_b::clear "

Params: (NONE)"

%feature("docstring") gr_vector_sink_b::data "

Params: (NONE)"

%feature("docstring") gr_make_vector_sink_b "Creates a vector_sink_b block.

unsigned char sink that writes to a vector

Params: (vlen)"

%feature("docstring") gr_vector_sink_c "gr_complex sink that writes to a vector"

%feature("docstring") gr_vector_sink_c::gr_vector_sink_c "

Params: (vlen)"

%feature("docstring") gr_vector_sink_c::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_vector_sink_c::reset "

Params: (NONE)"

%feature("docstring") gr_vector_sink_c::clear "

Params: (NONE)"

%feature("docstring") gr_vector_sink_c::data "

Params: (NONE)"

%feature("docstring") gr_make_vector_sink_c "Creates a vector_sink_c block.

gr_complex sink that writes to a vector

Params: (vlen)"

%feature("docstring") gr_vector_sink_f "float sink that writes to a vector"

%feature("docstring") gr_vector_sink_f::gr_vector_sink_f "

Params: (vlen)"

%feature("docstring") gr_vector_sink_f::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_vector_sink_f::reset "

Params: (NONE)"

%feature("docstring") gr_vector_sink_f::clear "

Params: (NONE)"

%feature("docstring") gr_vector_sink_f::data "

Params: (NONE)"

%feature("docstring") gr_make_vector_sink_f "Creates a vector_sink_f block.

float sink that writes to a vector

Params: (vlen)"

%feature("docstring") gr_vector_sink_i "int sink that writes to a vector"

%feature("docstring") gr_vector_sink_i::gr_vector_sink_i "

Params: (vlen)"

%feature("docstring") gr_vector_sink_i::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_vector_sink_i::reset "

Params: (NONE)"

%feature("docstring") gr_vector_sink_i::clear "

Params: (NONE)"

%feature("docstring") gr_vector_sink_i::data "

Params: (NONE)"

%feature("docstring") gr_make_vector_sink_i "Creates a vector_sink_i block.

int sink that writes to a vector

Params: (vlen)"

%feature("docstring") gr_vector_sink_s "short sink that writes to a vector"

%feature("docstring") gr_vector_sink_s::gr_vector_sink_s "

Params: (vlen)"

%feature("docstring") gr_vector_sink_s::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_vector_sink_s::reset "

Params: (NONE)"

%feature("docstring") gr_vector_sink_s::clear "

Params: (NONE)"

%feature("docstring") gr_vector_sink_s::data "

Params: (NONE)"

%feature("docstring") gr_make_vector_sink_s "Creates a vector_sink_s block.

short sink that writes to a vector

Params: (vlen)"

%feature("docstring") gr_vector_source_b "source of unsigned char's that gets its data from a vector"

%feature("docstring") gr_vector_source_b::gr_vector_source_b "

Params: (data, repeat, vlen)"

%feature("docstring") gr_vector_source_b::rewind "

Params: (NONE)"

%feature("docstring") gr_vector_source_b::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_vector_source_b "Creates a vector_source_b block.

source of unsigned char's that gets its data from a vector

Params: (data, repeat, vlen)"

%feature("docstring") gr_vector_source_c "source of gr_complex's that gets its data from a vector"

%feature("docstring") gr_vector_source_c::gr_vector_source_c "

Params: (data, repeat, vlen)"

%feature("docstring") gr_vector_source_c::rewind "

Params: (NONE)"

%feature("docstring") gr_vector_source_c::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_vector_source_c "Creates a vector_source_c block.

source of gr_complex's that gets its data from a vector

Params: (data, repeat, vlen)"

%feature("docstring") gr_vector_source_f "source of float's that gets its data from a vector"

%feature("docstring") gr_vector_source_f::gr_vector_source_f "

Params: (data, repeat, vlen)"

%feature("docstring") gr_vector_source_f::rewind "

Params: (NONE)"

%feature("docstring") gr_vector_source_f::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_vector_source_f "Creates a vector_source_f block.

source of float's that gets its data from a vector

Params: (data, repeat, vlen)"

%feature("docstring") gr_vector_source_i "source of int's that gets its data from a vector"

%feature("docstring") gr_vector_source_i::gr_vector_source_i "

Params: (data, repeat, vlen)"

%feature("docstring") gr_vector_source_i::rewind "

Params: (NONE)"

%feature("docstring") gr_vector_source_i::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_vector_source_i "Creates a vector_source_i block.

source of int's that gets its data from a vector

Params: (data, repeat, vlen)"

%feature("docstring") gr_vector_source_s "source of short's that gets its data from a vector"

%feature("docstring") gr_vector_source_s::gr_vector_source_s "

Params: (data, repeat, vlen)"

%feature("docstring") gr_vector_source_s::rewind "

Params: (NONE)"

%feature("docstring") gr_vector_source_s::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_vector_source_s "Creates a vector_source_s block.

source of short's that gets its data from a vector

Params: (data, repeat, vlen)"

%feature("docstring") gr_vector_to_stream "convert a stream of blocks of nitems_per_block items into a stream of items"

%feature("docstring") gr_vector_to_stream::gr_vector_to_stream "

Params: (item_size, nitems_per_block)"

%feature("docstring") gr_vector_to_stream::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_vector_to_stream "Creates a vector_to_stream block.

convert a stream of blocks of nitems_per_block items into a stream of items

Params: (item_size, nitems_per_block)"

%feature("docstring") gr_vector_to_streams "Convert 1 stream of vectors of length N to N streams of items."

%feature("docstring") gr_vector_to_streams::gr_vector_to_streams "

Params: (item_size, nstreams)"

%feature("docstring") gr_vector_to_streams::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_vector_to_streams "Creates a vector_to_streams block.

Convert 1 stream of vectors of length N to N streams of items.

Params: (item_size, nstreams)"

%feature("docstring") gr_wavelet_ff "compute wavelet transform using gsl routines"

%feature("docstring") gr_wavelet_ff::gr_wavelet_ff "

Params: (size, order, forward)"

%feature("docstring") gr_wavelet_ff::~gr_wavelet_ff "

Params: (NONE)"

%feature("docstring") gr_wavelet_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_wavelet_ff "Creates a wavelet_ff block.

compute wavelet transform using gsl routines

Params: (size, order, forward)"

%feature("docstring") gr_wavfile_sink "Read stream from a Microsoft PCM (.wav) file, output floats.

Values are within [-1;1]. Check gr_make_wavfile_source() for extra info."

%feature("docstring") gr_wavfile_sink::gr_wavfile_sink "

Params: (filename, n_channels, sample_rate, bits_per_sample)"

%feature("docstring") gr_wavfile_sink::convert_to_short "Convert a sample value within [-1;+1] to a corresponding short integer value.

Params: (sample)"

%feature("docstring") gr_wavfile_sink::close_wav "Writes information to the WAV header which is not available a-priori (chunk size etc.) and closes the file. Not thread-safe and assumes d_fp is a valid file pointer, should thus only be called by other methods.

Params: (NONE)"

%feature("docstring") gr_wavfile_sink::~gr_wavfile_sink "

Params: (NONE)"

%feature("docstring") gr_wavfile_sink::open "Opens a new file and writes a WAV header. Thread-safe.

Params: (filename)"

%feature("docstring") gr_wavfile_sink::close "Closes the currently active file and completes the WAV header. Thread-safe.

Params: (NONE)"

%feature("docstring") gr_wavfile_sink::do_update "If any file changes have occurred, update now. This is called internally by work() and thus doesn't usually need to be called by hand.

Params: (NONE)"

%feature("docstring") gr_wavfile_sink::set_sample_rate "Set the sample rate. This will not affect the WAV file currently opened. Any following open() calls will use this new sample rate.

Params: (sample_rate)"

%feature("docstring") gr_wavfile_sink::set_bits_per_sample "Set bits per sample. This will not affect the WAV file currently opened (see set_sample_rate()). If the value is neither 8 nor 16, the call is ignored and the current value is kept.

Params: (bits_per_sample)"

%feature("docstring") gr_wavfile_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_wavfile_sink "Creates a wavfile_sink block.

Read stream from a Microsoft PCM (.wav) file, output floats.

Values are within [-1;1]. Check gr_make_wavfile_source() for extra info.

Params: (filename, n_channels, sample_rate, bits_per_sample)"

%feature("docstring") gr_wavfile_source "Read stream from a Microsoft PCM (.wav) file, output floats.

Unless otherwise called, values are within [-1;1]. Check gr_make_wavfile_source() for extra info."

%feature("docstring") gr_wavfile_source::gr_wavfile_source "

Params: (filename, repeat)"

%feature("docstring") gr_wavfile_source::convert_to_float "Convert an integer sample value to a float value within [-1;1].

Params: (sample)"

%feature("docstring") gr_wavfile_source::~gr_wavfile_source "

Params: (NONE)"

%feature("docstring") gr_wavfile_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_wavfile_source::sample_rate "Read the sample rate as specified in the wav file header.

Params: (NONE)"

%feature("docstring") gr_wavfile_source::bits_per_sample "Return the number of bits per sample as specified in the wav file header. Only 8 or 16 bit are supported here.

Params: (NONE)"

%feature("docstring") gr_wavfile_source::channels "Return the number of channels in the wav file as specified in the wav file header. This is also the max number of outputs you can have.

Params: (NONE)"

%feature("docstring") gr_make_wavfile_source "Creates a wavfile_source block.

Read stream from a Microsoft PCM (.wav) file, output floats.

Unless otherwise called, values are within [-1;1]. Check gr_make_wavfile_source() for extra info.

Params: (filename, repeat)"

%feature("docstring") gr_wvps_ff "computes the Wavelet Power Spectrum from a set of wavelet coefficients"

%feature("docstring") gr_wvps_ff::gr_wvps_ff "

Params: (ilen)"

%feature("docstring") gr_wvps_ff::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_wvps_ff "Creates a wvps_ff block.

computes the Wavelet Power Spectrum from a set of wavelet coefficients

Params: (ilen)"

%feature("docstring") gr_xor_bb "output = input_0 ^ input_1 ^ , ... ^ input_N)

bitwise boolean xor across all input streams."

%feature("docstring") gr_xor_bb::gr_xor_bb "

Params: (NONE)"

%feature("docstring") gr_xor_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_xor_bb "Creates a xor_bb block.

output = input_0 ^ input_1 ^ , ... ^ input_N)

bitwise boolean xor across all input streams.

Params: (NONE)"

%feature("docstring") gr_xor_ii "output = input_0 ^ input_1 ^ , ... ^ input_N)

bitwise boolean xor across all input streams."

%feature("docstring") gr_xor_ii::gr_xor_ii "

Params: (NONE)"

%feature("docstring") gr_xor_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_xor_ii "Creates a xor_ii block.

output = input_0 ^ input_1 ^ , ... ^ input_N)

bitwise boolean xor across all input streams.

Params: (NONE)"

%feature("docstring") gr_xor_ss "output = input_0 ^ input_1 ^ , ... ^ input_N)

bitwise boolean xor across all input streams."

%feature("docstring") gr_xor_ss::gr_xor_ss "

Params: (NONE)"

%feature("docstring") gr_xor_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_make_xor_ss "Creates a xor_ss block.

output = input_0 ^ input_1 ^ , ... ^ input_N)

bitwise boolean xor across all input streams.

Params: (NONE)"



%feature("docstring") msdd_source_simple::msdd_source_simple "

Params: (src, port_src)"

%feature("docstring") msdd_source_simple::~msdd_source_simple "

Params: (NONE)"

%feature("docstring") msdd_source_simple::stop "Called to disable drivers, etc for i/o devices.

Params: (NONE)"

%feature("docstring") msdd_source_simple::start "Called to enable drivers, etc for i/o devices.

This allows a block to enable an associated driver to begin transfering data just before we start to execute the scheduler. The end result is that this reduces latency in the pipeline when dealing with audio devices, usrps, etc.

Params: (NONE)"

%feature("docstring") msdd_source_simple::set_decim_rate "

Params: (int)"

%feature("docstring") msdd_source_simple::set_rx_freq "

Params: (, )"

%feature("docstring") msdd_source_simple::set_pga "

Params: (, )"

%feature("docstring") msdd_source_simple::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") msdd_source_simple::adc_freq "

Params: (NONE)"

%feature("docstring") msdd_source_simple::decim_rate "

Params: (NONE)"

%feature("docstring") msdd_source_simple::gain_range "

Params: (NONE)"

%feature("docstring") msdd_source_simple::freq_range "

Params: (NONE)"

%feature("docstring") msdd_make_source_simple "Creates a source_simple block.



Params: (src, port_src)"



%feature("docstring") noaa_hrpt_decoder::noaa_hrpt_decoder "

Params: (verbose, output_files)"

%feature("docstring") noaa_hrpt_decoder::process_mfnum "

Params: (NONE)"

%feature("docstring") noaa_hrpt_decoder::process_address "

Params: (NONE)"

%feature("docstring") noaa_hrpt_decoder::process_day_of_year "

Params: (NONE)"

%feature("docstring") noaa_hrpt_decoder::process_milli1 "

Params: (NONE)"

%feature("docstring") noaa_hrpt_decoder::process_milli2 "

Params: (NONE)"

%feature("docstring") noaa_hrpt_decoder::process_milli3 "

Params: (NONE)"

%feature("docstring") noaa_hrpt_decoder::~noaa_hrpt_decoder "

Params: (NONE)"

%feature("docstring") noaa_hrpt_decoder::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") noaa_make_hrpt_decoder "Creates a hrpt_decoder block.



Params: (verbose, output_files)"



%feature("docstring") noaa_hrpt_deframer::noaa_hrpt_deframer "

Params: (NONE)"

%feature("docstring") noaa_hrpt_deframer::enter_idle "

Params: (NONE)"

%feature("docstring") noaa_hrpt_deframer::enter_synced "

Params: (NONE)"

%feature("docstring") noaa_hrpt_deframer::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") noaa_make_hrpt_deframer "Creates a hrpt_deframer block.



Params: (NONE)"



%feature("docstring") noaa_hrpt_pll_cf::noaa_hrpt_pll_cf "

Params: (alpha, beta, max_offset)"

%feature("docstring") noaa_hrpt_pll_cf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") noaa_hrpt_pll_cf::set_alpha "

Params: (alpha)"

%feature("docstring") noaa_hrpt_pll_cf::set_beta "

Params: (beta)"

%feature("docstring") noaa_hrpt_pll_cf::set_max_offset "

Params: (max_offset)"

%feature("docstring") noaa_make_hrpt_pll_cf "Creates a hrpt_pll_cf block.



Params: (alpha, beta, max_offset)"

%feature("docstring") pager_flex_deinterleave "flex deinterleave description"

%feature("docstring") pager_flex_deinterleave::pager_flex_deinterleave "

Params: (NONE)"

%feature("docstring") pager_flex_deinterleave::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") pager_make_flex_deinterleave "Creates a flex_deinterleave block.

flex deinterleave description

Params: (NONE)"

%feature("docstring") pager_flex_frame "flex_frame."

%feature("docstring") pager_flex_frame::pager_flex_frame "

Params: (NONE)"

%feature("docstring") pager_flex_frame::~pager_flex_frame "

Params: (NONE)"

%feature("docstring") pager_make_flex_frame "Creates a flex_frame block.

flex_frame.

Params: (NONE)"

%feature("docstring") pager_flex_sync "flex sync description"

%feature("docstring") pager_flex_sync::pager_flex_sync "

Params: (NONE)"

%feature("docstring") pager_flex_sync::enter_idle "

Params: (NONE)"

%feature("docstring") pager_flex_sync::enter_syncing "

Params: (NONE)"

%feature("docstring") pager_flex_sync::enter_sync1 "

Params: (NONE)"

%feature("docstring") pager_flex_sync::enter_sync2 "

Params: (NONE)"

%feature("docstring") pager_flex_sync::enter_data "

Params: (NONE)"

%feature("docstring") pager_flex_sync::index_avg "

Params: (start, end)"

%feature("docstring") pager_flex_sync::test_sync "

Params: (sym)"

%feature("docstring") pager_flex_sync::output_symbol "

Params: (sym)"

%feature("docstring") pager_flex_sync::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") pager_flex_sync::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") pager_make_flex_sync "Creates a flex_sync block.

flex sync description

Params: (NONE)"

%feature("docstring") pager_slicer_fb "slicer description"

%feature("docstring") pager_slicer_fb::pager_slicer_fb "

Params: (alpha)"

%feature("docstring") pager_slicer_fb::slice "

Params: (sample)"

%feature("docstring") pager_slicer_fb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") pager_slicer_fb::dc_offset "

Params: (NONE)"

%feature("docstring") pager_make_slicer_fb "Creates a slicer_fb block.

slicer description

Params: (alpha)"

%feature("docstring") trellis_encoder_bb "Convolutional encoder."

%feature("docstring") trellis_encoder_bb::trellis_encoder_bb "

Params: (FSM, ST)"

%feature("docstring") trellis_encoder_bb::FSM "

Params: (NONE)"

%feature("docstring") trellis_encoder_bb::ST "

Params: (NONE)"

%feature("docstring") trellis_encoder_bb::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") trellis_make_encoder_bb "Creates a encoder_bb block.

Convolutional encoder.

Params: (FSM, ST)"

%feature("docstring") trellis_encoder_bi "Convolutional encoder."

%feature("docstring") trellis_encoder_bi::trellis_encoder_bi "

Params: (FSM, ST)"

%feature("docstring") trellis_encoder_bi::FSM "

Params: (NONE)"

%feature("docstring") trellis_encoder_bi::ST "

Params: (NONE)"

%feature("docstring") trellis_encoder_bi::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") trellis_make_encoder_bi "Creates a encoder_bi block.

Convolutional encoder.

Params: (FSM, ST)"

%feature("docstring") trellis_encoder_bs "Convolutional encoder."

%feature("docstring") trellis_encoder_bs::trellis_encoder_bs "

Params: (FSM, ST)"

%feature("docstring") trellis_encoder_bs::FSM "

Params: (NONE)"

%feature("docstring") trellis_encoder_bs::ST "

Params: (NONE)"

%feature("docstring") trellis_encoder_bs::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") trellis_make_encoder_bs "Creates a encoder_bs block.

Convolutional encoder.

Params: (FSM, ST)"

%feature("docstring") trellis_encoder_ii "Convolutional encoder."

%feature("docstring") trellis_encoder_ii::trellis_encoder_ii "

Params: (FSM, ST)"

%feature("docstring") trellis_encoder_ii::FSM "

Params: (NONE)"

%feature("docstring") trellis_encoder_ii::ST "

Params: (NONE)"

%feature("docstring") trellis_encoder_ii::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") trellis_make_encoder_ii "Creates a encoder_ii block.

Convolutional encoder.

Params: (FSM, ST)"

%feature("docstring") trellis_encoder_si "Convolutional encoder."

%feature("docstring") trellis_encoder_si::trellis_encoder_si "

Params: (FSM, ST)"

%feature("docstring") trellis_encoder_si::FSM "

Params: (NONE)"

%feature("docstring") trellis_encoder_si::ST "

Params: (NONE)"

%feature("docstring") trellis_encoder_si::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") trellis_make_encoder_si "Creates a encoder_si block.

Convolutional encoder.

Params: (FSM, ST)"

%feature("docstring") trellis_encoder_ss "Convolutional encoder."

%feature("docstring") trellis_encoder_ss::trellis_encoder_ss "

Params: (FSM, ST)"

%feature("docstring") trellis_encoder_ss::FSM "

Params: (NONE)"

%feature("docstring") trellis_encoder_ss::ST "

Params: (NONE)"

%feature("docstring") trellis_encoder_ss::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") trellis_make_encoder_ss "Creates a encoder_ss block.

Convolutional encoder.

Params: (FSM, ST)"

%feature("docstring") trellis_metrics_c "Evaluate metrics for use by the Viterbi algorithm."

%feature("docstring") trellis_metrics_c::trellis_metrics_c "

Params: (O, D, TABLE, TYPE)"

%feature("docstring") trellis_metrics_c::O "

Params: (NONE)"

%feature("docstring") trellis_metrics_c::D "

Params: (NONE)"

%feature("docstring") trellis_metrics_c::TYPE "

Params: (NONE)"

%feature("docstring") trellis_metrics_c::TABLE "

Params: (NONE)"

%feature("docstring") trellis_metrics_c::set_TABLE "

Params: (table)"

%feature("docstring") trellis_metrics_c::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_metrics_c::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_metrics_c "Creates a metrics_c block.

Evaluate metrics for use by the Viterbi algorithm.

Params: (O, D, TABLE, TYPE)"

%feature("docstring") trellis_metrics_f "Evaluate metrics for use by the Viterbi algorithm."

%feature("docstring") trellis_metrics_f::trellis_metrics_f "

Params: (O, D, TABLE, TYPE)"

%feature("docstring") trellis_metrics_f::O "

Params: (NONE)"

%feature("docstring") trellis_metrics_f::D "

Params: (NONE)"

%feature("docstring") trellis_metrics_f::TYPE "

Params: (NONE)"

%feature("docstring") trellis_metrics_f::TABLE "

Params: (NONE)"

%feature("docstring") trellis_metrics_f::set_TABLE "

Params: (table)"

%feature("docstring") trellis_metrics_f::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_metrics_f::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_metrics_f "Creates a metrics_f block.

Evaluate metrics for use by the Viterbi algorithm.

Params: (O, D, TABLE, TYPE)"

%feature("docstring") trellis_metrics_i "Evaluate metrics for use by the Viterbi algorithm."

%feature("docstring") trellis_metrics_i::trellis_metrics_i "

Params: (O, D, TABLE, TYPE)"

%feature("docstring") trellis_metrics_i::O "

Params: (NONE)"

%feature("docstring") trellis_metrics_i::D "

Params: (NONE)"

%feature("docstring") trellis_metrics_i::TYPE "

Params: (NONE)"

%feature("docstring") trellis_metrics_i::TABLE "

Params: (NONE)"

%feature("docstring") trellis_metrics_i::set_TABLE "

Params: (table)"

%feature("docstring") trellis_metrics_i::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_metrics_i::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_metrics_i "Creates a metrics_i block.

Evaluate metrics for use by the Viterbi algorithm.

Params: (O, D, TABLE, TYPE)"

%feature("docstring") trellis_metrics_s "Evaluate metrics for use by the Viterbi algorithm."

%feature("docstring") trellis_metrics_s::trellis_metrics_s "

Params: (O, D, TABLE, TYPE)"

%feature("docstring") trellis_metrics_s::O "

Params: (NONE)"

%feature("docstring") trellis_metrics_s::D "

Params: (NONE)"

%feature("docstring") trellis_metrics_s::TYPE "

Params: (NONE)"

%feature("docstring") trellis_metrics_s::TABLE "

Params: (NONE)"

%feature("docstring") trellis_metrics_s::set_TABLE "

Params: (table)"

%feature("docstring") trellis_metrics_s::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_metrics_s::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_metrics_s "Creates a metrics_s block.

Evaluate metrics for use by the Viterbi algorithm.

Params: (O, D, TABLE, TYPE)"

%feature("docstring") trellis_permutation "Permutation."

%feature("docstring") trellis_permutation::trellis_permutation "

Params: (K, TABLE, SYMS_PER_BLOCK, NBYTES)"

%feature("docstring") trellis_permutation::K "

Params: (NONE)"

%feature("docstring") trellis_permutation::TABLE "

Params: (NONE)"

%feature("docstring") trellis_permutation::SYMS_PER_BLOCK "

Params: (NONE)"

%feature("docstring") trellis_permutation::NBYTES_INOUT "

Params: (NONE)"

%feature("docstring") trellis_permutation::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") trellis_make_permutation "Creates a permutation block.

Permutation.

Params: (K, TABLE, SYMS_PER_BLOCK, NBYTES)"



%feature("docstring") trellis_siso_combined_f::trellis_siso_combined_f "

Params: (FSM, K, S0, SK, POSTI, POSTO, d_SISO_TYPE, D, TABLE, TYPE)"

%feature("docstring") trellis_siso_combined_f::FSM "

Params: (NONE)"

%feature("docstring") trellis_siso_combined_f::K "

Params: (NONE)"

%feature("docstring") trellis_siso_combined_f::S0 "

Params: (NONE)"

%feature("docstring") trellis_siso_combined_f::SK "

Params: (NONE)"

%feature("docstring") trellis_siso_combined_f::POSTI "

Params: (NONE)"

%feature("docstring") trellis_siso_combined_f::POSTO "

Params: (NONE)"

%feature("docstring") trellis_siso_combined_f::SISO_TYPE "

Params: (NONE)"

%feature("docstring") trellis_siso_combined_f::D "

Params: (NONE)"

%feature("docstring") trellis_siso_combined_f::TABLE "

Params: (NONE)"

%feature("docstring") trellis_siso_combined_f::TYPE "

Params: (NONE)"

%feature("docstring") trellis_siso_combined_f::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_siso_combined_f::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_siso_combined_f "Creates a siso_combined_f block.



Params: (FSM, K, S0, SK, POSTI, POSTO, d_SISO_TYPE, D, TABLE, TYPE)"



%feature("docstring") trellis_siso_f::trellis_siso_f "

Params: (FSM, K, S0, SK, POSTI, POSTO, d_SISO_TYPE)"

%feature("docstring") trellis_siso_f::FSM "

Params: (NONE)"

%feature("docstring") trellis_siso_f::K "

Params: (NONE)"

%feature("docstring") trellis_siso_f::S0 "

Params: (NONE)"

%feature("docstring") trellis_siso_f::SK "

Params: (NONE)"

%feature("docstring") trellis_siso_f::POSTI "

Params: (NONE)"

%feature("docstring") trellis_siso_f::POSTO "

Params: (NONE)"

%feature("docstring") trellis_siso_f::SISO_TYPE "

Params: (NONE)"

%feature("docstring") trellis_siso_f::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_siso_f::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_siso_f "Creates a siso_f block.



Params: (FSM, K, S0, SK, POSTI, POSTO, d_SISO_TYPE)"



%feature("docstring") trellis_viterbi_b::trellis_viterbi_b "

Params: (FSM, K, S0, SK)"

%feature("docstring") trellis_viterbi_b::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_b::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_b::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_b::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_b::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_b::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_b "Creates a viterbi_b block.



Params: (FSM, K, S0, SK)"



%feature("docstring") trellis_viterbi_combined_cb::trellis_viterbi_combined_cb "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_cb::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cb::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cb::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cb::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cb::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cb::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cb::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cb::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_cb::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_cb::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_cb "Creates a viterbi_combined_cb block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_combined_ci::trellis_viterbi_combined_ci "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_ci::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ci::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ci::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ci::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ci::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ci::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ci::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ci::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_ci::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_ci::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_ci "Creates a viterbi_combined_ci block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_combined_cs::trellis_viterbi_combined_cs "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_cs::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cs::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cs::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cs::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cs::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cs::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cs::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_cs::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_cs::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_cs::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_cs "Creates a viterbi_combined_cs block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_combined_fb::trellis_viterbi_combined_fb "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_fb::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fb::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fb::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fb::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fb::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fb::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fb::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fb::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_fb::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_fb::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_fb "Creates a viterbi_combined_fb block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_combined_fi::trellis_viterbi_combined_fi "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_fi::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fi::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fi::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fi::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fi::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fi::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fi::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fi::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_fi::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_fi::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_fi "Creates a viterbi_combined_fi block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_combined_fs::trellis_viterbi_combined_fs "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_fs::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fs::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fs::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fs::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fs::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fs::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fs::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_fs::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_fs::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_fs::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_fs "Creates a viterbi_combined_fs block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_combined_ib::trellis_viterbi_combined_ib "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_ib::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ib::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ib::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ib::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ib::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ib::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ib::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ib::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_ib::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_ib::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_ib "Creates a viterbi_combined_ib block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_combined_ii::trellis_viterbi_combined_ii "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_ii::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ii::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ii::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ii::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ii::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ii::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ii::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ii::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_ii::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_ii::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_ii "Creates a viterbi_combined_ii block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_combined_is::trellis_viterbi_combined_is "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_is::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_is::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_is::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_is::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_is::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_is::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_is::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_is::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_is::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_is::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_is "Creates a viterbi_combined_is block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_combined_sb::trellis_viterbi_combined_sb "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_sb::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_sb::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_sb::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_sb::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_sb::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_sb::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_sb::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_sb::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_sb::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_sb::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_sb "Creates a viterbi_combined_sb block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_combined_si::trellis_viterbi_combined_si "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_si::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_si::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_si::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_si::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_si::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_si::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_si::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_si::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_si::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_si::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_si "Creates a viterbi_combined_si block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_combined_ss::trellis_viterbi_combined_ss "

Params: (FSM, K, S0, SK, D, TABLE, TYPE)"

%feature("docstring") trellis_viterbi_combined_ss::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ss::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ss::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ss::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ss::D "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ss::TABLE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ss::TYPE "

Params: (NONE)"

%feature("docstring") trellis_viterbi_combined_ss::set_TABLE "

Params: (table)"

%feature("docstring") trellis_viterbi_combined_ss::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_combined_ss::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_combined_ss "Creates a viterbi_combined_ss block.



Params: (FSM, K, S0, SK, D, TABLE, TYPE)"



%feature("docstring") trellis_viterbi_i::trellis_viterbi_i "

Params: (FSM, K, S0, SK)"

%feature("docstring") trellis_viterbi_i::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_i::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_i::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_i::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_i::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_i::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_i "Creates a viterbi_i block.



Params: (FSM, K, S0, SK)"



%feature("docstring") trellis_viterbi_s::trellis_viterbi_s "

Params: (FSM, K, S0, SK)"

%feature("docstring") trellis_viterbi_s::FSM "

Params: (NONE)"

%feature("docstring") trellis_viterbi_s::K "

Params: (NONE)"

%feature("docstring") trellis_viterbi_s::S0 "

Params: (NONE)"

%feature("docstring") trellis_viterbi_s::SK "

Params: (NONE)"

%feature("docstring") trellis_viterbi_s::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") trellis_viterbi_s::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") trellis_make_viterbi_s "Creates a viterbi_s block.



Params: (FSM, K, S0, SK)"



%feature("docstring") usrp2_sink_16sc::usrp2_sink_16sc "

Params: (ifc, mac)"

%feature("docstring") usrp2_sink_16sc::~usrp2_sink_16sc "

Params: (NONE)"

%feature("docstring") usrp2_sink_16sc::work "Derived class must override this.

Params: (noutput_items, input_items, output_items)"

%feature("docstring") usrp2_make_sink_16sc "Creates a sink_16sc block.



Params: (ifc, mac)"



%feature("docstring") usrp2_sink_32fc::usrp2_sink_32fc "

Params: (ifc, mac)"

%feature("docstring") usrp2_sink_32fc::~usrp2_sink_32fc "

Params: (NONE)"

%feature("docstring") usrp2_sink_32fc::work "Derived class must override this.

Params: (noutput_items, input_items, output_items)"

%feature("docstring") usrp2_make_sink_32fc "Creates a sink_32fc block.



Params: (ifc, mac)"



%feature("docstring") usrp2_source_16sc::usrp2_source_16sc "

Params: (ifc, mac)"

%feature("docstring") usrp2_source_16sc::~usrp2_source_16sc "

Params: (NONE)"

%feature("docstring") usrp2_source_16sc::work "Derived class must override this.

Params: (noutput_items, input_items, output_items)"

%feature("docstring") usrp2_make_source_16sc "Creates a source_16sc block.



Params: (ifc, mac)"



%feature("docstring") usrp2_source_32fc::usrp2_source_32fc "

Params: (ifc, mac)"

%feature("docstring") usrp2_source_32fc::~usrp2_source_32fc "

Params: (NONE)"

%feature("docstring") usrp2_source_32fc::work "Derived class must override this.

Params: (noutput_items, input_items, output_items)"

%feature("docstring") usrp2_make_source_32fc "Creates a source_32fc block.



Params: (ifc, mac)"

%feature("docstring") usrp_sink_c "Interface to Universal Software Radio Peripheral Tx path

input: gr_complex."

%feature("docstring") usrp_sink_c::usrp_sink_c "

Params: (which_board, interp_rate, nchan, mux, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_sink_c::copy_to_usrp_buffer "convert between input item format and usrp native format

Params: (input_items, input_index, input_items_available, input_items_consumed, usrp_buffer, usrp_buffer_length, bytes_written)"

%feature("docstring") usrp_sink_c::~usrp_sink_c "

Params: (NONE)"

%feature("docstring") usrp_make_sink_c "Creates a sink_c block.

Interface to Universal Software Radio Peripheral Tx path

input: gr_complex.

Params: (which_board, interp_rate, nchan, mux, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_sink_s "Interface to Universal Software Radio Peripheral Tx path

input: short."

%feature("docstring") usrp_sink_s::usrp_sink_s "

Params: (which_board, interp_rate, nchan, mux, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_sink_s::copy_to_usrp_buffer "convert between input item format and usrp native format

Params: (input_items, input_index, input_items_available, input_items_consumed, usrp_buffer, usrp_buffer_length, bytes_written)"

%feature("docstring") usrp_sink_s::~usrp_sink_s "

Params: (NONE)"

%feature("docstring") usrp_make_sink_s "Creates a sink_s block.

Interface to Universal Software Radio Peripheral Tx path

input: short.

Params: (which_board, interp_rate, nchan, mux, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_source_c "Interface to Universal Software Radio Peripheral Rx path

output: 1 stream of complex<float>"

%feature("docstring") usrp_source_c::usrp_source_c "

Params: (which_board, decim_rate, nchan, mux, mode, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_source_c::ninput_bytes_reqd_for_noutput_items "return number of usrp input bytes required to produce noutput items.

Params: (noutput_items)"

%feature("docstring") usrp_source_c::copy_from_usrp_buffer "convert between native usrp format and output item format

The copy must consume all bytes available. That is,  must equal .

Params: (output_items, output_index, output_items_available, output_items_produced, usrp_buffer, usrp_buffer_length, bytes_read)"

%feature("docstring") usrp_source_c::~usrp_source_c "

Params: (NONE)"

%feature("docstring") usrp_make_source_c "Creates a source_c block.

Interface to Universal Software Radio Peripheral Rx path

output: 1 stream of complex<float>

Params: (which_board, decim_rate, nchan, mux, mode, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_source_s "Interface to Universal Software Radio Peripheral Rx path

output: 1 stream of short."

%feature("docstring") usrp_source_s::usrp_source_s "

Params: (which_board, decim_rate, nchan, mux, mode, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_source_s::ninput_bytes_reqd_for_noutput_items "return number of usrp input bytes required to produce noutput items.

Params: (noutput_items)"

%feature("docstring") usrp_source_s::copy_from_usrp_buffer "convert between native usrp format and output item format

The copy must consume all bytes available. That is,  must equal .

Params: (output_items, output_index, output_items_available, output_items_produced, usrp_buffer, usrp_buffer_length, bytes_read)"

%feature("docstring") usrp_source_s::~usrp_source_s "

Params: (NONE)"

%feature("docstring") usrp_make_source_s "Creates a source_s block.

Interface to Universal Software Radio Peripheral Rx path

output: 1 stream of short.

Params: (which_board, decim_rate, nchan, mux, mode, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") aadvarky_enough "

Params: (aad)"

%feature("docstring") main "

Params: (NONE)"

%feature("docstring") audio_alsa_make_sink "make an alsa audio sink.

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_alsa_make_source "Make an ALSA audio source.

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_jack_make_sink "make an JACK audio sink.

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") jack_sink_process "

Params: (nframes, arg)"

%feature("docstring") audio_jack_make_source "make a JACK audio source.

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") jack_source_process "

Params: (nframes, arg)"

%feature("docstring") audio_oss_make_sink "

Params: (sampling_rate, dev, ok_to_block)"

%feature("docstring") audio_oss_make_source "

Params: (sampling_rate, dev, ok_to_block)"

%feature("docstring") audio_osx_make_sink "

Params: (sample_rate, device_name, do_block, channel_config, max_sample_count)"

%feature("docstring") audio_osx_make_source "

Params: (sample_rate, device_name, do_block, channel_config, max_sample_count)"

%feature("docstring") audio_portaudio_make_sink "PORTAUDIO audio sink.

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_portaudio_make_source "PORTAUDIO audio source.

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_windows_make_sink "

Params: (sampling_freq, dev)"

%feature("docstring") audio_windows_make_source "

Params: (sampling_freq, dev)"

%feature("docstring") dec2base "change base

Params: (num, base, s)"

%feature("docstring") dec2bases "

Params: (num, bases, s)"

%feature("docstring") base2dec "

Params: (s, base)"

%feature("docstring") bases2dec "

Params: (s, bases)"

%feature("docstring") ccomplex_dotprod_generic "

Params: (input, taps, n_2_ccomplex_blocks, result)"

%feature("docstring") ccomplex_dotprod_3dnow "

Params: (input, taps, n_2_ccomplex_blocks, result)"

%feature("docstring") ccomplex_dotprod_3dnowext "

Params: (input, taps, n_2_ccomplex_blocks, result)"

%feature("docstring") ccomplex_dotprod_sse "

Params: (input, taps, n_2_ccomplex_blocks, result)"

%feature("docstring") modnn "

Params: (rs, x)"

%feature("docstring") complex_dotprod_generic "

Params: (input, taps, n_2_complex_blocks, result)"

%feature("docstring") complex_dotprod_3dnow "

Params: (input, taps, n_2_complex_blocks, result)"

%feature("docstring") complex_dotprod_3dnowext "

Params: (input, taps, n_2_complex_blocks, result)"

%feature("docstring") complex_dotprod_sse "

Params: (input, taps, n_2_complex_blocks, result)"

%feature("docstring") complex_vec_test0 "

Params: (NONE)"

%feature("docstring") complex_vec_test1 "

Params: (input)"

%feature("docstring") complex_scalar_test0 "

Params: (NONE)"

%feature("docstring") complex_scalar_test1 "

Params: (input)"

%feature("docstring") int_vec_test0 "

Params: (NONE)"

%feature("docstring") int_vec_test1 "

Params: (input)"

%feature("docstring") float_vec_test0 "

Params: (NONE)"

%feature("docstring") float_vec_test1 "

Params: (input)"

%feature("docstring") getopt "

Params: (argc, argv, optstring)"

%feature("docstring") usleep "

Params: (usec)"

%feature("docstring") nanosleep "

Params: (req, rem)"

%feature("docstring") gettimeofday "

Params: (tv, tz)"

%feature("docstring") create_atsci_equalizer_nop "

Params: (NONE)"

%feature("docstring") create_atsci_equalizer_lms "

Params: (NONE)"

%feature("docstring") create_atsci_equalizer_lms2 "

Params: (NONE)"

%feature("docstring") create_atsci_fs_checker "Factory that creates appropriate atsci_fs_checker

Params: (NONE)"

%feature("docstring") create_atsci_fs_correlator "Factory that creates appropriate atsci_fs_correlator

Params: (NONE)"

%feature("docstring") cvsd_make_decode_bs "Constructor parameters to initialize the CVSD decoder. The default values are modeled after the Bluetooth standard and should not be changed, except by an advanced user.

Params: (min_step, max_step, step_decay, accum_decay, K, J, pos_accum_max, neg_accum_max)"

%feature("docstring") cvsd_make_encode_sb "Constructor parameters to initialize the CVSD encoder. The default values are modeled after the Bluetooth standard and should not be changed except by an advanced user.

Params: (min_step, max_step, step_decay, accum_decay, K, J, pos_accum_max, neg_accum_max)"

%feature("docstring") darwin_error_str "

Params: (result)"

%feature("docstring") darwin_to_errno "

Params: (result)"

%feature("docstring") ep_to_pipeRef "

Params: (device, ep)"

%feature("docstring") operator<< "

Params: (os, x)"

%feature("docstring") instantiate_dbs "

Params: (dbid, usrp, which_side)"

%feature("docstring") int_seq_to_str "

Params: (seq)"

%feature("docstring") str_to_int_seq "

Params: (str)"

%feature("docstring") make_dial_tone "

Params: (NONE)"

%feature("docstring") dotprod_ccf_armv7_a "

Params: (a, b, res, n)"

%feature("docstring") dotprod_fff_altivec "

Params: (a, b, n)"

%feature("docstring") dotprod_fff_armv7_a "

Params: (a, b, n)"

%feature("docstring") fcomplex_dotprod_3dnow "

Params: (input, taps, n_2_complex_blocks, result)"

%feature("docstring") fcomplex_dotprod_sse "

Params: (input, taps, n_2_complex_blocks, result)"

%feature("docstring") _fft_1d_r2 "

Params: (out, in, W, log2_size)"

%feature("docstring") mod255 "

Params: (x)"

%feature("docstring") float_dotprod_generic "

Params: (input, taps, n_4_float_blocks)"

%feature("docstring") float_dotprod_3dnow "

Params: (input, taps, n_4_float_blocks)"

%feature("docstring") float_dotprod_sse "

Params: (input, taps, n_4_float_blocks)"

%feature("docstring") ep_to_pipeRef "

Params: (device, ep)"

%feature("docstring") g72x_init_state "

Params: ()"

%feature("docstring") g721_encoder "

Params: (sample, in_coding, state_ptr)"

%feature("docstring") g721_decoder "

Params: (code, out_coding, state_ptr)"

%feature("docstring") g723_24_encoder "

Params: (sample, in_coding, state_ptr)"

%feature("docstring") g723_24_decoder "

Params: (code, out_coding, state_ptr)"

%feature("docstring") g723_40_encoder "

Params: (sample, in_coding, state_ptr)"

%feature("docstring") g723_40_decoder "

Params: (code, out_coding, state_ptr)"

%feature("docstring") quantize "

Params: (d, y, table, size)"

%feature("docstring") reconstruct "

Params: (, , )"

%feature("docstring") update "

Params: (code_size, y, wi, fi, dq, sr, dqsez, state_ptr)"

%feature("docstring") tandem_adjust_alaw "

Params: (sr, se, y, i, sign, qtab)"

%feature("docstring") tandem_adjust_ulaw "

Params: (sr, se, y, i, sign, qtab)"

%feature("docstring") linear2alaw "

Params: (pcm_val)"

%feature("docstring") alaw2linear "

Params: (a_val)"

%feature("docstring") linear2ulaw "

Params: (pcm_val)"

%feature("docstring") ulaw2linear "

Params: (u_val)"

%feature("docstring") predictor_zero "

Params: (state_ptr)"

%feature("docstring") predictor_pole "

Params: (state_ptr)"

%feature("docstring") step_size "

Params: (state_ptr)"

%feature("docstring") gc_aligned_alloc "Return pointer to chunk of storage of size size bytes. The allocation will be aligned to an  boundary.

Throws if can't allocate memory. The storage should be freed with \"free\" when done. The memory is initialized to zero.

Params: (size, alignment)"

%feature("docstring") gc_udelay "

Params: (usecs)"

%feature("docstring") gc_cdelay "

Params: (cpu_cycles)"

%feature("docstring") gc_jd_queue_init "Initialize the queue to empty.

Params: (q)"

%feature("docstring") gc_jd_queue_enqueue "Add  to the tail of .

Params: (q, item)"

%feature("docstring") gc_jd_queue_dequeue "Remove and return item at head of queue, or 0 if queue is empty.

Params: (q)"

%feature("docstring") gc_jd_queue_dequeue "Remove and return item at head of queue.

If return is not GCQ_OK, we're holding a lock-line reservation that covers the queue.

Params: (q, item_ea, jd_tag, item)"

%feature("docstring") gc_jd_stack_init "Initialize the stack to empty.

Params: (stack)"

%feature("docstring") gc_jd_stack_push "Add  to the top of .

Params: (stack, item)"

%feature("docstring") gc_jd_stack_pop "pop and return top item on stack, or 0 if stack is empty

Params: (stack)"

%feature("docstring") ea_to_jdp "

Params: (ea)"

%feature("docstring") jdp_to_ea "

Params: (item)"

%feature("docstring") gc_program_handle_from_filename "Return a boost::shared_ptr to an spe_program_handle_t.

Calls spe_image_open to open the file. If successful returns a boost::shared_ptr that will call spe_image_close when it's time to free the object.

Returns the equivalent of the NULL pointer if the file cannot be opened, or if it's not an SPE ELF object file.

Params: (filename)"

%feature("docstring") gc_program_handle_from_address "Return a boost::shared_ptr to an spe_program_handle_t.

If successful returns a boost::shared_ptr that does nothing when it's time to free the object.

Params: (handle)"

%feature("docstring") gc_job_status_string "map gc_job_status_t into a string

Params: (status)"

%feature("docstring") gc_make_job_manager "

Params: (options)"

%feature("docstring") gcpd_find_table "find the gc_proc_def table in the SPE program

Params: (program, table, nentries, ls_addr)"

%feature("docstring") gc_uniform_deviate "Return a uniformly distributed value in the range [0, 1.0) (Linear congruential generator. YMMV. Caveat emptor.).

Params: ()"

%feature("docstring") gc_set_seed "

Params: (seed)"

%feature("docstring") ea_to_ptr "

Params: (ea)"

%feature("docstring") ptr_to_ea "

Params: (p)"

%feature("docstring") __get_cpuid_max "

Params: (__ext, __sig)"

%feature("docstring") __get_cpuid "

Params: (__level, __eax, __ebx, __ecx, __edx)"

%feature("docstring") __get_cpuid_max "

Params: (__ext, __sig)"

%feature("docstring") __get_cpuid "

Params: (__level, __eax, __ebx, __ecx, __edx)"

%feature("docstring") gcp_fft_1d_r2_submit "Submit a job that computes the forward or inverse FFT.

Returns a shared_ptr to a job descriptor which should be passed to wait_job*. Throws an exception in the event of a problem. This uses the FFTW conventions for scaling. That is, neither the forward nor inverse are scaled by 1/fft_length.

Params: (mgr, log2_fft_length, forward, shift, out, in, twiddle, window)"

%feature("docstring") gcp_fft_1d_r2_twiddle "Compute twiddle factors.

Params: (log2_fft_length, W)"

%feature("docstring") operator< "

Params: (lhs, rhs)"

%feature("docstring") gr_basic_block_ncurrently_allocated "

Params: (NONE)"

%feature("docstring") operator<< "

Params: (os, basic_block)"

%feature("docstring") operator<< "

Params: (os, m)"

%feature("docstring") gr_block_detail_ncurrently_allocated "

Params: (NONE)"

%feature("docstring") gr_buffer_add_reader "Create a new gr_buffer_reader and attach it to buffer .

Params: (buf, nzero_preload, link)"

%feature("docstring") gr_buffer_ncurrently_allocated "returns # of gr_buffers currently allocated

Params: (NONE)"

%feature("docstring") gr_buffer_reader_ncurrently_allocated "returns # of gr_buffer_readers currently allocated

Params: (NONE)"

%feature("docstring") is_complex "

Params: (x)"

%feature("docstring") gr_prefix "return ./configure --prefix argument. Typically /usr/local

Params: (NONE)"

%feature("docstring") gr_sysconfdir "return ./configure --sysconfdir argument. Typically $prefix/etc or /etc

Params: (NONE)"

%feature("docstring") gr_prefsdir "return preferences file directory. Typically $sysconfdir/etc/conf.d

Params: (NONE)"

%feature("docstring") gr_build_date "return date/time of build, as set when 'bootstrap' is run

Params: (NONE)"

%feature("docstring") gr_version "return version string defined in configure.ac

Params: (NONE)"

%feature("docstring") gr_count_bits8 "

Params: (x)"

%feature("docstring") gr_count_bits16 "

Params: (x)"

%feature("docstring") gr_count_bits32 "

Params: (x)"

%feature("docstring") gr_count_bits64 "

Params: (x)"

%feature("docstring") gr_update_crc32 "update running CRC-32

Update a running CRC with the bytes buf[0..len-1] The CRC should be initialized to all 1's, and the transmitted value is the 1's complement of the final running CRC. The resulting CRC should be transmitted in big endian order.

Params: (crc, buf, len)"

%feature("docstring") gr_crc32 "

Params: (buf, len)"

%feature("docstring") gr_dispatcher_singleton "

Params: (NONE)"

%feature("docstring") gr_expj "

Params: (phase)"

%feature("docstring") gr_feval_dd_example "trivial examples / test cases showing C++ calling Python code

Params: (f, x)"

%feature("docstring") gr_feval_cc_example "

Params: (f, x)"

%feature("docstring") gr_feval_ll_example "

Params: (f, x)"

%feature("docstring") gr_feval_example "

Params: (f)"

%feature("docstring") gr_fir_sysconfig_singleton "

Params: (NONE)"

%feature("docstring") operator<< "

Params: (os, endp)"

%feature("docstring") gr_make_io_signature "Create an i/o signature.

Params: (min_streams, max_streams, sizeof_stream_item)"

%feature("docstring") gr_make_io_signature2 "Create an i/o signature.

Params: (min_streams, max_streams, sizeof_stream_item1, sizeof_stream_item2)"

%feature("docstring") gr_make_io_signature3 "Create an i/o signature.

Params: (min_streams, max_streams, sizeof_stream_item1, sizeof_stream_item2, sizeof_stream_item3)"

%feature("docstring") gr_make_io_signaturev "Create an i/o signature.

If there are more streams than there are entries in sizeof_stream_items, the value of the last entry in sizeof_stream_items is used for the missing values. sizeof_stream_items must contain at least 1 entry.

Params: (min_streams, max_streams, sizeof_stream_items)"

%feature("docstring") gr_log2_const "

Params: (NONE)"

%feature("docstring") gr_log2_const< 1 > "

Params: (NONE)"

%feature("docstring") gr_log2_const< 2 > "

Params: (NONE)"

%feature("docstring") gr_log2_const< 4 > "

Params: (NONE)"

%feature("docstring") gr_log2_const< 8 > "

Params: (NONE)"

%feature("docstring") gr_log2_const< 16 > "

Params: (NONE)"

%feature("docstring") gr_log2_const< 32 > "

Params: (NONE)"

%feature("docstring") gr_log2_const< 64 > "

Params: (NONE)"

%feature("docstring") gr_log2_const< 128 > "

Params: (NONE)"

%feature("docstring") gr_log2_const< 256 > "

Params: (NONE)"

%feature("docstring") gr_log2_const< 512 > "

Params: (NONE)"

%feature("docstring") gr_log2_const< 1024 > "

Params: (NONE)"

%feature("docstring") gr_is_power_of_2 "

Params: (x)"

%feature("docstring") gr_fast_atan2f "Fast arc tangent using table lookup and linear interpolation.

This function calculates the angle of the vector (x,y) based on a table lookup and linear interpolation. The table uses a 256 point table covering -45 to +45 degrees and uses symetry to determine the final angle value in the range of -180 to 180 degrees. Note that this function uses the small angle approximation for values close to zero. This routine calculates the arc tangent with an average error of +/- 0.045 degrees.

Params: (y, x)"

%feature("docstring") gr_branchless_clip "

Params: (x, clip)"

%feature("docstring") gr_clip "

Params: (x, clip)"

%feature("docstring") gr_binary_slicer "

Params: (x)"

%feature("docstring") gr_quad_45deg_slicer "

Params: (r, i)"

%feature("docstring") gr_quad_0deg_slicer "

Params: (r, i)"

%feature("docstring") gr_branchless_binary_slicer "

Params: (x)"

%feature("docstring") gr_branchless_quad_0deg_slicer "

Params: (r, i)"

%feature("docstring") gr_branchless_quad_45deg_slicer "

Params: (r, i)"

%feature("docstring") gr_p2_round_down "

Params: (x, pow2)"

%feature("docstring") gr_p2_round_up "

Params: (x, pow2)"

%feature("docstring") gr_p2_modulo "

Params: (x, pow2)"

%feature("docstring") gr_p2_modulo_neg "

Params: (x, pow2)"

%feature("docstring") gr_make_message_from_string "

Params: (s, type, arg1, arg2)"

%feature("docstring") gr_message_ncurrently_allocated "

Params: (NONE)"

%feature("docstring") gr_rounduppow2 "

Params: (n)"

%feature("docstring") gr_zero_vector "

Params: (v)"

%feature("docstring") gr_make_msg_queue "

Params: (limit)"

%feature("docstring") gr_pagesize "return the page size in bytes

Params: (NONE)"

%feature("docstring") gr_enable_realtime_scheduling "If possible, enable high-priority \"real time\" scheduling.

Params: (NONE)"

%feature("docstring") gr_remez "Parks-McClellan FIR filter design.

Calculates the optimal (in the Chebyshev/minimax sense) FIR filter inpulse reponse given a set of band edges, the desired reponse on those bands, and the weight given to the error in those bands.


Frequency is in the range [0, 1], with 1 being the Nyquist frequency (Fs/2)

Params: (order, bands, ampl, error_weight, filter_type, grid_density)"

%feature("docstring") gr_reverse "

Params: (taps)"

%feature("docstring") gr_sincos "

Params: (x, sin, cos)"

%feature("docstring") gr_sincosf "

Params: (x, sin, cos)"

%feature("docstring") gr_tmp_path "directory to create temporary files

Params: (NONE)"

%feature("docstring") gr_appdata_path "directory to store application data

Params: (NONE)"

%feature("docstring") get_unittest_path "

Params: (filename)"

%feature("docstring") gri_add_const_ss "Low-level, high-speed add_const_ss primitive.

copy src to dst adding konst

Params: (dst, src, nshorts, konst)"

%feature("docstring") gri_alsa_dump_hw_params "

Params: (pcm, hwparams, fp)"

%feature("docstring") gri_alsa_pick_acceptable_format "

Params: (pcm, hwparams, acceptable_formats, nacceptable_formats, selected_format, error_msg_tag, verbose)"

%feature("docstring") gri_char_to_float "

Params: (in, out, nsamples)"

%feature("docstring") gri_float_to_char "convert array of floats to chars with rounding and saturation.

Params: (in, out, nsamples)"

%feature("docstring") gri_float_to_short "convert array of floats to shorts with rounding and saturation.

Params: (in, out, nsamples)"

%feature("docstring") gri_float_to_uchar "convert array of floats to unsigned chars with rounding and saturation.

Params: (in, out, nsamples)"

%feature("docstring") gri_interleaved_short_to_complex "

Params: (in, out, nsamples)"

%feature("docstring") gri_pa_find_device_by_name "

Params: (name)"

%feature("docstring") gri_print_devices "

Params: (NONE)"

%feature("docstring") gri_short_to_float "

Params: (in, out, nsamples)"

%feature("docstring") gri_uchar_to_float "

Params: (in, out, nsamples)"

%feature("docstring") gri_wav_read_sample "Read one sample from an open WAV file at the current position.

Takes care of endianness.

Params: (fp, bytes_per_sample)"

%feature("docstring") gri_wavheader_write "Write a valid RIFF file header.

Note: Some header values are kept blank because they're usually not known a-priori (file and chunk lengths). Use gri_wavheader_complete() to fill these in.

Params: (fp, sample_rate, nchans, bytes_per_sample)"

%feature("docstring") gri_wav_write_sample "Write one sample to an open WAV file at the current position.

Takes care of endianness.

Params: (fp, sample, bytes_per_sample)"

%feature("docstring") gri_wavheader_complete "Complete a WAV header.

Note: The stream position is changed during this function. If anything needs to be written to the WAV file after calling this function (which shouldn't happen), you need to fseek() to the end of the file (or whereever).

 File pointer to an open WAV file with a blank header  Length of all samples written to the file in bytes.

Params: (fp, byte_count)"

%feature("docstring") gsm_fr_make_decode_ps "

Params: (NONE)"

%feature("docstring") gsm_fr_make_encode_sp "

Params: (NONE)"

%feature("docstring") make_i2c_bbio_pp "

Params: (pp)"

%feature("docstring") make_i2c_bitbang "

Params: (io)"

%feature("docstring") htonll "

Params: (x)"

%feature("docstring") ntohll "

Params: (x)"

%feature("docstring") ntohx "

Params: (x)"

%feature("docstring") htonx "

Params: (x)"

%feature("docstring") modnn "

Params: (rs, x)"

%feature("docstring") fft_1d_r2 "

Params: (out, in, W, log2_size)"

%feature("docstring") malloc16Align "

Params: (size)"

%feature("docstring") calloc16Align "

Params: (nmemb, size)"

%feature("docstring") free16Align "

Params: (p)"

%feature("docstring") md5_init_ctx "

Params: (ctx)"

%feature("docstring") md5_process_block "

Params: (buffer, len, ctx)"

%feature("docstring") md5_process_bytes "

Params: (buffer, len, ctx)"

%feature("docstring") md5_finish_ctx "

Params: (ctx, resbuf)"

%feature("docstring") md5_read_ctx "

Params: (ctx, resbuf)"

%feature("docstring") md5_stream "

Params: (stream, resblock)"

%feature("docstring") md5_buffer "

Params: (buffer, len, resblock)"

%feature("docstring") smp_mb "

Params: ()"

%feature("docstring") smp_rmb "

Params: ()"

%feature("docstring") smp_wmb "

Params: ()"

%feature("docstring") msdd_rs_make_source_simple "

Params: (src, port_src)"

%feature("docstring") pageri_bch3221 "

Params: (data)"

%feature("docstring") find_flex_mode "

Params: (sync_code)"

%feature("docstring") is_alphanumeric_page "

Params: (type)"

%feature("docstring") is_numeric_page "

Params: (type)"

%feature("docstring") is_tone_page "

Params: (type)"

%feature("docstring") pageri_reverse_bits8 "

Params: (val)"

%feature("docstring") pageri_reverse_bits32 "

Params: (val)"

%feature("docstring") posix_memalign "

Params: (memptr, alignment, size)"

%feature("docstring") make_ppio "Factory method.

Split out from class to make life easier for SWIG

Params: (which_pp)"

%feature("docstring") make_ppio_ppdev "

Params: (which)"

%feature("docstring") make_psk "

Params: (gr_constell)"

%feature("docstring") make_qam "

Params: (gr_constell)"

%feature("docstring") SWAP "

Params: (a, b)"

%feature("docstring") quicksort_index "

Params: (p, index, left, right)"

%feature("docstring") encode_rs_char "

Params: (rs, data, parity)"

%feature("docstring") decode_rs_char "

Params: (rs, data, eras_pos, no_eras)"

%feature("docstring") init_rs_char "

Params: (symsize, gfpoly, fcr, prim, nroots)"

%feature("docstring") free_rs_char "

Params: (rs)"

%feature("docstring") encode_rs_int "

Params: (rs, data, parity)"

%feature("docstring") decode_rs_int "

Params: (rs, data, eras_pos, no_eras)"

%feature("docstring") init_rs_int "

Params: (symsize, gfpoly, fcr, prim, nroots)"

%feature("docstring") free_rs_int "

Params: (rs)"

%feature("docstring") encode_rs_8 "

Params: (data, parity)"

%feature("docstring") decode_rs_8 "

Params: (data, eras_pos, no_eras)"

%feature("docstring") short_dotprod_generic "

Params: (input, taps, n_4_short_blocks)"

%feature("docstring") short_dotprod_mmx "

Params: (input, taps, n_4_short_blocks)"

%feature("docstring") short_dotprod_sse2 "

Params: (input, taps, n_4_short_blocks)"

%feature("docstring") qsimp "

Params: (func, a, b)"

%feature("docstring") strtod_si "convert string at s to double honoring any trailing SI suffixes

Params: (s, result)"

%feature("docstring") calc_metric "

Params: (O, D, TABLE, in, metric, type)"

%feature("docstring") uhd_make_multi_usrp_sink "

Params: (device_addr, io_type, num_channels)"

%feature("docstring") uhd_make_multi_usrp_source "

Params: (device_addr, io_type, num_channels)"

%feature("docstring") uhd_make_single_usrp_sink "

Params: (device_addr, io_type, num_channels)"

%feature("docstring") uhd_make_single_usrp_source "

Params: (device_addr, io_type, num_channels)"

%feature("docstring") operator<< "

Params: (os, x)"

%feature("docstring") bswap_16 "routines for convertering between host and usrp byte order

Prior to including this file, the user must include \"config.h\" which will or won't define WORDS_BIGENDIAN based on the result of the AC_C_BIGENDIAN autoconf test.

Params: (x)"

%feature("docstring") bswap_32 "

Params: (x)"

%feature("docstring") host_to_usrp_u32 "

Params: (x)"

%feature("docstring") usrp_to_host_u32 "

Params: (x)"

%feature("docstring") host_to_usrp_short "

Params: (x)"

%feature("docstring") usrp_to_host_short "

Params: (x)"

%feature("docstring") usrp_rx_config_stream_count "given a usrp_rx_config word, return the number of I & Q streams that are interleaved on the USB.

Params: (usrp_rx_config)"

%feature("docstring") usrp_tx_config_stream_count "given a usrp_tx_config word, return the number of I & Q streams that are interleaved on the USB.

Params: (usrp_tx_config)"

%feature("docstring") usrp_one_time_init "initialize libusb; Behavior differs depending on libusb version

libusb-0.12

Probe busses and devices. The argument is ignored and defaults to NULL. Safe to call more than once.

libusb-1.0

If an location to a libusb_context is passed in, create and write in the new context. If no argument is provided, initialize libusb with the default (NULL) context.

Generally _not_ safe to call more than once with non-NULL argument since a new context will be created each time.

Params: (ctx)"

%feature("docstring") usrp_deinit "deinitialize libusb

libusb-0.1: No effect

libusb-1.0: Deinitialize context ctx

Params: (ctx)"

%feature("docstring") usrp_rescan "

Params: (NONE)"

%feature("docstring") usrp_find_device "locate Nth (zero based) USRP device in system. Return pointer or 0 if not found.

The following kinds of devices are considered USRPs:

unconfigured USRP (no firwmare loaded) configured USRP (firmware loaded) unconfigured Cypress FX2 (only if fx2_ok_p is true)

Params: (nth, fx2_ok_p, ctx)"

%feature("docstring") usrp_usrp_p "

Params: (q)"

%feature("docstring") usrp_usrp0_p "

Params: (q)"

%feature("docstring") usrp_usrp1_p "

Params: (q)"

%feature("docstring") usrp_usrp2_p "

Params: (q)"

%feature("docstring") usrp_hw_rev "

Params: (q)"

%feature("docstring") usrp_fx2_p "

Params: (q)"

%feature("docstring") usrp_unconfigured_usrp_p "

Params: (q)"

%feature("docstring") usrp_configured_usrp_p "

Params: (q)"

%feature("docstring") usrp_open_cmd_interface "given a libusb_device return an instance of the appropriate libusb_device_handle

These routines claim the specified interface and select the correct alternate interface. (USB nomenclature is totally screwed!)

If interface can't be opened, or is already claimed by some other process, 0 is returned.

Params: (dev)"

%feature("docstring") usrp_open_rx_interface "

Params: (dev)"

%feature("docstring") usrp_open_tx_interface "

Params: (dev)"

%feature("docstring") usrp_close_interface "close interface.

Params: (udh)"

%feature("docstring") usrp_load_firmware "load intel hex format file into USRP/Cypress FX2 (8051).

The filename extension is typically *.ihx

Note that loading firmware may cause the device to renumerate. I.e., change its configuration, invalidating the current device handle.

Params: (udh, filename, force)"

%feature("docstring") usrp_load_firmware_nth "load intel hex format file into USRP FX2 (8051).

The filename extension is typically *.ihx

Note that loading firmware may cause the device to renumerate. I.e., change its configuration, invalidating the current device handle. If the result is ULS_OK, usrp_load_firmware_nth delays 1 second then rescans the busses and devices.

Params: (nth, filename, force, ctx)"

%feature("docstring") usrp_load_fpga "load fpga configuration bitstream

Params: (udh, filename, force)"

%feature("docstring") usrp_load_standard_bits "load the regular firmware and fpga bitstream in the Nth USRP.

This is the normal starting point...

Params: (nth, force, fpga_filename, firmware_filename, ctx)"

%feature("docstring") usrp_set_hash "copy the given  into the USRP hash slot . The usrp implements two hash slots, 0 and 1.

Params: (udh, which, hash)"

%feature("docstring") usrp_get_hash "retrieve the  from the USRP hash slot . The usrp implements two hash slots, 0 and 1.

Params: (udh, which, hash)"

%feature("docstring") usrp_write_fpga_reg "

Params: (udh, reg, value)"

%feature("docstring") usrp_read_fpga_reg "

Params: (udh, reg, value)"

%feature("docstring") usrp_set_fpga_reset "

Params: (udh, on)"

%feature("docstring") usrp_set_fpga_tx_enable "

Params: (udh, on)"

%feature("docstring") usrp_set_fpga_rx_enable "

Params: (udh, on)"

%feature("docstring") usrp_set_fpga_tx_reset "

Params: (udh, on)"

%feature("docstring") usrp_set_fpga_rx_reset "

Params: (udh, on)"

%feature("docstring") usrp_set_led "

Params: (udh, which, on)"

%feature("docstring") usrp_check_rx_overrun "

Params: (udh, overrun_p)"

%feature("docstring") usrp_check_tx_underrun "

Params: (udh, underrun_p)"

%feature("docstring") usrp_i2c_write "

Params: (udh, i2c_addr, buf, len)"

%feature("docstring") usrp_i2c_read "

Params: (udh, i2c_addr, buf, len)"

%feature("docstring") usrp_spi_write "

Params: (udh, optional_header, enables, format, buf, len)"

%feature("docstring") usrp_spi_read "

Params: (udh, optional_header, enables, format, buf, len)"

%feature("docstring") usrp_9862_write "

Params: (udh, which_codec, regno, value)"

%feature("docstring") usrp_9862_read "

Params: (udh, which_codec, regno, value)"

%feature("docstring") usrp_9862_write_many "Write multiple 9862 regs at once.

contains alternating register_number, register_value pairs.  must be even and is the length of buf in bytes.

Params: (udh, which_codec, buf, len)"

%feature("docstring") usrp_9862_write_many_all "write specified regs to all 9862's in the system

Params: (udh, buf, len)"

%feature("docstring") usrp_eeprom_write "

Params: (udh, i2c_addr, eeprom_offset, buf, len)"

%feature("docstring") usrp_eeprom_read "

Params: (udh, i2c_addr, eeprom_offset, buf, len)"

%feature("docstring") usrp_write_aux_dac "write to the specified aux dac.

which Tx or Rx slot to write. N.B., SLOT_TX_A and SLOT_RX_A share the same AUX DAC's SLOT_TX_B and SLOT_RX_B share the same AUX DAC's

 [0,3] RX slots must use only 0 and 1. TX slots must use only 2 and 3.

AUX DAC 3 is really the 9862 sigma delta output.

 to write to aux dac. All dacs take straight binary values. Although dacs 0, 1 and 2 are 8-bit and dac 3 is 12-bit, the interface is in terms of 12-bit values [0,4095]

Params: (uhd, slot, which_dac, value)"

%feature("docstring") usrp_read_aux_adc "Read the specified aux adc.

which Tx or Rx slot to read aux dac  [0,1] which of the two adcs to read : return value, 12-bit straight binary.

Params: (udh, slot, which_adc, value)"

%feature("docstring") usrp_dbid_to_string "usrp daughterboard id to name mapping

Params: (dbid)"

%feature("docstring") usrp_read_dboard_eeprom "Read and return parsed daughterboard eeprom.

Params: (udh, slot_id, eeprom)"

%feature("docstring") usrp_write_dboard_offsets "write ADC/DAC offset calibration constants to d'board eeprom

Params: (udh, slot_id, offset0, offset1)"

%feature("docstring") usrp_serial_number "return a usrp's serial number.

Note that this only works on a configured usrp.

Params: (udh)"

%feature("docstring") usrp_open_interface "

Params: (dev, interface, altinterface)"

%feature("docstring") write_cmd "

Params: (udh, request, value, index, bytes, len)"

%feature("docstring") _get_usb_device "

Params: (udh)"

%feature("docstring") _get_usb_device_descriptor "

Params: (q)"

%feature("docstring") _get_usb_string_descriptor "

Params: (udh, index, data, length)"

%feature("docstring") _usb_control_transfer "

Params: (udh, request_type, request, value, index, data, length, timeout)"

%feature("docstring") make_usrp_rx_cfile "

Params: (which, spec, decim, freq, gain, width8, nohb, output_shorts, nsamples, filename)"

%feature("docstring") str_to_subdev "

Params: (spec_str)"

%feature("docstring") make_usrp_siggen "

Params: (which, spec, rf_freq, interp, wfreq, waveform, amp, gain, offset, nsamples)"

%feature("docstring") video_sdl_make_sink_s "

Params: (framerate, width, height, format, dst_width, dst_height)"

%feature("docstring") video_sdl_make_sink_uc "

Params: (framerate, width, height, format, dst_width, dst_height)"

%feature("docstring") gen_met "

Params: (mettab, amp, esn0, bias, scale)"

%feature("docstring") encode "

Params: (symbols, data, nbytes, encstate)"

%feature("docstring") viterbi_chunks_init "

Params: (state)"

%feature("docstring") viterbi_butterfly2 "

Params: (symbols, mettab, state0, state1)"

%feature("docstring") viterbi_get_output "

Params: (state, outbuf)"

%feature("docstring") volk_16s_add_quad_aligned16 "

Params: (target0, target1, target2, target3, src0, src1, src2, src3, src4, num_bytes)"

%feature("docstring") volk_16s_add_quad_aligned16_manual "

Params: (target0, target1, target2, target3, src0, src1, src2, src3, src4, num_bytes, arch)"

%feature("docstring") volk_16s_branch_4_state_8_aligned16 "

Params: (target, src0, permuters, cntl2, cntl3, scalars)"

%feature("docstring") volk_16s_branch_4_state_8_aligned16_manual "

Params: (target, src0, permuters, cntl2, cntl3, scalars, arch)"

%feature("docstring") volk_16sc_deinterleave_16s_aligned16 "

Params: (iBuffer, qBuffer, complexVector, num_points)"

%feature("docstring") volk_16sc_deinterleave_16s_aligned16_manual "

Params: (iBuffer, qBuffer, complexVector, num_points, arch)"

%feature("docstring") volk_16sc_deinterleave_32f_aligned16 "

Params: (iBuffer, qBuffer, complexVector, scalar, num_points)"

%feature("docstring") volk_16sc_deinterleave_32f_aligned16_manual "

Params: (iBuffer, qBuffer, complexVector, scalar, num_points, arch)"

%feature("docstring") volk_16sc_deinterleave_real_16s_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") volk_16sc_deinterleave_real_16s_aligned16_manual "

Params: (iBuffer, complexVector, num_points, arch)"

%feature("docstring") volk_16sc_deinterleave_real_32f_aligned16 "

Params: (iBuffer, complexVector, scalar, num_points)"

%feature("docstring") volk_16sc_deinterleave_real_32f_aligned16_manual "

Params: (iBuffer, complexVector, scalar, num_points, arch)"

%feature("docstring") volk_16sc_deinterleave_real_8s_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") volk_16sc_deinterleave_real_8s_aligned16_manual "

Params: (iBuffer, complexVector, num_points, arch)"

%feature("docstring") volk_16sc_magnitude_16s_aligned16 "

Params: (magnitudeVector, complexVector, num_points)"

%feature("docstring") volk_16sc_magnitude_16s_aligned16_manual "

Params: (magnitudeVector, complexVector, num_points, arch)"

%feature("docstring") volk_16sc_magnitude_32f_aligned16 "

Params: (magnitudeVector, complexVector, scalar, num_points)"

%feature("docstring") volk_16sc_magnitude_32f_aligned16_manual "

Params: (magnitudeVector, complexVector, scalar, num_points, arch)"

%feature("docstring") volk_16s_convert_32f_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_16s_convert_32f_aligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") volk_16s_convert_32f_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_16s_convert_32f_unaligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") volk_16s_convert_8s_aligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") volk_16s_convert_8s_aligned16_manual "

Params: (outputVector, inputVector, num_points, arch)"

%feature("docstring") volk_16s_convert_8s_unaligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") volk_16s_convert_8s_unaligned16_manual "

Params: (outputVector, inputVector, num_points, arch)"

%feature("docstring") volk_16s_max_star_aligned16 "

Params: (target, src0, num_bytes)"

%feature("docstring") volk_16s_max_star_aligned16_manual "

Params: (target, src0, num_bytes, arch)"

%feature("docstring") volk_16s_max_star_horizontal_aligned16 "

Params: (target, src0, num_bytes)"

%feature("docstring") volk_16s_max_star_horizontal_aligned16_manual "

Params: (target, src0, num_bytes, arch)"

%feature("docstring") volk_16s_permute_and_scalar_add_aligned16 "

Params: (target, src0, permute_indexes, cntl0, cntl1, cntl2, cntl3, scalars, num_bytes)"

%feature("docstring") volk_16s_permute_and_scalar_add_aligned16_manual "

Params: (target, src0, permute_indexes, cntl0, cntl1, cntl2, cntl3, scalars, num_bytes, arch)"

%feature("docstring") volk_16s_quad_max_star_aligned16 "

Params: (target, src0, src1, src2, src3, num_bytes)"

%feature("docstring") volk_16s_quad_max_star_aligned16_manual "

Params: (target, src0, src1, src2, src3, num_bytes, arch)"

%feature("docstring") volk_16u_byteswap_aligned16 "

Params: (intsToSwap, num_points)"

%feature("docstring") volk_16u_byteswap_aligned16_manual "

Params: (intsToSwap, num_points, arch)"

%feature("docstring") volk_32f_accumulator_aligned16 "

Params: (result, inputBuffer, num_points)"

%feature("docstring") volk_32f_accumulator_aligned16_manual "

Params: (result, inputBuffer, num_points, arch)"

%feature("docstring") volk_32f_add_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_32f_add_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_32fc_32f_multiply_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_32fc_32f_multiply_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_32fc_32f_power_32fc_aligned16 "

Params: (cVector, aVector, power, num_points)"

%feature("docstring") volk_32fc_32f_power_32fc_aligned16_manual "

Params: (cVector, aVector, power, num_points, arch)"

%feature("docstring") volk_32f_calc_spectral_noise_floor_aligned16 "

Params: (noiseFloorAmplitude, realDataPoints, spectralExclusionValue, num_points)"

%feature("docstring") volk_32f_calc_spectral_noise_floor_aligned16_manual "

Params: (noiseFloorAmplitude, realDataPoints, spectralExclusionValue, num_points, arch)"

%feature("docstring") volk_32fc_atan2_32f_aligned16 "

Params: (outputVector, inputVector, normalizeFactor, num_points)"

%feature("docstring") volk_32fc_atan2_32f_aligned16_manual "

Params: (outputVector, inputVector, normalizeFactor, num_points, arch)"

%feature("docstring") volk_32fc_conjugate_dot_prod_aligned16 "

Params: (result, input, taps, num_bytes)"

%feature("docstring") volk_32fc_conjugate_dot_prod_aligned16_manual "

Params: (result, input, taps, num_bytes, arch)"

%feature("docstring") volk_32fc_conjugate_dot_prod_unaligned "

Params: (result, input, taps, num_bytes)"

%feature("docstring") volk_32fc_conjugate_dot_prod_unaligned_manual "

Params: (result, input, taps, num_bytes, arch)"

%feature("docstring") volk_32fc_deinterleave_32f_aligned16 "

Params: (iBuffer, qBuffer, complexVector, num_points)"

%feature("docstring") volk_32fc_deinterleave_32f_aligned16_manual "

Params: (iBuffer, qBuffer, complexVector, num_points, arch)"

%feature("docstring") volk_32fc_deinterleave_64f_aligned16 "

Params: (iBuffer, qBuffer, complexVector, num_points)"

%feature("docstring") volk_32fc_deinterleave_64f_aligned16_manual "

Params: (iBuffer, qBuffer, complexVector, num_points, arch)"

%feature("docstring") volk_32fc_deinterleave_real_16s_aligned16 "

Params: (iBuffer, complexVector, scalar, num_points)"

%feature("docstring") volk_32fc_deinterleave_real_16s_aligned16_manual "

Params: (iBuffer, complexVector, scalar, num_points, arch)"

%feature("docstring") volk_32fc_deinterleave_real_32f_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") volk_32fc_deinterleave_real_32f_aligned16_manual "

Params: (iBuffer, complexVector, num_points, arch)"

%feature("docstring") volk_32fc_deinterleave_real_64f_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") volk_32fc_deinterleave_real_64f_aligned16_manual "

Params: (iBuffer, complexVector, num_points, arch)"

%feature("docstring") volk_32fc_dot_prod_aligned16 "

Params: (result, input, taps, num_bytes)"

%feature("docstring") volk_32fc_dot_prod_aligned16_manual "

Params: (result, input, taps, num_bytes, arch)"

%feature("docstring") volk_32fc_index_max_aligned16 "

Params: (target, src0, num_bytes)"

%feature("docstring") volk_32fc_index_max_aligned16_manual "

Params: (target, src0, num_bytes, arch)"

%feature("docstring") volk_32fc_magnitude_16s_aligned16 "

Params: (magnitudeVector, complexVector, scalar, num_points)"

%feature("docstring") volk_32fc_magnitude_16s_aligned16_manual "

Params: (magnitudeVector, complexVector, scalar, num_points, arch)"

%feature("docstring") volk_32fc_magnitude_32f_aligned16 "

Params: (magnitudeVector, complexVector, num_points)"

%feature("docstring") volk_32fc_magnitude_32f_aligned16_manual "

Params: (magnitudeVector, complexVector, num_points, arch)"

%feature("docstring") volk_32fc_multiply_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_32fc_multiply_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_32f_convert_16s_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_32f_convert_16s_aligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") volk_32f_convert_16s_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_32f_convert_16s_unaligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") volk_32f_convert_32s_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_32f_convert_32s_aligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") volk_32f_convert_32s_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_32f_convert_32s_unaligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") volk_32f_convert_64f_aligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") volk_32f_convert_64f_aligned16_manual "

Params: (outputVector, inputVector, num_points, arch)"

%feature("docstring") volk_32f_convert_64f_unaligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") volk_32f_convert_64f_unaligned16_manual "

Params: (outputVector, inputVector, num_points, arch)"

%feature("docstring") volk_32f_convert_8s_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_32f_convert_8s_aligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") volk_32f_convert_8s_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_32f_convert_8s_unaligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") volk_32fc_power_spectral_density_32f_aligned16 "

Params: (logPowerOutput, complexFFTInput, normalizationFactor, rbw, num_points)"

%feature("docstring") volk_32fc_power_spectral_density_32f_aligned16_manual "

Params: (logPowerOutput, complexFFTInput, normalizationFactor, rbw, num_points, arch)"

%feature("docstring") volk_32fc_power_spectrum_32f_aligned16 "

Params: (logPowerOutput, complexFFTInput, normalizationFactor, num_points)"

%feature("docstring") volk_32fc_power_spectrum_32f_aligned16_manual "

Params: (logPowerOutput, complexFFTInput, normalizationFactor, num_points, arch)"

%feature("docstring") volk_32fc_square_dist_aligned16 "

Params: (target, src0, points, num_bytes)"

%feature("docstring") volk_32fc_square_dist_aligned16_manual "

Params: (target, src0, points, num_bytes, arch)"

%feature("docstring") volk_32fc_square_dist_scalar_mult_aligned16 "

Params: (target, src0, points, scalar, num_bytes)"

%feature("docstring") volk_32fc_square_dist_scalar_mult_aligned16_manual "

Params: (target, src0, points, scalar, num_bytes, arch)"

%feature("docstring") volk_32f_divide_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_32f_divide_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_32f_dot_prod_aligned16 "

Params: (result, input, taps, num_points)"

%feature("docstring") volk_32f_dot_prod_aligned16_manual "

Params: (result, input, taps, num_points, arch)"

%feature("docstring") volk_32f_dot_prod_unaligned16 "

Params: (result, input, taps, num_points)"

%feature("docstring") volk_32f_dot_prod_unaligned16_manual "

Params: (result, input, taps, num_points, arch)"

%feature("docstring") volk_32f_fm_detect_aligned16 "

Params: (outputVector, inputVector, bound, saveValue, num_points)"

%feature("docstring") volk_32f_fm_detect_aligned16_manual "

Params: (outputVector, inputVector, bound, saveValue, num_points, arch)"

%feature("docstring") volk_32f_index_max_aligned16 "

Params: (target, src0, num_points)"

%feature("docstring") volk_32f_index_max_aligned16_manual "

Params: (target, src0, num_points, arch)"

%feature("docstring") volk_32f_interleave_16sc_aligned16 "

Params: (complexVector, iBuffer, qBuffer, scalar, num_points)"

%feature("docstring") volk_32f_interleave_16sc_aligned16_manual "

Params: (complexVector, iBuffer, qBuffer, scalar, num_points, arch)"

%feature("docstring") volk_32f_interleave_32fc_aligned16 "

Params: (complexVector, iBuffer, qBuffer, num_points)"

%feature("docstring") volk_32f_interleave_32fc_aligned16_manual "

Params: (complexVector, iBuffer, qBuffer, num_points, arch)"

%feature("docstring") volk_32f_max_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_32f_max_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_32f_min_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_32f_min_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_32f_multiply_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_32f_multiply_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_32f_normalize_aligned16 "

Params: (vecBuffer, scalar, num_points)"

%feature("docstring") volk_32f_normalize_aligned16_manual "

Params: (vecBuffer, scalar, num_points, arch)"

%feature("docstring") volk_32f_power_aligned16 "

Params: (cVector, aVector, power, num_points)"

%feature("docstring") volk_32f_power_aligned16_manual "

Params: (cVector, aVector, power, num_points, arch)"

%feature("docstring") volk_32f_sqrt_aligned16 "

Params: (cVector, aVector, num_points)"

%feature("docstring") volk_32f_sqrt_aligned16_manual "

Params: (cVector, aVector, num_points, arch)"

%feature("docstring") volk_32f_stddev_aligned16 "

Params: (stddev, inputBuffer, mean, num_points)"

%feature("docstring") volk_32f_stddev_aligned16_manual "

Params: (stddev, inputBuffer, mean, num_points, arch)"

%feature("docstring") volk_32f_stddev_and_mean_aligned16 "

Params: (stddev, mean, inputBuffer, num_points)"

%feature("docstring") volk_32f_stddev_and_mean_aligned16_manual "

Params: (stddev, mean, inputBuffer, num_points, arch)"

%feature("docstring") volk_32f_subtract_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_32f_subtract_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_32f_sum_of_poly_aligned16 "

Params: (target, src0, center_point_array, cutoff, num_bytes)"

%feature("docstring") volk_32f_sum_of_poly_aligned16_manual "

Params: (target, src0, center_point_array, cutoff, num_bytes, arch)"

%feature("docstring") volk_32s_and_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_32s_and_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_32s_convert_32f_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_32s_convert_32f_aligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") volk_32s_convert_32f_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_32s_convert_32f_unaligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") volk_32s_or_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_32s_or_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_32u_byteswap_aligned16 "

Params: (intsToSwap, num_points)"

%feature("docstring") volk_32u_byteswap_aligned16_manual "

Params: (intsToSwap, num_points, arch)"

%feature("docstring") volk_32u_popcnt_aligned16 "

Params: (ret, value)"

%feature("docstring") volk_32u_popcnt_aligned16_manual "

Params: (ret, value, arch)"

%feature("docstring") volk_64f_convert_32f_aligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") volk_64f_convert_32f_aligned16_manual "

Params: (outputVector, inputVector, num_points, arch)"

%feature("docstring") volk_64f_convert_32f_unaligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") volk_64f_convert_32f_unaligned16_manual "

Params: (outputVector, inputVector, num_points, arch)"

%feature("docstring") volk_64f_max_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_64f_max_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_64f_min_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_64f_min_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_64u_byteswap_aligned16 "

Params: (intsToSwap, num_points)"

%feature("docstring") volk_64u_byteswap_aligned16_manual "

Params: (intsToSwap, num_points, arch)"

%feature("docstring") volk_64u_popcnt_aligned16 "

Params: (ret, value)"

%feature("docstring") volk_64u_popcnt_aligned16_manual "

Params: (ret, value, arch)"

%feature("docstring") volk_8sc_deinterleave_16s_aligned16 "

Params: (iBuffer, qBuffer, complexVector, num_points)"

%feature("docstring") volk_8sc_deinterleave_16s_aligned16_manual "

Params: (iBuffer, qBuffer, complexVector, num_points, arch)"

%feature("docstring") volk_8sc_deinterleave_32f_aligned16 "

Params: (iBuffer, qBuffer, complexVector, scalar, num_points)"

%feature("docstring") volk_8sc_deinterleave_32f_aligned16_manual "

Params: (iBuffer, qBuffer, complexVector, scalar, num_points, arch)"

%feature("docstring") volk_8sc_deinterleave_real_16s_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") volk_8sc_deinterleave_real_16s_aligned16_manual "

Params: (iBuffer, complexVector, num_points, arch)"

%feature("docstring") volk_8sc_deinterleave_real_32f_aligned16 "

Params: (iBuffer, complexVector, scalar, num_points)"

%feature("docstring") volk_8sc_deinterleave_real_32f_aligned16_manual "

Params: (iBuffer, complexVector, scalar, num_points, arch)"

%feature("docstring") volk_8sc_deinterleave_real_8s_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") volk_8sc_deinterleave_real_8s_aligned16_manual "

Params: (iBuffer, complexVector, num_points, arch)"

%feature("docstring") volk_8sc_multiply_conjugate_16sc_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") volk_8sc_multiply_conjugate_16sc_aligned16_manual "

Params: (cVector, aVector, bVector, num_points, arch)"

%feature("docstring") volk_8sc_multiply_conjugate_32fc_aligned16 "

Params: (cVector, aVector, bVector, scalar, num_points)"

%feature("docstring") volk_8sc_multiply_conjugate_32fc_aligned16_manual "

Params: (cVector, aVector, bVector, scalar, num_points, arch)"

%feature("docstring") volk_8s_convert_16s_aligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") volk_8s_convert_16s_aligned16_manual "

Params: (outputVector, inputVector, num_points, arch)"

%feature("docstring") volk_8s_convert_16s_unaligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") volk_8s_convert_16s_unaligned16_manual "

Params: (outputVector, inputVector, num_points, arch)"

%feature("docstring") volk_8s_convert_32f_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_8s_convert_32f_aligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") volk_8s_convert_32f_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_8s_convert_32f_unaligned16_manual "

Params: (outputVector, inputVector, scalar, num_points, arch)"

%feature("docstring") lv_creal "

Params: (x)"

%feature("docstring") lv_cimag "

Params: (x)"

%feature("docstring") lv_conj "

Params: (x)"

%feature("docstring") lv_cpow "

Params: (x, y)"

%feature("docstring") lv_32fc_init "

Params: (x, y)"

%feature("docstring") volk_cpu_init "

Params: (NONE)"

%feature("docstring") volk_get_lvarch "

Params: (NONE)"

%feature("docstring") volk_environment_init "

Params: (NONE)"

%feature("docstring") default_acquire_volk_16s_add_quad_aligned16 "

Params: (target0, target1, target2, target3, src0, src1, src2, src3, src4, num_bytes)"

%feature("docstring") default_acquire_volk_16s_branch_4_state_8_aligned16 "

Params: (target, src0, permuters, cntl2, cntl3, scalars)"

%feature("docstring") default_acquire_volk_16sc_deinterleave_16s_aligned16 "

Params: (iBuffer, qBuffer, complexVector, num_points)"

%feature("docstring") default_acquire_volk_16sc_deinterleave_32f_aligned16 "

Params: (iBuffer, qBuffer, complexVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_16sc_deinterleave_real_16s_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") default_acquire_volk_16sc_deinterleave_real_32f_aligned16 "

Params: (iBuffer, complexVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_16sc_deinterleave_real_8s_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") default_acquire_volk_16sc_magnitude_16s_aligned16 "

Params: (magnitudeVector, complexVector, num_points)"

%feature("docstring") default_acquire_volk_16sc_magnitude_32f_aligned16 "

Params: (magnitudeVector, complexVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_16s_convert_32f_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_16s_convert_32f_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_16s_convert_8s_aligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") default_acquire_volk_16s_convert_8s_unaligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") default_acquire_volk_16s_max_star_aligned16 "

Params: (target, src0, num_bytes)"

%feature("docstring") default_acquire_volk_16s_max_star_horizontal_aligned16 "

Params: (target, src0, num_bytes)"

%feature("docstring") default_acquire_volk_16s_permute_and_scalar_add_aligned16 "

Params: (target, src0, permute_indexes, cntl0, cntl1, cntl2, cntl3, scalars, num_bytes)"

%feature("docstring") default_acquire_volk_16s_quad_max_star_aligned16 "

Params: (target, src0, src1, src2, src3, num_bytes)"

%feature("docstring") default_acquire_volk_16u_byteswap_aligned16 "

Params: (intsToSwap, num_points)"

%feature("docstring") default_acquire_volk_32f_accumulator_aligned16 "

Params: (result, inputBuffer, num_points)"

%feature("docstring") default_acquire_volk_32f_add_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_32fc_32f_multiply_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_32fc_32f_power_32fc_aligned16 "

Params: (cVector, aVector, power, num_points)"

%feature("docstring") default_acquire_volk_32f_calc_spectral_noise_floor_aligned16 "

Params: (noiseFloorAmplitude, realDataPoints, spectralExclusionValue, num_points)"

%feature("docstring") default_acquire_volk_32fc_atan2_32f_aligned16 "

Params: (outputVector, inputVector, normalizeFactor, num_points)"

%feature("docstring") default_acquire_volk_32fc_conjugate_dot_prod_aligned16 "

Params: (result, input, taps, num_bytes)"

%feature("docstring") default_acquire_volk_32fc_conjugate_dot_prod_unaligned "

Params: (result, input, taps, num_bytes)"

%feature("docstring") default_acquire_volk_32fc_deinterleave_32f_aligned16 "

Params: (iBuffer, qBuffer, complexVector, num_points)"

%feature("docstring") default_acquire_volk_32fc_deinterleave_64f_aligned16 "

Params: (iBuffer, qBuffer, complexVector, num_points)"

%feature("docstring") default_acquire_volk_32fc_deinterleave_real_16s_aligned16 "

Params: (iBuffer, complexVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_32fc_deinterleave_real_32f_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") default_acquire_volk_32fc_deinterleave_real_64f_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") default_acquire_volk_32fc_dot_prod_aligned16 "

Params: (result, input, taps, num_bytes)"

%feature("docstring") default_acquire_volk_32fc_index_max_aligned16 "

Params: (target, src0, num_bytes)"

%feature("docstring") default_acquire_volk_32fc_magnitude_16s_aligned16 "

Params: (magnitudeVector, complexVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_32fc_magnitude_32f_aligned16 "

Params: (magnitudeVector, complexVector, num_points)"

%feature("docstring") default_acquire_volk_32fc_multiply_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_32f_convert_16s_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_32f_convert_16s_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_32f_convert_32s_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_32f_convert_32s_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_32f_convert_64f_aligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") default_acquire_volk_32f_convert_64f_unaligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") default_acquire_volk_32f_convert_8s_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_32f_convert_8s_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_32fc_power_spectral_density_32f_aligned16 "

Params: (logPowerOutput, complexFFTInput, normalizationFactor, rbw, num_points)"

%feature("docstring") default_acquire_volk_32fc_power_spectrum_32f_aligned16 "

Params: (logPowerOutput, complexFFTInput, normalizationFactor, num_points)"

%feature("docstring") default_acquire_volk_32fc_square_dist_aligned16 "

Params: (target, src0, points, num_bytes)"

%feature("docstring") default_acquire_volk_32fc_square_dist_scalar_mult_aligned16 "

Params: (target, src0, points, scalar, num_bytes)"

%feature("docstring") default_acquire_volk_32f_divide_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_32f_dot_prod_aligned16 "

Params: (result, input, taps, num_points)"

%feature("docstring") default_acquire_volk_32f_dot_prod_unaligned16 "

Params: (result, input, taps, num_points)"

%feature("docstring") default_acquire_volk_32f_fm_detect_aligned16 "

Params: (outputVector, inputVector, bound, saveValue, num_points)"

%feature("docstring") default_acquire_volk_32f_index_max_aligned16 "

Params: (target, src0, num_points)"

%feature("docstring") default_acquire_volk_32f_interleave_16sc_aligned16 "

Params: (complexVector, iBuffer, qBuffer, scalar, num_points)"

%feature("docstring") default_acquire_volk_32f_interleave_32fc_aligned16 "

Params: (complexVector, iBuffer, qBuffer, num_points)"

%feature("docstring") default_acquire_volk_32f_max_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_32f_min_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_32f_multiply_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_32f_normalize_aligned16 "

Params: (vecBuffer, scalar, num_points)"

%feature("docstring") default_acquire_volk_32f_power_aligned16 "

Params: (cVector, aVector, power, num_points)"

%feature("docstring") default_acquire_volk_32f_sqrt_aligned16 "

Params: (cVector, aVector, num_points)"

%feature("docstring") default_acquire_volk_32f_stddev_aligned16 "

Params: (stddev, inputBuffer, mean, num_points)"

%feature("docstring") default_acquire_volk_32f_stddev_and_mean_aligned16 "

Params: (stddev, mean, inputBuffer, num_points)"

%feature("docstring") default_acquire_volk_32f_subtract_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_32f_sum_of_poly_aligned16 "

Params: (target, src0, center_point_array, cutoff, num_bytes)"

%feature("docstring") default_acquire_volk_32s_and_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_32s_convert_32f_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_32s_convert_32f_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_32s_or_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_32u_byteswap_aligned16 "

Params: (intsToSwap, num_points)"

%feature("docstring") default_acquire_volk_32u_popcnt_aligned16 "

Params: (ret, value)"

%feature("docstring") default_acquire_volk_64f_convert_32f_aligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") default_acquire_volk_64f_convert_32f_unaligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") default_acquire_volk_64f_max_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_64f_min_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_64u_byteswap_aligned16 "

Params: (intsToSwap, num_points)"

%feature("docstring") default_acquire_volk_64u_popcnt_aligned16 "

Params: (ret, value)"

%feature("docstring") default_acquire_volk_8sc_deinterleave_16s_aligned16 "

Params: (iBuffer, qBuffer, complexVector, num_points)"

%feature("docstring") default_acquire_volk_8sc_deinterleave_32f_aligned16 "

Params: (iBuffer, qBuffer, complexVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_8sc_deinterleave_real_16s_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") default_acquire_volk_8sc_deinterleave_real_32f_aligned16 "

Params: (iBuffer, complexVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_8sc_deinterleave_real_8s_aligned16 "

Params: (iBuffer, complexVector, num_points)"

%feature("docstring") default_acquire_volk_8sc_multiply_conjugate_16sc_aligned16 "

Params: (cVector, aVector, bVector, num_points)"

%feature("docstring") default_acquire_volk_8sc_multiply_conjugate_32fc_aligned16 "

Params: (cVector, aVector, bVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_8s_convert_16s_aligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") default_acquire_volk_8s_convert_16s_unaligned16 "

Params: (outputVector, inputVector, num_points)"

%feature("docstring") default_acquire_volk_8s_convert_32f_aligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") default_acquire_volk_8s_convert_32f_unaligned16 "

Params: (outputVector, inputVector, scalar, num_points)"

%feature("docstring") volk_rank_archs "

Params: (arch_defs, arch)"

%feature("docstring") get_volk_runtime "

Params: (NONE)"

%feature("docstring") volk_runtime_init "

Params: (NONE)"

%feature("docstring") scm_xyzzy_init "

Params: ()"

%feature("docstring") xyzzy_init "

Params: (filename)"

%feature("docstring") xyzzy_file_exists "

Params: (filename)"

%feature("docstring") xyzzy_open_file "

Params: (filename)"



%feature("docstring") _1200_common::_1200_common "

Params: (NONE)"

%feature("docstring") _1200_common::~_1200_common "

Params: (NONE)"

%feature("docstring") _1200_common::freq_min "

Params: (NONE)"

%feature("docstring") _1200_common::freq_max "

Params: (NONE)"



%feature("docstring") _1800_common::_1800_common "

Params: (NONE)"

%feature("docstring") _1800_common::~_1800_common "

Params: (NONE)"

%feature("docstring") _1800_common::freq_min "

Params: (NONE)"

%feature("docstring") _1800_common::freq_max "

Params: (NONE)"



%feature("docstring") _2200_common::_2200_common "

Params: (NONE)"

%feature("docstring") _2200_common::~_2200_common "

Params: (NONE)"

%feature("docstring") _2200_common::freq_min "

Params: (NONE)"

%feature("docstring") _2200_common::freq_max "

Params: (NONE)"



%feature("docstring") _2400_common::_2400_common "

Params: (NONE)"

%feature("docstring") _2400_common::~_2400_common "

Params: (NONE)"

%feature("docstring") _2400_common::freq_min "

Params: (NONE)"

%feature("docstring") _2400_common::freq_max "

Params: (NONE)"



%feature("docstring") _400_common::_400_common "

Params: (NONE)"

%feature("docstring") _400_common::~_400_common "

Params: (NONE)"

%feature("docstring") _400_common::freq_min "

Params: (NONE)"

%feature("docstring") _400_common::freq_max "

Params: (NONE)"



%feature("docstring") _400_rx::_400_rx "

Params: (NONE)"

%feature("docstring") _400_rx::~_400_rx "

Params: (NONE)"



%feature("docstring") _400_tx::_400_tx "

Params: (NONE)"

%feature("docstring") _400_tx::~_400_tx "

Params: (NONE)"



%feature("docstring") _900_common::_900_common "

Params: (NONE)"

%feature("docstring") _900_common::~_900_common "

Params: (NONE)"

%feature("docstring") _900_common::freq_min "

Params: (NONE)"

%feature("docstring") _900_common::freq_max "

Params: (NONE)"



%feature("docstring") _AD4360_common::_AD4360_common "

Params: (NONE)"

%feature("docstring") _AD4360_common::~_AD4360_common "

Params: (NONE)"

%feature("docstring") _AD4360_common::freq_min "

Params: (NONE)"

%feature("docstring") _AD4360_common::freq_max "

Params: (NONE)"

%feature("docstring") _AD4360_common::_compute_regs "

Params: (refclk_freq, freq, retR, retcontrol, retN, retfreq)"

%feature("docstring") _AD4360_common::_compute_control_reg "

Params: (NONE)"

%feature("docstring") _AD4360_common::_refclk_divisor "

Params: (NONE)"

%feature("docstring") _AD4360_common::_prescaler "

Params: (NONE)"

%feature("docstring") _AD4360_common::R_DIV "

Params: (div)"



%feature("docstring") _ADF410X_common::_ADF410X_common "

Params: (NONE)"

%feature("docstring") _ADF410X_common::~_ADF410X_common "

Params: (NONE)"

%feature("docstring") _ADF410X_common::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"

%feature("docstring") _ADF410X_common::_write_all "

Params: (R, N, control)"

%feature("docstring") _ADF410X_common::_write_R "

Params: (R)"

%feature("docstring") _ADF410X_common::_write_N "

Params: (N)"

%feature("docstring") _ADF410X_common::_write_func "

Params: (func)"

%feature("docstring") _ADF410X_common::_write_init "

Params: (init)"

%feature("docstring") _ADF410X_common::_prescaler "

Params: (NONE)"

%feature("docstring") _ADF410X_common::_write_it "

Params: (v)"

%feature("docstring") _ADF410X_common::_refclk_freq "

Params: (NONE)"

%feature("docstring") _ADF410X_common::_rx_write_io "

Params: (value, mask)"

%feature("docstring") _ADF410X_common::_lock_detect "

Params: (NONE)"

%feature("docstring") _ADF410X_common::usrp "

Params: (NONE)"



%feature("docstring") _lo_common::_lo_common "

Params: (NONE)"

%feature("docstring") _lo_common::~_lo_common "

Params: (NONE)"

%feature("docstring") _lo_common::freq_min "

Params: (NONE)"

%feature("docstring") _lo_common::freq_max "

Params: (NONE)"

%feature("docstring") _lo_common::set_divider "

Params: (main_or_aux, divisor)"

%feature("docstring") _lo_common::set_freq "

Params: (freq)"

%feature("docstring") Aadvark "Models the mammal Aadvark.

Sadly the model is incomplete and cannot capture all aspects of an aadvark yet.

This line is uninformative and is only to test line breaks in the comments."

%feature("docstring") Aadvark::print "Outputs the vital aadvark statistics.

Params: (NONE)"

%feature("docstring") Aadvark::Aadvark "

Params: (aaness)"

%feature("docstring") Aadvark::get_aadvarkness "

Params: (NONE)"



%feature("docstring") adf4350::adf4350 "

Params: (NONE)"

%feature("docstring") adf4350::~adf4350 "

Params: (NONE)"

%feature("docstring") adf4350::_update "

Params: (NONE)"

%feature("docstring") adf4350::compute_register "

Params: (addr)"

%feature("docstring") adf4350::_set_freq "

Params: (freq, refclock_freq)"

%feature("docstring") adf4350::_get_freq "

Params: (refclock_freq)"

%feature("docstring") adf4350::_get_max_freq "

Params: (NONE)"

%feature("docstring") adf4350::_get_min_freq "

Params: (NONE)"



%feature("docstring") adf4350_regs::adf4350_regs "

Params: (NONE)"

%feature("docstring") adf4350_regs::~adf4350_regs "

Params: (NONE)"

%feature("docstring") adf4350_regs::_reg_shift "

Params: (data, shift)"

%feature("docstring") adf4350_regs::compute_register "

Params: (addr)"



%feature("docstring") std::allocator "STL class."

%feature("docstring") atsc_data_segment "contains 832 3 bit symbols. The low 3 bits in the byte hold the symbol."

%feature("docstring") atsc_data_segment::operator== "

Params: (other)"

%feature("docstring") atsc_data_segment::operator!= "

Params: (other)"



%feature("docstring") atsc_mpeg_packet::operator== "

Params: (other)"

%feature("docstring") atsc_mpeg_packet::operator!= "

Params: (other)"



%feature("docstring") atsc_mpeg_packet_no_sync::operator== "

Params: (other)"

%feature("docstring") atsc_mpeg_packet_no_sync::operator!= "

Params: (other)"



%feature("docstring") atsc_mpeg_packet_rs_encoded::operator== "

Params: (other)"

%feature("docstring") atsc_mpeg_packet_rs_encoded::operator!= "

Params: (other)"



%feature("docstring") atsc_root_raised_cosine::taps "

Params: (sampling_freq)"



%feature("docstring") atsc_root_raised_cosine_bandpass::atsc_root_raised_cosine_bandpass "

Params: (center_freq)"

%feature("docstring") atsc_root_raised_cosine_bandpass::taps "

Params: (sampling_freq)"

%feature("docstring") atsc_soft_data_segment "Contains 832 bipolar floating point symbols. Nominal values are +/- {1, 3, 5, 7}. This data type represents the input to the viterbi decoder."

%feature("docstring") atsc_soft_data_segment::operator== "

Params: (other)"

%feature("docstring") atsc_soft_data_segment::operator!= "

Params: (other)"



%feature("docstring") atsc_vsbtx_lp::taps "

Params: (sampling_freq)"

%feature("docstring") atsci_basic_trellis_encoder "ATSC trellis encoder building block.

Note this is NOT the 12x interleaved interface.

This implements a single instance of the ATSC trellis encoder. This is a rate 2/3 encoder (really a constraint length 3, rate 1/2 encoder with the top bit passed through unencoded. This does not implement the \"precoding\" of the top bit, because the NTSC rejection filter is not supported."

%feature("docstring") atsci_basic_trellis_encoder::atsci_basic_trellis_encoder "

Params: (NONE)"

%feature("docstring") atsci_basic_trellis_encoder::encode "Encode two bit INPUT into 3 bit return value. Domain is [0,3], Range is [0,7]. The mapping to bipolar levels is not done.

Params: (input)"

%feature("docstring") atsci_basic_trellis_encoder::reset "reset encoder state

Params: (NONE)"

%feature("docstring") atsci_data_deinterleaver "atsc convolutional data deinterleaver"

%feature("docstring") atsci_data_deinterleaver::atsci_data_deinterleaver "

Params: (NONE)"

%feature("docstring") atsci_data_deinterleaver::deinterleave "

Params: (out, in)"

%feature("docstring") atsci_data_deinterleaver::remap_pli "

Params: (out, in)"

%feature("docstring") atsci_data_interleaver "atsc convolutional data interleaver"

%feature("docstring") atsci_data_interleaver::atsci_data_interleaver "

Params: (NONE)"

%feature("docstring") atsci_data_interleaver::interleave "

Params: (out, in)"

%feature("docstring") atsci_equalizer "abstract base class for ATSC equalizer"

%feature("docstring") atsci_equalizer::atsci_equalizer "

Params: (NONE)"

%feature("docstring") atsci_equalizer::~atsci_equalizer "

Params: (NONE)"

%feature("docstring") atsci_equalizer::reset "reset state (e.g., on channel change)

Note, subclasses must invoke the superclass's method too!

Params: (NONE)"

%feature("docstring") atsci_equalizer::filter "produce  of output from given inputs and tags

This is the main entry point. It examines the input_tags and local state and invokes the appropriate virtual function to handle each sub-segment of the input data.

 must have (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] are referenced to compute the output values.

 must have nsamples valid entries. input_tags[0] .. input_tags[nsamples - 1] are referenced to compute the output values.

Params: (input_samples, input_tags, output_samples, nsamples)"

%feature("docstring") atsci_equalizer::ntaps "how much history the input data stream requires.

This must return a value >= 1. Think of this as the number of samples you need to look at to compute a single output sample.

Params: (NONE)"

%feature("docstring") atsci_equalizer::npretaps "how many taps are \"in the future\".

This allows us to handle what the ATSC folks call \"pre-ghosts\". What it really does is allow the caller to jack with the offset between the tags and the data so that everything magically works out.

npretaps () must return a value between 0 and ntaps() - 1.

If npretaps () returns 0, this means that the equalizer will only handle multipath \"in the past.\" I suspect that a good value would be something like 15% - 20% of ntaps ().

Params: (NONE)"

%feature("docstring") atsci_equalizer::filter_normal "Input range is known NOT TO CONTAIN data segment syncs or field syncs. This should be the fast path. In the non decicion directed case, this just runs the input through the filter without adapting it.

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples)"

%feature("docstring") atsci_equalizer::filter_data_seg_sync "Input range is known to consist of only a data segment sync or a portion of a data segment sync.  will be in [1,4].  will be in [0,3].  is the offset of the input from the beginning of the data segment sync pattern.

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples, offset)"

%feature("docstring") atsci_equalizer::filter_field_sync "Input range is known to consist of only a field sync segment or a portion of a field sync segment.  will be in [1,832].  will be in [0,831].  is the offset of the input from the beginning of the data segment sync pattern. We consider the 4 symbols of the immediately preceding data segment sync to be the first symbols of the field sync segment.  is in [0,1] and specifies which field (duh).

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples, offset, which_field)"



%feature("docstring") atsci_equalizer_lms::atsci_equalizer_lms "

Params: (NONE)"

%feature("docstring") atsci_equalizer_lms::~atsci_equalizer_lms "

Params: (NONE)"

%feature("docstring") atsci_equalizer_lms::reset "reset state (e.g., on channel change)

Note, subclasses must invoke the superclass's method too!

Params: (NONE)"

%feature("docstring") atsci_equalizer_lms::ntaps "how much history the input data stream requires.

This must return a value >= 1. Think of this as the number of samples you need to look at to compute a single output sample.

Params: (NONE)"

%feature("docstring") atsci_equalizer_lms::npretaps "how many taps are \"in the future\".

This allows us to handle what the ATSC folks call \"pre-ghosts\". What it really does is allow the caller to jack with the offset between the tags and the data so that everything magically works out.

npretaps () must return a value between 0 and ntaps() - 1.

If npretaps () returns 0, this means that the equalizer will only handle multipath \"in the past.\" I suspect that a good value would be something like 15% - 20% of ntaps ().

Params: (NONE)"

%feature("docstring") atsci_equalizer_lms::filter_normal "Input range is known NOT TO CONTAIN data segment syncs or field syncs. This should be the fast path. In the non decicion directed case, this just runs the input through the filter without adapting it.

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples)"

%feature("docstring") atsci_equalizer_lms::filter_data_seg_sync "Input range is known to consist of only a data segment sync or a portion of a data segment sync.  will be in [1,4].  will be in [0,3].  is the offset of the input from the beginning of the data segment sync pattern.

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples, offset)"

%feature("docstring") atsci_equalizer_lms::filter_field_sync "Input range is known to consist of only a field sync segment or a portion of a field sync segment.  will be in [1,832].  will be in [0,831].  is the offset of the input from the beginning of the data segment sync pattern. We consider the 4 symbols of the immediately preceding data segment sync to be the first symbols of the field sync segment.  is in [0,1] and specifies which field (duh).

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples, offset, which_field)"

%feature("docstring") atsci_equalizer_lms::filterN "

Params: (input_samples, output_samples, nsamples)"

%feature("docstring") atsci_equalizer_lms::adaptN "

Params: (input_samples, training_pattern, output_samples, nsamples)"

%feature("docstring") atsci_equalizer_lms::filter1 "

Params: (input)"

%feature("docstring") atsci_equalizer_lms::adapt1 "

Params: (input, ideal_output)"



%feature("docstring") atsci_equalizer_lms2::atsci_equalizer_lms2 "

Params: (NONE)"

%feature("docstring") atsci_equalizer_lms2::~atsci_equalizer_lms2 "

Params: (NONE)"

%feature("docstring") atsci_equalizer_lms2::reset "reset state (e.g., on channel change)

Note, subclasses must invoke the superclass's method too!

Params: (NONE)"

%feature("docstring") atsci_equalizer_lms2::ntaps "how much history the input data stream requires.

This must return a value >= 1. Think of this as the number of samples you need to look at to compute a single output sample.

Params: (NONE)"

%feature("docstring") atsci_equalizer_lms2::npretaps "how many taps are \"in the future\".

This allows us to handle what the ATSC folks call \"pre-ghosts\". What it really does is allow the caller to jack with the offset between the tags and the data so that everything magically works out.

npretaps () must return a value between 0 and ntaps() - 1.

If npretaps () returns 0, this means that the equalizer will only handle multipath \"in the past.\" I suspect that a good value would be something like 15% - 20% of ntaps ().

Params: (NONE)"

%feature("docstring") atsci_equalizer_lms2::filter_normal "Input range is known NOT TO CONTAIN data segment syncs or field syncs. This should be the fast path. In the non decicion directed case, this just runs the input through the filter without adapting it.

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples)"

%feature("docstring") atsci_equalizer_lms2::filter_data_seg_sync "Input range is known to consist of only a data segment sync or a portion of a data segment sync.  will be in [1,4].  will be in [0,3].  is the offset of the input from the beginning of the data segment sync pattern.

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples, offset)"

%feature("docstring") atsci_equalizer_lms2::filter_field_sync "Input range is known to consist of only a field sync segment or a portion of a field sync segment.  will be in [1,832].  will be in [0,831].  is the offset of the input from the beginning of the data segment sync pattern. We consider the 4 symbols of the immediately preceding data segment sync to be the first symbols of the field sync segment.  is in [0,1] and specifies which field (duh).

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples, offset, which_field)"

%feature("docstring") atsci_equalizer_lms2::filterN "

Params: (input_samples, output_samples, nsamples)"

%feature("docstring") atsci_equalizer_lms2::adaptN "

Params: (input_samples, training_pattern, output_samples, nsamples)"

%feature("docstring") atsci_equalizer_lms2::filter1 "

Params: (input)"

%feature("docstring") atsci_equalizer_lms2::adapt1 "

Params: (input, ideal_output)"



%feature("docstring") atsci_equalizer_nop::scale "

Params: (input)"

%feature("docstring") atsci_equalizer_nop::scale_and_train "

Params: (input)"

%feature("docstring") atsci_equalizer_nop::atsci_equalizer_nop "

Params: (NONE)"

%feature("docstring") atsci_equalizer_nop::~atsci_equalizer_nop "

Params: (NONE)"

%feature("docstring") atsci_equalizer_nop::reset "reset state (e.g., on channel change)

Note, subclasses must invoke the superclass's method too!

Params: (NONE)"

%feature("docstring") atsci_equalizer_nop::ntaps "how much history the input data stream requires.

This must return a value >= 1. Think of this as the number of samples you need to look at to compute a single output sample.

Params: (NONE)"

%feature("docstring") atsci_equalizer_nop::npretaps "how many taps are \"in the future\".

This allows us to handle what the ATSC folks call \"pre-ghosts\". What it really does is allow the caller to jack with the offset between the tags and the data so that everything magically works out.

npretaps () must return a value between 0 and ntaps() - 1.

If npretaps () returns 0, this means that the equalizer will only handle multipath \"in the past.\" I suspect that a good value would be something like 15% - 20% of ntaps ().

Params: (NONE)"

%feature("docstring") atsci_equalizer_nop::filter_normal "Input range is known NOT TO CONTAIN data segment syncs or field syncs. This should be the fast path. In the non decicion directed case, this just runs the input through the filter without adapting it.

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples)"

%feature("docstring") atsci_equalizer_nop::filter_data_seg_sync "Input range is known to consist of only a data segment sync or a portion of a data segment sync.  will be in [1,4].  will be in [0,3].  is the offset of the input from the beginning of the data segment sync pattern.

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples, offset)"

%feature("docstring") atsci_equalizer_nop::filter_field_sync "Input range is known to consist of only a field sync segment or a portion of a field sync segment.  will be in [1,832].  will be in [0,831].  is the offset of the input from the beginning of the data segment sync pattern. We consider the 4 symbols of the immediately preceding data segment sync to be the first symbols of the field sync segment.  is in [0,1] and specifies which field (duh).

 has (nsamples + ntaps() - 1) valid entries. input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be referenced to compute the output values.

Params: (input_samples, output_samples, nsamples, offset, which_field)"



%feature("docstring") atsci_exp2_lp::taps "

Params: (sampling_freq)"

%feature("docstring") atsci_fake_single_viterbi "single channel viterbi decoder"

%feature("docstring") atsci_fake_single_viterbi::atsci_fake_single_viterbi "

Params: (NONE)"

%feature("docstring") atsci_fake_single_viterbi::decode "ideally takes on the values +/- 1,3,5,7 return is decoded dibit in the range [0, 3]

Params: (input)"

%feature("docstring") atsci_fake_single_viterbi::reset "

Params: (NONE)"

%feature("docstring") atsci_fake_single_viterbi::delay "internal delay of decoder

Params: (NONE)"

%feature("docstring") atsci_fs_checker "abstract base class for ATSC field sync checker

Processes input samples one at a time looking for an occurence of either the field sync 1 or field sync 2 pattern.

Note that unlike atsci_fs_correlator, this class uses the symbol_num in input_tag to avoid having to test each symbol position.

For each sample processed, an output sample and an output tag are produced. The output samples are identical to the input samples but are delayed by a number of samples given by . The output tag associated with the the given output sample indicates whether this sample is the beginning of one of the field syncs or is an ordinary sample. The tags are defined in atsci_sync_tag.h.

For ease of use, the field sync patterns are defined to begin with the first symbol of the 4 symbol data segment sync pattern that immediately proceeds the actual PN 511 code. This makes it easier for downstream code to determine the location of data segment syncs merely by counting. They'll occur every 832 samples assuming everything is working."

%feature("docstring") atsci_fs_checker::atsci_fs_checker "

Params: (NONE)"

%feature("docstring") atsci_fs_checker::~atsci_fs_checker "

Params: (NONE)"

%feature("docstring") atsci_fs_checker::reset "

Params: (NONE)"

%feature("docstring") atsci_fs_checker::filter "

Params: (input_sample, input_tag, output_sample, output_tag)"

%feature("docstring") atsci_fs_checker::delay "return delay in samples from input to output

Params: (NONE)"

%feature("docstring") atsci_fs_checker_naive "Naive concrete implementation of field sync checker."

%feature("docstring") atsci_fs_checker_naive::wrap "

Params: (index)"

%feature("docstring") atsci_fs_checker_naive::incr "

Params: (index)"

%feature("docstring") atsci_fs_checker_naive::decr "

Params: (index)"

%feature("docstring") atsci_fs_checker_naive::atsci_fs_checker_naive "

Params: (NONE)"

%feature("docstring") atsci_fs_checker_naive::~atsci_fs_checker_naive "

Params: (NONE)"

%feature("docstring") atsci_fs_checker_naive::reset "

Params: (NONE)"

%feature("docstring") atsci_fs_checker_naive::filter "

Params: (input_sample, input_tag, output_sample, output_tag)"

%feature("docstring") atsci_fs_checker_naive::delay "return delay in samples from input to output

Params: (NONE)"

%feature("docstring") atsci_fs_correlator "abstract base class for ATSC field sync correlator

Processes input samples one at a time looking for an occurence of either the field sync 1 or field sync 2 pattern.

For each sample processed, an output sample and an output tag are produced. The output samples are identical to the input samples but are delayed by a number of samples given by . The output tag associated with the the given output sample indicates whether this sample is the beginning of one of the field syncs or is an ordinary sample. The tags are defined in atsci_sync_tag.h.

For ease of use, the field sync patterns are defined to begin with the first symbol of the 4 symbol data segment sync pattern that immediately proceeds the actual PN 511 code. This makes it easier for downstream code to determine the location of data segment syncs merely by counting. They'll occur every 832 samples assuming everything is working."

%feature("docstring") atsci_fs_correlator::atsci_fs_correlator "

Params: (NONE)"

%feature("docstring") atsci_fs_correlator::~atsci_fs_correlator "

Params: (NONE)"

%feature("docstring") atsci_fs_correlator::reset "

Params: (NONE)"

%feature("docstring") atsci_fs_correlator::filter "

Params: (input_sample, output_sample, output_tag)"

%feature("docstring") atsci_fs_correlator::delay "return delay in samples from input to output

Params: (NONE)"

%feature("docstring") atsci_fs_correlator_naive "Naive concrete implementation of field sync correlator."

%feature("docstring") atsci_fs_correlator_naive::wrap "

Params: (index)"

%feature("docstring") atsci_fs_correlator_naive::incr "

Params: (index)"

%feature("docstring") atsci_fs_correlator_naive::decr "

Params: (index)"

%feature("docstring") atsci_fs_correlator_naive::atsci_fs_correlator_naive "

Params: (NONE)"

%feature("docstring") atsci_fs_correlator_naive::~atsci_fs_correlator_naive "

Params: (NONE)"

%feature("docstring") atsci_fs_correlator_naive::reset "

Params: (NONE)"

%feature("docstring") atsci_fs_correlator_naive::filter "

Params: (input_sample, output_sample, output_tag)"

%feature("docstring") atsci_fs_correlator_naive::delay "return delay in samples from input to output

Params: (NONE)"

%feature("docstring") atsci_interpolator "interpolator control for segment and symbol sync recovery"

%feature("docstring") atsci_interpolator::atsci_interpolator "must be >= 1.8

Params: (nominal_ratio_of_rx_clock_to_symbol_freq)"

%feature("docstring") atsci_interpolator::~atsci_interpolator "

Params: (NONE)"

%feature("docstring") atsci_interpolator::reset "call on channel change

Params: (NONE)"

%feature("docstring") atsci_interpolator::update "produce next sample referenced to Tx clock

If there aren't enough input_samples left to produce an output, return false, else true.

Params: (input_samples, nsamples, index, timing_adjustment, output_sample)"

%feature("docstring") atsci_interpolator::ntaps "

Params: (NONE)"

%feature("docstring") atsci_interpolator::mu "

Params: (NONE)"

%feature("docstring") atsci_interpolator::w "

Params: (NONE)"

%feature("docstring") atsci_interpolator::incr "

Params: (NONE)"

%feature("docstring") atsci_randomizer "ATSC data \"whitener\".

The data randomizer described in ATSC standard A/53B. See figure D4 on page 54."

%feature("docstring") atsci_randomizer::atsci_randomizer "

Params: (NONE)"

%feature("docstring") atsci_randomizer::reset "reset randomizer LFSR

must be called during the Data Segment Sync interval prior to the first data segment. I.e., the LFSR is reset prior to the first field of each VSB data frame.

Params: (NONE)"

%feature("docstring") atsci_randomizer::randomize "randomize (whiten) mpeg packet and remove leading MPEG-2 sync byte

Params: (out, in)"

%feature("docstring") atsci_randomizer::derandomize "derandomize (de-whiten) mpeg packet and add leading MPEG-2 sync byte

Params: (out, in)"

%feature("docstring") atsci_randomizer::state "

Params: (NONE)"

%feature("docstring") atsci_randomizer::initialize_output_map "

Params: (NONE)"

%feature("docstring") atsci_randomizer::slow_output_map "

Params: (st)"

%feature("docstring") atsci_randomizer::fast_output_map "

Params: (st)"

%feature("docstring") atsci_randomizer::output "return current output value

Params: (NONE)"

%feature("docstring") atsci_randomizer::clk "clock LFSR; advance to next state.

Params: (NONE)"

%feature("docstring") atsci_randomizer::output_and_clk "return current output value and advance to next state

Params: (NONE)"

%feature("docstring") atsci_reed_solomon "ATSC Reed-Solomon encoder / decoder.

The t=10 (207,187) code described in ATSC standard A/53B. See figure D5 on page 55."

%feature("docstring") atsci_reed_solomon::atsci_reed_solomon "

Params: (NONE)"

%feature("docstring") atsci_reed_solomon::~atsci_reed_solomon "

Params: (NONE)"

%feature("docstring") atsci_reed_solomon::encode "Add RS error correction encoding.

Params: (out, in)"

%feature("docstring") atsci_reed_solomon::decode "Decode RS encoded packet.

Params: (out, in)"

%feature("docstring") atsci_single_viterbi "single channel viterbi decoder"

%feature("docstring") atsci_single_viterbi::atsci_single_viterbi "

Params: (NONE)"

%feature("docstring") atsci_single_viterbi::decode "ideally takes on the values +/- 1,3,5,7 return is decoded dibit in the range [0, 3]

Params: (input)"

%feature("docstring") atsci_single_viterbi::reset "

Params: (NONE)"

%feature("docstring") atsci_single_viterbi::delay "internal delay of decoder

Params: (NONE)"

%feature("docstring") atsci_slicer_agc "Automatic Gain Control class for atsc slicer.

Given perfect data, output values will be +/- {7, 5, 3, 1}"

%feature("docstring") atsci_slicer_agc::atsci_slicer_agc "

Params: (NONE)"

%feature("docstring") atsci_slicer_agc::gain "

Params: (NONE)"

%feature("docstring") atsci_slicer_agc::scale "

Params: (input)"

%feature("docstring") atsci_sliding_correlator "look for the PN 511 field sync pattern"

%feature("docstring") atsci_sliding_correlator::atsci_sliding_correlator "

Params: (NONE)"

%feature("docstring") atsci_sliding_correlator::~atsci_sliding_correlator "

Params: (NONE)"

%feature("docstring") atsci_sliding_correlator::input_bit "input hard decision bit, return correlation (0,511)

Params: (bit)"

%feature("docstring") atsci_sliding_correlator::input_int "input sample, return correlation (0,511)

Params: (sample)"

%feature("docstring") atsci_sliding_correlator::input_float "input sample, return correlation (0,511)

Params: (sample)"

%feature("docstring") atsci_sliding_correlator::reset "

Params: (NONE)"

%feature("docstring") atsci_sssr "ATSC Segment and Symbol Sync Recovery.

This class implements data segment sync tracking and symbol timing using the method described in \"ATSC/VSB Tutorial - Receiver Technology\" by Wayne E. Bretl of Zenith, pgs 41-45."

%feature("docstring") atsci_sssr::incr_counter "

Params: (NONE)"

%feature("docstring") atsci_sssr::incr_symbol_index "

Params: (NONE)"

%feature("docstring") atsci_sssr::atsci_sssr "

Params: (NONE)"

%feature("docstring") atsci_sssr::~atsci_sssr "

Params: (NONE)"

%feature("docstring") atsci_sssr::reset "call on channel change

Params: (NONE)"

%feature("docstring") atsci_sssr::update "process a single sample at the ATSC symbol rate (~10.76 MSPS)

This block computes an indication of our timing error and keeps track of where the segment sync's occur.  is returned to indicate how the interpolator timing needs to be adjusted to track the transmitter's symbol timing. If  is true, then  is the index of this sample in the current segment. The symbols are numbered from 0 to 831, where symbols 0, 1, 2 and 3 correspond to the data segment sync pattern, nominally +5, -5, -5, +5.

Params: (sample_in, seg_locked, symbol_index, timing_adjust)"

%feature("docstring") atsci_trellis_encoder "fancy, schmancy 12-way interleaved trellis encoder for ATSC"

%feature("docstring") atsci_trellis_encoder::atsci_trellis_encoder "

Params: (NONE)"

%feature("docstring") atsci_trellis_encoder::~atsci_trellis_encoder "

Params: (NONE)"

%feature("docstring") atsci_trellis_encoder::reset "reset all encoder states

Params: (NONE)"

%feature("docstring") atsci_trellis_encoder::encode "Take 12 RS encoded, convolutionally interleaved segments and produce 12 trellis coded data segments. We work in groups of 12 because that's the smallest number of segments that composes a single full cycle of the encoder mux.

Params: (out, in)"

%feature("docstring") atsci_trellis_encoder::encode_helper "

Params: (out, in)"

%feature("docstring") atsci_viterbi_decoder "fancy, schmancy 12-way interleaved viterbi decoder for ATSC"

%feature("docstring") atsci_viterbi_decoder::atsci_viterbi_decoder "

Params: (NONE)"

%feature("docstring") atsci_viterbi_decoder::~atsci_viterbi_decoder "

Params: (NONE)"

%feature("docstring") atsci_viterbi_decoder::reset "reset all decoder states

Params: (NONE)"

%feature("docstring") atsci_viterbi_decoder::decode "Take 12 data segments of soft decisions (floats) and produce 12 RS encoded data segments. We work in groups of 12 because that's the smallest number of segments that composes a single full cycle of the decoder mux.

Params: (out, in)"

%feature("docstring") atsci_viterbi_decoder::decode_helper "

Params: (out, in)"

%feature("docstring") audio_alsa_sink "audio sink using ALSA

The sink has N input streams of floats, where N depends on the hardware characteristics of the selected device.

Input samples must be in the range [-1,1]."

%feature("docstring") audio_alsa_sink::output_error_msg "

Params: (msg, err)"

%feature("docstring") audio_alsa_sink::bail "

Params: (msg, err)"

%feature("docstring") audio_alsa_sink::audio_alsa_sink "

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_alsa_sink::write_buffer "

Params: (buffer, nframes, sizeof_frame)"

%feature("docstring") audio_alsa_sink::work_s16 "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_alsa_sink::work_s16_1x2 "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_alsa_sink::work_s32 "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_alsa_sink::work_s32_1x2 "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_alsa_sink::~audio_alsa_sink "

Params: (NONE)"

%feature("docstring") audio_alsa_sink::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") audio_alsa_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_alsa_source "audio source using ALSA

The source has between 1 and N input streams of floats, where N is depends on the hardware characteristics of the selected device.

Output samples will be in the range [-1,1]."

%feature("docstring") audio_alsa_source::output_error_msg "

Params: (msg, err)"

%feature("docstring") audio_alsa_source::bail "

Params: (msg, err)"

%feature("docstring") audio_alsa_source::audio_alsa_source "

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_alsa_source::read_buffer "

Params: (buffer, nframes, sizeof_frame)"

%feature("docstring") audio_alsa_source::work_s16 "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_alsa_source::work_s16_2x1 "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_alsa_source::work_s32 "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_alsa_source::work_s32_2x1 "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_alsa_source::~audio_alsa_source "

Params: (NONE)"

%feature("docstring") audio_alsa_source::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") audio_alsa_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_jack_sink "audio sink using JACK

The sink has one input stream of floats.

Input samples must be in the range [-1,1]."

%feature("docstring") audio_jack_sink::output_error_msg "

Params: (msg, err)"

%feature("docstring") audio_jack_sink::bail "

Params: (msg, err)"

%feature("docstring") audio_jack_sink::audio_jack_sink "

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_jack_sink::~audio_jack_sink "

Params: (NONE)"

%feature("docstring") audio_jack_sink::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") audio_jack_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_jack_source "audio source using JACK

The source has one input stream of floats.

Output samples will be in the range [-1,1]."

%feature("docstring") audio_jack_source::output_error_msg "

Params: (msg, err)"

%feature("docstring") audio_jack_source::bail "

Params: (msg, err)"

%feature("docstring") audio_jack_source::audio_jack_source "

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_jack_source::~audio_jack_source "

Params: (NONE)"

%feature("docstring") audio_jack_source::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") audio_jack_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_oss_sink "audio sink using OSS

input signature is one or two streams of floats. Input samples must be in the range [-1,1]."

%feature("docstring") audio_oss_sink::audio_oss_sink "

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_oss_sink::~audio_oss_sink "

Params: (NONE)"

%feature("docstring") audio_oss_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_oss_source "audio source using OSS

Output signature is one or two streams of floats. Output samples will be in the range [-1,1]."

%feature("docstring") audio_oss_source::audio_oss_source "

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_oss_source::~audio_oss_source "

Params: (NONE)"

%feature("docstring") audio_oss_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_osx_sink "audio sink using OSX

input signature is one or two streams of floats. Input samples must be in the range [-1,1]."

%feature("docstring") audio_osx_sink::audio_osx_sink "

Params: (sample_rate, device_name, do_block, channel_config, max_sample_count)"

%feature("docstring") audio_osx_sink::~audio_osx_sink "

Params: (NONE)"

%feature("docstring") audio_osx_sink::IsRunning "

Params: (NONE)"

%feature("docstring") audio_osx_sink::start "Called to enable drivers, etc for i/o devices.

This allows a block to enable an associated driver to begin transfering data just before we start to execute the scheduler. The end result is that this reduces latency in the pipeline when dealing with audio devices, usrps, etc.

Params: (NONE)"

%feature("docstring") audio_osx_sink::stop "Called to disable drivers, etc for i/o devices.

Params: (NONE)"

%feature("docstring") audio_osx_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_osx_sink::AUOutputCallback "

Params: (inRefCon, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData)"

%feature("docstring") audio_osx_source "audio source using OSX

Input signature is one or two streams of floats. Samples must be in the range [-1,1]."

%feature("docstring") audio_osx_source::audio_osx_source "

Params: (sample_rate, device_name, do_block, channel_config, max_sample_count)"

%feature("docstring") audio_osx_source::~audio_osx_source "

Params: (NONE)"

%feature("docstring") audio_osx_source::start "Called to enable drivers, etc for i/o devices.

This allows a block to enable an associated driver to begin transfering data just before we start to execute the scheduler. The end result is that this reduces latency in the pipeline when dealing with audio devices, usrps, etc.

Params: (NONE)"

%feature("docstring") audio_osx_source::stop "Called to disable drivers, etc for i/o devices.

Params: (NONE)"

%feature("docstring") audio_osx_source::IsRunning "

Params: (NONE)"

%feature("docstring") audio_osx_source::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") audio_osx_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_osx_source::SetDefaultInputDeviceAsCurrent "

Params: (NONE)"

%feature("docstring") audio_osx_source::AllocAudioBufferList "

Params: (t_ABL, n_channels, inputBufferSizeBytes)"

%feature("docstring") audio_osx_source::FreeAudioBufferList "

Params: (t_ABL)"

%feature("docstring") audio_osx_source::ConverterCallback "

Params: (inAudioConverter, ioNumberDataPackets, ioData, outASPD, inUserData)"

%feature("docstring") audio_osx_source::AUInputCallback "

Params: (inRefCon, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData)"

%feature("docstring") audio_portaudio_sink "\ Audio sink using PORTAUDIO

Input samples must be in the range [-1,1]."

%feature("docstring") audio_portaudio_sink::output_error_msg "

Params: (msg, err)"

%feature("docstring") audio_portaudio_sink::bail "

Params: (msg, err)"

%feature("docstring") audio_portaudio_sink::create_ringbuffer "

Params: (NONE)"

%feature("docstring") audio_portaudio_sink::audio_portaudio_sink "

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_portaudio_sink::~audio_portaudio_sink "

Params: (NONE)"

%feature("docstring") audio_portaudio_sink::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") audio_portaudio_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_portaudio_source "\ Audio source using PORTAUDIO

Input samples must be in the range [-1,1]."

%feature("docstring") audio_portaudio_source::output_error_msg "

Params: (msg, err)"

%feature("docstring") audio_portaudio_source::bail "

Params: (msg, err)"

%feature("docstring") audio_portaudio_source::create_ringbuffer "

Params: (NONE)"

%feature("docstring") audio_portaudio_source::audio_portaudio_source "

Params: (sampling_rate, device_name, ok_to_block)"

%feature("docstring") audio_portaudio_source::~audio_portaudio_source "

Params: (NONE)"

%feature("docstring") audio_portaudio_source::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") audio_portaudio_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_windows_sink "audio sink using winmm mmsystem (win32 only)

input signature is one or two streams of floats. Input samples must be in the range [-1,1]."

%feature("docstring") audio_windows_sink::string_to_int "

Params: (s)"

%feature("docstring") audio_windows_sink::audio_windows_sink "

Params: (sampling_freq, device_name)"

%feature("docstring") audio_windows_sink::open_waveout_device "

Params: ()"

%feature("docstring") audio_windows_sink::write_waveout "

Params: (lp_data, dw_data_size)"

%feature("docstring") audio_windows_sink::~audio_windows_sink "

Params: (NONE)"

%feature("docstring") audio_windows_sink::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") audio_windows_source "audio source using winmm mmsystem (win32 only)

Output signature is one or two streams of floats. Output samples will be in the range [-1,1]."

%feature("docstring") audio_windows_source::audio_windows_source "

Params: (sampling_freq, device_name)"

%feature("docstring") audio_windows_source::~audio_windows_source "

Params: (NONE)"

%feature("docstring") audio_windows_source::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") std::auto_ptr "STL class."

%feature("docstring") std::bad_alloc "STL class."

%feature("docstring") std::bad_cast "STL class."

%feature("docstring") std::bad_exception "STL class."

%feature("docstring") std::bad_typeid "STL class."

%feature("docstring") std::basic_fstream "STL class."

%feature("docstring") std::basic_ifstream "STL class."

%feature("docstring") std::basic_ios "STL class."

%feature("docstring") std::basic_iostream "STL class."

%feature("docstring") std::basic_istream "STL class."

%feature("docstring") std::basic_istringstream "STL class."

%feature("docstring") std::basic_ofstream "STL class."

%feature("docstring") std::basic_ostream "STL class."

%feature("docstring") std::basic_ostringstream "STL class."

%feature("docstring") std::basic_string "STL class."

%feature("docstring") std::basic_stringstream "STL class."

%feature("docstring") std::bitset "STL class."



%feature("docstring") msdd::BufferCopyBehavior::operator() "

Params: (a, b, output_index, nitems)"

%feature("docstring") msdd::BufferCopyBehavior::~BufferCopyBehavior "

Params: (NONE)"



%feature("docstring") msdd::BufferCopyBehaviorComplex::operator() "

Params: (a, b, output_index, nitems)"



%feature("docstring") msdd::BufferCopyBehaviorGeneric::operator() "

Params: (a, b, output_index, nitems)"



%feature("docstring") circular_buffer::delete_mutex_cond "

Params: (NONE)"

%feature("docstring") circular_buffer::circular_buffer "

Params: (bufLen_I, doWriteBlock, doFullRead)"

%feature("docstring") circular_buffer::~circular_buffer "

Params: (NONE)"

%feature("docstring") circular_buffer::n_avail_write_items "

Params: (NONE)"

%feature("docstring") circular_buffer::n_avail_read_items "

Params: (NONE)"

%feature("docstring") circular_buffer::buffer_length_items "

Params: (NONE)"

%feature("docstring") circular_buffer::do_write_block "

Params: (NONE)"

%feature("docstring") circular_buffer::do_full_read "

Params: (NONE)"

%feature("docstring") circular_buffer::reset "

Params: (NONE)"

%feature("docstring") circular_buffer::enqueue "

Params: (buf, bufLen_I)"

%feature("docstring") circular_buffer::dequeue "

Params: (buf, bufLen_I)"

%feature("docstring") circular_buffer::abort "

Params: (NONE)"



%feature("docstring") circular_linked_list::circular_linked_list "

Params: (n_nodes)"

%feature("docstring") circular_linked_list::~circular_linked_list "

Params: (NONE)"

%feature("docstring") circular_linked_list::find_next_available_node "

Params: (NONE)"

%feature("docstring") circular_linked_list::make_node_available "

Params: (l_node)"

%feature("docstring") circular_linked_list::iterate_start "

Params: (NONE)"

%feature("docstring") circular_linked_list::iterate_next "

Params: (NONE)"

%feature("docstring") circular_linked_list::object "

Params: (NONE)"

%feature("docstring") circular_linked_list::num_nodes "

Params: (NONE)"

%feature("docstring") circular_linked_list::num_used "

Params: (NONE)"

%feature("docstring") circular_linked_list::num_available "

Params: (NONE)"

%feature("docstring") circular_linked_list::num_used_inc "

Params: ()"

%feature("docstring") circular_linked_list::num_used_dec "

Params: ()"

%feature("docstring") circular_linked_list::in_use "

Params: (NONE)"

%feature("docstring") std::complex "STL class."

%feature("docstring") std::basic_string::const_iterator "STL iterator class."

%feature("docstring") std::string::const_iterator "STL iterator class."

%feature("docstring") std::wstring::const_iterator "STL iterator class."

%feature("docstring") std::deque::const_iterator "STL iterator class."

%feature("docstring") std::list::const_iterator "STL iterator class."

%feature("docstring") std::map::const_iterator "STL iterator class."

%feature("docstring") std::multimap::const_iterator "STL iterator class."

%feature("docstring") std::set::const_iterator "STL iterator class."

%feature("docstring") std::multiset::const_iterator "STL iterator class."

%feature("docstring") std::vector::const_iterator "STL iterator class."

%feature("docstring") std::basic_string::const_reverse_iterator "STL iterator class."

%feature("docstring") std::string::const_reverse_iterator "STL iterator class."

%feature("docstring") std::wstring::const_reverse_iterator "STL iterator class."

%feature("docstring") std::deque::const_reverse_iterator "STL iterator class."

%feature("docstring") std::list::const_reverse_iterator "STL iterator class."

%feature("docstring") std::map::const_reverse_iterator "STL iterator class."

%feature("docstring") std::multimap::const_reverse_iterator "STL iterator class."

%feature("docstring") std::set::const_reverse_iterator "STL iterator class."

%feature("docstring") std::multiset::const_reverse_iterator "STL iterator class."

%feature("docstring") std::vector::const_reverse_iterator "STL iterator class."

%feature("docstring") convolutional_interleaver "template class for generic convolutional interleaver"

%feature("docstring") convolutional_interleaver::convolutional_interleaver "

Params: (interleave_p, nbanks, fifo_size_incr)"

%feature("docstring") convolutional_interleaver::~convolutional_interleaver "

Params: (NONE)"

%feature("docstring") convolutional_interleaver::reset "reset interleaver (flushes contents and resets commutator)

Params: (NONE)"

%feature("docstring") convolutional_interleaver::sync "sync interleaver (resets commutator, but doesn't flush fifos)

Params: (NONE)"

%feature("docstring") convolutional_interleaver::end_to_end_delay "return end to end delay in symbols (delay through concatenated interleaver / deinterleaver)

Params: (NONE)"

%feature("docstring") convolutional_interleaver::transform "transform a single symbol

Params: (input)"



%feature("docstring") usrp2::copy_handler::copy_handler "

Params: (dest, len)"

%feature("docstring") usrp2::copy_handler::~copy_handler "

Params: (NONE)"

%feature("docstring") usrp2::copy_handler::operator() "

Params: (base, len)"

%feature("docstring") usrp2::copy_handler::bytes "

Params: (NONE)"

%feature("docstring") usrp2::copy_handler::times "

Params: (NONE)"

%feature("docstring") usrp2::copy_handler::full "

Params: (NONE)"

%feature("docstring") usrp2::data_handler "Abstract function object called to handle received data blocks."

%feature("docstring") usrp2::data_handler::operator() "

Params: (base, len)"

%feature("docstring") usrp2::data_handler::~data_handler "

Params: (NONE)"

%feature("docstring") db_base "Abstract base class for all USRP daughterboards."

%feature("docstring") db_base::_enable_refclk "

Params: (enable)"

%feature("docstring") db_base::_refclk_freq "

Params: (NONE)"

%feature("docstring") db_base::_refclk_divisor "

Params: (NONE)"

%feature("docstring") db_base::usrp "

Params: (NONE)"

%feature("docstring") db_base::shutdown "Called to shutdown daughterboard. Called from dtor and usrp_basic dtor.

N.B., any class that overrides shutdown MUST call shutdown in its destructor.

Params: (NONE)"

%feature("docstring") db_base::db_base "

Params: (usrp, which)"

%feature("docstring") db_base::~db_base "

Params: (NONE)"

%feature("docstring") db_base::dbid "

Params: (NONE)"

%feature("docstring") db_base::name "

Params: (NONE)"

%feature("docstring") db_base::side_and_name "

Params: (NONE)"

%feature("docstring") db_base::which "

Params: (NONE)"

%feature("docstring") db_base::bypass_adc_buffers "

Params: (bypass)"

%feature("docstring") db_base::set_atr_mask "

Params: (v)"

%feature("docstring") db_base::set_atr_txval "

Params: (v)"

%feature("docstring") db_base::set_atr_rxval "

Params: (v)"

%feature("docstring") db_base::set_atr_tx_delay "

Params: (v)"

%feature("docstring") db_base::set_atr_rx_delay "

Params: (v)"

%feature("docstring") db_base::set_lo_offset "

Params: (offset)"

%feature("docstring") db_base::lo_offset "

Params: (NONE)"

%feature("docstring") db_base::gain_min "

Params: (NONE)"

%feature("docstring") db_base::gain_max "

Params: (NONE)"

%feature("docstring") db_base::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_base::freq_min "

Params: (NONE)"

%feature("docstring") db_base::freq_max "

Params: (NONE)"

%feature("docstring") db_base::set_freq "

Params: (target_freq)"

%feature("docstring") db_base::set_gain "

Params: (gain)"

%feature("docstring") db_base::is_quadrature "

Params: (NONE)"

%feature("docstring") db_base::i_and_q_swapped "

Params: (NONE)"

%feature("docstring") db_base::spectrum_inverted "

Params: (NONE)"

%feature("docstring") db_base::set_enable "

Params: (on)"

%feature("docstring") db_base::set_auto_tr "

Params: (on)"

%feature("docstring") db_base::select_rx_antenna "

Params: (which_antenna)"

%feature("docstring") db_base::set_bw "

Params: (bw)"



%feature("docstring") db_basic_rx::db_basic_rx "

Params: (usrp, which, subdev)"

%feature("docstring") db_basic_rx::~db_basic_rx "

Params: (NONE)"

%feature("docstring") db_basic_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_basic_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_basic_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_basic_rx::freq_min "

Params: (NONE)"

%feature("docstring") db_basic_rx::freq_max "

Params: (NONE)"

%feature("docstring") db_basic_rx::set_freq "

Params: (target_freq)"

%feature("docstring") db_basic_rx::set_gain "

Params: (gain)"

%feature("docstring") db_basic_rx::is_quadrature "

Params: (NONE)"



%feature("docstring") db_basic_tx::db_basic_tx "

Params: (usrp, which)"

%feature("docstring") db_basic_tx::~db_basic_tx "

Params: (NONE)"

%feature("docstring") db_basic_tx::gain_min "

Params: (NONE)"

%feature("docstring") db_basic_tx::gain_max "

Params: (NONE)"

%feature("docstring") db_basic_tx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_basic_tx::freq_min "

Params: (NONE)"

%feature("docstring") db_basic_tx::freq_max "

Params: (NONE)"

%feature("docstring") db_basic_tx::set_freq "

Params: (target_freq)"

%feature("docstring") db_basic_tx::set_gain "

Params: (gain)"

%feature("docstring") db_basic_tx::is_quadrature "

Params: (NONE)"



%feature("docstring") db_bitshark_rx::_set_pga "

Params: (pga_gain)"

%feature("docstring") db_bitshark_rx::shutdown "Called to shutdown daughterboard. Called from dtor and usrp_basic dtor.

N.B., any class that overrides shutdown MUST call shutdown in its destructor.

Params: (NONE)"

%feature("docstring") db_bitshark_rx::db_bitshark_rx "

Params: (usrp, which)"

%feature("docstring") db_bitshark_rx::~db_bitshark_rx "

Params: (NONE)"

%feature("docstring") db_bitshark_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_bitshark_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_bitshark_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_bitshark_rx::freq_min "

Params: (NONE)"

%feature("docstring") db_bitshark_rx::freq_max "

Params: (NONE)"

%feature("docstring") db_bitshark_rx::set_freq "

Params: (freq)"

%feature("docstring") db_bitshark_rx::set_gain "

Params: (gain)"

%feature("docstring") db_bitshark_rx::set_bw "

Params: (bw)"

%feature("docstring") db_bitshark_rx::set_clock_scheme "

Params: (clock_scheme, ref_clock_freq)"

%feature("docstring") db_bitshark_rx::is_quadrature "

Params: (NONE)"

%feature("docstring") db_bitshark_rx::i_and_q_swapped "

Params: (NONE)"



%feature("docstring") db_dbs_rx::_write_reg "

Params: (regno, v)"

%feature("docstring") db_dbs_rx::_write_regs "

Params: (starting_regno, vals)"

%feature("docstring") db_dbs_rx::_read_status "

Params: (NONE)"

%feature("docstring") db_dbs_rx::_send_reg "

Params: (regno)"

%feature("docstring") db_dbs_rx::_set_m "

Params: (m)"

%feature("docstring") db_dbs_rx::_set_fdac "

Params: (fdac)"

%feature("docstring") db_dbs_rx::_set_dl "

Params: (dl)"

%feature("docstring") db_dbs_rx::_set_gc2 "

Params: (gc2)"

%feature("docstring") db_dbs_rx::_set_gc1 "

Params: (gc1)"

%feature("docstring") db_dbs_rx::_set_pga "

Params: (pga_gain)"

%feature("docstring") db_dbs_rx::_set_osc "

Params: (osc)"

%feature("docstring") db_dbs_rx::_set_cp "

Params: (cp)"

%feature("docstring") db_dbs_rx::_set_n "

Params: (n)"

%feature("docstring") db_dbs_rx::_set_div2 "

Params: (div2)"

%feature("docstring") db_dbs_rx::_set_r "

Params: (r)"

%feature("docstring") db_dbs_rx::_set_ade "

Params: (ade)"

%feature("docstring") db_dbs_rx::_refclk_divisor "

Params: (NONE)"

%feature("docstring") db_dbs_rx::shutdown "Called to shutdown daughterboard. Called from dtor and usrp_basic dtor.

N.B., any class that overrides shutdown MUST call shutdown in its destructor.

Params: (NONE)"

%feature("docstring") db_dbs_rx::db_dbs_rx "

Params: (usrp, which)"

%feature("docstring") db_dbs_rx::~db_dbs_rx "

Params: (NONE)"

%feature("docstring") db_dbs_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_dbs_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_dbs_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_dbs_rx::freq_min "

Params: (NONE)"

%feature("docstring") db_dbs_rx::freq_max "

Params: (NONE)"

%feature("docstring") db_dbs_rx::set_freq "

Params: (freq)"

%feature("docstring") db_dbs_rx::set_gain "

Params: (gain)"

%feature("docstring") db_dbs_rx::is_quadrature "

Params: (NONE)"

%feature("docstring") db_dbs_rx::set_bw "

Params: (bw)"



%feature("docstring") db_dtt754::db_dtt754 "

Params: (usrp, which)"

%feature("docstring") db_dtt754::~db_dtt754 "

Params: (NONE)"

%feature("docstring") db_dtt754::gain_min "

Params: (NONE)"

%feature("docstring") db_dtt754::gain_max "

Params: (NONE)"

%feature("docstring") db_dtt754::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_dtt754::set_gain "

Params: (gain)"

%feature("docstring") db_dtt754::freq_min "

Params: (NONE)"

%feature("docstring") db_dtt754::freq_max "

Params: (NONE)"

%feature("docstring") db_dtt754::set_freq "

Params: (target_freq)"

%feature("docstring") db_dtt754::is_quadrature "

Params: (NONE)"

%feature("docstring") db_dtt754::spectrum_inverted "

Params: (NONE)"

%feature("docstring") db_dtt754::set_bw "

Params: (bw)"

%feature("docstring") db_dtt754::_set_rfagc "

Params: (gain)"

%feature("docstring") db_dtt754::_set_ifagc "

Params: (gain)"

%feature("docstring") db_dtt754::_set_pga "

Params: (pga_gain)"



%feature("docstring") db_dtt768::db_dtt768 "

Params: (usrp, which)"

%feature("docstring") db_dtt768::~db_dtt768 "

Params: (NONE)"

%feature("docstring") db_dtt768::gain_min "

Params: (NONE)"

%feature("docstring") db_dtt768::gain_max "

Params: (NONE)"

%feature("docstring") db_dtt768::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_dtt768::set_gain "

Params: (gain)"

%feature("docstring") db_dtt768::freq_min "

Params: (NONE)"

%feature("docstring") db_dtt768::freq_max "

Params: (NONE)"

%feature("docstring") db_dtt768::set_freq "

Params: (target_freq)"

%feature("docstring") db_dtt768::is_quadrature "

Params: (NONE)"

%feature("docstring") db_dtt768::spectrum_inverted "

Params: (NONE)"

%feature("docstring") db_dtt768::set_bw "

Params: (bw)"

%feature("docstring") db_dtt768::_set_rfagc "

Params: (gain)"

%feature("docstring") db_dtt768::_set_ifagc "

Params: (gain)"

%feature("docstring") db_dtt768::_set_pga "

Params: (pga_gain)"



%feature("docstring") db_flexrf_1200_rx::db_flexrf_1200_rx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1200_rx::~db_flexrf_1200_rx "

Params: (NONE)"

%feature("docstring") db_flexrf_1200_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_flexrf_1200_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_flexrf_1200_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_flexrf_1200_rx::i_and_q_swapped "

Params: (NONE)"

%feature("docstring") db_flexrf_1200_rx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_1200_rx_mimo_a::db_flexrf_1200_rx_mimo_a "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1200_rx_mimo_a::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_1200_rx_mimo_b::db_flexrf_1200_rx_mimo_b "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1200_rx_mimo_b::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_1200_tx::db_flexrf_1200_tx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1200_tx::~db_flexrf_1200_tx "

Params: (NONE)"

%feature("docstring") db_flexrf_1200_tx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_1200_tx_mimo_a::db_flexrf_1200_tx_mimo_a "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1200_tx_mimo_a::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_1200_tx_mimo_b::db_flexrf_1200_tx_mimo_b "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1200_tx_mimo_b::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_1800_rx::db_flexrf_1800_rx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1800_rx::~db_flexrf_1800_rx "

Params: (NONE)"

%feature("docstring") db_flexrf_1800_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_flexrf_1800_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_flexrf_1800_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_flexrf_1800_rx::i_and_q_swapped "

Params: (NONE)"

%feature("docstring") db_flexrf_1800_rx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_1800_rx_mimo_a::db_flexrf_1800_rx_mimo_a "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1800_rx_mimo_a::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_1800_rx_mimo_b::db_flexrf_1800_rx_mimo_b "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1800_rx_mimo_b::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_1800_tx::db_flexrf_1800_tx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1800_tx::~db_flexrf_1800_tx "

Params: (NONE)"

%feature("docstring") db_flexrf_1800_tx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_1800_tx_mimo_a::db_flexrf_1800_tx_mimo_a "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1800_tx_mimo_a::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_1800_tx_mimo_b::db_flexrf_1800_tx_mimo_b "

Params: (usrp, which)"

%feature("docstring") db_flexrf_1800_tx_mimo_b::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_2200_rx::db_flexrf_2200_rx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_2200_rx::~db_flexrf_2200_rx "

Params: (NONE)"

%feature("docstring") db_flexrf_2200_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_flexrf_2200_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_flexrf_2200_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_flexrf_2200_rx::i_and_q_swapped "

Params: (NONE)"

%feature("docstring") db_flexrf_2200_rx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_2200_tx::db_flexrf_2200_tx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_2200_tx::~db_flexrf_2200_tx "

Params: (NONE)"

%feature("docstring") db_flexrf_2200_tx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_2400_rx::db_flexrf_2400_rx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_2400_rx::~db_flexrf_2400_rx "

Params: (NONE)"

%feature("docstring") db_flexrf_2400_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_flexrf_2400_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_flexrf_2400_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_flexrf_2400_rx::i_and_q_swapped "

Params: (NONE)"

%feature("docstring") db_flexrf_2400_rx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_2400_rx_mimo_a::db_flexrf_2400_rx_mimo_a "

Params: (usrp, which)"

%feature("docstring") db_flexrf_2400_rx_mimo_a::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_2400_rx_mimo_b::db_flexrf_2400_rx_mimo_b "

Params: (usrp, which)"

%feature("docstring") db_flexrf_2400_rx_mimo_b::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_2400_tx::db_flexrf_2400_tx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_2400_tx::~db_flexrf_2400_tx "

Params: (NONE)"

%feature("docstring") db_flexrf_2400_tx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_2400_tx_mimo_a::db_flexrf_2400_tx_mimo_a "

Params: (usrp, which)"

%feature("docstring") db_flexrf_2400_tx_mimo_a::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_2400_tx_mimo_b::db_flexrf_2400_tx_mimo_b "

Params: (usrp, which)"

%feature("docstring") db_flexrf_2400_tx_mimo_b::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_400_rx::db_flexrf_400_rx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_400_rx::~db_flexrf_400_rx "

Params: (NONE)"

%feature("docstring") db_flexrf_400_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_flexrf_400_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_flexrf_400_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_flexrf_400_rx::i_and_q_swapped "

Params: (NONE)"

%feature("docstring") db_flexrf_400_rx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_400_rx_mimo_a::db_flexrf_400_rx_mimo_a "

Params: (usrp, which)"

%feature("docstring") db_flexrf_400_rx_mimo_a::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_400_rx_mimo_b::db_flexrf_400_rx_mimo_b "

Params: (usrp, which)"

%feature("docstring") db_flexrf_400_rx_mimo_b::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_400_tx::db_flexrf_400_tx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_400_tx::~db_flexrf_400_tx "

Params: (NONE)"

%feature("docstring") db_flexrf_400_tx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_400_tx_mimo_a::db_flexrf_400_tx_mimo_a "

Params: (usrp, which)"

%feature("docstring") db_flexrf_400_tx_mimo_a::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_400_tx_mimo_b::db_flexrf_400_tx_mimo_b "

Params: (usrp, which)"

%feature("docstring") db_flexrf_400_tx_mimo_b::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_900_rx::db_flexrf_900_rx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_900_rx::~db_flexrf_900_rx "

Params: (NONE)"

%feature("docstring") db_flexrf_900_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_flexrf_900_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_flexrf_900_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_flexrf_900_rx::i_and_q_swapped "

Params: (NONE)"

%feature("docstring") db_flexrf_900_rx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_900_rx_mimo_a::db_flexrf_900_rx_mimo_a "

Params: (usrp, which)"

%feature("docstring") db_flexrf_900_rx_mimo_a::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_900_rx_mimo_b::db_flexrf_900_rx_mimo_b "

Params: (usrp, which)"

%feature("docstring") db_flexrf_900_rx_mimo_b::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_900_tx::db_flexrf_900_tx "

Params: (usrp, which)"

%feature("docstring") db_flexrf_900_tx::~db_flexrf_900_tx "

Params: (NONE)"

%feature("docstring") db_flexrf_900_tx::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"



%feature("docstring") db_flexrf_900_tx_mimo_a::db_flexrf_900_tx_mimo_a "

Params: (usrp, which)"

%feature("docstring") db_flexrf_900_tx_mimo_a::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_flexrf_900_tx_mimo_b::db_flexrf_900_tx_mimo_b "

Params: (usrp, which)"

%feature("docstring") db_flexrf_900_tx_mimo_b::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_lf_rx::db_lf_rx "

Params: (usrp, which, subdev)"

%feature("docstring") db_lf_rx::~db_lf_rx "

Params: (NONE)"

%feature("docstring") db_lf_rx::freq_min "

Params: (NONE)"

%feature("docstring") db_lf_rx::freq_max "

Params: (NONE)"



%feature("docstring") db_lf_tx::db_lf_tx "

Params: (usrp, which)"

%feature("docstring") db_lf_tx::~db_lf_tx "

Params: (NONE)"

%feature("docstring") db_lf_tx::freq_min "

Params: (NONE)"

%feature("docstring") db_lf_tx::freq_max "

Params: (NONE)"



%feature("docstring") db_tv_rx::_set_rfagc "

Params: (gain)"

%feature("docstring") db_tv_rx::_set_ifagc "

Params: (gain)"

%feature("docstring") db_tv_rx::_set_pga "

Params: (pga_gain)"

%feature("docstring") db_tv_rx::db_tv_rx "

Params: (usrp, which, first_IF, second_IF)"

%feature("docstring") db_tv_rx::~db_tv_rx "

Params: (NONE)"

%feature("docstring") db_tv_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_tv_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_tv_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_tv_rx::freq_min "

Params: (NONE)"

%feature("docstring") db_tv_rx::freq_max "

Params: (NONE)"

%feature("docstring") db_tv_rx::set_freq "

Params: (target_freq)"

%feature("docstring") db_tv_rx::set_gain "

Params: (gain)"

%feature("docstring") db_tv_rx::is_quadrature "

Params: (NONE)"

%feature("docstring") db_tv_rx::spectrum_inverted "

Params: (NONE)"



%feature("docstring") db_tv_rx_mimo::db_tv_rx_mimo "

Params: (usrp, which, first_IF, second_IF)"

%feature("docstring") db_tv_rx_mimo::_refclk_divisor "

Params: (NONE)"



%feature("docstring") db_wbx_lo_rx::db_wbx_lo_rx "

Params: (usrp, which)"

%feature("docstring") db_wbx_lo_rx::~db_wbx_lo_rx "

Params: (NONE)"

%feature("docstring") db_wbx_lo_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_wbx_lo_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_wbx_lo_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_wbx_lo_rx::_refclk_freq "

Params: (NONE)"

%feature("docstring") db_wbx_lo_rx::_rx_write_io "

Params: (value, mask)"

%feature("docstring") db_wbx_lo_rx::_lock_detect "

Params: (NONE)"

%feature("docstring") db_wbx_lo_rx::usrp "

Params: (NONE)"



%feature("docstring") db_wbx_lo_tx::db_wbx_lo_tx "

Params: (usrp, which)"

%feature("docstring") db_wbx_lo_tx::~db_wbx_lo_tx "

Params: (NONE)"

%feature("docstring") db_wbx_lo_tx::gain_min "

Params: (NONE)"

%feature("docstring") db_wbx_lo_tx::gain_max "

Params: (NONE)"

%feature("docstring") db_wbx_lo_tx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_wbx_lo_tx::set_gain "

Params: (gain)"

%feature("docstring") db_wbx_lo_tx::_refclk_freq "

Params: (NONE)"

%feature("docstring") db_wbx_lo_tx::_rx_write_io "

Params: (value, mask)"

%feature("docstring") db_wbx_lo_tx::_lock_detect "

Params: (NONE)"

%feature("docstring") db_wbx_lo_tx::usrp "

Params: (NONE)"



%feature("docstring") db_wbxng_rx::shutdown "Called to shutdown daughterboard. Called from dtor and usrp_basic dtor.

N.B., any class that overrides shutdown MUST call shutdown in its destructor.

Params: (NONE)"

%feature("docstring") db_wbxng_rx::_set_attn "

Params: (attn)"

%feature("docstring") db_wbxng_rx::db_wbxng_rx "

Params: (usrp, which)"

%feature("docstring") db_wbxng_rx::~db_wbxng_rx "

Params: (NONE)"

%feature("docstring") db_wbxng_rx::set_auto_tr "

Params: (on)"

%feature("docstring") db_wbxng_rx::select_rx_antenna "

Params: (which_antenna)"

%feature("docstring") db_wbxng_rx::set_gain "

Params: (gain)"

%feature("docstring") db_wbxng_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_wbxng_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_wbxng_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_wbxng_rx::i_and_q_swapped "

Params: (NONE)"



%feature("docstring") db_wbxng_tx::shutdown "Called to shutdown daughterboard. Called from dtor and usrp_basic dtor.

N.B., any class that overrides shutdown MUST call shutdown in its destructor.

Params: (NONE)"

%feature("docstring") db_wbxng_tx::db_wbxng_tx "

Params: (usrp, which)"

%feature("docstring") db_wbxng_tx::~db_wbxng_tx "

Params: (NONE)"

%feature("docstring") db_wbxng_tx::gain_min "

Params: (NONE)"

%feature("docstring") db_wbxng_tx::gain_max "

Params: (NONE)"

%feature("docstring") db_wbxng_tx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_wbxng_tx::set_auto_tr "

Params: (on)"

%feature("docstring") db_wbxng_tx::set_enable "

Params: (on)"

%feature("docstring") db_wbxng_tx::set_gain "

Params: (gain)"



%feature("docstring") db_xcvr2450_base::db_xcvr2450_base "

Params: (usrp, which)"

%feature("docstring") db_xcvr2450_base::~db_xcvr2450_base "

Params: (NONE)"

%feature("docstring") db_xcvr2450_base::set_freq "

Params: (target_freq)"

%feature("docstring") db_xcvr2450_base::is_quadrature "

Params: (NONE)"

%feature("docstring") db_xcvr2450_base::freq_min "

Params: (NONE)"

%feature("docstring") db_xcvr2450_base::freq_max "

Params: (NONE)"

%feature("docstring") db_xcvr2450_base::shutdown_common "

Params: (NONE)"



%feature("docstring") db_xcvr2450_rx::shutdown "Called to shutdown daughterboard. Called from dtor and usrp_basic dtor.

N.B., any class that overrides shutdown MUST call shutdown in its destructor.

Params: (NONE)"

%feature("docstring") db_xcvr2450_rx::db_xcvr2450_rx "

Params: (usrp, which)"

%feature("docstring") db_xcvr2450_rx::~db_xcvr2450_rx "

Params: (NONE)"

%feature("docstring") db_xcvr2450_rx::gain_min "

Params: (NONE)"

%feature("docstring") db_xcvr2450_rx::gain_max "

Params: (NONE)"

%feature("docstring") db_xcvr2450_rx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_xcvr2450_rx::set_gain "

Params: (gain)"



%feature("docstring") db_xcvr2450_tx::shutdown "Called to shutdown daughterboard. Called from dtor and usrp_basic dtor.

N.B., any class that overrides shutdown MUST call shutdown in its destructor.

Params: (NONE)"

%feature("docstring") db_xcvr2450_tx::db_xcvr2450_tx "

Params: (usrp, which)"

%feature("docstring") db_xcvr2450_tx::~db_xcvr2450_tx "

Params: (NONE)"

%feature("docstring") db_xcvr2450_tx::gain_min "

Params: (NONE)"

%feature("docstring") db_xcvr2450_tx::gain_max "

Params: (NONE)"

%feature("docstring") db_xcvr2450_tx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") db_xcvr2450_tx::set_gain "

Params: (gain)"

%feature("docstring") db_xcvr2450_tx::i_and_q_swapped "

Params: (NONE)"

%feature("docstring") std::deque "STL class."



%feature("docstring") dial_tone::dial_tone "

Params: (NONE)"

%feature("docstring") sssr::digital_correlator "digital correlator for 1001 and 0110 patterns"

%feature("docstring") sssr::digital_correlator::digital_correlator "

Params: (NONE)"

%feature("docstring") sssr::digital_correlator::reset "called on channel change

Params: (NONE)"

%feature("docstring") sssr::digital_correlator::update "clock bit in and return true if we've seen 1001

Params: (bit)"

%feature("docstring") std::domain_error "STL class."

%feature("docstring") usrp2::eth_buffer "high-performance interface to send and receive raw ethernet frames with out-of-order retirement of received frames.

On many systems it should be possible to implement this on top of libpcap"

%feature("docstring") usrp2::eth_buffer::frame_available "

Params: (NONE)"

%feature("docstring") usrp2::eth_buffer::inc_head "

Params: (NONE)"

%feature("docstring") usrp2::eth_buffer::eth_buffer "

Params: (rx_bufsize)"

%feature("docstring") usrp2::eth_buffer::~eth_buffer "

Params: (NONE)"

%feature("docstring") usrp2::eth_buffer::open "open the specified interface

Params: (ifname, protocol)"

%feature("docstring") usrp2::eth_buffer::close "close the interface

Params: (NONE)"

%feature("docstring") usrp2::eth_buffer::attach_pktfilter "attach packet filter to socket to restrict which packets read sees.

Params: (pf)"

%feature("docstring") usrp2::eth_buffer::mac "return 6 byte string containing our MAC address

Params: (NONE)"

%feature("docstring") usrp2::eth_buffer::rx_frames "Call  for each frame in the receive buffer.

If  is 0, rx_frames will not wait for frames if none are available, and f will not be invoked. If  is -1 (the default), rx_frames will block indefinitely until frames are available. If  is positive, it indicates the number of milliseconds to wait for a frame to become available. Once the timeout has expired, rx_frames will return, f never having been invoked.

 will be called on each ethernet frame that is available.  returns a bit mask with one of the following set or cleared:

data_handler::KEEP - hold onto the frame and present it again during the next call to rx_frames, otherwise discard it

data_handler::DONE - return from rx_frames now even though more frames might be available; otherwise continue if more frames are ready.

The idea of holding onto a frame for the next iteration allows the caller to scan the received packet stream for particular classes of frames (such as command replies) leaving the rest intact. On the next call all kept frames, followed by any new frames received, will be presented in order to . See usrp2.cc for an example of the pattern.

Params: (f, timeout)"

%feature("docstring") usrp2::eth_buffer::release_frame "

Params: (p)"

%feature("docstring") usrp2::eth_buffer::tx_frame "

Params: (base, len, flags)"

%feature("docstring") usrp2::eth_buffer::tx_framev "

Params: (iov, iovcnt, flags)"

%feature("docstring") usrp2::eth_buffer::max_frames "

Params: (NONE)"

%feature("docstring") usrp2::ethernet "Read and write ethernet frames.

This provides a low level interface to hardware that communicates via raw (non-IP) ethernet frames."

%feature("docstring") usrp2::ethernet::ethernet "

Params: (NONE)"

%feature("docstring") usrp2::ethernet::~ethernet "

Params: (NONE)"

%feature("docstring") usrp2::ethernet::open "

Params: (ifname, protocol)"

%feature("docstring") usrp2::ethernet::close "

Params: (NONE)"

%feature("docstring") usrp2::ethernet::attach_pktfilter "attach packet filter to socket to restrict which packets read sees.

Params: (pf)"

%feature("docstring") usrp2::ethernet::mac "return 6 byte string containing our MAC address

Params: (NONE)"

%feature("docstring") usrp2::ethernet::fd "Return file descriptor associated with socket.

Params: (NONE)"

%feature("docstring") usrp2::ethernet::read_packet "Read packet from interface.

Returned packet includes 14-byte ethhdr

Params: (buf, buflen)"

%feature("docstring") usrp2::ethernet::read_packet_dont_block "Read packet from interface, but don't block waiting.

Returned packet includes 14-byte ethhdr

Params: (buf, buflen)"

%feature("docstring") usrp2::ethernet::write_packet "

Params: (buf, buflen)"

%feature("docstring") usrp2::ethernet::write_packetv "

Params: (iov, iovlen)"

%feature("docstring") std::exception "STL class."

%feature("docstring") std::ios_base::failure "STL class."



%feature("docstring") flexrf_base::flexrf_base "

Params: (usrp, which, _power_on)"

%feature("docstring") flexrf_base::~flexrf_base "

Params: (NONE)"

%feature("docstring") flexrf_base::set_freq "

Params: (freq)"

%feature("docstring") flexrf_base::is_quadrature "

Params: (NONE)"

%feature("docstring") flexrf_base::freq_min "

Params: (NONE)"

%feature("docstring") flexrf_base::freq_max "

Params: (NONE)"

%feature("docstring") flexrf_base::_write_all "

Params: (R, control, N)"

%feature("docstring") flexrf_base::_write_control "

Params: (control)"

%feature("docstring") flexrf_base::_write_R "

Params: (R)"

%feature("docstring") flexrf_base::_write_N "

Params: (N)"

%feature("docstring") flexrf_base::_write_it "

Params: (v)"

%feature("docstring") flexrf_base::_lock_detect "

Params: (NONE)"

%feature("docstring") flexrf_base::_compute_regs "

Params: (freq, retR, retcontrol, retN, retfreq)"

%feature("docstring") flexrf_base::_compute_control_reg "

Params: (NONE)"

%feature("docstring") flexrf_base::_refclk_divisor "

Params: (NONE)"

%feature("docstring") flexrf_base::_set_pga "

Params: (pga_gain)"

%feature("docstring") flexrf_base::power_on "

Params: (NONE)"

%feature("docstring") flexrf_base::power_off "

Params: (NONE)"



%feature("docstring") flexrf_base_rx::shutdown "Called to shutdown daughterboard. Called from dtor and usrp_basic dtor.

N.B., any class that overrides shutdown MUST call shutdown in its destructor.

Params: (NONE)"

%feature("docstring") flexrf_base_rx::flexrf_base_rx "

Params: (usrp, which, _power_on)"

%feature("docstring") flexrf_base_rx::~flexrf_base_rx "

Params: (NONE)"

%feature("docstring") flexrf_base_rx::set_auto_tr "

Params: (on)"

%feature("docstring") flexrf_base_rx::select_rx_antenna "

Params: (which_antenna)"

%feature("docstring") flexrf_base_rx::set_gain "

Params: (gain)"



%feature("docstring") flexrf_base_tx::shutdown "Called to shutdown daughterboard. Called from dtor and usrp_basic dtor.

N.B., any class that overrides shutdown MUST call shutdown in its destructor.

Params: (NONE)"

%feature("docstring") flexrf_base_tx::flexrf_base_tx "

Params: (usrp, which, _power_on)"

%feature("docstring") flexrf_base_tx::~flexrf_base_tx "

Params: (NONE)"

%feature("docstring") flexrf_base_tx::gain_min "

Params: (NONE)"

%feature("docstring") flexrf_base_tx::gain_max "

Params: (NONE)"

%feature("docstring") flexrf_base_tx::gain_db_per_step "

Params: (NONE)"

%feature("docstring") flexrf_base_tx::set_auto_tr "

Params: (on)"

%feature("docstring") flexrf_base_tx::set_enable "

Params: (on)"

%feature("docstring") flexrf_base_tx::set_gain "

Params: (gain)"

%feature("docstring") fsm "FSM class."

%feature("docstring") fsm::generate_PS_PI "

Params: (NONE)"

%feature("docstring") fsm::generate_TM "

Params: (NONE)"

%feature("docstring") fsm::find_es "

Params: (es)"

%feature("docstring") fsm::fsm "

Params: (NONE)"

%feature("docstring") fsm::I "

Params: (NONE)"

%feature("docstring") fsm::S "

Params: (NONE)"

%feature("docstring") fsm::O "

Params: (NONE)"

%feature("docstring") fsm::NS "

Params: (NONE)"

%feature("docstring") fsm::OS "

Params: (NONE)"

%feature("docstring") fsm::PS "

Params: (NONE)"

%feature("docstring") fsm::PI "

Params: (NONE)"

%feature("docstring") fsm::TMi "

Params: (NONE)"

%feature("docstring") fsm::TMl "

Params: (NONE)"

%feature("docstring") fsm::write_trellis_svg "

Params: (filename, number_stages)"

%feature("docstring") fsm::write_fsm_txt "

Params: (filename)"

%feature("docstring") std::fstream "STL class."

%feature("docstring") fusb_devhandle "abstract usb device handle"

%feature("docstring") fusb_devhandle::fusb_devhandle "

Params: (rhs)"

%feature("docstring") fusb_devhandle::operator= "

Params: (rhs)"

%feature("docstring") fusb_devhandle::~fusb_devhandle "

Params: (NONE)"

%feature("docstring") fusb_devhandle::make_ephandle "return an ephandle of the correct subtype

Params: (endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_devhandle::get_usb_dev_handle "

Params: (NONE)"

%feature("docstring") fusb_devhandle_darwin "darwin implementation of fusb_devhandle

This is currently identical to the generic implementation and is intended as a starting point for whatever magic is required to make usb fly."

%feature("docstring") fusb_devhandle_darwin::fusb_devhandle_darwin "

Params: (udh)"

%feature("docstring") fusb_devhandle_darwin::~fusb_devhandle_darwin "

Params: (NONE)"

%feature("docstring") fusb_devhandle_darwin::make_ephandle "return an ephandle of the correct subtype

Params: (endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_devhandle_generic "generic implementation of fusb_devhandle using only libusb"

%feature("docstring") fusb_devhandle_generic::fusb_devhandle_generic "

Params: (udh)"

%feature("docstring") fusb_devhandle_generic::~fusb_devhandle_generic "

Params: (NONE)"

%feature("docstring") fusb_devhandle_generic::make_ephandle "return an ephandle of the correct subtype

Params: (endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_devhandle_libusb1 "libusb1 implementation of fusb_devhandle"

%feature("docstring") fusb_devhandle_libusb1::pending_add "

Params: (lut)"

%feature("docstring") fusb_devhandle_libusb1::pending_get "

Params: (NONE)"

%feature("docstring") fusb_devhandle_libusb1::fusb_devhandle_libusb1 "

Params: (udh, ctx)"

%feature("docstring") fusb_devhandle_libusb1::~fusb_devhandle_libusb1 "

Params: (NONE)"

%feature("docstring") fusb_devhandle_libusb1::make_ephandle "return an ephandle of the correct subtype

Params: (endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_devhandle_libusb1::_submit_lut "

Params: ()"

%feature("docstring") fusb_devhandle_libusb1::_cancel_lut "

Params: ()"

%feature("docstring") fusb_devhandle_libusb1::_cancel_pending_rqsts "

Params: (eph)"

%feature("docstring") fusb_devhandle_libusb1::_reap "

Params: (ok_to_block_p)"

%feature("docstring") fusb_devhandle_libusb1::_wait_for_completion "

Params: (NONE)"

%feature("docstring") fusb_devhandle_libusb1::pending_remove "

Params: (lut)"

%feature("docstring") fusb_devhandle_libusb1::_teardown "

Params: (NONE)"

%feature("docstring") fusb_devhandle_linux "linux specific implementation of fusb_devhandle using usbdevice_fs"

%feature("docstring") fusb_devhandle_linux::pending_add "

Params: (urb)"

%feature("docstring") fusb_devhandle_linux::pending_remove "

Params: (urb)"

%feature("docstring") fusb_devhandle_linux::pending_get "

Params: (NONE)"

%feature("docstring") fusb_devhandle_linux::fusb_devhandle_linux "

Params: (udh)"

%feature("docstring") fusb_devhandle_linux::~fusb_devhandle_linux "

Params: (NONE)"

%feature("docstring") fusb_devhandle_linux::make_ephandle "return an ephandle of the correct subtype

Params: (endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_devhandle_linux::_submit_urb "

Params: (urb)"

%feature("docstring") fusb_devhandle_linux::_cancel_urb "

Params: (urb)"

%feature("docstring") fusb_devhandle_linux::_cancel_pending_rqsts "

Params: (eph)"

%feature("docstring") fusb_devhandle_linux::_reap "

Params: (ok_to_block_p)"

%feature("docstring") fusb_devhandle_linux::_wait_for_completion "

Params: (NONE)"

%feature("docstring") fusb_devhandle_ra_wb "generic implementation of fusb_devhandle using only libusb"

%feature("docstring") fusb_devhandle_ra_wb::fusb_devhandle_ra_wb "

Params: (udh)"

%feature("docstring") fusb_devhandle_ra_wb::~fusb_devhandle_ra_wb "

Params: (NONE)"

%feature("docstring") fusb_devhandle_ra_wb::make_ephandle "return an ephandle of the correct subtype

Params: (endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_devhandle_win32 "win32 implementation of fusb_devhandle using libusb-win32"

%feature("docstring") fusb_devhandle_win32::fusb_devhandle_win32 "

Params: (udh)"

%feature("docstring") fusb_devhandle_win32::~fusb_devhandle_win32 "

Params: (NONE)"

%feature("docstring") fusb_devhandle_win32::make_ephandle "return an ephandle of the correct subtype

Params: (endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_ephandle "abstract usb end point handle"

%feature("docstring") fusb_ephandle::fusb_ephandle "

Params: (rhs)"

%feature("docstring") fusb_ephandle::operator= "

Params: (rhs)"

%feature("docstring") fusb_ephandle::~fusb_ephandle "

Params: (NONE)"

%feature("docstring") fusb_ephandle::start "begin streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle::stop "stop streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle::write "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle::read "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle::wait_for_completion "

Params: (NONE)"

%feature("docstring") fusb_ephandle::block_size "returns current block size.

Params: (NONE)"

%feature("docstring") fusb_ephandle_darwin "darwin implementation of fusb_ephandle

This is currently identical to the generic implementation and is intended as a starting point for whatever magic is required to make usb fly."

%feature("docstring") fusb_ephandle_darwin::write_completed "

Params: (ret_io_size, result, io_size)"

%feature("docstring") fusb_ephandle_darwin::read_completed "

Params: (ret_io_size, result, io_size)"

%feature("docstring") fusb_ephandle_darwin::run_thread "

Params: (arg)"

%feature("docstring") fusb_ephandle_darwin::read_thread "

Params: (arg)"

%feature("docstring") fusb_ephandle_darwin::read_issue "

Params: (l_both)"

%feature("docstring") fusb_ephandle_darwin::fusb_ephandle_darwin "

Params: (dh, endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_ephandle_darwin::~fusb_ephandle_darwin "

Params: (NONE)"

%feature("docstring") fusb_ephandle_darwin::start "begin streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_darwin::stop "stop streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_darwin::write "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_darwin::read "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_darwin::abort "

Params: (NONE)"

%feature("docstring") fusb_ephandle_darwin::wait_for_completion "

Params: (NONE)"

%feature("docstring") fusb_ephandle_generic "generic implementation of fusb_ephandle using only libusb"

%feature("docstring") fusb_ephandle_generic::fusb_ephandle_generic "

Params: (dh, endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_ephandle_generic::~fusb_ephandle_generic "

Params: (NONE)"

%feature("docstring") fusb_ephandle_generic::start "begin streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_generic::stop "stop streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_generic::write "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_generic::read "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_generic::wait_for_completion "

Params: (NONE)"

%feature("docstring") fusb_ephandle_libusb1 "libusb1 implementation of fusb_ephandle"

%feature("docstring") fusb_ephandle_libusb1::get_write_work_in_progress "

Params: (NONE)"

%feature("docstring") fusb_ephandle_libusb1::reap_complete_writes "

Params: (NONE)"

%feature("docstring") fusb_ephandle_libusb1::reload_read_buffer "

Params: (NONE)"

%feature("docstring") fusb_ephandle_libusb1::submit_lut "

Params: (lut)"

%feature("docstring") fusb_ephandle_libusb1::fusb_ephandle_libusb1 "

Params: (dh, endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_ephandle_libusb1::~fusb_ephandle_libusb1 "

Params: (NONE)"

%feature("docstring") fusb_ephandle_libusb1::start "begin streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_libusb1::stop "stop streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_libusb1::write "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_libusb1::read "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_libusb1::wait_for_completion "

Params: (NONE)"

%feature("docstring") fusb_ephandle_libusb1::free_list_add "

Params: (lut)"

%feature("docstring") fusb_ephandle_libusb1::completed_list_add "

Params: (lut)"

%feature("docstring") fusb_ephandle_libusb1::free_list_get "

Params: (NONE)"

%feature("docstring") fusb_ephandle_libusb1::completed_list_get "

Params: (NONE)"

%feature("docstring") fusb_ephandle_libusb1::get_fusb_devhandle_libusb1 "

Params: (NONE)"

%feature("docstring") fusb_ephandle_linux "linux specific implementation of fusb_ephandle using usbdevice_fs"

%feature("docstring") fusb_ephandle_linux::get_write_work_in_progress "

Params: (NONE)"

%feature("docstring") fusb_ephandle_linux::reap_complete_writes "

Params: (NONE)"

%feature("docstring") fusb_ephandle_linux::reload_read_buffer "

Params: (NONE)"

%feature("docstring") fusb_ephandle_linux::submit_urb "

Params: (urb)"

%feature("docstring") fusb_ephandle_linux::fusb_ephandle_linux "

Params: (dh, endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_ephandle_linux::~fusb_ephandle_linux "

Params: (NONE)"

%feature("docstring") fusb_ephandle_linux::start "begin streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_linux::stop "stop streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_linux::write "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_linux::read "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_linux::wait_for_completion "

Params: (NONE)"

%feature("docstring") fusb_ephandle_linux::free_list_add "

Params: (urb)"

%feature("docstring") fusb_ephandle_linux::completed_list_add "

Params: (urb)"

%feature("docstring") fusb_ephandle_linux::free_list_get "

Params: (NONE)"

%feature("docstring") fusb_ephandle_linux::completed_list_get "

Params: (NONE)"

%feature("docstring") fusb_ephandle_ra_wb "generic implementation of fusb_ephandle using only libusb"

%feature("docstring") fusb_ephandle_ra_wb::fusb_ephandle_ra_wb "

Params: (dh, endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_ephandle_ra_wb::~fusb_ephandle_ra_wb "

Params: (NONE)"

%feature("docstring") fusb_ephandle_ra_wb::start "begin streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_ra_wb::stop "stop streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_ra_wb::write "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_ra_wb::read "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_ra_wb::wait_for_completion "

Params: (NONE)"

%feature("docstring") fusb_ephandle_win32 "win32 implementation of fusb_ephandle using libusb-win32"

%feature("docstring") fusb_ephandle_win32::fusb_ephandle_win32 "

Params: (dh, endpoint, input_p, block_size, nblocks)"

%feature("docstring") fusb_ephandle_win32::~fusb_ephandle_win32 "

Params: (NONE)"

%feature("docstring") fusb_ephandle_win32::start "begin streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_win32::stop "stop streaming i/o

Params: (NONE)"

%feature("docstring") fusb_ephandle_win32::write "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_win32::read "

Params: (buffer, nbytes)"

%feature("docstring") fusb_ephandle_win32::wait_for_completion "

Params: (NONE)"

%feature("docstring") fusb_sysconfig "factory for creating concrete instances of the appropriate subtype."

%feature("docstring") fusb_sysconfig::make_devhandle "returns fusb_devhandle or throws if trouble

Params: (udh, ctx)"

%feature("docstring") fusb_sysconfig::max_block_size "Returns max block size in bytes (hard limit).

Params: (NONE)"

%feature("docstring") fusb_sysconfig::default_block_size "Returns default block size in bytes.

Params: (NONE)"

%feature("docstring") fusb_sysconfig::default_buffer_size "Returns the default buffer size in bytes.

Params: (NONE)"



%feature("docstring") gc_bad_align::gc_bad_align "

Params: (msg)"



%feature("docstring") gc_bad_alloc::gc_bad_alloc "

Params: (msg)"



%feature("docstring") gc_bad_submit::gc_bad_submit "

Params: (name, status)"



%feature("docstring") gc_client_thread_info::gc_client_thread_info "

Params: (NONE)"

%feature("docstring") gc_client_thread_info::~gc_client_thread_info "

Params: (NONE)"



%feature("docstring") gc_exception::gc_exception "

Params: (msg)"

%feature("docstring") gc_job_manager "Abstract class that manages SPE jobs.

There is typically a single instance derived from this class. It is safe to call its methods from any thread."

%feature("docstring") gc_job_manager::gc_job_manager "

Params: (options)"

%feature("docstring") gc_job_manager::~gc_job_manager "

Params: (NONE)"

%feature("docstring") gc_job_manager::shutdown "Stop accepting new jobs. Wait for existing jobs to complete. Return all managed SPE's to the system.

Params: (NONE)"

%feature("docstring") gc_job_manager::nspes "Return number of SPE's currently allocated to job manager.

Params: (NONE)"

%feature("docstring") gc_job_manager::alloc_job_desc "Return a pointer to a properly aligned job descriptor, or throws gc_bad_alloc if there are none available.

Params: (NONE)"

%feature("docstring") gc_job_manager::free_job_desc "

Params: (jd)"

%feature("docstring") gc_job_manager::submit_job "Submit a job for asynchronous processing on an SPE.

The caller must not read or write the job description or any of the memory associated with any indirect arguments until after calling wait_job.

Params: (jd)"

%feature("docstring") gc_job_manager::wait_job "Wait for job to complete.

A thread may only wait for jobs which it submitted.

Params: (jd)"

%feature("docstring") gc_job_manager::wait_jobs "wait for 1 or more jobs to complete.

A thread may only wait for jobs which it submitted.

Params: (njobs, jd, done, mode)"

%feature("docstring") gc_job_manager::ea_args_maxsize "Return the maximum number of bytes of EA arguments that may be copied to or from the SPE in a single job. The limit applies independently to the \"get\" and \"put\" args.

Params: (NONE)"

%feature("docstring") gc_job_manager::lookup_proc "Return gc_proc_id_t associated with spu procedure  if one exists, otherwise throws gc_unknown_proc.

Params: (proc_name)"

%feature("docstring") gc_job_manager::proc_names "Return a vector of all known spu procedure names.

Params: (NONE)"

%feature("docstring") gc_job_manager::set_singleton "Set the singleton gc_job_manager instance.

The singleton is weakly held, thus the caller must maintain a reference to the mgr for the duration. (If we held the manager strongly, the destructor would never be called, and the resources (SPEs) would not be returned.) Bottom line: the caller is responsible for life-time management.

Params: (mgr)"

%feature("docstring") gc_job_manager::singleton "Retrieve the singleton gc_job_manager instance.

Returns the singleton gc_job_manager instance or raises boost::bad_weak_ptr if the singleton is empty.

Params: (NONE)"

%feature("docstring") gc_job_manager_impl "Concrete class that manages SPE jobs.

This class contains all the implementation details."

%feature("docstring") gc_job_manager_impl::alloc_cti "

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::free_cti "

Params: (cti)"

%feature("docstring") gc_job_manager_impl::create_event_handler "

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::set_eh_state "

Params: (s)"

%feature("docstring") gc_job_manager_impl::set_ea_args_maxsize "

Params: (maxsize)"

%feature("docstring") gc_job_manager_impl::notify_clients_jobs_are_done "

Params: (spe_num, completion_info_idx)"

%feature("docstring") gc_job_manager_impl::send_all_spes "

Params: (msg)"

%feature("docstring") gc_job_manager_impl::send_spe "

Params: (spe, msg)"

%feature("docstring") gc_job_manager_impl::print_event "

Params: (evt)"

%feature("docstring") gc_job_manager_impl::handle_event "

Params: (evt)"

%feature("docstring") gc_job_manager_impl::incr_njobs_active "

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::decr_njobs_active "

Params: (n)"

%feature("docstring") gc_job_manager_impl::tell_spes_to_check_queue "

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::poll_for_job_completion "

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::bv_zero "

Params: (bv)"

%feature("docstring") gc_job_manager_impl::bv_clr "

Params: (bv, bitno)"

%feature("docstring") gc_job_manager_impl::bv_set "

Params: (bv, bitno)"

%feature("docstring") gc_job_manager_impl::bv_isset "

Params: (bv, bitno)"

%feature("docstring") gc_job_manager_impl::bv_isclr "

Params: (bv, bitno)"

%feature("docstring") gc_job_manager_impl::setup_logfiles "

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::sync_logfiles "

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::unmap_logfiles "

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::gc_job_manager_impl "

Params: (options)"

%feature("docstring") gc_job_manager_impl::event_handler_loop "

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::job_completer_loop "

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::~gc_job_manager_impl "

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::shutdown "Stop accepting new jobs. Wait for existing jobs to complete. Return all managed SPE's to the system.

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::nspes "Return number of SPE's currently allocated to job manager.

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::alloc_job_desc "Return a pointer to a properly aligned job descriptor, or zero if none are available.

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::free_job_desc "

Params: (jd)"

%feature("docstring") gc_job_manager_impl::submit_job "Submit a job for asynchronous processing on an SPE.

The caller must not read or write the job description or any of the memory associated with any indirect arguments until after calling wait_job.

Params: (jd)"

%feature("docstring") gc_job_manager_impl::wait_job "Wait for job to complete.

A thread may only wait for jobs which it submitted.

Params: (jd)"

%feature("docstring") gc_job_manager_impl::wait_jobs "wait for 1 or more jobs to complete.

A thread may only wait for jobs which it submitted.

Params: (njobs, jd, done, mode)"

%feature("docstring") gc_job_manager_impl::ea_args_maxsize "Return the maximum number of bytes of EA arguments that may be copied to or from the SPE in a single job. The limit applies independently to the \"get\" and \"put\" args.

Params: (NONE)"

%feature("docstring") gc_job_manager_impl::lookup_proc "Return gc_proc_id_t associated with spu procedure  if one exists, otherwise throws gc_unknown_proc.

Params: (proc_name)"

%feature("docstring") gc_job_manager_impl::proc_names "Return a vector of all known spu procedure names.

Params: (NONE)"



%feature("docstring") gc_unknown_proc::gc_unknown_proc "

Params: (msg)"

%feature("docstring") gr_adaptive_fir_ccf "Adaptive FIR filter with gr_complex input, gr_complex output and float taps."

%feature("docstring") gr_adaptive_fir_ccf::error "

Params: (out)"

%feature("docstring") gr_adaptive_fir_ccf::update_tap "

Params: (tap, in)"

%feature("docstring") gr_adaptive_fir_ccf::gr_adaptive_fir_ccf "

Params: (name, decimation, taps)"

%feature("docstring") gr_adaptive_fir_ccf::set_taps "

Params: (taps)"

%feature("docstring") gr_adaptive_fir_ccf::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"



%feature("docstring") gr_base_error_handler::gr_base_error_handler "

Params: (NONE)"

%feature("docstring") gr_base_error_handler::nwarnings "

Params: (NONE)"

%feature("docstring") gr_base_error_handler::nerrors "

Params: (NONE)"

%feature("docstring") gr_base_error_handler::reset_counts "

Params: (NONE)"

%feature("docstring") gr_base_error_handler::count_error "

Params: (s)"

%feature("docstring") gr_basic_block "The abstract base class for all signal processing blocks.

Basic blocks are the bare abstraction of an entity that has a name, a set of inputs and outputs, and a message queue. These are never instantiated directly; rather, this is the abstract parent class of both gr_hier_block, which is a recursive container, and gr_block, which implements actual signal processing functions."

%feature("docstring") gr_basic_block::dispatch_msg "

Params: (msg)"

%feature("docstring") gr_basic_block::gr_basic_block "Protected constructor prevents instantiation by non-derived classes.

Params: (name, input_signature, output_signature)"

%feature("docstring") gr_basic_block::set_input_signature "may only be called during constructor

Params: (iosig)"

%feature("docstring") gr_basic_block::set_output_signature "may only be called during constructor

Params: (iosig)"

%feature("docstring") gr_basic_block::set_color "Allow the flowgraph to set for sorting and partitioning.

Params: (color)"

%feature("docstring") gr_basic_block::color "

Params: (NONE)"

%feature("docstring") gr_basic_block::~gr_basic_block "

Params: (NONE)"

%feature("docstring") gr_basic_block::unique_id "

Params: (NONE)"

%feature("docstring") gr_basic_block::name "

Params: (NONE)"

%feature("docstring") gr_basic_block::input_signature "

Params: (NONE)"

%feature("docstring") gr_basic_block::output_signature "

Params: (NONE)"

%feature("docstring") gr_basic_block::to_basic_block "

Params: (NONE)"

%feature("docstring") gr_basic_block::check_topology "Confirm that ninputs and noutputs is an acceptable combination.

This function is called by the runtime system whenever the topology changes. Most classes do not need to override this. This check is in addition to the constraints specified by the input and output gr_io_signatures.

Params: (ninputs, noutputs)"

%feature("docstring") gr_basic_block::set_msg_handler "Set the callback that is fired when messages are available.

can be any kind of function pointer or function object that has the signature: 

(You may want to use boost::bind to massage your callable into the correct form. See gr_nop.{h,cc} for an example that sets up a class method as the callback.)

Blocks that desire to handle messages must call this method in their constructors to register the handler that will be invoked when messages are available.

If the block inherits from gr_block, the runtime system will ensure that msg_handler is called in a thread-safe manner, such that work and msg_handler will never be called concurrently. This allows msg_handler to update state variables without having to worry about thread-safety issues with work, general_work or another invocation of msg_handler.

If the block inherits from gr_hier_block2, the runtime system will ensure that no reentrant calls are made to msg_handler.

Params: (msg_handler)"

%feature("docstring") gr_block "The abstract base class for all 'terminal' processing blocks.

A signal processing flow is constructed by creating a tree of hierarchical blocks, which at any level may also contain terminal nodes that actually implement signal processing functions. This is the base class for all such leaf nodes.

Blocks have a set of input streams and output streams. The input_signature and output_signature define the number of input streams and output streams respectively, and the type of the data items in each stream.

Although blocks may consume data on each input stream at a different rate, all outputs streams must produce data at the same rate. That rate may be different from any of the input rates.

User derived blocks override two methods, forecast and general_work, to implement their signal processing behavior. forecast is called by the system scheduler to determine how many items are required on each input stream in order to produce a given number of output items.

general_work is called to perform the signal processing in the block. It reads the input items and writes the output items."

%feature("docstring") gr_block::~gr_block "

Params: (NONE)"

%feature("docstring") gr_block::history "Assume block computes y_i = f(x_i, x_i-1, x_i-2, x_i-3...) History is the number of x_i's that are examined to produce one y_i. This comes in handy for FIR filters, where we use history to ensure that our input contains the appropriate \"history\" for the filter. History should be equal to the number of filter taps.

Params: (NONE)"

%feature("docstring") gr_block::set_history "

Params: (history)"

%feature("docstring") gr_block::fixed_rate "Return true if this block has a fixed input to output rate.

If true, then fixed_rate_in_to_out and fixed_rate_out_to_in may be called.

Params: (NONE)"

%feature("docstring") gr_block::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_block::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_block::start "Called to enable drivers, etc for i/o devices.

This allows a block to enable an associated driver to begin transfering data just before we start to execute the scheduler. The end result is that this reduces latency in the pipeline when dealing with audio devices, usrps, etc.

Params: (NONE)"

%feature("docstring") gr_block::stop "Called to disable drivers, etc for i/o devices.

Params: (NONE)"

%feature("docstring") gr_block::set_output_multiple "Constrain the noutput_items argument passed to forecast and general_work.

set_output_multiple causes the scheduler to ensure that the noutput_items argument passed to forecast and general_work will be an integer multiple of

Params: (multiple)"

%feature("docstring") gr_block::output_multiple "

Params: (NONE)"

%feature("docstring") gr_block::consume "Tell the scheduler  of input stream  were consumed.

Params: (which_input, how_many_items)"

%feature("docstring") gr_block::consume_each "Tell the scheduler  were consumed on each input stream.

Params: (how_many_items)"

%feature("docstring") gr_block::produce "Tell the scheduler  were produced on output stream .

If the block's general_work method calls produce,  must return WORK_CALLED_PRODUCE.

Params: (which_output, how_many_items)"

%feature("docstring") gr_block::set_relative_rate "Set the approximate output rate / input rate.

Provide a hint to the buffer allocator and scheduler. The default relative_rate is 1.0

decimators have relative_rates < 1.0 interpolators have relative_rates > 1.0

Params: (relative_rate)"

%feature("docstring") gr_block::relative_rate "return the approximate output rate / input rate

Params: (NONE)"

%feature("docstring") gr_block::fixed_rate_ninput_to_noutput "Given ninput samples, return number of output samples that will be produced. N.B. this is only defined if fixed_rate returns true. Generally speaking, you don't need to override this.

Params: (ninput)"

%feature("docstring") gr_block::fixed_rate_noutput_to_ninput "Given noutput samples, return number of input samples required to produce noutput. N.B. this is only defined if fixed_rate returns true. Generally speaking, you don't need to override this.

Params: (noutput)"

%feature("docstring") gr_block::nitems_read "Return the number of items read on input stream which_input.

Params: (which_input)"

%feature("docstring") gr_block::nitems_written "Return the number of items written on output stream which_output.

Params: (which_output)"

%feature("docstring") gr_block::tag_propagation_policy "Asks for the policy used by the scheduler to moved tags downstream.

Params: (NONE)"

%feature("docstring") gr_block::set_tag_propagation_policy "Set the policy by the scheduler to determine how tags are moved downstream.

Params: (p)"

%feature("docstring") gr_block::detail "

Params: (NONE)"

%feature("docstring") gr_block::set_detail "

Params: (detail)"

%feature("docstring") gr_block::gr_block "

Params: (name, input_signature, output_signature)"

%feature("docstring") gr_block::set_fixed_rate "

Params: (fixed_rate)"

%feature("docstring") gr_block::add_item_tag "Adds a new tag onto the given output buffer.

Params: (which_output, abs_offset, key, value, srcid)"

%feature("docstring") gr_block::get_tags_in_range "Given a [start,end), returns a vector of all tags in the range.

Range of counts is from start to end-1.

Tags are tuples of: (item count, source id, key, value)

Params: (v, which_input, abs_start, abs_end)"

%feature("docstring") gr_block_executor "Manage the execution of a single block."

%feature("docstring") gr_block_executor::gr_block_executor "

Params: (block)"

%feature("docstring") gr_block_executor::~gr_block_executor "

Params: (NONE)"

%feature("docstring") gr_block_executor::run_one_iteration "

Params: (NONE)"

%feature("docstring") gr_buffer_reader "How we keep track of the readers of a gr_buffer."

%feature("docstring") gr_buffer_reader::~gr_buffer_reader "

Params: (NONE)"

%feature("docstring") gr_buffer_reader::items_available "Return number of items available for reading.

Params: (NONE)"

%feature("docstring") gr_buffer_reader::buffer "Return buffer this reader reads from.

Params: (NONE)"

%feature("docstring") gr_buffer_reader::max_possible_items_available "Return maximum number of items that could ever be available for reading. This is used as a sanity check in the scheduler to avoid looping forever.

Params: (NONE)"

%feature("docstring") gr_buffer_reader::read_pointer "return pointer to read buffer.

The return value points to items_available() number of items

Params: (NONE)"

%feature("docstring") gr_buffer_reader::update_read_pointer "

Params: (nitems)"

%feature("docstring") gr_buffer_reader::set_done "

Params: (done)"

%feature("docstring") gr_buffer_reader::done "

Params: (NONE)"

%feature("docstring") gr_buffer_reader::mutex "

Params: (NONE)"

%feature("docstring") gr_buffer_reader::nitems_read "

Params: (NONE)"

%feature("docstring") gr_buffer_reader::link "Return the block that reads via this reader.

Params: (NONE)"

%feature("docstring") gr_buffer_reader::get_tags_in_range "Given a [start,end), returns a vector all tags in the range.

Get a vector of tags in given range. Range of counts is from start to end-1.

Tags are tuples of: (item count, source id, key, value)

Params: (v, abs_start, abs_end)"

%feature("docstring") gr_buffer_reader::gr_buffer_reader "constructor is private. Use gr_buffer::add_reader to create instances

Params: (buffer, read_index, link)"



%feature("docstring") gr_circular_file::gr_circular_file "

Params: (filename, writable, size)"

%feature("docstring") gr_circular_file::~gr_circular_file "

Params: (NONE)"

%feature("docstring") gr_circular_file::write "

Params: (data, nbytes)"

%feature("docstring") gr_circular_file::read "

Params: (data, nbytes)"

%feature("docstring") gr_circular_file::reset_read_pointer "

Params: (NONE)"



%feature("docstring") gr_constellation_sector::gr_constellation_sector "

Params: (constellation, n_sectors)"

%feature("docstring") gr_constellation_sector::decision_maker "Returns the constellation point that matches best. Also calculates the phase error.

Params: (sample)"

%feature("docstring") gr_constellation_sector::get_sector "

Params: (sample)"

%feature("docstring") gr_constellation_sector::calc_sector_value "

Params: (sector)"

%feature("docstring") gr_constellation_sector::find_sector_values "

Params: (NONE)"

%feature("docstring") gr_edge "Class representing a connection between to graph endpoints."

%feature("docstring") gr_edge::gr_edge "

Params: (NONE)"

%feature("docstring") gr_edge::~gr_edge "

Params: (NONE)"

%feature("docstring") gr_edge::src "

Params: (NONE)"

%feature("docstring") gr_edge::dst "

Params: (NONE)"

%feature("docstring") gr_endpoint "Class representing a specific input or output graph endpoint."

%feature("docstring") gr_endpoint::gr_endpoint "

Params: (NONE)"

%feature("docstring") gr_endpoint::block "

Params: (NONE)"

%feature("docstring") gr_endpoint::port "

Params: (NONE)"

%feature("docstring") gr_endpoint::operator== "

Params: (other)"

%feature("docstring") gr_error_handler "abstract error handler"

%feature("docstring") gr_error_handler::gr_error_handler "

Params: (NONE)"

%feature("docstring") gr_error_handler::~gr_error_handler "

Params: (NONE)"

%feature("docstring") gr_error_handler::message "

Params: (format, )"

%feature("docstring") gr_error_handler::warning "

Params: (format, )"

%feature("docstring") gr_error_handler::error "

Params: (format, )"

%feature("docstring") gr_error_handler::fatal "

Params: (format, )"

%feature("docstring") gr_error_handler::nwarnings "

Params: (NONE)"

%feature("docstring") gr_error_handler::nerrors "

Params: (NONE)"

%feature("docstring") gr_error_handler::reset_counts "

Params: (NONE)"

%feature("docstring") gr_error_handler::verror "

Params: (s, format, )"

%feature("docstring") gr_error_handler::verror_text "

Params: (s, text)"

%feature("docstring") gr_error_handler::default_handler "

Params: (NONE)"

%feature("docstring") gr_error_handler::silent_handler "

Params: (NONE)"

%feature("docstring") gr_error_handler::has_default_handler "

Params: (NONE)"

%feature("docstring") gr_error_handler::set_default_handler "

Params: (errh)"

%feature("docstring") gr_error_handler::count_error "

Params: (s)"

%feature("docstring") gr_error_handler::handle_text "

Params: (s, str)"

%feature("docstring") gr_error_handler::make_text "

Params: (s, format, )"

%feature("docstring") gr_feval "base class for evaluating a function: void -> void

This class is designed to be subclassed in Python or C++ and is callable from both places. It uses SWIG's \"director\" feature to implement the magic. It's slow. Don't use it in a performance critical path.

Override eval to define the behavior. Use calleval to invoke eval (this kludge is required to allow a python specific \"shim\" to be inserted."

%feature("docstring") gr_feval::eval "override this to define the function

Params: (NONE)"

%feature("docstring") gr_feval::gr_feval "

Params: (NONE)"

%feature("docstring") gr_feval::~gr_feval "

Params: (NONE)"

%feature("docstring") gr_feval::calleval "

Params: (NONE)"

%feature("docstring") gr_feval_cc "base class for evaluating a function: complex -> complex

This class is designed to be subclassed in Python or C++ and is callable from both places. It uses SWIG's \"director\" feature to implement the magic. It's slow. Don't use it in a performance critical path.

Override eval to define the behavior. Use calleval to invoke eval (this kludge is required to allow a python specific \"shim\" to be inserted."

%feature("docstring") gr_feval_cc::eval "override this to define the function

Params: (x)"

%feature("docstring") gr_feval_cc::gr_feval_cc "

Params: (NONE)"

%feature("docstring") gr_feval_cc::~gr_feval_cc "

Params: (NONE)"

%feature("docstring") gr_feval_cc::calleval "

Params: (x)"

%feature("docstring") gr_feval_dd "base class for evaluating a function: double -> double

This class is designed to be subclassed in Python or C++ and is callable from both places. It uses SWIG's \"director\" feature to implement the magic. It's slow. Don't use it in a performance critical path.

Override eval to define the behavior. Use calleval to invoke eval (this kludge is required to allow a python specific \"shim\" to be inserted."

%feature("docstring") gr_feval_dd::eval "override this to define the function

Params: (x)"

%feature("docstring") gr_feval_dd::gr_feval_dd "

Params: (NONE)"

%feature("docstring") gr_feval_dd::~gr_feval_dd "

Params: (NONE)"

%feature("docstring") gr_feval_dd::calleval "

Params: (x)"

%feature("docstring") gr_feval_ll "base class for evaluating a function: long -> long

This class is designed to be subclassed in Python or C++ and is callable from both places. It uses SWIG's \"director\" feature to implement the magic. It's slow. Don't use it in a performance critical path.

Override eval to define the behavior. Use calleval to invoke eval (this kludge is required to allow a python specific \"shim\" to be inserted."

%feature("docstring") gr_feval_ll::eval "override this to define the function

Params: (x)"

%feature("docstring") gr_feval_ll::gr_feval_ll "

Params: (NONE)"

%feature("docstring") gr_feval_ll::~gr_feval_ll "

Params: (NONE)"

%feature("docstring") gr_feval_ll::calleval "

Params: (x)"



%feature("docstring") gr_file_error_handler::gr_file_error_handler "

Params: (file)"

%feature("docstring") gr_file_error_handler::~gr_file_error_handler "

Params: (NONE)"

%feature("docstring") gr_file_error_handler::handle_text "

Params: (s, str)"

%feature("docstring") gr_file_sink_base "Common base class for file sinks."

%feature("docstring") gr_file_sink_base::gr_file_sink_base "

Params: (filename, is_binary)"

%feature("docstring") gr_file_sink_base::~gr_file_sink_base "

Params: (NONE)"

%feature("docstring") gr_file_sink_base::open "Open filename and begin output to it.

Params: (filename)"

%feature("docstring") gr_file_sink_base::close "Close current output file.

Closes current output file and ignores any output until open is called to connect to another file.

Params: (NONE)"

%feature("docstring") gr_file_sink_base::do_update "if we've had an update, do it now.

Params: (NONE)"

%feature("docstring") gr_file_sink_base::set_unbuffered "turn on unbuffered writes for slower outputs

Params: (unbuffered)"

%feature("docstring") gr_fir_ccc "Abstract class for FIR with gr_complex input, gr_complex output and gr_complex taps

This is the abstract class for a Finite Impulse Response filter.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_ccc::gr_fir_ccc "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (NONE)"

%feature("docstring") gr_fir_ccc::~gr_fir_ccc "

Params: (NONE)"

%feature("docstring") gr_fir_ccc::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_ccc::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_ccc::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_ccc::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_ccc::ntaps "

Params: (NONE)"

%feature("docstring") gr_fir_ccc::get_taps "

Params: (NONE)"

%feature("docstring") gr_fir_ccc_3dnow "3DNow! version of gr_fir_ccc"

%feature("docstring") gr_fir_ccc_3dnow::gr_fir_ccc_3dnow "

Params: (NONE)"



%feature("docstring") gr_fir_ccc_3dnowext::gr_fir_ccc_3dnowext "

Params: (NONE)"

%feature("docstring") gr_fir_ccc_generic "Concrete class for generic implementation of FIR with gr_complex input, gr_complex output and gr_complex taps.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_ccc_generic::gr_fir_ccc_generic "

Params: (NONE)"

%feature("docstring") gr_fir_ccc_generic::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_ccc_generic::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_ccc_generic::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_ccc_simd "common base class for SIMD versions of gr_fir_ccc

This base class handles alignment issues common to SSE and 3DNOW subclasses."

%feature("docstring") gr_fir_ccc_simd::gr_fir_ccc_simd "

Params: (NONE)"

%feature("docstring") gr_fir_ccc_simd::~gr_fir_ccc_simd "

Params: (NONE)"

%feature("docstring") gr_fir_ccc_simd::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_ccc_simd::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_ccc_sse "SSE version of gr_fir_ccc."

%feature("docstring") gr_fir_ccc_sse::gr_fir_ccc_sse "

Params: (NONE)"

%feature("docstring") gr_fir_ccf "Abstract class for FIR with gr_complex input, gr_complex output and float taps

This is the abstract class for a Finite Impulse Response filter.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_ccf::gr_fir_ccf "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (NONE)"

%feature("docstring") gr_fir_ccf::~gr_fir_ccf "

Params: (NONE)"

%feature("docstring") gr_fir_ccf::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_ccf::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_ccf::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_ccf::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_ccf::ntaps "

Params: (NONE)"

%feature("docstring") gr_fir_ccf::get_taps "

Params: (NONE)"

%feature("docstring") gr_fir_ccf_3dnow "3DNow! version of gr_fir_ccf"

%feature("docstring") gr_fir_ccf_3dnow::gr_fir_ccf_3dnow "

Params: (NONE)"

%feature("docstring") gr_fir_ccf_armv7_a "armv7_a using NEON coprocessor version of gr_fir_ccf"

%feature("docstring") gr_fir_ccf_armv7_a::gr_fir_ccf_armv7_a "

Params: (NONE)"

%feature("docstring") gr_fir_ccf_armv7_a::~gr_fir_ccf_armv7_a "

Params: (NONE)"

%feature("docstring") gr_fir_ccf_armv7_a::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_ccf_armv7_a::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_ccf_generic "Concrete class for generic implementation of FIR with gr_complex input, gr_complex output and float taps.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_ccf_generic::gr_fir_ccf_generic "

Params: (NONE)"

%feature("docstring") gr_fir_ccf_generic::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_ccf_generic::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_ccf_generic::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_ccf_simd "common base class for SIMD versions of gr_fir_ccf

This base class handles alignment issues common to SSE and 3DNOW subclasses."

%feature("docstring") gr_fir_ccf_simd::gr_fir_ccf_simd "

Params: (NONE)"

%feature("docstring") gr_fir_ccf_simd::~gr_fir_ccf_simd "

Params: (NONE)"

%feature("docstring") gr_fir_ccf_simd::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_ccf_simd::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_ccf_sse "SSE version of gr_fir_ccf."

%feature("docstring") gr_fir_ccf_sse::gr_fir_ccf_sse "

Params: (NONE)"

%feature("docstring") gr_fir_fcc "Abstract class for FIR with float input, gr_complex output and gr_complex taps

This is the abstract class for a Finite Impulse Response filter.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_fcc::gr_fir_fcc "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (NONE)"

%feature("docstring") gr_fir_fcc::~gr_fir_fcc "

Params: (NONE)"

%feature("docstring") gr_fir_fcc::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_fcc::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_fcc::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_fcc::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_fcc::ntaps "

Params: (NONE)"

%feature("docstring") gr_fir_fcc::get_taps "

Params: (NONE)"

%feature("docstring") gr_fir_fcc_3dnow "3DNow! version of gr_fir_fcc"

%feature("docstring") gr_fir_fcc_3dnow::gr_fir_fcc_3dnow "

Params: (NONE)"

%feature("docstring") gr_fir_fcc_generic "Concrete class for generic implementation of FIR with float input, gr_complex output and gr_complex taps.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_fcc_generic::gr_fir_fcc_generic "

Params: (NONE)"

%feature("docstring") gr_fir_fcc_generic::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_fcc_generic::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_fcc_generic::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_fcc_simd "common base class for SIMD versions of gr_fir_fcc

This base class handles alignment issues common to SSE and 3DNOW subclasses."

%feature("docstring") gr_fir_fcc_simd::gr_fir_fcc_simd "

Params: (NONE)"

%feature("docstring") gr_fir_fcc_simd::~gr_fir_fcc_simd "

Params: (NONE)"

%feature("docstring") gr_fir_fcc_simd::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_fcc_simd::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_fcc_sse "SSE version of gr_fir_fcc."

%feature("docstring") gr_fir_fcc_sse::gr_fir_fcc_sse "

Params: (NONE)"

%feature("docstring") gr_fir_fff "Abstract class for FIR with float input, float output and float taps

This is the abstract class for a Finite Impulse Response filter.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_fff::gr_fir_fff "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (NONE)"

%feature("docstring") gr_fir_fff::~gr_fir_fff "

Params: (NONE)"

%feature("docstring") gr_fir_fff::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_fff::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_fff::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_fff::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_fff::ntaps "

Params: (NONE)"

%feature("docstring") gr_fir_fff::get_taps "

Params: (NONE)"

%feature("docstring") gr_fir_fff_3dnow "3DNow! version of gr_fir_fff"

%feature("docstring") gr_fir_fff_3dnow::gr_fir_fff_3dnow "

Params: (NONE)"

%feature("docstring") gr_fir_fff_altivec "altivec version of gr_fir_fff"

%feature("docstring") gr_fir_fff_altivec::gr_fir_fff_altivec "

Params: (NONE)"

%feature("docstring") gr_fir_fff_altivec::~gr_fir_fff_altivec "

Params: (NONE)"

%feature("docstring") gr_fir_fff_altivec::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_fff_altivec::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_fff_armv7_a "armv7_a using NEON coprocessor version of gr_fir_fff"

%feature("docstring") gr_fir_fff_armv7_a::gr_fir_fff_armv7_a "

Params: (NONE)"

%feature("docstring") gr_fir_fff_armv7_a::~gr_fir_fff_armv7_a "

Params: (NONE)"

%feature("docstring") gr_fir_fff_armv7_a::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_fff_armv7_a::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_fff_generic "Concrete class for generic implementation of FIR with float input, float output and float taps.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_fff_generic::gr_fir_fff_generic "

Params: (NONE)"

%feature("docstring") gr_fir_fff_generic::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_fff_generic::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_fff_generic::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_fff_simd "common base class for SIMD versions of gr_fir_fff

This base class handles alignment issues common to SSE and 3DNOW subclasses."

%feature("docstring") gr_fir_fff_simd::gr_fir_fff_simd "

Params: (NONE)"

%feature("docstring") gr_fir_fff_simd::~gr_fir_fff_simd "

Params: (NONE)"

%feature("docstring") gr_fir_fff_simd::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_fff_simd::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_fff_sse "SSE version of gr_fir_fff."

%feature("docstring") gr_fir_fff_sse::gr_fir_fff_sse "

Params: (NONE)"

%feature("docstring") gr_fir_fsf "Abstract class for FIR with float input, short output and float taps

This is the abstract class for a Finite Impulse Response filter.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_fsf::gr_fir_fsf "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (NONE)"

%feature("docstring") gr_fir_fsf::~gr_fir_fsf "

Params: (NONE)"

%feature("docstring") gr_fir_fsf::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_fsf::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_fsf::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_fsf::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_fsf::ntaps "

Params: (NONE)"

%feature("docstring") gr_fir_fsf::get_taps "

Params: (NONE)"

%feature("docstring") gr_fir_fsf_3dnow "3DNow! version of gr_fir_fsf"

%feature("docstring") gr_fir_fsf_3dnow::gr_fir_fsf_3dnow "

Params: (NONE)"

%feature("docstring") gr_fir_fsf_generic "Concrete class for generic implementation of FIR with float input, short output and float taps.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_fsf_generic::gr_fir_fsf_generic "

Params: (NONE)"

%feature("docstring") gr_fir_fsf_generic::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_fsf_generic::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_fsf_generic::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_fsf_simd "common base class for SIMD versions of gr_fir_fsf

This base class handles alignment issues common to SSE and 3DNOW subclasses."

%feature("docstring") gr_fir_fsf_simd::gr_fir_fsf_simd "

Params: (NONE)"

%feature("docstring") gr_fir_fsf_simd::~gr_fir_fsf_simd "

Params: (NONE)"

%feature("docstring") gr_fir_fsf_simd::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_fsf_simd::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_fsf_sse "SSE version of gr_fir_fsf."

%feature("docstring") gr_fir_fsf_sse::gr_fir_fsf_sse "

Params: (NONE)"

%feature("docstring") gr_fir_scc "Abstract class for FIR with short input, gr_complex output and gr_complex taps

This is the abstract class for a Finite Impulse Response filter.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_scc::gr_fir_scc "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (NONE)"

%feature("docstring") gr_fir_scc::~gr_fir_scc "

Params: (NONE)"

%feature("docstring") gr_fir_scc::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_scc::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_scc::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_scc::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_scc::ntaps "

Params: (NONE)"

%feature("docstring") gr_fir_scc::get_taps "

Params: (NONE)"

%feature("docstring") gr_fir_scc_3dnow "3DNow! version of gr_fir_scc"

%feature("docstring") gr_fir_scc_3dnow::gr_fir_scc_3dnow "

Params: (NONE)"

%feature("docstring") gr_fir_scc_3dnowext "3DNow! Ext version of gr_fir_scc"

%feature("docstring") gr_fir_scc_3dnowext::gr_fir_scc_3dnowext "

Params: (NONE)"

%feature("docstring") gr_fir_scc_generic "Concrete class for generic implementation of FIR with short input, gr_complex output and gr_complex taps.

The trailing suffix has the form _IOT where I codes the input type, O codes the output type, and T codes the tap type. I,O,T are elements of the set 's' (short), 'f' (float), 'c' (gr_complex), 'i' (int)"

%feature("docstring") gr_fir_scc_generic::gr_fir_scc_generic "

Params: (NONE)"

%feature("docstring") gr_fir_scc_generic::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_scc_generic::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gr_fir_scc_generic::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gr_fir_scc_simd "common base class for SIMD versions of gr_fir_scc

This base class handles alignment issues common to SSE and 3DNOW subclasses."

%feature("docstring") gr_fir_scc_simd::gr_fir_scc_simd "

Params: (NONE)"

%feature("docstring") gr_fir_scc_simd::~gr_fir_scc_simd "

Params: (NONE)"

%feature("docstring") gr_fir_scc_simd::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gr_fir_scc_simd::filter "compute a single output value.

must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

Params: (input)"

%feature("docstring") gr_fir_scc_sse "SSE version of gr_fir_scc."

%feature("docstring") gr_fir_scc_sse::gr_fir_scc_sse "

Params: (NONE)"

%feature("docstring") gr_fir_sysconfig "abstract base class for configuring the automatic selection of the fastest gr_fir for your platform.

This is used internally by gr_fir_util."

%feature("docstring") gr_fir_sysconfig::~gr_fir_sysconfig "

Params: (NONE)"

%feature("docstring") gr_fir_sysconfig::create_gr_fir_ccf "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig::create_gr_fir_fcc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig::create_gr_fir_ccc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig::create_gr_fir_fff "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig::create_gr_fir_scc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig::create_gr_fir_fsf "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig::get_gr_fir_ccf_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig::get_gr_fir_fcc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig::get_gr_fir_ccc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig::get_gr_fir_fff_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig::get_gr_fir_scc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig::get_gr_fir_fsf_info "

Params: (info)"



%feature("docstring") gr_fir_sysconfig_armv7_a::create_gr_fir_ccf "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_armv7_a::create_gr_fir_fcc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_armv7_a::create_gr_fir_fff "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_armv7_a::create_gr_fir_fsf "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_armv7_a::create_gr_fir_scc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_armv7_a::create_gr_fir_ccc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_armv7_a::get_gr_fir_ccf_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_armv7_a::get_gr_fir_fcc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_armv7_a::get_gr_fir_fff_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_armv7_a::get_gr_fir_fsf_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_armv7_a::get_gr_fir_scc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_armv7_a::get_gr_fir_ccc_info "

Params: (info)"



%feature("docstring") gr_fir_sysconfig_generic::create_gr_fir_ccf "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_generic::create_gr_fir_fcc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_generic::create_gr_fir_ccc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_generic::create_gr_fir_fff "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_generic::create_gr_fir_scc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_generic::create_gr_fir_fsf "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_generic::get_gr_fir_ccf_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_generic::get_gr_fir_fcc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_generic::get_gr_fir_ccc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_generic::get_gr_fir_fff_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_generic::get_gr_fir_scc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_generic::get_gr_fir_fsf_info "

Params: (info)"



%feature("docstring") gr_fir_sysconfig_powerpc::create_gr_fir_ccf "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_powerpc::create_gr_fir_fcc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_powerpc::create_gr_fir_fff "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_powerpc::create_gr_fir_fsf "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_powerpc::create_gr_fir_scc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_powerpc::create_gr_fir_ccc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_powerpc::get_gr_fir_ccf_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_powerpc::get_gr_fir_fcc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_powerpc::get_gr_fir_fff_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_powerpc::get_gr_fir_fsf_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_powerpc::get_gr_fir_scc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_powerpc::get_gr_fir_ccc_info "

Params: (info)"



%feature("docstring") gr_fir_sysconfig_x86::create_gr_fir_ccf "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_x86::create_gr_fir_fcc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_x86::create_gr_fir_fff "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_x86::create_gr_fir_fsf "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_x86::create_gr_fir_scc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_x86::create_gr_fir_ccc "

Params: (taps)"

%feature("docstring") gr_fir_sysconfig_x86::get_gr_fir_ccf_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_x86::get_gr_fir_fcc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_x86::get_gr_fir_fff_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_x86::get_gr_fir_fsf_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_x86::get_gr_fir_scc_info "

Params: (info)"

%feature("docstring") gr_fir_sysconfig_x86::get_gr_fir_ccc_info "

Params: (info)"

%feature("docstring") gr_firdes "Finite Impulse Response (FIR) filter design functions."

%feature("docstring") gr_firdes::low_pass "use \"window method\" to design a low-pass FIR filter

overall gain of filter (typically 1.0)  sampling freq (Hz)  center of transition band (Hz)  width of transition band (Hz). The normalized width of the transition band is what sets the number of taps required. Narrow --> more taps  What kind of window to use. Determines maximum attenuation and passband ripple.  parameter for Kaiser window

Params: (gain, sampling_freq, cutoff_freq, transition_width, window, beta)"

%feature("docstring") gr_firdes::low_pass_2 "use \"window method\" to design a low-pass FIR filter

overall gain of filter (typically 1.0)  sampling freq (Hz)  center of transition band (Hz)  width of transition band (Hz).  required stopband attenuation The normalized width of the transition band and the required stop band attenuation is what sets the number of taps required. Narrow --> more taps More attenuatin --> more taps  What kind of window to use. Determines maximum attenuation and passband ripple.  parameter for Kaiser window

Params: (gain, sampling_freq, cutoff_freq, transition_width, attenuation_dB, window, beta)"

%feature("docstring") gr_firdes::high_pass "use \"window method\" to design a high-pass FIR filter

overall gain of filter (typically 1.0)  sampling freq (Hz)  center of transition band (Hz)  width of transition band (Hz). The normalized width of the transition band is what sets the number of taps required. Narrow --> more taps  What kind of window to use. Determines maximum attenuation and passband ripple.  parameter for Kaiser window

Params: (gain, sampling_freq, cutoff_freq, transition_width, window, beta)"

%feature("docstring") gr_firdes::high_pass_2 "use \"window method\" to design a high-pass FIR filter

overall gain of filter (typically 1.0)  sampling freq (Hz)  center of transition band (Hz)  width of transition band (Hz).  out of band attenuation The normalized width of the transition band and the required stop band attenuation is what sets the number of taps required. Narrow --> more taps More attenuation --> more taps  What kind of window to use. Determines maximum attenuation and passband ripple.  parameter for Kaiser window

Params: (gain, sampling_freq, cutoff_freq, transition_width, attenuation_dB, window, beta)"

%feature("docstring") gr_firdes::band_pass "use \"window method\" to design a band-pass FIR filter

overall gain of filter (typically 1.0)  sampling freq (Hz)  center of transition band (Hz)  center of transition band (Hz)  width of transition band (Hz). The normalized width of the transition band is what sets the number of taps required. Narrow --> more taps  What kind of window to use. Determines maximum attenuation and passband ripple.  parameter for Kaiser window

Params: (gain, sampling_freq, low_cutoff_freq, high_cutoff_freq, transition_width, window, beta)"

%feature("docstring") gr_firdes::band_pass_2 "use \"window method\" to design a band-pass FIR filter

overall gain of filter (typically 1.0)  sampling freq (Hz)  center of transition band (Hz)  center of transition band (Hz)  width of transition band (Hz).  out of band attenuation The normalized width of the transition band and the required stop band attenuation is what sets the number of taps required. Narrow --> more taps More attenuation --> more taps  What kind of window to use. Determines maximum attenuation and passband ripple.  parameter for Kaiser window

Params: (gain, sampling_freq, low_cutoff_freq, high_cutoff_freq, transition_width, attenuation_dB, window, beta)"

%feature("docstring") gr_firdes::complex_band_pass "use \"window method\" to design a complex band-pass FIR filter

overall gain of filter (typically 1.0)  sampling freq (Hz)  center of transition band (Hz)  center of transition band (Hz)  width of transition band (Hz). The normalized width of the transition band is what sets the number of taps required. Narrow --> more taps  What kind of window to use. Determines maximum attenuation and passband ripple.  parameter for Kaiser window

Params: (gain, sampling_freq, low_cutoff_freq, high_cutoff_freq, transition_width, window, beta)"

%feature("docstring") gr_firdes::complex_band_pass_2 "use \"window method\" to design a complex band-pass FIR filter

overall gain of filter (typically 1.0)  sampling freq (Hz)  center of transition band (Hz)  center of transition band (Hz)  width of transition band (Hz).  out of band attenuation The normalized width of the transition band and the required stop band attenuation is what sets the number of taps required. Narrow --> more taps More attenuation --> more taps  What kind of window to use. Determines maximum attenuation and passband ripple.  parameter for Kaiser window

Params: (gain, sampling_freq, low_cutoff_freq, high_cutoff_freq, transition_width, attenuation_dB, window, beta)"

%feature("docstring") gr_firdes::band_reject "use \"window method\" to design a band-reject FIR filter

overall gain of filter (typically 1.0)  sampling freq (Hz)  center of transition band (Hz)  center of transition band (Hz)  width of transition band (Hz). The normalized width of the transition band is what sets the number of taps required. Narrow --> more taps  What kind of window to use. Determines maximum attenuation and passband ripple.  parameter for Kaiser window

Params: (gain, sampling_freq, low_cutoff_freq, high_cutoff_freq, transition_width, window, beta)"

%feature("docstring") gr_firdes::band_reject_2 "use \"window method\" to design a band-reject FIR filter

overall gain of filter (typically 1.0)  sampling freq (Hz)  center of transition band (Hz)  center of transition band (Hz)  width of transition band (Hz).  out of band attenuation The normalized width of the transition band and the required stop band attenuation is what sets the number of taps required. Narrow --> more taps More attenuation --> more taps  What kind of window to use. Determines maximum attenuation and passband ripple.  parameter for Kaiser window

Params: (gain, sampling_freq, low_cutoff_freq, high_cutoff_freq, transition_width, attenuation_dB, window, beta)"

%feature("docstring") gr_firdes::hilbert "design a Hilbert Transform Filter

Number of taps, must be odd  What kind of window to use  Only used for Kaiser

Params: (ntaps, windowtype, beta)"

%feature("docstring") gr_firdes::root_raised_cosine "design a Root Cosine FIR Filter (do we need a window?)

overall gain of filter (typically 1.0)  sampling freq (Hz)  rate: symbol rate, must be a factor of sample rate  excess bandwidth factor  number of taps

Params: (gain, sampling_freq, symbol_rate, alpha, ntaps)"

%feature("docstring") gr_firdes::gaussian "design a Gaussian filter

overall gain of filter (typically 1.0)  per bit: symbol rate, must be a factor of sample rate  number of taps

Params: (gain, spb, bt, ntaps)"

%feature("docstring") gr_firdes::window "

Params: (type, ntaps, beta)"

%feature("docstring") gr_firdes::bessi0 "

Params: (x)"

%feature("docstring") gr_firdes::sanity_check_1f "

Params: (sampling_freq, f1, transition_width)"

%feature("docstring") gr_firdes::sanity_check_2f "

Params: (sampling_freq, f1, f2, transition_width)"

%feature("docstring") gr_firdes::sanity_check_2f_c "

Params: (sampling_freq, f1, f2, transition_width)"

%feature("docstring") gr_firdes::compute_ntaps "

Params: (sampling_freq, transition_width, window_type, beta)"

%feature("docstring") gr_firdes::compute_ntaps_windes "

Params: (sampling_freq, transition_width, attenuation_dB)"

%feature("docstring") gr_fxpt "fixed point sine and cosine and friends.

fixed pt radians --------- -------- -2**31 -pi 0 0 2**31-1 pi - epsilon"

%feature("docstring") gr_fxpt::float_to_fixed "

Params: (x)"

%feature("docstring") gr_fxpt::fixed_to_float "

Params: (x)"

%feature("docstring") gr_fxpt::sin "Given a fixed point angle x, return float sine (x).

Params: (x)"

%feature("docstring") gr_fxpt::cos "

Params: (x)"

%feature("docstring") gr_fxpt_nco "Numerically Controlled Oscillator (NCO)."

%feature("docstring") gr_fxpt_nco::gr_fxpt_nco "

Params: (NONE)"

%feature("docstring") gr_fxpt_nco::~gr_fxpt_nco "

Params: (NONE)"

%feature("docstring") gr_fxpt_nco::set_phase "

Params: (angle)"

%feature("docstring") gr_fxpt_nco::adjust_phase "

Params: (delta_phase)"

%feature("docstring") gr_fxpt_nco::set_freq "

Params: (angle_rate)"

%feature("docstring") gr_fxpt_nco::adjust_freq "

Params: (delta_angle_rate)"

%feature("docstring") gr_fxpt_nco::step "

Params: (NONE)"

%feature("docstring") gr_fxpt_nco::get_phase "

Params: (NONE)"

%feature("docstring") gr_fxpt_nco::get_freq "

Params: (NONE)"

%feature("docstring") gr_fxpt_nco::sincos "

Params: (sinx, cosx)"

%feature("docstring") gr_fxpt_nco::sin "

Params: (output, noutput_items, ampl)"

%feature("docstring") gr_fxpt_nco::cos "

Params: (output, noutput_items, ampl)"

%feature("docstring") gr_fxpt_vco "Voltage Controlled Oscillator (VCO)."

%feature("docstring") gr_fxpt_vco::gr_fxpt_vco "

Params: (NONE)"

%feature("docstring") gr_fxpt_vco::~gr_fxpt_vco "

Params: (NONE)"

%feature("docstring") gr_fxpt_vco::set_phase "

Params: (angle)"

%feature("docstring") gr_fxpt_vco::adjust_phase "

Params: (delta_phase)"

%feature("docstring") gr_fxpt_vco::get_phase "

Params: (NONE)"

%feature("docstring") gr_fxpt_vco::sincos "

Params: (sinx, cosx)"

%feature("docstring") gr_fxpt_vco::cos "

Params: (output, input, noutput_items, k, ampl)"

%feature("docstring") gr_fxpt_vco::sin "

Params: (NONE)"



%feature("docstring") gr_hier_block2_detail::gr_hier_block2_detail "

Params: (owner)"

%feature("docstring") gr_hier_block2_detail::~gr_hier_block2_detail "

Params: (NONE)"

%feature("docstring") gr_hier_block2_detail::connect "

Params: (block)"

%feature("docstring") gr_hier_block2_detail::disconnect "

Params: (block)"

%feature("docstring") gr_hier_block2_detail::disconnect_all "

Params: (NONE)"

%feature("docstring") gr_hier_block2_detail::lock "

Params: (NONE)"

%feature("docstring") gr_hier_block2_detail::unlock "

Params: (NONE)"

%feature("docstring") gr_hier_block2_detail::flatten_aux "

Params: (sfg)"

%feature("docstring") gr_hier_block2_detail::connect_input "

Params: (my_port, port, block)"

%feature("docstring") gr_hier_block2_detail::connect_output "

Params: (my_port, port, block)"

%feature("docstring") gr_hier_block2_detail::disconnect_input "

Params: (my_port, port, block)"

%feature("docstring") gr_hier_block2_detail::disconnect_output "

Params: (my_port, port, block)"

%feature("docstring") gr_hier_block2_detail::resolve_port "

Params: (port, is_input)"

%feature("docstring") gr_hier_block2_detail::resolve_endpoint "

Params: (endp, is_input)"

%feature("docstring") gr_io_signature "i/o signature for input and output ports.

misc"

%feature("docstring") gr_io_signature::gr_io_signature "

Params: (min_streams, max_streams, sizeof_stream_items)"

%feature("docstring") gr_io_signature::~gr_io_signature "

Params: (NONE)"

%feature("docstring") gr_io_signature::min_streams "

Params: (NONE)"

%feature("docstring") gr_io_signature::max_streams "

Params: (NONE)"

%feature("docstring") gr_io_signature::sizeof_stream_item "

Params: (index)"

%feature("docstring") gr_io_signature::sizeof_stream_items "

Params: (NONE)"

%feature("docstring") gr_local_sighandler "Get and set signal handler.

Constructor installs new handler, destructor reinstalls original value."

%feature("docstring") gr_local_sighandler::gr_local_sighandler "

Params: (signum, new_handler)"

%feature("docstring") gr_local_sighandler::~gr_local_sighandler "

Params: (NONE)"

%feature("docstring") gr_local_sighandler::throw_signal "

Params: (signum)"

%feature("docstring") gr_msg_accepter "Accepts messages and inserts them into a message queue, then notifies subclass gr_basic_block there is a message pending."

%feature("docstring") gr_msg_accepter::gr_msg_accepter "

Params: (NONE)"

%feature("docstring") gr_msg_accepter::~gr_msg_accepter "

Params: (NONE)"

%feature("docstring") gr_msg_accepter::post "send  to

Sending a message is an asynchronous operation. The  call will not wait for the message either to arrive at the destination or to be received.

Params: (msg)"

%feature("docstring") gr_msg_handler "abstract class of message handlers"

%feature("docstring") gr_msg_handler::~gr_msg_handler "

Params: (NONE)"

%feature("docstring") gr_msg_handler::handle "handle

Params: (msg)"

%feature("docstring") gr_msg_queue "thread-safe message queue"

%feature("docstring") gr_msg_queue::gr_msg_queue "

Params: (limit)"

%feature("docstring") gr_msg_queue::~gr_msg_queue "

Params: (NONE)"

%feature("docstring") gr_msg_queue::handle "Generic msg_handler method: insert the message.

Params: (msg)"

%feature("docstring") gr_msg_queue::insert_tail "Insert message at tail of queue.

Block if queue if full.

Params: (msg)"

%feature("docstring") gr_msg_queue::delete_head "Delete message from head of queue and return it. Block if no message is available.

Params: (NONE)"

%feature("docstring") gr_msg_queue::delete_head_nowait "If there's a message in the q, delete it and return it. If no message is available, return 0.

Params: (NONE)"

%feature("docstring") gr_msg_queue::flush "Delete all messages from the queue.

Params: (NONE)"

%feature("docstring") gr_msg_queue::empty_p "is the queue empty?

Params: (NONE)"

%feature("docstring") gr_msg_queue::full_p "is the queue full?

Params: (NONE)"

%feature("docstring") gr_msg_queue::count "return number of messages in queue

Params: (NONE)"

%feature("docstring") gr_msg_queue::limit "return limit on number of message in queue. 0 -> unbounded

Params: (NONE)"

%feature("docstring") gr_nco "base class template for Numerically Controlled Oscillator (NCO)"

%feature("docstring") gr_nco::gr_nco "

Params: (NONE)"

%feature("docstring") gr_nco::~gr_nco "

Params: (NONE)"

%feature("docstring") gr_nco::set_phase "

Params: (angle)"

%feature("docstring") gr_nco::adjust_phase "

Params: (delta_phase)"

%feature("docstring") gr_nco::set_freq "

Params: (angle_rate)"

%feature("docstring") gr_nco::adjust_freq "

Params: (delta_angle_rate)"

%feature("docstring") gr_nco::step "

Params: (NONE)"

%feature("docstring") gr_nco::get_phase "

Params: (NONE)"

%feature("docstring") gr_nco::get_freq "

Params: (NONE)"

%feature("docstring") gr_nco::sincos "

Params: (sinx, cosx)"

%feature("docstring") gr_nco::cos "

Params: (NONE)"

%feature("docstring") gr_nco::sin "

Params: (NONE)"

%feature("docstring") gr_oscope_guts "guts of oscilloscope trigger and buffer module

This module processes sets of samples provided the  method. When appropriate given the updateRate, sampleRate and trigger conditions, process_sample will periodically write output records of captured data to output_fd. For each trigger event, nchannels records will be written. Each record consists of get_samples_per_output_record binary floats. The trigger instant occurs at the 1/2 way point in the buffer. Thus, output records consist of 50% pre-trigger data and 50% post-trigger data."

%feature("docstring") gr_oscope_guts::gr_oscope_guts "

Params: (rhs)"

%feature("docstring") gr_oscope_guts::operator= "

Params: (rhs)"

%feature("docstring") gr_oscope_guts::trigger_changed "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::update_rate_or_decimation_changed "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::found_trigger "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::write_output_records "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::enter_hold_off "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::enter_look_for_trigger "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::enter_post_trigger "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::~gr_oscope_guts "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::process_sample "points to nchannels float values. These are the values for each channel at this sample time.

Params: (channel_data)"

%feature("docstring") gr_oscope_guts::set_update_rate "

Params: (update_rate)"

%feature("docstring") gr_oscope_guts::set_decimation_count "

Params: (decimation_count)"

%feature("docstring") gr_oscope_guts::set_trigger_channel "

Params: (channel)"

%feature("docstring") gr_oscope_guts::set_trigger_mode "

Params: (mode)"

%feature("docstring") gr_oscope_guts::set_trigger_slope "

Params: (slope)"

%feature("docstring") gr_oscope_guts::set_trigger_level "

Params: (trigger_level)"

%feature("docstring") gr_oscope_guts::set_trigger_level_auto "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::set_sample_rate "

Params: (sample_rate)"

%feature("docstring") gr_oscope_guts::set_num_channels "

Params: (nchannels)"

%feature("docstring") gr_oscope_guts::num_channels "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::sample_rate "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::update_rate "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::get_decimation_count "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::get_trigger_channel "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::get_trigger_mode "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::get_trigger_slope "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::get_trigger_level "

Params: (NONE)"

%feature("docstring") gr_oscope_guts::get_samples_per_output_record "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_x "Abstract class for python oscilloscope module.

Don't instantiate this. Use gr_oscope_sink_f or gr_oscope_sink_c instead."

%feature("docstring") gr_oscope_sink_x::gr_oscope_sink_x "

Params: (name, input_sig, sampling_rate)"

%feature("docstring") gr_oscope_sink_x::~gr_oscope_sink_x "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_x::set_update_rate "

Params: (update_rate)"

%feature("docstring") gr_oscope_sink_x::set_decimation_count "

Params: (decimation_count)"

%feature("docstring") gr_oscope_sink_x::set_trigger_channel "

Params: (channel)"

%feature("docstring") gr_oscope_sink_x::set_trigger_mode "

Params: (mode)"

%feature("docstring") gr_oscope_sink_x::set_trigger_slope "

Params: (slope)"

%feature("docstring") gr_oscope_sink_x::set_trigger_level "

Params: (trigger_level)"

%feature("docstring") gr_oscope_sink_x::set_trigger_level_auto "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_x::set_sample_rate "

Params: (sample_rate)"

%feature("docstring") gr_oscope_sink_x::set_num_channels "

Params: (nchannels)"

%feature("docstring") gr_oscope_sink_x::num_channels "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_x::sample_rate "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_x::update_rate "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_x::get_decimation_count "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_x::get_trigger_channel "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_x::get_trigger_mode "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_x::get_trigger_slope "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_x::get_trigger_level "

Params: (NONE)"

%feature("docstring") gr_oscope_sink_x::get_samples_per_output_record "

Params: (NONE)"



%feature("docstring") gr_preferences::get "

Params: (key)"

%feature("docstring") gr_preferences::set "

Params: (key, value)"

%feature("docstring") gr_prefs "Base class for representing user preferences a la windows INI files.

The real implementation is in Python, and is accessable from C++ via the magic of SWIG directors."

%feature("docstring") gr_prefs::singleton "

Params: (NONE)"

%feature("docstring") gr_prefs::set_singleton "

Params: (p)"

%feature("docstring") gr_prefs::~gr_prefs "

Params: (NONE)"

%feature("docstring") gr_prefs::has_section "Does  exist?

Params: (section)"

%feature("docstring") gr_prefs::has_option "Does  exist?

Params: (section, option)"

%feature("docstring") gr_prefs::get_string "If option exists return associated value; else default_val.

Params: (section, option, default_val)"

%feature("docstring") gr_prefs::get_bool "If option exists and value can be converted to bool, return it; else default_val.

Params: (section, option, default_val)"

%feature("docstring") gr_prefs::get_long "If option exists and value can be converted to long, return it; else default_val.

Params: (section, option, default_val)"

%feature("docstring") gr_prefs::get_double "If option exists and value can be converted to double, return it; else default_val.

Params: (section, option, default_val)"

%feature("docstring") gr_random "pseudo random number generator"

%feature("docstring") gr_random::gr_random "

Params: (seed)"

%feature("docstring") gr_random::reseed "

Params: (seed)"

%feature("docstring") gr_random::ran1 "uniform random deviate in the range [0.0, 1.0)

Params: (NONE)"

%feature("docstring") gr_random::gasdev "normally distributed deviate with zero mean and variance 1

Params: (NONE)"

%feature("docstring") gr_random::laplacian "

Params: (NONE)"

%feature("docstring") gr_random::impulse "

Params: (factor)"

%feature("docstring") gr_random::rayleigh "

Params: (NONE)"

%feature("docstring") gr_random::rayleigh_complex "

Params: (NONE)"



%feature("docstring") gr_rotator::gr_rotator "

Params: (NONE)"

%feature("docstring") gr_rotator::set_phase "

Params: (phase)"

%feature("docstring") gr_rotator::set_phase_incr "

Params: (incr)"

%feature("docstring") gr_rotator::rotate "

Params: (in)"

%feature("docstring") gr_scheduler "Abstract scheduler that takes a flattened flow graph and runs it.

Preconditions: details, buffers and buffer readers have been assigned."

%feature("docstring") gr_scheduler::gr_scheduler "Construct a scheduler and begin evaluating the graph.

The scheduler will continue running until all blocks until they report that they are done or the stop method is called.

Params: (ffg)"

%feature("docstring") gr_scheduler::~gr_scheduler "

Params: (NONE)"

%feature("docstring") gr_scheduler::stop "Tell the scheduler to stop executing.

Params: (NONE)"

%feature("docstring") gr_scheduler::wait "Block until the graph is done.

Params: (NONE)"

%feature("docstring") gr_scheduler_sts "Concrete scheduler that uses the single_threaded_scheduler."

%feature("docstring") gr_scheduler_sts::gr_scheduler_sts "Construct a scheduler and begin evaluating the graph.

The scheduler will continue running until all blocks until they report that they are done or the stop method is called.

Params: (ffg)"

%feature("docstring") gr_scheduler_sts::make "

Params: (ffg)"

%feature("docstring") gr_scheduler_sts::~gr_scheduler_sts "

Params: (NONE)"

%feature("docstring") gr_scheduler_sts::stop "Tell the scheduler to stop executing.

Params: (NONE)"

%feature("docstring") gr_scheduler_sts::wait "Block until the graph is done.

Params: (NONE)"

%feature("docstring") gr_scheduler_tpb "Concrete scheduler that uses a kernel thread-per-block."

%feature("docstring") gr_scheduler_tpb::gr_scheduler_tpb "Construct a scheduler and begin evaluating the graph.

The scheduler will continue running until all blocks until they report that they are done or the stop method is called.

Params: (ffg)"

%feature("docstring") gr_scheduler_tpb::make "

Params: (ffg)"

%feature("docstring") gr_scheduler_tpb::~gr_scheduler_tpb "

Params: (NONE)"

%feature("docstring") gr_scheduler_tpb::stop "Tell the scheduler to stop executing.

Params: (NONE)"

%feature("docstring") gr_scheduler_tpb::wait "Block until the graph is done.

Params: (NONE)"

%feature("docstring") gr_select_handler "Abstract handler for select based notification."

%feature("docstring") gr_select_handler::gr_select_handler "

Params: (file_descriptor)"

%feature("docstring") gr_select_handler::~gr_select_handler "

Params: (NONE)"

%feature("docstring") gr_select_handler::fd "

Params: (NONE)"

%feature("docstring") gr_select_handler::file_descriptor "

Params: (NONE)"

%feature("docstring") gr_select_handler::handle_read "Called when file_descriptor is readable.

Called when the dispatcher detects that file_descriptor can be read without blocking.

Params: (NONE)"

%feature("docstring") gr_select_handler::handle_write "Called when file_descriptor is writable.

Called when dispatcher detects that file descriptor can be written without blocking.

Params: (NONE)"

%feature("docstring") gr_select_handler::readable "Called each time around the dispatcher loop to determine whether this handler's file descriptor should be added to the list on which read events can occur. The default method returns true, indicating that by default, all handlers are interested in read events.

Params: (NONE)"

%feature("docstring") gr_select_handler::writable "Called each time around the dispatcher loop to determine whether this handler's file descriptor should be added to the list on which write events can occur. The default method returns true, indicating that by default, all handlers are interested in write events.

Params: (NONE)"

%feature("docstring") gr_signal "Representation of signal."

%feature("docstring") gr_signal::gr_signal "

Params: (signum)"

%feature("docstring") gr_signal::signal "

Params: (NONE)"

%feature("docstring") gr_signal::name "

Params: (NONE)"

%feature("docstring") gr_single_pole_iir "class template for single pole IIR filter"

%feature("docstring") gr_single_pole_iir::gr_single_pole_iir "construct new single pole IIR with given alpha

computes y(i) = (1-alpha) * y(i-1) + alpha * x(i)

Params: (alpha)"

%feature("docstring") gr_single_pole_iir::filter "compute a single output value.

Params: (input)"

%feature("docstring") gr_single_pole_iir::filterN "compute an array of N output values.  must have n valid entries.

Params: (output, input, n)"

%feature("docstring") gr_single_pole_iir::set_taps "install  as the current taps.

Params: (alpha)"

%feature("docstring") gr_single_pole_iir::reset "reset state to zero

Params: (NONE)"

%feature("docstring") gr_single_pole_iir::prev_output "

Params: (NONE)"



%feature("docstring") gr_single_pole_iir< gr_complex, i_type, double >::gr_single_pole_iir "construct new single pole IIR with given alpha

computes y(i) = (1-alpha) * y(i-1) + alpha * x(i)

Params: (alpha)"

%feature("docstring") gr_single_pole_iir< gr_complex, i_type, double >::filter "compute a single output value.

Params: (input)"

%feature("docstring") gr_single_pole_iir< gr_complex, i_type, double >::filterN "compute an array of N output values.  must have n valid entries.

Params: (output, input, n)"

%feature("docstring") gr_single_pole_iir< gr_complex, i_type, double >::set_taps "install  as the current taps.

Params: (alpha)"

%feature("docstring") gr_single_pole_iir< gr_complex, i_type, double >::reset "reset state to zero

Params: (NONE)"

%feature("docstring") gr_single_pole_iir< gr_complex, i_type, double >::prev_output "

Params: (NONE)"



%feature("docstring") gr_squelch_base_cc::update_state "

Params: (sample)"

%feature("docstring") gr_squelch_base_cc::mute "

Params: (NONE)"

%feature("docstring") gr_squelch_base_cc::gr_squelch_base_cc "

Params: (name, ramp, gate)"

%feature("docstring") gr_squelch_base_cc::ramp "

Params: (NONE)"

%feature("docstring") gr_squelch_base_cc::set_ramp "

Params: (ramp)"

%feature("docstring") gr_squelch_base_cc::gate "

Params: (NONE)"

%feature("docstring") gr_squelch_base_cc::set_gate "

Params: (gate)"

%feature("docstring") gr_squelch_base_cc::unmuted "

Params: (NONE)"

%feature("docstring") gr_squelch_base_cc::squelch_range "

Params: (NONE)"

%feature("docstring") gr_squelch_base_cc::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"



%feature("docstring") gr_squelch_base_ff::update_state "

Params: (sample)"

%feature("docstring") gr_squelch_base_ff::mute "

Params: (NONE)"

%feature("docstring") gr_squelch_base_ff::gr_squelch_base_ff "

Params: (name, ramp, gate)"

%feature("docstring") gr_squelch_base_ff::ramp "

Params: (NONE)"

%feature("docstring") gr_squelch_base_ff::set_ramp "

Params: (ramp)"

%feature("docstring") gr_squelch_base_ff::gate "

Params: (NONE)"

%feature("docstring") gr_squelch_base_ff::set_gate "

Params: (gate)"

%feature("docstring") gr_squelch_base_ff::unmuted "

Params: (NONE)"

%feature("docstring") gr_squelch_base_ff::squelch_range "

Params: (NONE)"

%feature("docstring") gr_squelch_base_ff::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_sync_block "synchronous 1:1 input to output with history

Override work to provide the signal processing implementation."

%feature("docstring") gr_sync_block::gr_sync_block "

Params: (name, input_signature, output_signature)"

%feature("docstring") gr_sync_block::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr_sync_block::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_sync_block::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_sync_block::fixed_rate_ninput_to_noutput "Given ninput samples, return number of output samples that will be produced. N.B. this is only defined if fixed_rate returns true. Generally speaking, you don't need to override this.

Params: (ninput)"

%feature("docstring") gr_sync_block::fixed_rate_noutput_to_ninput "Given noutput samples, return number of input samples required to produce noutput. N.B. this is only defined if fixed_rate returns true. Generally speaking, you don't need to override this.

Params: (noutput)"

%feature("docstring") gr_sync_decimator "synchronous N:1 input to output with history

Override work to provide the signal processing implementation."

%feature("docstring") gr_sync_decimator::gr_sync_decimator "

Params: (name, input_signature, output_signature, decimation)"

%feature("docstring") gr_sync_decimator::decimation "

Params: (NONE)"

%feature("docstring") gr_sync_decimator::set_decimation "

Params: (decimation)"

%feature("docstring") gr_sync_decimator::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_sync_decimator::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_sync_decimator::fixed_rate_ninput_to_noutput "Given ninput samples, return number of output samples that will be produced. N.B. this is only defined if fixed_rate returns true. Generally speaking, you don't need to override this.

Params: (ninput)"

%feature("docstring") gr_sync_decimator::fixed_rate_noutput_to_ninput "Given noutput samples, return number of input samples required to produce noutput. N.B. this is only defined if fixed_rate returns true. Generally speaking, you don't need to override this.

Params: (noutput)"

%feature("docstring") gr_sync_interpolator "synchronous 1:N input to output with history

Override work to provide the signal processing implementation."

%feature("docstring") gr_sync_interpolator::gr_sync_interpolator "

Params: (name, input_signature, output_signature, interpolation)"

%feature("docstring") gr_sync_interpolator::interpolation "

Params: (NONE)"

%feature("docstring") gr_sync_interpolator::set_interpolation "

Params: (interpolation)"

%feature("docstring") gr_sync_interpolator::forecast "Estimate input requirements given output request.

Given a request to product , estimate the number of data items required on each input stream. The estimate doesn't have to be exact, but should be close.

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr_sync_interpolator::general_work "compute output items from input items

general_work must call consume or consume_each to indicate how many items were consumed on each input stream.

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr_sync_interpolator::fixed_rate_ninput_to_noutput "Given ninput samples, return number of output samples that will be produced. N.B. this is only defined if fixed_rate returns true. Generally speaking, you don't need to override this.

Params: (ninput)"

%feature("docstring") gr_sync_interpolator::fixed_rate_noutput_to_ninput "Given noutput samples, return number of input samples required to produce noutput. N.B. this is only defined if fixed_rate returns true. Generally speaking, you don't need to override this.

Params: (noutput)"

%feature("docstring") gr_top_block_impl "Abstract implementation details of gr_top_block

The actual implementation of gr_top_block. Separate class allows decoupling of changes from dependent classes."

%feature("docstring") gr_top_block_impl::gr_top_block_impl "

Params: (owner)"

%feature("docstring") gr_top_block_impl::~gr_top_block_impl "

Params: (NONE)"

%feature("docstring") gr_top_block_impl::start "

Params: (NONE)"

%feature("docstring") gr_top_block_impl::stop "

Params: (NONE)"

%feature("docstring") gr_top_block_impl::wait "

Params: (NONE)"

%feature("docstring") gr_top_block_impl::lock "

Params: (NONE)"

%feature("docstring") gr_top_block_impl::unlock "

Params: (NONE)"

%feature("docstring") gr_top_block_impl::dump "

Params: (NONE)"

%feature("docstring") gr_top_block_impl::restart "

Params: (NONE)"

%feature("docstring") gr_tpb_thread_body "The body of each thread-per-block thread.

One of these is instantiated in its own thread for each block. The constructor turns into the main loop which returns when the block is done or is interrupted."

%feature("docstring") gr_tpb_thread_body::gr_tpb_thread_body "

Params: (block)"

%feature("docstring") gr_tpb_thread_body::~gr_tpb_thread_body "

Params: (NONE)"

%feature("docstring") gr_vco "base class template for Voltage Controlled Oscillator (VCO)"

%feature("docstring") gr_vco::gr_vco "

Params: (NONE)"

%feature("docstring") gr_vco::~gr_vco "

Params: (NONE)"

%feature("docstring") gr_vco::set_phase "

Params: (angle)"

%feature("docstring") gr_vco::adjust_phase "

Params: (delta_phase)"

%feature("docstring") gr_vco::get_phase "

Params: (NONE)"

%feature("docstring") gr_vco::sincos "

Params: (sinx, cosx)"

%feature("docstring") gr_vco::cos "

Params: (NONE)"

%feature("docstring") gr_vco::sin "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf "abstract class to implement doubly mapped virtual memory circular buffers"

%feature("docstring") gr_vmcircbuf::gr_vmcircbuf "

Params: (size)"

%feature("docstring") gr_vmcircbuf::~gr_vmcircbuf "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf::pointer_to_first_copy "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf::pointer_to_second_copy "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_createfilemapping "concrete class to implement circular buffers with mmap and shm_open"

%feature("docstring") gr_vmcircbuf_createfilemapping::gr_vmcircbuf_createfilemapping "

Params: (size)"

%feature("docstring") gr_vmcircbuf_createfilemapping::~gr_vmcircbuf_createfilemapping "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_createfilemapping_factory "concrete factory for circular buffers built using mmap and shm_open"

%feature("docstring") gr_vmcircbuf_createfilemapping_factory::singleton "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_createfilemapping_factory::name "return name of this factory

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_createfilemapping_factory::granularity "return granularity of mapping, typically equal to page size

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_createfilemapping_factory::make "return a gr_vmcircbuf, or 0 if unable.

Call this to create a doubly mapped circular buffer.

Params: (size)"

%feature("docstring") gr_vmcircbuf_factory "abstract factory for creating circular buffers"

%feature("docstring") gr_vmcircbuf_factory::gr_vmcircbuf_factory "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_factory::~gr_vmcircbuf_factory "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_factory::name "return name of this factory

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_factory::granularity "return granularity of mapping, typically equal to page size

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_factory::make "return a gr_vmcircbuf, or 0 if unable.

Call this to create a doubly mapped circular buffer.

Params: (size)"

%feature("docstring") gr_vmcircbuf_mmap_shm_open "concrete class to implement circular buffers with mmap and shm_open"

%feature("docstring") gr_vmcircbuf_mmap_shm_open::gr_vmcircbuf_mmap_shm_open "

Params: (size)"

%feature("docstring") gr_vmcircbuf_mmap_shm_open::~gr_vmcircbuf_mmap_shm_open "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_mmap_shm_open_factory "concrete factory for circular buffers built using mmap and shm_open"

%feature("docstring") gr_vmcircbuf_mmap_shm_open_factory::singleton "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_mmap_shm_open_factory::name "return name of this factory

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_mmap_shm_open_factory::granularity "return granularity of mapping, typically equal to page size

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_mmap_shm_open_factory::make "return a gr_vmcircbuf, or 0 if unable.

Call this to create a doubly mapped circular buffer.

Params: (size)"

%feature("docstring") gr_vmcircbuf_mmap_tmpfile "concrete class to implement circular buffers with mmap and shm_open"

%feature("docstring") gr_vmcircbuf_mmap_tmpfile::gr_vmcircbuf_mmap_tmpfile "

Params: (size)"

%feature("docstring") gr_vmcircbuf_mmap_tmpfile::~gr_vmcircbuf_mmap_tmpfile "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_mmap_tmpfile_factory "concrete factory for circular buffers built using mmap and shm_open"

%feature("docstring") gr_vmcircbuf_mmap_tmpfile_factory::singleton "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_mmap_tmpfile_factory::name "return name of this factory

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_mmap_tmpfile_factory::granularity "return granularity of mapping, typically equal to page size

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_mmap_tmpfile_factory::make "return a gr_vmcircbuf, or 0 if unable.

Call this to create a doubly mapped circular buffer.

Params: (size)"



%feature("docstring") gr_vmcircbuf_sysconfig::get_default_factory "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_sysconfig::granularity "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_sysconfig::make "

Params: (size)"

%feature("docstring") gr_vmcircbuf_sysconfig::all_factories "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_sysconfig::set_default_factory "

Params: (f)"

%feature("docstring") gr_vmcircbuf_sysconfig::test_factory "Does this factory really work?

verbose = 0: silent verbose = 1: names of factories tested and results verbose = 2: all intermediate results

Params: (f, verbose)"

%feature("docstring") gr_vmcircbuf_sysconfig::test_all_factories "Test all factories, return true if at least one of them works verbose = 0: silent verbose = 1: names of factories tested and results verbose = 2: all intermediate results.

Params: (verbose)"

%feature("docstring") gr_vmcircbuf_sysv_shm "concrete class to implement circular buffers with mmap and shm_open"

%feature("docstring") gr_vmcircbuf_sysv_shm::gr_vmcircbuf_sysv_shm "

Params: (size)"

%feature("docstring") gr_vmcircbuf_sysv_shm::~gr_vmcircbuf_sysv_shm "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_sysv_shm_factory "concrete factory for circular buffers built using mmap and shm_open"

%feature("docstring") gr_vmcircbuf_sysv_shm_factory::singleton "

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_sysv_shm_factory::name "return name of this factory

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_sysv_shm_factory::granularity "return granularity of mapping, typically equal to page size

Params: (NONE)"

%feature("docstring") gr_vmcircbuf_sysv_shm_factory::make "return a gr_vmcircbuf, or 0 if unable.

Call this to create a doubly mapped circular buffer.

Params: (size)"

%feature("docstring") gri_agc2_cc "high performance Automatic Gain Control class

For Power the absolute value of the complex number is used."

%feature("docstring") gri_agc2_cc::gri_agc2_cc "

Params: (attack_rate, decay_rate, reference, gain, max_gain)"

%feature("docstring") gri_agc2_cc::decay_rate "

Params: (NONE)"

%feature("docstring") gri_agc2_cc::attack_rate "

Params: (NONE)"

%feature("docstring") gri_agc2_cc::reference "

Params: (NONE)"

%feature("docstring") gri_agc2_cc::gain "

Params: (NONE)"

%feature("docstring") gri_agc2_cc::max_gain "

Params: (NONE)"

%feature("docstring") gri_agc2_cc::set_decay_rate "

Params: (rate)"

%feature("docstring") gri_agc2_cc::set_attack_rate "

Params: (rate)"

%feature("docstring") gri_agc2_cc::set_reference "

Params: (reference)"

%feature("docstring") gri_agc2_cc::set_gain "

Params: (gain)"

%feature("docstring") gri_agc2_cc::set_max_gain "

Params: (max_gain)"

%feature("docstring") gri_agc2_cc::scale "

Params: (input)"

%feature("docstring") gri_agc2_cc::scaleN "

Params: (output, input, n)"

%feature("docstring") gri_agc2_ff "high performance Automatic Gain Control class with attack and decay rate

Power is approximated by absolute value"

%feature("docstring") gri_agc2_ff::gri_agc2_ff "

Params: (attack_rate, decay_rate, reference, gain, max_gain)"

%feature("docstring") gri_agc2_ff::attack_rate "

Params: (NONE)"

%feature("docstring") gri_agc2_ff::decay_rate "

Params: (NONE)"

%feature("docstring") gri_agc2_ff::reference "

Params: (NONE)"

%feature("docstring") gri_agc2_ff::gain "

Params: (NONE)"

%feature("docstring") gri_agc2_ff::max_gain "

Params: (NONE)"

%feature("docstring") gri_agc2_ff::set_attack_rate "

Params: (rate)"

%feature("docstring") gri_agc2_ff::set_decay_rate "

Params: (rate)"

%feature("docstring") gri_agc2_ff::set_reference "

Params: (reference)"

%feature("docstring") gri_agc2_ff::set_gain "

Params: (gain)"

%feature("docstring") gri_agc2_ff::set_max_gain "

Params: (max_gain)"

%feature("docstring") gri_agc2_ff::scale "

Params: (input)"

%feature("docstring") gri_agc2_ff::scaleN "

Params: (output, input, n)"

%feature("docstring") gri_agc_cc "high performance Automatic Gain Control class

For Power the absolute value of the complex number is used."

%feature("docstring") gri_agc_cc::gri_agc_cc "

Params: (rate, reference, gain, max_gain)"

%feature("docstring") gri_agc_cc::rate "

Params: (NONE)"

%feature("docstring") gri_agc_cc::reference "

Params: (NONE)"

%feature("docstring") gri_agc_cc::gain "

Params: (NONE)"

%feature("docstring") gri_agc_cc::max_gain "

Params: (NONE)"

%feature("docstring") gri_agc_cc::set_rate "

Params: (rate)"

%feature("docstring") gri_agc_cc::set_reference "

Params: (reference)"

%feature("docstring") gri_agc_cc::set_gain "

Params: (gain)"

%feature("docstring") gri_agc_cc::set_max_gain "

Params: (max_gain)"

%feature("docstring") gri_agc_cc::scale "

Params: (input)"

%feature("docstring") gri_agc_cc::scaleN "

Params: (output, input, n)"

%feature("docstring") gri_agc_ff "high performance Automatic Gain Control class

Power is approximated by absolute value"

%feature("docstring") gri_agc_ff::gri_agc_ff "

Params: (rate, reference, gain, max_gain)"

%feature("docstring") gri_agc_ff::rate "

Params: (NONE)"

%feature("docstring") gri_agc_ff::reference "

Params: (NONE)"

%feature("docstring") gri_agc_ff::gain "

Params: (NONE)"

%feature("docstring") gri_agc_ff::max_gain "

Params: (NONE)"

%feature("docstring") gri_agc_ff::set_rate "

Params: (rate)"

%feature("docstring") gri_agc_ff::set_reference "

Params: (reference)"

%feature("docstring") gri_agc_ff::set_gain "

Params: (gain)"

%feature("docstring") gri_agc_ff::set_max_gain "

Params: (max_gain)"

%feature("docstring") gri_agc_ff::scale "

Params: (input)"

%feature("docstring") gri_agc_ff::scaleN "

Params: (output, input, n)"

%feature("docstring") gri_fft_complex "FFT: complex in, complex out."

%feature("docstring") gri_fft_complex::gri_fft_complex "

Params: (fft_size, forward)"

%feature("docstring") gri_fft_complex::~gri_fft_complex "

Params: (NONE)"

%feature("docstring") gri_fft_complex::get_inbuf "

Params: (NONE)"

%feature("docstring") gri_fft_complex::get_outbuf "

Params: (NONE)"

%feature("docstring") gri_fft_complex::inbuf_length "

Params: (NONE)"

%feature("docstring") gri_fft_complex::outbuf_length "

Params: (NONE)"

%feature("docstring") gri_fft_complex::execute "compute FFT. The input comes from inbuf, the output is placed in outbuf.

Params: (NONE)"

%feature("docstring") gri_fft_filter_ccc_generic "Fast FFT filter with gr_complex input, gr_complex output and gr_complex taps."

%feature("docstring") gri_fft_filter_ccc_generic::compute_sizes "

Params: (ntaps)"

%feature("docstring") gri_fft_filter_ccc_generic::tailsize "

Params: (NONE)"

%feature("docstring") gri_fft_filter_ccc_generic::gri_fft_filter_ccc_generic "Construct an FFT filter for complex vectors with the given taps and decimation rate.

This is the basic implementation for performing FFT filter for fast convolution in other blocks for complex vectors (such as gr_fft_filter_ccc).

Params: (decimation, taps)"

%feature("docstring") gri_fft_filter_ccc_generic::~gri_fft_filter_ccc_generic "

Params: (NONE)"

%feature("docstring") gri_fft_filter_ccc_generic::set_taps "Set new taps for the filter.

Sets new taps and resets the class properties to handle different sizes

Params: (taps)"

%feature("docstring") gri_fft_filter_ccc_generic::filter "Perform the filter operation.

Params: (nitems, input, output)"

%feature("docstring") gri_fft_filter_ccc_sse "Fast FFT filter with gr_complex input, gr_complex output and gr_complex taps."

%feature("docstring") gri_fft_filter_ccc_sse::compute_sizes "

Params: (ntaps)"

%feature("docstring") gri_fft_filter_ccc_sse::tailsize "

Params: (NONE)"

%feature("docstring") gri_fft_filter_ccc_sse::gri_fft_filter_ccc_sse "Construct an FFT filter for complex vectors with the given taps and decimation rate.

This is the basic implementation for performing FFT filter for fast convolution in other blocks for complex vectors (such as gr_fft_filter_ccc).

Params: (decimation, taps)"

%feature("docstring") gri_fft_filter_ccc_sse::~gri_fft_filter_ccc_sse "

Params: (NONE)"

%feature("docstring") gri_fft_filter_ccc_sse::set_taps "Set new taps for the filter.

Sets new taps and resets the class properties to handle different sizes

Params: (taps)"

%feature("docstring") gri_fft_filter_ccc_sse::filter "Perform the filter operation.

Params: (nitems, input, output)"



%feature("docstring") gri_fft_filter_fff_generic::compute_sizes "

Params: (ntaps)"

%feature("docstring") gri_fft_filter_fff_generic::tailsize "

Params: (NONE)"

%feature("docstring") gri_fft_filter_fff_generic::gri_fft_filter_fff_generic "Construct a FFT filter for float vectors with the given taps and decimation rate.

This is the basic implementation for performing FFT filter for fast convolution in other blocks for floating point vectors (such as gr_fft_filter_fff).

Params: (decimation, taps)"

%feature("docstring") gri_fft_filter_fff_generic::~gri_fft_filter_fff_generic "

Params: (NONE)"

%feature("docstring") gri_fft_filter_fff_generic::set_taps "Set new taps for the filter.

Sets new taps and resets the class properties to handle different sizes

Params: (taps)"

%feature("docstring") gri_fft_filter_fff_generic::filter "Perform the filter operation.

Params: (nitems, input, output)"



%feature("docstring") gri_fft_filter_fff_sse::compute_sizes "

Params: (ntaps)"

%feature("docstring") gri_fft_filter_fff_sse::tailsize "

Params: (NONE)"

%feature("docstring") gri_fft_filter_fff_sse::gri_fft_filter_fff_sse "Construct a FFT filter for float vectors with the given taps and decimation rate.

This is the basic implementation for performing FFT filter for fast convolution in other blocks for floating point vectors (such as gr_fft_filter_fff).

Params: (decimation, taps)"

%feature("docstring") gri_fft_filter_fff_sse::~gri_fft_filter_fff_sse "

Params: (NONE)"

%feature("docstring") gri_fft_filter_fff_sse::set_taps "Set new taps for the filter.

Sets new taps and resets the class properties to handle different sizes

Params: (taps)"

%feature("docstring") gri_fft_filter_fff_sse::filter "Perform the filter operation.

Params: (nitems, input, output)"

%feature("docstring") gri_fft_planner "Export reference to planner mutex for those apps that want to use FFTW w/o using the gri_fftw* classes."

%feature("docstring") gri_fft_planner::mutex "Return reference to planner mutex

Params: (NONE)"

%feature("docstring") gri_fft_real_fwd "FFT: real in, complex out."

%feature("docstring") gri_fft_real_fwd::gri_fft_real_fwd "

Params: (fft_size)"

%feature("docstring") gri_fft_real_fwd::~gri_fft_real_fwd "

Params: (NONE)"

%feature("docstring") gri_fft_real_fwd::get_inbuf "

Params: (NONE)"

%feature("docstring") gri_fft_real_fwd::get_outbuf "

Params: (NONE)"

%feature("docstring") gri_fft_real_fwd::inbuf_length "

Params: (NONE)"

%feature("docstring") gri_fft_real_fwd::outbuf_length "

Params: (NONE)"

%feature("docstring") gri_fft_real_fwd::execute "compute FFT. The input comes from inbuf, the output is placed in outbuf.

Params: (NONE)"

%feature("docstring") gri_fft_real_rev "FFT: complex in, float out."

%feature("docstring") gri_fft_real_rev::gri_fft_real_rev "

Params: (fft_size)"

%feature("docstring") gri_fft_real_rev::~gri_fft_real_rev "

Params: (NONE)"

%feature("docstring") gri_fft_real_rev::get_inbuf "

Params: (NONE)"

%feature("docstring") gri_fft_real_rev::get_outbuf "

Params: (NONE)"

%feature("docstring") gri_fft_real_rev::inbuf_length "

Params: (NONE)"

%feature("docstring") gri_fft_real_rev::outbuf_length "

Params: (NONE)"

%feature("docstring") gri_fft_real_rev::execute "compute FFT. The input comes from inbuf, the output is placed in outbuf.

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_ccc "FIR with internal buffer for gr_complex input, gr_complex output and gr_complex taps."

%feature("docstring") gri_fir_filter_with_buffer_ccc::gri_fir_filter_with_buffer_ccc "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_ccc::~gri_fir_filter_with_buffer_ccc "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_ccc::filter "compute a single output value.

is a single input value of the filter type

Params: (input)"

%feature("docstring") gri_fir_filter_with_buffer_ccc::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gri_fir_filter_with_buffer_ccc::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gri_fir_filter_with_buffer_ccc::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_ccc::ntaps "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_ccc::get_taps "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_ccf "FIR with internal buffer for gr_complex input, gr_complex output and float taps."

%feature("docstring") gri_fir_filter_with_buffer_ccf::gri_fir_filter_with_buffer_ccf "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_ccf::~gri_fir_filter_with_buffer_ccf "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_ccf::filter "compute a single output value.

is a single input value of the filter type

Params: (input)"

%feature("docstring") gri_fir_filter_with_buffer_ccf::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gri_fir_filter_with_buffer_ccf::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gri_fir_filter_with_buffer_ccf::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_ccf::ntaps "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_ccf::get_taps "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_fcc "FIR with internal buffer for float input, gr_complex output and gr_complex taps."

%feature("docstring") gri_fir_filter_with_buffer_fcc::gri_fir_filter_with_buffer_fcc "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_fcc::~gri_fir_filter_with_buffer_fcc "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_fcc::filter "compute a single output value.

is a single input value of the filter type

Params: (input)"

%feature("docstring") gri_fir_filter_with_buffer_fcc::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gri_fir_filter_with_buffer_fcc::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gri_fir_filter_with_buffer_fcc::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_fcc::ntaps "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_fcc::get_taps "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_fff "FIR with internal buffer for float input, float output and float taps."

%feature("docstring") gri_fir_filter_with_buffer_fff::gri_fir_filter_with_buffer_fff "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_fff::~gri_fir_filter_with_buffer_fff "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_fff::filter "compute a single output value.

is a single input value of the filter type

Params: (input)"

%feature("docstring") gri_fir_filter_with_buffer_fff::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gri_fir_filter_with_buffer_fff::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gri_fir_filter_with_buffer_fff::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_fff::ntaps "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_fff::get_taps "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_fsf "FIR with internal buffer for float input, short output and float taps."

%feature("docstring") gri_fir_filter_with_buffer_fsf::gri_fir_filter_with_buffer_fsf "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_fsf::~gri_fir_filter_with_buffer_fsf "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_fsf::filter "compute a single output value.

is a single input value of the filter type

Params: (input)"

%feature("docstring") gri_fir_filter_with_buffer_fsf::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gri_fir_filter_with_buffer_fsf::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gri_fir_filter_with_buffer_fsf::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_fsf::ntaps "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_fsf::get_taps "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_scc "FIR with internal buffer for short input, gr_complex output and gr_complex taps."

%feature("docstring") gri_fir_filter_with_buffer_scc::gri_fir_filter_with_buffer_scc "construct new FIR with given taps.

Note that taps must be in forward order, e.g., coefficient 0 is stored in new_taps[0], coefficient 1 is stored in new_taps[1], etc.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_scc::~gri_fir_filter_with_buffer_scc "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_scc::filter "compute a single output value.

is a single input value of the filter type

Params: (input)"

%feature("docstring") gri_fir_filter_with_buffer_scc::filterN "compute an array of N output values.

must have (n - 1 + ntaps()) valid entries. input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n)"

%feature("docstring") gri_fir_filter_with_buffer_scc::filterNdec "compute an array of N output values, decimating the input

must have (decimate * (n - 1) + ntaps()) valid entries. input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to compute the output values.

Params: (output, input, n, decimate)"

%feature("docstring") gri_fir_filter_with_buffer_scc::set_taps "install  as the current taps.

Params: (taps)"

%feature("docstring") gri_fir_filter_with_buffer_scc::ntaps "

Params: (NONE)"

%feature("docstring") gri_fir_filter_with_buffer_scc::get_taps "

Params: (NONE)"

%feature("docstring") gri_glfsr "Galois Linear Feedback Shift Register using specified polynomial mask

Generates a maximal length pseudo-random sequence of length 2^degree-1."

%feature("docstring") gri_glfsr::gri_glfsr "

Params: (mask, seed)"

%feature("docstring") gri_glfsr::next_bit "

Params: (NONE)"

%feature("docstring") gri_glfsr::mask "

Params: (NONE)"

%feature("docstring") gri_glfsr::glfsr_mask "

Params: (degree)"

%feature("docstring") gri_goertzel "Implements Goertzel single-bin DFT calculation."

%feature("docstring") gri_goertzel::gri_goertzel "

Params: (NONE)"

%feature("docstring") gri_goertzel::gri_setparms "

Params: (rate, len, freq)"

%feature("docstring") gri_goertzel::batch "

Params: (in)"

%feature("docstring") gri_goertzel::input "

Params: (in)"

%feature("docstring") gri_goertzel::output "

Params: (NONE)"

%feature("docstring") gri_goertzel::ready "

Params: (NONE)"

%feature("docstring") gri_iir "base class template for Infinite Impulse Response filter (IIR)"

%feature("docstring") gri_iir::gri_iir "Construct an IIR with the given taps.

This filter uses the Direct Form I implementation, where  contains the feed-forward taps, and  the feedback ones.

 and  must have equal numbers of taps

The input and output satisfy a difference equation of the form



with the corresponding rational system function



Note that some texts define the system function with a + in the denominator. If you're using that convention, you'll need to negate the feedback taps.

Params: (fftaps, fbtaps)"

%feature("docstring") gri_iir::~gri_iir "

Params: (NONE)"

%feature("docstring") gri_iir::filter "compute a single output value.

Params: (input)"

%feature("docstring") gri_iir::filter_n "compute an array of N output values.  must have N valid entries.

Params: (output, input, n)"

%feature("docstring") gri_iir::ntaps_ff "

Params: (NONE)"

%feature("docstring") gri_iir::ntaps_fb "

Params: (NONE)"

%feature("docstring") gri_iir::set_taps "install new taps.

Params: (fftaps, fbtaps)"

%feature("docstring") gri_lfsr "Fibonacci Linear Feedback Shift Register using specified polynomial mask

Generates a maximal length pseudo-random sequence of length 2^degree-1.

Constructor: gri_lfsr(int mask, int seed, int reg_len);

mask - polynomial coefficients representing the locations of feedback taps from a shift register which are xor'ed together to form the new high order bit.

Some common masks might be: x^4 + x^3 + x^0 = 0x19 x^5 + x^3 + x^0 = 0x29 x^6 + x^5 + x^0 = 0x61

seed - the initialization vector placed into the register durring initialization. Low order bit corresponds to x^0 coefficient -- the first to be shifted as output.

reg_len - specifies the length of the feedback shift register to be used. Durring each iteration, the register is rightshifted one and the new bit is placed in bit reg_len. reg_len should generally be at least order(mask) + 1

see  for more explanation.

next_bit() - Standard LFSR operation

Perform one cycle of the LFSR. The output bit is taken from the shift register LSB. The shift register MSB is assigned from the modulo 2 sum of the masked shift register.

next_bit_scramble(unsigned char input) - Scramble an input stream

Perform one cycle of the LFSR. The output bit is taken from the shift register LSB. The shift register MSB is assigned from the modulo 2 sum of the masked shift register and the input LSB.

next_bit_descramble(unsigned char input) - Descramble an input stream

Perform one cycle of the LFSR. The output bit is taken from the modulo 2 sum of the masked shift register and the input LSB. The shift register MSB is assigned from the LSB of the input.

See  for operation of these last two functions (see multiplicative scrambler.)"

%feature("docstring") gri_lfsr::popCount "

Params: (x)"

%feature("docstring") gri_lfsr::gri_lfsr "

Params: (mask, seed, reg_len)"

%feature("docstring") gri_lfsr::next_bit "

Params: (NONE)"

%feature("docstring") gri_lfsr::next_bit_scramble "

Params: (input)"

%feature("docstring") gri_lfsr::next_bit_descramble "

Params: (input)"

%feature("docstring") gri_lfsr::reset "Reset shift register to initial seed value

Params: (NONE)"

%feature("docstring") gri_lfsr::pre_shift "Rotate the register through x number of bits where we are just throwing away the results to get queued up correctly

Params: (num)"

%feature("docstring") gri_lfsr::mask "

Params: (NONE)"

%feature("docstring") gri_lfsr_15_1_0 "Linear Feedback Shift Register using primitive polynomial x^15 + x + 1

Generates a maximal length pseudo-random sequence of length 2^15 - 1 bits."

%feature("docstring") gri_lfsr_15_1_0::gri_lfsr_15_1_0 "

Params: (NONE)"

%feature("docstring") gri_lfsr_15_1_0::reset "

Params: (NONE)"

%feature("docstring") gri_lfsr_15_1_0::next_bit "

Params: (NONE)"

%feature("docstring") gri_lfsr_15_1_0::next_byte "

Params: (NONE)"

%feature("docstring") gri_lfsr_32k "generate pseudo-random sequence of length 32768 bits.

This is based on gri_lfsr_15_1_0 with an extra 0 added at the end of the sequence."

%feature("docstring") gri_lfsr_32k::gri_lfsr_32k "

Params: (NONE)"

%feature("docstring") gri_lfsr_32k::reset "

Params: (NONE)"

%feature("docstring") gri_lfsr_32k::next_bit "

Params: (NONE)"

%feature("docstring") gri_lfsr_32k::next_byte "

Params: (NONE)"

%feature("docstring") gri_lfsr_32k::next_short "

Params: (NONE)"

%feature("docstring") gri_mmse_fir_interpolator "Compute intermediate samples between signal samples x(k*Ts)

This implements a Mininum Mean Squared Error interpolator with 8 taps. It is suitable for signals where the bandwidth of interest B = 1/(4*Ts) Where Ts is the time between samples.

Although mu, the fractional delay, is specified as a float, it is actually quantized. 0.0 <= mu <= 1.0. That is, mu is quantized in the interpolate method to 32nd's of a sample."

%feature("docstring") gri_mmse_fir_interpolator::gri_mmse_fir_interpolator "

Params: (NONE)"

%feature("docstring") gri_mmse_fir_interpolator::~gri_mmse_fir_interpolator "

Params: (NONE)"

%feature("docstring") gri_mmse_fir_interpolator::ntaps "

Params: (NONE)"

%feature("docstring") gri_mmse_fir_interpolator::nsteps "

Params: (NONE)"

%feature("docstring") gri_mmse_fir_interpolator::interpolate "compute a single interpolated output value.  must have ntaps() valid entries. input[0] .. input[ntaps() - 1] are referenced to compute the output value.

must be in the range [0, 1] and specifies the fractional delay.

Params: (input, mu)"

%feature("docstring") gri_mmse_fir_interpolator_cc "Compute intermediate samples between signal samples x(k*Ts)

This implements a Mininum Mean Squared Error interpolator with 8 taps. It is suitable for signals where the bandwidth of interest B = 1/(4*Ts) Where Ts is the time between samples.

Although mu, the fractional delay, is specified as a float, it is actually quantized. 0.0 <= mu <= 1.0. That is, mu is quantized in the interpolate method to 32nd's of a sample."

%feature("docstring") gri_mmse_fir_interpolator_cc::gri_mmse_fir_interpolator_cc "

Params: (NONE)"

%feature("docstring") gri_mmse_fir_interpolator_cc::~gri_mmse_fir_interpolator_cc "

Params: (NONE)"

%feature("docstring") gri_mmse_fir_interpolator_cc::ntaps "

Params: (NONE)"

%feature("docstring") gri_mmse_fir_interpolator_cc::nsteps "

Params: (NONE)"

%feature("docstring") gri_mmse_fir_interpolator_cc::interpolate "compute a single interpolated output value.

must have ntaps() valid entries and be 8-byte aligned. input[0] .. input[ntaps() - 1] are referenced to compute the output value. 
 must be in the range [0, 1] and specifies the fractional delay.

Params: (input, mu)"

%feature("docstring") gsm_fr_decode_ps "GSM 06.10 Full Rate Vocoder Decoder."

%feature("docstring") gsm_fr_decode_ps::gsm_fr_decode_ps "

Params: (NONE)"

%feature("docstring") gsm_fr_decode_ps::~gsm_fr_decode_ps "

Params: (NONE)"

%feature("docstring") gsm_fr_decode_ps::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gsm_fr_encode_sp "GSM 06.10 Full Rate Vocoder Encoder

shorts in; 33 byte packets out."

%feature("docstring") gsm_fr_encode_sp::gsm_fr_encode_sp "

Params: (NONE)"

%feature("docstring") gsm_fr_encode_sp::~gsm_fr_encode_sp "

Params: (NONE)"

%feature("docstring") gsm_fr_encode_sp::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") i2c "abstract class for controlling i2c bus"

%feature("docstring") i2c::i2c "

Params: (NONE)"

%feature("docstring") i2c::~i2c "

Params: (NONE)"

%feature("docstring") i2c::write "

Params: (addr, buf, nbytes)"

%feature("docstring") i2c::read "

Params: (addr, buf, max_bytes)"

%feature("docstring") i2c_bbio "abstract class that implements bit banging i/o for i2c bus."

%feature("docstring") i2c_bbio::i2c_bbio "

Params: (NONE)"

%feature("docstring") i2c_bbio::~i2c_bbio "

Params: (NONE)"

%feature("docstring") i2c_bbio::set_scl "

Params: (state)"

%feature("docstring") i2c_bbio::set_sda "

Params: (state)"

%feature("docstring") i2c_bbio::get_sda "

Params: (NONE)"

%feature("docstring") i2c_bbio::lock "

Params: (NONE)"

%feature("docstring") i2c_bbio::unlock "

Params: (NONE)"

%feature("docstring") i2c_bbio_pp "concrete class that bit bangs eval board i2c bus using parallel port

This class talks to the i2c bus on the microtune eval board using the parallel port. This works for both the 4937 and 4702 boards."

%feature("docstring") i2c_bbio_pp::i2c_bbio_pp "

Params: (pp)"

%feature("docstring") i2c_bbio_pp::set_scl "

Params: (state)"

%feature("docstring") i2c_bbio_pp::set_sda "

Params: (state)"

%feature("docstring") i2c_bbio_pp::get_sda "

Params: (NONE)"

%feature("docstring") i2c_bbio_pp::lock "

Params: (NONE)"

%feature("docstring") i2c_bbio_pp::unlock "

Params: (NONE)"

%feature("docstring") i2c_bitbang "class for controlling i2c bus"

%feature("docstring") i2c_bitbang::i2c_bitbang "

Params: (io)"

%feature("docstring") i2c_bitbang::start "

Params: (NONE)"

%feature("docstring") i2c_bitbang::stop "

Params: (NONE)"

%feature("docstring") i2c_bitbang::write_bit "

Params: (bit)"

%feature("docstring") i2c_bitbang::write_byte "

Params: (byte)"

%feature("docstring") i2c_bitbang::set_sda "

Params: (bit)"

%feature("docstring") i2c_bitbang::set_scl "

Params: (bit)"

%feature("docstring") i2c_bitbang::get_sda "

Params: (NONE)"

%feature("docstring") i2c_bitbang::~i2c_bitbang "

Params: (NONE)"

%feature("docstring") i2c_bitbang::write "

Params: (addr, buf, nbytes)"

%feature("docstring") i2c_bitbang::read "

Params: (addr, buf, max_bytes)"

%feature("docstring") std::ifstream "STL class."



%feature("docstring") usrp2::usrp2::impl::inc_enqueued "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::dec_enqueued "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::init_et_hdrs "

Params: (p, dst)"

%feature("docstring") usrp2::usrp2::impl::init_etf_hdrs "

Params: (p, dst, word0_flags, chan, timestamp)"

%feature("docstring") usrp2::usrp2::impl::start_bg "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::stop_bg "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::init_config_rx_v2_cmd "

Params: (cmd)"

%feature("docstring") usrp2::usrp2::impl::init_config_tx_v2_cmd "

Params: (cmd)"

%feature("docstring") usrp2::usrp2::impl::transmit_cmd_and_wait "

Params: (cmd, len, p, secs)"

%feature("docstring") usrp2::usrp2::impl::transmit_cmd "

Params: (cmd, len)"

%feature("docstring") usrp2::usrp2::impl::operator() "

Params: (base, len)"

%feature("docstring") usrp2::usrp2::impl::handle_control_packet "

Params: (base, len)"

%feature("docstring") usrp2::usrp2::impl::handle_data_packet "

Params: (base, len)"

%feature("docstring") usrp2::usrp2::impl::dboard_info "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::reset_db "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::impl "

Params: (ifc, p, rx_bufsize)"

%feature("docstring") usrp2::usrp2::impl::~impl "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::mac_addr "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::interface_name "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::set_rx_antenna "

Params: (ant)"

%feature("docstring") usrp2::usrp2::impl::set_rx_gain "

Params: (gain)"

%feature("docstring") usrp2::usrp2::impl::rx_gain_min "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::rx_gain_max "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::rx_gain_db_per_step "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::set_rx_lo_offset "

Params: (frequency)"

%feature("docstring") usrp2::usrp2::impl::set_rx_center_freq "

Params: (frequency, result)"

%feature("docstring") usrp2::usrp2::impl::rx_freq_min "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::rx_freq_max "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::set_rx_decim "

Params: (decimation_factor)"

%feature("docstring") usrp2::usrp2::impl::rx_decim "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::set_rx_scale_iq "

Params: (scale_i, scale_q)"

%feature("docstring") usrp2::usrp2::impl::set_gpio_ddr "

Params: (bank, value, mask)"

%feature("docstring") usrp2::usrp2::impl::set_gpio_sels "

Params: (bank, src)"

%feature("docstring") usrp2::usrp2::impl::enable_gpio_streaming "

Params: (bank, enable)"

%feature("docstring") usrp2::usrp2::impl::write_gpio "

Params: (bank, value, mask)"

%feature("docstring") usrp2::usrp2::impl::read_gpio "

Params: (bank, value)"

%feature("docstring") usrp2::usrp2::impl::start_rx_streaming "

Params: (channel, items_per_frame)"

%feature("docstring") usrp2::usrp2::impl::start_rx_streaming_at "

Params: (channel, items_per_frame, time)"

%feature("docstring") usrp2::usrp2::impl::sync_and_start_rx_streaming_at "

Params: (channel, items_per_frame, time)"

%feature("docstring") usrp2::usrp2::impl::rx_samples "

Params: (channel, handler)"

%feature("docstring") usrp2::usrp2::impl::flush_rx_samples "

Params: (channel)"

%feature("docstring") usrp2::usrp2::impl::stop_rx_streaming "

Params: (channel)"

%feature("docstring") usrp2::usrp2::impl::rx_overruns "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::rx_missing "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::set_tx_antenna "

Params: (ant)"

%feature("docstring") usrp2::usrp2::impl::set_tx_gain "

Params: (gain)"

%feature("docstring") usrp2::usrp2::impl::tx_gain_min "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::tx_gain_max "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::tx_gain_db_per_step "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::set_tx_lo_offset "

Params: (frequency)"

%feature("docstring") usrp2::usrp2::impl::set_tx_center_freq "

Params: (frequency, result)"

%feature("docstring") usrp2::usrp2::impl::tx_freq_min "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::tx_freq_max "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::set_tx_interp "

Params: (interpolation_factor)"

%feature("docstring") usrp2::usrp2::impl::tx_interp "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::default_tx_scale_iq "

Params: (interpolation_factor, scale_i, scale_q)"

%feature("docstring") usrp2::usrp2::impl::set_tx_scale_iq "

Params: (scale_i, scale_q)"

%feature("docstring") usrp2::usrp2::impl::tx_32fc "

Params: (channel, samples, nsamples, metadata)"

%feature("docstring") usrp2::usrp2::impl::tx_16sc "

Params: (channel, samples, nsamples, metadata)"

%feature("docstring") usrp2::usrp2::impl::tx_raw "

Params: (channel, items, nitems, metadata)"

%feature("docstring") usrp2::usrp2::impl::config_mimo "

Params: (flags)"

%feature("docstring") usrp2::usrp2::impl::fpga_master_clock_freq "

Params: (freq)"

%feature("docstring") usrp2::usrp2::impl::adc_rate "

Params: (rate)"

%feature("docstring") usrp2::usrp2::impl::dac_rate "

Params: (rate)"

%feature("docstring") usrp2::usrp2::impl::tx_daughterboard_id "

Params: (dbid)"

%feature("docstring") usrp2::usrp2::impl::rx_daughterboard_id "

Params: (dbid)"

%feature("docstring") usrp2::usrp2::impl::burn_mac_addr "

Params: (new_addr)"

%feature("docstring") usrp2::usrp2::impl::sync_to_pps "

Params: (NONE)"

%feature("docstring") usrp2::usrp2::impl::sync_every_pps "

Params: (enable)"

%feature("docstring") usrp2::usrp2::impl::peek32 "

Params: (addr, words)"

%feature("docstring") usrp2::usrp2::impl::poke32 "

Params: (addr, data)"

%feature("docstring") usrp2::usrp2::impl::bg_loop "

Params: (NONE)"

%feature("docstring") interleaver "INTERLEAVER class."

%feature("docstring") interleaver::interleaver "

Params: (NONE)"

%feature("docstring") interleaver::K "

Params: (NONE)"

%feature("docstring") interleaver::INTER "

Params: (NONE)"

%feature("docstring") interleaver::DEINTER "

Params: (NONE)"

%feature("docstring") interleaver::write_interleaver_txt "

Params: (filename)"

%feature("docstring") interleaver_fifo "template class for interleaver fifo"

%feature("docstring") interleaver_fifo::interleaver_fifo "

Params: (size)"

%feature("docstring") interleaver_fifo::~interleaver_fifo "

Params: (NONE)"

%feature("docstring") interleaver_fifo::reset "reset interleaver (flushes contents and resets commutator)

Params: (NONE)"

%feature("docstring") interleaver_fifo::stuff "stuff a symbol into the fifo and return the oldest

Params: (input)"

%feature("docstring") std::invalid_argument "STL class."

%feature("docstring") std::ios "STL class."

%feature("docstring") std::ios_base "STL class."

%feature("docstring") std::istream "STL class."

%feature("docstring") std::istringstream "STL class."

%feature("docstring") std::basic_string::iterator "STL iterator class."

%feature("docstring") std::multimap::iterator "STL iterator class."

%feature("docstring") std::wstring::iterator "STL iterator class."

%feature("docstring") std::set::iterator "STL iterator class."

%feature("docstring") std::map::iterator "STL iterator class."

%feature("docstring") std::deque::iterator "STL iterator class."

%feature("docstring") std::list::iterator "STL iterator class."

%feature("docstring") std::multiset::iterator "STL iterator class."

%feature("docstring") std::string::iterator "STL iterator class."

%feature("docstring") std::vector::iterator "STL iterator class."

%feature("docstring") std::length_error "STL class."

%feature("docstring") std::list "STL class."

%feature("docstring") std::logic_error "STL class."

%feature("docstring") std::map "STL class."

%feature("docstring") microtune_4702 "class for controlling microtune 4702 tuner module"

%feature("docstring") microtune_4702::microtune_4702 "

Params: (i2c, i2c_addr)"

%feature("docstring") microtune_4702::~microtune_4702 "

Params: (NONE)"

%feature("docstring") microtune_4702::set_RF_freq "select RF frequency to be tuned to output frequency.  is the requested frequency in Hz,  is set to the actual frequency tuned. It takes about 100 ms for the PLL to settle.

Params: (freq, actual_freq)"

%feature("docstring") microtune_4702::pll_locked_p "

Params: (NONE)"

%feature("docstring") microtune_4702::get_output_freq "

Params: (NONE)"

%feature("docstring") microtune_4702_eval_board "control microtune 4702 eval board"

%feature("docstring") microtune_4702_eval_board::microtune_4702_eval_board "

Params: (which_pp)"

%feature("docstring") microtune_4702_eval_board::~microtune_4702_eval_board "

Params: (NONE)"

%feature("docstring") microtune_4702_eval_board::set_AGC "set RF and IF AGC levels together (scale [0, 1000])

This provides a simple linear interface for adjusting both the RF and IF gain in consort. This is the easy to use interface. 0 corresponds to minimum gain. 1000 corresponds to maximum gain.

Params: (value_0_1000)"

%feature("docstring") microtune_4937 "class for controlling microtune 4937 tuner module"

%feature("docstring") microtune_4937::microtune_4937 "

Params: (i2c, i2c_addr)"

%feature("docstring") microtune_4937::~microtune_4937 "

Params: (NONE)"

%feature("docstring") microtune_4937::set_RF_freq "select RF frequency to be tuned to output frequency.  is the requested frequency in Hz,  is set to the actual frequency tuned. It takes about 100 ms for the PLL to settle.

Params: (freq, actual_freq)"

%feature("docstring") microtune_4937::pll_locked_p "

Params: (NONE)"

%feature("docstring") microtune_4937::get_output_freq "

Params: (NONE)"

%feature("docstring") microtune_4937_eval_board "control microtune 4937 eval board"

%feature("docstring") microtune_4937_eval_board::microtune_4937_eval_board "

Params: (which_pp)"

%feature("docstring") microtune_4937_eval_board::~microtune_4937_eval_board "

Params: (NONE)"

%feature("docstring") microtune_4937_eval_board::set_AGC "set RF and IF AGC levels together (scale [0, 1000])

This provides a simple linear interface for adjusting both the RF and IF gain in consort. This is the easy to use interface. 0 corresponds to minimum gain. 1000 corresponds to maximum gain.

Params: (value_0_1000)"

%feature("docstring") microtune_xxxx "abstract class for controlling microtune {4937,4702} tuner modules"

%feature("docstring") microtune_xxxx::microtune_xxxx "

Params: (NONE)"

%feature("docstring") microtune_xxxx::~microtune_xxxx "

Params: (NONE)"

%feature("docstring") microtune_xxxx::set_RF_freq "select RF frequency to be tuned to output frequency.  is the requested frequency in Hz,  is set to the actual frequency tuned. It takes about 100 ms for the PLL to settle.

Params: (freq, actual_freq)"

%feature("docstring") microtune_xxxx::pll_locked_p "

Params: (NONE)"

%feature("docstring") microtune_xxxx::get_output_freq "

Params: (NONE)"

%feature("docstring") microtune_xxxx_eval_board "abstract class for controlling microtune xxxx eval board"

%feature("docstring") microtune_xxxx_eval_board::microtune_xxxx_eval_board "

Params: (which_pp)"

%feature("docstring") microtune_xxxx_eval_board::~microtune_xxxx_eval_board "

Params: (NONE)"

%feature("docstring") microtune_xxxx_eval_board::board_present_p "is the eval board present?

Params: (NONE)"

%feature("docstring") microtune_xxxx_eval_board::set_RF_AGC_voltage "set RF and IF AGC control voltages ([0, 5] volts)

Params: (volts)"

%feature("docstring") microtune_xxxx_eval_board::set_IF_AGC_voltage "

Params: (volts)"

%feature("docstring") microtune_xxxx_eval_board::set_AGC "set RF and IF AGC levels together (scale [0, 1000])

This provides a simple linear interface for adjusting both the RF and IF gain in consort. This is the easy to use interface. 0 corresponds to minimum gain. 1000 corresponds to maximum gain.

Params: (value_0_1000)"

%feature("docstring") microtune_xxxx_eval_board::set_RF_freq "select RF frequency to be tuned to output frequency.  is the requested frequency in Hz,  is set to the actual frequency tuned. It takes about 100 ms for the PLL to settle.

Params: (freq, actual_freq)"

%feature("docstring") microtune_xxxx_eval_board::pll_locked_p "

Params: (NONE)"

%feature("docstring") microtune_xxxx_eval_board::get_output_freq "

Params: (NONE)"

%feature("docstring") microtune_xxxx_eval_board::write_dac "

Params: (which, value)"

%feature("docstring") microtune_xxxx_eval_board::write_both_dacs "

Params: (val0, val1)"



%feature("docstring") MSDD6000::MSDD6000 "

Params: (ip_addr)"

%feature("docstring") MSDD6000::~MSDD6000 "

Params: (NONE)"

%feature("docstring") MSDD6000::set_decim "

Params: (decim_pow2)"

%feature("docstring") MSDD6000::set_fc "

Params: (center_mhz, offset_hz)"

%feature("docstring") MSDD6000::set_ddc_gain "

Params: (gain)"

%feature("docstring") MSDD6000::set_rf_attn "

Params: (attn)"

%feature("docstring") MSDD6000::set_output "

Params: (mode, arg)"

%feature("docstring") MSDD6000::start "

Params: (NONE)"

%feature("docstring") MSDD6000::stop "

Params: (NONE)"

%feature("docstring") MSDD6000::send_request "

Params: (, , , , )"

%feature("docstring") MSDD6000::read "

Params: (, )"



%feature("docstring") MSDD6000_RS::MSDD6000_RS "

Params: (ip_addr)"

%feature("docstring") MSDD6000_RS::~MSDD6000_RS "

Params: (NONE)"

%feature("docstring") MSDD6000_RS::set_rf_attn "

Params: (attn)"

%feature("docstring") MSDD6000_RS::set_ddc_gain "

Params: (gain)"

%feature("docstring") MSDD6000_RS::set_fc "

Params: (center_mhz, offset_hz)"

%feature("docstring") MSDD6000_RS::set_ddc_samp_rate "

Params: (sample_rate_khz)"

%feature("docstring") MSDD6000_RS::set_ddc_bw "

Params: (bw_khz)"

%feature("docstring") MSDD6000_RS::start "

Params: (NONE)"

%feature("docstring") MSDD6000_RS::stop "

Params: (NONE)"

%feature("docstring") MSDD6000_RS::start_data "

Params: (NONE)"

%feature("docstring") MSDD6000_RS::stop_data "

Params: (NONE)"

%feature("docstring") MSDD6000_RS::pull_ddc_samp_rate "

Params: (NONE)"

%feature("docstring") MSDD6000_RS::pull_ddc_bw "

Params: (NONE)"

%feature("docstring") MSDD6000_RS::pull_rx_freq "

Params: (NONE)"

%feature("docstring") MSDD6000_RS::pull_ddc_gain "

Params: (NONE)"

%feature("docstring") MSDD6000_RS::pull_rf_atten "

Params: (NONE)"

%feature("docstring") MSDD6000_RS::send_request "

Params: (, , , , , , )"

%feature("docstring") MSDD6000_RS::read "

Params: (, )"



%feature("docstring") msdd_rs_source_simple::msdd_rs_source_simple "

Params: (src, port_src)"

%feature("docstring") msdd_rs_source_simple::~msdd_rs_source_simple "

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::stop "Called to disable drivers, etc for i/o devices.

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::start "Called to enable drivers, etc for i/o devices.

This allows a block to enable an associated driver to begin transfering data just before we start to execute the scheduler. The end result is that this reduces latency in the pipeline when dealing with audio devices, usrps, etc.

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::start_data "

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::stop_data "

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::set_ddc_samp_rate "

Params: ()"

%feature("docstring") msdd_rs_source_simple::set_ddc_bw "

Params: ()"

%feature("docstring") msdd_rs_source_simple::set_rx_freq "

Params: ()"

%feature("docstring") msdd_rs_source_simple::set_ddc_gain "

Params: ()"

%feature("docstring") msdd_rs_source_simple::set_rf_atten "

Params: ()"

%feature("docstring") msdd_rs_source_simple::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") msdd_rs_source_simple::pull_adc_freq "

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::pull_ddc_samp_rate "

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::pull_ddc_bw "

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::pull_rx_freq "

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::pull_ddc_gain "

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::pull_rf_atten "

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::gain_range "

Params: (NONE)"

%feature("docstring") msdd_rs_source_simple::freq_range "

Params: (NONE)"

%feature("docstring") gruel::msg_accepter "Virtual base class that accepts messages."

%feature("docstring") gruel::msg_accepter::msg_accepter "

Params: (NONE)"

%feature("docstring") gruel::msg_accepter::~msg_accepter "

Params: (NONE)"

%feature("docstring") gruel::msg_accepter::post "send  to

Sending a message is an asynchronous operation. The  call will not wait for the message either to arrive at the destination or to be received.

Params: (msg)"

%feature("docstring") gruel::msg_accepter_msgq "Concrete class that accepts messages and inserts them into a message queue."

%feature("docstring") gruel::msg_accepter_msgq::msg_accepter_msgq "

Params: (msgq)"

%feature("docstring") gruel::msg_accepter_msgq::~msg_accepter_msgq "

Params: (NONE)"

%feature("docstring") gruel::msg_accepter_msgq::post "send  to

Sending a message is an asynchronous operation. The  call will not wait for the message either to arrive at the destination or to be received.

Params: (msg)"

%feature("docstring") gruel::msg_accepter_msgq::msg_queue "

Params: (NONE)"

%feature("docstring") gruel::msg_queue "thread-safe message queue"

%feature("docstring") gruel::msg_queue::msg_queue "

Params: (limit)"

%feature("docstring") gruel::msg_queue::~msg_queue "

Params: (NONE)"

%feature("docstring") gruel::msg_queue::insert_tail "Insert message at tail of queue.

Block if queue if full.

Params: (msg)"

%feature("docstring") gruel::msg_queue::delete_head "Delete message from head of queue and return it. Block if no message is available.

Params: (NONE)"

%feature("docstring") gruel::msg_queue::delete_head_nowait "If there's a message in the q, delete it and return it. If no message is available, return pmt_t().

Params: (NONE)"

%feature("docstring") gruel::msg_queue::flush "Delete all messages from the queue.

Params: (NONE)"

%feature("docstring") gruel::msg_queue::empty_p "is the queue empty?

Params: (NONE)"

%feature("docstring") gruel::msg_queue::full_p "is the queue full?

Params: (NONE)"

%feature("docstring") gruel::msg_queue::count "return number of messages in queue

Params: (NONE)"

%feature("docstring") gruel::msg_queue::limit "return limit on number of message in queue. 0 -> unbounded

Params: (NONE)"

%feature("docstring") std::multimap "STL class."

%feature("docstring") std::multiset "STL class."

%feature("docstring") std::ofstream "STL class."

%feature("docstring") std::ostream "STL class."

%feature("docstring") std::ostringstream "STL class."

%feature("docstring") std::out_of_range "STL class."

%feature("docstring") std::overflow_error "STL class."

%feature("docstring") usrp2::pending_reply "Control mechanism to allow API calls to block waiting for reply packets"

%feature("docstring") usrp2::pending_reply::pending_reply "Construct a pending reply from the reply ID, response packet buffer, and buffer length.

Params: (rid, buffer, len)"

%feature("docstring") usrp2::pending_reply::~pending_reply "Destructor. Signals creating thread.

Params: (NONE)"

%feature("docstring") usrp2::pending_reply::wait_for_completion "Block, waiting for reply packet. Returns: 1 = ok, reply packet in buffer 0 = timeout

Params: (secs)"

%feature("docstring") usrp2::pending_reply::notify_completion "Allows creating thread to resume after copying reply into buffer

Params: (NONE)"

%feature("docstring") usrp2::pending_reply::rid "Retrieve pending reply ID

Params: (NONE)"

%feature("docstring") usrp2::pending_reply::buffer "Retrieve destination buffer address

Params: (NONE)"

%feature("docstring") usrp2::pending_reply::len "Retrieve destination buffer length

Params: (NONE)"



%feature("docstring") usrp2::pktfilter::pktfilter "

Params: (NONE)"

%feature("docstring") usrp2::pktfilter::~pktfilter "

Params: (NONE)"

%feature("docstring") usrp2::pktfilter::make_ethertype "Return a filter that harvests packets with the specified ethertype.

Params: (ethertype)"

%feature("docstring") usrp2::pktfilter::make_ethertype_inbound "Return a filter that harvests inbound packets with the specified ethertype.

Params: (ethertype, our_mac)"

%feature("docstring") usrp2::pktfilter::make_ethertype_inbound_target "Return a filter that harvests inbound packets with the specified ethertype and source MAC address.

Params: (ethertype, usrp_mac)"

%feature("docstring") plinfo "pipeline info that flows with data

Not all modules need all the info"

%feature("docstring") plinfo::plinfo "

Params: (NONE)"

%feature("docstring") plinfo::field_sync1_p "

Params: (NONE)"

%feature("docstring") plinfo::field_sync2_p "

Params: (NONE)"

%feature("docstring") plinfo::field_sync_p "

Params: (NONE)"

%feature("docstring") plinfo::regular_seg_p "

Params: (NONE)"

%feature("docstring") plinfo::in_field1_p "

Params: (NONE)"

%feature("docstring") plinfo::in_field2_p "

Params: (NONE)"

%feature("docstring") plinfo::first_regular_seg_p "

Params: (NONE)"

%feature("docstring") plinfo::transport_error_p "

Params: (NONE)"

%feature("docstring") plinfo::segno "

Params: (NONE)"

%feature("docstring") plinfo::flags "

Params: (NONE)"

%feature("docstring") plinfo::set_field_sync1 "

Params: (NONE)"

%feature("docstring") plinfo::set_field_sync2 "

Params: (NONE)"

%feature("docstring") plinfo::set_regular_seg "

Params: (field2, segno)"

%feature("docstring") plinfo::set_transport_error "

Params: (error)"

%feature("docstring") plinfo::operator== "

Params: (other)"

%feature("docstring") plinfo::operator!= "

Params: (other)"

%feature("docstring") plinfo::delay "Set  such that it reflects a  pipeline delay from .

Params: (out, in, nsegs_of_delay)"

%feature("docstring") plinfo::sanity_check "confirm that  is plausible

Params: (in)"



%feature("docstring") pmt::pmt_any::pmt_any "

Params: (any)"

%feature("docstring") pmt::pmt_any::is_any "

Params: (NONE)"

%feature("docstring") pmt::pmt_any::ref "

Params: (NONE)"

%feature("docstring") pmt::pmt_any::set "

Params: (any)"



%feature("docstring") pmt::pmt_base::pmt_base "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::~pmt_base "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_bool "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_symbol "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_number "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_integer "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_uint64 "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_real "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_complex "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_null "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_pair "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_tuple "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_dict "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_any "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_uniform_vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_u8vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_s8vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_u16vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_s16vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_u32vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_s32vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_u64vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_s64vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_f32vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_f64vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_c32vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_base::is_c64vector "

Params: (NONE)"



%feature("docstring") pmt::pmt_bool::pmt_bool "

Params: (NONE)"

%feature("docstring") pmt::pmt_bool::is_bool "

Params: (NONE)"



%feature("docstring") pmt::pmt_c32vector::pmt_c32vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_c32vector::is_c32vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_c32vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_c32vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_c32vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_c32vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_c32vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_c32vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_c32vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt_c32vector::pmt_c32vector "

Params: (k, fill)"

%feature("docstring") pmt_c32vector::is_c32vector "

Params: (NONE)"

%feature("docstring") pmt_c32vector::length "

Params: (NONE)"

%feature("docstring") pmt_c32vector::ref "

Params: (k)"

%feature("docstring") pmt_c32vector::set "

Params: (k, x)"

%feature("docstring") pmt_c32vector::elements "

Params: (len)"

%feature("docstring") pmt_c32vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_c32vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_c32vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_c64vector::pmt_c64vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_c64vector::is_c64vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_c64vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_c64vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_c64vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_c64vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_c64vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_c64vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_c64vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt_c64vector::pmt_c64vector "

Params: (k, fill)"

%feature("docstring") pmt_c64vector::is_c64vector "

Params: (NONE)"

%feature("docstring") pmt_c64vector::length "

Params: (NONE)"

%feature("docstring") pmt_c64vector::ref "

Params: (k)"

%feature("docstring") pmt_c64vector::set "

Params: (k, x)"

%feature("docstring") pmt_c64vector::elements "

Params: (len)"

%feature("docstring") pmt_c64vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_c64vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_c64vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_complex::pmt_complex "

Params: (value)"

%feature("docstring") pmt::pmt_complex::is_number "

Params: (NONE)"

%feature("docstring") pmt::pmt_complex::is_complex "

Params: (NONE)"

%feature("docstring") pmt::pmt_complex::value "

Params: (NONE)"



%feature("docstring") pmt::pmt_exception::pmt_exception "

Params: (msg, obj)"



%feature("docstring") pmt_f32vector::pmt_f32vector "

Params: (k, fill)"

%feature("docstring") pmt_f32vector::is_f32vector "

Params: (NONE)"

%feature("docstring") pmt_f32vector::length "

Params: (NONE)"

%feature("docstring") pmt_f32vector::ref "

Params: (k)"

%feature("docstring") pmt_f32vector::set "

Params: (k, x)"

%feature("docstring") pmt_f32vector::elements "

Params: (len)"

%feature("docstring") pmt_f32vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_f32vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_f32vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_f32vector::pmt_f32vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_f32vector::is_f32vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_f32vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_f32vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_f32vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_f32vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_f32vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_f32vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_f32vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt_f64vector::pmt_f64vector "

Params: (k, fill)"

%feature("docstring") pmt_f64vector::is_f64vector "

Params: (NONE)"

%feature("docstring") pmt_f64vector::length "

Params: (NONE)"

%feature("docstring") pmt_f64vector::ref "

Params: (k)"

%feature("docstring") pmt_f64vector::set "

Params: (k, x)"

%feature("docstring") pmt_f64vector::elements "

Params: (len)"

%feature("docstring") pmt_f64vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_f64vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_f64vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_f64vector::pmt_f64vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_f64vector::is_f64vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_f64vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_f64vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_f64vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_f64vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_f64vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_f64vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_f64vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_integer::pmt_integer "

Params: (value)"

%feature("docstring") pmt::pmt_integer::is_number "

Params: (NONE)"

%feature("docstring") pmt::pmt_integer::is_integer "

Params: (NONE)"

%feature("docstring") pmt::pmt_integer::value "

Params: (NONE)"



%feature("docstring") pmt::pmt_notimplemented::pmt_notimplemented "

Params: (msg, obj)"



%feature("docstring") pmt::pmt_null::pmt_null "

Params: (NONE)"

%feature("docstring") pmt::pmt_null::is_null "

Params: (NONE)"



%feature("docstring") pmt::pmt_out_of_range::pmt_out_of_range "

Params: (msg, obj)"



%feature("docstring") pmt::pmt_pair::pmt_pair "

Params: (car, cdr)"

%feature("docstring") pmt::pmt_pair::is_pair "

Params: (NONE)"

%feature("docstring") pmt::pmt_pair::car "

Params: (NONE)"

%feature("docstring") pmt::pmt_pair::cdr "

Params: (NONE)"

%feature("docstring") pmt::pmt_pair::set_car "

Params: (car)"

%feature("docstring") pmt::pmt_pair::set_cdr "

Params: (cdr)"

%feature("docstring") pmt::pmt_pool "very simple thread-safe fixed-size allocation pool

FIXME may want to go to global allocation with per-thread free list. This would eliminate virtually all lock contention."

%feature("docstring") pmt::pmt_pool::pmt_pool "

Params: (itemsize, alignment, allocation_size, max_items)"

%feature("docstring") pmt::pmt_pool::~pmt_pool "

Params: (NONE)"

%feature("docstring") pmt::pmt_pool::malloc "

Params: (NONE)"

%feature("docstring") pmt::pmt_pool::free "

Params: (p)"



%feature("docstring") pmt::pmt_real::pmt_real "

Params: (value)"

%feature("docstring") pmt::pmt_real::is_number "

Params: (NONE)"

%feature("docstring") pmt::pmt_real::is_real "

Params: (NONE)"

%feature("docstring") pmt::pmt_real::value "

Params: (NONE)"



%feature("docstring") pmt_s16vector::pmt_s16vector "

Params: (k, fill)"

%feature("docstring") pmt_s16vector::is_s16vector "

Params: (NONE)"

%feature("docstring") pmt_s16vector::length "

Params: (NONE)"

%feature("docstring") pmt_s16vector::ref "

Params: (k)"

%feature("docstring") pmt_s16vector::set "

Params: (k, x)"

%feature("docstring") pmt_s16vector::elements "

Params: (len)"

%feature("docstring") pmt_s16vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_s16vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_s16vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_s16vector::pmt_s16vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_s16vector::is_s16vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_s16vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_s16vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_s16vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_s16vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_s16vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_s16vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_s16vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt_s32vector::pmt_s32vector "

Params: (k, fill)"

%feature("docstring") pmt_s32vector::is_s32vector "

Params: (NONE)"

%feature("docstring") pmt_s32vector::length "

Params: (NONE)"

%feature("docstring") pmt_s32vector::ref "

Params: (k)"

%feature("docstring") pmt_s32vector::set "

Params: (k, x)"

%feature("docstring") pmt_s32vector::elements "

Params: (len)"

%feature("docstring") pmt_s32vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_s32vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_s32vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_s32vector::pmt_s32vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_s32vector::is_s32vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_s32vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_s32vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_s32vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_s32vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_s32vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_s32vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_s32vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_s64vector::pmt_s64vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_s64vector::is_s64vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_s64vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_s64vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_s64vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_s64vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_s64vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_s64vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_s64vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt_s64vector::pmt_s64vector "

Params: (k, fill)"

%feature("docstring") pmt_s64vector::is_s64vector "

Params: (NONE)"

%feature("docstring") pmt_s64vector::length "

Params: (NONE)"

%feature("docstring") pmt_s64vector::ref "

Params: (k)"

%feature("docstring") pmt_s64vector::set "

Params: (k, x)"

%feature("docstring") pmt_s64vector::elements "

Params: (len)"

%feature("docstring") pmt_s64vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_s64vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_s64vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt_s8vector::pmt_s8vector "

Params: (k, fill)"

%feature("docstring") pmt_s8vector::is_s8vector "

Params: (NONE)"

%feature("docstring") pmt_s8vector::length "

Params: (NONE)"

%feature("docstring") pmt_s8vector::ref "

Params: (k)"

%feature("docstring") pmt_s8vector::set "

Params: (k, x)"

%feature("docstring") pmt_s8vector::elements "

Params: (len)"

%feature("docstring") pmt_s8vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_s8vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_s8vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_s8vector::pmt_s8vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_s8vector::is_s8vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_s8vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_s8vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_s8vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_s8vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_s8vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_s8vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_s8vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_symbol::pmt_symbol "

Params: (name)"

%feature("docstring") pmt::pmt_symbol::is_symbol "

Params: (NONE)"

%feature("docstring") pmt::pmt_symbol::name "

Params: (NONE)"

%feature("docstring") pmt::pmt_symbol::next "

Params: (NONE)"

%feature("docstring") pmt::pmt_symbol::set_next "

Params: (next)"



%feature("docstring") pmt::pmt_tuple::pmt_tuple "

Params: (len)"

%feature("docstring") pmt::pmt_tuple::is_tuple "

Params: (NONE)"

%feature("docstring") pmt::pmt_tuple::ref "

Params: (k)"

%feature("docstring") pmt::pmt_tuple::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_tuple::_ref "

Params: (k)"

%feature("docstring") pmt::pmt_tuple::_set "

Params: (k, v)"



%feature("docstring") pmt::pmt_u16vector::pmt_u16vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_u16vector::is_u16vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_u16vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_u16vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_u16vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_u16vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_u16vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_u16vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_u16vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt_u16vector::pmt_u16vector "

Params: (k, fill)"

%feature("docstring") pmt_u16vector::is_u16vector "

Params: (NONE)"

%feature("docstring") pmt_u16vector::length "

Params: (NONE)"

%feature("docstring") pmt_u16vector::ref "

Params: (k)"

%feature("docstring") pmt_u16vector::set "

Params: (k, x)"

%feature("docstring") pmt_u16vector::elements "

Params: (len)"

%feature("docstring") pmt_u16vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_u16vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_u16vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt_u32vector::pmt_u32vector "

Params: (k, fill)"

%feature("docstring") pmt_u32vector::is_u32vector "

Params: (NONE)"

%feature("docstring") pmt_u32vector::length "

Params: (NONE)"

%feature("docstring") pmt_u32vector::ref "

Params: (k)"

%feature("docstring") pmt_u32vector::set "

Params: (k, x)"

%feature("docstring") pmt_u32vector::elements "

Params: (len)"

%feature("docstring") pmt_u32vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_u32vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_u32vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_u32vector::pmt_u32vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_u32vector::is_u32vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_u32vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_u32vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_u32vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_u32vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_u32vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_u32vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_u32vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt_u64vector::pmt_u64vector "

Params: (k, fill)"

%feature("docstring") pmt_u64vector::is_u64vector "

Params: (NONE)"

%feature("docstring") pmt_u64vector::length "

Params: (NONE)"

%feature("docstring") pmt_u64vector::ref "

Params: (k)"

%feature("docstring") pmt_u64vector::set "

Params: (k, x)"

%feature("docstring") pmt_u64vector::elements "

Params: (len)"

%feature("docstring") pmt_u64vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_u64vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_u64vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_u64vector::pmt_u64vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_u64vector::is_u64vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_u64vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_u64vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_u64vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_u64vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_u64vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_u64vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_u64vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt_u8vector::pmt_u8vector "

Params: (k, fill)"

%feature("docstring") pmt_u8vector::is_u8vector "

Params: (NONE)"

%feature("docstring") pmt_u8vector::length "

Params: (NONE)"

%feature("docstring") pmt_u8vector::ref "

Params: (k)"

%feature("docstring") pmt_u8vector::set "

Params: (k, x)"

%feature("docstring") pmt_u8vector::elements "

Params: (len)"

%feature("docstring") pmt_u8vector::writable_elements "

Params: (len)"

%feature("docstring") pmt_u8vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt_u8vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_u8vector::pmt_u8vector "

Params: (k, fill)"

%feature("docstring") pmt::pmt_u8vector::is_u8vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_u8vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_u8vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_u8vector::set "

Params: (k, x)"

%feature("docstring") pmt::pmt_u8vector::elements "

Params: (len)"

%feature("docstring") pmt::pmt_u8vector::writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_u8vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_u8vector::uniform_writable_elements "

Params: (len)"



%feature("docstring") pmt::pmt_uint64::pmt_uint64 "

Params: (value)"

%feature("docstring") pmt::pmt_uint64::is_number "

Params: (NONE)"

%feature("docstring") pmt::pmt_uint64::is_uint64 "

Params: (NONE)"

%feature("docstring") pmt::pmt_uint64::value "

Params: (NONE)"



%feature("docstring") pmt::pmt_uniform_vector::is_uniform_vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_uniform_vector::uniform_elements "

Params: (len)"

%feature("docstring") pmt::pmt_uniform_vector::uniform_writable_elements "

Params: (len)"

%feature("docstring") pmt::pmt_uniform_vector::length "

Params: (NONE)"



%feature("docstring") pmt::pmt_vector::pmt_vector "

Params: (len, fill)"

%feature("docstring") pmt::pmt_vector::is_vector "

Params: (NONE)"

%feature("docstring") pmt::pmt_vector::ref "

Params: (k)"

%feature("docstring") pmt::pmt_vector::set "

Params: (k, obj)"

%feature("docstring") pmt::pmt_vector::fill "

Params: (fill)"

%feature("docstring") pmt::pmt_vector::length "

Params: (NONE)"

%feature("docstring") pmt::pmt_vector::_ref "

Params: (k)"



%feature("docstring") pmt::pmt_wrong_type::pmt_wrong_type "

Params: (msg, obj)"

%feature("docstring") ppio "abstract class that provides low level access to parallel port bits"

%feature("docstring") ppio::ppio "

Params: (NONE)"

%feature("docstring") ppio::~ppio "

Params: (NONE)"

%feature("docstring") ppio::write_data "

Params: (v)"

%feature("docstring") ppio::read_data "

Params: (NONE)"

%feature("docstring") ppio::write_control "

Params: (v)"

%feature("docstring") ppio::read_control "

Params: (NONE)"

%feature("docstring") ppio::read_status "

Params: (NONE)"

%feature("docstring") ppio::lock "

Params: (NONE)"

%feature("docstring") ppio::unlock "

Params: (NONE)"

%feature("docstring") ppio_ppdev "access to parallel port bits using the linux ppdev interface"

%feature("docstring") ppio_ppdev::ppio_ppdev "

Params: (which)"

%feature("docstring") ppio_ppdev::~ppio_ppdev "

Params: (NONE)"

%feature("docstring") ppio_ppdev::write_data "

Params: (v)"

%feature("docstring") ppio_ppdev::read_data "

Params: (NONE)"

%feature("docstring") ppio_ppdev::write_control "

Params: (v)"

%feature("docstring") ppio_ppdev::read_control "

Params: (NONE)"

%feature("docstring") ppio_ppdev::read_status "

Params: (NONE)"

%feature("docstring") ppio_ppdev::lock "

Params: (NONE)"

%feature("docstring") ppio_ppdev::unlock "

Params: (NONE)"

%feature("docstring") std::priority_queue "STL class."



%feature("docstring") psk::psk "

Params: (gr_constell)"



%feature("docstring") qam::qam "

Params: (gr_constell)"

%feature("docstring") sssr::quad_filter "quad filter (used to compute timing error)"

%feature("docstring") sssr::quad_filter::quad_filter "

Params: (NONE)"

%feature("docstring") sssr::quad_filter::reset "called on channel change

Params: (NONE)"

%feature("docstring") sssr::quad_filter::update "

Params: (sample)"

%feature("docstring") std::queue "STL class."

%feature("docstring") std::range_error "STL class."

%feature("docstring") std::wstring::reverse_iterator "STL iterator class."

%feature("docstring") std::multimap::reverse_iterator "STL iterator class."

%feature("docstring") std::vector::reverse_iterator "STL iterator class."

%feature("docstring") std::deque::reverse_iterator "STL iterator class."

%feature("docstring") std::multiset::reverse_iterator "STL iterator class."

%feature("docstring") std::basic_string::reverse_iterator "STL iterator class."

%feature("docstring") std::set::reverse_iterator "STL iterator class."

%feature("docstring") std::map::reverse_iterator "STL iterator class."

%feature("docstring") std::list::reverse_iterator "STL iterator class."

%feature("docstring") std::string::reverse_iterator "STL iterator class."



%feature("docstring") usrp2::ring::inc_read_ind "

Params: (NONE)"

%feature("docstring") usrp2::ring::inc_write_ind "

Params: (NONE)"

%feature("docstring") usrp2::ring::empty "

Params: (NONE)"

%feature("docstring") usrp2::ring::full "

Params: (NONE)"

%feature("docstring") usrp2::ring::ring "

Params: (entries)"

%feature("docstring") usrp2::ring::wait_for_not_empty "

Params: (NONE)"

%feature("docstring") usrp2::ring::enqueue "

Params: (p, len)"

%feature("docstring") usrp2::ring::dequeue "

Params: (p, len)"

%feature("docstring") std::runtime_error "STL class."



%feature("docstring") rx_16sc_handler::rx_16sc_handler "

Params: (max_samples, max_quantum, dest)"

%feature("docstring") rx_16sc_handler::make "

Params: (max_samples, max_quantum, dest)"

%feature("docstring") rx_16sc_handler::operator() "Function operator invoked by USRP2 RX API. Derived classes must override this method but then invoke it at the start of their processing. This operator will always be called at least once.


 points to the raw sample data received off of the ethernet. The data is packed into big-endian 32-bit unsigned ints for transport, but the actual format of the data is dependent on the current configuration of the USRP2. The most common format is 16-bit I & Q, with I in the top of the 32-bit word.

Params: (items, nitems, metadata)"

%feature("docstring") rx_16sc_handler::~rx_16sc_handler "

Params: (NONE)"



%feature("docstring") rx_32fc_handler::rx_32fc_handler "

Params: (max_samples, max_quantum, dest)"

%feature("docstring") rx_32fc_handler::make "

Params: (max_samples, max_quantum, dest)"

%feature("docstring") rx_32fc_handler::operator() "Function operator invoked by USRP2 RX API. Derived classes must override this method but then invoke it at the start of their processing. This operator will always be called at least once.


 points to the raw sample data received off of the ethernet. The data is packed into big-endian 32-bit unsigned ints for transport, but the actual format of the data is dependent on the current configuration of the USRP2. The most common format is 16-bit I & Q, with I in the top of the 32-bit word.

Params: (items, nitems, metadata)"

%feature("docstring") rx_32fc_handler::~rx_32fc_handler "

Params: (NONE)"



%feature("docstring") usrp2::rx_nop_handler::rx_nop_handler "Constructor

Params: (max_samples, max_quantum)"

%feature("docstring") usrp2::rx_nop_handler::~rx_nop_handler "Destructor. Derived classes must implement their own, non-inline destructor.

Params: (NONE)"

%feature("docstring") usrp2::rx_nop_handler::nframes "Returns number of frames this copier was called with.

Params: (NONE)"

%feature("docstring") usrp2::rx_nop_handler::nsamples "Returns actual number of samples copied.

Params: (NONE)"

%feature("docstring") usrp2::rx_nop_handler::max_samples "Returns maximum number of samples that will be copied.

Params: (NONE)"

%feature("docstring") usrp2::rx_nop_handler::has_errored_p "Returns true if an error has occurred. Derived classes must set d_err to true when an error occurs in the () operator

Params: (NONE)"

%feature("docstring") usrp2::rx_nop_handler::has_finished_p "Returns true if this instance has reached the maximum number of samples.

Params: (NONE)"

%feature("docstring") usrp2::rx_nop_handler::operator() "Function operator invoked by USRP2 RX API. Derived classes must override this method but then invoke it at the start of their processing. This operator will always be called at least once.


 points to the raw sample data received off of the ethernet. The data is packed into big-endian 32-bit unsigned ints for transport, but the actual format of the data is dependent on the current configuration of the USRP2. The most common format is 16-bit I & Q, with I in the top of the 32-bit word.

Params: (items, nitems, metadata)"

%feature("docstring") usrp2::rx_sample_handler "Abstract function object called to handle received data blocks.

An object derived from this class is passed to usrp2::rx_samples to process the received frames of samples."

%feature("docstring") usrp2::rx_sample_handler::~rx_sample_handler "

Params: (NONE)"

%feature("docstring") usrp2::rx_sample_handler::operator() "points to the raw sample data received off of the ethernet. The data is packed into big-endian 32-bit unsigned ints for transport, but the actual format of the data is dependent on the current configuration of the USRP2. The most common format is 16-bit I & Q, with I in the top of the 32-bit word.

This is the general purpose, low level interface and relies on other functions to handle all required endian-swapping and format conversion.

Params: (items, nitems, metadata)"



%feature("docstring") s_both::s_both "

Params: (l_node, l_this)"

%feature("docstring") s_both::~s_both "

Params: (NONE)"

%feature("docstring") s_both::node "

Params: (NONE)"

%feature("docstring") s_both::This "

Params: (NONE)"

%feature("docstring") s_both::set "

Params: (l_node, l_this)"



%feature("docstring") s_buffer::s_buffer "

Params: (n_alloc)"

%feature("docstring") s_buffer::~s_buffer "

Params: (NONE)"

%feature("docstring") s_buffer::n_used "

Params: (NONE)"

%feature("docstring") s_buffer::n_alloc "

Params: (NONE)"

%feature("docstring") s_buffer::buffer "

Params: (l_buffer, bufLen)"

%feature("docstring") s_buffer::reset "

Params: (NONE)"



%feature("docstring") s_node::s_node "

Params: (l_object, l_prev, l_next)"

%feature("docstring") s_node::~s_node "

Params: (NONE)"

%feature("docstring") s_node::remove "

Params: (NONE)"

%feature("docstring") s_node::insert_before "

Params: (l_next)"

%feature("docstring") s_node::insert_after "

Params: (l_prev)"

%feature("docstring") s_node::object "

Params: (NONE)"

%feature("docstring") s_node::available "

Params: (NONE)"

%feature("docstring") s_node::set_available "

Params: (NONE)"

%feature("docstring") s_node::set_not_available "

Params: (NONE)"

%feature("docstring") s_node::next "

Params: (NONE)"

%feature("docstring") s_node::prev "

Params: (NONE)"

%feature("docstring") s_node::both "

Params: (NONE)"

%feature("docstring") sdr_1000_base "Very low level interface to SDR 1000 xcvr hardware."

%feature("docstring") sdr_1000_base::sdr_1000_base "

Params: (which_pp)"

%feature("docstring") sdr_1000_base::~sdr_1000_base "

Params: (NONE)"

%feature("docstring") sdr_1000_base::reset "

Params: (NONE)"

%feature("docstring") sdr_1000_base::write_latch "

Params: (which, value, mask)"

%feature("docstring") sssr::seg_sync_integrator "segment sync integrator"

%feature("docstring") sssr::seg_sync_integrator::seg_sync_integrator "

Params: (NONE)"

%feature("docstring") sssr::seg_sync_integrator::reset "called on channel change

Params: (NONE)"

%feature("docstring") sssr::seg_sync_integrator::update "update current tap with weight and return integrated correlation value

Params: (weight, index)"

%feature("docstring") sssr::seg_sync_integrator::find_max "return index of maximum correlation value

Params: (value)"

%feature("docstring") std::set "STL class."

%feature("docstring") boost::shared_ptr "shared_ptr documentation stub

An enhanced relative of scoped_ptr with reference counted copy semantics. The object pointed to is deleted when the last shared_ptr pointing to it is destroyed or reset."



%feature("docstring") atsci_sliding_correlator::shift_reg::shift_reg "

Params: (NONE)"

%feature("docstring") atsci_sliding_correlator::shift_reg::reset "

Params: (NONE)"

%feature("docstring") atsci_sliding_correlator::shift_reg::shift_in "

Params: (bit)"



%feature("docstring") std::stack "STL class."

%feature("docstring") std::string "STL class."

%feature("docstring") std::stringstream "STL class."



%feature("docstring") gruel::thread_body_wrapper::thread_body_wrapper "

Params: (f, name)"

%feature("docstring") gruel::thread_body_wrapper::operator() "

Params: (NONE)"



%feature("docstring") gruel::thread_group::thread_group "

Params: (NONE)"

%feature("docstring") gruel::thread_group::~thread_group "

Params: (NONE)"

%feature("docstring") gruel::thread_group::create_thread "

Params: (threadfunc)"

%feature("docstring") gruel::thread_group::add_thread "

Params: (thrd)"

%feature("docstring") gruel::thread_group::remove_thread "

Params: (thrd)"

%feature("docstring") gruel::thread_group::join_all "

Params: (NONE)"

%feature("docstring") gruel::thread_group::interrupt_all "

Params: (NONE)"

%feature("docstring") gruel::thread_group::size "

Params: (NONE)"



%feature("docstring") usrp2::tune_result::tune_result "

Params: (baseband, dxc, residual, inverted)"



%feature("docstring") uhd_multi_usrp_sink::uhd_multi_usrp_sink "Set the IO signature for this block.

Params: (sig)"

%feature("docstring") uhd_multi_usrp_sink::set_subdev_spec "Set the subdevice specification.

Params: (spec, mboard)"

%feature("docstring") uhd_multi_usrp_sink::set_samp_rate "Set the sample rate for the usrp device.

Params: (rate)"

%feature("docstring") uhd_multi_usrp_sink::get_samp_rate "Get the sample rate for the usrp device. This is the actual sample rate and may differ from the rate set.

Params: ()"

%feature("docstring") uhd_multi_usrp_sink::set_center_freq "Tune the usrp device to the desired center frequency.

Params: (tune_request, chan)"

%feature("docstring") uhd_multi_usrp_sink::get_freq_range "Get the tunable frequency range.

Params: (chan)"

%feature("docstring") uhd_multi_usrp_sink::set_gain "Set the gain for the dboard.

Params: (gain, chan)"

%feature("docstring") uhd_multi_usrp_sink::get_gain "Get the actual dboard gain setting.

Params: (chan)"

%feature("docstring") uhd_multi_usrp_sink::get_gain_range "Get the settable gain range.

Params: (chan)"

%feature("docstring") uhd_multi_usrp_sink::set_antenna "Set the antenna to use.

Params: (ant, chan)"

%feature("docstring") uhd_multi_usrp_sink::get_antenna "Get the antenna in use.

Params: (chan)"

%feature("docstring") uhd_multi_usrp_sink::get_antennas "Get a list of possible antennas.

Params: (chan)"

%feature("docstring") uhd_multi_usrp_sink::set_bandwidth "Set the subdevice bandpass filter.

Params: (bandwidth, chan)"

%feature("docstring") uhd_multi_usrp_sink::set_clock_config "Set the clock configuration.

Params: (clock_config, mboard)"

%feature("docstring") uhd_multi_usrp_sink::get_time_now "Get the current time registers.

Params: ()"

%feature("docstring") uhd_multi_usrp_sink::set_time_next_pps "Set the time registers at the next pps.

Params: (time_spec)"

%feature("docstring") uhd_multi_usrp_sink::set_time_unknown_pps "Sync the time registers with an unknown pps edge.

Params: (time_spec)"

%feature("docstring") uhd_multi_usrp_sink::get_device "Get access to the underlying uhd device object.

Params: ()"



%feature("docstring") uhd_multi_usrp_source::uhd_multi_usrp_source "Set the IO signature for this block.

Params: (sig)"

%feature("docstring") uhd_multi_usrp_source::set_subdev_spec "Set the subdevice specification.

Params: (spec, mboard)"

%feature("docstring") uhd_multi_usrp_source::set_samp_rate "Set the sample rate for the usrp device.

Params: (rate)"

%feature("docstring") uhd_multi_usrp_source::get_samp_rate "Get the sample rate for the usrp device. This is the actual sample rate and may differ from the rate set.

Params: ()"

%feature("docstring") uhd_multi_usrp_source::set_center_freq "Tune the usrp device to the desired center frequency.

Params: (tune_request, chan)"

%feature("docstring") uhd_multi_usrp_source::get_freq_range "Get the tunable frequency range.

Params: (chan)"

%feature("docstring") uhd_multi_usrp_source::set_gain "Set the gain for the dboard.

Params: (gain, chan)"

%feature("docstring") uhd_multi_usrp_source::get_gain "Get the actual dboard gain setting.

Params: (chan)"

%feature("docstring") uhd_multi_usrp_source::get_gain_range "Get the settable gain range.

Params: (chan)"

%feature("docstring") uhd_multi_usrp_source::set_antenna "Set the antenna to use.

Params: (ant, chan)"

%feature("docstring") uhd_multi_usrp_source::get_antenna "Get the antenna in use.

Params: (chan)"

%feature("docstring") uhd_multi_usrp_source::get_antennas "Get a list of possible antennas.

Params: (chan)"

%feature("docstring") uhd_multi_usrp_source::set_bandwidth "Set the subdevice bandpass filter.

Params: (bandwidth, chan)"

%feature("docstring") uhd_multi_usrp_source::set_clock_config "Set the clock configuration.

Params: (clock_config, mboard)"

%feature("docstring") uhd_multi_usrp_source::get_time_now "Get the current time registers.

Params: ()"

%feature("docstring") uhd_multi_usrp_source::set_time_next_pps "Set the time registers at the next pps.

Params: (time_spec)"

%feature("docstring") uhd_multi_usrp_source::set_time_unknown_pps "Sync the time registers with an unknown pps edge.

Params: (time_spec)"

%feature("docstring") uhd_multi_usrp_source::get_device "Get access to the underlying uhd device object.

Params: ()"



%feature("docstring") uhd_single_usrp_sink::uhd_single_usrp_sink "Set the IO signature for this block.

Params: (sig)"

%feature("docstring") uhd_single_usrp_sink::set_subdev_spec "Set the subdevice specification.

Params: (spec)"

%feature("docstring") uhd_single_usrp_sink::set_samp_rate "Set the sample rate for the usrp device.

Params: (rate)"

%feature("docstring") uhd_single_usrp_sink::get_samp_rate "Get the sample rate for the usrp device. This is the actual sample rate and may differ from the rate set.

Params: ()"

%feature("docstring") uhd_single_usrp_sink::set_center_freq "Tune the usrp device to the desired center frequency.

Params: (tune_request, chan)"

%feature("docstring") uhd_single_usrp_sink::get_freq_range "Get the tunable frequency range.

Params: (chan)"

%feature("docstring") uhd_single_usrp_sink::set_gain "Set the gain for the dboard.

Params: (gain, chan)"

%feature("docstring") uhd_single_usrp_sink::get_gain "Get the actual dboard gain setting.

Params: (chan)"

%feature("docstring") uhd_single_usrp_sink::get_gain_range "Get the settable gain range.

Params: (chan)"

%feature("docstring") uhd_single_usrp_sink::set_antenna "Set the antenna to use.

Params: (ant, chan)"

%feature("docstring") uhd_single_usrp_sink::get_antenna "Get the antenna in use.

Params: (chan)"

%feature("docstring") uhd_single_usrp_sink::get_antennas "Get a list of possible antennas.

Params: (chan)"

%feature("docstring") uhd_single_usrp_sink::set_bandwidth "Set the subdevice bandpass filter.

Params: (bandwidth, chan)"

%feature("docstring") uhd_single_usrp_sink::set_clock_config "Set the clock configuration.

Params: (clock_config)"

%feature("docstring") uhd_single_usrp_sink::get_time_now "Get the current time registers.

Params: ()"

%feature("docstring") uhd_single_usrp_sink::set_time_now "Set the time registers asap.

Params: (time_spec)"

%feature("docstring") uhd_single_usrp_sink::set_time_next_pps "Set the time registers at the next pps.

Params: (time_spec)"

%feature("docstring") uhd_single_usrp_sink::get_device "Get access to the underlying uhd device object.

Params: ()"



%feature("docstring") uhd_single_usrp_source::uhd_single_usrp_source "Set the IO signature for this block.

Params: (sig)"

%feature("docstring") uhd_single_usrp_source::set_subdev_spec "Set the subdevice specification.

Params: (spec)"

%feature("docstring") uhd_single_usrp_source::set_samp_rate "Set the sample rate for the usrp device.

Params: (rate)"

%feature("docstring") uhd_single_usrp_source::get_samp_rate "Get the sample rate for the usrp device. This is the actual sample rate and may differ from the rate set.

Params: ()"

%feature("docstring") uhd_single_usrp_source::set_center_freq "Tune the usrp device to the desired center frequency.

Params: (tune_request, chan)"

%feature("docstring") uhd_single_usrp_source::get_freq_range "Get the tunable frequency range.

Params: (chan)"

%feature("docstring") uhd_single_usrp_source::set_gain "Set the gain for the dboard.

Params: (gain, chan)"

%feature("docstring") uhd_single_usrp_source::get_gain "Get the actual dboard gain setting.

Params: (chan)"

%feature("docstring") uhd_single_usrp_source::get_gain_range "Get the settable gain range.

Params: (chan)"

%feature("docstring") uhd_single_usrp_source::set_antenna "Set the antenna to use.

Params: (ant, chan)"

%feature("docstring") uhd_single_usrp_source::get_antenna "Get the antenna in use.

Params: (chan)"

%feature("docstring") uhd_single_usrp_source::get_antennas "Get a list of possible antennas.

Params: (chan)"

%feature("docstring") uhd_single_usrp_source::set_bandwidth "Set the subdevice bandpass filter.

Params: (bandwidth, chan)"

%feature("docstring") uhd_single_usrp_source::set_clock_config "Set the clock configuration.

Params: (clock_config)"

%feature("docstring") uhd_single_usrp_source::get_time_now "Get the current time registers.

Params: ()"

%feature("docstring") uhd_single_usrp_source::set_time_now "Set the time registers asap.

Params: (time_spec)"

%feature("docstring") uhd_single_usrp_source::set_time_next_pps "Set the time registers at the next pps.

Params: (time_spec)"

%feature("docstring") uhd_single_usrp_source::get_device "Get access to the underlying uhd device object.

Params: ()"

%feature("docstring") std::underflow_error "STL class."

%feature("docstring") usrp2::usrp2 "standard C++ interface to USRP2"

%feature("docstring") usrp2::usrp2::make "Static function to return an instance of usrp2 as a shared pointer

Params: (ifc, addr, rx_bufsize)"

%feature("docstring") usrp2::usrp2::~usrp2 "Class destructor

Params: (NONE)"

%feature("docstring") usrp2::usrp2::mac_addr "Returns the MAC address associated with this USRP

Params: (NONE)"

%feature("docstring") usrp2::usrp2::interface_name "Returns the GbE interface name associated with this USRP

Params: (NONE)"

%feature("docstring") usrp2::usrp2::set_rx_antenna "Set the rx antenna

Params: (ant)"

%feature("docstring") usrp2::usrp2::set_rx_gain "Set receiver gain

Params: (gain)"

%feature("docstring") usrp2::usrp2::rx_gain_min "return minimum Rx gain

Params: (NONE)"

%feature("docstring") usrp2::usrp2::rx_gain_max "return maximum Rx gain

Params: (NONE)"

%feature("docstring") usrp2::usrp2::rx_gain_db_per_step "return Rx gain db_per_step

Params: (NONE)"

%feature("docstring") usrp2::usrp2::set_rx_lo_offset "Set receive daughterboard LO offset frequency.

Params: (frequency)"

%feature("docstring") usrp2::usrp2::set_rx_center_freq "Set receiver center frequency

Params: (frequency, result)"

%feature("docstring") usrp2::usrp2::rx_freq_min "return minimum Rx center frequency

Params: (NONE)"

%feature("docstring") usrp2::usrp2::rx_freq_max "return maximum Rx center frequency

Params: (NONE)"

%feature("docstring") usrp2::usrp2::set_rx_decim "Set receiver sample rate decimation

Params: (decimation_factor)"

%feature("docstring") usrp2::usrp2::rx_decim "Return current decimation factor.

Params: (NONE)"

%feature("docstring") usrp2::usrp2::set_rx_scale_iq "Set receiver IQ magnitude scaling

Params: (scale_i, scale_q)"

%feature("docstring") usrp2::usrp2::start_rx_streaming "Set received sample format

domain: complex or real type: floating, fixed point, or raw depth: bits per sample

Sets format over the wire for samples from USRP2.

Start streaming receive mode. USRP2 will send a continuous stream of DSP pipeline samples to host. Call rx_samples(...) to access.

Params: (channel, items_per_frame)"

%feature("docstring") usrp2::usrp2::start_rx_streaming_at "Start streaming receive mode at specified timestamp. USRP2 will send a continuous stream of DSP pipeline samples to host. Call rx_samples(...) to access.

Params: (channel, items_per_frame, time)"

%feature("docstring") usrp2::usrp2::sync_and_start_rx_streaming_at "Sync to PPS and start streaming receive mode at specified timestamp. Just like calling sync_to_pps() and start_rx_streaming_at().

Params: (channel, items_per_frame, time)"

%feature("docstring") usrp2::usrp2::stop_rx_streaming "Stop streaming receive mode.

Params: (channel)"

%feature("docstring") usrp2::usrp2::rx_samples "Receive data from the specified channel This method is used to receive all data: streaming or discrete.

Params: (channel, handler)"

%feature("docstring") usrp2::usrp2::rx_overruns "Returns number of times receive overruns have occurred

Params: (NONE)"

%feature("docstring") usrp2::usrp2::rx_missing "Returns total number of missing frames from overruns.

Params: (NONE)"

%feature("docstring") usrp2::usrp2::set_tx_antenna "Set the tx antenna

Params: (ant)"

%feature("docstring") usrp2::usrp2::set_tx_gain "Set transmitter gain

Params: (gain)"

%feature("docstring") usrp2::usrp2::tx_gain_min "return minimum Tx gain

Params: (NONE)"

%feature("docstring") usrp2::usrp2::tx_gain_max "return maximum Tx gain

Params: (NONE)"

%feature("docstring") usrp2::usrp2::tx_gain_db_per_step "return Tx gain db_per_step

Params: (NONE)"

%feature("docstring") usrp2::usrp2::set_tx_lo_offset "

Params: (frequency)"

%feature("docstring") usrp2::usrp2::set_tx_center_freq "Set transmitter center frequency

Params: (frequency, result)"

%feature("docstring") usrp2::usrp2::tx_freq_min "return minimum Tx center frequency

Params: (NONE)"

%feature("docstring") usrp2::usrp2::tx_freq_max "return maximum Tx center frequency

Params: (NONE)"

%feature("docstring") usrp2::usrp2::set_tx_interp "Set transmitter sample rate interpolation

Params: (interpolation_factor)"

%feature("docstring") usrp2::usrp2::tx_interp "Return current interpolation factor.

Params: (NONE)"

%feature("docstring") usrp2::usrp2::default_tx_scale_iq "

Params: (interpolation_factor, scale_i, scale_q)"

%feature("docstring") usrp2::usrp2::set_tx_scale_iq "Set transmit IQ magnitude scaling

Params: (scale_i, scale_q)"

%feature("docstring") usrp2::usrp2::tx_32fc "transmit complex<float> samples to USRP2

Set transmit sample format

domain: complex or real type: floating, fixed point, or raw depth: bits per sample

Sets format over the wire for samples to USRP2.


The complex<float> samples are converted to the appropriate \"on the wire\" representation, depending on the current USRP2 configuration. Typically, this is big-endian 16-bit I & Q.

Params: (channel, samples, nsamples, metadata)"

%feature("docstring") usrp2::usrp2::tx_16sc "transmit complex<int16_t> samples to USRP2

The complex<int16_t> samples are converted to the appropriate \"on the wire\" representation, depending on the current USRP2 configuration. Typically, this is big-endian 16-bit I & Q.

Params: (channel, samples, nsamples, metadata)"

%feature("docstring") usrp2::usrp2::tx_raw "transmit raw uint32_t data items to USRP2

The caller is responsible for ensuring that the items are formatted appropriately for the USRP2 and its configuration. This method is used primarily by the system itself. Users should call tx_32fc or tx_16sc instead.

Params: (channel, items, nitems, metadata)"

%feature("docstring") usrp2::usrp2::config_mimo "MIMO configuration.

Params: (flags)"

%feature("docstring") usrp2::usrp2::fpga_master_clock_freq "Get frequency of master oscillator in Hz.

Params: (freq)"

%feature("docstring") usrp2::usrp2::adc_rate "

Params: (rate)"

%feature("docstring") usrp2::usrp2::dac_rate "

Params: (rate)"

%feature("docstring") usrp2::usrp2::tx_daughterboard_id "Get Tx daughterboard ID.

daughterboard id >= 0 if successful, -1 if no daugherboard installed, -2 if invalid EEPROM on daughterboard.

Params: (dbid)"

%feature("docstring") usrp2::usrp2::rx_daughterboard_id "Get Rx daughterboard ID.

daughterboard id >= 0 if successful, -1 if no daugherboard installed, -2 if invalid EEPROM on daughterboard.

Params: (dbid)"

%feature("docstring") usrp2::usrp2::burn_mac_addr "Burn new mac address into EEPROM on USRP2

Params: (new_addr)"

%feature("docstring") usrp2::usrp2::sync_to_pps "Reset master time to 0 at next PPS edge

Params: (NONE)"

%feature("docstring") usrp2::usrp2::sync_every_pps "Reset master time to 0 at every PPS edge

Params: (enable)"

%feature("docstring") usrp2::usrp2::peek32 "Read memory from Wishbone bus as 32-bit words. Handles endian swapping if needed.



WARNING: Attempts to read memory from addresses that do not correspond to RAM or memory-mapped peripherals may cause the USRP2 to hang, requiring a power cycle.

Params: (addr, words)"

%feature("docstring") usrp2::usrp2::poke32 "Write memory to Wishbone bus as 32-bit words. Handles endian swapping if needed.



WARNING: Attempts to read memory from addresses that do not correspond to RAM or memory-mapped peripherals may cause the USRP2 to hang, requiring a power cycle.

Params: (addr, data)"

%feature("docstring") usrp2::usrp2::set_gpio_ddr "Set daughterboard GPIO data direction register.



WARNING: Improper usage of this function may result in damage to the USRP2

Params: (bank, value, mask)"

%feature("docstring") usrp2::usrp2::set_gpio_sels "Set daughterboard GPIO output selection register. For those GPIO pins that are configured as outputs in the DDR, this settings configures the source of the pin value.



WARNING: Improper usage of this function may result in damage to the USRP2

Params: (bank, sels)"

%feature("docstring") usrp2::usrp2::write_gpio "Set daughterboard GPIO pin values.



WARNING: Improper usage of this function may result in damage to the USRP2

Params: (bank, value, mask)"

%feature("docstring") usrp2::usrp2::read_gpio "Read daughterboard GPIO pin values

Params: (bank, value)"

%feature("docstring") usrp2::usrp2::enable_gpio_streaming "Set GPIO streaming mode

Individually enables streaming GPIO pins through LSBs of DSP samples.

On receive, io_rx[15] replaces I[0], io_rx[14] replaces Q[0] On transmit, I[0] maps to io_tx[15], Q[0] maps to io_tx[14] (Transmit streaming is not yet implemented.)

The selected GPIO pins must have been set as inputs or outputs and, for transmit, set to software control.

When enabled, the replaced DSP sample LSBs become 0.



WARNING: Improper usage of this function may result in damage to the USRP2

Params: (bank, enable)"

%feature("docstring") usrp2::usrp2::find_existing_or_make_new "

Params: (ifc, p, rx_bufsize)"

%feature("docstring") usrp2::usrp2::usrp2 "

Params: (ifc, p, rx_bufsize)"

%feature("docstring") usrp2_base "Base class for all USRP2 blocks"

%feature("docstring") usrp2_base::usrp2_base "

Params: (name, input_signature, output_signature, ifc, mac)"

%feature("docstring") usrp2_base::~usrp2_base "

Params: (NONE)"

%feature("docstring") usrp2_base::mac_addr "Get USRP2 hardware MAC address.

Params: (NONE)"

%feature("docstring") usrp2_base::interface_name "Get interface name used to communicat with USRP2.

Params: (NONE)"

%feature("docstring") usrp2_base::fpga_master_clock_freq "Get USRP2 master clock rate.

Params: (freq)"

%feature("docstring") usrp2_base::config_mimo "MIMO configuration.

Params: (flags)"

%feature("docstring") usrp2_base::sync_to_pps "Set master time to 0 at next PPS rising edge.

Params: (NONE)"

%feature("docstring") usrp2_base::sync_every_pps "Reset master time to 0 at every PPS edge

Params: (enable)"

%feature("docstring") usrp2_base::peek32 "Read memory from Wishbone bus as words.

Params: (addr, words)"

%feature("docstring") usrp2_base::poke32 "Write memory to Wishbone bus as words.

Params: (addr, data)"

%feature("docstring") usrp2_base::start "Called by scheduler when starting flowgraph.

Params: (NONE)"

%feature("docstring") usrp2_base::stop "Called by scheduler when stopping flowgraph.

Params: (NONE)"

%feature("docstring") usrp2_base::work "Derived class must override this.

Params: (noutput_items, input_items, output_items)"

%feature("docstring") usrp2_sink_base "Base class for all USRP2 transmit blocks"

%feature("docstring") usrp2_sink_base::usrp2_sink_base "

Params: (name, input_signature, ifc, mac)"

%feature("docstring") usrp2_sink_base::~usrp2_sink_base "

Params: (NONE)"

%feature("docstring") usrp2_sink_base::set_antenna "Set antenna.

Params: (ant)"

%feature("docstring") usrp2_sink_base::set_gain "Set transmitter gain.

Params: (gain)"

%feature("docstring") usrp2_sink_base::set_lo_offset "Set transmitter LO offset frequency.

Params: (frequency)"

%feature("docstring") usrp2_sink_base::set_center_freq "Set transmitter center frequency.

Params: (frequency, tr)"

%feature("docstring") usrp2_sink_base::set_interp "Set transmit interpolation rate.

Params: (interp_factor)"

%feature("docstring") usrp2_sink_base::default_scale_iq "Calculate default scale_iq for given interpolation factor.

Params: (interpolation_factor, scale_i, scale_q)"

%feature("docstring") usrp2_sink_base::set_scale_iq "Set transmit IQ scale factors.

Params: (scale_i, scale_q)"

%feature("docstring") usrp2_sink_base::interp "Get transmit interpolation rate.

Params: (NONE)"

%feature("docstring") usrp2_sink_base::dac_rate "Get DAC sample rate in Hz.

Params: (rate)"

%feature("docstring") usrp2_sink_base::gain_min "Returns minimum Tx gain.

Params: (NONE)"

%feature("docstring") usrp2_sink_base::gain_max "Returns maximum Tx gain.

Params: (NONE)"

%feature("docstring") usrp2_sink_base::gain_db_per_step "Returns Tx gain db_per_step.

Params: (NONE)"

%feature("docstring") usrp2_sink_base::freq_min "Returns minimum Tx center frequency.

Params: (NONE)"

%feature("docstring") usrp2_sink_base::freq_max "Returns maximum Tx center frequency.

Params: (NONE)"

%feature("docstring") usrp2_sink_base::daughterboard_id "Get Tx daughterboard ID.

daughterboard id >= 0 if successful, -1 if no daugherboard installed, -2 if invalid EEPROM on daughterboard.

Params: (dbid)"

%feature("docstring") usrp2_sink_base::set_gpio_ddr "Set daughterboard GPIO data direction register.

Params: (value, mask)"

%feature("docstring") usrp2_sink_base::set_gpio_sels "Set daughterboard GPIO output selection register.

Params: (sels)"

%feature("docstring") usrp2_sink_base::write_gpio "Set daughterboard GPIO pin values.

Params: (value, mask)"

%feature("docstring") usrp2_sink_base::read_gpio "Read daughterboard GPIO pin values.

Params: (value)"

%feature("docstring") usrp2_sink_base::start_streaming_at "First samples begin streaming to USRP2 at given time.

Params: (time)"

%feature("docstring") usrp2_source_base "Base class for all USRP2 source blocks"

%feature("docstring") usrp2_source_base::usrp2_source_base "

Params: (name, output_signature, ifc, mac)"

%feature("docstring") usrp2_source_base::~usrp2_source_base "

Params: (NONE)"

%feature("docstring") usrp2_source_base::set_antenna "Set antenna.

Params: (ant)"

%feature("docstring") usrp2_source_base::set_gain "Set receiver gain.

Params: (gain)"

%feature("docstring") usrp2_source_base::set_lo_offset "Set receive LO offset frequency.

Params: (frequency)"

%feature("docstring") usrp2_source_base::set_center_freq "Set receiver center frequency.

Params: (frequency, tr)"

%feature("docstring") usrp2_source_base::set_decim "Set receive decimation rate.

Params: (decimation_factor)"

%feature("docstring") usrp2_source_base::set_scale_iq "Set receive IQ scale factors.

Params: (scale_i, scale_q)"

%feature("docstring") usrp2_source_base::decim "Get receive decimation rate.

Params: (NONE)"

%feature("docstring") usrp2_source_base::adc_rate "Get the ADC sample rate.

Params: (rate)"

%feature("docstring") usrp2_source_base::gain_min "Returns minimum Rx gain.

Params: (NONE)"

%feature("docstring") usrp2_source_base::gain_max "Returns maximum Rx gain.

Params: (NONE)"

%feature("docstring") usrp2_source_base::gain_db_per_step "Returns Rx gain db_per_step.

Params: (NONE)"

%feature("docstring") usrp2_source_base::freq_min "Returns minimum Rx center frequency.

Params: (NONE)"

%feature("docstring") usrp2_source_base::freq_max "Returns maximum Rx center frequency.

Params: (NONE)"

%feature("docstring") usrp2_source_base::daughterboard_id "Get Rx daughterboard ID.

daughterboard id >= 0 if successful, -1 if no daugherboard installed, -2 if invalid EEPROM on daughterboard.

Params: (dbid)"

%feature("docstring") usrp2_source_base::overruns "Returns number of receiver overruns.

Params: (NONE)"

%feature("docstring") usrp2_source_base::missing "Returns number of missing sequence numbers.

Params: (NONE)"

%feature("docstring") usrp2_source_base::start "Called by scheduler when starting flowgraph.

Params: (NONE)"

%feature("docstring") usrp2_source_base::stop "Called by scheduler when stopping flowgraph.

Params: (NONE)"

%feature("docstring") usrp2_source_base::set_gpio_ddr "Set daughterboard GPIO data direction register.

Params: (value, mask)"

%feature("docstring") usrp2_source_base::set_gpio_sels "Set daughterboard GPIO output selection register.

Params: (sels)"

%feature("docstring") usrp2_source_base::write_gpio "Set daughterboard GPIO pin values.

Params: (value, mask)"

%feature("docstring") usrp2_source_base::read_gpio "Read daughterboard GPIO pin values.

Params: (value)"

%feature("docstring") usrp2_source_base::enable_gpio_streaming "Enable streaming GPIO in sample LSBs.

Params: (enable)"

%feature("docstring") usrp_base "base class for GNU Radio interface to the USRP"

%feature("docstring") usrp_base::usrp_base "

Params: (name, input_signature, output_signature)"

%feature("docstring") usrp_base::set_usrp_basic "

Params: (u)"

%feature("docstring") usrp_base::~usrp_base "

Params: (NONE)"

%feature("docstring") usrp_base::db "

Params: (NONE)"

%feature("docstring") usrp_base::selected_subdev "given a usrp_subdev_spec, return the corresponding daughterboard object.

Params: (ss)"

%feature("docstring") usrp_base::fpga_master_clock_freq "return frequency of master oscillator on USRP

Params: (NONE)"

%feature("docstring") usrp_base::set_fpga_master_clock_freq "Tell API that the master oscillator on the USRP is operating at a non-standard fixed frequency. This is only needed for custom USRP hardware modified to operate at a different frequency from the default factory configuration. This function must be called prior to any other API function.

Params: (master_clock)"

%feature("docstring") usrp_base::set_verbose "

Params: (on)"

%feature("docstring") usrp_base::write_eeprom "Write EEPROM on motherboard or any daughterboard.

Params: (i2c_addr, eeprom_offset, buf)"

%feature("docstring") usrp_base::read_eeprom "Read EEPROM on motherboard or any daughterboard.

Params: (i2c_addr, eeprom_offset, len)"

%feature("docstring") usrp_base::write_i2c "Write to I2C peripheral.

Params: (i2c_addr, buf)"

%feature("docstring") usrp_base::read_i2c "Read from I2C peripheral.

Params: (i2c_addr, len)"

%feature("docstring") usrp_base::set_adc_offset "Set ADC offset correction.

Params: (which_adc, offset)"

%feature("docstring") usrp_base::set_dac_offset "Set DAC offset correction.

Params: (which_dac, offset, offset_pin)"

%feature("docstring") usrp_base::set_adc_buffer_bypass "Control ADC input buffer.

Params: (which_adc, bypass)"

%feature("docstring") usrp_base::set_dc_offset_cl_enable "Enable/disable automatic DC offset removal control loop in FPGA.

If the corresponding bit is set, enable the automatic DC offset correction control loop.





By default the control loop is enabled on all ADC's.

Params: (bits, mask)"

%feature("docstring") usrp_base::serial_number "return the usrp's serial number.

Params: (NONE)"

%feature("docstring") usrp_base::daughterboard_id "Return daughterboard ID for given side [0,1].

Params: (which_side)"

%feature("docstring") usrp_base::write_atr_tx_delay "Clock ticks to delay rising of T/R signal.

Params: (value)"

%feature("docstring") usrp_base::write_atr_rx_delay "Clock ticks to delay falling edge of T/R signal.

Params: (value)"

%feature("docstring") usrp_base::set_pga "Set Programmable Gain Amplifier (PGA).

gain is rounded to closest setting supported by hardware.

Params: (which_amp, gain_in_db)"

%feature("docstring") usrp_base::pga "Return programmable gain amplifier gain setting in dB.

Params: (which_amp)"

%feature("docstring") usrp_base::pga_min "Return minimum legal PGA gain in dB.

Params: (NONE)"

%feature("docstring") usrp_base::pga_max "Return maximum legal PGA gain in dB.

Params: (NONE)"

%feature("docstring") usrp_base::pga_db_per_step "Return hardware step size of PGA (linear in dB).

Params: (NONE)"

%feature("docstring") usrp_base::_write_oe "Write direction register (output enables) for pins that go to daughterboard.

Each d'board has 16-bits of general purpose i/o. Setting the bit makes it an output from the FPGA to the d'board.

This register is initialized based on a value stored in the d'board EEPROM. In general, you shouldn't be using this routine without a very good reason. Using this method incorrectly will kill your USRP motherboard and/or daughterboard.

Params: (which_side, value, mask)"

%feature("docstring") usrp_base::write_io "Write daughterboard i/o pin value.

Params: (which_side, value, mask)"

%feature("docstring") usrp_base::read_io "Read daughterboard i/o pin value.

Params: (which_side)"

%feature("docstring") usrp_base::write_refclk "Write daughterboard refclk config register.

Params: (which_side, value)"

%feature("docstring") usrp_base::write_atr_mask "

Params: (which_side, value)"

%feature("docstring") usrp_base::write_atr_txval "

Params: (which_side, value)"

%feature("docstring") usrp_base::write_atr_rxval "

Params: (which_side, value)"

%feature("docstring") usrp_base::write_aux_dac "Write auxiliary digital to analog converter.

Params: (which_side, which_dac, value)"

%feature("docstring") usrp_base::read_aux_adc "Read auxiliary analog to digital converter.

Params: (which_side, which_adc)"

%feature("docstring") usrp_base::converter_rate "returns A/D or D/A converter rate in Hz

Params: (NONE)"

%feature("docstring") usrp_base::_set_led "

Params: (which_led, on)"

%feature("docstring") usrp_base::_write_fpga_reg "Write FPGA register.

Params: (regno, value)"

%feature("docstring") usrp_base::_read_fpga_reg "Read FPGA register.

Params: (regno, value)"

%feature("docstring") usrp_base::_write_fpga_reg_masked "Write FPGA register with mask.

Params: (regno, value, mask)"

%feature("docstring") usrp_base::_write_9862 "Write AD9862 register.

Params: (which_codec, regno, value)"

%feature("docstring") usrp_base::_read_9862 "Read AD9862 register.

Params: (which_codec, regno)"

%feature("docstring") usrp_base::_write_spi "Write data to SPI bus peripheral.

If  specifies that optional_header bytes are present, they are written to the peripheral immediately prior to writing .

Params: (optional_header, enables, format, buf)"

%feature("docstring") usrp_base::_read_spi "

Params: (optional_header, enables, format, len)"

%feature("docstring") usrp_base::pick_subdev "Return an existing daughterboard from list of candidate dbids, or the first found on side A or side B.


Throws std::runtime_error if not found

Params: (candidates)"

%feature("docstring") usrp_basic "abstract base class for usrp operations"

%feature("docstring") usrp_basic::shutdown_daughterboards "

Params: (NONE)"

%feature("docstring") usrp_basic::init_db "

Params: (u)"

%feature("docstring") usrp_basic::usrp_basic "

Params: (which_board, open_interface, fpga_filename, firmware_filename)"

%feature("docstring") usrp_basic::set_usb_data_rate "advise usrp_basic of usb data rate (bytes/sec)

N.B., this doesn't tweak any hardware. Derived classes should call this to inform us of the data rate whenever it's first set or if it changes.

Params: (usb_data_rate)"

%feature("docstring") usrp_basic::_write_aux_dac "Write auxiliary digital to analog converter.

Params: (slot, which_dac, value)"

%feature("docstring") usrp_basic::_read_aux_adc "Read auxiliary analog to digital converter.

Params: (slot, which_adc, value)"

%feature("docstring") usrp_basic::~usrp_basic "

Params: (NONE)"

%feature("docstring") usrp_basic::db "Return a vector of vectors that contain shared pointers to the daughterboard instance(s) associated with the specified side.

It is an error to use the returned objects after the usrp_basic object has been destroyed.

Params: (NONE)"

%feature("docstring") usrp_basic::is_valid "is the subdev_spec valid?

Params: (ss)"

%feature("docstring") usrp_basic::selected_subdev "given a subdev_spec, return the corresponding daughterboard object.

Params: (ss)"

%feature("docstring") usrp_basic::fpga_master_clock_freq "return frequency of master oscillator on USRP

Params: (NONE)"

%feature("docstring") usrp_basic::set_fpga_master_clock_freq "Tell API that the master oscillator on the USRP is operating at a non-standard fixed frequency. This is only needed for custom USRP hardware modified to operate at a different frequency from the default factory configuration. This function must be called prior to any other API function.

Params: (master_clock)"

%feature("docstring") usrp_basic::usb_data_rate "

Params: (NONE)"

%feature("docstring") usrp_basic::set_verbose "

Params: (on)"

%feature("docstring") usrp_basic::write_eeprom "Write EEPROM on motherboard or any daughterboard.

Params: (i2c_addr, eeprom_offset, buf)"

%feature("docstring") usrp_basic::read_eeprom "Read EEPROM on motherboard or any daughterboard.

Params: (i2c_addr, eeprom_offset, len)"

%feature("docstring") usrp_basic::write_i2c "Write to I2C peripheral.

Params: (i2c_addr, buf)"

%feature("docstring") usrp_basic::read_i2c "Read from I2C peripheral.

Params: (i2c_addr, len)"

%feature("docstring") usrp_basic::set_adc_offset "Set ADC offset correction.

Params: (which_adc, offset)"

%feature("docstring") usrp_basic::set_dac_offset "Set DAC offset correction.

Params: (which_dac, offset, offset_pin)"

%feature("docstring") usrp_basic::set_adc_buffer_bypass "Control ADC input buffer.

Params: (which_adc, bypass)"

%feature("docstring") usrp_basic::set_dc_offset_cl_enable "Enable/disable automatic DC offset removal control loop in FPGA.

If the corresponding bit is set, enable the automatic DC offset correction control loop.





By default the control loop is enabled on all ADC's.

Params: (bits, mask)"

%feature("docstring") usrp_basic::serial_number "return the usrp's serial number.

Params: (NONE)"

%feature("docstring") usrp_basic::daughterboard_id "Return daughterboard ID for given side [0,1].

Params: (which_side)"

%feature("docstring") usrp_basic::write_atr_tx_delay "Clock ticks to delay rising of T/R signal.

Params: (value)"

%feature("docstring") usrp_basic::write_atr_rx_delay "Clock ticks to delay falling edge of T/R signal.

Params: (value)"

%feature("docstring") usrp_basic::common_set_pga "Set Programmable Gain Amplifier(PGA).

gain is rounded to closest setting supported by hardware.

Params: (txrx, which_amp, gain_in_db)"

%feature("docstring") usrp_basic::common_pga "Return programmable gain amplifier gain setting in dB.

Params: (txrx, which_amp)"

%feature("docstring") usrp_basic::common_pga_min "Return minimum legal PGA gain in dB.

Params: (txrx)"

%feature("docstring") usrp_basic::common_pga_max "Return maximum legal PGA gain in dB.

Params: (txrx)"

%feature("docstring") usrp_basic::common_pga_db_per_step "Return hardware step size of PGA(linear in dB).

Params: (txrx)"

%feature("docstring") usrp_basic::_common_write_oe "Write direction register(output enables) for pins that go to daughterboard.

Each d'board has 16-bits of general purpose i/o. Setting the bit makes it an output from the FPGA to the d'board.

This register is initialized based on a value stored in the d'board EEPROM. In general, you shouldn't be using this routine without a very good reason. Using this method incorrectly will kill your USRP motherboard and/or daughterboard.

Params: (txrx, which_side, value, mask)"

%feature("docstring") usrp_basic::common_write_io "Write daughterboard i/o pin value.

Params: (txrx, which_side, value, mask)"

%feature("docstring") usrp_basic::common_read_io "Read daughterboard i/o pin value.

Params: (txrx, which_side, value)"

%feature("docstring") usrp_basic::common_write_refclk "Write daughterboard refclk config register.

Params: (txrx, which_side, value)"

%feature("docstring") usrp_basic::common_write_atr_mask "Automatic Transmit/Receive switching.

Params: (txrx, which_side, value)"

%feature("docstring") usrp_basic::common_write_atr_txval "

Params: (txrx, which_side, value)"

%feature("docstring") usrp_basic::common_write_atr_rxval "

Params: (txrx, which_side, value)"

%feature("docstring") usrp_basic::common_write_aux_dac "Write auxiliary digital to analog converter.

Params: (txrx, which_side, which_dac, value)"

%feature("docstring") usrp_basic::common_read_aux_adc "Read auxiliary analog to digital converter.

Params: (txrx, which_side, which_adc, value)"

%feature("docstring") usrp_basic::set_pga "Set Programmable Gain Amplifier (PGA).

gain is rounded to closest setting supported by hardware.

Params: (which_amp, gain_in_db)"

%feature("docstring") usrp_basic::pga "Return programmable gain amplifier gain setting in dB.

Params: (which_amp)"

%feature("docstring") usrp_basic::pga_min "Return minimum legal PGA gain in dB.

Params: (NONE)"

%feature("docstring") usrp_basic::pga_max "Return maximum legal PGA gain in dB.

Params: (NONE)"

%feature("docstring") usrp_basic::pga_db_per_step "Return hardware step size of PGA (linear in dB).

Params: (NONE)"

%feature("docstring") usrp_basic::_write_oe "Write direction register (output enables) for pins that go to daughterboard.

Each d'board has 16-bits of general purpose i/o. Setting the bit makes it an output from the FPGA to the d'board.

This register is initialized based on a value stored in the d'board EEPROM. In general, you shouldn't be using this routine without a very good reason. Using this method incorrectly will kill your USRP motherboard and/or daughterboard.

Params: (which_side, value, mask)"

%feature("docstring") usrp_basic::write_io "Write daughterboard i/o pin value.

Params: (which_side, value, mask)"

%feature("docstring") usrp_basic::read_io "Read daughterboard i/o pin value.

Params: (which_side, value)"

%feature("docstring") usrp_basic::write_refclk "Write daughterboard refclk config register.

Params: (which_side, value)"

%feature("docstring") usrp_basic::write_atr_mask "

Params: (which_side, value)"

%feature("docstring") usrp_basic::write_atr_txval "

Params: (which_side, value)"

%feature("docstring") usrp_basic::write_atr_rxval "

Params: (which_side, value)"

%feature("docstring") usrp_basic::write_aux_dac "Write auxiliary digital to analog converter.

Params: (which_side, which_dac, value)"

%feature("docstring") usrp_basic::read_aux_adc "Read auxiliary analog to digital converter.

Params: (which_side, which_adc, value)"

%feature("docstring") usrp_basic::block_size "returns current fusb block size

Params: (NONE)"

%feature("docstring") usrp_basic::converter_rate "returns A/D or D/A converter rate in Hz

Params: (NONE)"

%feature("docstring") usrp_basic::_set_led "

Params: (which_led, on)"

%feature("docstring") usrp_basic::_write_fpga_reg "Write FPGA register.

Params: (regno, value)"

%feature("docstring") usrp_basic::_read_fpga_reg "Read FPGA register.

Params: (regno, value)"

%feature("docstring") usrp_basic::_write_fpga_reg_masked "Write FPGA register with mask.

Params: (regno, value, mask)"

%feature("docstring") usrp_basic::_write_9862 "Write AD9862 register.

Params: (which_codec, regno, value)"

%feature("docstring") usrp_basic::_read_9862 "Read AD9862 register.

Params: (which_codec, regno, value)"

%feature("docstring") usrp_basic::_write_spi "Write data to SPI bus peripheral.

If  specifies that optional_header bytes are present, they are written to the peripheral immediately prior to writing .

Params: (optional_header, enables, format, buf)"

%feature("docstring") usrp_basic::_read_spi "

Params: (optional_header, enables, format, len)"

%feature("docstring") usrp_basic::start "Start data transfers. Called in base class to derived class order.

Params: (NONE)"

%feature("docstring") usrp_basic::stop "Stop data transfers. Called in base class to derived class order.

Params: (NONE)"

%feature("docstring") usrp_basic_rx "class for accessing the receive side of the USRP"

%feature("docstring") usrp_basic_rx::usrp_basic_rx "

Params: (which_board, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_basic_rx::set_rx_enable "

Params: (on)"

%feature("docstring") usrp_basic_rx::rx_enable "

Params: (NONE)"

%feature("docstring") usrp_basic_rx::disable_rx "

Params: (NONE)"

%feature("docstring") usrp_basic_rx::restore_rx "

Params: (on)"

%feature("docstring") usrp_basic_rx::probe_rx_slots "

Params: (verbose)"

%feature("docstring") usrp_basic_rx::~usrp_basic_rx "

Params: (NONE)"

%feature("docstring") usrp_basic_rx::set_fpga_rx_sample_rate_divisor "tell the fpga the rate rx samples are coming from the A/D's

div = fpga_master_clock_freq () / sample_rate

sample_rate is determined by a myriad of registers in the 9862. That's why you have to tell us, so we can tell the fpga.

Params: (div)"

%feature("docstring") usrp_basic_rx::read "read data from the D/A's via the FPGA.  must be a multiple of 512 bytes.

If overrun is non-NULL it will be set true iff an RX overrun is detected.

Params: (buf, len, overrun)"

%feature("docstring") usrp_basic_rx::converter_rate "sampling rate of A/D converter

Params: (NONE)"

%feature("docstring") usrp_basic_rx::adc_rate "

Params: (NONE)"

%feature("docstring") usrp_basic_rx::daughterboard_id "Return daughterboard ID for given side [0,1].

Params: (which_side)"

%feature("docstring") usrp_basic_rx::set_pga "Set Programmable Gain Amplifier (PGA).

gain is rounded to closest setting supported by hardware.

Params: (which_amp, gain_in_db)"

%feature("docstring") usrp_basic_rx::pga "Return programmable gain amplifier gain setting in dB.

Params: (which_amp)"

%feature("docstring") usrp_basic_rx::pga_min "Return minimum legal PGA gain in dB.

Params: (NONE)"

%feature("docstring") usrp_basic_rx::pga_max "Return maximum legal PGA gain in dB.

Params: (NONE)"

%feature("docstring") usrp_basic_rx::pga_db_per_step "Return hardware step size of PGA (linear in dB).

Params: (NONE)"

%feature("docstring") usrp_basic_rx::_write_oe "Write direction register (output enables) for pins that go to daughterboard.

Each d'board has 16-bits of general purpose i/o. Setting the bit makes it an output from the FPGA to the d'board.

This register is initialized based on a value stored in the d'board EEPROM. In general, you shouldn't be using this routine without a very good reason. Using this method incorrectly will kill your USRP motherboard and/or daughterboard.

Params: (which_side, value, mask)"

%feature("docstring") usrp_basic_rx::write_io "Write daughterboard i/o pin value.

Params: (which_side, value, mask)"

%feature("docstring") usrp_basic_rx::read_io "Read daughterboard i/o pin value.

Params: (which_side, value)"

%feature("docstring") usrp_basic_rx::write_refclk "Write daughterboard refclk config register.

Params: (which_side, value)"

%feature("docstring") usrp_basic_rx::write_atr_mask "

Params: (which_side, value)"

%feature("docstring") usrp_basic_rx::write_atr_txval "

Params: (which_side, value)"

%feature("docstring") usrp_basic_rx::write_atr_rxval "

Params: (which_side, value)"

%feature("docstring") usrp_basic_rx::write_aux_dac "Write auxiliary digital to analog converter.

Params: (which_side, which_dac, value)"

%feature("docstring") usrp_basic_rx::read_aux_adc "Read auxiliary analog to digital converter.

Params: (which_side, which_adc, value)"

%feature("docstring") usrp_basic_rx::block_size "returns current fusb block size

Params: (NONE)"

%feature("docstring") usrp_basic_rx::start "Start data transfers. Called in base class to derived class order.

Params: (NONE)"

%feature("docstring") usrp_basic_rx::stop "Stop data transfers. Called in base class to derived class order.

Params: (NONE)"

%feature("docstring") usrp_basic_rx::make "invokes constructor, returns instance or 0 if trouble

Params: (which_board, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_basic_tx "class for accessing the transmit side of the USRP"

%feature("docstring") usrp_basic_tx::usrp_basic_tx "

Params: (which_board, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_basic_tx::set_tx_enable "

Params: (on)"

%feature("docstring") usrp_basic_tx::tx_enable "

Params: (NONE)"

%feature("docstring") usrp_basic_tx::disable_tx "

Params: (NONE)"

%feature("docstring") usrp_basic_tx::restore_tx "

Params: (on)"

%feature("docstring") usrp_basic_tx::probe_tx_slots "

Params: (verbose)"

%feature("docstring") usrp_basic_tx::~usrp_basic_tx "

Params: (NONE)"

%feature("docstring") usrp_basic_tx::set_fpga_tx_sample_rate_divisor "tell the fpga the rate tx samples are going to the D/A's

div = fpga_master_clock_freq () * 2

sample_rate is determined by a myriad of registers in the 9862. That's why you have to tell us, so we can tell the fpga.

Params: (div)"

%feature("docstring") usrp_basic_tx::write "Write data to the A/D's via the FPGA.

must be a multiple of 512 bytes. 
if  is non-NULL, it will be set to true iff a transmit underrun condition is detected.

Params: (buf, len, underrun)"

%feature("docstring") usrp_basic_tx::wait_for_completion "

Params: (NONE)"

%feature("docstring") usrp_basic_tx::converter_rate "sampling rate of D/A converter

Params: (NONE)"

%feature("docstring") usrp_basic_tx::dac_rate "

Params: (NONE)"

%feature("docstring") usrp_basic_tx::daughterboard_id "Return daughterboard ID for given side [0,1].

Params: (which_side)"

%feature("docstring") usrp_basic_tx::set_pga "Set Programmable Gain Amplifier (PGA).

gain is rounded to closest setting supported by hardware.

Params: (which_amp, gain_in_db)"

%feature("docstring") usrp_basic_tx::pga "Return programmable gain amplifier gain setting in dB.

Params: (which_amp)"

%feature("docstring") usrp_basic_tx::pga_min "Return minimum legal PGA gain in dB.

Params: (NONE)"

%feature("docstring") usrp_basic_tx::pga_max "Return maximum legal PGA gain in dB.

Params: (NONE)"

%feature("docstring") usrp_basic_tx::pga_db_per_step "Return hardware step size of PGA (linear in dB).

Params: (NONE)"

%feature("docstring") usrp_basic_tx::_write_oe "Write direction register (output enables) for pins that go to daughterboard.

Each d'board has 16-bits of general purpose i/o. Setting the bit makes it an output from the FPGA to the d'board.

This register is initialized based on a value stored in the d'board EEPROM. In general, you shouldn't be using this routine without a very good reason. Using this method incorrectly will kill your USRP motherboard and/or daughterboard.

Params: (which_side, value, mask)"

%feature("docstring") usrp_basic_tx::write_io "Write daughterboard i/o pin value.

Params: (which_side, value, mask)"

%feature("docstring") usrp_basic_tx::read_io "Read daughterboard i/o pin value.

Params: (which_side, value)"

%feature("docstring") usrp_basic_tx::write_refclk "Write daughterboard refclk config register.

Params: (which_side, value)"

%feature("docstring") usrp_basic_tx::write_atr_mask "

Params: (which_side, value)"

%feature("docstring") usrp_basic_tx::write_atr_txval "

Params: (which_side, value)"

%feature("docstring") usrp_basic_tx::write_atr_rxval "

Params: (which_side, value)"

%feature("docstring") usrp_basic_tx::write_aux_dac "Write auxiliary digital to analog converter.

Params: (which_side, which_dac, value)"

%feature("docstring") usrp_basic_tx::read_aux_adc "Read auxiliary analog to digital converter.

Params: (which_side, which_adc, value)"

%feature("docstring") usrp_basic_tx::block_size "returns current fusb block size

Params: (NONE)"

%feature("docstring") usrp_basic_tx::start "Start data transfers. Called in base class to derived class order.

Params: (NONE)"

%feature("docstring") usrp_basic_tx::stop "Stop data transfers. Called in base class to derived class order.

Params: (NONE)"

%feature("docstring") usrp_basic_tx::make "invokes constructor, returns instance or 0 if trouble

Params: (which_board, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_local_sighandler "Get and set signal handler.

Constructor installs new handler, destructor reinstalls original value."

%feature("docstring") usrp_local_sighandler::usrp_local_sighandler "

Params: (signum, new_handler)"

%feature("docstring") usrp_local_sighandler::~usrp_local_sighandler "

Params: (NONE)"

%feature("docstring") usrp_local_sighandler::throw_signal "

Params: (signum)"



%feature("docstring") usrp_rx_cfile::usrp_rx_cfile "

Params: (which, spec, decim, freq, gain, width8, nohb, output_shorts, nsamples, filename)"



%feature("docstring") usrp_siggen::usrp_siggen "

Params: (which, spec, rf_freq, interp, wfreq, waveform, amp, gain, offset, nsamples)"

%feature("docstring") usrp_signal "Representation of signal."

%feature("docstring") usrp_signal::usrp_signal "

Params: (signum)"

%feature("docstring") usrp_signal::signal "

Params: (NONE)"

%feature("docstring") usrp_signal::name "

Params: (NONE)"

%feature("docstring") usrp_sink_base "abstract interface to Universal Software Radio Peripheral Tx path (Rev 1)"

%feature("docstring") usrp_sink_base::usrp_sink_base "

Params: (name, input_signature, which_board, interp_rate, nchan, mux, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_sink_base::copy_to_usrp_buffer "convert between input item format and usrp native format

Params: (input_items, input_index, input_items_available, input_items_consumed, usrp_buffer, usrp_buffer_length, bytes_written)"

%feature("docstring") usrp_sink_base::~usrp_sink_base "

Params: (NONE)"

%feature("docstring") usrp_sink_base::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") usrp_sink_base::set_interp_rate "Set interpolator rate.  must be in [4, 1024] and a multiple of 4.

The final complex sample rate across the USB is dac_freq () / interp_rate () * nchannels ()

Params: (rate)"

%feature("docstring") usrp_sink_base::set_nchannels "

Params: (nchan)"

%feature("docstring") usrp_sink_base::set_mux "

Params: (mux)"

%feature("docstring") usrp_sink_base::determine_tx_mux_value "

Params: (ss)"

%feature("docstring") usrp_sink_base::set_tx_freq "set the frequency of the digital up converter.

must be 0.  is the center frequency in Hz. It must be in the range [-44M, 44M]. The frequency specified is quantized. Use tx_freq to retrieve the actual value used.

Params: (channel, freq)"

%feature("docstring") usrp_sink_base::dac_rate "

Params: (NONE)"

%feature("docstring") usrp_sink_base::dac_freq "

Params: (NONE)"

%feature("docstring") usrp_sink_base::interp_rate "

Params: (NONE)"

%feature("docstring") usrp_sink_base::nchannels "

Params: (NONE)"

%feature("docstring") usrp_sink_base::mux "

Params: (NONE)"

%feature("docstring") usrp_sink_base::tx_freq "

Params: (channel)"

%feature("docstring") usrp_sink_base::nunderruns "

Params: (NONE)"

%feature("docstring") usrp_sink_base::has_rx_halfband "

Params: (NONE)"

%feature("docstring") usrp_sink_base::has_tx_halfband "

Params: (NONE)"

%feature("docstring") usrp_sink_base::nddcs "

Params: (NONE)"

%feature("docstring") usrp_sink_base::nducs "

Params: (NONE)"

%feature("docstring") usrp_sink_base::start "Called to enable drivers, etc for i/o devices.

This allows a block to enable an associated driver to begin transfering data just before we start to execute the scheduler. The end result is that this reduces latency in the pipeline when dealing with audio devices, usrps, etc.

Params: (NONE)"

%feature("docstring") usrp_sink_base::stop "Called to disable drivers, etc for i/o devices.

Params: (NONE)"

%feature("docstring") usrp_sink_base::tune "High-level \"tune\" method. Works for the single channel case.

This method adjusts both the daughterboard LO and the DUC so that DC in the complex baseband samples ends up at RF target_freq.

Params: (chan, db, target_freq, result)"

%feature("docstring") usrp_sink_base::pick_tx_subdevice "Select suitable Tx daughterboard.

Params: (NONE)"

%feature("docstring") usrp_source_base "abstract interface to Universal Software Radio Peripheral Rx path (Rev 1)"

%feature("docstring") usrp_source_base::usrp_source_base "

Params: (name, output_signature, which_board, decim_rate, nchan, mux, mode, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_source_base::ninput_bytes_reqd_for_noutput_items "return number of usrp input bytes required to produce noutput items.

Params: (noutput_items)"

%feature("docstring") usrp_source_base::sizeof_basic_sample "number of bytes in a low-level sample

Params: (NONE)"

%feature("docstring") usrp_source_base::copy_from_usrp_buffer "convert between native usrp format and output item format

The copy must consume all bytes available. That is,  must equal .

Params: (output_items, output_index, output_items_available, output_items_produced, usrp_buffer, usrp_buffer_length, bytes_read)"

%feature("docstring") usrp_source_base::~usrp_source_base "

Params: (NONE)"

%feature("docstring") usrp_source_base::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") usrp_source_base::set_decim_rate "Set decimator rate.  must be EVEN and in [8, 256].

The final complex sample rate across the USB is adc_freq () / decim_rate ()

Params: (rate)"

%feature("docstring") usrp_source_base::set_nchannels "

Params: (nchan)"

%feature("docstring") usrp_source_base::set_mux "

Params: (mux)"

%feature("docstring") usrp_source_base::determine_rx_mux_value "

Params: (ss)"

%feature("docstring") usrp_source_base::set_rx_freq "set the center frequency of the digital down converter.

must be 0.  is the center frequency in Hz. It must be in the range [-FIXME, FIXME]. The frequency specified is quantized. Use rx_freq to retrieve the actual value used.

Params: (channel, freq)"

%feature("docstring") usrp_source_base::set_fpga_mode "set fpga special modes

Params: (mode)"

%feature("docstring") usrp_source_base::set_ddc_phase "Set the digital down converter phase register.

Params: (channel, phase)"

%feature("docstring") usrp_source_base::adc_rate "

Params: (NONE)"

%feature("docstring") usrp_source_base::adc_freq "

Params: (NONE)"

%feature("docstring") usrp_source_base::decim_rate "

Params: (NONE)"

%feature("docstring") usrp_source_base::nchannels "

Params: (NONE)"

%feature("docstring") usrp_source_base::mux "

Params: (NONE)"

%feature("docstring") usrp_source_base::rx_freq "

Params: (channel)"

%feature("docstring") usrp_source_base::noverruns "

Params: (NONE)"

%feature("docstring") usrp_source_base::has_rx_halfband "

Params: (NONE)"

%feature("docstring") usrp_source_base::has_tx_halfband "

Params: (NONE)"

%feature("docstring") usrp_source_base::nddcs "

Params: (NONE)"

%feature("docstring") usrp_source_base::nducs "

Params: (NONE)"

%feature("docstring") usrp_source_base::start "Called to enable drivers, etc for i/o devices.

This allows a block to enable an associated driver to begin transfering data just before we start to execute the scheduler. The end result is that this reduces latency in the pipeline when dealing with audio devices, usrps, etc.

Params: (NONE)"

%feature("docstring") usrp_source_base::stop "Called to disable drivers, etc for i/o devices.

Params: (NONE)"

%feature("docstring") usrp_source_base::set_format "Specify Rx data format.

Rx data format control register

3 2 1 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 +-----------------------------------------+-+-+---------+-------+ | Reserved (Must be zero) |B|Q| WIDTH | SHIFT | +-----------------------------------------+-+-+---------+-------+

SHIFT specifies arithmetic right shift [0, 15] WIDTH specifies bit-width of I & Q samples across the USB [1, 16] (not all valid) Q if set deliver both I & Q, else just I B if set bypass half-band filter.

Right now the acceptable values are:

B Q WIDTH SHIFT 0 1 16 0 0 1 8 8

More valid combos to come.

Default value is 0x00000300 16-bits, 0 shift, deliver both I & Q.

Params: (format)"

%feature("docstring") usrp_source_base::format "return current format

Params: (NONE)"

%feature("docstring") usrp_source_base::tune "High-level \"tune\" method. Works for the single channel case.

This method adjusts both the daughterboard LO and the DDC so that target_freq ends up at DC in the complex baseband samples.

Params: (chan, db, target_freq, result)"

%feature("docstring") usrp_source_base::pick_rx_subdevice "Select suitable Rx daughterboard.

Params: (NONE)"

%feature("docstring") usrp_source_base::make_format "

Params: (width, shift, want_q, bypass_halfband)"

%feature("docstring") usrp_source_base::format_width "

Params: (format)"

%feature("docstring") usrp_source_base::format_shift "

Params: (format)"

%feature("docstring") usrp_source_base::format_want_q "

Params: (format)"

%feature("docstring") usrp_source_base::format_bypass_halfband "

Params: (format)"



%feature("docstring") usrp_standard_common::usrp_standard_common "

Params: (parent)"

%feature("docstring") usrp_standard_common::has_rx_halfband "does the FPGA implement the final Rx half-band filter? If it doesn't, the maximum decimation factor with proper gain is 1/2 of what it would otherwise be.

Params: (NONE)"

%feature("docstring") usrp_standard_common::nddcs "number of digital downconverters implemented in the FPGA This will be 0, 1, 2 or 4.

Params: (NONE)"

%feature("docstring") usrp_standard_common::has_tx_halfband "does the FPGA implement the initial Tx half-band filter?

Params: (NONE)"

%feature("docstring") usrp_standard_common::nducs "number of digital upconverters implemented in the FPGA This will be 0, 1, or 2.

Params: (NONE)"

%feature("docstring") usrp_standard_common::calc_dxc_freq "Calculate the frequency to use for setting the digital up or down converter.

Params: (target_freq, baseband_freq, fs, dxc_freq, inverted)"

%feature("docstring") usrp_standard_rx "The C++ interface the receive side of the USRP

This is the recommended interface to USRP receive functionality for applications that use the USRP but not GNU Radio."

%feature("docstring") usrp_standard_rx::usrp_standard_rx "

Params: (which_board, decim_rate, nchan, mux, mode, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_standard_rx::write_hw_mux_reg "

Params: (NONE)"

%feature("docstring") usrp_standard_rx::~usrp_standard_rx "

Params: (NONE)"

%feature("docstring") usrp_standard_rx::set_decim_rate "Set decimator rate.  MUST BE EVEN and in [8, 256].

The final complex sample rate across the USB is adc_freq () / decim_rate () * nchannels ()

Params: (rate)"

%feature("docstring") usrp_standard_rx::set_nchannels "Set number of active channels.  must be 1, 2 or 4.

The final complex sample rate across the USB is adc_freq () / decim_rate () * nchannels ()

Params: (nchannels)"

%feature("docstring") usrp_standard_rx::set_mux "Set input mux configuration.

This determines which ADC (or constant zero) is connected to each DDC input. There are 4 DDCs. Each has two inputs.

Params: (mux)"

%feature("docstring") usrp_standard_rx::determine_rx_mux_value "Determine the appropriate Rx mux value as a function of the subdevice choosen and the characteristics of the respective daughterboard.

Params: (ss)"

%feature("docstring") usrp_standard_rx::set_rx_freq "set the frequency of the digital down converter.

must be in the range [0,3].  is the center frequency in Hz.  may be either negative or postive. The frequency specified is quantized. Use rx_freq to retrieve the actual value used.

Params: (channel, freq)"

%feature("docstring") usrp_standard_rx::set_fpga_mode "set fpga mode

Params: (mode)"

%feature("docstring") usrp_standard_rx::set_ddc_phase "Set the digital down converter phase register.

Params: (channel, phase)"

%feature("docstring") usrp_standard_rx::set_format "Specify Rx data format.

Rx data format control register

3 2 1 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 +-----------------------------------------+-+-+---------+-------+ | Reserved (Must be zero) |B|Q| WIDTH | SHIFT | +-----------------------------------------+-+-+---------+-------+

SHIFT specifies arithmetic right shift [0, 15] WIDTH specifies bit-width of I & Q samples across the USB [1, 16] (not all valid) Q if set deliver both I & Q, else just I B if set bypass half-band filter.

Right now the acceptable values are:

B Q WIDTH SHIFT 0 1 16 0 0 1 8 8

More valid combos to come.

Default value is 0x00000300 16-bits, 0 shift, deliver both I & Q.

Params: (format)"

%feature("docstring") usrp_standard_rx::tune "High-level \"tune\" method. Works for the single channel case.

This method adjusts both the daughterboard LO and the DDC so that target_freq ends up at DC in the complex baseband samples.

Params: (chan, db, target_freq, result)"

%feature("docstring") usrp_standard_rx::decim_rate "

Params: (NONE)"

%feature("docstring") usrp_standard_rx::rx_freq "

Params: (channel)"

%feature("docstring") usrp_standard_rx::nchannels "

Params: (NONE)"

%feature("docstring") usrp_standard_rx::mux "

Params: (NONE)"

%feature("docstring") usrp_standard_rx::format "

Params: (NONE)"

%feature("docstring") usrp_standard_rx::start "Start data transfers. Called in base class to derived class order.

Params: (NONE)"

%feature("docstring") usrp_standard_rx::stop "Stop data transfers. Called in base class to derived class order.

Params: (NONE)"

%feature("docstring") usrp_standard_rx::make "invokes constructor, returns shared_ptr or shared_ptr equivalent of 0 if trouble

Params: (which_board, decim_rate, nchan, mux, mode, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_standard_rx::make_format "

Params: (width, shift, want_q, bypass_halfband)"

%feature("docstring") usrp_standard_rx::format_width "

Params: (format)"

%feature("docstring") usrp_standard_rx::format_shift "

Params: (format)"

%feature("docstring") usrp_standard_rx::format_want_q "

Params: (format)"

%feature("docstring") usrp_standard_rx::format_bypass_halfband "

Params: (format)"

%feature("docstring") usrp_standard_tx "The C++ interface the transmit side of the USRP

This is the recommended interface to USRP transmit functionality for applications that use the USRP but not GNU Radio.

Uses digital upconverter (coarse & fine modulators) in AD9862..."

%feature("docstring") usrp_standard_tx::set_coarse_modulator "

Params: (channel, cm)"

%feature("docstring") usrp_standard_tx::coarse_modulator "

Params: (channel)"

%feature("docstring") usrp_standard_tx::usrp_standard_tx "

Params: (which_board, interp_rate, nchan, mux, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"

%feature("docstring") usrp_standard_tx::write_hw_mux_reg "

Params: (NONE)"

%feature("docstring") usrp_standard_tx::~usrp_standard_tx "

Params: (NONE)"

%feature("docstring") usrp_standard_tx::set_interp_rate "Set interpolator rate.  must be in [4, 512] and a multiple of 4.

The final complex sample rate across the USB is dac_freq () / interp_rate () * nchannels ()

Params: (rate)"

%feature("docstring") usrp_standard_tx::set_nchannels "Set number of active channels.  must be 1 or 2.

The final complex sample rate across the USB is dac_freq () / decim_rate () * nchannels ()

Params: (nchannels)"

%feature("docstring") usrp_standard_tx::set_mux "Set output mux configuration.

Params: (mux)"

%feature("docstring") usrp_standard_tx::determine_tx_mux_value "Determine the appropriate Tx mux value as a function of the subdevice choosen and the characteristics of the respective daughterboard.

Params: (ss)"

%feature("docstring") usrp_standard_tx::set_tx_freq "set the frequency of the digital up converter.

must be in the range [0,1].  is the center frequency in Hz. It must be in the range [-44M, 44M]. The frequency specified is quantized. Use tx_freq to retrieve the actual value used.

Params: (channel, freq)"

%feature("docstring") usrp_standard_tx::interp_rate "

Params: (NONE)"

%feature("docstring") usrp_standard_tx::tx_freq "

Params: (channel)"

%feature("docstring") usrp_standard_tx::nchannels "

Params: (NONE)"

%feature("docstring") usrp_standard_tx::mux "

Params: (NONE)"

%feature("docstring") usrp_standard_tx::tune "High-level \"tune\" method. Works for the single channel case.

This method adjusts both the daughterboard LO and the DUC so that DC in the complex baseband samples ends up at RF target_freq.

Params: (chan, db, target_freq, result)"

%feature("docstring") usrp_standard_tx::start "Start data transfers. Called in base class to derived class order.

Params: (NONE)"

%feature("docstring") usrp_standard_tx::stop "Stop data transfers. Called in base class to derived class order.

Params: (NONE)"

%feature("docstring") usrp_standard_tx::make "invokes constructor, returns shared_ptr or shared_ptr equivalent of 0 if trouble

Params: (which_board, interp_rate, nchan, mux, fusb_block_size, fusb_nblocks, fpga_filename, firmware_filename)"



%feature("docstring") usrp_tune_result::usrp_tune_result "

Params: (baseband, dxc, residual, _inverted)"

%feature("docstring") std::valarray "STL class."

%feature("docstring") std::vector "vector documentation stub

STL class."

%feature("docstring") video_sdl_sink_s "video sink using SDL

input signature is one, two or three streams of signed short. One stream: stream is grey (Y) two streems: first is grey (Y), second is alternating U and V Three streams: first is grey (Y), second is U, third is V Input samples must be in the range [0,255]."

%feature("docstring") video_sdl_sink_s::video_sdl_sink_s "

Params: (framerate, width, height, format, dst_width, dst_height)"

%feature("docstring") video_sdl_sink_s::copy_line_pixel_interleaved "

Params: (dst_pixels_u, dst_pixels_v, src_pixels, src_width)"

%feature("docstring") video_sdl_sink_s::copy_line_line_interleaved "

Params: (dst_pixels_u, dst_pixels_v, src_pixels, src_width)"

%feature("docstring") video_sdl_sink_s::copy_line_single_plane "

Params: (dst_pixels, src_pixels, src_width)"

%feature("docstring") video_sdl_sink_s::copy_line_single_plane_dec2 "

Params: (dst_pixels, src_pixels, src_width)"

%feature("docstring") video_sdl_sink_s::copy_plane_to_surface "

Params: (plane, noutput_items, src_pixels)"

%feature("docstring") video_sdl_sink_s::~video_sdl_sink_s "

Params: (NONE)"

%feature("docstring") video_sdl_sink_s::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"

%feature("docstring") video_sdl_sink_uc "video sink using SDL

input signature is one, two or three streams of uchar. One stream: stream is grey (Y) two streems: first is grey (Y), second is alternating U and V Three streams: first is grey (Y), second is U, third is V Input samples must be in the range [0,255]."

%feature("docstring") video_sdl_sink_uc::video_sdl_sink_uc "

Params: (framerate, width, height, format, dst_width, dst_height)"

%feature("docstring") video_sdl_sink_uc::copy_line_pixel_interleaved "

Params: (dst_pixels_u, dst_pixels_v, src_pixels, src_width)"

%feature("docstring") video_sdl_sink_uc::copy_line_line_interleaved "

Params: (dst_pixels_u, dst_pixels_v, src_pixels, src_width)"

%feature("docstring") video_sdl_sink_uc::copy_line_single_plane "

Params: (dst_pixels, src_pixels, src_width)"

%feature("docstring") video_sdl_sink_uc::copy_line_single_plane_dec2 "

Params: (dst_pixels, src_pixels, src_width)"

%feature("docstring") video_sdl_sink_uc::copy_plane_to_surface "

Params: (plane, noutput_items, src_pixels)"

%feature("docstring") video_sdl_sink_uc::~video_sdl_sink_uc "

Params: (NONE)"

%feature("docstring") video_sdl_sink_uc::work "just like gr_block::general_work, only this arranges to call consume_each for you

The user must override work to define the signal processing code

Params: (noutput_items, input_items, output_items)"



%feature("docstring") wbx_base::shutdown "Called to shutdown daughterboard. Called from dtor and usrp_basic dtor.

N.B., any class that overrides shutdown MUST call shutdown in its destructor.

Params: (NONE)"

%feature("docstring") wbx_base::_lock_detect "

Params: (NONE)"

%feature("docstring") wbx_base::_tx_write_oe "

Params: (value, mask)"

%feature("docstring") wbx_base::_rx_write_oe "

Params: (value, mask)"

%feature("docstring") wbx_base::_tx_write_io "

Params: (value, mask)"

%feature("docstring") wbx_base::_rx_write_io "

Params: (value, mask)"

%feature("docstring") wbx_base::_rx_read_io "

Params: (NONE)"

%feature("docstring") wbx_base::_tx_read_io "

Params: (NONE)"

%feature("docstring") wbx_base::_compute_regs "

Params: (freq)"

%feature("docstring") wbx_base::_refclk_freq "

Params: (NONE)"

%feature("docstring") wbx_base::_refclk_divisor "

Params: (NONE)"

%feature("docstring") wbx_base::_set_pga "

Params: (pga_gain)"

%feature("docstring") wbx_base::wbx_base "

Params: (usrp, which)"

%feature("docstring") wbx_base::~wbx_base "

Params: (NONE)"

%feature("docstring") wbx_base::set_freq "

Params: (freq)"

%feature("docstring") wbx_base::gain_min "

Params: (NONE)"

%feature("docstring") wbx_base::gain_max "

Params: (NONE)"

%feature("docstring") wbx_base::gain_db_per_step "

Params: (NONE)"

%feature("docstring") wbx_base::set_gain "

Params: (gain)"

%feature("docstring") wbx_base::is_quadrature "

Params: (NONE)"



%feature("docstring") wbx_base_rx::wbx_base_rx "

Params: (usrp, which)"

%feature("docstring") wbx_base_rx::~wbx_base_rx "

Params: (NONE)"

%feature("docstring") wbx_base_rx::set_auto_tr "

Params: (on)"

%feature("docstring") wbx_base_rx::select_rx_antenna "

Params: (which_antenna)"

%feature("docstring") wbx_base_rx::set_gain "

Params: (gain)"

%feature("docstring") wbx_base_rx::i_and_q_swapped "

Params: (NONE)"



%feature("docstring") wbx_base_tx::wbx_base_tx "

Params: (usrp, which)"

%feature("docstring") wbx_base_tx::~wbx_base_tx "

Params: (NONE)"

%feature("docstring") wbx_base_tx::set_auto_tr "

Params: (on)"

%feature("docstring") wbx_base_tx::set_enable "

Params: (on)"



%feature("docstring") wbxng_base::wbxng_base "

Params: (usrp, which)"

%feature("docstring") wbxng_base::~wbxng_base "

Params: (NONE)"

%feature("docstring") wbxng_base::set_freq "

Params: (freq)"

%feature("docstring") wbxng_base::is_quadrature "

Params: (NONE)"

%feature("docstring") wbxng_base::freq_min "

Params: (NONE)"

%feature("docstring") wbxng_base::freq_max "

Params: (NONE)"

%feature("docstring") wbxng_base::_write_spi "

Params: (data)"

%feature("docstring") wbxng_base::_refclk_divisor "

Params: (NONE)"

%feature("docstring") wbxng_base::_get_locked "

Params: (NONE)"

%feature("docstring") wbxng_base::_set_pga "

Params: (pga_gain)"

%feature("docstring") std::wfstream "STL class."

%feature("docstring") std::wifstream "STL class."

%feature("docstring") std::wios "STL class."

%feature("docstring") std::wistream "STL class."

%feature("docstring") std::wistringstream "STL class."

%feature("docstring") std::wofstream "STL class."

%feature("docstring") std::wostream "STL class."

%feature("docstring") std::wostringstream "STL class."

%feature("docstring") std::wstring "STL class."

%feature("docstring") std::wstringstream "STL class."
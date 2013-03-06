gnuradio.gr
===========

.. automodule:: gnuradio.gr

Signal Processing Blocks
------------------------

Top Block and Hierarchical Block Base Classes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:
   
   gnuradio.gr.top_block
   gnuradio.gr.hier_block2

Signal Sources
^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.lfsr_32k_source_s
   gnuradio.gr.null_source
   gnuradio.gr.vector_source_b
   gnuradio.gr.vector_source_c
   gnuradio.gr.vector_source_f
   gnuradio.gr.vector_source_i
   gnuradio.gr.vector_source_s
   gnuradio.gr.file_descriptor_source
   gnuradio.gr.file_source
   gnuradio.gr.udp_source
   gnuradio.gr.wavfile_source

Signal Sinks
^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.bin_statistics_f
   gnuradio.gr.check_counting_s
   gnuradio.gr.check_lfsr_32k_s
   gnuradio.gr.null_sink
   gnuradio.gr.vector_sink_b
   gnuradio.gr.vector_sink_c
   gnuradio.gr.vector_sink_f
   gnuradio.gr.vector_sink_i
   gnuradio.gr.vector_sink_s
   gnuradio.gr.file_descriptor_sink
   gnuradio.gr.file_sink
   gnuradio.gr.histo_sink_f
   gnuradio.gr.oscope_sink_f
   gnuradio.gr.udp_sink
   gnuradio.gr.wavfile_sink


Information Coding and Decoding
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.fake_channel_encoder_pp
   gnuradio.gr.fake_channel_decoder_pp



Miscellaneous Blocks
^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.copy
   gnuradio.gr.kludge_copy
   gnuradio.gr.nop
   gnuradio.gr.pa_2x2_phase_combiner
   gnuradio.gr.channel_model

Slicing and Dicing Streams
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autosummary::
   :nosignatures:

   gnuradio.gr.head
   gnuradio.gr.skiphead

Digital Filter Design
---------------------

.. autosummary::
   :nosignatures:

   gnuradio.gr.firdes
   gnuradio.gr.remez

Miscellaneous
-------------

.. autosummary::
   :nosignatures:

   gnuradio.gr.feval_dd
   gnuradio.gr.feval_cc
   gnuradio.gr.feval_ll
   gnuradio.gr.feval
   gnuradio.gr.prefs
   gnuradio.gr.test
   gnuradio.gr.message
   gnuradio.gr.msg_queue
   gnuradio.gr.enable_realtime_scheduling

Implementation Details
----------------------

.. autosummary::
   :nosignatures:

   gnuradio.gr.block_detail
   gnuradio.gr.buffer
   gnuradio.gr.dispatcher
   gnuradio.gr.single_threaded_scheduler


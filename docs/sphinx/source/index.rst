gnuradio
========

.. automodule:: gnuradio

Core Framework
--------------

.. autosummary::
   :nosignatures:

   gnuradio.gr
   gnuradio.digital
   gnuradio.audio
   gnuradio.trellis
   gnuradio.wavelet
   gnuradio.fft
   gnuradio.gr_unittest
   gnuradio.qtgui
   gnuradio.wxgui
   gnuradio.window

.. toctree::
   :hidden:   

   gnuradio.gr <gr/index>
   gnuradio.digital <digital/index>
   gnuradio.audio <audio/index>
   gnuradio.gr_unittest <gr_unittest/index>
   gnuradio.trellis <trellis/index>
   gnuradio.wavelet <wavelet>
   gnuradio.fft <fft>
   gnuradio.qtgui <qtgui/index>
   gnuradio.wxgui <wxgui/index>
   gnuradio.window <window/index>

Hardware Interfaces
-------------------

.. autosummary::
   :nosignatures:
   
   gnuradio.uhd

.. toctree::
   :hidden:   

   gnuradio.uhd <uhd>

Utilities
---------

.. autosummary::
   :nosignatures:

   gnuradio.plot_data
   gnuradio.eng_notation
   gnuradio.eng_option

.. toctree::
   :hidden:   

   gnuradio.plot_data <plot_data>
   gnuradio.eng_notation <eng_notation/index>
   gnuradio.eng_option <eng_option/index>

Framework Extensions
--------------------

.. autosummary::
   :nosignatures:

   gnuradio.atsc
   gnuradio.noaa
   gnuradio.pager
   gnuradio.video_sdl
   gnuradio.vocoder

.. toctree::
   :hidden:   

   gnuradio.atsc <atsc/index>
   gnuradio.noaa <noaa>
   gnuradio.pager <pager/index>
   gnuradio.video_sdl <video_sdl>
   gnuradio.vocoder <vocoder/index>


.. Use this to add to the toctree but not displayed
.. It's mostly to get rid of warnings
 
.. toctree::
   :hidden:   
   
   internal <gr/internal>
   top_block <gr/top_block>
   misc <gr/misc>
   atsc_blks <atsc/blks>
   digital_blocks <digital/blocks>
   constellations <digital/constellations>
   ofdm <digital/ofdm>
   pkt_utils <digital/pkt_utils>
   digital_utilities <digital/utilities>
   pager_blks <pager/blks>
   pyqt_filter <pyqt_filter>
   pyqt_plot <pyqt_plot>
   trellis_blks <trellis/blks>
   trellis_objs <trellis/objs>
   vocoder_blks <vocoder/blks>
   window_detail <window/detail>
   wxgui_blks <wxgui/blks>

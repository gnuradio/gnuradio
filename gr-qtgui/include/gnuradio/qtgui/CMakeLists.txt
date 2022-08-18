# Copyright 2010-2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install the header files
########################################################################
install(
    FILES api.h
          ber_sink_b.h
          constellationdisplayform.h
          ConstellationDisplayPlot.h
          const_sink_c.h
          displayform.h
          eyedisplaysform.h
          DisplayPlot.h
          timecontrolpanel.h
          eyecontrolpanel.h
          eyedisplayform.h
          EyeDisplayPlot.h
          eye_sink_c.h
          eye_sink_f.h
          form_menus.h
          eye_sink_c.h
          eye_sink_f.h
          eyecontrolpanel.h
          EyeDisplayPlot.h
          freqdisplayform.h
          freqcontrolpanel.h
          freq_sink_c.h
          freq_sink_f.h
          FrequencyDisplayPlot.h
          histogram_sink_f.h
          histogramdisplayform.h
          HistogramDisplayPlot.h
          edit_box_msg.h
          number_sink.h
          numberdisplayform.h
          plot_raster.h
          plot_waterfall.h
          qtgui_types.h
          sink_c.h
          sink_f.h
          spectrumUpdateEvents.h
          timedisplayform.h
          timecontrolpanel.h
          TimeDomainDisplayPlot.h
          timerasterdisplayform.h
          TimeRasterDisplayPlot.h
          timeRasterGlobalData.h
          time_raster_sink_b.h
          time_raster_sink_f.h
          time_sink_c.h
          time_sink_f.h
          trigger_mode.h
          utils.h
          waterfalldisplayform.h
          WaterfallDisplayPlot.h
          waterfallGlobalData.h
          waterfall_sink_c.h
          waterfall_sink_f.h
          vectordisplayform.h
          VectorDisplayPlot.h
          vector_sink_f.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/qtgui)

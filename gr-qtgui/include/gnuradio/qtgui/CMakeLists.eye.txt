# Copyright 2010-2012 Free Software Foundation, Inc.
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

########################################################################
# Install the header files
########################################################################
install(FILES
  api.h
  ber_sink_b.h
  CMakeLists.txt
  const_sink_c.h
  constellationdisplayform.h
  ConstellationDisplayPlot.h
  displayform.h
  DisplayPlot.h
  displaysform.h
  edit_box_msg.h
  form_menus.h
  eye_sink_c.h
  eye_sink_f.h
  freq_sink_c.h
  freq_sink_f.h
  freqdisplayform.h
  freqcontrolpanel.h
  FrequencyDisplayPlot.h
  histogram_sink_f.h
  histogramdisplayform.h
  HistogramDisplayPlot.h
  number_sink.h
  numberdisplayform.h
  plot_raster.h
  plot_waterfall.h
  qtgui_types.h
  sink_c.h
  sink_f.h
  spectrumUpdateEvents.h
  time_raster_sink_b.h
  time_raster_sink_f.h
  time_sink_c.h
  time_sink_f.h
  timecontrolpanel.h
  eyecontrolpanel.h
  eyedisplayform.h
  EyeDisplayPlot.h
  timedisplayform.h
  TimeDomainDisplayPlot.h
  timerasterdisplayform.h
  TimeRasterDisplayPlot.h
  timeRasterGlobalData.h
  trigger_mode.h
  utils.h
  vector_sink_f.h
  vectordisplayform.h
  VectorDisplayPlot.h
  waterfall_sink_c.h
  waterfall_sink_f.h
  waterfalldisplayform.h
  WaterfallDisplayPlot.h
  waterfallGlobalData.h
  DESTINATION ${GR_INCLUDE_DIR}/gnuradio/qtgui
)

# NOTE: The following are private, not installed, and their symbols
# are unavailable for external API use.

# spectrumdisplayform.h
# SpectrumGUIClass.h

#!/usr/bin/env python

import wx

def slider(parent, min, max, callback):
    """
    Return a wx.Slider object.

    @param min: minimum slider value
    @type min: float
    @param max: maximum slider value
    @type max: float
    @param callback: function of one arg invoked when slider moves.
    @rtype: wx.Slider
    """
    new_id = wx.NewId()
    s = wx.Slider(parent, new_id, (max+min)/2, min, max, wx.DefaultPosition,
                  wx.Size(250,-1), wx.SL_HORIZONTAL | wx.SL_LABELS)
    wx.EVT_COMMAND_SCROLL(parent, new_id,
                          lambda evt : callback(evt.GetInt()))
    return s


# ----------------------------------------------------------------
# 		             Demo app
# ----------------------------------------------------------------
if __name__ == '__main__':

    from gnuradio.wxgui import stdgui

    class demo_graph(stdgui.gui_flow_graph):

        def __init__(self, frame, panel, vbox, argv):
            stdgui.gui_flow_graph.__init__ (self, frame, panel, vbox, argv)

            vbox.Add(slider(panel, 23, 47, self.my_callback1), 1, wx.ALIGN_CENTER)
            vbox.Add(slider(panel, -100, 100, self.my_callback2), 1, wx.ALIGN_CENTER)

        def my_callback1(self, val):
            print "cb1 = ", val

        def my_callback2(self, val):
            print "cb2 = ", val

    def main ():
        app = stdgui.stdapp (demo_graph, "Slider Demo")
        app.MainLoop ()

    main ()

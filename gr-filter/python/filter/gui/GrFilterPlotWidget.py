from pyqtgraph import PlotWidget, graphicsItems
from .CustomViewBox import CustomViewBox


class GrFilterPlotWidget(PlotWidget):
    def __init__(self, parent=None, background='default', **kargs):
        PlotWidget.__init__(self, parent, background,
                            enableMenu=False, viewBox=CustomViewBox())

    def drop_plotdata(self):
        for plitem in self.items():
            if isinstance(plitem, (graphicsItems.PlotCurveItem.PlotCurveItem, graphicsItems.ScatterPlotItem.ScatterPlotItem, graphicsItems.PlotDataItem.PlotDataItem)):
                plitem.clear()

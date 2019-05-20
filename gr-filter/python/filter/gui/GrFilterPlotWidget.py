from pyqtgraph import PlotWidget
from .CustomViewBox import CustomViewBox


class GrFilterPlotWidget(PlotWidget):
    def __init__(self,parent=None, background='default', **kargs):
        PlotWidget.__init__(self,parent,background,enableMenu=False,viewBox=CustomViewBox())


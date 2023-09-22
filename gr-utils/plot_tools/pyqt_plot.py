# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pyqt_plot.ui'
#
# Created by: PyQt4 UI code generator 4.12.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import Qwt
from PyQt5 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8

    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(927, 718)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.gridLayout = QtGui.QGridLayout(self.centralwidget)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.plotHBar = QtGui.QScrollBar(self.centralwidget)
        self.plotHBar.setOrientation(QtCore.Qt.Horizontal)
        self.plotHBar.setObjectName(_fromUtf8("plotHBar"))
        self.gridLayout.addWidget(self.plotHBar, 1, 0, 1, 3)
        self.filePosBox = QtGui.QGroupBox(self.centralwidget)
        self.filePosBox.setMinimumSize(QtCore.QSize(0, 120))
        self.filePosBox.setObjectName(_fromUtf8("filePosBox"))
        self.gridLayout_4 = QtGui.QGridLayout(self.filePosBox)
        self.gridLayout_4.setObjectName(_fromUtf8("gridLayout_4"))
        self.filePosLayout = QtGui.QFormLayout()
        self.filePosLayout.setObjectName(_fromUtf8("filePosLayout"))
        self.filePosStartLineEdit = QtGui.QLineEdit(self.filePosBox)
        self.filePosStartLineEdit.setMinimumSize(QtCore.QSize(50, 0))
        self.filePosStartLineEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.filePosStartLineEdit.setObjectName(
            _fromUtf8("filePosStartLineEdit"))
        self.filePosLayout.setWidget(
            0, QtGui.QFormLayout.FieldRole, self.filePosStartLineEdit)
        self.filePosStopLabel = QtGui.QLabel(self.filePosBox)
        self.filePosStopLabel.setObjectName(_fromUtf8("filePosStopLabel"))
        self.filePosLayout.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.filePosStopLabel)
        self.filePosStopLineEdit = QtGui.QLineEdit(self.filePosBox)
        self.filePosStopLineEdit.setMinimumSize(QtCore.QSize(50, 0))
        self.filePosStopLineEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.filePosStopLineEdit.setObjectName(
            _fromUtf8("filePosStopLineEdit"))
        self.filePosLayout.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.filePosStopLineEdit)
        self.filePosLengthLabel = QtGui.QLabel(self.filePosBox)
        self.filePosLengthLabel.setObjectName(_fromUtf8("filePosLengthLabel"))
        self.filePosLayout.setWidget(
            2, QtGui.QFormLayout.LabelRole, self.filePosLengthLabel)
        self.filePosLengthLineEdit = QtGui.QLineEdit(self.filePosBox)
        self.filePosLengthLineEdit.setMinimumSize(QtCore.QSize(50, 0))
        self.filePosLengthLineEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.filePosLengthLineEdit.setObjectName(
            _fromUtf8("filePosLengthLineEdit"))
        self.filePosLayout.setWidget(
            2, QtGui.QFormLayout.FieldRole, self.filePosLengthLineEdit)
        self.filePosStartLabel = QtGui.QLabel(self.filePosBox)
        self.filePosStartLabel.setObjectName(_fromUtf8("filePosStartLabel"))
        self.filePosLayout.setWidget(
            0, QtGui.QFormLayout.LabelRole, self.filePosStartLabel)
        self.gridLayout_4.addLayout(self.filePosLayout, 0, 0, 1, 1)
        self.fileTimeLayout = QtGui.QFormLayout()
        self.fileTimeLayout.setObjectName(_fromUtf8("fileTimeLayout"))
        self.fileTimeStartLabel = QtGui.QLabel(self.filePosBox)
        self.fileTimeStartLabel.setObjectName(_fromUtf8("fileTimeStartLabel"))
        self.fileTimeLayout.setWidget(
            0, QtGui.QFormLayout.LabelRole, self.fileTimeStartLabel)
        self.fileTimeStartLineEdit = QtGui.QLineEdit(self.filePosBox)
        self.fileTimeStartLineEdit.setMinimumSize(QtCore.QSize(50, 0))
        self.fileTimeStartLineEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.fileTimeStartLineEdit.setObjectName(
            _fromUtf8("fileTimeStartLineEdit"))
        self.fileTimeLayout.setWidget(
            0, QtGui.QFormLayout.FieldRole, self.fileTimeStartLineEdit)
        self.fileTimeStopLabel = QtGui.QLabel(self.filePosBox)
        self.fileTimeStopLabel.setObjectName(_fromUtf8("fileTimeStopLabel"))
        self.fileTimeLayout.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.fileTimeStopLabel)
        self.fileTimeStopLineEdit = QtGui.QLineEdit(self.filePosBox)
        self.fileTimeStopLineEdit.setMinimumSize(QtCore.QSize(50, 0))
        self.fileTimeStopLineEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.fileTimeStopLineEdit.setObjectName(
            _fromUtf8("fileTimeStopLineEdit"))
        self.fileTimeLayout.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.fileTimeStopLineEdit)
        self.fileTimeLengthLabel = QtGui.QLabel(self.filePosBox)
        self.fileTimeLengthLabel.setObjectName(
            _fromUtf8("fileTimeLengthLabel"))
        self.fileTimeLayout.setWidget(
            2, QtGui.QFormLayout.LabelRole, self.fileTimeLengthLabel)
        self.fileTimeLengthLineEdit = QtGui.QLineEdit(self.filePosBox)
        self.fileTimeLengthLineEdit.setMinimumSize(QtCore.QSize(50, 0))
        self.fileTimeLengthLineEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.fileTimeLengthLineEdit.setObjectName(
            _fromUtf8("fileTimeLengthLineEdit"))
        self.fileTimeLayout.setWidget(
            2, QtGui.QFormLayout.FieldRole, self.fileTimeLengthLineEdit)
        self.gridLayout_4.addLayout(self.fileTimeLayout, 0, 1, 1, 1)
        self.gridLayout.addWidget(self.filePosBox, 2, 0, 1, 1)
        self.displayGroupBox = QtGui.QGroupBox(self.centralwidget)
        self.displayGroupBox.setMinimumSize(QtCore.QSize(170, 0))
        self.displayGroupBox.setObjectName(_fromUtf8("displayGroupBox"))
        self.gridLayout_2 = QtGui.QGridLayout(self.displayGroupBox)
        self.gridLayout_2.setObjectName(_fromUtf8("gridLayout_2"))
        self.colorComboBox = QtGui.QComboBox(self.displayGroupBox)
        self.colorComboBox.setObjectName(_fromUtf8("colorComboBox"))
        self.gridLayout_2.addWidget(self.colorComboBox, 0, 0, 1, 2)
        self.lineWidthSpinBox = QtGui.QSpinBox(self.displayGroupBox)
        self.lineWidthSpinBox.setMinimumSize(QtCore.QSize(100, 0))
        self.lineWidthSpinBox.setMaximumSize(QtCore.QSize(100, 16777215))
        self.lineWidthSpinBox.setObjectName(_fromUtf8("lineWidthSpinBox"))
        self.gridLayout_2.addWidget(self.lineWidthSpinBox, 1, 1, 1, 1)
        self.lineWidthLabel = QtGui.QLabel(self.displayGroupBox)
        self.lineWidthLabel.setObjectName(_fromUtf8("lineWidthLabel"))
        self.gridLayout_2.addWidget(self.lineWidthLabel, 1, 0, 1, 1)
        self.lineStyleLabel = QtGui.QLabel(self.displayGroupBox)
        self.lineStyleLabel.setObjectName(_fromUtf8("lineStyleLabel"))
        self.gridLayout_2.addWidget(self.lineStyleLabel, 2, 0, 1, 1)
        self.lineStyleComboBox = QtGui.QComboBox(self.displayGroupBox)
        self.lineStyleComboBox.setMinimumSize(QtCore.QSize(100, 0))
        self.lineStyleComboBox.setMaximumSize(QtCore.QSize(100, 16777215))
        self.lineStyleComboBox.setObjectName(_fromUtf8("lineStyleComboBox"))
        self.gridLayout_2.addWidget(self.lineStyleComboBox, 2, 1, 1, 1)
        self.styleSizeLabel = QtGui.QLabel(self.displayGroupBox)
        self.styleSizeLabel.setObjectName(_fromUtf8("styleSizeLabel"))
        self.gridLayout_2.addWidget(self.styleSizeLabel, 3, 0, 1, 1)
        self.styleSizeSpinBox = QtGui.QSpinBox(self.displayGroupBox)
        self.styleSizeSpinBox.setMinimumSize(QtCore.QSize(100, 0))
        self.styleSizeSpinBox.setMaximumSize(QtCore.QSize(100, 16777215))
        self.styleSizeSpinBox.setObjectName(_fromUtf8("styleSizeSpinBox"))
        self.gridLayout_2.addWidget(self.styleSizeSpinBox, 3, 1, 1, 1)
        self.gridLayout.addWidget(self.displayGroupBox, 2, 2, 1, 1)
        self.sysGroupBox = QtGui.QGroupBox(self.centralwidget)
        self.sysGroupBox.setMinimumSize(QtCore.QSize(200, 0))
        self.sysGroupBox.setObjectName(_fromUtf8("sysGroupBox"))
        self.formLayout = QtGui.QFormLayout(self.sysGroupBox)
        self.formLayout.setFieldGrowthPolicy(
            QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout.setObjectName(_fromUtf8("formLayout"))
        self.sampleRateLabel = QtGui.QLabel(self.sysGroupBox)
        self.sampleRateLabel.setObjectName(_fromUtf8("sampleRateLabel"))
        self.formLayout.setWidget(
            0, QtGui.QFormLayout.LabelRole, self.sampleRateLabel)
        self.sampleRateLineEdit = QtGui.QLineEdit(self.sysGroupBox)
        self.sampleRateLineEdit.setMinimumSize(QtCore.QSize(50, 0))
        self.sampleRateLineEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.sampleRateLineEdit.setObjectName(_fromUtf8("sampleRateLineEdit"))
        self.formLayout.setWidget(
            0, QtGui.QFormLayout.FieldRole, self.sampleRateLineEdit)
        self.gridLayout.addWidget(self.sysGroupBox, 2, 1, 1, 1)
        self.frame = QtGui.QFrame(self.centralwidget)
        self.frame.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtGui.QFrame.Raised)
        self.frame.setObjectName(_fromUtf8("frame"))
        self.gridLayout_3 = QtGui.QGridLayout(self.frame)
        self.gridLayout_3.setObjectName(_fromUtf8("gridLayout_3"))
        self.tabGroup = QtGui.QTabWidget(self.frame)
        self.tabGroup.setMinimumSize(QtCore.QSize(0, 0))
        self.tabGroup.setObjectName(_fromUtf8("tabGroup"))
        self.timeTab = QtGui.QWidget()
        self.timeTab.setObjectName(_fromUtf8("timeTab"))
        self.horizontalLayout = QtGui.QHBoxLayout(self.timeTab)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.timePlot = Qwt5.QwtPlot(self.timeTab)
        self.timePlot.setObjectName(_fromUtf8("timePlot"))
        self.horizontalLayout.addWidget(self.timePlot)
        self.timePlot.raise_()
        self.tabGroup.addTab(self.timeTab, _fromUtf8(""))
        self.freqTab = QtGui.QWidget()
        self.freqTab.setObjectName(_fromUtf8("freqTab"))
        self.horizontalLayout_2 = QtGui.QHBoxLayout(self.freqTab)
        self.horizontalLayout_2.setMargin(0)
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.fftPropBox = QtGui.QGroupBox(self.freqTab)
        self.fftPropBox.setMinimumSize(QtCore.QSize(160, 0))
        self.fftPropBox.setObjectName(_fromUtf8("fftPropBox"))
        self.formLayout_4 = QtGui.QFormLayout(self.fftPropBox)
        self.formLayout_4.setFieldGrowthPolicy(
            QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout_4.setObjectName(_fromUtf8("formLayout_4"))
        self.psdFFTSizeLabel = QtGui.QLabel(self.fftPropBox)
        self.psdFFTSizeLabel.setObjectName(_fromUtf8("psdFFTSizeLabel"))
        self.formLayout_4.setWidget(
            0, QtGui.QFormLayout.LabelRole, self.psdFFTSizeLabel)
        self.psdFFTComboBox = QtGui.QComboBox(self.fftPropBox)
        self.psdFFTComboBox.setMinimumSize(QtCore.QSize(96, 0))
        self.psdFFTComboBox.setMaximumSize(QtCore.QSize(96, 16777215))
        self.psdFFTComboBox.setObjectName(_fromUtf8("psdFFTComboBox"))
        self.formLayout_4.setWidget(
            0, QtGui.QFormLayout.FieldRole, self.psdFFTComboBox)
        self.psdFFTSizeLabel.raise_()
        self.psdFFTComboBox.raise_()
        self.horizontalLayout_2.addWidget(self.fftPropBox)
        self.freqPlot = Qwt5.QwtPlot(self.freqTab)
        self.freqPlot.setObjectName(_fromUtf8("freqPlot"))
        self.horizontalLayout_2.addWidget(self.freqPlot)
        self.tabGroup.addTab(self.freqTab, _fromUtf8(""))
        self.specTab = QtGui.QWidget()
        self.specTab.setObjectName(_fromUtf8("specTab"))
        self.horizontalLayout_3 = QtGui.QHBoxLayout(self.specTab)
        self.horizontalLayout_3.setMargin(0)
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.groupBox = QtGui.QGroupBox(self.specTab)
        self.groupBox.setObjectName(_fromUtf8("groupBox"))
        self.formLayout_3 = QtGui.QFormLayout(self.groupBox)
        self.formLayout_3.setObjectName(_fromUtf8("formLayout_3"))
        self.specFFTLabel = QtGui.QLabel(self.groupBox)
        self.specFFTLabel.setObjectName(_fromUtf8("specFFTLabel"))
        self.formLayout_3.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.specFFTLabel)
        self.specFFTComboBox = QtGui.QComboBox(self.groupBox)
        self.specFFTComboBox.setMinimumSize(QtCore.QSize(96, 0))
        self.specFFTComboBox.setMaximumSize(QtCore.QSize(96, 16777215))
        self.specFFTComboBox.setObjectName(_fromUtf8("specFFTComboBox"))
        self.formLayout_3.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.specFFTComboBox)
        self.horizontalLayout_3.addWidget(self.groupBox)
        self.specPlot = Qwt5.QwtPlot(self.specTab)
        self.specPlot.setObjectName(_fromUtf8("specPlot"))
        self.horizontalLayout_3.addWidget(self.specPlot)
        self.specPlot.raise_()
        self.groupBox.raise_()
        self.tabGroup.addTab(self.specTab, _fromUtf8(""))
        self.gridLayout_3.addWidget(self.tabGroup, 0, 0, 1, 1)
        self.tabGroup.raise_()
        self.gridLayout.addWidget(self.frame, 0, 0, 1, 3)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 927, 24))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        self.menu_File = QtGui.QMenu(self.menubar)
        self.menu_File.setObjectName(_fromUtf8("menu_File"))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)
        self.action_open = QtGui.QAction(MainWindow)
        self.action_open.setObjectName(_fromUtf8("action_open"))
        self.action_exit = QtGui.QAction(MainWindow)
        self.action_exit.setObjectName(_fromUtf8("action_exit"))
        self.action_reload = QtGui.QAction(MainWindow)
        self.action_reload.setObjectName(_fromUtf8("action_reload"))
        self.menu_File.addAction(self.action_open)
        self.menu_File.addAction(self.action_reload)
        self.menu_File.addAction(self.action_exit)
        self.menubar.addAction(self.menu_File.menuAction())

        self.retranslateUi(MainWindow)
        self.tabGroup.setCurrentIndex(0)
        QtCore.QObject.connect(self.action_exit, QtCore.SIGNAL(
            _fromUtf8("activated()")), MainWindow.close)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow", None))
        self.filePosBox.setTitle(_translate(
            "MainWindow", "File Position", None))
        self.filePosStopLabel.setText(_translate("MainWindow", "Stop", None))
        self.filePosLengthLabel.setText(
            _translate("MainWindow", "Length", None))
        self.filePosStartLabel.setText(_translate("MainWindow", "Start", None))
        self.fileTimeStartLabel.setText(_translate(
            "MainWindow", "time start (sec)", None))
        self.fileTimeStopLabel.setText(_translate(
            "MainWindow", "time stop (sec)", None))
        self.fileTimeLengthLabel.setText(_translate(
            "MainWindow", "time length (sec)", None))
        self.displayGroupBox.setTitle(_translate(
            "MainWindow", "Display Properties", None))
        self.lineWidthLabel.setText(
            _translate("MainWindow", "Line Width", None))
        self.lineStyleLabel.setText(
            _translate("MainWindow", "Line Style", None))
        self.styleSizeLabel.setText(
            _translate("MainWindow", "Style Size", None))
        self.sysGroupBox.setTitle(_translate(
            "MainWindow", "System Properties", None))
        self.sampleRateLabel.setText(
            _translate("MainWindow", "Sample Rate", None))
        self.tabGroup.setTabText(self.tabGroup.indexOf(
            self.timeTab), _translate("MainWindow", "Time Domain", None))
        self.fftPropBox.setTitle(_translate(
            "MainWindow", "FFT Properties", None))
        self.psdFFTSizeLabel.setText(
            _translate("MainWindow", "FFT Size", None))
        self.tabGroup.setTabText(self.tabGroup.indexOf(
            self.freqTab), _translate("MainWindow", "Frequency Domain", None))
        self.groupBox.setTitle(_translate(
            "MainWindow", "Spectrogram Properties", None))
        self.specFFTLabel.setText(_translate("MainWindow", "FFT Size", None))
        self.tabGroup.setTabText(self.tabGroup.indexOf(
            self.specTab), _translate("MainWindow", "Spectrogram", None))
        self.menu_File.setTitle(_translate("MainWindow", "&File", None))
        self.action_open.setText(_translate("MainWindow", "&Open", None))
        self.action_open.setShortcut(_translate("MainWindow", "Ctrl+O", None))
        self.action_exit.setText(_translate("MainWindow", "E&xit", None))
        self.action_reload.setText(_translate("MainWindow", "&Reload", None))

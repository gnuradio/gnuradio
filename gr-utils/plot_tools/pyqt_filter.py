# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pyqt_filter.ui'
#
# Created: Thu Mar 17 10:51:17 2011
#      by: PyQt4 UI code generator 4.7.4
#
# WARNING! All changes made in this file will be lost!

from PyQt5.Qwt import QwtPlot
from PyQt5 import QtCore, QtGui


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(1124, 696)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.gridLayout = QtGui.QGridLayout(self.centralwidget)
        self.gridLayout.setObjectName("gridLayout")
        self.filterFrame = QtGui.QFrame(self.centralwidget)
        self.filterFrame.setMinimumSize(QtCore.QSize(300, 0))
        self.filterFrame.setMaximumSize(QtCore.QSize(300, 16777215))
        self.filterFrame.setFrameShape(QtGui.QFrame.StyledPanel)
        self.filterFrame.setFrameShadow(QtGui.QFrame.Raised)
        self.filterFrame.setObjectName("filterFrame")
        self.verticalLayout = QtGui.QVBoxLayout(self.filterFrame)
        self.verticalLayout.setObjectName("verticalLayout")
        self.filterTypeComboBox = QtGui.QComboBox(self.filterFrame)
        self.filterTypeComboBox.setObjectName("filterTypeComboBox")
        self.filterTypeComboBox.addItem("")
        self.filterTypeComboBox.addItem("")
        self.filterTypeComboBox.addItem("")
        self.filterTypeComboBox.addItem("")
        self.filterTypeComboBox.addItem("")
        self.filterTypeComboBox.addItem("")
        self.filterTypeComboBox.addItem("")
        self.verticalLayout.addWidget(self.filterTypeComboBox)
        self.filterDesignTypeComboBox = QtGui.QComboBox(self.filterFrame)
        self.filterDesignTypeComboBox.setObjectName("filterDesignTypeComboBox")
        self.filterDesignTypeComboBox.addItem("")
        self.filterDesignTypeComboBox.addItem("")
        self.filterDesignTypeComboBox.addItem("")
        self.filterDesignTypeComboBox.addItem("")
        self.filterDesignTypeComboBox.addItem("")
        self.filterDesignTypeComboBox.addItem("")
        self.filterDesignTypeComboBox.addItem("")
        self.verticalLayout.addWidget(self.filterDesignTypeComboBox)
        self.globalParamsLayout = QtGui.QFormLayout()
        self.globalParamsLayout.setFieldGrowthPolicy(
            QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.globalParamsLayout.setObjectName("globalParamsLayout")
        self.sampleRateLabel = QtGui.QLabel(self.filterFrame)
        self.sampleRateLabel.setMaximumSize(QtCore.QSize(16777215, 30))
        self.sampleRateLabel.setObjectName("sampleRateLabel")
        self.globalParamsLayout.setWidget(
            0, QtGui.QFormLayout.LabelRole, self.sampleRateLabel)
        self.sampleRateEdit = QtGui.QLineEdit(self.filterFrame)
        self.sampleRateEdit.setMaximumSize(QtCore.QSize(16777215, 30))
        self.sampleRateEdit.setObjectName("sampleRateEdit")
        self.globalParamsLayout.setWidget(
            0, QtGui.QFormLayout.FieldRole, self.sampleRateEdit)
        self.filterGainLabel = QtGui.QLabel(self.filterFrame)
        self.filterGainLabel.setObjectName("filterGainLabel")
        self.globalParamsLayout.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.filterGainLabel)
        self.filterGainEdit = QtGui.QLineEdit(self.filterFrame)
        self.filterGainEdit.setObjectName("filterGainEdit")
        self.globalParamsLayout.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.filterGainEdit)
        self.verticalLayout.addLayout(self.globalParamsLayout)
        self.filterTypeWidget = QtGui.QStackedWidget(self.filterFrame)
        self.filterTypeWidget.setObjectName("filterTypeWidget")
        self.firlpfPage = QtGui.QWidget()
        self.firlpfPage.setObjectName("firlpfPage")
        self.formLayout = QtGui.QFormLayout(self.firlpfPage)
        self.formLayout.setObjectName("formLayout")
        self.endofLpfPassBandLabel = QtGui.QLabel(self.firlpfPage)
        self.endofLpfPassBandLabel.setObjectName("endofLpfPassBandLabel")
        self.formLayout.setWidget(
            0, QtGui.QFormLayout.LabelRole, self.endofLpfPassBandLabel)
        self.endofLpfPassBandEdit = QtGui.QLineEdit(self.firlpfPage)
        self.endofLpfPassBandEdit.setObjectName("endofLpfPassBandEdit")
        self.formLayout.setWidget(
            0, QtGui.QFormLayout.FieldRole, self.endofLpfPassBandEdit)
        self.startofLpfStopBandLabel = QtGui.QLabel(self.firlpfPage)
        self.startofLpfStopBandLabel.setObjectName("startofLpfStopBandLabel")
        self.formLayout.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.startofLpfStopBandLabel)
        self.startofLpfStopBandEdit = QtGui.QLineEdit(self.firlpfPage)
        self.startofLpfStopBandEdit.setObjectName("startofLpfStopBandEdit")
        self.formLayout.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.startofLpfStopBandEdit)
        self.lpfStopBandAttenLabel = QtGui.QLabel(self.firlpfPage)
        self.lpfStopBandAttenLabel.setObjectName("lpfStopBandAttenLabel")
        self.formLayout.setWidget(
            2, QtGui.QFormLayout.LabelRole, self.lpfStopBandAttenLabel)
        self.lpfStopBandAttenEdit = QtGui.QLineEdit(self.firlpfPage)
        self.lpfStopBandAttenEdit.setObjectName("lpfStopBandAttenEdit")
        self.formLayout.setWidget(
            2, QtGui.QFormLayout.FieldRole, self.lpfStopBandAttenEdit)
        self.lpfPassBandRippleEdit = QtGui.QLineEdit(self.firlpfPage)
        self.lpfPassBandRippleEdit.setObjectName("lpfPassBandRippleEdit")
        self.formLayout.setWidget(
            3, QtGui.QFormLayout.FieldRole, self.lpfPassBandRippleEdit)
        self.lpfPassBandRippleLabel = QtGui.QLabel(self.firlpfPage)
        self.lpfPassBandRippleLabel.setObjectName("lpfPassBandRippleLabel")
        self.formLayout.setWidget(
            3, QtGui.QFormLayout.LabelRole, self.lpfPassBandRippleLabel)
        self.filterTypeWidget.addWidget(self.firlpfPage)
        self.firbpfPage = QtGui.QWidget()
        self.firbpfPage.setObjectName("firbpfPage")
        self.formLayout_2 = QtGui.QFormLayout(self.firbpfPage)
        self.formLayout_2.setObjectName("formLayout_2")
        self.startofBpfPassBandLabel = QtGui.QLabel(self.firbpfPage)
        self.startofBpfPassBandLabel.setObjectName("startofBpfPassBandLabel")
        self.formLayout_2.setWidget(
            0, QtGui.QFormLayout.LabelRole, self.startofBpfPassBandLabel)
        self.startofBpfPassBandEdit = QtGui.QLineEdit(self.firbpfPage)
        self.startofBpfPassBandEdit.setObjectName("startofBpfPassBandEdit")
        self.formLayout_2.setWidget(
            0, QtGui.QFormLayout.FieldRole, self.startofBpfPassBandEdit)
        self.endofBpfPassBandLabel = QtGui.QLabel(self.firbpfPage)
        self.endofBpfPassBandLabel.setObjectName("endofBpfPassBandLabel")
        self.formLayout_2.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.endofBpfPassBandLabel)
        self.endofBpfPassBandEdit = QtGui.QLineEdit(self.firbpfPage)
        self.endofBpfPassBandEdit.setObjectName("endofBpfPassBandEdit")
        self.formLayout_2.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.endofBpfPassBandEdit)
        self.bpfStopBandAttenEdit = QtGui.QLineEdit(self.firbpfPage)
        self.bpfStopBandAttenEdit.setObjectName("bpfStopBandAttenEdit")
        self.formLayout_2.setWidget(
            3, QtGui.QFormLayout.FieldRole, self.bpfStopBandAttenEdit)
        self.bpfStopBandAttenLabel = QtGui.QLabel(self.firbpfPage)
        self.bpfStopBandAttenLabel.setObjectName("bpfStopBandAttenLabel")
        self.formLayout_2.setWidget(
            3, QtGui.QFormLayout.LabelRole, self.bpfStopBandAttenLabel)
        self.bpfTransitionLabel = QtGui.QLabel(self.firbpfPage)
        self.bpfTransitionLabel.setObjectName("bpfTransitionLabel")
        self.formLayout_2.setWidget(
            2, QtGui.QFormLayout.LabelRole, self.bpfTransitionLabel)
        self.bpfTransitionEdit = QtGui.QLineEdit(self.firbpfPage)
        self.bpfTransitionEdit.setObjectName("bpfTransitionEdit")
        self.formLayout_2.setWidget(
            2, QtGui.QFormLayout.FieldRole, self.bpfTransitionEdit)
        self.bpfPassBandRippleEdit = QtGui.QLineEdit(self.firbpfPage)
        self.bpfPassBandRippleEdit.setObjectName("bpfPassBandRippleEdit")
        self.formLayout_2.setWidget(
            4, QtGui.QFormLayout.FieldRole, self.bpfPassBandRippleEdit)
        self.bpfPassBandRippleLabel = QtGui.QLabel(self.firbpfPage)
        self.bpfPassBandRippleLabel.setObjectName("bpfPassBandRippleLabel")
        self.formLayout_2.setWidget(
            4, QtGui.QFormLayout.LabelRole, self.bpfPassBandRippleLabel)
        self.filterTypeWidget.addWidget(self.firbpfPage)
        self.firbnfPage = QtGui.QWidget()
        self.firbnfPage.setObjectName("firbnfPage")
        self.formLayout_5 = QtGui.QFormLayout(self.firbnfPage)
        self.formLayout_5.setFieldGrowthPolicy(
            QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout_5.setObjectName("formLayout_5")
        self.startofBnfStopBandLabel = QtGui.QLabel(self.firbnfPage)
        self.startofBnfStopBandLabel.setObjectName("startofBnfStopBandLabel")
        self.formLayout_5.setWidget(
            0, QtGui.QFormLayout.LabelRole, self.startofBnfStopBandLabel)
        self.startofBnfStopBandEdit = QtGui.QLineEdit(self.firbnfPage)
        self.startofBnfStopBandEdit.setObjectName("startofBnfStopBandEdit")
        self.formLayout_5.setWidget(
            0, QtGui.QFormLayout.FieldRole, self.startofBnfStopBandEdit)
        self.endofBnfStopBandLabel = QtGui.QLabel(self.firbnfPage)
        self.endofBnfStopBandLabel.setObjectName("endofBnfStopBandLabel")
        self.formLayout_5.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.endofBnfStopBandLabel)
        self.endofBnfStopBandEdit = QtGui.QLineEdit(self.firbnfPage)
        self.endofBnfStopBandEdit.setObjectName("endofBnfStopBandEdit")
        self.formLayout_5.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.endofBnfStopBandEdit)
        self.bnfTransitionLabel = QtGui.QLabel(self.firbnfPage)
        self.bnfTransitionLabel.setObjectName("bnfTransitionLabel")
        self.formLayout_5.setWidget(
            2, QtGui.QFormLayout.LabelRole, self.bnfTransitionLabel)
        self.bnfTransitionEdit = QtGui.QLineEdit(self.firbnfPage)
        self.bnfTransitionEdit.setObjectName("bnfTransitionEdit")
        self.formLayout_5.setWidget(
            2, QtGui.QFormLayout.FieldRole, self.bnfTransitionEdit)
        self.bnfStopBandAttenLabel = QtGui.QLabel(self.firbnfPage)
        self.bnfStopBandAttenLabel.setObjectName("bnfStopBandAttenLabel")
        self.formLayout_5.setWidget(
            3, QtGui.QFormLayout.LabelRole, self.bnfStopBandAttenLabel)
        self.bnfStopBandAttenEdit = QtGui.QLineEdit(self.firbnfPage)
        self.bnfStopBandAttenEdit.setObjectName("bnfStopBandAttenEdit")
        self.formLayout_5.setWidget(
            3, QtGui.QFormLayout.FieldRole, self.bnfStopBandAttenEdit)
        self.bnfPassBandRippleLabel = QtGui.QLabel(self.firbnfPage)
        self.bnfPassBandRippleLabel.setObjectName("bnfPassBandRippleLabel")
        self.formLayout_5.setWidget(
            4, QtGui.QFormLayout.LabelRole, self.bnfPassBandRippleLabel)
        self.bnfPassBandRippleEdit = QtGui.QLineEdit(self.firbnfPage)
        self.bnfPassBandRippleEdit.setObjectName("bnfPassBandRippleEdit")
        self.formLayout_5.setWidget(
            4, QtGui.QFormLayout.FieldRole, self.bnfPassBandRippleEdit)
        self.filterTypeWidget.addWidget(self.firbnfPage)
        self.firhpfPage = QtGui.QWidget()
        self.firhpfPage.setObjectName("firhpfPage")
        self.formLayout_3 = QtGui.QFormLayout(self.firhpfPage)
        self.formLayout_3.setFieldGrowthPolicy(
            QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout_3.setObjectName("formLayout_3")
        self.endofHpfStopBandLabel = QtGui.QLabel(self.firhpfPage)
        self.endofHpfStopBandLabel.setObjectName("endofHpfStopBandLabel")
        self.formLayout_3.setWidget(
            0, QtGui.QFormLayout.LabelRole, self.endofHpfStopBandLabel)
        self.endofHpfStopBandEdit = QtGui.QLineEdit(self.firhpfPage)
        self.endofHpfStopBandEdit.setObjectName("endofHpfStopBandEdit")
        self.formLayout_3.setWidget(
            0, QtGui.QFormLayout.FieldRole, self.endofHpfStopBandEdit)
        self.startofHpfPassBandLabel = QtGui.QLabel(self.firhpfPage)
        self.startofHpfPassBandLabel.setObjectName("startofHpfPassBandLabel")
        self.formLayout_3.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.startofHpfPassBandLabel)
        self.startofHpfPassBandEdit = QtGui.QLineEdit(self.firhpfPage)
        self.startofHpfPassBandEdit.setObjectName("startofHpfPassBandEdit")
        self.formLayout_3.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.startofHpfPassBandEdit)
        self.hpfStopBandAttenLabel = QtGui.QLabel(self.firhpfPage)
        self.hpfStopBandAttenLabel.setObjectName("hpfStopBandAttenLabel")
        self.formLayout_3.setWidget(
            2, QtGui.QFormLayout.LabelRole, self.hpfStopBandAttenLabel)
        self.hpfStopBandAttenEdit = QtGui.QLineEdit(self.firhpfPage)
        self.hpfStopBandAttenEdit.setObjectName("hpfStopBandAttenEdit")
        self.formLayout_3.setWidget(
            2, QtGui.QFormLayout.FieldRole, self.hpfStopBandAttenEdit)
        self.hpfPassBandRippleLabel = QtGui.QLabel(self.firhpfPage)
        self.hpfPassBandRippleLabel.setObjectName("hpfPassBandRippleLabel")
        self.formLayout_3.setWidget(
            3, QtGui.QFormLayout.LabelRole, self.hpfPassBandRippleLabel)
        self.hpfPassBandRippleEdit = QtGui.QLineEdit(self.firhpfPage)
        self.hpfPassBandRippleEdit.setObjectName("hpfPassBandRippleEdit")
        self.formLayout_3.setWidget(
            3, QtGui.QFormLayout.FieldRole, self.hpfPassBandRippleEdit)
        self.filterTypeWidget.addWidget(self.firhpfPage)
        self.rrcPage = QtGui.QWidget()
        self.rrcPage.setObjectName("rrcPage")
        self.formLayout_6 = QtGui.QFormLayout(self.rrcPage)
        self.formLayout_6.setObjectName("formLayout_6")
        self.rrcSymbolRateLabel = QtGui.QLabel(self.rrcPage)
        self.rrcSymbolRateLabel.setObjectName("rrcSymbolRateLabel")
        self.formLayout_6.setWidget(
            0, QtGui.QFormLayout.LabelRole, self.rrcSymbolRateLabel)
        self.rrcAlphaLabel = QtGui.QLabel(self.rrcPage)
        self.rrcAlphaLabel.setObjectName("rrcAlphaLabel")
        self.formLayout_6.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.rrcAlphaLabel)
        self.rrcNumTapsLabel = QtGui.QLabel(self.rrcPage)
        self.rrcNumTapsLabel.setObjectName("rrcNumTapsLabel")
        self.formLayout_6.setWidget(
            2, QtGui.QFormLayout.LabelRole, self.rrcNumTapsLabel)
        self.rrcSymbolRateEdit = QtGui.QLineEdit(self.rrcPage)
        self.rrcSymbolRateEdit.setObjectName("rrcSymbolRateEdit")
        self.formLayout_6.setWidget(
            0, QtGui.QFormLayout.FieldRole, self.rrcSymbolRateEdit)
        self.rrcAlphaEdit = QtGui.QLineEdit(self.rrcPage)
        self.rrcAlphaEdit.setObjectName("rrcAlphaEdit")
        self.formLayout_6.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.rrcAlphaEdit)
        self.rrcNumTapsEdit = QtGui.QLineEdit(self.rrcPage)
        self.rrcNumTapsEdit.setObjectName("rrcNumTapsEdit")
        self.formLayout_6.setWidget(
            2, QtGui.QFormLayout.FieldRole, self.rrcNumTapsEdit)
        self.filterTypeWidget.addWidget(self.rrcPage)
        self.gausPage = QtGui.QWidget()
        self.gausPage.setObjectName("gausPage")
        self.formLayout_7 = QtGui.QFormLayout(self.gausPage)
        self.formLayout_7.setObjectName("formLayout_7")
        self.gausSymbolRateLabel = QtGui.QLabel(self.gausPage)
        self.gausSymbolRateLabel.setObjectName("gausSymbolRateLabel")
        self.formLayout_7.setWidget(
            0, QtGui.QFormLayout.LabelRole, self.gausSymbolRateLabel)
        self.gausSymbolRateEdit = QtGui.QLineEdit(self.gausPage)
        self.gausSymbolRateEdit.setObjectName("gausSymbolRateEdit")
        self.formLayout_7.setWidget(
            0, QtGui.QFormLayout.FieldRole, self.gausSymbolRateEdit)
        self.gausBTLabel = QtGui.QLabel(self.gausPage)
        self.gausBTLabel.setObjectName("gausBTLabel")
        self.formLayout_7.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.gausBTLabel)
        self.gausBTEdit = QtGui.QLineEdit(self.gausPage)
        self.gausBTEdit.setObjectName("gausBTEdit")
        self.formLayout_7.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.gausBTEdit)
        self.gausNumTapsLabel = QtGui.QLabel(self.gausPage)
        self.gausNumTapsLabel.setObjectName("gausNumTapsLabel")
        self.formLayout_7.setWidget(
            2, QtGui.QFormLayout.LabelRole, self.gausNumTapsLabel)
        self.gausNumTapsEdit = QtGui.QLineEdit(self.gausPage)
        self.gausNumTapsEdit.setObjectName("gausNumTapsEdit")
        self.formLayout_7.setWidget(
            2, QtGui.QFormLayout.FieldRole, self.gausNumTapsEdit)
        self.filterTypeWidget.addWidget(self.gausPage)
        self.verticalLayout.addWidget(self.filterTypeWidget)
        self.filterPropsBox = QtGui.QGroupBox(self.filterFrame)
        self.filterPropsBox.setObjectName("filterPropsBox")
        self.formLayout_8 = QtGui.QFormLayout(self.filterPropsBox)
        self.formLayout_8.setFieldGrowthPolicy(
            QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout_8.setObjectName("formLayout_8")
        self.nTapsLabel = QtGui.QLabel(self.filterPropsBox)
        self.nTapsLabel.setMinimumSize(QtCore.QSize(150, 0))
        self.nTapsLabel.setObjectName("nTapsLabel")
        self.formLayout_8.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.nTapsLabel)
        self.nTapsEdit = QtGui.QLabel(self.filterPropsBox)
        self.nTapsEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.nTapsEdit.setFrameShape(QtGui.QFrame.Box)
        self.nTapsEdit.setFrameShadow(QtGui.QFrame.Raised)
        self.nTapsEdit.setText("")
        self.nTapsEdit.setObjectName("nTapsEdit")
        self.formLayout_8.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.nTapsEdit)
        self.verticalLayout.addWidget(self.filterPropsBox)
        self.sysParamsBox = QtGui.QGroupBox(self.filterFrame)
        self.sysParamsBox.setObjectName("sysParamsBox")
        self.formLayout_4 = QtGui.QFormLayout(self.sysParamsBox)
        self.formLayout_4.setObjectName("formLayout_4")
        self.nfftEdit = QtGui.QLineEdit(self.sysParamsBox)
        self.nfftEdit.setObjectName("nfftEdit")
        self.formLayout_4.setWidget(
            1, QtGui.QFormLayout.FieldRole, self.nfftEdit)
        self.nfftLabel = QtGui.QLabel(self.sysParamsBox)
        self.nfftLabel.setMinimumSize(QtCore.QSize(150, 0))
        self.nfftLabel.setObjectName("nfftLabel")
        self.formLayout_4.setWidget(
            1, QtGui.QFormLayout.LabelRole, self.nfftLabel)
        self.verticalLayout.addWidget(self.sysParamsBox)
        self.designButton = QtGui.QPushButton(self.filterFrame)
        self.designButton.setMinimumSize(QtCore.QSize(0, 0))
        self.designButton.setMaximumSize(QtCore.QSize(200, 16777215))
        self.designButton.setAutoDefault(True)
        self.designButton.setDefault(True)
        self.designButton.setObjectName("designButton")
        self.verticalLayout.addWidget(self.designButton)
        self.gridLayout.addWidget(self.filterFrame, 1, 0, 1, 1)
        self.tabGroup = QtGui.QTabWidget(self.centralwidget)
        self.tabGroup.setMinimumSize(QtCore.QSize(800, 0))
        self.tabGroup.setObjectName("tabGroup")
        self.freqTab = QtGui.QWidget()
        self.freqTab.setObjectName("freqTab")
        self.horizontalLayout_2 = QtGui.QHBoxLayout(self.freqTab)
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.freqPlot = QwtPlot(self.freqTab)
        self.freqPlot.setObjectName("freqPlot")
        self.horizontalLayout_2.addWidget(self.freqPlot)
        self.tabGroup.addTab(self.freqTab, "")
        self.timeTab = QtGui.QWidget()
        self.timeTab.setObjectName("timeTab")
        self.horizontalLayout = QtGui.QHBoxLayout(self.timeTab)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.timePlot = QwtPlot(self.timeTab)
        self.timePlot.setObjectName("timePlot")
        self.horizontalLayout.addWidget(self.timePlot)
        self.tabGroup.addTab(self.timeTab, "")
        self.phaseTab = QtGui.QWidget()
        self.phaseTab.setObjectName("phaseTab")
        self.horizontalLayout_3 = QtGui.QHBoxLayout(self.phaseTab)
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.phasePlot = QwtPlot(self.phaseTab)
        self.phasePlot.setObjectName("phasePlot")
        self.horizontalLayout_3.addWidget(self.phasePlot)
        self.tabGroup.addTab(self.phaseTab, "")
        self.groupTab = QtGui.QWidget()
        self.groupTab.setObjectName("groupTab")
        self.horizontalLayout_4 = QtGui.QHBoxLayout(self.groupTab)
        self.horizontalLayout_4.setObjectName("horizontalLayout_4")
        self.groupPlot = QwtPlot(self.groupTab)
        self.groupPlot.setObjectName("groupPlot")
        self.horizontalLayout_4.addWidget(self.groupPlot)
        self.tabGroup.addTab(self.groupTab, "")
        self.gridLayout.addWidget(self.tabGroup, 1, 1, 1, 1)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 1124, 27))
        self.menubar.setObjectName("menubar")
        self.menu_File = QtGui.QMenu(self.menubar)
        self.menu_File.setObjectName("menu_File")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.action_exit = QtGui.QAction(MainWindow)
        self.action_exit.setObjectName("action_exit")
        self.action_save = QtGui.QAction(MainWindow)
        self.action_save.setObjectName("action_save")
        self.action_open = QtGui.QAction(MainWindow)
        self.action_open.setObjectName("action_open")
        self.menu_File.addAction(self.action_open)
        self.menu_File.addAction(self.action_save)
        self.menu_File.addAction(self.action_exit)
        self.menubar.addAction(self.menu_File.menuAction())

        self.retranslateUi(MainWindow)
        self.filterTypeWidget.setCurrentIndex(5)
        self.tabGroup.setCurrentIndex(0)
        QtCore.QObject.connect(self.action_exit, QtCore.SIGNAL(
            "activated()"), MainWindow.close)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)
        MainWindow.setTabOrder(self.filterTypeComboBox,
                               self.filterDesignTypeComboBox)
        MainWindow.setTabOrder(
            self.filterDesignTypeComboBox, self.sampleRateEdit)
        MainWindow.setTabOrder(self.sampleRateEdit, self.filterGainEdit)
        MainWindow.setTabOrder(self.filterGainEdit, self.endofLpfPassBandEdit)
        MainWindow.setTabOrder(self.endofLpfPassBandEdit,
                               self.startofLpfStopBandEdit)
        MainWindow.setTabOrder(self.startofLpfStopBandEdit,
                               self.lpfStopBandAttenEdit)
        MainWindow.setTabOrder(self.lpfStopBandAttenEdit,
                               self.lpfPassBandRippleEdit)
        MainWindow.setTabOrder(self.lpfPassBandRippleEdit,
                               self.startofBpfPassBandEdit)
        MainWindow.setTabOrder(self.startofBpfPassBandEdit,
                               self.endofBpfPassBandEdit)
        MainWindow.setTabOrder(self.endofBpfPassBandEdit,
                               self.bpfTransitionEdit)
        MainWindow.setTabOrder(self.bpfTransitionEdit,
                               self.bpfStopBandAttenEdit)
        MainWindow.setTabOrder(self.bpfStopBandAttenEdit,
                               self.bpfPassBandRippleEdit)
        MainWindow.setTabOrder(self.bpfPassBandRippleEdit,
                               self.startofBnfStopBandEdit)
        MainWindow.setTabOrder(self.startofBnfStopBandEdit,
                               self.endofBnfStopBandEdit)
        MainWindow.setTabOrder(self.endofBnfStopBandEdit,
                               self.bnfTransitionEdit)
        MainWindow.setTabOrder(self.bnfTransitionEdit,
                               self.bnfStopBandAttenEdit)
        MainWindow.setTabOrder(self.bnfStopBandAttenEdit,
                               self.bnfPassBandRippleEdit)
        MainWindow.setTabOrder(self.bnfPassBandRippleEdit,
                               self.endofHpfStopBandEdit)
        MainWindow.setTabOrder(self.endofHpfStopBandEdit,
                               self.startofHpfPassBandEdit)
        MainWindow.setTabOrder(self.startofHpfPassBandEdit,
                               self.hpfStopBandAttenEdit)
        MainWindow.setTabOrder(self.hpfStopBandAttenEdit,
                               self.hpfPassBandRippleEdit)
        MainWindow.setTabOrder(
            self.hpfPassBandRippleEdit, self.rrcSymbolRateEdit)
        MainWindow.setTabOrder(self.rrcSymbolRateEdit, self.rrcAlphaEdit)
        MainWindow.setTabOrder(self.rrcAlphaEdit, self.rrcNumTapsEdit)
        MainWindow.setTabOrder(self.rrcNumTapsEdit, self.gausSymbolRateEdit)
        MainWindow.setTabOrder(self.gausSymbolRateEdit, self.gausBTEdit)
        MainWindow.setTabOrder(self.gausBTEdit, self.gausNumTapsEdit)
        MainWindow.setTabOrder(self.gausNumTapsEdit, self.nfftEdit)
        MainWindow.setTabOrder(self.nfftEdit, self.designButton)
        MainWindow.setTabOrder(self.designButton, self.tabGroup)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate(
            "MainWindow", "GNU Radio Filter Design Tool", None, QtGui.QApplication.UnicodeUTF8))
        self.filterTypeComboBox.setItemText(0, QtGui.QApplication.translate(
            "MainWindow", "Low Pass", None, QtGui.QApplication.UnicodeUTF8))
        self.filterTypeComboBox.setItemText(1, QtGui.QApplication.translate(
            "MainWindow", "Band Pass", None, QtGui.QApplication.UnicodeUTF8))
        self.filterTypeComboBox.setItemText(2, QtGui.QApplication.translate(
            "MainWindow", "Complex Band Pass", None, QtGui.QApplication.UnicodeUTF8))
        self.filterTypeComboBox.setItemText(3, QtGui.QApplication.translate(
            "MainWindow", "Band Notch", None, QtGui.QApplication.UnicodeUTF8))
        self.filterTypeComboBox.setItemText(4, QtGui.QApplication.translate(
            "MainWindow", "High Pass", None, QtGui.QApplication.UnicodeUTF8))
        self.filterTypeComboBox.setItemText(5, QtGui.QApplication.translate(
            "MainWindow", "Root Raised Cosine", None, QtGui.QApplication.UnicodeUTF8))
        self.filterTypeComboBox.setItemText(6, QtGui.QApplication.translate(
            "MainWindow", "Gaussian", None, QtGui.QApplication.UnicodeUTF8))
        self.filterDesignTypeComboBox.setItemText(0, QtGui.QApplication.translate(
            "MainWindow", "Hamming Window", None, QtGui.QApplication.UnicodeUTF8))
        self.filterDesignTypeComboBox.setItemText(1, QtGui.QApplication.translate(
            "MainWindow", "Hann Window", None, QtGui.QApplication.UnicodeUTF8))
        self.filterDesignTypeComboBox.setItemText(2, QtGui.QApplication.translate(
            "MainWindow", "Blackman Window", None, QtGui.QApplication.UnicodeUTF8))
        self.filterDesignTypeComboBox.setItemText(3, QtGui.QApplication.translate(
            "MainWindow", "Rectangular Window", None, QtGui.QApplication.UnicodeUTF8))
        self.filterDesignTypeComboBox.setItemText(4, QtGui.QApplication.translate(
            "MainWindow", "Kaiser Window", None, QtGui.QApplication.UnicodeUTF8))
        self.filterDesignTypeComboBox.setItemText(5, QtGui.QApplication.translate(
            "MainWindow", "Blackman-harris Window", None, QtGui.QApplication.UnicodeUTF8))
        self.filterDesignTypeComboBox.setItemText(6, QtGui.QApplication.translate(
            "MainWindow", "Equiripple", None, QtGui.QApplication.UnicodeUTF8))
        self.sampleRateLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Sample Rate (sps)", None, QtGui.QApplication.UnicodeUTF8))
        self.filterGainLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Filter Gain", None, QtGui.QApplication.UnicodeUTF8))
        self.endofLpfPassBandLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "End of Pass Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.startofLpfStopBandLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Start of Stop Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.lpfStopBandAttenLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Stop Band Attenuation (dB)", None, QtGui.QApplication.UnicodeUTF8))
        self.lpfPassBandRippleLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Pass Band Ripple (dB)", None, QtGui.QApplication.UnicodeUTF8))
        self.startofBpfPassBandLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Start of Pass Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.endofBpfPassBandLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "End of Pass Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.bpfStopBandAttenLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Stop Band Attenuation (dB)", None, QtGui.QApplication.UnicodeUTF8))
        self.bpfTransitionLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Transition Width (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.bpfPassBandRippleLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Pass Band Ripple (dB)", None, QtGui.QApplication.UnicodeUTF8))
        self.startofBnfStopBandLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Start of Stop Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.endofBnfStopBandLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "End of Stop Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.bnfTransitionLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Transition Width (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.bnfStopBandAttenLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Stop Band Attenuation (dB)", None, QtGui.QApplication.UnicodeUTF8))
        self.bnfPassBandRippleLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Pass Band Ripple (dB)", None, QtGui.QApplication.UnicodeUTF8))
        self.endofHpfStopBandLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "End of Stop Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.startofHpfPassBandLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Start of Pass Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.hpfStopBandAttenLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Stop Band Attenuation (dB)", None, QtGui.QApplication.UnicodeUTF8))
        self.hpfPassBandRippleLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Pass Band Ripple (dB)", None, QtGui.QApplication.UnicodeUTF8))
        self.rrcSymbolRateLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Symbol Rate (sps)", None, QtGui.QApplication.UnicodeUTF8))
        self.rrcAlphaLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Roll-off Factor", None, QtGui.QApplication.UnicodeUTF8))
        self.rrcNumTapsLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Number of Taps", None, QtGui.QApplication.UnicodeUTF8))
        self.gausSymbolRateLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Symbol Rate (sps)", None, QtGui.QApplication.UnicodeUTF8))
        self.gausBTLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Roll-off Factor", None, QtGui.QApplication.UnicodeUTF8))
        self.gausNumTapsLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Number of Taps", None, QtGui.QApplication.UnicodeUTF8))
        self.filterPropsBox.setTitle(QtGui.QApplication.translate(
            "MainWindow", "Filter Properties", None, QtGui.QApplication.UnicodeUTF8))
        self.nTapsLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Number of Taps:", None, QtGui.QApplication.UnicodeUTF8))
        self.sysParamsBox.setTitle(QtGui.QApplication.translate(
            "MainWindow", "System Parameters", None, QtGui.QApplication.UnicodeUTF8))
        self.nfftLabel.setText(QtGui.QApplication.translate(
            "MainWindow", "Num FFT points", None, QtGui.QApplication.UnicodeUTF8))
        self.designButton.setText(QtGui.QApplication.translate(
            "MainWindow", "Design", None, QtGui.QApplication.UnicodeUTF8))
        self.tabGroup.setTabText(self.tabGroup.indexOf(self.freqTab), QtGui.QApplication.translate(
            "MainWindow", "Frequency Domain", None, QtGui.QApplication.UnicodeUTF8))
        self.tabGroup.setTabText(self.tabGroup.indexOf(self.timeTab), QtGui.QApplication.translate(
            "MainWindow", "Time Domain", None, QtGui.QApplication.UnicodeUTF8))
        self.tabGroup.setTabText(self.tabGroup.indexOf(self.phaseTab), QtGui.QApplication.translate(
            "MainWindow", "Phase", None, QtGui.QApplication.UnicodeUTF8))
        self.tabGroup.setTabText(self.tabGroup.indexOf(self.groupTab), QtGui.QApplication.translate(
            "MainWindow", "Group Delay", None, QtGui.QApplication.UnicodeUTF8))
        self.menu_File.setTitle(QtGui.QApplication.translate(
            "MainWindow", "&File", None, QtGui.QApplication.UnicodeUTF8))
        self.action_exit.setText(QtGui.QApplication.translate(
            "MainWindow", "E&xit", None, QtGui.QApplication.UnicodeUTF8))
        self.action_save.setText(QtGui.QApplication.translate(
            "MainWindow", "&Save", None, QtGui.QApplication.UnicodeUTF8))
        self.action_save.setShortcut(QtGui.QApplication.translate(
            "MainWindow", "Ctrl+S", None, QtGui.QApplication.UnicodeUTF8))
        self.action_open.setText(QtGui.QApplication.translate(
            "MainWindow", "&Open", None, QtGui.QApplication.UnicodeUTF8))
        self.action_open.setShortcut(QtGui.QApplication.translate(
            "MainWindow", "Ctrl+O", None, QtGui.QApplication.UnicodeUTF8))


#from qwt_plot import QwtPlot

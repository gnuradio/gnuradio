# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'usrp_display_qtgui.ui'
#
# Created: Thu Jul 16 22:06:24 2009
#      by: PyQt4 UI code generator 4.4.3
# Updated: Migrated to PyQt5 with modern signal/slot syntax
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(820, 774)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.gridLayout_2 = QtWidgets.QGridLayout(self.centralwidget)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.groupBox = QtWidgets.QGroupBox(self.centralwidget)
        sizePolicy = QtWidgets.QSizePolicy(
            QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(
            self.groupBox.sizePolicy().hasHeightForWidth())
        self.groupBox.setSizePolicy(sizePolicy)
        self.groupBox.setMinimumSize(QtCore.QSize(240, 150))
        self.groupBox.setMaximumSize(QtCore.QSize(240, 16777215))
        self.groupBox.setObjectName("groupBox")
        self.formLayoutWidget = QtWidgets.QWidget(self.groupBox)
        self.formLayoutWidget.setGeometry(QtCore.QRect(10, 20, 221, 124))
        self.formLayoutWidget.setObjectName("formLayoutWidget")
        self.formLayout = QtWidgets.QFormLayout(self.formLayoutWidget)
        self.formLayout.setObjectName("formLayout")
        self.frequencyLabel = QtWidgets.QLabel(self.formLayoutWidget)
        self.frequencyLabel.setObjectName("frequencyLabel")
        self.formLayout.setWidget(
            0, QtWidgets.QFormLayout.LabelRole, self.frequencyLabel)
        self.gainLabel = QtWidgets.QLabel(self.formLayoutWidget)
        self.gainLabel.setObjectName("gainLabel")
        self.formLayout.setWidget(
            1, QtWidgets.QFormLayout.LabelRole, self.gainLabel)
        self.bandwidthLabel = QtWidgets.QLabel(self.formLayoutWidget)
        self.bandwidthLabel.setObjectName("bandwidthLabel")
        self.formLayout.setWidget(
            2, QtWidgets.QFormLayout.LabelRole, self.bandwidthLabel)
        self.frequencyEdit = QtWidgets.QLineEdit(self.formLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(
            QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(
            self.frequencyEdit.sizePolicy().hasHeightForWidth())
        self.frequencyEdit.setSizePolicy(sizePolicy)
        self.frequencyEdit.setMinimumSize(QtCore.QSize(120, 26))
        self.frequencyEdit.setObjectName("frequencyEdit")
        self.formLayout.setWidget(
            0, QtWidgets.QFormLayout.FieldRole, self.frequencyEdit)
        self.gainEdit = QtWidgets.QLineEdit(self.formLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(
            QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(
            self.gainEdit.sizePolicy().hasHeightForWidth())
        self.gainEdit.setSizePolicy(sizePolicy)
        self.gainEdit.setMinimumSize(QtCore.QSize(120, 26))
        self.gainEdit.setObjectName("gainEdit")
        self.formLayout.setWidget(
            1, QtWidgets.QFormLayout.FieldRole, self.gainEdit)
        self.bandwidthEdit = QtWidgets.QLineEdit(self.formLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(
            QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(
            self.bandwidthEdit.sizePolicy().hasHeightForWidth())
        self.bandwidthEdit.setSizePolicy(sizePolicy)
        self.bandwidthEdit.setMinimumSize(QtCore.QSize(120, 26))
        self.bandwidthEdit.setObjectName("bandwidthEdit")
        self.formLayout.setWidget(
            2, QtWidgets.QFormLayout.FieldRole, self.bandwidthEdit)
        self.amplifierLabel = QtWidgets.QLabel(self.formLayoutWidget)
        self.amplifierLabel.setObjectName("amplifierLabel")
        self.formLayout.setWidget(
            3, QtWidgets.QFormLayout.LabelRole, self.amplifierLabel)
        self.amplifierEdit = QtWidgets.QLineEdit(self.formLayoutWidget)
        sizePolicy = QtWidgets.QSizePolicy(
            QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(
            self.amplifierEdit.sizePolicy().hasHeightForWidth())
        self.amplifierEdit.setSizePolicy(sizePolicy)
        self.amplifierEdit.setMinimumSize(QtCore.QSize(120, 26))
        self.amplifierEdit.setObjectName("amplifierEdit")
        self.formLayout.setWidget(
            3, QtWidgets.QFormLayout.FieldRole, self.amplifierEdit)
        self.horizontalLayout_2.addWidget(self.groupBox)
        self.frame_2 = QtWidgets.QFrame(self.centralwidget)
        self.frame_2.setMinimumSize(QtCore.QSize(200, 0))
        self.frame_2.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame_2.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame_2.setObjectName("frame_2")
        self.verticalLayoutWidget = QtWidgets.QWidget(self.frame_2)
        self.verticalLayoutWidget.setGeometry(QtCore.QRect(10, -1, 191, 151))
        self.verticalLayoutWidget.setObjectName("verticalLayoutWidget")
        self.verticalLayout_3 = QtWidgets.QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout_3.setObjectName("verticalLayout_3")
        self.dcCancelCheckBox = QtWidgets.QCheckBox(self.verticalLayoutWidget)
        self.dcCancelCheckBox.setObjectName("dcCancelCheckBox")
        self.verticalLayout_3.addWidget(self.dcCancelCheckBox)
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.dcGainLabel = QtWidgets.QLabel(self.verticalLayoutWidget)
        self.dcGainLabel.setObjectName("dcGainLabel")
        self.horizontalLayout.addWidget(self.dcGainLabel)
        self.dcGainEdit = QtWidgets.QLineEdit(self.verticalLayoutWidget)
        self.dcGainEdit.setObjectName("dcGainEdit")
        self.horizontalLayout.addWidget(self.dcGainEdit)
        self.verticalLayout_3.addLayout(self.horizontalLayout)
        spacerItem = QtWidgets.QSpacerItem(
            20, 40, QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Expanding)
        self.verticalLayout_3.addItem(spacerItem)
        self.horizontalLayout_2.addWidget(self.frame_2)
        spacerItem1 = QtWidgets.QSpacerItem(
            40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem1)
        self.verticalLayout = QtWidgets.QVBoxLayout()
        self.verticalLayout.setObjectName("verticalLayout")
        spacerItem2 = QtWidgets.QSpacerItem(
            20, 80, QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Fixed)
        self.verticalLayout.addItem(spacerItem2)
        self.pauseButton = QtWidgets.QPushButton(self.centralwidget)
        sizePolicy = QtWidgets.QSizePolicy(
            QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(
            self.pauseButton.sizePolicy().hasHeightForWidth())
        self.pauseButton.setSizePolicy(sizePolicy)
        self.pauseButton.setObjectName("pauseButton")
        self.verticalLayout.addWidget(self.pauseButton)
        self.closeButton = QtWidgets.QPushButton(self.centralwidget)
        sizePolicy = QtWidgets.QSizePolicy(
            QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(
            self.closeButton.sizePolicy().hasHeightForWidth())
        self.closeButton.setSizePolicy(sizePolicy)
        self.closeButton.setMinimumSize(QtCore.QSize(75, 0))
        self.closeButton.setObjectName("closeButton")
        self.verticalLayout.addWidget(self.closeButton)
        self.horizontalLayout_2.addLayout(self.verticalLayout)
        self.gridLayout_2.addLayout(self.horizontalLayout_2, 1, 0, 1, 1)
        self.verticalLayout_2 = QtWidgets.QVBoxLayout()
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.frame = QtWidgets.QFrame(self.centralwidget)
        sizePolicy = QtWidgets.QSizePolicy(
            QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(1)
        sizePolicy.setHeightForWidth(
            self.frame.sizePolicy().hasHeightForWidth())
        self.frame.setSizePolicy(sizePolicy)
        self.frame.setMinimumSize(QtCore.QSize(800, 550))
        self.frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame.setObjectName("frame")
        self.gridLayout = QtWidgets.QGridLayout(self.frame)
        self.gridLayout.setObjectName("gridLayout")
        self.sinkLayout = QtWidgets.QHBoxLayout()
        self.sinkLayout.setObjectName("sinkLayout")
        self.gridLayout.addLayout(self.sinkLayout, 0, 0, 1, 1)
        self.verticalLayout_2.addWidget(self.frame)
        self.gridLayout_2.addLayout(self.verticalLayout_2, 0, 0, 1, 1)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 820, 24))
        self.menubar.setObjectName("menubar")
        self.menuFile = QtWidgets.QMenu(self.menubar)
        self.menuFile.setObjectName("menuFile")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.actionExit = QtWidgets.QAction(MainWindow)
        self.actionExit.setObjectName("actionExit")
        self.actionSaveData = QtWidgets.QAction(MainWindow)
        self.actionSaveData.setObjectName("actionSaveData")
        self.menuFile.addAction(self.actionSaveData)
        self.menuFile.addAction(self.actionExit)
        self.menubar.addAction(self.menuFile.menuAction())

        self.retranslateUi(MainWindow)
        self.closeButton.clicked.connect(MainWindow.close)
        self.actionExit.triggered.connect(MainWindow.close)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtWidgets.QApplication.translate(
            "MainWindow", "USRP Display", None))
        self.groupBox.setTitle(QtWidgets.QApplication.translate(
            "MainWindow", "Receiver Parameters", None))
        self.frequencyLabel.setText(QtWidgets.QApplication.translate(
            "MainWindow", "Frequency (Hz)", None))
        self.gainLabel.setText(QtWidgets.QApplication.translate(
            "MainWindow", "RF Gain", None))
        self.bandwidthLabel.setText(QtWidgets.QApplication.translate(
            "MainWindow", "Bandwidth", None))
        self.amplifierLabel.setText(QtWidgets.QApplication.translate(
            "MainWindow", "Amplifier", None))
        self.dcCancelCheckBox.setText(QtWidgets.QApplication.translate(
            "MainWindow", "Cancel DC", None))
        self.dcGainLabel.setText(QtWidgets.QApplication.translate(
            "MainWindow", "DC Canceller Gain", None))
        self.pauseButton.setText(QtWidgets.QApplication.translate(
            "MainWindow", "Pause", None))
        self.closeButton.setText(QtWidgets.QApplication.translate(
            "MainWindow", "Close", None))
        self.menuFile.setTitle(QtWidgets.QApplication.translate(
            "MainWindow", "&File", None))
        self.actionExit.setText(QtWidgets.QApplication.translate(
            "MainWindow", "E&xit", None))
        self.actionSaveData.setText(QtWidgets.QApplication.translate(
            "MainWindow", "&Save Data", None))

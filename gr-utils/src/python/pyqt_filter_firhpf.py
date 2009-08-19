# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pyqt_filter_firhpf.ui'
#
# Created: Tue Aug 18 22:19:04 2009
#      by: PyQt4 UI code generator 4.4.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_firhpfForm(object):
    def setupUi(self, firhpfForm):
        firhpfForm.setObjectName("firhpfForm")
        firhpfForm.resize(335, 300)
        firhpfForm.setMinimumSize(QtCore.QSize(200, 0))
        self.formLayout = QtGui.QFormLayout(firhpfForm)
        self.formLayout.setFieldGrowthPolicy(QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout.setObjectName("formLayout")
        self.endofPassBandLabel = QtGui.QLabel(firhpfForm)
        self.endofPassBandLabel.setObjectName("endofPassBandLabel")
        self.formLayout.setWidget(0, QtGui.QFormLayout.LabelRole, self.endofPassBandLabel)
        self.endofPassBandEdit = QtGui.QLineEdit(firhpfForm)
        self.endofPassBandEdit.setObjectName("endofPassBandEdit")
        self.formLayout.setWidget(0, QtGui.QFormLayout.FieldRole, self.endofPassBandEdit)
        self.startofStopBandLabel = QtGui.QLabel(firhpfForm)
        self.startofStopBandLabel.setObjectName("startofStopBandLabel")
        self.formLayout.setWidget(1, QtGui.QFormLayout.LabelRole, self.startofStopBandLabel)
        self.startofStopBandEdit = QtGui.QLineEdit(firhpfForm)
        self.startofStopBandEdit.setObjectName("startofStopBandEdit")
        self.formLayout.setWidget(1, QtGui.QFormLayout.FieldRole, self.startofStopBandEdit)
        self.stopBandAttenLabel = QtGui.QLabel(firhpfForm)
        self.stopBandAttenLabel.setObjectName("stopBandAttenLabel")
        self.formLayout.setWidget(2, QtGui.QFormLayout.LabelRole, self.stopBandAttenLabel)
        self.stopBandAttenEdit = QtGui.QLineEdit(firhpfForm)
        self.stopBandAttenEdit.setObjectName("stopBandAttenEdit")
        self.formLayout.setWidget(2, QtGui.QFormLayout.FieldRole, self.stopBandAttenEdit)

        self.retranslateUi(firhpfForm)
        QtCore.QMetaObject.connectSlotsByName(firhpfForm)

    def retranslateUi(self, firhpfForm):
        firhpfForm.setWindowTitle(QtGui.QApplication.translate("firhpfForm", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.endofPassBandLabel.setText(QtGui.QApplication.translate("firhpfForm", "End of Stop Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.startofStopBandLabel.setText(QtGui.QApplication.translate("firhpfForm", "Start of Pass Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.stopBandAttenLabel.setText(QtGui.QApplication.translate("firhpfForm", "Stop Band Attenuation (dB)", None, QtGui.QApplication.UnicodeUTF8))


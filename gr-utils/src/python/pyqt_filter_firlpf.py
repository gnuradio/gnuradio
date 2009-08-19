# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pyqt_filter_firlpf.ui'
#
# Created: Tue Aug 18 22:19:03 2009
#      by: PyQt4 UI code generator 4.4.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_firlpfForm(object):
    def setupUi(self, firlpfForm):
        firlpfForm.setObjectName("firlpfForm")
        firlpfForm.resize(335, 300)
        firlpfForm.setMinimumSize(QtCore.QSize(200, 0))
        self.formLayout = QtGui.QFormLayout(firlpfForm)
        self.formLayout.setFieldGrowthPolicy(QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout.setObjectName("formLayout")
        self.endofPassBandLabel = QtGui.QLabel(firlpfForm)
        self.endofPassBandLabel.setObjectName("endofPassBandLabel")
        self.formLayout.setWidget(0, QtGui.QFormLayout.LabelRole, self.endofPassBandLabel)
        self.endofPassBandEdit = QtGui.QLineEdit(firlpfForm)
        self.endofPassBandEdit.setObjectName("endofPassBandEdit")
        self.formLayout.setWidget(0, QtGui.QFormLayout.FieldRole, self.endofPassBandEdit)
        self.startofStopBandLabel = QtGui.QLabel(firlpfForm)
        self.startofStopBandLabel.setObjectName("startofStopBandLabel")
        self.formLayout.setWidget(1, QtGui.QFormLayout.LabelRole, self.startofStopBandLabel)
        self.stopBandAttenLabel = QtGui.QLabel(firlpfForm)
        self.stopBandAttenLabel.setObjectName("stopBandAttenLabel")
        self.formLayout.setWidget(2, QtGui.QFormLayout.LabelRole, self.stopBandAttenLabel)
        self.stopBandAttenEdit = QtGui.QLineEdit(firlpfForm)
        self.stopBandAttenEdit.setObjectName("stopBandAttenEdit")
        self.formLayout.setWidget(2, QtGui.QFormLayout.FieldRole, self.stopBandAttenEdit)
        self.startofStopBandEdit = QtGui.QLineEdit(firlpfForm)
        self.startofStopBandEdit.setObjectName("startofStopBandEdit")
        self.formLayout.setWidget(1, QtGui.QFormLayout.FieldRole, self.startofStopBandEdit)

        self.retranslateUi(firlpfForm)
        QtCore.QMetaObject.connectSlotsByName(firlpfForm)

    def retranslateUi(self, firlpfForm):
        firlpfForm.setWindowTitle(QtGui.QApplication.translate("firlpfForm", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.endofPassBandLabel.setText(QtGui.QApplication.translate("firlpfForm", "End of Pass Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.startofStopBandLabel.setText(QtGui.QApplication.translate("firlpfForm", "Start of Stop Band (Hz)", None, QtGui.QApplication.UnicodeUTF8))
        self.stopBandAttenLabel.setText(QtGui.QApplication.translate("firlpfForm", "Stop Band Attenuation (dB)", None, QtGui.QApplication.UnicodeUTF8))


from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
import inspect
import itertools    

class msUnitWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super( msUnitWidget, self).__init__(data, mainwindows)
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QLabel("Unit:"))
        self.lineEdit = QtGui.QLineEdit(data.getStr())
        hbox.addWidget(self.lineEdit)
        bttnOk = QtGui.QPushButton("ok")
        bttnOk.clicked.connect(self.updateUnit)
        hbox.addWidget(bttnOk)
        self.vbox.addLayout(hbox)

        self.setMinimumSize(270, 120)
        self.setMaximumSize(270,120)
    
    def updateUnit(self):
        
        self.data.set(str(self.lineEdit.text()))

        
        
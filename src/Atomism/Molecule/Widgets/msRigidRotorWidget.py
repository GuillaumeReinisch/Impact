"""
Created on Sat Aug 24 16:39:31 2013

@author: chico
"""

from libimpact import *
from msWidget import *
from PySide import QtCore, QtGui
from Dialogs.queryTreeMapperDialog import *

class msRigidRotorWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msRigidRotorWidget, self).__init__(data,mainwindows)
        
        hbox=QtGui.QHBoxLayout()        
        bttn = QtGui.QPushButton("Set mmt of inertia / System") 
        bttn.clicked.connect(self.setSystem)
        hbox.addWidget(bttn)
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)
        
        hbox=QtGui.QHBoxLayout()
        self.I1label = QtGui.QLabel("I1 = " +str(data.getI1()))
        hbox.addWidget( self.I1label )
        hbox.addStretch(1)
        hbox.addWidget( QtGui.QPushButton("set")  )
        self.vbox.addLayout(hbox)
        
        hbox=QtGui.QHBoxLayout()
        self.I2label = QtGui.QLabel("I2 = " +str(data.getI2()))
        hbox.addWidget(self.I2label  )
        hbox.addStretch(1)
        hbox.addWidget( QtGui.QPushButton("set")  )
        self.vbox.addLayout(hbox)
        
        hbox=QtGui.QHBoxLayout()
        self.I3label = QtGui.QLabel("I3 = " +str(data.getI3()))
        hbox.addWidget(self.I3label  )
        hbox.addStretch(1)
        hbox.addWidget( QtGui.QPushButton("set")  )
        self.vbox.addLayout(hbox)
        
        self.groupbox.setTitle("Rigid rotor")
        self.setMinimumSize(550,200)
      
    def setSystem(self):
        widget = queryTreeMapperDialog("select the system","select the system in the tree","msSystem", self.mainwindows.rootObject,  self.data.setMmtOfInertia , self)
        widget.exec_()
        self.updateLabels()
        
    def updateLabels(self):
        self.I1label.setText("I1 = " +str(self.data.getI1()))
        self.I2label.setText("I1 = " +str(self.data.getI2()))
        self.I3label.setText("I1 = " +str(self.data.getI3()))
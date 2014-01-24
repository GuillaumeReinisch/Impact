"""
    Created on Sat Aug 24 15:38:04 2013
    
    @author: chico
    """

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from Dialogs.simpleDialogs import *
import inspect
import itertools
import re
import functools



class msReactionFallOffWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msReactionFallOffWidget, self).__init__(data, mainwindows)
        
        self.vbox.addWidget(QtGui.QLabel("Low pressure definition"))
        self.vbox.addSpacing(10)
        self.gridbox = QtGui.QGridLayout()
        self.gridbox.addWidget( QtGui.QLabel("A:"),1,1)
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget( QtGui.QLabel(str('{:.3e}'.format(data.getALow()))+" /"+data.getUnits().getTimeStr()) )
        hbox.addStretch(1)
        bttn =  QtGui.QPushButton("set")
        bttn.clicked.connect(self.setA)
        hbox.addWidget( bttn)
        self.gridbox.addLayout( hbox , 1,2 )
        
        self.gridbox.addWidget( QtGui.QLabel("b:"),2,1)
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget( QtGui.QLabel( str('{:.3e}'.format(data.getBetaLow())) ) )
        hbox.addStretch(1)
        bttn =  QtGui.QPushButton("set")
        bttn.clicked.connect(self.setBeta)
        hbox.addWidget( bttn)
        self.gridbox.addLayout( hbox , 2,2 )
        
        self.gridbox.addWidget( QtGui.QLabel("Ea:"),3,1)
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget( QtGui.QLabel(str('{:.3e}'.format(data.getEaLow()))+" "+self.data.getUnits().getEnergyStr()+"/"+self.data.getUnits().getQuantityStr() ) )
        hbox.addStretch(1)
        bttn =  QtGui.QPushButton("set")
        bttn.clicked.connect(self.setEa)
        hbox.addWidget( bttn)
        self.gridbox.addLayout( hbox , 3,2 )
        
        self.vbox.addLayout(self.gridbox)
        self.setMinimumSize(300,170 )
        self.setMaximumSize(300,170 )

    def setCoeff(self,value,name):        
        self.data.setCoefficient(name,value)
    
    def setA(self):
        A = self.data.getA()
        widget = queryDoubleDialog("k(T)=A.T^Beta.exp(-Ea/RT)","enter the pre-exponentialfactor in /"+self.data.getUnits().getTimeStr(),  A, functools.partial(self.setCoeff,"ALow" ))
        widget.exec_()
        self.mainwindows.reload()
    
    def setBeta(self):
        beta = self.data.getBeta()
        widget = queryDoubleDialog("k(T)=A.T^Beta.exp(-Ea/RT)","enter the beta parameter in",  Beta, functools.partial(self.setCoeff,"BetaLow" ) )
        widget.exec_()
        self.mainwindows.reload()
    
    def setEa(self):
        Ea = self.data.getEa()
        widget = queryDoubleDialog("k(T)=A.T^Beta.exp(-Ea/RT)","enter the activation energy parameter in "+self.data.getUnits().getEnergyStr()+"/"+self.data.getUnits().getQuantityStr(),  Ea, functools.partial(self.setCoeff,"EaLow" ) )
        widget.exec_()

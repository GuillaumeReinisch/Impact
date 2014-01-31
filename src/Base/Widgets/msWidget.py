"""
Created on Mon Sep  9 08:55:04 2013

@author: chico
"""

# -*- coding: utf-8 -*-
"""
Created on Sat Aug 24 15:38:04 2013

@author: chico
"""

from libimpact import *
from PySide import QtCore, QtGui



        
class msWidget(QtGui.QWidget):
    """ This class defines the base widget of all ms*Widget classes 
    """    
    def __init__(self, data, mainwindows):
        super(msWidget, self).__init__()
        self.data = data
        self.mainwindows = mainwindows
        self.mainbox  =  QtGui.QVBoxLayout()
        self.mainbox.setSpacing(5)
        self.groupbox  =  QtGui.QGroupBox( self.data.getType().split(":")[-1] )
        self.vbox = QtGui.QVBoxLayout()
        self.vbox.setSpacing(2)
        self.groupbox.setLayout(self.vbox)                  
        self.mainbox.addWidget(self.groupbox)
        self.setLayout(self.mainbox)
        
        self.setMaximumHeight(50)


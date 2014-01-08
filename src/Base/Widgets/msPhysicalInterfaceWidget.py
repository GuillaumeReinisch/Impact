"""
Created on Sat Aug 24 16:17:53 2013

@author: chico
"""

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from msUnitsManagerWidget import *
from msParamsManagerWidget  import *

class msPhysicalInterfaceWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msPhysicalInterfaceWidget, self).__init__(data,mainwindows)
        
        
        self.Units          = msUnitsManagerWidget(self.data.getUnits(),mainwindows)
        self.Parameters = msParamsManagerWidget(self.data.getParameters(),mainwindows)
        height               = max(self.Units.size().height(),self.Parameters.size().height())
        self.Parameters.resize(self.Parameters.width(),height)
        self.Units.resize(self.Units.width(),height)
       
        self.vbox.addWidget(self.Units)
        self.vbox.addWidget(self.Parameters)
        self.setMinimumHeight(550)
        self.setMaximumHeight(550)

        self.setMinimumWidth(550)
        self.setMaximumWidth(550)
        self.groupbox.setTitle("&Physical interface")

 
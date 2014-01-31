"""
Created on Sat Aug 24 16:39:31 2013

@author: chico
"""

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from msGeneralizedCoordinatesWidget import *
from msQuantumInterfaceWidget import *
from Dialogs.queryTreeMapperDialog import *

class msLinearRigidRotorWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msLinearRigidRotorWidget, self).__init__(data,mainwindows)
        
        try:
           data.computeEigenValues()
        except:
	   pass
	 
        self.qInterface = msQuantumInterfaceWidget(data,mainwindows)
        self.vbox.addWidget(self.qInterface)
        
        self.groupbox.setTitle("Linear rigid rotor")
       
        self.setMinimumSize(550,400)

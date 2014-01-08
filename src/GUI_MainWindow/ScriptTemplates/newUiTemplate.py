""" 
Created the $date 

@author:
"""

from libimpact import *
from PySide import QtCore, QtGui
from widgets.msWidget import *

class $classname(msWidget):
    
    """
    Widget for the class $classname2
    """
    def __init__(self, data , mainwindow):
        super($classname, self).__init__(data,mainwindow)
        
        label = QtGui.QLabel("Hello world " + data.getType() )
        self.vbox.addWidget(label)
        
        self.groupbox.setTitle(data.getType())
        self.setMinimumSize(550,100)
        self.setMaximumSize(550,100)
              
  

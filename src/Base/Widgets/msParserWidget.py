"""
Created on Sat Aug 24 16:17:53 2013

@author: chico
"""

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *

class msParserWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msParserWidget, self).__init__(data,mainwindows)
        
        
        """ First create the Units group box
        """
        
        self.expression = QtGui.QTextEdit()
        self.expression.setText(self.data.getExpression())
        
        self.vbox.addWidget(QtGui.QLabel("expression:")) 
        self.vbox.addWidget(self.expression)    
        self.update = QtGui.QPushButton("update")
        self.vbox.addWidget(self.update)  
        
        self.update.clicked.connect(self.updateExpression)
        
        self.groupbox.setTitle("Parser")
        self.setMinimumSize(400,200)
        
    
    def updateExpression(self):
        
        self.data.setExpression(str(self.expression.toPlainText()))
        self.mainwindows.clickTreeView()
  

"""
Created on Mon Sep  9 10:38:14 2013

@author: chico
"""
from libimpact import *
from PySide import QtCore, QtGui
from widgets.msWidget import *
from Dialogs.simpleDialogs import *
import string

class msExceptionDialog(QtGui.QMessageBox):
    
    def __init__(self,data):
        super(msExceptionDialog, self).__init__()
        
        self.setWindowTitle(data.getInfo().split(':')[0])
        self.setIcon(QtGui.QMessageBox.Critical)
        self.setText( data.getInfo() )
        
        text = "Object: " + data.getId()+"\n\nFunction: "+data.getFunction()+"\n\nTraceback: \n"
        for i in data.getContexts():
	     text=text+"\n  - "+i
	if len(data.getContexts())==0:
           text = text + "You need to activate the debug mode for traceback information"
           
        self.setInformativeText(text)

    def ok(self):
        self.close()
# -*- coding: utf-8 -*-
"""
Created on Thu Aug  1 14:33:31 2013

@author: chico
"""


import sys, functools
from PySide import QtCore, QtGui

from TreeModels import *
from DockElements.PythonConsole import *
from DockElements.PythonHighlighter import *

class OutLog:
    """ Writer of python's out streams into a QTextEdit
    """  
    def __init__(self, edit, out=None, color=None):
        """(edit, out=None, color=None) -> can write stdout, stderr to a
        QTextEdit.
        edit = QTextEdit
        out = alternate stream ( can be the original sys.stdout )
        color = alternate color (i.e. color stderr a different color)
        """
        self.edit = edit
        self.out = None
        self.color = color

    def write(self, m):
        if self.color:
            tc = self.edit.textColor()
            self.edit.setTextColor(self.color)

        self.edit.moveCursor(QtGui.QTextCursor.End)
        self.edit.insertPlainText( m )

        if self.color:
            self.edit.setTextColor(tc)

        if self.out:
            self.out.write(m)
   
    def flush(self):
        pass

class uiScriptWidget(QtGui.QWidget):
    """ This class defines the script widget, a tab widget with QTextEdit 
    highlighted by QSynthaxHighlighter
    """    
    def __init__( self , mainwindow):
        super(uiScriptWidget, self).__init__()
        self.mainWindow = mainwindow
        self.textEdits=[]
        self.initUI()
        #self.addTab("untitled_0")
        
    def setStdOut(self,out):
        """ set the output widget where out streams are written
        
        out : output widget
        """
        self.out = out
        
    def initUI(self):

        vbox  =  QtGui.QVBoxLayout()
        
        self.tabWidget = QtGui.QTabWidget()       
        self.tabWidget.setTabsClosable(1)
        self.tabWidget.tabCloseRequested.connect(self.closeTab)
        vbox.addWidget( self.tabWidget )   
        self.setLayout(vbox)
        
        self.out  = self.mainWindow.outStream
        self.nTab = 0
    
    def closeCurrent(self):
        self.tabWidget.removeTab(self.tabWidget.currentIndex())
        
    def closeTab(self,index):
        self.tabWidget.removeTab(index)
        
        
    def addTab(self,name):
        text = QtGui.QTextEdit()
        self.tabWidget.addTab(text,name)
        self.tabWidget.setCurrentIndex(self.nTab)
        text.textChanged.connect(self.scriptChanged) 
        self.highlighter = Python(text.document())
        self.nTab = self.nTab +1
        self.textEdits.append(text)

    def currentScript(self):
        return self.tabWidget.currentWidget()  
     
    def scriptChanged(self):
        i = self.tabWidget.currentIndex()
        #if( self.tabWidget.tabText(i)[-1]!="*"):
        #    self.currentScript.setTabText( i, self.tabWidget.tabText(i) + "*" )    
        
    def copy(self):
        if(self.nTab>0):
            self.currentScript().copy()
            
    def cut(self):
        if(self.nTab>0):
            self.currentScript().cut()
        
    def paste(self):
        if(self.nTab>0):
            self.currentScript().paste()
        
    def undo(self):
        if(self.nTab>0):
            self.currentScript().undo()

    def redo(self):
        if(self.nTab>0):
            self.currentScript().redo()
            
    def delete(self):
        if(self.nTab>0):
            self.currentScript().delete()
            
    def runBtn(self):
        if(self.nTab>0):
            sys.stdout = OutLog( self.out, sys.stdout)
            mycode = self.tabWidget.currentWidget().toPlainText()
            try:
                self.mainWindow.runScript(mycode)
            except:
                self.mainWindow.reportException(sys.exc_info())
                
    def runSelected(self):
        if(self.nTab>0):
            sys.stdout = OutLog( self.out, sys.stdout)
            mycode = self.tabWidget.currentWidget().textCursor().selectedText()
            try:
                self.mainWindow.runScript(mycode)
            except:
                self.mainWindow.reportException(sys.exc_info())               
                
    def runLine(self):
        self.tabWidget.currentWidget().textCursor().select(QtGui.QTextCursor.LineUnderCursor)
        self.runSelected()

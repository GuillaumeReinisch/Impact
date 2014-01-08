# -*- coding: utf-8 -*-
"""
Created on Fri Aug 23 16:52:41 2013

@author: chico
"""

import sys
from PySide import QtCore, QtGui, QtWebKit
from libimpact import *
import inspect
from string import Template
import datetime


class ChildGroupBox(QtGui.QGroupBox):

    def __init__(self, parent=None):
        super(ChildGroupBox,self).__init__(parent)

        self.className = QtGui.QComboBox()

        self.className.addItem("msTreeMapper")
        self.addClassAvailable(msTreeMapper,self.className)
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QLabel("Child type:"))
        hbox.addWidget(self.className)
        self.vbox = QtGui.QVBoxLayout()
        self.vbox.addLayout(hbox )
        
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QLabel("Child name:"))
        self.lineEdit=QtGui.QLineEdit()
        hbox.addWidget(self.lineEdit)
        self.vbox.addLayout(hbox )
        self.vbox.addWidget(self.className )
        self.setLayout(self.vbox)
        self.setTitle("child")
        self.setMinimumSize(300,150)
        self.setMaximumSize(300,150)
   
    def addClassAvailable(self,classebase,combobox):
        
        if(len(classebase.__subclasses__())==0):
            return
            
        for cls in classebase.__subclasses__():
            name =  str(cls)
            if( len(name.split('\''))>1 ):
                name = name.split('\'')[1]
            if( len(name.split('.'))>1 ):
                name = name.split('.')[1]  
            combobox.addItem(name)
            self.addClassAvailable(cls,combobox)
            
            
class uiNewClassScriptTemplate(QtGui.QDialog):
    
    def __init__(self,scriptwidget):
        super(uiNewClassScriptTemplate, self).__init__()
        self.scriptWidget = scriptwidget
        self.initUI()
        
    def initUI(self):
        
        self.vbox = QtGui.QVBoxLayout()
        gridBox = QtGui.QGridLayout()
        
        gridBox.addWidget(QtGui.QLabel("Class name:"),1,1)
        self.classNameEdit = QtGui.QLineEdit("myclass")
        gridBox.addWidget(self.classNameEdit,1,2)
        gridBox.addWidget(QtGui.QLabel("Parent class:"),2,1)
        self.parentclassName = QtGui.QComboBox()
        self.parentclassName.addItem("msTreeMapper")
        self.addClassAvailable(msTreeMapper,self.parentclassName)
        gridBox.addWidget(self.parentclassName,2,2)
        
        gridBox.addWidget(QtGui.QLabel("default variable name"),3,1)
        self.defaultVarName = QtGui.QLineEdit("default_VarName")
        gridBox.addWidget(QtGui.QLabel("default Id"),4,1)
	self.defaultId = QtGui.QLineEdit("default_Id")
	gridBox.addWidget(self.defaultVarName,3,2)
	gridBox.addWidget(self.defaultId,4,2)
	
	
        self.vbox.addLayout(gridBox)
        self.doc = QtGui.QTextEdit("myclass documentation")
        self.vbox.addWidget(self.doc)
   
        hbox2 = QtGui.QHBoxLayout()
        button = QtGui.QPushButton("Add child")
        button.clicked.connect(self.addChild)
        hbox2.addWidget(button)
        hbox2.addStretch()
        self.vbox.addLayout(hbox2)
        
        hbox2 = QtGui.QHBoxLayout()
        button = QtGui.QPushButton("Ok")
        button.clicked.connect(self.ok)
        hbox2.addWidget(button)
        hbox2.addWidget(QtGui.QPushButton("Cancel"))
        
        vbox2 =  QtGui.QVBoxLayout()
        vbox2.addLayout(self.vbox)
        vbox2.addLayout(hbox2)
        
        self.setLayout(vbox2)
        self.childGroupBoxes=[]
        self.args=[]
        self.argsType=[]
        self.argsdoc=[]
        
    def addChild(self):
        v = ChildGroupBox()
        self.childGroupBoxes.append( v)
        self.vbox.addWidget(v)
        
        
    def addClassAvailable(self,classebase,combobox):
        
        if(len(classebase.__subclasses__())==0):
            return
            
        for cls in classebase.__subclasses__():
            name =  str(cls)
            if( len(name.split('\''))>1 ):
                name = name.split('\'')[1]
            if( len(name.split('.'))>1 ):
                name = name.split('.')[1]  
            combobox.addItem(name)
            self.addClassAvailable(cls,combobox)
            
    def ok(self):
        
        f = open('../python/ScriptTemplates/newClassTemplate.py', 'r')
        template = str(f.read())
        
        argsString =""
        argsDoc = ""
        argsComp=""
        i = 0
        for arg in self.args:
            argsString = argsString + arg.text()          
            argsDoc = argsDoc + arg.text() + " : " + self.argsdoc[i].text()
            argsComp = argsComp + arg.text() + "(" +self.argsType[i].currentText()+ ")"
            if (arg!= self.args[-1]):
                argsString = argsString + " , "
                argsDoc = argsDoc + "\n        "
                argsComp = argsComp + " , "
                
        d = dict(classname=self.classNameEdit.text(),parentclass=self.parentclassName.currentText(),classdoc=self.doc.toPlainText(),
                  date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M"),defaultVarName = self.defaultVarName.text(),defaultId = self.defaultId.text() )
        
        self.scriptWidget.addTab(datetime.datetime.now().strftime("%H:%M")+"*")   
        #text = QtGui.QTextEdit()
        text = self.scriptWidget.currentScript()
        text.filename=""
        #self.scriptWidget.tabWidget.addTab(text,"untittled*")    
        for line in template.splitlines():
            string = Template(line)
            text.append(string.safe_substitute(d))

	classname=self.classNameEdit.text()
	
        for gb in self.childGroupBoxes:
            var = "self."+gb.lineEdit.text()
            type = gb.className.currentText()
            text.append("        "+var+"= msChild_"+type+".New()")
            text.append("        self._declareChild_("+var+","+type+".New(),\""+gb.lineEdit.text()+"\")")
            text.append("")           
            
        for gb in self.childGroupBoxes:
            var = gb.lineEdit.text()
            type = gb.className.currentText()
            function = "get"+var
            text.append("    def get"+var+"(self):")
            text.append("        \"\"\" "+function +"( ("+classname+") self ) -> "+type +":")
	    text.append("        return the "+var +" child. \"\"\" ")
            text.append("        return self."+var+".getSharedPtr()")
            text.append("")
            
        for gb in self.childGroupBoxes:
            var = gb.lineEdit.text()
            type = gb.className.currentText()
            function = "set"+var
            text.append("    def set"+var+"(self, var ):")
            text.append("        \"\"\" "+function +"( ("+classname+") self , ("+type+") arg2 ) -> msTreeMapper :")
	    text.append("        set the "+var +" child. \"\"\" ")
            text.append("        self._updateChild_(self."+var+" , var)")
            text.append("        return self")
            
        self.close()    
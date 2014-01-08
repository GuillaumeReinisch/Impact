# -*- coding: utf-8 -*-
"""
Created on Sun Aug 25 14:22:22 2013

@author: chico
"""


import sys
from PySide import QtCore, QtGui, QtWebKit
from libimpact import *
import inspect
from string import Template
import datetime

class uiNewGraphicScriptTemplate(QtGui.QDialog):
    
    def __init__(self,scriptwidget):
        super(uiNewGraphicScriptTemplate, self).__init__()
        self.scriptWidget = scriptwidget
        self.initUI()
        
    def initUI(self):
        
        vbox = QtGui.QVBoxLayout()
        
        hbox0 = QtGui.QHBoxLayout()
        hbox0.addWidget(QtGui.QLabel("Widget for object:"))
        self.parentclassName = QtGui.QComboBox()
        self.parentclassName.addItem("msTreeMapper")
        self.addClassAvailable(msTreeMapper,self.parentclassName)
        hbox0.addWidget(self.parentclassName) 
       
        vbox.addLayout(hbox0)
        
        hbox2 = QtGui.QHBoxLayout()
        button = QtGui.QPushButton("Ok")
        button.clicked.connect(self.ok)
        hbox2.addWidget(button)
        hbox2.addWidget(QtGui.QPushButton("Cancel"))
        vbox.addLayout(hbox2)
        
        self.setLayout(vbox)
        
        self.args=[]
        self.argsType=[]
        self.argsdoc=[]
        
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
        
        f = open('../python/ScriptTemplates/newUiTemplate.py', 'r')
        template = str(f.read())
                        
        d = dict(classname=self.parentclassName.currentText()+"Widget",
		 classname2=self.parentclassName.currentText(),
                 date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M") )
        
        self.scriptWidget.addTab(datetime.datetime.now().strftime("%H:%M")+"*")   
        #text = QtGui.QTextEdit()
        text = self.scriptWidget.currentScript()
        
        #self.scriptWidget.tabWidget.addTab(text,"untittled*")    
        for line in template.splitlines():
            string = Template(line)
            text.append(string.safe_substitute(d))
        self.close()
        self.container.close()
        

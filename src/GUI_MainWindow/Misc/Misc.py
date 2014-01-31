# -*- coding: utf-8 -*-
"""
Created on Fri Sep  6 09:34:59 2013

@author: chico
"""
from PySide import QtCore, QtGui, QtWebKit
import logging
from libimpact import *
import re
from os import listdir
from os.path import isfile, join

class msPushButtonWithDoc(QtGui.QPushButton):
    def __init__(self,text,fct,obj,htmlView):
        super(msPushButtonWithDoc, self).__init__(text)
        self.setMouseTracking(True)
        self.method = fct
        self.myobject = obj
        self.htmlView = htmlView
        
    def mouseMoveEvent(self,event):
        
        self.htmlView.loadFileAndFocus(self.myobject,self.method) 
        
class msDoxygenHtmlView(QtWebKit.QWebView):
    def __init__(self,mainwindow):
        super(msDoxygenHtmlView, self).__init__()
        self.setMouseTracking(True)
        self.method = ""
        self.myObject =""
        self.docDirectory = mainwindow.config["DOC_INSTALL_PREFIX"]+"/html"
        if not self.docDirectory[-1]=="/":
	    self.docDirectory = self.docDirectory+"/"
	    
    def loadFile(self,myobject):
        
        if(self.myObject==myobject and self.method=="details"):
	    return
	  
        name = self.getDoxygenName(myobject.getType()).split(':')[-2]
        files = [ f for f in listdir(self.docDirectory) if isfile(join(self.docDirectory,f)) ]
        for file in files:
            if str( str(name)+".html") in file:
	        self.pathFileLoaded = self.docDirectory + file
                self.load(self.docDirectory+file+"#details")
                self.myObject = myobject
                self.method = "details"
    
    def loadFileAndFocus(self,myobject,method):
        
        if(self.myObject==myobject and self.method==method):
	    return
	  
        name = self.getDoxygenName(myobject.getType()).split(':')[-2]
        files = [ f for f in listdir(self.docDirectory) if isfile(join(self.docDirectory,f)) ]
        for file in files:
            if str( str(name)+".html") in file:
	        fileDoc = open(self.docDirectory+file, "r" )
                for line in fileDoc:
                    if str(">"+method+"</a>") in line:
                        line = line[0:line.find(str(">"+method+"</a>"))]
                        line =line.split("href=\"")[-1]
                        line = line.split("\"")[0]
                        self.load(self.docDirectory+line)
                        self.myObject = myobject
                        self.method = method
                        return	  
	  
    def getDoxygenName(self,name):
        s1 = re.sub('(.)([A-Z][a-z]+)',r'\1_\2',name)
        return re.sub('([a-z0-9])([A-Z])',r'\1_\2',s1).lower()
      
        
class QtStreamHandler(logging.Handler):
    def __init__(self,  parent,  main):
        logging.Handler.__init__(self)
        self.parent = parent
        self.main = main
       
        self.textWidget = parent
        self.formater =logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')

    def emit(self,record):
        
        item = self.formater.format(record)
        self.textWidget.append(self.formater.format(record))
       

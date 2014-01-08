"""
Created on Fri Sep  6 09:37:01 2013

@author: chico
"""
import sys, os
from PySide import QtCore, QtGui, QtWebKit
from libimpact import *
from widgets import *
import inspect

class WidgetsContainer(QtGui.QWidget):
    
    def __init__(self,mainwin):
        super(WidgetsContainer, self).__init__()
        
        self.mainWindow   = mainwin
        mainBox = QtGui.QVBoxLayout()
        
        self.scrollArea = QtGui.QScrollArea()

        self.widgetsGroup = QtGui.QGroupBox()  
        self.vbox         = QtGui.QVBoxLayout()
        self.widgetsGroup.setLayout(self.vbox)   
        self.scrollArea.setWidget(self.widgetsGroup)        
        
        mainBox.addWidget(self.scrollArea)
        
        self.setLayout(mainBox)
        self.scrollArea.resize(700,1)
        
    def changeTab(self,i):
        
        if(i==0):
            self.MainWindowContainer.createMenus()
        if(i==1):
            self.MainWindowContainer.createMenusScript()
             
    def sizeHint(self):
        return QtCore.QSize(700,self.scrollArea.height())
                 
    def update(self,myobject):       
        """
        update( self, (msTreeMapper)myobject ) -> void       
        load all the widget available associated to an object pointer
        
        myobject : pointer to the msTreeMapper derived object
        
        """
        msLogger.enterFunction("void WidgetsContainer::update(self,myobject)","")
        
        self.clear()
        self.widgets = []
        types = myobject.getType().split(':')
        self.width=0
        self.height=0

        mapClasses={}
        for cls in msWidget.msWidget.__subclasses__():
            mapClasses[ str(cls).split("'")[-2].split(".")[-1] ] =  cls 
        
        for thetype in types[0:-1]:
            widget=0
            thetype = thetype[:]+"Widget"
            msLogger.write(DEBUG,"create widget of type "+thetype)
            try:
                if( thetype in mapClasses ):
                    widget = mapClasses[thetype](myobject,self.mainWindow)
                    msLogger.write(DEBUG,"widget of type "+thetype+ " created")
                    self.width  = max(self.width,widget.size().width())
                    self.height = self.height + widget.size().height()
                else:
                    msLogger.write(WARNING, "Widget "+thetype+" does not exist")
            except:
                self.mainWindow.reportException(sys)
                self.addWidgetsInLayout()
                msLogger.exitFunction("void WidgetsContainer::update(self,myobject)")
                return
                #try something with python logging
                #pass
            if widget:
                self.widgets.append(widget)       

        self.addWidgetsInLayout()
        msLogger.exitFunction("void WidgetsContainer::update(self,myobject)")
    
    def addWidgetsInLayout(self):
        a=100
        self.widgetsGroup.setMinimumSize(self.width+a,self.height+a)
        self.widgetsGroup.setMaximumSize(self.width+a,self.height+a)
        self.vbox.setSpacing(0)
        for widget in self.widgets:
            self.vbox.addWidget( widget, QtCore.Qt.AlignLeft )


    def clear(self):
        b = self.vbox.takeAt(0)
        while b:
            if(len(self.widgets)>0):
                self.widgets.pop(0)
            b.widget().deleteLater()
            b = self.vbox.takeAt(0)
            

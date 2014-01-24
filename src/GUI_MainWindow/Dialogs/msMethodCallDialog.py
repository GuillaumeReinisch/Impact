"""
Created on Tue Jul 23 12:54:50 2013

@author: chico
"""


import sys
from PySide import QtCore, QtGui, QtWebKit
from libimpact import *
import inspect
import re
from TreeModels.TreeMapperModel import *
import functools
import unicodedata
import thread

def getArgsType(docstr):
    
    definition = docstr.split(':')[0]
    arguments=[]
    arguments.append(definition)
    if(',' in definition):
        arguments=definition.split(',')
    result=[]
    for argument in arguments:
        arg = re.search(r"\((\w+)\)",argument)
        try:
            result.append(arg.group(1))
        except:
            pass
    return result
        

class msMethodCallDialog(QtGui.QDialog):
    
    def __init__(self,method,parent,mainwindow):
        super(msMethodCallDialog, self).__init__()
        self.method = method  
        self.parent = parent
        self.isTreeViewReturnDisplayed=0
        self.isreturn = 0
        self.arguments = []
        self.mainwindow = mainwindow
        self.initUI()
        
    def initUI(self):
        vbox = QtGui.QVBoxLayout()
        
        vbox.addWidget(QtGui.QLabel("Documentation: " + inspect.getdoc(self.method) ))
        
        """ Create the arguments groupbox
        """        
        arglist =getArgsType(inspect.getdoc(self.method))
        
        start=0
        if(str(type(self.method))=="<type 'instancemethod'>"):
            start=1
        
        if len(arglist)>start:
            self.groupboxArgs = QtGui.QGroupBox("Arguments")
            vboxargs = QtGui.QVBoxLayout()
            
            i = 0
            for arg in arglist[start:]:
                hbox = QtGui.QHBoxLayout()
                hbox.addWidget( QtGui.QLabel("Argument of type: " + arg) )
                
                vboxargs.addLayout(hbox)
                if 'str' in arg:
                    textarg = QtGui.QTextEdit()
                    filepathBttn = QtGui.QPushButton("insert file path")
                    fct=functools.partial(self.insertFilePath, textarg )
                    hbox.addWidget(filepathBttn)
                    filepathBttn.clicked.connect(fct)
                    vboxargs.addWidget(textarg)
                    fct=functools.partial(self.addArgumentFromString, textarg.toPlainText ,i)
                    self.arguments.append("")
                    textarg.textChanged.connect(fct)
                    
                elif 'int' in arg:
                    intarg = QtGui.QSpinBox()
                    vboxargs.addWidget(intarg)
                    fct=functools.partial(self.addArgumentFromInt, intarg.value , i )
                    self.arguments.append(0)
                    intarg.valueChanged.connect(fct)
                
                elif 'float' in arg:
                    floatarg = QtGui.QLineEdit("0")
                    vboxargs.addWidget(floatarg)
                    fct=functools.partial(self.addArgumentFromFloat, floatarg.text , i )
                    self.arguments.append(0)
                    floatarg.textChanged.connect(fct)
                
                else:
                    newBttn = QtGui.QPushButton("New object")
                    hbox.addStretch(1)
                    hbox.addWidget(newBttn)
                    treeview = QtGui.QTreeView()
                    fct=functools.partial(self.clickNew, treeview, hbox, arg, i )
                    newBttn.clicked.connect(fct)
                    
                    vboxargs.addWidget(treeview)
                    model = TreeMapperModel(self.parent,arg)
                    treeview.setModel(model)
                    fct=functools.partial(self.addArgumentFromTreeview,treeview,i)
                    treeview.clicked.connect(fct)
                    self.arguments.append(None)
                    
                i = i+1
                
            self.groupboxArgs.setLayout(vboxargs)
            vbox.addWidget(self.groupboxArgs)
        
        """ Create the return GroupBox
        """
        if( not '-> None' in inspect.getdoc(self.method)):
            
            self.groupboxReturn = QtGui.QGroupBox("Return")
            
            self.vboxreturn = QtGui.QVBoxLayout()
                        
            if( '-> str' in inspect.getdoc(self.method)):
                self.vboxreturn.addWidget(QtGui.QLabel("result returned in out stream"))
            else:
                hbox=QtGui.QHBoxLayout()
                hbox.addWidget(QtGui.QLabel("msTreeMapper return type, options"))
                
                vboxopt=QtGui.QVBoxLayout()
                self.radioIgnore=QtGui.QRadioButton("Ignore")
                self.radioIgnore.setChecked(1)
                self.radioIgnore.clicked.connect(self.hideTreeToAddChild)
                vboxopt.addWidget(self.radioIgnore)
                
                self.radioAdd=QtGui.QRadioButton("Add as new child")
                self.radioAdd.clicked.connect(self.displayTreeToAddChild)
                vboxopt.addWidget(self.radioAdd)
                
                hbox.addLayout(vboxopt)
                
                self.vboxreturn.addLayout(hbox)
                
            self.groupboxReturn.setLayout(self.vboxreturn)
            vbox.addWidget(self.groupboxReturn)
            
        
	hbox = QtGui.QHBoxLayout()
	hbox.addStretch(1)
	self.checkBoxThread = QtGui.QCheckBox("execute in new thread")
	hbox.addWidget(self.checkBoxThread)
        button = QtGui.QPushButton("execute")
        button.setDefault(1)
        button.setMaximumWidth(200)
        button.clicked.connect(self.execute)
        hbox.addWidget(button)
        vbox.addLayout(hbox)
        
        if inspect.getdoc(self.method).find("New")>=0:
            self.radioAdd.setChecked(1)
            self.displayTreeToAddChild()
        self.setLayout(vbox)
    
    
    def findInClass(self, cls0, className ):
      
	for cls in cls0.__subclasses__():
	        name = str(cls).split('.')[1].split('\'')[0]
            
                if(name == className ):
                    self.class0 =cls
                    
		self.findInClass(cls,myobject)
		
    def addDerivedClass(self, cls0, combo):
      
	for cls in cls0.__subclasses__():
	    name = str(cls).split('.')[1].split('\'')[0]
            combo.addItem(name)
            self.addDerivedClass(cls,combo)
		
    def clickNew(self, treeview, hbox, className, i):
      
        combo = QtGui.QComboBox()
        try:
	    self.class0 = globals()[className]
	except:
            self.findInClass(msTreeMapper,className)
        combo.addItem(className) 
        self.addDerivedClass(self.class0,combo)
        hbox.addWidget(combo)
        treeview.setVisible(0)
        self.addArgumentFromNew(i,combo,0)
        fct=functools.partial(self.addArgumentFromNew,i,combo)
        combo.currentIndexChanged.connect(fct)
    
    def addArgument(self, getdata, i ):
        self.arguments[i]=getdata()
   
    def addArgumentFromString(self, getdata, i ):
        self.arguments[i]=str(getdata())

    def addArgumentFromInt(self, getdata, i , a):
        self.arguments[i]=int(getdata())

    def addArgumentFromFloat(self, getdata, i , a):
        self.arguments[i]=float(getdata())
    
    def addArgumentFromTreeview(self, treeview , i , a):
        self.arguments[i] =treeview.currentIndex().internalPointer().getRawdata()

    def addArgumentFromNew(self, i , combo, a ):
        
        className = combo.currentText()
        try:
	    self.class0 = globals()[className]
	except:
            self.findInClass(msTreeMapper,className)
        
        self.arguments[i] =self.class0.New()
        
    def insertFilePath(self, textedit):

        filename, filtr = QtGui.QFileDialog.getOpenFileName(self,
                                                    "Choose a file name", '.')
        textedit.append(filename)

    def displayTreeToAddChild(self):
        if( not self.isTreeViewReturnDisplayed):
            
            self.NameVariable = QtGui.QLineEdit()
            hbox = QtGui.QHBoxLayout()
            hbox.addWidget(QtGui.QLabel("Nom de la variable"))
            hbox.addWidget(QtGui.QPushButton("check reserved variable name"))
            self.vboxreturn.addLayout(hbox)
            self.vboxreturn.addWidget(self.NameVariable)
            
            self.vboxreturn.addWidget(QtGui.QLabel("Select parent object"))
            self.treeviewReturn = QtGui.QTreeView()
            model = TreeMapperModel(self.parent,"msTreeMapper")
            self.treeviewReturn.setModel(model)
            
            self.vboxreturn.addWidget(self.treeviewReturn)
            self.isTreeViewReturnDisplayed=1
    
            modelIndex = self.treeviewReturn.indexBelow(self.treeviewReturn.rootIndex())
            selection = self.treeviewReturn.selectionModel()
            selection.select(modelIndex,QtGui.QItemSelectionModel.Select)
        
        
        self.NameVariable.setFocus()
        self.isreturn = 1
         
    def hideTreeToAddChild(self):
        self.isreturn = 0
        if( self.isTreeViewReturnDisplayed):
            self.treeviewReturn.setVisible(0)
            
    def execute(self):
        msLogger.enterFunction("void msMethodCallDialog::execute(self)","")
        
        try:
            if(len(self.arguments)==0):
	        if(self.checkBoxThread.isChecked()):
		    a = thread.start_new_thread( self.method, ("Thread-1", ) )
		else:
                    a = self.method()
                    
            if(len(self.arguments)>0):
	        if(self.checkBoxThread.isChecked()):
		    a = thread.start_new_thread( self.method, ("Thread-1", self.arguments , ) )
		else:
                    a = self.method(*self.arguments)
                    
            if(self.isreturn):
                if(str(self.NameVariable.text())==""):
                    ret=QtGui.QMessageBox.critical(self,'An exception occured',
                                           'You need to set the variable name', 
                                           QtGui.QMessageBox.Yes,
                                           QtGui.QMessageBox.Yes)
                    return
                try:
                    self.treeviewReturn.currentIndex().internalPointer().getRawdata().addChild(a,str(self.NameVariable.text()))
                except:
                    ret=QtGui.QMessageBox.warning(self,'Warning',
                                           'No parent object selected, root object used',
                                           QtGui.QMessageBox.Yes,
                                           QtGui.QMessageBox.Yes)
                    self.treeviewReturn.model().rootData.addChild(a,str(self.NameVariable.text()))
    
            msLogger.write(DEBUG,"method called successfully, create the new tree model")
            model = TreeMapperModel(self.mainwindow.rootObject,"msTreeMapper")
            self.mainwindow.mainTreeWidget.setModel(model)
            msLogger.write(DEBUG,"reload the main window's data structure")
            self.mainwindow.reload()
        
        except:
            message = str(sys.exc_info())
            self.mainwindow.reportException(sys.exc_info())
        msLogger.exitFunction("void msMethodCallDialog::execute(self)")
        self.close()
        
        
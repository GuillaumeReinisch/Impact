
from libimpact import *
from PySide import QtCore, QtGui
from TreeModels.TreeMapperModel import *
import re
from TreeModels.simpletreemodel import *

class DebugTreeModel(TreeModel):
    
    def __init__(self, data):
        super(DebugTreeModel, self).__init__(None)
        """ (msLogger) data : Logger to represent
        """
        self.rootItem = TreeItem(("Function","Type","Priority","Time","Duration"))
        self.level = 0
        self.addDataChildren(data,self.rootItem)

    def addDataChildren(self,data,parent):
        
        for child in data:
            
            textData=[]
            function = child.getFunctionName().split("(")[0].split(" ")[-1]
            type =""
            if function.find("::")>=0:
	        if len(function.split("::"))>1:
                   type = function.split("::")[-2]
                   function = function.split("::")[-1]

            textData.append(function)
            textData.append(type)
            textData.append(child.getPriority().replace("\n",""))
            textData.append(child.getTime().split(" ")[3])
            textData.append(child.getTimeElapsed())
            item=TreeItem(textData,parent)
            item.setRawdata(child)
            parent.appendChild(item)
            self.level = self.level + 1
            try:
               self.addDataChildren(child.getFunctionsCalled(),parent.child(parent.childCount()-1))
            except:
               print "can not add children in level ",self.level
            self.level = self.level - 1
            


class DebugTreeWidget(QtGui.QTreeView):
    
    def __init__(self, logger, debugAttributs):
        super(DebugTreeWidget, self).__init__()
        self.logger=logger
        #self.clicked.connect(self.clickDebugTreeView)
        self.debugAttributs = debugAttributs

    def update(self):
        self.setModel(DebugTreeModel(self.logger.getTreeRepresentation() ))

    def keyPressEvent(self,e):
        super(DebugTreeWidget, self).keyPressEvent(e)
        self.clickDebugTreeView()

    def addChildMessages(self,logElement,level):
        
        childsFunction = logElement.getFunctionsCalled()
        
        for child in childsFunction:
	  
	    prefix = ""
	    for i in range(0,level):
	        prefix = prefix + "    "
	        
	    for elem in child.getMessages():
	        
	        if 'DEBUG' in elem:
                    self.debugAttributs.setTextColor(QtCore.Qt.darkYellow)
                if 'INFO' in elem:
                    self.debugAttributs.setTextColor(QtCore.Qt.darkGreen)
                    
                #self.debugAttributs.append(prefix + elem.split(type)[1]) 
                self.debugAttributs.append(prefix + elem )  
                self.addChildMessages(child, level + 1 )
      
    def clickDebugTreeView(self):
        """ action executed when the debug tree view is clicked (i.e. an object
        selected). The associated text list is displayed in the self.debugAttributs QTextEdit
        """
        data = self.currentIndex().internalPointer().getRawdata()
        self.debugAttributs.clear()
        type = "none"
        for elem in data.getMessages():
            if 'DEBUG' in elem:
                self.debugAttributs.setTextColor(QtCore.Qt.darkYellow)
            if 'INFO' in elem:
                self.debugAttributs.setTextColor(QtCore.Qt.darkGreen)    
            self.debugAttributs.append(elem)
        self.debugAttributs.append("")
        self.addChildMessages(data,1)

class DebugWidget(QtGui.QWidget):
    
    def __init__(self, logger, mainWindows):
        super(DebugWidget, self).__init__()
        self.logger=logger
        self.mainWindows = mainWindows

        mainvbox = QtGui.QVBoxLayout()
        hbox = QtGui.QHBoxLayout()
        updateBttn = QtGui.QPushButton("update")
        updateBttn.clicked.connect(self.update)
        hbox.addWidget(updateBttn)
        hbox.addWidget(QtGui.QPushButton("clear"))
        hbox.addStretch(1)
        mainvbox.addLayout(hbox)
        filterGroup = QtGui.QGroupBox("filter")
        vboxfilter  = QtGui.QVBoxLayout()

        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QCheckBox("Variable"))
        hbox.addWidget(QtGui.QCheckBox("Function"))
        hbox.addWidget(QtGui.QCheckBox("Text"))
        hbox.addStretch(1)
        vboxfilter.addLayout(hbox)
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QLineEdit("text to search"))
        hbox.addWidget(QtGui.QPushButton("ok"))
        vboxfilter.addLayout(hbox)
        filterGroup.setLayout(vboxfilter)

        mainvbox.addWidget(filterGroup)
        
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QLabel("Debug verbosity:"))    
        self.comboLoggerPriority = QtGui.QComboBox()
        self.comboLoggerPriority.addItem( "Warning" )
        self.comboLoggerPriority.addItem( "Debug" )
        self.comboLoggerPriority.addItem( "Info" )
        self.comboLoggerPriority.currentIndexChanged.connect(self.updatePriority)
        hbox.addWidget(self.comboLoggerPriority )
        self.valueSpinBox = QtGui.QSpinBox()
        self.valueSpinBox.valueChanged.connect(self.updateLogLevelMax)
        self.valueSpinBox.setValue(3)
        hbox.addWidget(self.valueSpinBox)
        self.treeRepresentationBttn = QtGui.QPushButton("turn off debug tree")
        self.treeRepresentationBttn.clicked.connect(self.changeTreeRepresentationState)
        hbox.addWidget(self.treeRepresentationBttn)
        hbox.addStretch(1)
        mainvbox.addLayout(hbox)
        
        self.debugAttributs = QtGui.QTextEdit()
        self.debugTreeWidget = DebugTreeWidget( logger , self.debugAttributs)
        splitter =  QtGui.QSplitter()
        splitter.setOrientation(QtCore.Qt.Vertical)
        splitter.addWidget(self.debugTreeWidget)
        splitter.addWidget(self.debugAttributs)
        mainvbox.addWidget(splitter)

        self.setLayout(mainvbox)

    def update(self):
        self.debugTreeWidget.update()
  
    def updateLogLevelMax(self):
        self.mainWindows.Logger.setVerboseLevel(self.valueSpinBox.value())

    def updatePriority(self):

	print self.comboLoggerPriority.currentText()
	
        if self.comboLoggerPriority.currentText() == "Warning":
	    self.mainWindows.Logger.setPriorityMin(Priority.WARNING)
	    print "Logger set to WARNING"
	    
	if self.comboLoggerPriority.currentText() == "Debug":
	    self.mainWindows.Logger.setPriorityMin(Priority.DEBUG)
	    print "Logger set to DEBUG"
	    
	if self.comboLoggerPriority.currentText() == "Info":
	    self.mainWindows.Logger.setPriorityMin(Priority.INFO)
	    print "Logger set to INFO"
	    
	    
    def changeTreeRepresentationState(self):
        if self.treeRepresentationBttn.text() == "turn off debug tree" :
	    self.mainWindows.Logger.stopTreeRepresentation()
	    self.treeRepresentationBttn.setText("turn on debug tree")
	    return
	if self.treeRepresentationBttn.text() == "turn on debug tree" :
	    self.mainWindows.Logger.startTreeRepresentation()
	    self.treeRepresentationBttn.setText("turn off debug tree")
	    return 
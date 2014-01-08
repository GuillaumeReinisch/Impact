from libimpact import *
from PySide import QtCore, QtGui
from widgets.msWidget import *
from Dialogs.queryTreeMapperDialog import *
import inspect
import itertools
from TreeModels.simpletreemodel import *

class SamplingDataModel(TreeModel):
    def __init__(self, data, parent=None):
        super(SamplingDataModel, self).__init__(parent)
       
        textData=[]
        
        for q in data.getCoordinates().getVariables():
	    textData.append(q.getId())
	    
	textData.append("value")
        
        self.rootItem = TreeItem(textData)
        
        self.addDataChildren( data, self.rootItem )
    
    def addDataChildren(self,data,parent):
               
        for conf in data.getConfigurations():
	  
	    textData=[]
	    	     
	    for q in conf.getConfiguration():
		textData.append(q)
	    
	    for value in conf.getValues():
		textData.append(str(value.key()) +":"+ str(conf.getValues()[value.key()]))
                 
            item=TreeItem(textData,parent)
            parent.appendChild(item)
        
        return data


class msSamplerWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super( msSamplerWidget, self).__init__(data, mainwindows)
        
        hbox=QtGui.QHBoxLayout()
        bttn = QtGui.QPushButton("Sample")
        bttn.clicked.connect(self.studyFct)
        hbox.addWidget(bttn)
        bttn = QtGui.QPushButton("scalar product")
        bttn.clicked.connect(self.scalarProduct)
        hbox.addWidget(bttn)
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)
        
        self.groupbox.setTitle("Sampler")
        self.vbox.addSpacing(10)
        self.vbox.addWidget(QtGui.QLabel("Configurations recorded") )
        
        self.DataList = QtGui.QTreeView()
        model = SamplingDataModel(self.data)
        
        self.DataList.setModel(model)
        self.vbox.addWidget(self.DataList)
        
        self.setMinimumSize(550, 400)
        self.setMaximumSize(550,400)
    
    def studyFct(self):
        
        #widget = queryTreeMapperDialog("select the scalar function","select the scalar function in the tree","msScalarFunction", self.mainwindows.rootObject,  self.data.sample , self)
        #widget.exec_()
        self.returnVar = self.data.sample()
        #model = SamplingDataModel(self.data)
        #self.DataList.setModel(model)
        self.mainwindows.reload()
    
    def scalarProduct(self):
        pass

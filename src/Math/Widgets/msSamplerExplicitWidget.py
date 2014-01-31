"""
    Created on Sat Aug 24 16:39:31 2013
    
    @author: chico
    """

from libimpact import *
from PySide import QtCore, QtGui
from widgets.msWidget import *
from TreeModels.TreeMapperModel import *
import matplotlib.pyplot as plt
from Dialogs.simpleDialogs import *
from TreeModels.simpletreemodel import *

class SampleTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(SampleTreeModel, self).__init__(parent)
        
        self.sampler = data
        self.coors   = data.getCoordinates()
        textData=[]
        textData.append("index")
        self.variables = self.coors.getVariables()
        for v in self.variables:
            textData.append(v.getId())
            
        self.rootItem = TreeItem(textData)
        self.addDataChildren(data,self.rootItem)
    
    def addDataChildren(self,data,parent):
        
        index = 0
        self.sampler.setSampleIndex(0)
        while self.sampler.getNextPoint():
            
            textData=[]
            textData.append(str(index))
            for v in self.variables:
                
                textData.append(str(v.getValue()))
            
            item=TreeItem(textData,parent)
                #item.setRawdata( child )
            parent.appendChild(item)
            index =index + 1
        return data

class msSamplerExplicitWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msSamplerExplicitWidget, self).__init__(data,mainwindows)
        
        """ First create the Units group box
            """
        hbox =QtGui.QHBoxLayout()
        self.labeIndex = QtGui.QLabel("current sample: "+str(data.currentSampleIndex() ))
        hbox.addWidget(self.labeIndex )
        hbox.addStretch(1)
        bttn = QtGui.QPushButton("set current sample")
        hbox.addWidget(bttn)
        bttn.clicked.connect(self.queryIndexSample)
        self.vbox.addLayout(hbox)
        index = data.currentSampleIndex()
        self.SampleList = QtGui.QTreeView()
        model = SampleTreeModel(self.data)
        self.SampleList.setModel(model)
        self.vbox.addWidget(self.SampleList)
        try:
            data.setSample(index)
        except:
	    pass
        self.groupbox.setTitle("&Explicit sampler")
        self.setMinimumSize(550, 250)
    
    def queryIndexSample(self):
        widget = queryIntDialog("Explicit sampler","enter the sample index:",  self.data.currentSampleIndex(), self.data.setSample )
        widget.exec_()
        self.labeIndex.setText( "current sample: "+str(self.data.currentSampleIndex() ))
        

    
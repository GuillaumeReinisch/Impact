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

class DiracsTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(DiracsTreeModel, self).__init__(parent)
        
        self.sampler = data.getSampler()
        self.coors   = data.getCoordinates()
        textData=[]
        textData.append("index")
        self.variables = self.coors.getVariables()
        for v in self.variables:
            textData.append(v.getId())
        textData.append("field value")
        self.rootItem = TreeItem(textData)
        self.addDataChildren(data,self.rootItem)
    
    def addDataChildren(self,data,parent):
        
        index = 0
        while self.sampler.getNextPoint():
            
            textData=[]
            textData.append(str(index))
            for v in self.variables:
                
                textData.append(str(v.getValue()))
            
            textData.append(data.evaluate())
            item=TreeItem(textData,parent)
                #item.setRawdata( child )
            parent.appendChild(item)
            index =index + 1
        return data

class msDiracsWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msDiracsWidget, self).__init__(data,mainwindows)
        
        """ First create the Units group box
            """
        self.DiracsList = QtGui.QTreeView()
        
        hbox =QtGui.QHBoxLayout()
        self.labeIndex = QtGui.QLabel("current sample: "+str(data.getSampler().currentSampleIndex() ))
        hbox.addWidget(self.labeIndex )
        hbox.addStretch(1)
        bttn = QtGui.QPushButton("set current sample")
        hbox.addWidget(bttn)
        bttn.clicked.connect(self.queryIndexSample)
        self.vbox.addLayout(hbox)
        
        model = DiracsTreeModel(self.data)
        self.DiracsList.setModel(model)
        
        self.vbox.addWidget(self.DiracsList)
        
        hbox =QtGui.QHBoxLayout()
        hbox.addStretch(1)
        bttn=QtGui.QPushButton("Plots")
        hbox.addWidget(bttn)
        self.vbox.addLayout(hbox)
        bttn.clicked.connect(self.plot1d)
        self.groupbox.setTitle("&Diracs function")
        self.setMinimumSize(550, 250)
    
    def queryIndexSample(self):
        widget = queryIntDialog("Explicit sampler","enter the sample index:",  self.data.getSampler().currentSampleIndex(), self.data.getSampler().setSampleIndex )
        widget.exec_()
        self.labeIndex.setText( "current sample: "+str(self.data.getSampler().currentSampleIndex() ))

    
    def plot1d(self):
        fig, axes = plt.subplots(1, 1)
        x=[]
        y=[]
        i = 0
        sampler = self.data.getSampler()
        
        while sampler.iterate():
            x.append(i-sampler.noOfSamples()/1000.)
            x.append(i)
            x.append(i+sampler.noOfSamples()/1000.)
            y.append(0)
            y.append(self.data.evaluate())
            y.append(0)
            i = i +1
        
        axes.plot(x,y,"b-")
        axes.set_xlabel("sample index" )
        axes.set_ylabel(self.data.getId() + " [" + self.data.getUnit().getStr() + "]")
        fig.show()
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

class MultiDiracsTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(MultiDiracsTreeModel, self).__init__(parent)
        
        self.sampler = data.getSampler()
        self.coors   = data.getCoordinates()
        textData=[]
        textData.append("index")
        self.variables = self.coors.getVariables()
        for v in self.variables:
            textData.append(v.getId())
        
        self.Diracs = data.getDiracsFunctions()
        
        for fct in self.Diracs:
            textData.append(fct.getId())
        
        self.rootItem = TreeItem(textData)
        self.addDataChildren(data,self.rootItem)
    
    def addDataChildren(self,data,parent):
        
        index = 0
        while self.sampler.getNextPoint():
            
            textData=[]
            textData.append(str(index))
            
            for v in self.variables:
                textData.append(str(v.getValue()))
            
            for fct in self.Diracs:
                textData.append(fct.evaluate())
            
            item=TreeItem(textData,parent)
            #item.setRawdata( child )
            parent.appendChild(item)
            index =index + 1
        return data

class msMultiDiracsWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msMultiDiracsWidget, self).__init__(data,mainwindows)
        
        """ First create the Units group box
            """
        self.DiracsList = QtGui.QTreeView()
        
        hbox =QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QLabel("current sample: "+str(data.getSampler().currentSampleIndex() )))
        hbox.addStretch(1)
        bttn = QtGui.QPushButton("set current sample")
        hbox.addWidget(bttn)
        bttn.clicked.connect(self.queryIndexSample)
        self.vbox.addLayout(hbox)
        
        model = MultiDiracsTreeModel(self.data)
        self.DiracsList.setModel(model)
        
        self.vbox.addWidget(self.DiracsList)
        
        hbox =QtGui.QHBoxLayout()
        hbox.addStretch(1)
        bttn=QtGui.QPushButton("Plot")
        hbox.addWidget(bttn)
        self.vbox.addLayout(hbox)
        bttn.clicked.connect(self.plot1d)
        self.groupbox.setTitle("&Multi diracs functions")
        self.setMinimumSize(500, 250)
    
    def queryIndexSample(self):
        widget = queryIntDialog("Explicit sampler","enter the sample index:",  self.data.getSampler().currentSampleIndex(), self.data.getSampler().setSampleIndex )
        widget.exec_()
        self.mainwindows.reload()
    
    
    def plot1d(self):
        fig, axes = plt.subplots(1, 1)
        xs=[]
        ys=[]
        i = 0
        sampler = self.data.getSampler()
        Diracs = self.data.getDiracsFunctions()
        
        for fct in Diracs:
            yi = []
            ys.append(yi)
            xi=[]
            xs.append(xi)
        
        while sampler.iterate():
            j=0
            for fct in Diracs:
                xs[j].append(i)
                ys[j].append(fct.evaluate())
                j = j+1
            
            i = i +1
        
        j=0
        for fct in Diracs:
            axes.plot(xs[j],ys[j])
            j = j+1
        
        axes.set_xlabel("sample index" )
        axes.set_ylabel("field values")
        fig.show()
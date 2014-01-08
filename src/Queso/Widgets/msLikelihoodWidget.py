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
        
        self.sampler = data.getMeasures().getSampler()
        self.coors   = data.getMeasures().getCoordinates()
        textData=[]
        textData.append("index")
        self.variables = self.coors.getVariables()
        for v in self.variables:
            textData.append(v.getId())
       
        self.Model = data.getPredictiveModel()
        self.Qois= self.Model.getQois()
        
        i = 0
        for qoi in self.Qois:
	    textData.append(qoi.key()+" exp.")
            textData.append(qoi.key()+" pred.")
            i = i +1
        
        self.rootItem = TreeItem(textData)
        self.addDataChildren(data,self.rootItem)
    
    def addDataChildren(self,data,parent):
        
        index = 0
        self.sampler.setCoordinates(self.coors)
        while self.sampler.getNextPoint():
	    
            self.Model.predict()
            
            textData=[]
            textData.append(str(index))
            
            for v in self.variables:
                textData.append(str(v.getValue()))
            
            self.Qois= self.Model.getQois()
            
            for qoi in self.Qois:
	        textData.append(data.getMeasures().getValueById(qoi.key()))
                textData.append(self.Qois[qoi.key()])

            item=TreeItem(textData,parent)
            #item.setRawdata( child )
            parent.appendChild(item)
            
            index =index + 1
           
        return data

class msLikelihoodWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msLikelihoodWidget, self).__init__(data,mainwindows)
        
        """ First create the Units group box
            """
        self.DiracsList = QtGui.QTreeView()
        
        model = MultiDiracsTreeModel(self.data)
        
        self.DiracsList.setModel(model)
        self.vbox.addWidget(self.DiracsList)

        bttn = QtGui.QPushButton("plot")
        bttn.clicked.connect(self.plot1d)
        self.vbox.addWidget(bttn)
        
        self.groupbox.setTitle("&Likelihood")
        self.setMinimumSize(550, 250)
        self.setMaximumSize(550, 250)
    
    def queryIndexSample(self):
        widget = queryIntDialog("Explicit sampler","enter the sample index:",  self.data.getSampler().currentSampleIndex(), self.data.getSampler().setSampleIndex )
        widget.exec_()
        self.mainwindows.reload()
    
    
    def plot1d(self):
      
        colors = "bgrcmykw"
   
        fig, axes = plt.subplots(1, 1)
        xs=[]
        ys=[]
        xsQoi = []
        ysQoi = []
        
        i = 0
        sampler = self.data.getMeasures().getSampler()
        Diracs = self.data.getMeasures().getDiracsFunctions()
        Model = self.data.getPredictiveModel()
        Coors = Model.getScenarioVariables()
        
        for fct in  Model.getQois():
            yi = []
            ys.append(yi)
            xi=[]
            xs.append(xi)
            xiQoi =[]
            xsQoi.append(xiQoi)
            yiQoi = []
            ysQoi.append(yiQoi)
        
        while sampler.getNextPoint():
            j=0
            Model.predict()
            QoIs = Model.getQois()
            
            for q in QoIs:
                xs[j].append(i)
                ys[j].append(self.data.getMeasures().getValueById(q.key()))
                j = j+1
            j=0
            
            for q in QoIs:
                xsQoi[j].append(i)
                ysQoi[j].append( QoIs[q.key()] )
                
                j = j+1
                
            i = i +1
        
        j=0
        for fct in QoIs:
            axes.plot(xs[j],ys[j],color=colors[j],linestyle="-.")
            j = j+1
        
        j = 0
        for q in QoIs:
            axes.plot(xsQoi[j],ysQoi[j],color=colors[j],linestyle="-")
            j = j+1
        
        axes.set_xlabel("sample index" )
        axes.set_ylabel("field values")
        fig.show()
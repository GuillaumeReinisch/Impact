""" 
Created the 2013-12-05 15:12 

@author:
"""

from libimpact import *
from PySide import QtCore, QtGui
from widgets.msWidget import *
from Dialogs.simpleDialogs import *

from math import *
import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.mlab import griddata

from widgets.msGeneralizedCoordinatesWidget import *
from widgets.msSamplerExplicitWidget import *

class msInverseProblemWidget(msWidget):
    
    """
    Widget for the class msInverseProblem
    """
    def __init__(self, data , mainwindow):
        super(msInverseProblemWidget, self).__init__(data,mainwindow)
        
        
        
        self.paramSpace = msGeneralizedCoordinatesWidget( self.data.getAllParametersContainer() , self.mainwindows )
        self.paramSpace.listView.clicked.connect(self.selectParam)
        self.vbox.addWidget(self.paramSpace)
        self.paramSpace.groupbox.setTitle("&Parameters space")
        try:
            self.paramSpace.widgetScalVar.slider.valueChanged[int].connect(self.updateParamChange)
            self.paramSpace.widgetScalVar.valueSpinBox.valueChanged[float].connect(self.updateParamChange)
        except:
            pass
        
        self.Sampler = msSamplerExplicitWidget( self.data.getLastChainSamples() , self.mainwindows )
        self.vbox.addWidget(self.Sampler)
        self.Sampler.groupbox.setTitle("&Sampler")
        
        hbox = QtGui.QHBoxLayout()
        self.plotSample2DBttn = QtGui.QPushButton("plot sample 2D")
        self.plotSample2DBttn.clicked.connect(self.plot2DClicked)
        hbox.addWidget(self.plotSample2DBttn)
        hbox.addStretch()
        self.vbox.addLayout(hbox)
        
        self.groupbox.setTitle("Inverse problem")
        
        self.LastChain  = data.getLastChainSamples()
        self.Parameters = data.getAllParametersContainer()
        self.setMinimumSize(600,500)
        
    def updateParamChange(self):
        self.data.updateParametersModification()
            
    def selectParam(self):
        #self.buttonPlot.setVisible(1)
        self.indiceParam = self.paramSpace.listView.currentIndex().row()
        self.param =self.paramSpace.listView.currentIndex().internalPointer().getRawdata()
            
    def plot2D(self,i,j):
      
        fig, axes = plt.subplots(1, 1)
     
        array = []
        labels=[]
        x = []
        y = []
        
        self.LastChain.setSample(0)
        while self.LastChain.getNextPoint():
	    x.append( self.Parameters.getVariable(j).getValue() )
	    y.append( self.Parameters.getVariable(i).getValue() )
     
        nbins = 50
        H, xedges, yedges = np.histogram2d(x,y,bins=nbins)
        while(H.max()>100):
            nbins = nbins + 1 
            H, xedges, yedges = np.histogram2d(x,y,bins=nbins)
  
        H = np.rot90(H)
        H = np.flipud(H)
        Hmasked = np.ma.masked_where(H==0,H) 
        axes.pcolormesh(xedges,yedges,Hmasked)
        axes.locator_params(nbins = 3)
        
        axes.set_xlabel("x")
        axes.set_ylabel("y")
        fig.show()
        
        
    def plot2DClicked(self):
        
        widget = query2IntDialog("Set the parameters indexes",self.plot2D )
        widget.Label1.setText("index 1")
        widget.Label2.setText("index 2")
        
        widget.exec_()
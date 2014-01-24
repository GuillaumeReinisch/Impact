"""
    Created on Sat Aug 24 15:38:04 2013
    
    @author: chico
    """

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from Dialogs.simpleDialogs import *
import inspect
import itertools
import re
import functools
from TreeModels.TreeMapperModel import *
from TreeModels.simpletreemodel import *

class CoeffRateTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(CoeffRateTreeModel, self).__init__(data,"msTreeMapper",parent)

        self.rootItem = TreeItem(("name",
                                  "value"))
        self.addDataChildren(data, self.rootItem )
        
    def addDataChildren(self,data,parent):
        
        a = data.getCoefficients()
        for i in a:
	    textData=[]
            textData.append( i.key() )
            textData.append( a[i.key()] )
            item=TreeItem(textData)
            item.setRawdata( i.key() )
            parent.appendChild(item)
            
        return data

class msReactionRateWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msReactionRateWidget, self).__init__(data, mainwindows)
        
        self.groupbox.setTitle("Reaction rate")
        
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget( QtGui.QLabel("Order:"),1,1)
        hbox.addWidget( QtGui.QLabel(str(data.getOrder()) ) )
        bttn =  QtGui.QPushButton("set")
        bttn.clicked.connect(self.setOrder)
        hbox.addWidget( bttn)
        self.vbox.addLayout(hbox)
        
        self.vbox.addWidget(QtGui.QLabel("Parameters:"))
        self.listCoeffs = QtGui.QTreeView()
        self.listCoeffs.setModel( CoeffRateTreeModel(data) )
	self.listCoeffs.clicked.connect(self.queryCoeff)
        self.vbox.addWidget(self.listCoeffs)
	
	self.vbox.addSpacing(20)
	hbox = QtGui.QHBoxLayout()
	self.labelRateTittle = QtGui.QLabel("rate(298 K) = ")
	hbox.addWidget(self.labelRateTittle)
	
	self.labelRateValue = QtGui.QLabel( str('{:.3e}'.format(data.computeRate(298)) ) )	
	hbox.addWidget(self.labelRateValue)
	hbox.addWidget(  QtGui.QLabel( data.getUnit().getStr() )  )
	self.vbox.addLayout(hbox)
	
	hbox = QtGui.QHBoxLayout()
	bttn = QtGui.QPushButton("compute")
	bttn.clicked.connect(self.queryT)
	hbox.addWidget(bttn)
	hbox.addStretch(1)
	self.vbox.addLayout(hbox)
	
        self.setMinimumSize(500,300 )
        self.setMaximumSize(500,300 )

    def computeRate(self,T):
        self.labelRateTittle.setText("rate("+str(T)+"K) = ")
        self.labelRateValue.setText( str('{:.3e}'.format(self.data.computeRate(T)) ) )	
        
    def queryT(self):
        widget = queryDoubleDialog("Temperature ?", "T [K] =",  298, self.computeRate )
        widget.exec_()
    
    def setCoeff(self,value):
        data = self.listCoeffs.currentIndex().internalPointer().getRawdata()
        self.data.setCoefficient(str(data), value )
    
    def queryCoeff(self):
        data = self.listCoeffs.currentIndex().internalPointer().getRawdata()
        widget = queryDoubleDialog("set coefficient ", str(data),  self.data.getCoefficients()[data], self.setCoeff )
        widget.exec_()
        self.mainwindows.reload()
        
    def setOrder(self):
        order = self.data.getOrder()
        widget = queryIntDialog("set the order","order: ", order, self.data.setOrder)
        widget.exec_()
        self.mainwindows.reload()
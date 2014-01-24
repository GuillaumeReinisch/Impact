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

class RateReactionTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(RateReactionTreeModel, self).__init__(data,"msTreeMapper",parent)

        self.rootItem = TreeItem(("index",
                                  "rate",
                                  "unit","order"))
        self.addDataChildren(data, self.rootItem )
        
    def addDataChildren(self,data,parent):
        i = 0
        for rate in data.getForwardRates():
            
            textData=[]
            textData.append(i)
            textData.append(rate.computeRate(data.getChemicalMixture().getTemperature()))
            textData.append(rate.getUnit().getStr())
            textData.append(rate.getOrder())
            item=TreeItem(textData)
            #item.setRawdata(att)
            parent.appendChild(item)
            i=i+1
            
        return data

class msReactionWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msReactionWidget, self).__init__(data, mainwindows)
        
        hbox = QtGui.QHBoxLayout()
        reactants = itertools.izip( data.getReactants(), data.getStoechiometryReactants() ,range(0,len(data.getReactants()) ) )
        for r,s,i in reactants:
	    hbox.addWidget(QtGui.QLabel(str(s)+" "+r.getId()))
	    if i < len(data.getReactants()) -1 :
	        hbox.addWidget(QtGui.QLabel("+"))
	        
	if data.getReversibility():       
	   hbox.addWidget(QtGui.QLabel("<->"))
	else:
	   hbox.addWidget(QtGui.QLabel("->"))
	   
	products = itertools.izip( data.getProducts(), data.getStoechiometryProducts(),range(0,len(data.getProducts()) ) )
        for p,s,i in products:
	    hbox.addWidget(QtGui.QLabel(str(s)+" "+p.getId()))
	    if i < len(data.getProducts()) -1 :
	        hbox.addWidget(QtGui.QLabel("+"))
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)
        
        self.vbox.addSpacing(10)
        
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget( QtGui.QLabel("Mixture's temperature:"))
        hbox.addWidget( QtGui.QLabel(str(data.getChemicalMixture().getTemperature())+" K") )
        bttn =  QtGui.QPushButton("set")
        bttn.clicked.connect(self.setTemperature)
        hbox.addWidget( bttn)
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)
        
        self.vbox.addSpacing(10)
        self.vbox.addWidget( QtGui.QLabel("Forward rates:"))
        self.listRates = QtGui.QTreeView()
        self.listRates.setModel( RateReactionTreeModel(data) )
        self.vbox.addWidget(self.listRates)
	
	self.groupbox.setTitle("Reaction")
	
        self.setMinimumSize(550,400 )
        self.setMaximumSize(550,400 )
        
        
    def setTemperature(self):
        T = self.data.getChemicalMixture().getTemperature()
        widget = queryDoubleDialog("set the temperature","enter the temperature in "+self.data.getUnits().getTempStr(),  T, 
				   self.data.getChemicalMixture().setTemperature )
        widget.exec_()
        self.mainwindows.reload()
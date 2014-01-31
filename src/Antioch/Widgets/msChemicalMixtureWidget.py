
from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from Dialogs.simpleDialogs import *
from TreeModels.simpletreemodel import *
from Misc.Misc            import *

import itertools
import functools
import sys, os

class SpeciesTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(SpeciesTreeModel, self).__init__(data,"msTreeMapper",parent)

        self.rootItem = TreeItem(("name",
                                  "mole fraction",
                                  "molar densities ["+data.getUnits().getStr(unitTypeVar.vQuantityDensity)+"]"))
        self.addDataChildren(data, self.rootItem )
        
    def addDataChildren(self,data,parent):
        
        tuple = itertools.izip(  data.getEntitiesNames(),
                                 data.getMoleFractions(),
                                 data.getMolarDensities() )
	n1 = 0
	n2 = 0
        for name,molFrac,molDens in tuple:
            
            textData=[]
            textData.append(name)
            textData.append(molFrac)
            textData.append(molDens)
            
            item=TreeItem(textData)
            parent.appendChild(item)
            n1 = n1 + molFrac
            n2 = n2 + molDens
            
        textData=[]
        textData.append("total")
        textData.append(n1)
        textData.append(n2)
        item=TreeItem(textData)
        parent.appendChild(item)     
        return data


class msChemicalMixtureWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msChemicalMixtureWidget, self).__init__(data, mainwindows)
        
        msLogger.enterFunction("msMicroscopicPhaseWidget::msMicroscopicPhaseWidget(self,data,mainwindows)","")
   
        self.groupbox.setTitle("Chemical mixture")
        
        hbox = QtGui.QHBoxLayout()
        
        self.gridbox = QtGui.QGridLayout()
        self.gridbox.addWidget( QtGui.QLabel("Temperature :"),1,1)
        self.gridbox.addWidget( QtGui.QLabel(str(data.getTemperature())),1,2)
        self.gridbox.addWidget( QtGui.QLabel(data.getUnits().getTempStr()),1,3)
        bttn =  msPushButtonWithDoc("set","setTemperature",self.data,mainwindows.webView) 
        bttn.clicked.connect(self.setTemperature)
        self.gridbox.addWidget( bttn,1,4)

        self.gridbox.addWidget( QtGui.QLabel("Density :"),2,1)
        self.gridbox.addWidget( QtGui.QLabel(str(data.getDensity())),2,2)
        self.gridbox.addWidget( QtGui.QLabel(data.getUnits().getStr(unitTypeVar.vDensity)),2,3)
        bttn =  msPushButtonWithDoc("set","setDensity",self.data,mainwindows.webView) 
        bttn.clicked.connect(self.setDensity)
        self.gridbox.addWidget( bttn,2,4)
 
        self.gridbox.addWidget( QtGui.QLabel("Molar density :"),3,1)
        self.gridbox.addWidget( QtGui.QLabel("todo"),3,2)
        self.gridbox.addWidget( QtGui.QLabel(data.getUnits().getStr(unitTypeVar.vQuantityDensity)),3,3)
        bttn =  QtGui.QPushButton("set")
        bttn.clicked.connect(self.setDensity)
        bttn.setEnabled(0)
        self.gridbox.addWidget( bttn,3,4)
        
        hbox.addLayout(self.gridbox)
        hbox.addStretch(1)
        
        self.vbox.addLayout(hbox)
        self.vbox.addSpacing(20)
        
        self.speciesList = QtGui.QTreeView()
        self.vbox.addWidget(self.speciesList  )

        model = SpeciesTreeModel(data)
        self.speciesList.setModel(model)

        hbox =  QtGui.QHBoxLayout()
        bttnMass = QtGui.QPushButton("set mass fractions")
        bttnMass.setEnabled(0)
        bttnMole = msPushButtonWithDoc("set molar fractions","setMoleFractions",self.data,mainwindows.webView) 
        bttnMole.clicked.connect(self.setMoleFractions)
        hbox.addWidget(bttnMass)
        hbox.addWidget(bttnMole)
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)

        msLogger.exitFunction("msMicroscopicPhaseWidget::msMicroscopicPhaseWidget(self,data,mainwindows)")
        
        self.setMinimumSize(550,700 )
   
    def setDensity(self):
        d = self.data.getDensity()
        widget = queryDoubleDialog("set the density","density in "+self.data.getUnits().getStr(unitTypeVar.vDensity),  d, self.data.setDensity )
        widget.exec_()
        self.mainwindows.reload()
   
    def setMoleFractions(self,fracStr):
        mapFraction = msMap_string_double()
	for element in fracStr.split(' '):
	    if element.find(":") :
	      mapFraction[element.split(':')[0]]=float(element.split(':')[1])
	self.data.setMoleFractions(mapFraction)
	
    def queryMoleFractions(self):
        widget = queryDoubleDialog("set molar fractions","enter the molar fraction (e.g. \"H2:0.5 CH4:0.2 CH3:0.1\") ","", self.setMoleFraction )
        widget.exec_()
        self.mainwindows.reload()
   
    def setTemperature(self):
        T = self.data.getTemperature()
        widget = queryDoubleDialog("set the temperature","enter the temperature in "+self.data.getUnits().getTempStr(),  T, self.data.setTemperature )
        widget.exec_()
        self.mainwindows.reload()

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from Dialogs.simpleDialogs import *
from TreeModels.simpletreemodel import *

import itertools
import functools
import sys, os

class SpeciesTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(SpeciesTreeModel, self).__init__(data,"msTreeMapper",parent)

	unitE = data.getUnits().getStr(unitTypeVar.vEnergyByQuantity)
	unitS = data.getUnits().getStr(unitTypeVar.vEntropyByQuantity)
	
        self.rootItem = TreeItem(("name","H ["+unitE+"]","Cp ["+unitS+"]","Cv ["+unitS+"]","S ["+unitS+"]"))
        self.addDataChildren(data, self.rootItem )
        
    def addDataChildren(self,data,parent):
        
        tuple = itertools.izip(  data.getEntitiesNames(),
                                 data.Hs(),data.Cps(),data.Cvs(),data.Ss() )
	
        for name,h,cp,cv,s in tuple:
            
            textData=[]
            textData.append(name)
            textData.append(h)
            textData.append(cp)   
            textData.append(cv)
            textData.append(s)
            item=TreeItem(textData)
            parent.appendChild(item)
            
        textData=[]
        textData.append("total")
        textData.append(data.HTot())
        textData.append(data.CpTot())
        textData.append(data.CvTot())                
        textData.append(data.STot())
        item=TreeItem(textData)
        parent.appendChild(item)
        return data

## \brief Widget for msMicroscopicPhase
class msThermoMixtureWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msThermoMixtureWidget, self).__init__(data, mainwindows)
        
        msLogger.enterFunction("msThermoMixtureWidget::msThermoMixtureWidget(self,data,mainwindows)","")
   
        self.groupbox.setTitle("Thermodynamic mixture")
        hbox = QtGui.QHBoxLayout()
        
        self.speciesList = QtGui.QTreeView()
        self.vbox.addWidget(self.speciesList  )
        
        model = SpeciesTreeModel(data)
        self.speciesList.setModel(model)
        
        self.vbox.addLayout(hbox)

        msLogger.exitFunction("msThermoMixtureWidget::msThermoMixtureWidget(self,data,mainwindows)")
        
        self.setMinimumSize(550,700 )
        self.setMaximumSize(550,700 )

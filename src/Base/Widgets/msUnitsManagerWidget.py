      
"""
Created on Sat Aug 24 15:38:04 2013

@author: chico
"""

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
import inspect
import itertools            
from TreeModels.simpletreemodel import *

class msUnitsManagerDialog(QtGui.QDialog):

    def __init__(self, defaultValues, unitswidget, parent=None):
        super(msUnitsManagerDialog,self).__init__(parent)

        self.unitsWidget = unitswidget
        self.setWindowTitle("msUnitsManager")
        
        targets = self.unitsWidget.targets
	
        self.vbox = QtGui.QVBoxLayout()
        self.gridbox = QtGui.QGridLayout()
        self.comboboxes = []
	
        for unit,i in itertools.izip( targets, range(0,len(targets)) ):

            self.gridbox.addWidget( QtGui.QLabel(msUnit.getStringVariableType(unit)), i+1,1)
            combobox = QtGui.QComboBox()
            
            listUnits = msUnitsManager.listUnitsAvailable(targets[i])
            
            combobox.addItem( defaultValues[i] )
            
            for j in listUnits:
                
                if( j.split("^")[0] != defaultValues[i] ):
                    combobox.addItem(j.split("^")[0])
            
            self.comboboxes.append(combobox)
            self.gridbox.addWidget(combobox,i+1,2)
	
        self.vbox.addLayout(self.gridbox)
        hbox = QtGui.QHBoxLayout()
        hbox.addStretch(1)
        cancelButton = QtGui.QPushButton("cancel")
        cancelButton.clicked.connect(self.close)
        updateUnitsButton = QtGui.QPushButton("ok")
        updateUnitsButton.setDefault(1)
        updateUnitsButton.clicked.connect(self.update)
        hbox.addWidget(cancelButton)
        hbox.addWidget(updateUnitsButton)
        self.vbox.addLayout(hbox)
      
        self.setLayout(self.vbox)

        self.setMinimumSize(200, 50 * len(targets))
        self.setMaximumSize(200, 50 * len(targets))

    def update(self):
        string = ""
        for combo in self.comboboxes:
            string =string + str(combo.currentText())+" "
        self.unitsWidget.data.set(string)
        self.unitsWidget.mainwindows.reload()
        self.close()


class ObjectOfUnitsTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(ObjectOfUnitsTreeModel, self).__init__(data,"msTreeMapper",parent)

        self.rootItem = TreeItem(("id","variable","type"))
        self.addDataChildren(data, self.rootItem )
        
    def addDataChildren(self,data,parent):
        
        a = data.getObjectsUsingMe()
        for i in a:
	    textData=[]
	    textData.append( i.getId() )
            textData.append( i.getFullVariableName() )
            textData.append( i.getType().split(':')[-2])
            item=TreeItem(textData)
            parent.appendChild(item)
        
        return data
      
      
class msUnitsManagerWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msUnitsManagerWidget, self).__init__(data, mainwindows)
        
        self.targets = [ unitTypeVar.vEnergy, unitTypeVar.vTime,
                        unitTypeVar.vLength, unitTypeVar.vMass,
                        unitTypeVar.vAngle, unitTypeVar.vPressure,
                         unitTypeVar.vQuantity ]
        
        self.gridbox = QtGui.QGridLayout()
        self.defaults = []
        
        for unit,i in itertools.izip( self.targets, range(0,len(self.targets)) ):
            
            self.gridbox.addWidget( QtGui.QLabel(msUnit.getStringVariableType(unit)+":"),i+1,1)
            self.gridbox.addWidget( QtGui.QLabel(data.getStr(self.targets[i]).split("^")[0] ),i+1,2)
            self.defaults.append( data.getStr(self.targets[i]).split("^")[0] )
        
        updateUnitsButton = QtGui.QPushButton("update")
        updateUnitsButton.clicked.connect(self.update)
        self.gridbox.addWidget(updateUnitsButton,len(self.targets)+1,2)
        
        hbox = QtGui.QHBoxLayout()
        hbox.addLayout(self.gridbox)
        hbox.addSpacing(20)
        vbox = QtGui.QVBoxLayout()
        vbox.addWidget(QtGui.QLabel("Objects using the unit system:"))
        self.listObj = QtGui.QTreeView()
        self.listObj.setModel( ObjectOfUnitsTreeModel(data) )
        vbox.addWidget(self.listObj)
        hbox.addLayout(vbox)
        
        self.vbox.addLayout(hbox)
        
        self.setMinimumSize(520,40 * i )
        self.setMaximumSize(520,40 * i )
        self.groupbox.setTitle("Units manager")

    def update(self):
        dialog = msUnitsManagerDialog(self.defaults,self)
        dialog.exec_()
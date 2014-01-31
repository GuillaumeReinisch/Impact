"""
Created on Sat Aug 24 16:17:53 2013

@author: chico
"""

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from msUnitsManagerWidget import *
from msParamsManagerWidget  import *




class PhysicalVariableTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(PhysicalVariableTreeModel, self).__init__(data,"msTreeMapper",parent)

        self.rootItem = TreeItem(("name",
                                  "value","unit"))
        self.addDataChildren(data, self.rootItem )
        
    def addDataChildren(self,data,parent):
        
        a = data.getPhysicalVariables()
        for i in a:
	    textData=[]
            textData.append( i.getName() )
            textData.append( i.getValue() )
            textData.append( i.getUnit().getStr() )
            item=TreeItem(textData)
            parent.appendChild(item)
            
        return data

class msPhysicalInterfaceWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msPhysicalInterfaceWidget, self).__init__(data,mainwindows)
        
        
        self.Units          = msUnitsManagerWidget(self.data.getUnits(),mainwindows)
        self.Parameters = msParamsManagerWidget(self.data.getParameters(),mainwindows)
        height               = max(self.Units.size().height(),self.Parameters.size().height())
        self.Parameters.resize(self.Parameters.width(),height)
        self.Units.resize(self.Units.width(),height)
       
        self.vbox.addWidget(self.Units)
        self.vbox.addWidget(self.Parameters)
        
        self.vbox.addWidget(QtGui.QLabel("Declared physical variables:"))
        self.physVars = QtGui.QTreeView()
        self.physVars.setModel( PhysicalVariableTreeModel(data) )
        self.vbox.addWidget(self.physVars)
        
        self.setMinimumSize(550,600)
        """
        self.setMinimumSize(550,600 )
        self.setMaximumSize(550,600 )
        """
        self.groupbox.setTitle("&Physical interface")

 
"""
Created on Sat Aug 24 16:39:31 2013

@author: chico
"""

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from msGeneralizedCoordinatesWidget import *


class EigenvaluesTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(EigenvaluesTreeModel, self).__init__(parent)

	self.rootItem = TreeItem(("Index","Energy","g"))
	self.addDataChildren(data, self.rootItem )
        
    def addDataChildren(self,data,parent):
        
        i = 0
        
        while True:
            try:
                textData=[]
                textData.append(i)
                textData.append(data.eigenvalue(i))
                textData.append(data.degenerency(i))
                item=TreeItem(textData)
                parent.appendChild(item)
                i = i + 1
            except:
	        return data
	   
        return data
      
      
class msQuantumInterfaceWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msQuantumInterfaceWidget, self).__init__(data,mainwindows)
        
        
        hbox=QtGui.QHBoxLayout()
        hbox.addWidget( QtGui.QPushButton("Compute eigenstates")  )
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)
        
        self.listEigenvalues = QtGui.QTreeView()
        self.vbox.addWidget(self.listEigenvalues  )
        self.listEigenvalues.setModel(EigenvaluesTreeModel(data))

	hbox=QtGui.QHBoxLayout()
        hbox.addWidget( QtGui.QPushButton("get occupency probability w/T")  )
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)
        
        self.groupbox.setTitle("Quantum Interface")
        self.setMinimumSize(300,300)
        self.setMaximumSize(300,300)
 
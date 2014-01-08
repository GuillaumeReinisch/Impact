"""
    Created on Sat Aug 24 16:39:31 2013
    
    @author: chico
    """

from libimpact import *
from PySide import QtCore, QtGui
from msLagrangianWidget import *


class GenCoorsTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(GenCoorsTreeModel, self).__init__(parent)
        
        self.rootItem = TreeItem(("Id","Value","Unit"))
        self.addDataChildren(data, self.rootItem )
    
    def addDataChildren(self,data,parent):
        
        for i in data.getLagrangian().getGeneralizedCoordinates().getActives():
                   
            textData=[]
            textData.append(i.getId())
            textData.append(i.getValue())
            textData.append(i.getUnitStr())
            item=TreeItem(textData)
            parent.appendChild(item)
                               
        return data

class GenVelTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(GenVelTreeModel, self).__init__(parent)
        
        self.rootItem = TreeItem(("Id","Value","Unit"))
        self.addDataChildren(data, self.rootItem )
    
    def addDataChildren(self,data,parent):
        
        for i in data.getLagrangian().getGeneralizedVelocities().getActives():
            
            textData=[]
            textData.append(i.getId())
            textData.append(i.getValue())
            textData.append(i.getUnitStr())
            item=TreeItem(textData)
            parent.appendChild(item)
        
        return data

class msSolverLagrangianWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msSolverLagrangianWidget, self).__init__(data,mainwindows)
        
        hbox = QtGui.QHBoxLayout()
        stepBttn = QtGui.QPushButton("step")
        stepBttn.clicked.connect(data.step)
        hbox.addWidget(stepBttn)
        hbox.addStretching(1)
        self.vbox.addLayout(hbox)
        hbox = QtGui.QHBoxLayout()
        Gcoors = QtGui.QTreeView()
        Gcoors.setModel(GenCoorsTreeModel(data))
        hbox.addWidget(Gcoors)
        Vcoors = QtGui.QTreeView()
        Vcoors.setModel(GenVelTreeModel(data))
        hbox.addWidget(Vcoors)
        
        self.vbox.addLayout(hbox  )
        
        gbox = QtGui.QGridLayout()
        groupBox = QtGui.QGroupBox("Energies")
        
        gbox.addWidget(QtGui.QLabel("Etot"),1,1)
        gbox.addWidget(QtGui.QLabel("Epot"),1,2)
        gbox.addWidget(QtGui.QLabel("Ekin"),1,3)
        gbox.addWidget(QtGui.QLabel("L"),1,4)
        E = 0
        T = 0
        U = 0
        L = 0
        try:
            E = data.getLagrangian().E()
            T = data.getLagrangian().T()
            U = data.getLagrangian().U()
            L = data.getLagrangian().L()
        except:
            pass
        self.Etot = QtGui.QLabel('{:.3e}'.format(E))
        self.Epot = QtGui.QLabel('{:.3e}'.format(U))
        self.Ekin = QtGui.QLabel('{:.3e}'.format(T))
        self.L = QtGui.QLabel('{:.3e}'.format(L))
        
        gbox.addWidget(self.Etot,2,1)
        gbox.addWidget(self.Epot,2,2)
        gbox.addWidget(self.Ekin,2,3)
        gbox.addWidget(self.L,2,4)
        groupBox.setLayout(gbox)
        self.vbox.addWidget(groupBox)
        
        self.groupbox.setTitle("Solver Lagrangian")
        self.setMinimumSize(550,250)
        self.setMaximumSize(550,250)
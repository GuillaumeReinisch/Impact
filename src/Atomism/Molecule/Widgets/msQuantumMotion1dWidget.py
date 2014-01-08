"""
Created on Sat Aug 24 16:39:31 2013

@author: chico
"""

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from msGeneralizedCoordinatesWidget import *
from msQuantumInterfaceWidget import *
from Dialogs.queryTreeMapperDialog import *

class msQuantumMotion1dWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msQuantumMotion1dWidget, self).__init__(data,mainwindows)
        
        try:
            data.computeEigenValues()
        except:
            pass
        self.fixedKinEffCheckBox= QtGui.QCheckBox("freeze kinetic constant")
        self.fixedKinEffCheckBox.clicked.connect(self.fixKinConstant)
        self.vbox.addWidget(self.fixedKinEffCheckBox)
        self.qInterface = msQuantumInterfaceWidget(data,mainwindows)
        self.vbox.addWidget(self.qInterface)
        self.groupbox.setTitle("Quantum Motion 1D")
        
        hbox = QtGui.QHBoxLayout()
        plotES = QtGui.QPushButton("plot eigenstate")
        plotES.clicked.connect(self.plotEigenState)
        hbox.addWidget(plotES)
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)
        self.setMinimumSize(550,400)
        self.setMaximumSize(550,400)
        
    def fixKinConstant(self):
        if self.fixedKinEffCheckBox.isChecked():
	    self.data.freezeKinValue()
        if not self.fixedKinEffCheckBox.isChecked():
	    self.data.unfreezeKinValue()       
        
    def plotEigenState(self):
        index = self.qInterface.listEigenvalues.currentIndex().row()
        if index == -1:
	    print "You need to select an eigenvalue in the list"
	    return
	  
        es = self.data.getEigenvector(index)
        
        fig, axes = plt.subplots(1, 1)
        x = []
        y = []
        coor = es.getCoordinates().getVariable(0)
        coor.begin()
        while coor.increment():
	    x.append(coor.getValue())
	    y.append(es.evaluate())
	    
        axes.plot(x,y)
        axes.set_xlabel(coor.getId() + " [" + coor.getUnitStr() + "]" )
        axes.set_ylabel("Probability")
        fig.show()
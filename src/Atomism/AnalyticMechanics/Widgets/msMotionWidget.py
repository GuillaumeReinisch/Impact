"""
Created on Sat Aug 24 16:39:31 2013

@author: chico
"""

from libimpact import *
from PySide import QtCore, QtGui
from msGeneralizedCoordinatesWidget import *
from Dialogs.simpleDialogs import *

class msMotionWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        msWidget.__init__(self,data,mainwindows)

        self.vbox.addWidget(QtGui.QLabel("Number of degrees of freedom: "+str(data.noOfDof())))
        
        hbox = QtGui.QHBoxLayout() 
        hbox.addWidget(QtGui.QLabel("Symmetry number: "+str(data.noOfSymmetry())))
        symBttn = QtGui.QPushButton("set")
        hbox.addStretch(1)
        symBttn.clicked.connect(self.setSymmetryNumber)
        hbox.addWidget(symBttn)
        self.vbox.addLayout(hbox)
        
        hbox = QtGui.QHBoxLayout() 
        hbox.addWidget(QtGui.QLabel("Multiplicity number: "+str(data.noOfMultiplicity())))
        multBttn = QtGui.QPushButton("set")
        hbox.addStretch(1)
        multBttn.clicked.connect(self.setMultipicity)
        hbox.addWidget(multBttn)
        self.vbox.addLayout(hbox)
        
        groupBox = QtGui.QGroupBox("Canonical ensemble")
        gbox = QtGui.QGridLayout() 
        gbox.addWidget(QtGui.QLabel("Temperature [K]: "),1,1)
        hbox = QtGui.QHBoxLayout()
        self.tempLineEdit = QtGui.QLineEdit("0")
        hbox.addWidget(self.tempLineEdit)
        hbox.addStretch(1)
        gbox.addLayout(hbox,1,2)

        gbox.addWidget(QtGui.QLabel("Partition function: "),2,1)
        self.labelQ = QtGui.QLabel("0")
        gbox.addWidget(self.labelQ ,2,2)
        self.Qbttn = QtGui.QPushButton("compute")
        self.Qbttn.clicked.connect(self.computeQ)
        gbox.addWidget(self.Qbttn,2,3)
        self.plotQbttn = QtGui.QPushButton("plot")
        self.plotQbttn.clicked.connect(self.plotQ)
        gbox.addWidget(self.plotQbttn,2,4)

        gbox.addWidget(QtGui.QLabel("Part. fct. from DOS: "),3,1)
        self.labelQDos = QtGui.QLabel("0")
        gbox.addWidget(self.labelQDos ,3,2)
        self.QDosbttn = QtGui.QPushButton("compute")
        self.QDosbttn.clicked.connect(self.computeQDos)
        gbox.addWidget(self.QDosbttn,3,3)
        self.plotQDosbttn = QtGui.QPushButton("plot")
        self.plotQDosbttn.clicked.connect(self.plotQDos)
        gbox.addWidget(self.plotQDosbttn,3,4)
        
        gbox.addWidget(QtGui.QLabel("Entropy []: "),4,1)
        self.labelS = QtGui.QLabel( "0" )
        gbox.addWidget(self.labelS,4,2)
        self.Sbttn = QtGui.QPushButton("compute")
        self.Sbttn.clicked.connect(self.computeS)
        gbox.addWidget(self.Sbttn,4,3)                    
        self.plotSbttn = QtGui.QPushButton("plot")
        self.plotSbttn.clicked.connect(self.plotS)
        gbox.addWidget(self.plotSbttn,4,4)

        gbox.addWidget(QtGui.QLabel("Calorific capacity []: "),5,1)
        self.labelCp = QtGui.QLabel( "0" )
        gbox.addWidget(self.labelCp,5,2)
        self.Cpbttn = QtGui.QPushButton("compute")
        self.Cpbttn.clicked.connect(self.computeCp)
        gbox.addWidget(self.Cpbttn,5,3)
        self.plotCpbttn = QtGui.QPushButton("plot")
        self.plotCpbttn.clicked.connect(self.plotCp)
        gbox.addWidget(self.plotCpbttn,5,4)

        groupBox.setLayout(gbox)
        self.vbox.addWidget(groupBox)
        
        groupBox = QtGui.QGroupBox("Microcanonical ensemble")
        gbox = QtGui.QGridLayout() 
        gbox.addWidget(QtGui.QLabel("Energy ["+self.data.getUnits().getEnergyStr()+"]:"),1,1)
        hbox = QtGui.QHBoxLayout()
        self.energyLineEdit = QtGui.QLineEdit("0")
        hbox.addWidget(self.energyLineEdit)
        hbox.addStretch(1)
        gbox.addLayout(hbox,1,2)

        gbox.addWidget(QtGui.QLabel("DOS [/"+self.data.getUnits().getEnergyStr()+"]: "),2,1)
        self.labelDos = QtGui.QLabel( "0" )
        gbox.addWidget(self.labelDos,2,2)
        self.DosBttn = QtGui.QPushButton("compute")
        self.DosBttn.clicked.connect(self.computeDos)
        gbox.addWidget(self.DosBttn,2,3)             
        self.plotDOSbttn = QtGui.QPushButton("plot")
        self.plotDOSbttn.clicked.connect(self.plotDos)
        gbox.addWidget(self.plotDOSbttn,2,4)
        
        gbox.addWidget(QtGui.QLabel("SOS []: "),3,1)        
        self.labelSos = QtGui.QLabel( "0" )
        gbox.addWidget( self.labelSos,3,2)
        self.SosBttn = QtGui.QPushButton("compute")
        self.SosBttn.clicked.connect(self.computeSos)
        gbox.addWidget(self.SosBttn,3,3)  
        self.plotSOSbttn = QtGui.QPushButton("plot")
        self.plotSOSbttn.clicked.connect(self.plotSos)
        gbox.addWidget(self.plotSOSbttn,3,4)

        groupBox.setLayout(gbox)
        self.vbox.addWidget(groupBox)

        self.groupbox.setTitle("Motion")
        self.setMinimumSize(550,500)
        self.setMaximumSize(550,500)
        #self.evaluateField()

    def setSymmetryNumber(self):
        sym = self.data.noOfSymmetry()
        widget = queryIntDialog("set the temperature","enter the temperature in "+self.data.getUnits().getTempStr(),  sym, self.data.setSymmetryNumber )
        widget.exec_()
        self.mainwindows.reload()
 
    def setMultipicity(self):
        sym = self.data.noOfMultiplicity()
        widget = queryIntDialog("set the temperature","enter the temperature in "+self.data.getUnits().getTempStr(),  sym, self.data.setMultiplicity )
        widget.exec_()
        self.mainwindows.reload()
        
    def plot(self,fct,tittle):
        
        fig, axes = plt.subplots(1, 1)
        x = []
        y = []
        coor = fct.getCoordinates().getVariable(0)
        coor.begin()
        while coor.increment():
	    x.append(coor.getValue())
	    y.append(fct.evaluate())
	    
        axes.plot(x,y)
        axes.set_xlabel(coor.getId() + " [" + coor.getUnitStr() + "]" )
        axes.set_ylabel(tittle)
        fig.show()
        
    def computeQ(self):
        self.labelQ.setText( '{:.3e}'.format( self.data.Q(float(self.tempLineEdit.text()) )  ) )
 
    def plotQ(self):
        self.plot(self.data.getQ(),"Partition function")
 
    def computeQDos(self):
        self.labelQDos.setText( '{:.3e}'.format( self.data.QfromDOS(float(self.tempLineEdit.text()) )  ) )
 
    def plotQDos(self):
        self.plot(self.data.getQfromDOS(),"Partition function")
          
    def computeS(self):
        self.labelS.setText( '{:.3e}'.format(self.data.S(float(self.tempLineEdit.text()) ) ) ) 

    def plotS(self):
        self.plot(self.data.getS(),"Entropy")
 
    def computeCp(self):
        self.labelCp.setText( '{:.3e}'.format(self.data.Cp(float(self.tempLineEdit.text()) )  ) )
    
    def plotCp(self):
        self.plot(self.data.getCp(),"Calorific capacity at cst pressure")        
        
    def computeDos(self):
        self.labelDos.setText( '{:.3e}'.format(self.data.DOS(float(self.energyLineEdit.text()) ) ) )

    def plotDos(self):
        self.plot(self.data.getDOS(),"Density of states")
        
    def computeSos(self):
        self.labelSos.setText( '{:.3e}'.format(self.data.SOS(float(self.energyLineEdit.text()) ) ) )
        
    def plotSos(self):
        self.plot(self.data.getSOS(),"Sum of states")
        
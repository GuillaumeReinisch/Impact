"""
    Created on Sat Aug 24 16:39:31 2013
    
    @author: chico
    """

from libimpact import *
from PySide import QtCore, QtGui
from msGeneralizedCoordinatesWidget import *

class msLagrangianWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msLagrangianWidget, self).__init__(data,mainwindows)
        
        gbox = QtGui.QGridLayout()
        
        self.synchronization = QtGui.QCheckBox("synchronization/coordinates")
        self.vbox.addWidget(self.synchronization)
        
        self.vbox.addSpacing(10)
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
            E = data.E()
            T = data.T()
            U = data.U()
            L = data.L()
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
        self.vbox.addSpacing(10)
        
        groupBox = QtGui.QGroupBox("generalized coordinates")
        vbox = QtGui.QVBoxLayout()
        self.gencoors =  msGeneralizedCoordinatesWidget( data.getGeneralizedCoordinates(),mainwindows)
        vbox.addWidget( self.gencoors )
        try:
            self.gencoors.widgetScalVar.slider.valueChanged[int].connect(self.updateCoorChange)
            self.gencoors.widgetScalVar.valueSpinBox.valueChanged[float].connect(self.updateCoorChange)
        except:
            pass
        
        groupBox.setLayout(vbox)
        self.vbox.addWidget(groupBox)
   
        groupBox = QtGui.QGroupBox("generalized velocities")
        vbox = QtGui.QVBoxLayout()
        self.genvelo =  msGeneralizedCoordinatesWidget( data.getGeneralizedVelocities(),mainwindows)
        vbox.addWidget( self.genvelo )
        try:
            self.genvelo.widgetScalVar.slider.valueChanged[int].connect(self.updateVelocityChange)
            self.genvelo.widgetScalVar.valueSpinBox.valueChanged[float].connect(self.updateVelocityChange)
        except:
            pass
        groupBox.setLayout(vbox)
        self.vbox.addWidget(groupBox)
   
        self.vbox.addSpacing(20)
        
        hbox=QtGui.QHBoxLayout()
        hbox.addWidget( QtGui.QPushButton("compute hessian")  )
        hbox.addWidget( QtGui.QPushButton("compute observable") )
        hbox.addStretch(1)
        
        self.vbox.addLayout(hbox)
        
        self.groupbox.setTitle("Lagrangian")
        self.setMinimumSize(550,650)
        self.setMaximumSize(550,650)

    def updateVelocityChange(self):
        if self.synchronization.isChecked():
            self.Etot.setText('{:.3e}'.format(self.data.E()))
            self.Epot.setText('{:.3e}'.format(self.data.U()))
            self.Ekin.setText('{:.3e}'.format(self.data.T()))
            self.L.setText('{:.3e}'.format(self.data.L()))
    
    def updateCoorChange(self):
        if self.synchronization.isChecked():
            self.Etot.setText('{:.3e}'.format(self.data.E()))
            self.Epot.setText('{:.3e}'.format(self.data.U()))
            self.Ekin.setText('{:.3e}'.format(self.data.T()))
            self.L.setText('{:.3e}'.format(self.data.L()))  
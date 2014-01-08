"""
Created on Sat Aug 24 16:31:32 2013

@author: chico
"""
from libimpact import *
from PySide import QtCore, QtGui
import functools
from msWidget import *
from msUnitsManagerWidget import *
from msUnitWidget import*

class popupSetCoordinate(QtGui.QDialog):
    
    def __init__(self,data,methodupdate,mainwindows):
        super(popupSetCoordinate, self).__init__()
        self.data = data
        self.methodUpdate = methodupdate
        self.mainwindows = mainwindows
        self.initUI()
        
    def initUI(self):
        
        self.vbox=QtGui.QVBoxLayout()
        vbox = QtGui.QGridLayout()
        
        vbox.addWidget(QtGui.QLabel("Minimum"),1,1)
        vbox.addWidget(QtGui.QLabel("Maximum"),2,1)        
        vbox.addWidget(QtGui.QLabel("Derivation step"),3,1)
        vbox.addWidget(QtGui.QLabel("Integration step"),4,1)

        self.min=QtGui.QLineEdit()
        self.max=QtGui.QLineEdit()
        self.dq=QtGui.QLineEdit()
        self.Dq=QtGui.QLineEdit()
        
        self.min.setText(str(self.data.getMin()))
        self.max.setText(str(self.data.getMax()))
        self.dq.setText(str(self.data.getdq()))
        self.Dq.setText(str(self.data.getDq()))
        
        vbox.addWidget(self.min,1,2)
        vbox.addWidget(self.max,2,2)        
        vbox.addWidget(self.dq,3,2)
        vbox.addWidget(self.Dq,4,2)
        self.vbox.addLayout(vbox)
        self.unitWidget = msUnitWidget(self.data.getUnit(),self.mainwindows)
        self.vbox.addWidget(self.unitWidget,5,2) 
        self.setLayout(self.vbox)

        hbox = QtGui.QHBoxLayout()
        hbox.addStretch(1)
        okbtn=QtGui.QPushButton("update")
        okbtn.clicked.connect(self.execute)    
        hbox.addWidget(okbtn,6,2)
        self.vbox.addLayout(hbox)

    def execute(self):
        self.data.setMin(float(self.min.text()))
        self.data.setMax(float(self.max.text()))
        self.data.setdq( float(self.dq.text()))
        self.data.setDq( float(self.Dq.text()))
        self.data.setUnit(str(self.unitWidget.lineEdit.text() ))
        self.mainwindows.clickTreeView()
        self.close()
                        
   


class msScalarVariableWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msScalarVariableWidget, self).__init__(data,mainwindows)
        if(not data.isDerivedFrom("msScalarVariable")):
            raise Exception("uiScalarVariable can be instanciated only "+
                            "with a msScalarVariable derived object.")
        
        """ First create the Units group box
        """      
        self.groupbox.setContentsMargins(0,0,0,0)
        variable = data
        self.slider=QtGui.QSlider(QtCore.Qt.Horizontal,self)
        self.Dq = self.data.getDq()
        if self.Dq == 0:
            self.Dq = (variable.getMax()-variable.getMin())/100
        
        if(self.Dq!=0):
            self.slider.setRange(0,(variable.getMax()-variable.getMin())/self.Dq )
        self.slider.setSingleStep(1)
        if(self.Dq!=0):
            self.slider.setValue((variable.getValue()-variable.getMin())/self.Dq )
        
        hbox =  QtGui.QHBoxLayout()
        
        self.valueSpinBox = QtGui.QDoubleSpinBox()
        self.valueSpinBox.setRange(variable.getMin(), variable.getMax())
        self.valueSpinBox.setSingleStep(self.Dq)
        self.valueSpinBox.setValue(variable.getValue())
        
        self.slider.valueChanged[int].connect(self.updateFromSlidersChange)
        self.valueSpinBox.valueChanged[float].connect(self.updateFromSpinsChange)
        
        hbox.addWidget(self.valueSpinBox)
        hbox.addWidget(self.slider)
        
        self.vboxBttn              =  QtGui.QVBoxLayout()
        
        editbtn = QtGui.QPushButton("edit")
        self.vboxBttn.addWidget(editbtn) 
        
        slot=functools.partial(self.editCoordinate, variable)
        editbtn.clicked.connect(slot)
        hbox.addLayout(self.vboxBttn)  
        self.vbox.addLayout(hbox)
        self.setMinimumHeight(80)
        self.setMaximumHeight(80)

        self.setMinimumWidth(250)
        self.setMaximumWidth(250)
    
        
    def addDeleteOption(self,method):     
        btn = QtGui.QPushButton("delete")
        self.vboxBttn.addWidget(btn)
        slot=functools.partial(method, self.data)
        btn.clicked.connect(slot)
        self.setMinimumHeight(100)
        self.setMaximumHeight(100)
        
    def editCoordinate(self,coordinate):     
        widget=popupSetCoordinate(coordinate,self.updateCoordinate,self.mainwindows)
        widget.exec_()
        self.mainwindows.clickTreeView()
        
    def updateCoordinate(self):     
        variable =self.data
        self.slider.setRange(0,(variable.getMax()-variable.getMin())/self.Dq )
        self.slider.setSingleStep(1)
            
        self.valueSpinBox.setRange(variable.getMin(), variable.getMax())
        self.valueSpinBox.setSingleStep(self.Dq )
        self.valueSpinBox.setValue(variable.getValue())
        
    def updateFromSpinsChange(self):
        variable =self.data
        self.slider.valueChanged[int].disconnect(self.updateFromSlidersChange)
        self.valueSpinBox.valueChanged[float].disconnect(self.updateFromSpinsChange)
        
        variable.setValue( self.valueSpinBox.value() )
        self.slider.setValue((variable.getValue()-variable.getMin())/self.Dq )
       
        self.slider.valueChanged[int].connect(self.updateFromSlidersChange)
        self.valueSpinBox.valueChanged[float].connect(self.updateFromSpinsChange)
            
    def updateFromSlidersChange(self):
        variable =self.data
        self.slider.valueChanged[int].disconnect(self.updateFromSlidersChange)
        self.valueSpinBox.valueChanged[float].disconnect(self.updateFromSpinsChange)
         
        self.valueSpinBox.setValue( variable.getMin() + self.slider.value() * self.Dq )
        variable.setValue( variable.getMin() + self.slider.value() * self.Dq )
        
        self.slider.valueChanged[int].connect(self.updateFromSlidersChange)
        self.valueSpinBox.valueChanged[float].connect(self.updateFromSpinsChange)
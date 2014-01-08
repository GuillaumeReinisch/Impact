"""
Created on Sat Aug 24 16:39:31 2013

@author: chico
"""

from libimpact import *
from PySide import QtCore, QtGui
from msGeneralizedCoordinatesWidget import *
from TreeModels import *
#from PopUpBox import *
import matplotlib.pyplot as plt
import sys

class popupDefineConstant(QtGui.QDialog):
    
    def __init__(self,data):
        super(popupDefineConstant, self).__init__()
        self.data = data
        hbox=QtGui.QHBoxLayout()
        self.idBttn = QtGui.QLineEdit("id")
        hbox.addWidget( self.idBttn )
        self.valueBttn = QtGui.QLineEdit("value")
        hbox.addWidget(self.valueBttn)
        okbtn=QtGui.QPushButton("ok")
        okbtn.clicked.connect(self.execute)    
        hbox.addWidget(okbtn)
        self.setLayout(hbox)

    def execute(self):
        try:
            self.data.setConstant(str(self.idBttn.text()),float(self.valueBttn.text()) )
        except:
            raise("need a float value to define the constant")
        self.close()
    

class msScalarFunctionWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msScalarFunctionWidget, self).__init__(data,mainwindows)
        
        """ First create the Units group box
        """        
        hbox = QtGui.QHBoxLayout() 
        vbox = QtGui.QVBoxLayout() 
        vbox.addStretch(1)
        self.resultLabel = QtGui.QLabel("Current field value: ")
        vbox.addWidget(self.resultLabel)

        hbox1=QtGui.QHBoxLayout() 
        self.buttonEvaluate = QtGui.QPushButton("recompute field")
        self.buttonEvaluate.clicked.connect(self.evaluateField)
        self.buttonEvaluate.setMinimumWidth(150)
        hbox1.addWidget(self.buttonEvaluate)
        hbox1.addStretch(1)
        hbox2=QtGui.QHBoxLayout() 
        self.buttonPlot = QtGui.QPushButton("plot 1D")
        self.buttonPlot.setMinimumWidth(150)
        self.buttonPlot.clicked.connect(self.plot1d)
        self.buttonPlot.setVisible(0)
        hbox2.addWidget(self.buttonPlot)
        hbox2.addStretch(1)
        hbox3=QtGui.QHBoxLayout() 
        self.buttonResetunit = QtGui.QPushButton("reset unit")
        self.buttonResetunit.setMinimumWidth(150)
        hbox3.addWidget(self.buttonResetunit)
        hbox3.addStretch(1)

        vbox.addLayout(hbox1)
        vbox.addLayout(hbox3)
        vbox.addLayout(hbox2)
        vbox.addStretch(1)
        hbox.addLayout(vbox)
        hbox.addSpacing(100)
        vbox = QtGui.QVBoxLayout() 
        self.constantsList = QtGui.QListWidget() 
        for constant in self.data.getConstants():
            self.constantsList.addItem(constant)
        vbox.addWidget(self.constantsList)
        self.constantsList.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.constantsList.setDragEnabled(1)
        self.constantsList.viewport().setAcceptDrops(1)
        self.constantsList.setDropIndicatorShown(1)
        bttn = QtGui.QPushButton("define new constant")
        vbox.addWidget(bttn)
        bttn.clicked.connect(self.addConstant)
        hbox.addLayout(vbox)

        self.vbox.addLayout(hbox)
        
        self.synchronization = QtGui.QCheckBox("synchronization/coordinates")
        self.vbox.addWidget(self.synchronization)
        self.gencoors = msGeneralizedCoordinatesWidget(self.data.getCoordinates() , self.mainwindows )
        self.gencoors.listView.clicked.connect(self.selectCoor)
        self.vbox.addWidget(self.gencoors )    
        try:
            self.gencoors.widgetScalVar.slider.valueChanged[int].connect(self.updateCoorChange)
            self.gencoors.widgetScalVar.valueSpinBox.valueChanged[float].connect(self.updateCoorChange)
        except:
            pass

        self.groupbox.setTitle("&Scalar function")
        
        self.setMinimumHeight(self.gencoors.height()+250)
        self.setMaximumHeight(self.gencoors.height()+250)
        self.setMinimumWidth(550)
        self.setMaximumWidth(550)
        #self.evaluateField()

    def selectCoor(self):               
        self.buttonPlot.setVisible(1)
        self.indiceCoor = self.gencoors.listView.currentIndex().row()
        self.coor =self.gencoors.listView.currentIndex().internalPointer().getRawdata()

    def updateCoorChange(self):
        if self.synchronization.isChecked():
	    self.evaluateField()
 
    def addConstant(self):
        widget=popupDefineConstant(self.data)
        widget.exec_()
        self.mainwindows.clickTreeView()

    def evaluateField(self):
        result = 0
        try:
            result = self.data.evaluate()
        except:
            self.mainwindows.reportException(sys)
            
            
        self.resultLabel.setText("Current field value: " + str('{:.3e}'.format(result)))
        return result
  
    def plot1d(self):
        fig, axes = plt.subplots(1, 1)
        x = []
        y = []
        self.coor.begin()
        while self.coor.increment():
	    x.append(self.coor.getValue())
	    y.append(self.evaluateField())
	    
        axes.plot(x,y)
        axes.set_xlabel(self.coor.getId() + " [" + self.coor.getUnitStr() + "]" )
        axes.set_ylabel(self.data.getId() + " [" + self.data.getUnit().getStr() + "]")
        fig.show()
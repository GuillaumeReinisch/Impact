"""Created the 2013-08-25 14:33 

@author:
"""
import sys

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from msGeneralizedCoordinatesWidget import *
from msScalarVariableWidget import *



class KMatrixTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(KMatrixTreeModel, self).__init__(parent)

	row = []
	row.append("row/column ")
	
	for i in data.getCoordinates().getActives():
	    row.append( i.getId() )
        self.rootItem = TreeItem(row)
        
        self.addDataChildren(data, self.rootItem )
        
    def addDataChildren(self,data,parent):
        
        tuple = itertools.izip(  data.getCoordinates().getActives(),
                                 range(0,data.getCoordinates().noOfActive()))
        j = 0
        for var,i in tuple:
            
            textData=[]
            textData.append(var.getId())
            textData.append(data.getKMatrixElement(i,j))
            item=TreeItem(textData)
            parent.appendChild(item)
            j = j + 1
            
        return data
      
class msKineticOperatorWidget(msWidget):
    """
    graphical component for msZMat
    """

    def __init__(self, data, mainwindow):
        super(msKineticOperatorWidget, self).__init__(data,mainwindow)

        groupBox = QtGui.QGroupBox("Kinetic matrix")
        vbox = QtGui.QVBoxLayout()
        self.kMatrix = QtGui.QTreeView()
        vbox.addWidget(self.kMatrix  )

        hbox = QtGui.QHBoxLayout()
        self.labelKineticFunction = QtGui.QLabel("Kinetic function:")
        hbox.addWidget(self.labelKineticFunction)
        vbox.addLayout(hbox)
        hbox.addStretch(1)
        updateFieldBttn = QtGui.QPushButton("Recompute")
        updateFieldBttn.clicked.connect(self.evaluateField)
        hbox.addWidget(updateFieldBttn)
        groupBox.setLayout(vbox)
        self.vbox.addWidget(groupBox)
        self.evaluateField()
	    
        groupBox = QtGui.QGroupBox("Generalized coordinates")
        vbox = QtGui.QVBoxLayout()
        self.synchronization = QtGui.QCheckBox("synchronization/coordinates")
        vbox.addWidget(self.synchronization)
        
        self.gencoors = msGeneralizedCoordinatesWidget(self.data.getCoordinates() , self.mainwindows )
        self.gencoors.listView.clicked.connect(self.selectCoor)
        vbox.addWidget(self.gencoors )    
        try:
            self.gencoors.widgetScalVar.slider.valueChanged[int].connect(self.updateCoorChange)
            self.gencoors.widgetScalVar.valueSpinBox.valueChanged[float].connect(self.updateCoorChange)
        except:
            pass
	  
	groupBox.setLayout(vbox)  
	self.vbox.addWidget(groupBox)
	
	self.groupbox.setTitle("Kinetic operator")
        self.setMaximumSize(550,450)
        self.setMinimumSize(550,450)

    def evaluateField(self):               
        try:
	    self.labelKineticFunction.setText("Kinetic function= " + str(self.data.kineticFunction()) )
	    model = KMatrixTreeModel(self.data)
	    self.kMatrix.setModel(model)
        except:
	    self.labelKineticFunction.setText("Kinetic function: not defined yet" )
    
    def selectCoor(self):               
        self.buttonPlot.setVisible(1)
        self.indiceCoor = self.gencoors.listView.currentIndex().row()
        self.coor =self.gencoors.listView.currentIndex().internalPointer().getRawdata()

    def updateCoorChange(self):
        if self.synchronization.isChecked():
            self.evaluateField()
            self.data.setDynamicDofs()
            self.data.getSystem().computeCartCoordinates()

    def clickListView(self):
        self.widgetScalVar.data = self.listView.currentIndex().internalPointer().getRawdata()
        self.widgetScalVar.updateCoordinate( )
        self.widgetScalVar.slider.valueChanged[int].connect(self.updateListView)
        self.widgetScalVar.valueSpinBox.valueChanged[float].connect(self.updateListView)
        self.hbox.addWidget(self.widgetScalVar)

    def updateListView(self):
        model = VariablesTreeModel(self.data.getCoordinates(),self)
        self.listView.setModel(model) 
        self.data.setDynamicDofs()        
        self.data.getSystem().computeCartCoordinates()

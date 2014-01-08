"""
    Created on Sat Aug 24 16:39:31 2013
    
    @author: chico
    """

from libimpact import *
from PySide import QtCore, QtGui
from widgets.msGeneralizedCoordinatesWidget import *
from TreeModels import *
#from PopUpBox import *
import matplotlib.pyplot as plt
from TreeModels.simpletreemodel import * 

class QoiTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(QoiTreeModel, self).__init__(parent)

        self.rootItem = TreeItem(("Id","Value"))
        self.addDataChildren(data, self.rootItem )
    
    def addDataChildren(self,data,parent):
        for child in data:
            textData=[]
            textData.append(str(child).split(",")[0][1:])
            textData.append(str(child).split(",")[-1][:-1])
            item=TreeItem(textData,parent)
            parent.appendChild(item)
     
        return data
      
class msPredictiveModelWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msPredictiveModelWidget, self).__init__(data,mainwindows)
        
        """ First create the Units group box
            """
        
        hbox=QtGui.QHBoxLayout()
        self.buttonEvaluate = QtGui.QPushButton("compute QoIs")
        self.buttonEvaluate.clicked.connect(self.evaluateQoIs)
        hbox.addWidget(self.buttonEvaluate)
        
        self.buttonUpdate = QtGui.QPushButton("update parameters")
        self.buttonUpdate.clicked.connect(self.data.updateParametersModification)
        hbox.addWidget(self.buttonUpdate)
        
        hbox.addStretch(1)
        
        self.qoisList = QtGui.QTreeView()
        self.qoisList.setModel(QoiTreeModel(data.getQois()))
        hbox.addWidget(self.qoisList)
        
        self.vbox.addLayout(hbox)
        
        self.vbox.addStretch(1)
        
        self.vbox.addWidget(QtGui.QCheckBox("synchronization"))
        
        self.gencoors = msGeneralizedCoordinatesWidget(self.data.getScenarioVariables() , self.mainwindows )
        self.gencoors.listView.clicked.connect(self.selectCoor)
        self.vbox.addWidget(self.gencoors)
        self.gencoors.groupbox.setTitle("&Scenario variables")
        try:
            self.gencoors.widgetScalVar.slider.valueChanged[int].connect(self.updateCoorChange)
            self.gencoors.widgetScalVar.valueSpinBox.valueChanged[float].connect(self.updateCoorChange)
        except:
            pass
        
        self.paramSpace = msGeneralizedCoordinatesWidget(self.data.getParameterSpace() , self.mainwindows )
        self.paramSpace.listView.clicked.connect(self.selectParam)
        self.vbox.addWidget(self.paramSpace)
        self.paramSpace.groupbox.setTitle("&Parameters space")
        try:
            self.paramSpace.widgetScalVar.slider.valueChanged[int].connect(self.updateParamChange)
            self.paramSpace.widgetScalVar.valueSpinBox.valueChanged[float].connect(self.updateParamChange)
        except:
            pass
        
        self.groupbox.setTitle("&Predictive model")
        self.setMinimumHeight(self.gencoors.height()+self.paramSpace.height()+250)
        self.setMaximumHeight(self.gencoors.height()+self.paramSpace.height()+250)
        self.setMinimumWidth(550)
        self.setMaximumWidth(550)
    
    #self.evaluateField()
    
    def selectCoor(self):
        #self.buttonPlot.setVisible(1)
        self.indiceCoor = self.gencoors.listView.currentIndex().row()
        self.coor =self.gencoors.listView.currentIndex().internalPointer().getRawdata()
    
    def updateCoorChange(self):
        #self.evaluateQoIs()
        pass
    
    def selectParam(self):
        #self.buttonPlot.setVisible(1)
        self.indiceParam = self.paramSpace.listView.currentIndex().row()
        self.param =self.paramSpace.listView.currentIndex().internalPointer().getRawdata()
    
    def updateParamChange(self):
        self.data.updateParametersModification()
    
    def evaluateQoIs(self):
        self.data.predict()
        self.qoisList.setModel(QoiTreeModel(self.data.getQois()))
    
    def plot1d(self):
        fig, axes = plt.subplots(1, 1)
        axes.plot(self.data.getXSample1d(self.indiceCoor,200),self.data.getYSample1d(self.indiceCoor,200))
        axes.set_xlabel(self.coor.getId() + " [" + self.coor.getUnitStr() + "]" )
        axes.set_ylabel(self.data.getId() + " [" + self.data.getUnit().getStr() + "]")
        fig.show()
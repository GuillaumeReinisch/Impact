"""
Created on Sat Aug 24 16:30:01 2013

@author: chico
"""
from libimpact import *
from PySide import QtCore, QtGui
from msScalarVariableWidget import *
from TreeModels.TreeMapperModel import *
from TreeModels.simpletreemodel import *        

class VariablesTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(VariablesTreeModel, self).__init__(parent)

        self.rootItem = TreeItem(("Id","Value","Min","Max","Dq","dq","unit"))
        self.addDataChildren(data, self.rootItem )
        self.filter="msScalarVariable"
    
    def addDataChildren(self,data,parent):
        listchildren = data.getChildren()
        for child in listchildren:
            try:
                textData=[]
                textData.append(child.getId())
                textData.append(child.getValue())
                textData.append(child.getMin())
                textData.append(child.getMax())
                textData.append(child.getDq())
                textData.append(child.getdq())
                textData.append(child.getUnitStr())
                item=TreeItem(textData,parent)
                item.setRawdata(child)
                parent.appendChild(item)
                #self.addDataChildren(child,parent.child(parent.childCount()-1))
            except:
                pass
        return data


class msGeneralizedCoordinatesWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msGeneralizedCoordinatesWidget, self).__init__(data,mainwindows)
        
        self.listView = QtGui.QTreeView()
        self.listView.clicked.connect(self.clickListView)
        model = VariablesTreeModel(self.data,self)
        self.listView.setModel(model)   
        self.hbox = QtGui.QHBoxLayout()
        self.hbox.addWidget(self.listView)
        self.vbox.addLayout(self.hbox)
        self.setMinimumHeight( 170)
        self.setMaximumHeight(170)
        
        self.setMinimumWidth(450)
        self.setMaximumWidth(450)
        self.widgetScalVar = msScalarVariableWidget(self.data.getVariable(0) ,self.mainwindows)
    
    def clickListView(self):
  
        self.widgetScalVar.data = self.listView.currentIndex().internalPointer().getRawdata()
        self.widgetScalVar.updateCoordinate( )
        self.widgetScalVar.slider.valueChanged[int].connect(self.updateListView)
        self.widgetScalVar.valueSpinBox.valueChanged[float].connect(self.updateListView)
        self.hbox.addWidget(self.widgetScalVar)

    def updateListView(self):
        model = VariablesTreeModel(self.data,self)
        self.listView.setModel(model)        

    def deleteCoor(self,var):
        pass

"""
Created on Mon Sep  9 10:38:14 2013

@author: chico
"""
from libimpact import *
from PySide import QtCore, QtGui
from widgets.msWidget import *
from TreeModels.TreeMapperModel import *
from Dialogs.simpleDialogs import *
from TreeModels.simpletreemodel import *

class ParamTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(ParamTreeModel, self).__init__(parent)

        self.rootItem = TreeItem(("Id","Value","Unit","Comment"))
        self.addDataChildren(data, self.rootItem )
        self.filter="msParam"
    
    def addDataChildren(self,data,parent):
        listchildren = data.getChildren()
        for child in listchildren:
            try:
                textData=[]
                textData.append(child.getId())
                textData.append(child.getValue())
                textData.append(child.getUnit())
                textData.append(child.getAbstract())
                item=TreeItem(textData,parent)
                item.setRawdata( child )
                parent.appendChild(item)
                self.addDataChildren(child,parent.child(parent.childCount()-1))
            except:
                pass
        return data


class msParamsManagerWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msParamsManagerWidget, self).__init__(data,mainwindows)
        
       
        self.ParamList = QtGui.QTreeView()
        
        model = ParamTreeModel(self.data)
        self.ParamList.setModel(model)
        self.ParamList.setMaximumWidth(480)
        self.ParamList.setMinimumWidth(480)
        vbox  =  QtGui.QVBoxLayout(self)
        vbox.addWidget(self.ParamList)
        self.ParamList.clicked.connect(self.queryParam)
        
        self.frame = QtGui.QFrame()
        hbox       =  QtGui.QHBoxLayout()  
        updateParamButton = QtGui.QPushButton("update")
        hbox.addWidget( updateParamButton )
        
        vbox.addWidget(self.frame)
        
        self.vbox.addLayout(vbox)
        self.setMinimumHeight(200)
        self.setMaximumHeight(200)
        self.setMinimumWidth(550)
        self.setMaximumWidth(550)
        self.groupbox.setTitle("&Parameters manager")

    def setParam(self,value):
        data = self.ParamList.currentIndex().internalPointer().getRawdata()
        self.data.getParent().setParameter(data.getId(),str(value))
    
    def queryParam(self):
        u = msUnit.New()
        data = self.ParamList.currentIndex().internalPointer().getRawdata()
        u.set(data.getUnit())
        data = self.ParamList.currentIndex().internalPointer().getRawdata()
        widget = queryStringDialog("set parameter",data.getId()+" in "+self.data.getParent().getUnits().getStrFrom( u ),  data.getValue(), self.setParam )
        widget.vbox.insertWidget(0,QtGui.QLabel(data.getAbstract()))
        widget.exec_()
        self.mainwindows.reload()

   
"""Created the 2013-08-25 14:33 

@author:
"""
from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from TreeModels.TreeMapperModel import *
from TreeModels.simpletreemodel import *

class ElementsTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(ElementsTreeModel, self).__init__(parent)

        self.rootItem = TreeItem(("Id","x","y","z","mass"))
        self.addDataChildren(data, self.rootItem )
        self.filter="msScalarVariable"
    
    def addDataChildren(self,data,parent):
        listchildren = data.getChildren()
        for child in listchildren:
            try:
                textData=[]
                textData.append(child.getId())
                textData.append(child.getX())
                textData.append(child.getY())
                textData.append(child.getZ())
                item=TreeItem(textData,parent)
                item.setRawdata(child)
                parent.appendChild(item)
                #self.addDataChildren(child,parent.child(parent.childCount()-1))
            except:
                pass
        return data

class DofModel(TreeModel):

    def __init__(self, data, parent=None):
        super(DofModel, self).__init__(parent)

        self.rootItem = TreeItem(("no","value"))
        list = data.getDofs()
        i = 0
        for dof in list:
            textData=[]
            textData.append(str(i))
            textData.append(dof)           
            item=TreeItem(textData,parent)
            self.rootItem.appendChild(item) 
            i = i + 1
  

class msEntityWidget(msWidget):
    """
    graphical component for msZMat
    """
    def __init__(self, objectdb, mainwindow):
        super(msEntityWidget, self).__init__(objectdb,mainwindow)

        massBox=QtGui.QGroupBox('mass repartition')

        hbox = QtGui.QGridLayout()
        hbox.addWidget(QtGui.QLabel("Total mass ["+self.data.getUnits().getMassStr()+"]:"),0,0)
        hbox.addWidget(QtGui.QLabel('{:.2e}'.format(self.data.getMass()) ),0,1)   
        hbox.addWidget(QtGui.QLabel("Moment of inertia: "),1,0)
        vbox = QtGui.QVBoxLayout()
        inertia = self.data.getEigenInertia()
        vbox.addWidget(QtGui.QLabel('x='+'{:.2e}'.format(inertia[0]) ))
        vbox.addWidget(QtGui.QLabel('y='+'{:.2e}'.format(inertia[1]) ))
        vbox.addWidget(QtGui.QLabel('z='+'{:.2e}'.format(inertia[2]) ))
        hbox.addLayout(vbox,1,1)

        massBox.setLayout(hbox)
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(massBox)
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)
        self.vbox.addSpacing(10)
        self.data.computeCartCoordinates()
        self.listView = QtGui.QTreeView()
        model = ElementsTreeModel(self.data,self)
        self.listView.setModel(model)   
        self.vbox.addSpacing(10)

        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(self.listView,4)
        #self.vbox.addWidget(QtGui.QLabel(str(self.data.noOfDof()) + " degrees of freedom:"))
        self.vboxDof = QtGui.QVBoxLayout()
        self.vboxDof.addWidget(QtGui.QLabel("Dofs"))
        self.listViewDof = QtGui.QListWidget()
        list = self.data.getDofs()
        i = 0
        for dof in list:        
            self.listViewDof.addItem(str(i) +": " + str(dof) )
            i = i + 1

        self.listViewDof.clicked.connect(self.clickDofList)
        self.vboxDof.addWidget(self.listViewDof)
        self.groupBoxSetDof = QtGui.QGroupBox("set Dof ")
        vbox = QtGui.QVBoxLayout()
        self.lineEdit = QtGui.QLineEdit()
        vbox.addWidget(self.lineEdit)
        self.updateButton = QtGui.QPushButton("update")
        vbox.addWidget(self.updateButton)
        self.updateButton.clicked.connect(self.updateDof)
        self.groupBoxSetDof.setLayout(vbox)
        self.vboxDof.addWidget( self.groupBoxSetDof)
        hbox.addLayout(self.vboxDof,1)
        self.groupBoxSetDof.setVisible(0)
        self.vbox.addLayout(hbox)
        
        self.groupbox.setTitle("Entity")
        self.setMaximumSize(550,450)
        self.setMinimumSize(550,450)

    def clickDofList(self):
        self.groupBoxSetDof.setVisible(1)
        
    def updateDof(self):
        self.data.setDof(self.listViewDof.currentIndex().row(), float(self.lineEdit.text()) )
        self.mainwindows.clickTreeView()

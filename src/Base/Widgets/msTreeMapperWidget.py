"""
Created on Sat Aug 24 15:38:04 2013

@author: chico
"""

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from TreeModels.TreeMapperModel import *
import inspect
from Dialogs import *
from TreeModels.simpletreemodel import *

class MethodsTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(MethodsTreeModel, self).__init__(parent)

        self.rootItem = TreeItem(("Name","Abstract"))
        myobject = data
        
        for method in inspect.getmembers(myobject):
            
            helpstr=inspect.getdoc(method[1])
        
            if(not method[0][:1]=="_"):
                try:
                    if(len(helpstr.split(':'))>1):
                        helpstr=helpstr.split(':')[1].split('.')[0]
                except:
                    pass
                textData=[]
                textData.append(method[0])
                textData.append(helpstr.replace("\n","").replace("  ",""))
                item=TreeItem(textData,self.rootItem )
                item.setRawdata(method)
                self.rootItem.appendChild(item)

class msTreeMapperWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msTreeMapperWidget, self).__init__(data,mainwindows)
        
        label0 = QtGui.QLabel("Id:" + data.getId() )
        self.vbox.addWidget(label0)
        """
        self.vbox.addWidget(label0)
        self.listMethods = QtGui.QTreeView()
        self.listMethods.clicked.connect(self.execMethod)
        self.listMethods.setModel( MethodsTreeModel(self.data,self.listMethods ) ) 
        self.vbox.addWidget(self.listMethods)
	"""
        self.setMinimumSize(550,100)
        self.setMaximumSize(550,100)
        #print label1.size().width()
        self.groupbox.setTitle("Tree mapper")

    def execMethod(self):
        """ Execute a method of the object selected in the tree
        
        method : pointer to the method to execute
        """
        widget=msMethodCallDialog( self.listMethods.currentIndex().internalPointer().getRawdata()[1],self.mainwindows.rootObject,self.mainwindows)
        widget.exec_()
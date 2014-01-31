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
from Dialogs.simpleDialogs import *
import sys
from msExceptionWidget  import *
from Misc.Misc            import *

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
        
        label0 = QtGui.QLabel("Id : " + data.getId() )
        self.vbox.addWidget(label0)
        
        hbox = QtGui.QHBoxLayout()
        setIdBttn = msPushButtonWithDoc("Set id","setId",self.data,mainwindows.webView)
        setIdBttn.clicked.connect(self.setId)
        hbox.addWidget(setIdBttn)
        
        setComment = msPushButtonWithDoc("Set comment","setComment",self.data,mainwindows.webView)
        setComment.clicked.connect(self.setComment)
        hbox.addWidget(setComment)
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)
        
        hbox = QtGui.QHBoxLayout()
        sanityBttn = msPushButtonWithDoc("Sanity check","sanityCheck",self.data,mainwindows.webView)
        sanityBttn.clicked.connect(self.sanityCheck)
        hbox.addWidget(sanityBttn)
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)
        
        hbox = QtGui.QHBoxLayout()
        addChildBttn = msPushButtonWithDoc("Add child","addChild",self.data,mainwindows.webView)
        addChildBttn.clicked.connect(self.addChild)
        hbox.addWidget(addChildBttn)
        
        rmChildBttn = QtGui.QPushButton("remove child")
        hbox.addWidget(rmChildBttn)
        rmChildBttn.setEnabled(0)
        
        transfChildBttn = QtGui.QPushButton("transfert child")
        hbox.addWidget(transfChildBttn)
        transfChildBttn.setEnabled(0)
        
        deepCopyBttn = QtGui.QPushButton("Deep copy")
        deepCopyBttn.setEnabled(0)
        hbox.addWidget(deepCopyBttn)
        hbox.addStretch(1)
        self.vbox.addLayout(hbox)
        
        
        """
        self.vbox.addWidget(label0)
        self.listMethods = QtGui.QTreeView()
        self.listMethods.clicked.connect(self.execMethod)
        self.listMethods.setModel( MethodsTreeModel(self.data,self.listMethods ) ) 
        self.vbox.addWidget(self.listMethods)
	"""
        self.setMinimumSize(550,150)
        self.setMaximumSize(550,150)
        #print label1.size().width()
        self.groupbox.setTitle("Tree mapper")
        
    def sanityCheck(self):
      
        try:
	    self.data.sanityCheck()
	except msException as e:
	    dialog = msExceptionDialog(e)
	    dialog.exec_()
	except:
	    message = str(sys.exc_info())
            self.mainwindows.reportException(sys.exc_info())
	    
    def addChild(self):
        widget=msMethodCallDialog( self.data.addChild,self.mainwindows.rootObject,self.mainwindows)
        widget.exec_()
 
    def setId(self):
        widget = queryStringDialog("set the object's id","id : ", self.data.getId(), self.data.setId )
        widget.exec_()
        self.mainwindows.reload()
 
    def setComment(self):
        widget = queryStringDialog("set the object's comment","comment : ", self.data.getComments(), self.data.setComments )
        widget.exec_()
        self.mainwindows.reload()
        
    def execMethod(self):
        """ Execute a method of the object selected in the tree
        
        method : pointer to the method to execute
        """
        widget=msMethodCallDialog( self.listMethods.currentIndex().internalPointer().getRawdata()[1],self.mainwindows.rootObject,self.mainwindows)
        widget.exec_()
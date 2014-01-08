
from libimpact import *
from PySide import QtCore, QtGui
import inspect
import itertools
from TreeModels.TreeMapperModel import *

class queryTreeMapperDialog(QtGui.QDialog):
    
    def __init__(self,  title, invit, filter, rootelement, functionOk, parent):
        super(queryTreeMapperDialog,self).__init__(None)
        
        self.functionOk = functionOk
        self.setWindowTitle(title)
        
        self.parent = parent
        self.vbox = QtGui.QVBoxLayout()
        self.vbox.addWidget(QtGui.QLabel( invit ))
        
        self.treeview = QtGui.QTreeView()
        self.vbox.addWidget(self.treeview)
        model = TreeMapperModel(rootelement,filter)
        self.treeview.setModel(model)
        
        hbox=QtGui.QHBoxLayout()
        hbox.addStretch(1)
        """
            bttn = QtGui.QPushButton("cancel")
            bttn.clicked.connect(self.cancel)
            hbox.addWidget(bttn)
            """
        bttn = QtGui.QPushButton("ok")
        bttn.clicked.connect(self.ok)
        hbox.addWidget(bttn)
        self.vbox.addLayout(hbox)
        self.setLayout(self.vbox)

    
    def ok(self):
        self.variable = self.treeview.currentIndex().internalPointer().getRawdata()
        self.parent.returnVar = self.functionOk(self.variable)
        self.close()
    
    def cancel(self):
        self.close()

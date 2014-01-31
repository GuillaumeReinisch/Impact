"""
    Created on Sat Aug 24 15:38:04 2013
    
    @author: chico
    """

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from Dialogs.simpleDialogs import *
import inspect
import itertools
import re
import functools
from TreeModels.TreeMapperModel import *
from TreeModels.simpletreemodel import *

class CoeffThreeBodyTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(CoeffThreeBodyTreeModel, self).__init__(data,"msTreeMapper",parent)

        self.rootItem = TreeItem(("name",
                                  "value"))
        self.addDataChildren(data, self.rootItem )
        
    def addDataChildren(self,data,parent):
        
        tuple = itertools.izip(  data.getCoefficientsName(),
                               data.getCoefficients())
        line=data.getAttributeInString("Efficiencies")
        for couple in line.split(" ")[:-1]:
            textData=[]
            textData.append(couple.split(":")[0])
            textData.append(couple.split(":")[1])
            item=TreeItem(textData)
            #item.setRawdata(att)
            parent.appendChild(item)
            
        return data

class msReactionThreeBodyWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msReactionThreeBodyWidget, self).__init__(data, mainwindows)
        
        self.vbox.addWidget(QtGui.QLabel("Three bodies efficiency"))
        
        self.listCoeffs = QtGui.QTreeView()
        self.listCoeffs.setModel( CoeffThreeBodyTreeModel(data) )
        self.vbox.addWidget(self.listCoeffs)

        self.setMinimumSize(300,200 )

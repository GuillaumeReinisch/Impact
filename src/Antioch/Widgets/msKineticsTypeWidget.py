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

class CoeffReactionTreeModel(TreeModel):
    def __init__(self, data, parent=None):
        super(CoeffReactionTreeModel, self).__init__(data,"msTreeMapper",parent)

        self.rootItem = TreeItem(("name",
                                  "value"))
        self.addDataChildren(data, self.rootItem )
        
    def addDataChildren(self,data,parent):
        
        coeffs = data.getCoefficients()

        for coef in coeffs:
            
            textData=[]
            textData.append(coef.key() )
            textData.append( coeffs[coef.key()] )
            item=TreeItem(textData)
            #item.setRawdata(att)
            parent.appendChild(item)
            
        return data

class msKineticsTypeWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msKineticsTypeWidget, self).__init__(data, mainwindows)
        
        self.vbox.addWidget(QtGui.QLabel("Coefficients"))
        self.listCoeffs = QtGui.QTreeView()
        self.listCoeffs.setModel( CoeffReactionTreeModel(data) )
        self.vbox.addWidget(self.listCoeffs)
        
        self.groupbox.setTitle("Kinetic type")
        
        self.setMinimumSize(300,200 )

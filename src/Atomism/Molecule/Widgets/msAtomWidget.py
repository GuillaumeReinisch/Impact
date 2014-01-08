"""Created the 2013-08-25 14:33
    
    @author:
    """
import sys

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *

from xml.dom import minidom


class msAtomWidget(msWidget):
    """
        graphical component for msZMat
        """
    def __init__(self, data, mainwindow):
        super(msAtomWidget, self).__init__(data,mainwindow)
        
        doc = minidom.parse("elements_ODB.xml")
        root = doc.documentElement
        self.vbox.addWidget(QtGui.QLabel(data.getName()))
            
        dict={}

        listElements = root.getElementsByTagName('atom')
        for element in listElements:
            if element.getAttribute('id') == data.getName():
                for current in element.getElementsByTagName('scalar'):
                    id = current.getAttribute('dictRef')
                    v  = current.childNodes[0].nodeValue
                    dict[id]=v
            
        gridBox=QtGui.QGridLayout()
        i = 0
        for k, v in dict.iteritems():
            gridBox.addWidget(QtGui.QLabel(k.split(":")[1]),i,0)
            gridBox.addWidget(QtGui.QLabel(str(v)),i,1)
            i = i+1
        self.vbox.addLayout(gridBox)
        
        self.groupbox.setTitle("Atom (unit consistency not implemented yet below))")
        self.setMinimumSize(550,300 )
        self.setMaximumSize(550,300 )
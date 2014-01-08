"""Created the 2013-08-25 14:33 

@author:
"""
import sys

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *
from xml.dom import minidom

class msZMatWidget(msWidget):
    """
    graphical component for msZMat
    """
    def __init__(self, objectdb, mainwindow):
        super(msZMatWidget, self).__init__(objectdb,mainwindow)

        self.zmatTextEdit = QtGui.QTextEdit()
        for line in self.data.getString().split('\n'):
            self.zmatTextEdit.append(line)

        hbox=QtGui.QHBoxLayout()
        vbox=QtGui.QVBoxLayout()
        vbox.addWidget( self.zmatTextEdit )
        self.loadFromCartBttn = QtGui.QPushButton("load from xyz")
        vbox.addWidget(self.loadFromCartBttn)
        hbox.addLayout(vbox)
        vbox=QtGui.QVBoxLayout()
        self.loadBodBttn = QtGui.QPushButton("Load structure from BOD")
        self.loadBodBttn.clicked.connect(self.loadFromBod)
        vbox.addWidget(self.loadBodBttn)
        self.viewBttn = QtGui.QPushButton("watch")
        self.viewBttn.clicked.connect(self.watch)
        self.updateBttn = QtGui.QPushButton("update structure")
        self.updateBttn.clicked.connect(self.updateStructure)
        vbox.addWidget(self.updateBttn)
        vbox.addWidget(self.viewBttn)
        vbox.addStretch(1)
        hbox.addLayout(vbox)
        self.vbox.addLayout( hbox  )
        self.setMaximumSize(550,200)
        self.setMinimumSize(550,200)
  
    def updateStructure(self):

        text = str(self.zmatTextEdit.toPlainText()).replace('\n', ' ')
        try:
            self.data.set(text)
        except:
            self.mainwindows.reportException(sys.exc_info())

        self.mainwindows.reload()
   
    def watch(self):

        self.mainwindows.visu=msVisualizerZMat.New()
        try:
            self.mainwindows.visu.launch(self.data)
            #self.mainwindows.visu.watch(self.data)
        except:
            self.mainwindows.reportException(sys.exc_info())

    def loadFromBod(self):
        filename, filtr = QtGui.QFileDialog.getOpenFileName(self,
                                                            "Choose a file name", '.', "cml (*.cml)")
        if not filename:
            return

        doc = minidom.parse(filename)
        root = doc.documentElement
        listElements = root.getElementsByTagName('atom')
        names= msVector_string()
        xs = msVector_double()
        ys = msVector_double()
        zs = msVector_double()
        for element in listElements:
            names.push_back(str(element.getAttribute('elementType')) )
            xs.push_back(self.data.getUnits().convert("Angstrom", float(element.getAttribute('x3')) ))
            ys.push_back(self.data.getUnits().convert("Angstrom", float(element.getAttribute('y3')) ))
            zs.push_back(self.data.getUnits().convert("Angstrom", float(element.getAttribute('z3')) ))
        self.data.setFromCartesians(names,xs,ys,zs)
        self.mainwindows.reload()


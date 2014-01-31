"""Created the 2013-08-25 14:33
    
    @author:
    """
import sys

from libimpact import *
from PySide import QtCore, QtGui
from msWidget import *

from xml.dom import minidom


class msElementWidget(msWidget):
    """
        graphical component for msZMat
        """
    def __init__(self, data, mainwindow):
        super(msElementWidget, self).__init__(data,mainwindow)
        
        self.vbox.addWidget(QtGui.QLabel("Mass= "+str(data.getMass()) + " "+data.getUnits().getMassStr() ))
        
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QLabel("Position ["+data.getUnits().getLengthStr()+"]: "))
        vbox =  QtGui.QVBoxLayout()
	vbox.addWidget(QtGui.QLabel(str(data.getX())))
	vbox.addWidget(QtGui.QLabel(str(data.getY())))
	vbox.addWidget(QtGui.QLabel(str(data.getZ())))
	hbox.addLayout(vbox)
	
        self.vbox.addLayout(hbox)
        self.groupbox.setTitle("Element")
        self.setMinimumSize(350,200 )
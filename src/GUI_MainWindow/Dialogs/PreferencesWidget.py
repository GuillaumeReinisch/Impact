from libimpact import *
from PySide import QtCore, QtGui
from widgets.msWidget import *
from Dialogs.queryTreeMapperDialog import *
import inspect
import itertools


class GeneralGroupBox(QtGui.QGroupBox):

    def __init__(self, parent=None):
        super(GeneralGroupBox,self).__init__(parent)

        self.gridBox = QtGui.QGridLayout()
        self.gridBox.addWidget(QtGui.QLabel("impact's installation directory"),1,1)
        
        self.docDirectory = "../doc/"
        self.installDirLineEdit = QtGui.QLineEdit(self.docDirectory)
        self.gridBox.addWidget(self.installDirLineEdit ,1,2)
        
        bttn = QtGui.QPushButton("browser")
        self.gridBox.addWidget(bttn,1,3)
        
        self.setLayout(self.gridBox)
        self.setMinimumSize(500,500)
        self.setMaximumSize(500,500)
        
        
class DocumentationGroupBox(QtGui.QGroupBox):

    def __init__(self, parent=None):
        super(GeneralGroupBox,self).__init__(parent)

        self.gridBox = QtGui.QGridLayout()
        self.gridBox.addWidget(QtGui.QLabel("directory scanned:"),1,1)
        
        self.docDirectory = "../include/"
        self.installDirLineEdit = QtGui.QLineEdit(self.docDirectory)
        self.gridBox.addWidget(self.installDirLineEdit ,1,2)
        
        bttn = QtGui.QPushButton("run doxygen")
        self.gridBox.addWidget(bttn,1,3)
        
        self.setLayout(self.gridBox)
        self.setMinimumSize(500,500)
        self.setMaximumSize(500,500)
        
class PreferencesWidget(QtGui.QDialog):

    def __init__(self,parent=None):
        super(PreferencesWidget,self).__init__(parent)


	
        self.vbox=QtGui.QVBoxLayout()
        tab = QtGui.QTabWidget()
        self.general = GeneralGroupBox(parent)
        tab.addTab( self.general, "general" )
        self.vbox.addWidget(tab)
        """
        bttn = QtGui.QPushButton("cancel")
        bttn.clicked.connect(self.cancel)
        hbox.addWidget(bttn)
        """
        bttn = QtGui.QPushButton("ok")
        bttn.clicked.connect(self.ok)
        hbox=QtGui.QHBoxLayout()
        hbox.addStretch(1)
        hbox.addWidget(bttn)
        self.vbox.addLayout(hbox)
        self.setLayout(self.vbox)

    def ok(self):
	self.installDir = self.general.installDirLineEdit.text()
        self.close()

    def cancel(self):
        self.close()

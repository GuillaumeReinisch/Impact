
from libimpact import *
from PySide import QtCore, QtGui
import inspect
import itertools            

class queryDoubleDialog(QtGui.QDialog):

    def __init__(self,  title, invit , variable, functionOk, parent=None):
        super(queryDoubleDialog,self).__init__(parent)

        self.variable = variable
        self.functionOk = functionOk
        self.setWindowTitle(title)

        self.vbox = QtGui.QVBoxLayout()
        self.vbox.addWidget(QtGui.QLabel( invit ))
        self.lineEdit = QtGui.QLineEdit(str(variable))
        self.vbox.addWidget(self.lineEdit)

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
        self.variable = float( self.lineEdit.text() )
        self.functionOk(self.variable)
        self.close()

    def cancel(self):
        self.close()

class queryIntDialog(QtGui.QDialog):
    
    def __init__(self,  title, invit , variable, functionOk, parent=None):
        super(queryIntDialog,self).__init__(parent)
        
        self.variable = variable
        self.functionOk = functionOk
        self.setWindowTitle(title)
        
        self.vbox = QtGui.QVBoxLayout()
        self.vbox.addWidget(QtGui.QLabel( invit ))
        self.lineEdit = QtGui.QLineEdit(str(variable))
        self.vbox.addWidget(self.lineEdit)
        
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
        self.variable = int( self.lineEdit.text() )
        self.functionOk(self.variable)
        self.close()
    
    def cancel(self):
        self.close()


class query2IntDialog(QtGui.QDialog):
    
    def __init__(self,  title,functionOk, parent=None):
        super(query2IntDialog,self).__init__(parent)
        
        self.functionOk = functionOk
        self.setWindowTitle(title)
        
        self.vbox = QtGui.QVBoxLayout()
        
        hbox = QtGui.QHBoxLayout()
        self.Label1 = QtGui.QLabel( "Default invit" )
        hbox.addWidget(self.Label1)
        self.Edit1 = QtGui.QLineEdit("0")
        hbox.addWidget(self.Edit1)
        self.vbox.addLayout(hbox)
        
        hbox = QtGui.QHBoxLayout()
        self.Label2 = QtGui.QLabel( "Default invit" )
        hbox.addWidget(self.Label2)
        self.Edit2 = QtGui.QLineEdit("0")
        hbox.addWidget(self.Edit2)
        self.vbox.addLayout(hbox)
        
        hbox=QtGui.QHBoxLayout()
        hbox.addStretch(1)
        bttn = QtGui.QPushButton("ok")
        bttn.clicked.connect(self.ok)
        hbox.addWidget(bttn)
        self.vbox.addLayout(hbox)
        self.setLayout(self.vbox)
    
    def ok(self):
        self.functionOk(int( self.Edit1.text() ) , int( self.Edit2.text() ) )
        self.close()
    
    def cancel(self):
        self.close()
        
        
        
class queryStringDialog(QtGui.QDialog):

    def __init__(self,  title, invit , variable, functionOk, parent=None):
        super(queryStringDialog,self).__init__(parent)

        self.variable = variable
        self.functionOk = functionOk
        self.setWindowTitle(title)

        self.vbox = QtGui.QVBoxLayout()
        self.vbox.addWidget(QtGui.QLabel( invit ))
        self.lineEdit = QtGui.QLineEdit(str(variable))
        self.vbox.addWidget(self.lineEdit)

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
        self.variable = self.lineEdit.text() 
        self.functionOk(str(self.variable))
        self.close()

    def cancel(self):
        self.close()


class queryStringIntDialog(QtGui.QDialog):
    
    def __init__(self,  title, invit1 , variable1, invit2, variable2, functionOk, parent=None):
        super(queryStringIntDialog,self).__init__(parent)
        
        self.functionOk = functionOk
        self.setWindowTitle(title)
        
        self.vbox = QtGui.QVBoxLayout()
        self.vbox.addWidget(QtGui.QLabel( invit1 ))
        self.lineEdit1 = QtGui.QLineEdit(str(variable1))
        self.vbox.addWidget(self.lineEdit1)
        
        self.vbox.addWidget(QtGui.QLabel( invit2 ))
        self.lineEdit2 = QtGui.QLineEdit(str(variable2))
        self.vbox.addWidget(self.lineEdit2)
        
        hbox=QtGui.QHBoxLayout()
        hbox.addStretch(1)

        bttn = QtGui.QPushButton("ok")
        bttn.clicked.connect(self.ok)
        hbox.addWidget(bttn)
        self.vbox.addLayout(hbox)
        self.setLayout(self.vbox)
    
    def ok(self):
        self.variable1 = self.lineEdit1.text()
        self.variable2 = int(self.lineEdit2.text())
        
        self.functionOk(self.variable1,self.variable2)
        self.close()
    
    def cancel(self):
        self.close()
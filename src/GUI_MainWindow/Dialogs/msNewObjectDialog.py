"""
    Created on Sat Aug 24 16:39:31 2013
    
    @author: chico
    """

from libimpact import *
from PySide import QtCore, QtGui
import inspect
import functools

from msMethodCallDialog import *
"""
class LineEditCompletion(QtGui.QLineEdit):
    def __init__(self,parent):
        super(LineEditCompletion, self).__init__()
        self.parent = parent
        self.pos = 0

    def keyPressEvent(self,e):
        super(LineEditCompletion, self).keyPressEvent(e)
        if e.key() == QtCore.Qt.Key_Down:
            try:
                self.parent.listClasses.item(self.pos).setSelected(1)
                #self.setText(str(self.parent.listClasses.currentItem().text() ))
                self.pos = self.pos + 1
            except:
                pass

"""
class msNewObjectDialog(QtGui.QDialog):
    
    def __init__(self,mainwindow):
        super(msNewObjectDialog, self).__init__()

        self.mainwindow =mainwindow
        self.initUI()
    
    def initUI(self):
        
        vbox = QtGui.QVBoxLayout()
        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QLabel("type name:"))
        self.lineEdit = QtGui.QLineEdit()
        self.lineEdit.textChanged.connect(self.textChanged)
        self.lineEdit.returnPressed.connect(self.returnPressed)
        hbox.addWidget(self.lineEdit)
        vbox.addLayout(hbox)
        self.listClasses = QtGui.QListWidget()
        #vbox.addWidget(self.listClasses)
        vbox.addWidget(QtGui.QLabel("press enter for completion or validation"))
        self.setLayout(vbox)

    def textChanged(self):
        self.updateListClasses()
    
    def returnPressed(self):

        try:
             pos = QtGui.QCursor.pos()
             self.menu.popup(QtCore.QPoint(pos.x()-20,pos.y()-10))
        except:
            pass

    def updateListClasses(self):
        self.listClasses.clear()
        self.menu = QtGui.QMenu()
        self.addClassDerivedFrom(msTreeMapper)

    def addClassDerivedFrom(self,cls):
        """ Create the 'New' menu: list the class available for a new object
        (i.e. deriving from msTreeMapper). The menu's item are listed as the 
        class heritance diagramm. A recursive scheme is used.
        """
        if(len(cls.__subclasses__())==0):
            return
        name = str(cls).split('.')[1].split('\'')[0]

        for cls in cls.__subclasses__():
            name =  str(cls)
            if( len(name.split('\''))>1 ):
                name = name.split('\'')[1]
            if( len(name.split('.'))>1 ):
                name = name.split('.')[1]        
            
            action = QtGui.QAction(name, self,
                   statusTip="create a new object",
                   triggered=functools.partial(self.newObject, cls))
                
            if name.find(self.lineEdit.text())>=0:
                #self.listClasses.addItem(name)
                self.menu.addAction(action)

            self.addClassDerivedFrom(cls)
        #newAction.setMenu(self.menu)
        
    def newObject( self , object_cls ):
        """ Menu New-\object_cls
        
        object_cls : pointer to the class of the object 
        """
        self.close()
        widgetNew=msMethodCallDialog(object_cls.New,self.mainwindow.rootObject,self.mainwindow)
        widgetNew.exec_()

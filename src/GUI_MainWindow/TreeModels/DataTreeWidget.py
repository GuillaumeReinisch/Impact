

from PySide import QtCore, QtGui, QtWebKit

from libimpact import *
from TreeMapperModel import *
from Misc.msPlotter1D import *

from os import listdir
from os.path import isfile, join
import re
import inspect
import functools

def getDoxygenName(name):
    s1 = re.sub('(.)([A-Z][a-z]+)',r'\1_\2',name)
    return re.sub('([a-z0-9])([A-Z])',r'\1_\2',s1).lower()

def get_class_that_defined_method(meth,class0):
    for cls in inspect.getmro(class0):
        if meth[0] in cls.__dict__: return cls
    return None

class DataTreeWidget(QtGui.QTreeView):
    
    def __init__(self, widget ):
        super(DataTreeWidget, self).__init__()
        self.widget = widget
        self.clicked.connect(self.clickTreeView)
        self.setDragDropMode(QtGui.QAbstractItemView.DragDrop)
        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.setDragEnabled(1)
        self.viewport().setAcceptDrops(1)
        self.setDropIndicatorShown(1)
        self.docDirectory = "../doc/html/"
        
    def update(self,data):
        self.data = data
        self.modelTree = TreeMapperModel( data ,"msTreeMapper")
        self.setModel(  self.modelTree  )
    
    def reload(self):

        msLogger.enterFunction("void DataTreeWidget::reload(self)","")

        try:
            myobject = self.myobject
        except:
            msLogger.exitFunction("void DataTreeWidget::reload(self)")
            return

        self.widget.mainwindow.methodsMenu.clear()
        self.widget.mainwindow.methodsMenuPopUp.clear()
    
        action = QtGui.QAction("&Popup methods box", self,
                       shortcut=QtGui.QKeySequence(self.tr("Ctrl+Alt+P")),
                       statusTip= "popup the methods box",
                       triggered= self.widget.mainwindow.popupMethods )

        self.widget.mainwindow.methodsMenu.addAction(action)
        
        self._loadMenu(myobject)

        msLogger.write(DEBUG,"Update the Attributes panel")
        self.model = AttributTreeModel(myobject)
        self.widget.attributsList.setModel(self.model)

        msLogger.write(DEBUG,"Update the methods panel")
        
        self.model = MethodTreeModel(myobject)
        self.widget.methodsList.setModel(self.model)
    
        msLogger.write(DEBUG,"Update the widgets container")
        self.widget.mainwindow.msWidgetsContainer.update(myobject)
        
        msLogger.write(DEBUG,"Update the documentation")
        last = self.widget.mainwindow.getDoxygenName(myobject.getType()).split(':')
        self._loadHtmlDocumentation(last[-2])
        
        msLogger.exitFunction("void DataTreeWidget::reload(self)")


    def findInClass(self, cls0,myobject):
      
	for cls in cls0.__subclasses__():
	        name = str(cls).split('.')[1].split('\'')[0]
            
                if(name == myobject.__class__.__name__):
                    self.class0 =cls
                    
		self.findInClass(cls,myobject)
	 
      
    def _loadMenu(self,myobject):
        msLogger.enterFunction("void _loadMenu()","")
        msLogger.write(DEBUG,"Look for "+ myobject.__class__.__name__)
        
        try:
	    class0 = globals()[myobject.__class__.__name__]
	    self.class0 = class0
	except:
            self.findInClass(msTreeMapper,myobject)
                        
        menu = self.widget.mainwindow.methodsMenuPopUp
        
        list = inspect.getmro(self.class0)
        for cls in list[::-1]:
            
            name = str(cls)
            name=re.findall(r'\'(.+?)\'',name)[0]
            if "." in name:
                name=name.split(".")[-1]
            
            newAction = menu.addAction(name)
            submenu = QtGui.QMenu(name,self)
            
            for method in inspect.getmembers(myobject):
                
                helpstr=inspect.getdoc(method[1])
                if(not method[0][:1]=="_"):
                    try:
                        if(len(helpstr.split(':'))>1):
                            helpstr=helpstr.split(':')[1].split('.')[0]
                    except:
                        pass
                
                    if( cls == get_class_that_defined_method( method, self.class0  ) ):
                        action = QtGui.QAction(method[0], self,
                                               statusTip= helpstr,
                                               triggered=functools.partial(self.widget.mainwindow.execMethod, method))
                        submenu.addAction(action)
                
                newAction.setMenu(submenu)

        msLogger.exitFunction("void _loadMenu()")

    def _loadHtmlDocumentation(self,name):
        msLogger.enterFunction("void _loadHtmlDocumentation()","")
        
        files = [ f for f in listdir(self.docDirectory) if isfile(join(self.docDirectory,f)) ]
        for file in files:
            
            if str( str(name)+".html") in file:
                self.widget.mainwindow.webView.load(self.docDirectory+file+"#details")
    
        msLogger.exitFunction("void _loadHtmlDocumentation()")

    def keyPressEvent(self,e):
        super(DataTreeWidget, self).keyPressEvent(e)
        self.clickTreeView()
    
    def getRawData(self):
        try:
            self.lastIndex = self.currentIndex()
            return self.currentIndex().internalPointer().getRawdata()
        except:
            #return self.modelTree.rootData
            pass

    def clickTreeView(self):
        # update the methods available
        self.myobject = self.getRawData()
        self.reload()

class DataWidget(QtGui.QWidget):
    
    def __init__(self, mainWindow):
        super(DataWidget, self).__init__()

        self.mainwindow = mainWindow
        
        mainvbox=QtGui.QVBoxLayout()
        splitter =  QtGui.QSplitter()
        splitter.setOrientation(QtCore.Qt.Vertical)
        
        self.dataTreeWidget = DataTreeWidget(self)
        splitter.addWidget(self.dataTreeWidget)
        
        self.attributsList = QtGui.QTreeView()
        splitter.addWidget(self.attributsList  )
        
        self.methodsList = QtGui.QTreeView()
        self.methodsList.clicked.connect(self.clickMethodView)
        splitter.addWidget(self.methodsList  )
        
        mainvbox.addWidget(splitter)
        self.setLayout(mainvbox)
        
        self.docDirectory = "../doc/html/"
    
    def setModel(self,model):
        self.dataTreeWidget.setModel(model)

    def reload(self):
        self.dataTreeWidget.reload()
    
    def update(self,data):
        self.dataTreeWidget.update(data)

    def clickTreeView(self):
        self.dataTreeWidget.clickTreeView()
    
    def clickMethodView(self):
        method = self.methodsList.currentIndex().internalPointer().getRawdata()
        try:
            name = self.methodsList.currentIndex().parent().internalPointer().getRawdata()
            if "." in name:
                name = name.split(".")[1]
            name = getDoxygenName(name)
            files = [ f for f in listdir(self.docDirectory) if isfile(join(self.docDirectory,f)) ]
            for file in files:
    
                if str( str(name)+".html") in file:
                    fileDoc = open(self.docDirectory+file, "r" )
                    for line in fileDoc:
                        if str(">"+method+"</a>") in line:
                            line = line[0:line.find(str(">"+method+"</a>"))]
                            line =line.split("href=\"")[-1]
                            line = line.split("\"")[0]
                            self.dataTreeWidget.widget.mainwindow.webView.load(self.docDirectory+line)
                            return
        except:
            name=method
            name = getDoxygenName(name)
            files = [ f for f in listdir(self.docDirectory) if isfile(join(self.docDirectory,f)) ]
            for file in files:
                
                if str( str(name)+".html") in file:
                    self.dataTreeWidget.widget.mainwindow.webView.load(self.docDirectory+file+"#details")

    

    def currentIndex(self):
        return self.dataTreeWidget.clickTreeView()

    def getRawData(self):
        return self.dataTreeWidget.getRawData()

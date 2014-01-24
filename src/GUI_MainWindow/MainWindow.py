
# Load common modules
import imp
import json
import sys,traceback
import math
import re
import functools
import inspect
import logging
import datetime
import os
from os import listdir
from os.path import isfile, join
from subprocess import call

# Load Pyside basic modules
from PySide import QtCore, QtGui, QtWebKit

try:
    with open('./config.json'):
        config = json.loads(open('./config.json').read())
except IOError:
    raise Exception("Can not open the configuration file, it is created when running impact_gui.py.")
   
# Load libimpact
impact = imp.load_dynamic("libimpact",config["LIB_INSTALL_PREFIX"]+"/libimpact.so")
from libimpact import *

from DockElements    import *
from Dialogs         import *
from Dialogs.msMethodCallDialog         import *
from Dialogs.msNewObjectDialog         import *
from Dialogs.ConfigurationWidget         import *
from TreeModels      import *
from ScriptTemplates.NewClassScriptTemplateWidget      import *
from ScriptTemplates.uiNewGraphicScriptTemplate      import *
from TreeModels.TreeMapperModel      import *
from Misc            import *
from LoadersSavers.msXmlLoader   import *
from LoadersSavers.msXmlWritter  import *


class MainWindow(QtGui.QMainWindow):
    
    rootObject = msTreeMapper.New()
    """ This class defines the main windows
    """
    def __init__(self):
        super(MainWindow, self).__init__()
        
        self.config = config
        
        # start impact logger
        self.Logger = msLogger
        self.Logger.setPriorityMin(Priority.INFO)
        self.Logger.startTreeRepresentation()
        
        self.initUI()
           
        # set python logger
        dialogHandler = Misc.QtStreamHandler(self.outStream,  self)
        logging.basicConfig(level=logging.DEBUG)
        logging.getLogger().addHandler(dialogHandler)
        
        # new object root
        self.rootObject.setId("workspace")
        self.setRootObject()
        
    
    @staticmethod
    def getObject():
        return MainWindow.rootObject
                    
    def initUI(self):
        """ initialize the graphical interface elements: 
        the script widget (central element), the toolbars and the dock's parts
        """
        self.setWindowTitle("impact development interface")
        self.setUnifiedTitleAndToolBarOnMac(True)        
        
        # script widget
        self.outStream = ""
        self.scriptWidget = ScriptWidget.uiScriptWidget(self)
        self.scriptWidget.setStdOut(self.outStream)
        self.setCentralWidget(self.scriptWidget)
                
        self.createMenusAndAction()
        #self.createToolBars()
        
        # dock's elements        
        self.createDockElements()
        
        sys.stdout = ScriptWidget.OutLog( self.outStream, sys.stdout)
        
        self.isInit = 1
        self.statusBar().showMessage("Ready")
        
                 
    def setRootObject( self ):
        """ load self.rootObject as root in the graphical interface
        """
        self.mainTreeModelObject = TreeMapperModel(self.rootObject,"msTreeMapper")
        self.mainTreeWidget.setModel(self.mainTreeModelObject)        


    def createDockElements(self):
        """ Create the dock's elements, initial arrangment:
        - in the left   : the tree tab (data,files,debug) and the attribut dock
        - in the bottom : Exception printer, output, and doc
        - in the right  : the graphical widgets of the selected object
        """
        
        # left dock
        mainTreeDock = QtGui.QDockWidget("", self)

        tab = QtGui.QTabWidget(mainTreeDock)
        self.mainTreeWidget = DataTreeWidget.DataWidget(self)
        tab.addTab( self.mainTreeWidget, "data structure")
        
        self.browserTreeWidget = QtGui.QTreeView()
        self.browserTreeWidget.clicked.connect(self.clickFileTreeView)
        self.browserModel = QtGui.QFileSystemModel()
        self.browserModel.setRootPath(QtCore.QDir.currentPath())
        self.browserTreeWidget.setModel(self.browserModel)
        tab.addTab(self.browserTreeWidget,"browser")
        
        self.debugWidget = DebugTreeWidget.DebugWidget(msLogger , self )
        tab.addTab(self.debugWidget, "debug")

        mainTreeDock.setWidget(tab)
        self.viewMenu.addAction(mainTreeDock.toggleViewAction())
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, mainTreeDock)
        mainTreeDock.setMinimumSize(350,mainTreeDock.height())
        
        # bottom dock
        outDock1   = QtGui.QDockWidget("Readers", self)
        #tab = QtGui.QTabWidget(mainTreeDock)
        self.outStream = QtGui.QTextEdit()
        self.scriptWidget.setStdOut(self.outStream)
        #tab.addTab(self.outStream,"out streams")
        outDock1.setWidget(self.outStream)
        outDock1.resize(350,200)
        self.viewMenu.addAction(outDock1.toggleViewAction())
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, outDock1)

        outDock2   = QtGui.QDockWidget("Doc", self)
        #docTab  = QtGui.QTabWidget()
        self.webView=QtWebKit.QWebView()
        
        try:
	    with open(self.config["DOC_INSTALL_PREFIX"]+"/html/index.html"):
                 self.webView.load(self.config["DOC_INSTALL_PREFIX"]+"/html/index.html#overview")    
        except IOError:
            QtGui.QMessageBox.information(self, "Documentation",
                    "The documentation is not iniliazed, please check the 'Documentation' panel of the next window and click 'run doxygen'.")
	    widget = ConfigurationWidget()
            widget.exec_()
            self.webView.load(self.config["DOC_INSTALL_PREFIX"]+"/html/index.html#overview")   
 
        #docTab.addTab(self.webView,"contextual")
        #tab.addTab(docTab,"documentation")
        outDock2.setWidget(self.webView)
        self.viewMenu.addAction(outDock2.toggleViewAction())
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, outDock2)
        
        # right doc        
        dockGUIobjects = QtGui.QDockWidget("Graphical interface", self)
        self.msWidgetsContainer = WidgetsContainer.WidgetsContainer(self)
        dockGUIobjects.setWidget(self.msWidgetsContainer)
        self.addDockWidget(QtCore.Qt.RightDockWidgetArea, dockGUIobjects)
        self.viewMenu.addAction(dockGUIobjects.toggleViewAction())
    
    def clickTreeView(self):
        self.mainTreeWidget.clickTreeView()

    def reload(self):
        model = TreeMapperModel(self.rootObject,"msTreeMapper",self.mainTreeWidget.expandPtrCheckBox.isChecked())
        self.mainTreeWidget.setModel(model)
        self.mainTreeWidget.reload()
    
    def clickFileTreeView(self):
        """ action executed when the file tree view is clicked (i.e. an object 
        selected). If text file is selected, it is open in the editor
        """
        filename = self.browserModel.fileInfo(self.browserTreeWidget.currentIndex()).absoluteFilePath()
        file = open( filename, "r" )

        index = self.scriptWidget.addTab(filename.split('/')[-1])
        #text = QtGui.QTextEdit()
        text = self.scriptWidget.textEdits[-1]
        i=0
        for line in file:
            text.insertPlainText( line )
            i = i + 1
            
        file.close()
        text.filename = filename
        self.statusBar().showMessage("Open '%s'" % filename, 2000)
    
    def popupMethods(self):
        try:
            pos = QtGui.QCursor.pos()
            self.methodsMenuPopUp.popup(QtCore.QPoint(pos.x()-20,pos.y()-10))
        except:
            pass

    def getDoxygenName(self,name):
        s1 = re.sub('(.)([A-Z][a-z]+)',r'\1_\2',name)
        return re.sub('([a-z0-9])([A-Z])',r'\1_\2',s1).lower()
              
    def saveTree(self):
        """ Save the tree object
        """
        """ Save the current script in the associated file
        """
        filename, filtr = QtGui.QFileDialog.getSaveFileName(self,
                "Choose a file name", '.', "xml (*.xml)")
        if not filename:
            return
            
        writter = msXmlWritter.New()
        writter.save(MainWindow.getObject(),filename)
    
    def openTree(self):
        """ Save the tree object
        """
        filename, filtr = QtGui.QFileDialog.getOpenFileName(self,
                "Choose a file name", '.', "xml (*.xml)")
        if not filename:
            return
            
        loader = msXmlLoader.New()
        a = msTreeMapper.New()
        try:
            loader.open(a,filename)
        except:
            self.reportException("can not load the xml file")
            
        MainWindow.rootObject = a
        self.mainTreeWidget.update(a)
        self.debugWidget.update()
        
    """ 'script' menu actions
    """
    def saveScript(self):
        """ Save the current script in the associated file
        """
        text = self.scriptWidget.currentScript() 
        filename = text.filename
        if(filename==""):
            self.saveScriptAs()
            return
            
        file = QtCore.QFile(filename)
        if not file.open(QtCore.QFile.WriteOnly | QtCore.QFile.Text):
            QtGui.QMessageBox.warning(self, "Dock Widgets",
                    "Cannot write file %s:\n%s." % (filename, file.errorString()))
            return

        out = QtCore.QTextStream(file)
        QtGui.QApplication.setOverrideCursor(QtCore.Qt.WaitCursor)
        out << self.scriptWidget.tabWidget.currentWidget().toPlainText()
        QtGui.QApplication.restoreOverrideCursor()
        
        file.close()

    def saveScriptAs(self):
        """ Save the current script in a new file
        """
        filename, filtr = QtGui.QFileDialog.getSaveFileName(self,
                "Choose a file name", '.', "py (*.py)")
        if not filename:
            return

        file = QtCore.QFile(filename)
        if not file.open(QtCore.QFile.WriteOnly | QtCore.QFile.Text):
            QtGui.QMessageBox.warning(self, "Dock Widgets",
                    "Cannot write file %s:\n%s." % (filename, file.errorString()))
            return

        out = QtCore.QTextStream(file)
        QtGui.QApplication.setOverrideCursor(QtCore.Qt.WaitCursor)
        out << self.scriptWidget.tabWidget.currentWidget().toPlainText()
        QtGui.QApplication.restoreOverrideCursor()
        file.write(str(out))
        file.close()
        i = self.scriptWidget.tabWidget.currentIndex()
        self.scriptWidget.tabWidget.setTabText(i,filename.split('/')[-1])
        text = self.scriptWidget.currentScript() 
        text.filename = filename
        self.statusBar().showMessage("Saved '%s'" % filename, 2000)
        
    def openScript(self):
        """ Open a script from a new file
        """
        filename, filtr = QtGui.QFileDialog.getOpenFileName(self,
                "Choose a file name", '.', "py (*.py)")
        if not filename:
            return

        file = open( filename, "r" )
        
        index = self.scriptWidget.addTab(filename.split('/')[-1])
        #text = QtGui.QTextEdit()
        text = self.scriptWidget.textEdits[-1]
        i=0
        for line in file:
            text.insertPlainText( line )
            i = i + 1
            
        file.close()
        text.filename = filename
        self.statusBar().showMessage("Open '%s'" % filename, 2000)
        
    def importFileInImpact(self):
        text = self.scriptWidget.currentScript() 
        filename = text.filename
        if(filename==""):
            print "you need to save the file before importing it in impact"
            return
	print filename,'../python/'+filename.split("/")[-1]
	
        os.symlink(filename, '../python/'+filename.split("/")[-1]) 
        
    def launchConfiguration(self):
        """ launch the preferencies panel
        """
        widget = ConfigurationWidget()
        widget.exec_()
        
    def newScript(self):
        """ New script empty
        """
        self.scriptWidget.addTab(datetime.datetime.now().strftime("%H:%M")+"*")
        text = self.scriptWidget.currentScript() 
        text.filename = ""

    def newScriptFromTemplate(self):
        """ New script from a template
        """        
        widgetNew=uiNewClassScriptTemplate( self.scriptWidget )
        widgetNew.exec_()
        
    def newGraphicScriptFromTemplate(self):
        """ New script from a template
        """        
        widgetNew=uiNewGraphicScriptTemplate( self.scriptWidget )
        widgetNew.exec_()
        
    def runScript(self,mycode):
        """ New script from a template
        """          
        exec(mycode,globals(),globals())
        self.createMenusAndAction()
        self.reload()
 
    def runDoxygen(self):
        """ New script from a template
        """          
        call(["doxygen", "../doc/Doxyfile"])
 
 
    def insertVarInScript(self):
        """ insert the selected object in the main tree in the script at the
        cursor position
        """
        myobject = self.mainTreeWidget.getRawData()
        name     = myobject.getFullVariableName()
        path     = "MainWindow.getObject()"
        
        if (len(name.split('-')) >1):
            for i in name.split('-')[1:]:
                path = path  + ".getChild(\""+i+"\")"
                
        self.scriptWidget.currentScript().insertPlainText(path)
        

    def newObject( self , object_cls ):
        """ Menu New-\object_cls
        
        object_cls : pointer to the class of the object 
        """
        widgetNew=msMethodCallDialog(object_cls.New,self.rootObject,self)
        widgetNew.exec_()

    def newObjectPopup( self ):
        """ Menu New
        """
        widgetNew=msNewObjectDialog(self)
        widgetNew.exec_()
          
    def execMethod( self , method ):
        """ Execute a method of the object selected in the tree
        
        method : pointer to the method to execute
        """
        widget=msMethodCallDialog(method[1],self.rootObject,self)
        widget.exec_()

    def findInTree(self):
        pass

    def findInDebug(self):
        pass

    def findInScript(self):
        pass
    
    def about(self):
        QtGui.QMessageBox.about(self, "About impact development interface",
                "The interface is designed to facilitate the use and "
                "development of code based on the library impact.")
                
    def runscript(self):
        self.scriptWidget.runBtn()
        
    def runscriptLine(self):
        self.scriptWidget.runLine()
        
    def runscriptSelected(self):
        self.scriptWidget.runSelected()
        self.createMenusAndAction()
        
        
    def createToolBars(self):
        self.fileToolBar = self.addToolBar("File")
        self.fileToolBar.addAction(self.saveAct)
        #self.fileToolBar.addAction(self.printAct)

        self.editToolBar = self.addToolBar("Edit")
        #self.editToolBar.addAction(self.undoAct)

    def reportException(self,error):
        logging.exception("an exception occured")
        ret=QtGui.QMessageBox.critical(self,'Exception',
                 'An exception occured, check out stream',
                QtGui.QMessageBox.Yes,
                QtGui.QMessageBox.Yes)
                
    
    
    def createActions(self):
     
        """ Create actions to link with the menu items that are not context
        dependant. Action and menu that are context dependant are created 
        in the function createActionMenuContextual
        """
                
        self.saveAct = QtGui.QAction(QtGui.QIcon(':/images/save.png'),
                "&Save database...", self, shortcut=QtGui.QKeySequence(self.tr("Ctrl+Alt+S")),
                statusTip="Save the database",
                triggered=self.saveTree)
                
        self.openAct = QtGui.QAction(QtGui.QIcon(':/images/save.png'),
                "&Open database...", self, shortcut=QtGui.QKeySequence(self.tr("Ctrl+Alt+O")),
                statusTip="Load a database",
                triggered=self.openTree)
   
        self.newObjectAct = QtGui.QAction(QtGui.QIcon(':/images/new.png'),
                                "&Quick launch...", self, shortcut=QtGui.QKeySequence(self.tr("Ctrl+Alt+N")),
                                statusTip="Create a new object using a dialog box",
                                triggered=self.newObjectPopup)
       
        self.newScriptAct = QtGui.QAction(QtGui.QIcon(':/images/new.png'),
                "&New...", self, shortcut=QtGui.QKeySequence.New,
                statusTip="New empty script",
                triggered=self.newScript)
  
        self.newScriptTemplateAct = QtGui.QAction(QtGui.QIcon(':/images/new.png'),
                "new class template", self, shortcut=QtGui.QKeySequence(self.tr("Ctrl+Shift+N")),
                statusTip="insert template for a new impact object",
                triggered=self.newScriptFromTemplate)
	
        self.newGraphicScriptTemplateAct = QtGui.QAction(QtGui.QIcon(':/images/new.png'),
                "new graphical element template", self, 
                statusTip="insert template for a new graphic componant",
                triggered=self.newGraphicScriptFromTemplate)
                       
        self.openScriptAct = QtGui.QAction(QtGui.QIcon(':/images/open.png'),
                "&Open...", self, shortcut=QtGui.QKeySequence.Open,
                statusTip="open a file",
                triggered=self.openScript)
                
        self.saveScriptAct = QtGui.QAction(QtGui.QIcon(':/images/save.png'),
                "&Save...", self, shortcut=QtGui.QKeySequence.Save,
                statusTip="Save the current script",
                triggered=self.saveScript)

        self.saveAsScriptAct = QtGui.QAction(QtGui.QIcon(':/images/save.png'),
                "SaveAs...", self, shortcut=QtGui.QKeySequence.SaveAs,
                statusTip="Save the current script as a new file",
                triggered=self.saveScriptAs)
       
        self.closeScriptAct = QtGui.QAction(QtGui.QIcon(':/images/close.png'),
                "&Close", self, shortcut=QtGui.QKeySequence.Close,
                statusTip="Close the current script", triggered=self.scriptWidget.closeCurrent)
	
	self.importFileInImpactAct =  QtGui.QAction(
                "&Import file in impact", self,statusTip="import the file in impact workspace", triggered=self.importFileInImpact)
	
        self.preferencesAct = QtGui.QAction("&Configuration", self,
                statusTip="Configuration panels", triggered=self.launchConfiguration)
	 
        self.quitAct = QtGui.QAction("&Quit", self, shortcut="Ctrl+Q",
                statusTip="Quit the application", triggered=self.close)

        self.aboutAct = QtGui.QAction("&About", self,
                statusTip="Show the application's About box",
                triggered=self.about)
        
        self.copyScriptAct = QtGui.QAction(QtGui.QIcon(':/images/copy.png'),
                "&Copy", self, shortcut=QtGui.QKeySequence.Copy,
                statusTip="Copy text selection", triggered=self.scriptWidget.copy)
        
        self.cutScriptAct = QtGui.QAction(QtGui.QIcon(':/images/cut.png'),
                "&Cut", self, shortcut=QtGui.QKeySequence.Cut,
                statusTip="Cut text selection", triggered=self.scriptWidget.cut)
                
        self.pasteScriptAct = QtGui.QAction(QtGui.QIcon(':/images/paste.png'),
                "&Paste", self, shortcut=QtGui.QKeySequence.Paste,
                statusTip="Paste text", triggered=self.scriptWidget.paste)
                
        self.undoScriptAct = QtGui.QAction(QtGui.QIcon(':/images/undo.png'),
                "&Undo", self, shortcut=QtGui.QKeySequence.Undo,
                statusTip="Undo last action", triggered=self.scriptWidget.undo)
                
        self.redoScriptAct = QtGui.QAction(QtGui.QIcon(':/images/redo.png'),
                "&Redo", self, shortcut=QtGui.QKeySequence.Redo,
                statusTip="Redo last action", triggered=self.scriptWidget.redo)
                
        self.deleteScriptAct = QtGui.QAction(QtGui.QIcon(':/images/delete.png'),
                "&Delete", self, shortcut=QtGui.QKeySequence.Delete,
                statusTip="Delete selection", triggered=self.scriptWidget.delete)
 
        self.findTree = QtGui.QAction(QtGui.QIcon(':/images/find.png'),
                                      "&Find in data structure", self, shortcut=QtGui.QKeySequence(self.tr("Ctrl+Alt+F")),
                                      statusTip="Find keyword in the data structure", triggered=self.findInTree,enabled=0)

        self.findDebug = QtGui.QAction(QtGui.QIcon(':/images/find.png'),
                              "&Find in debug", self, shortcut=QtGui.QKeySequence(self.tr("Ctrl+Alt+G")),
                              statusTip="Find keyword in the debug report", triggered=self.findInDebug,enabled=0)

        self.findScript = QtGui.QAction(QtGui.QIcon(':/images/find.png'),
                               "&Find in script", self, shortcut=QtGui.QKeySequence(self.tr("Ctrl+F")),
                               statusTip="Find keyword in the debug report", triggered=self.findInScript,enabled=0)
    

        self.insertVarInScriptAct = QtGui.QAction(QtGui.QIcon(':/images/insertVarInScriptAct.png'),
                "&Get selected var", self, shortcut=QtGui.QKeySequence(self.tr("Ctrl+I")),
                statusTip="Get the variable selected in the tree list into the script", triggered=self.insertVarInScript )
        
        self.runScriptAct = QtGui.QAction(QtGui.QIcon(':/images/run.png'),
                "Run", self, shortcut=QtGui.QKeySequence(QtCore.Qt.Key_F5),
                statusTip="Run the current script", triggered=self.runscript)
 
        self.runLineScriptAct = QtGui.QAction(QtGui.QIcon(':/images/runLine.png'),
                "Run current line", self, shortcut=QtGui.QKeySequence(QtCore.Qt.Key_F3),
                statusTip="Run the current line of the script", triggered=self.runscriptLine)
 
        self.runSelectedScriptAct = QtGui.QAction(QtGui.QIcon(':/images/runSelected.png'),
                "Run the selection", self, shortcut=QtGui.QKeySequence(QtCore.Qt.Key_F4),
                statusTip="Run the current selection of the script", triggered=self.runscriptSelected)
	
	self.runDoxygenAct = QtGui.QAction(QtGui.QIcon(':/images/run.png'),
                "Run doxygen", self, shortcut=QtGui.QKeySequence(QtCore.Qt.Key_F1),
                statusTip="update the documentation", triggered=self.runDoxygen)
 
    
    def overNewMenuItem(self,cls,a=""):
      
        msLogger.enterFunction("void overNewMenuItem(self,name)","")
        cls = str(cls)
        name = cls.split('.')[-1].split('\'')[0]
        name = self.getDoxygenName(str(name))
        docDirectory = config["DOC_INSTALL_PREFIX"]+"/html"
        print docDirectory
        files = [ f for f in listdir(docDirectory) if isfile(join(docDirectory,f)) ]
        for file in files:
            if str( str(name)+".html") in file:
                self.webView.load(docDirectory+"/"+file+"#details")
                return
    
        msLogger.exitFunction("void overNewMenuItem(self,name)")
    
    def createActionMenuContextual(self,menu,cls):
        """ Create the 'New' menu: list the class available for a new object
        (i.e. deriving from msTreeMapper). The menu's item are listed as the 
        class heritance diagramm. A recursive scheme is used.
        """
        if(len(cls.__subclasses__())==0):
            return
        name = str(cls).split('.')[1].split('\'')[0]
        
        newAction = menu.addAction(name)
        submenu = QtGui.QMenu(str(cls),self)
        #submenu.hovered.connect( functools.partial(self.overNewMenuItem,str(cls)) )

        for cls in cls.__subclasses__():
            name =  str(cls)
            if( len(name.split('\''))>1 ):
                name = name.split('\'')[1]
            if( len(name.split('.'))>1 ):
                name = name.split('.')[1]        
                
            action = QtGui.QAction(name, self,
                   statusTip=inspect.getdoc(cls),
                   triggered=functools.partial(self.newObject, cls)
                   )
            action.hovered.connect( functools.partial(self.overNewMenuItem,cls) )
            submenu.addAction(action)
            self.createActionMenuContextual(submenu,cls)
        newAction.setMenu(submenu)
        
    def createMenusAndAction(self):
        """ this function create the menu and their associated action 
        """
        self.createActions()    
        self.menuBar().clear()
        self.fileMenu = self.menuBar().addMenu("&File")
        self.fileMenu.addAction(self.saveAct)
        self.fileMenu.addAction(self.openAct)
        self.fileMenu.addSeparator()
        self.fileMenu.addAction(self.newScriptAct)
        self.fileMenu.addAction(self.openScriptAct)
        self.fileMenu.addAction(self.saveScriptAct)
        self.fileMenu.addAction(self.saveAsScriptAct)     
        self.fileMenu.addAction(self.closeScriptAct)
        self.fileMenu.addAction(self.importFileInImpactAct)
        self.fileMenu.addSeparator()
        self.fileMenu.addAction(self.preferencesAct)
        self.fileMenu.addSeparator()       
        self.fileMenu.addAction(self.quitAct)

        self.newobjMenu = self.menuBar().addMenu("&New object")
        self.createActionMenuContextual( self.newobjMenu , msTreeMapper)
        self.newobjMenu.addSeparator()
        self.newobjMenu.addAction(self.newObjectAct)
        
        self.methodsMenu = self.menuBar().addMenu("&Methods")
        self.methodsMenuPopUp = QtGui.QMenu()
        
        self.editionMenu = self.menuBar().addMenu("&Edit")
        self.editionMenu.addAction(self.undoScriptAct)
        self.editionMenu.addAction(self.redoScriptAct)
        self.editionMenu.addSeparator()
        
        self.editionMenu.addAction(self.cutScriptAct)
        self.editionMenu.addAction(self.copyScriptAct)
        self.editionMenu.addAction(self.pasteScriptAct)
        self.editionMenu.addAction(self.deleteScriptAct)    
        
        self.findMenu = self.menuBar().addMenu("&Find")
        self.findMenu.addAction(self.findTree)
        self.findMenu.addAction(self.findDebug)
        self.findMenu.addSeparator()
        self.findMenu.addAction(self.findScript)
        
        self.insertMenu = self.menuBar().addMenu("&Insert") 
        self.insertMenu.addAction(self.insertVarInScriptAct)
        self.insertMenu.addAction(self.newScriptTemplateAct)
        self.insertMenu.addAction(self.newGraphicScriptTemplateAct)
        
        self.runMenu = self.menuBar().addMenu("&Run") 
        self.runMenu.addAction(self.runScriptAct)
        self.runMenu.addAction(self.runSelectedScriptAct)
        self.runMenu.addAction(self.runLineScriptAct)
        self.runMenu.addSeparator()          
        self.runMenu.addAction(self.runDoxygenAct)
        
        self.viewMenu = self.menuBar().addMenu("&View")

        self.helpMenu = self.menuBar().addMenu("&Help")
        self.helpMenu.addAction(self.aboutAct)


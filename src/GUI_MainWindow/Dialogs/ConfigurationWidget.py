from libimpact import *
from PySide import QtCore, QtGui
from widgets.msWidget import *
from Dialogs.queryTreeMapperDialog import *
import inspect
import itertools
from   string import Template
import json
import functools  
from subprocess import call

class GeneralGroupBox(QtGui.QGroupBox):

    def __init__(self, parent=None):
        super(GeneralGroupBox,self).__init__(parent)

	self.installDir = ""
        self.docDir     = ""
        self.pythonDir  = ""
        
        self.vbox = QtGui.QVBoxLayout()
        
        gbox = QtGui.QGridLayout()
        
        gbox.addWidget(QtGui.QLabel("Install directory:"),1,1)
        self.installDirLabel = QtGui.QLineEdit(self.installDir)
        gbox.addWidget(self.installDirLabel,1,2)
        bttn = QtGui.QPushButton("change")
        bttn.clicked.connect( functools.partial(self.browse,self.setImpactInstallDir) )
        gbox.addWidget(bttn,1,3)
        
        gbox.addWidget(QtGui.QLabel("Doc directory:"),2,1)
        self.docDirLabel = QtGui.QLineEdit(self.docDir)
        gbox.addWidget(self.docDirLabel,2,2)
        bttn = QtGui.QPushButton("change")
        bttn.clicked.connect( functools.partial(self.browse,self.setDocDir) )
        gbox.addWidget(bttn,2,3)
        
        gbox.addWidget(QtGui.QLabel("Python directory"),3,1)
        self.pythonDirLabel = QtGui.QLineEdit(self.pythonDir)
        gbox.addWidget(self.pythonDirLabel,3,2)
        bttn = QtGui.QPushButton("change")
        bttn.clicked.connect( functools.partial(self.browse,self.setPythonDir) )
        gbox.addWidget(bttn,3,3)
        
        self.vbox.addLayout(gbox)
        
        self.vbox.addStretch(1)
        self.setLayout(self.vbox)
        self.setMinimumSize(500,500)
        self.setMaximumSize(500,500)
    
    def setImpactInstallDir(self,direc):
        self.installDir = direc
        self.installDirLabel.setText(direc)
        
    def setDocDir(self,direc):
        self.docDir = direc
        self.docDirLabel.setText(direc)

    def setPythonDir(self,direc):
        self.pythonDir = direc
        self.pythonDirLabel.setText(direc)       
        
    def browse(self,setMethod):
      
        dirname = QtGui.QFileDialog.getExistingDirectory(self,
                "Open Directory", '.', QtGui.QFileDialog.ShowDirsOnly)
	setMethod(dirname)
	
class DocumentationGroupBox(QtGui.QGroupBox):

    def __init__(self, parent=None):
        super(DocumentationGroupBox,self).__init__(parent)

        self.vbox = QtGui.QVBoxLayout()
        self.parent = parent
        self.installDocDirectory = ""
        self.installDirLineEdit = QtGui.QLineEdit( self.installDocDirectory )
        
        self.vbox.addWidget( QtGui.QLabel("Source directories to link for Doxygen:") )
        
        self.srcDocDirs = [] 
        hbox = QtGui.QHBoxLayout()
        self.listIncludeDir = QtGui.QListWidget()
        hbox.addWidget(self.listIncludeDir)
        
        vbox = QtGui.QVBoxLayout()
        addDirBttn = QtGui.QPushButton("add directory")
        addDirBttn.clicked.connect(self.queryAndAddIncludeDir)
        #rmDirBttn = QtGui.QPushButton("remove directory")
        vbox.addWidget(addDirBttn)
        vbox.addSpacing(20)
        bttn = QtGui.QPushButton("run doxygen")
        bttn.clicked.connect(self.runDoxygen)
        vbox.addWidget(bttn)
        
        #vbox.addWidget(rmDirBttn)
        vbox.addStretch(1)
        hbox.addLayout(vbox)
        
        self.vbox.addLayout(hbox)
        self.setLayout(self.vbox)
        self.setMinimumSize(500,500)
        self.setMaximumSize(500,500)

    def queryAndAddIncludeDir(self):
      
        dirname = QtGui.QFileDialog.getExistingDirectory(self,
                "Open Directory", '.', QtGui.QFileDialog.ShowDirsOnly)
	self.addIncludeDir(dirname)

    def addIncludeDir(self,dirname):
	self.listIncludeDir.addItem(dirname)
	self.srcDocDirs.append(dirname)

    def setIncludeDirs(self,dirnames):
        for i in dirnames:
	    self.addIncludeDir(i)
	
    def runDoxygen(self):
        filein = open(self.parent.general.pythonDir+'/ScriptTemplates/Doxyfile.py', 'r')
	fileout = open(self.parent.general.docDir+'/Doxyfile', 'w')
	src = Template( filein.read() )                
        d = dict(installdir=self.parent.general.docDir,
		 includedirs=' '.join(self.srcDocDirs),
                 )
        fileout.write(src.substitute(d))
	call(["doxygen",self.parent.general.docDir + "/Doxyfile"])
	
	
class ConfigurationWidget(QtGui.QDialog):

    def __init__(self,parent=None):
        super(ConfigurationWidget,self).__init__(parent)

        self.setWindowTitle("Configuration")
        
        self.vbox=QtGui.QVBoxLayout()
        self.tab = QtGui.QTabWidget()
        self.general = GeneralGroupBox(self)
        self.tab.addTab( self.general, "General" )
        self.documentation = DocumentationGroupBox(self)
        self.tab.addTab( self.documentation , "Documentation" )
        self.general.parent = self
        self.vbox.addWidget(self.tab)
        
        bttn = QtGui.QPushButton("ok")
        bttn.clicked.connect(self.ok)
        hbox=QtGui.QHBoxLayout()
        hbox.addStretch(1)
        hbox.addWidget(bttn)
        self.vbox.addLayout(hbox)
        self.setLayout(self.vbox)
        
        try:
            with open('./config.json'):
                self.config = json.loads(open('./config.json').read())
                self.general.setImpactInstallDir(self.config["CMAKE_INSTALL_PREFIX"])
                self.general.setDocDir(self.config["DOC_INSTALL_PREFIX"])
                self.general.setPythonDir(self.config["PYTHON_INSTALL_PREFIX"])
                includes=[]
                includes.append(self.config["CMAKE_INSTALL_PREFIX"]+"/include")
                includes.append(self.config["ANTIOCH_INCLUDE_PATH"])
                includes.append(self.config["QUESO_INCLUDE_PATH"])
                self.documentation.setIncludeDirs(includes)
        except IOError:
	    pass

    def ok(self):
	
	filein = open(self.general.pythonDir+'/ScriptTemplates/Doxyfile.py', 'r')
	fileout = open(self.general.docDir+'/Doxyfile', 'w')
	src = Template( filein.read() )                
        d = dict(installdir=self.general.installDir+"/doc",
		 includedirs=' '.join(self.documentation.srcDocDirs),
                 )
        fileout.write(src.substitute(d))
        
        self.config['CMAKE_INSTALL_PREFIX']= self.general.installDir
	self.config['DOC_INSTALL_PREFIX'] =  self.general.docDir
	self.config['DOC_INCLUDE_DIRS'] = ' '.join(self.documentation.srcDocDirs)
	self.config['PYTHON_INSTALL_PREFIX']= self.general.pythonDir
		  
	json.dump(self.config, open('./config.json', 'w'))
	
	self.close()
	

    def cancel(self):
        self.close()

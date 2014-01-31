from Misc import *

from PySide import QtCore, QtGui
from TreeModels.TreeMapperModel import *
from msWidget import *
from Dialogs.simpleDialogs import *
import functools
from TreeModels.simpletreemodel import *

class StyleModel(TreeModel):
    def __init__(self, data, parent=None):
        super(StyleModel, self).__init__(parent)
        
        self.rootItem = TreeItem(("Function's id","style","index"))
        
        zip = itertools.izip(data.ScalarFunctions.getElements(),
                             data.Styles.getValue(),data.Indexes.getValue()
                             )
        i = 0
        for fct,style,index in zip:
            textData=[]
            textData.append( fct.getId() )
            textData.append(style)
            textData.append(index)
            item = TreeItem(textData,self.rootItem )
            item.setRawdata(i)
            self.rootItem.appendChild(item)
            i = i + 1

class msPlotter1DWidget(msWidget):
    
    def __init__(self,data,mainwindows):
        super(msPlotter1DWidget, self).__init__(data, mainwindows)
        
        self.groupbox.setTitle("plotter 1D")

        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QLabel("noOfSample w/ x axis:"+str(data.Sampling.getValue())))
        bttn = QtGui.QPushButton("set")
        bttn.clicked.connect(self.setSampling)
        hbox.addStretch(1)
        hbox.addWidget(bttn)
        self.vbox.addLayout(hbox)

        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QLabel("x label:"))
        self.xTextEdit = QtGui.QLineEdit(self.data.xLabel.getValue())
        hbox.addWidget(self.xTextEdit)
        self.vbox.addLayout(hbox)

        hbox = QtGui.QHBoxLayout()
        hbox.addWidget(QtGui.QLabel("y label:"))
        self.yTextEdit = QtGui.QLineEdit(self.data.yLabel.getValue())
        hbox.addWidget(self.yTextEdit)
        self.vbox.addLayout(hbox)
        self.vbox.addSpacing(10)
        
        self.listStyles = QtGui.QTreeView()
        self.listStyles.clicked.connect( self.queryStyleIndex )
        self.listStyles.setModel( StyleModel(self.data) )
        self.vbox.addWidget(self.listStyles)
        
        hbox = QtGui.QHBoxLayout()
        bttn = QtGui.QPushButton("plot")
        bttn.clicked.connect(self.plot)
        hbox.addStretch(1)
        hbox.addWidget(bttn)
        self.vbox.addLayout(hbox)
        self.setMinimumSize(350,350   )

    def plot(self):
        self.data.xLabel.setValue(str(self.xTextEdit.text()))
        self.data.yLabel.setValue(str(self.yTextEdit.text()))
        self.data.plot()
    
    def setStyleIndex(self,label,index):
        self.data.setStyleIndex(self.listStyles.currentIndex().internalPointer().getRawdata(),label,index)
    
    def queryStyleIndex(self):
        widget = queryStringIntDialog("set the style","style label:",  "b-","coordinate index",  "0", self.setStyleIndex )
        widget.exec_()
        self.mainwindows.reload()

    
    def setSampling(self):
        T = self.data.Sampling.getValue()
        widget = queryIntDialog("set the sampling","enter the number of sample;",  T, self.data.setSampling )
        widget.exec_()
        self.mainwindows.reload()
        
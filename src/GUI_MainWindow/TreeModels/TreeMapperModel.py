
############################################################################
##
## Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
##
## This file is part of the example classes of the Qt Toolkit.
##
## This file may be used under the terms of the GNU General Public
## License version 2.0 as published by the Free Software Foundation
## and appearing in the file LICENSE.GPL included in the packaging of
## this file.  Please review the following information to ensure GNU
## General Public Licensing requirements will be met:
## http://www.trolltech.com/products/qt/opensource.html
##
## If you are unsure which license is appropriate for your use, please
## review the following information:
## http://www.trolltech.com/products/qt/licensing.html or contact the
## sales department at sales@trolltech.com.
##
## This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
## WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
##
############################################################################

from PySide import QtCore, QtGui
from libimpact import *
from Misc import *
from Misc.msPlotter1D import *
from TreeModels import simpletreemodel

import itertools
import inspect
import functools
import re

class TreeItem(object):
    def __init__(self, data, parent=None):
        self.parentItem = parent
        self.itemData = data
        self.childItems = []
        self.rawData= None

    def setRawdata(self,data):
        self.rawData=data
        
    def getRawdata(self):
        return self.rawData
        
    def appendChild(self, item):
        self.childItems.append(item)

    def child(self, row):
        return self.childItems[row]

    def childCount(self):
        return len(self.childItems)

    def columnCount(self):
        return len(self.itemData)

    def data(self, column):
        try:
            return self.itemData[column]
        except IndexError:
            return None

    def parent(self):
        return self.parentItem

    def row(self):
        if self.parentItem:
            return self.parentItem.childItems.index(self)

        return 0

class AttributTreeModel(simpletreemodel.TreeModel):
    def __init__(self, data, parent=None):
        super(AttributTreeModel, self).__init__(data,"msTreeMapper",parent)

        self.rootItem = TreeItem(("key","Value"))
        self.addDataChildren(data, self.rootItem )
        self.filter="msParam"
    
    def addDataChildren(self,data,parent):
        attributes = data.getAttributes()
        for att in attributes:
            textData=[]
            textData.append(att.split("=")[0])
            value = ""
            for elem in att.split("=")[1:-1]:
                value = value + elem +"="
            value = value + att.split("=")[-1]
            textData.append(value)
            item=TreeItem(textData,parent)
            item.setRawdata(att)
            parent.appendChild(item)
            
        return data

def get_class_that_defined_method(meth,class0):
    for cls in inspect.getmro(class0):
        if meth[0] in cls.__dict__: return cls
    return None

class MethodTreeModel(simpletreemodel.TreeModel):
    def __init__(self, data, parent=None):
        super(MethodTreeModel, self).__init__(data,"msTreeMapper",parent)
        
        self.rootItem = TreeItem(("classes","methods"))
        self.addDataChildren( data, self.rootItem )
    
    def findInClass(self, cls0,myobject):
      
	for cls in cls0.__subclasses__():
	        name = str(cls).split('.')[1].split('\'')[0]
            
                if(name == myobject.__class__.__name__):
                    self.class0 =cls
                    
		self.findInClass(cls,myobject)
	 
       
    def addDataChildren(self,myobject,parent):

        list=[]
        try:
	    class0 = globals()[myobject.__class__.__name__]
	    self.class0 = class0
	except:
            self.findInClass(msTreeMapper,myobject)
	
        list = inspect.getmro(self.class0)
        for cls in list[::-1]:
                
            name = str(cls)
            textData=[]
            name=re.findall(r'\'(.+?)\'',name)[0]
            if "." in name:
                name=name.split(".")[-1]
            textData.append(name)
            textData.append("")
            item=TreeItem(textData,parent)
            item.setRawdata(name)
            parent.appendChild(item)

            for method in inspect.getmembers(myobject):
                    
                helpstr=inspect.getdoc(method[1])
                if(not method[0][:1]=="_"):
                    try:
                        if(len(helpstr.split(':'))>1):
                            helpstr=helpstr.split(':')[1].split('.')[0]
                    except:
                        pass
                        
                    if( cls == get_class_that_defined_method( method,self.class0  ) ):
                            textData=[]
                            textData.append("")
                            textData.append(method[0].rstrip('\r\n'))
                            item2=TreeItem(textData,item)
                            item2.setRawdata(method[0].rstrip('\r\n'))
                            item.appendChild(item2)
        
        return myobject


class TreeMapperModel(QtCore.QAbstractItemModel):
    """ This class defines the model view for a msTreeMapper object as a 
    tree view. 
    """
        
    def __init__( self , data , filtertype ="none", expandPtr = 0, parent=None ):
        """ 
        ___init___( self, (msTreeMapper) data, (string) filtertype ) -> void
        
        data : msTreeMapper derived object
        filtertype : only object deriving from filtertype are enabled
        """
        super(TreeMapperModel, self).__init__()

        self.rootItem = TreeItem(("Variable","type","id"))
        self.filter=filtertype
        self.expandPtr = expandPtr
        textData=[]
        textData.append(data.getId())
        textData.append(data.getType().split(':')[-2])
        parent=self.rootItem
        item=TreeItem(textData,self.rootItem)
        item.setRawdata(data)   
        self.rootData = data
        self.rootItem.appendChild(item)
        self.addDataChildren(data, parent.child(parent.childCount()-1))

    
    def addDataChildren(self,data,parent):
        
        listChildren = data.getChildren()
        names = data.getVariableNames()
        tuple = itertools.izip(listChildren,names)
        for child,name in tuple:
            textData=[]
            textData.append(name)
            textData.append(child.getType().split(':')[-2])
            
            if(not child.isAffiliatedTo(data) ):
	        path = ""
	        for e in data.getSymbolicLinkPath(child).split('/')[2:]:
	            path = path + "/"+e
                textData[1] = path
                
            textData.append(child.getId())
                       
            item=TreeItem(textData,parent)
            item.setRawdata(child)
            parent.appendChild(item)
            if( not(child.isAffiliatedTo(data)) and (self.expandPtr) ):
	        self.addDataChildren(child,parent.child(parent.childCount()-1))
            if( child.isAffiliatedTo(data)):
                self.addDataChildren(child,parent.child(parent.childCount()-1))
                
        return data

    def columnCount(self, parent):
        if parent.isValid():
            return parent.internalPointer().columnCount()
        else:
            return self.rootItem.columnCount()

    def data(self, index, role):
        if not index.isValid():
            return None

        if role != QtCore.Qt.DisplayRole:
            return None

        item = index.internalPointer()

        return item.data(index.column())

    def rawdata(self, index):
        if not index.isValid():
            return None

        item = index.internalPointer()
        return item.getRawdata()
        
    def flags(self, index):
        if not index.isValid():
            return QtCore.Qt.NoItemFlags
        
        if(self.filter=="none"):
	    return QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable
	  
        if( self.rawdata(index).isDerivedFrom(self.filter)):
            return QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable
          
        return QtCore.Qt.ItemFlags

    def headerData(self, section, orientation, role):
        if orientation == QtCore.Qt.Horizontal and role == QtCore.Qt.DisplayRole:
            return self.rootItem.data(section)

        return None

    def index(self, row, column, parent):
        if not self.hasIndex(row, column, parent):
            return QtCore.QModelIndex()

        if not parent.isValid():
            parentItem = self.rootItem
        else:
            parentItem = parent.internalPointer()

        childItem = parentItem.child(row)
        if childItem:
            return self.createIndex(row, column, childItem)
        else:
            return QtCore.QModelIndex()

    def parent(self, index):
        if not index.isValid():
            return QtCore.QModelIndex()

        childItem = index.internalPointer()
        parentItem = childItem.parent()

        if parentItem == self.rootItem:
            return QtCore.QModelIndex()

        return self.createIndex(parentItem.row(), 0, parentItem)

    def rowCount(self, parent):
        if parent.column() > 0:
            return 0

        if not parent.isValid():
            parentItem = self.rootItem
        else:
            parentItem = parent.internalPointer()

        return parentItem.childCount()

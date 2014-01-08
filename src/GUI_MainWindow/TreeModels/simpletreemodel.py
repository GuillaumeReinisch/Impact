
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
import itertools
import simpletreemodel_rc


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


class TreeModel(QtCore.QAbstractItemModel):
    def __init__(self, data, filtertype="", parent=None):
        super(TreeModel, self).__init__(parent)
        """
        self.rootItem = TreeItem(("Variable","type","id"))
        self.filter=filtertype
        textData=[]
        
        textData.append(data.getId())
        textData.append(data.getType().split(':')[-2])
        parent=self.rootItem
        item=TreeItem(textData,self.rootItem)
        item.setRawdata(data)
        self.rootItem.appendChild(item)
        self.rootData = data
        self.addDataChildren(data, parent.child(parent.childCount()-1))
        """

    
    def addDataChildren(self,data,parent):

        listChildren = data.getChildren()
        names = data.getVariableNames()
        tuple = itertools.izip(listChildren,names)
        for child,name in tuple:
            textData=[]
            textData.append(name)
            textData.append(child.getType().split(':')[-2])
            if(not child.isAffiliatedTo(data) ):
                textData[1] = textData[1] + " pointer"
            textData.append(child.getId())
            item=TreeItem(textData,parent)
            item.setRawdata(child)
            parent.appendChild(item)
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
        try:
            if( self.rawdata(index).isDerivedFrom(self.filter)):
                return QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable
        except:
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

        try:
            return self.createIndex(parentItem.row(), 0, parentItem)
        except:
            pass
                
    def rowCount(self, parent):
        if parent.column() > 0:
            return 0

        if not parent.isValid():
            parentItem = self.rootItem
        else:
            parentItem = parent.internalPointer()

        return parentItem.childCount()

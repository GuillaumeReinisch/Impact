# -*- coding: utf-8 -*-
"""
Created on Fri Aug 23 21:51:53 2013

@author: chico
"""

from PySide import QtCore, QtGui
import sys

            
            
class PythonConsoleWidget(QtGui.QLineEdit):
    """PythonConsoleWidget(QtGui.QLineEdit)
    Provides a custom widget to accept Python expressions and emit output
    to other components via a custom signal.
    """
    __pyqtSignals__ = ("pythonOutput(const QString &)",)
    def __init__(self, output, parent = None):
        QtGui.QLineEdit.__init__(self, parent)
        self.history = []
        self.current = -1
        self.Output = output
        self.connect(self, QtCore.SIGNAL("returnPressed()"),
                     self.execute)
    def keyReleaseEvent(self, event):
        if event.type() == QtCore.QEvent.KeyRelease:
            if event.key() == QtCore.Qt.Key_Up:
                current = max(0, self.current - 1)
                if 0 <= current < len(self.history):
                    self.setText(self.history[current])
                    self.current = current
                event.accept()
            elif event.key() == QtCore.Qt.Key_Down:
                current = min(len(self.history), self.current + 1)
                if 0 <= current < len(self.history):
                    self.setText(self.history[current])
                else:
                    self.clear()
                self.current = current
                event.accept()
    def execute(self):
        qApp = QtGui.qApp
        self.expression = self.text()
        sys.stdout = OutLog( self.Output, sys.stdout)
        try:
            ldict = locals()
            
            exec(unicode(self.expression),globals(),globals())
            """
            print result
            self.Output.setText(result)
            
            self.emit(QtCore.SIGNAL("pythonOutput(const QString &)"),
                      QtCore.QString(result))
            """
            # Clear the line edit, append the successful expression to the
            # history, and update the current command index.
                     
            self.clear()
            self.history.append(self.expression)
            self.current = len(self.history)
        except:
            pass

# -*- coding: utf-8 -*-
"""
Created on Sat Aug 24 18:11:47 2013

@author: chico
"""

from PySide import QtCore, QtGui

class Python (QtGui.QSyntaxHighlighter):
    """Syntax highlighter for the Python language.
    """
    # Python keywords
    keywords = [
        'and', 'as', 'assert', 'break', 'class', 
        'continue', 'def', 'del', 'elif', 'else', 
        'except', 'exec', 'finally', 'for', 'from', 
        'global', 'if', 'import', 'in', 'is', 
        'lambda', 'not', 'or', 'pass', 'print', 'raise', 
        'return', 'try', 'while', 'with', 'yield', 'True', 
        'False', 'None' ]
  
    # Python operators
    operators = [
        '=',
        # Comparison
        '==', '!=', '<', '<=', '>', '>=',
        # Arithmetic
        '\+', '-', '\*', '/', '//', '\%', '\*\*',
        # In-place
        '\+=', '-=', '\*=', '/=', '\%=',
        # Bitwise
        '\^', '\|', '\&', '\~', '>>', '<<' ]
  
    # Python braces
    braces = [
        '\{', '\}', '\(', '\)', '\[', '\]' ]
    def __init__(self, document):
        QtGui.QSyntaxHighlighter.__init__(self, document)
  
        # Multi-line strings (expression, flag, style)
        # FIXME: The triple-quotes in these two lines will mess up the
        # syntax highlighting from this point onward
        classFormat = QtGui.QTextCharFormat()		
        classFormat.setForeground(QtCore.Qt.darkGreen)	
        classFormat.setFontItalic(1)
        self.tri_single = (QtCore.QRegExp("'''"), 1, classFormat )
        self.tri_double = (QtCore.QRegExp('"""'), 2, classFormat )
  
        classFormat = QtGui.QTextCharFormat()	
        classFormat.setFontWeight(QtGui.QFont.Bold)	
        classFormat.setForeground(QtCore.Qt.blue)
        
        rules = []
        rules += [(r'\b%s\b' % w, 0, classFormat)
            for w in Python.keywords]
                
        classFormat = QtGui.QTextCharFormat()	
        classFormat.setForeground(QtCore.Qt.red)
        
        rules += [(r'%s' % o, 0, classFormat)
            for o in Python.operators]
        rules += [(r'%s' % b, 0, classFormat)
            for b in Python.braces]
        
        
        classFormatStr = QtGui.QTextCharFormat()	
        classFormatStr.setForeground(QtCore.Qt.green)
        classFormatDefclass = QtGui.QTextCharFormat()	
        classFormatDefclass.setForeground(QtCore.Qt.red)
        classFormatComment = QtGui.QTextCharFormat()	
        classFormatComment.setForeground(QtCore.Qt.gray)       
        rules += [
            # Double-quoted string, possibly containing escape sequences
            (r'"[^"\\]*(\\.[^"\\]*)*"', 0, classFormatStr),
            # Single-quoted string, possibly containing escape sequences
            (r"'[^'\\]*(\\.[^'\\]*)*'", 0, classFormatStr ),
            # 'def' followed by an identifier
            (r'\bdef\b\s*(\w+)', 1, classFormatDefclass),
            # 'class' followed by an identifier
            (r'\bclass\b\s*(\w+)', 1, classFormatDefclass),
            # From '#' until a newline
            (r'#[^\n]*', 0,classFormatComment ),
            # From #! to new line
            (r'#![^\n]*', 0, classFormatComment ) ]
        
        # Build a QRegExp for each pattern
        self.rules = [(QtCore.QRegExp(pat), index, fmt)
            for (pat, index, fmt) in rules]
  
    def highlightBlock(self, text):
        """Apply syntax highlighting to the given block of text.
        """
        # Do other syntax formatting
        for expression, nth, format in self.rules:
            index = expression.indexIn(text, 0)
  
            while index >= 0:
                # We actually want the index of the nth match
                index = expression.pos(nth)
                length = len( expression.cap(nth) )
                self.setFormat(index, length, format)
                index = expression.indexIn(text, index + length)
  
        self.setCurrentBlockState(0)
  
        # Do multi-line strings
        in_multiline = self.match_multiline(text, *self.tri_single)
        if not in_multiline:
            in_multiline = self.match_multiline(text, *self.tri_double)
  
    def match_multiline(self, text, delimiter, in_state, style):
        """Do highlighting of multi-line strings. ``delimiter`` should be a
        ``QRegExp`` for triple-single-quotes or triple-double-quotes, and
        ``in_state`` should be a unique integer to represent the corresponding
        state changes when inside those strings. Returns True if we're still
        inside a multi-line string when this function is finished. """
        # If inside triple-single quotes, start at 0
        if self.previousBlockState() == in_state:
            start = 0
            add = 0
        # Otherwise, look for the delimiter on this line
        else:
            start = delimiter.indexIn(text)
            # Move past this match
            add = delimiter.matchedLength()
  
        # As long as there's a delimiter match on this line...
        while start >= 0:
            # Look for the ending delimiter
            end = delimiter.indexIn(text, start + add)
            # Ending delimiter on this line?
            if end >= add:
                length = end - start + add + delimiter.matchedLength()
                self.setCurrentBlockState(0)
            # No; multi-line string
            else:
                self.setCurrentBlockState(in_state)
                length = len(text) - start + add
            # Apply formatting
            self.setFormat(start, length, style)
            # Look for the next match
  
            start = delimiter.indexIn(text, start + length)
  
        # Return True if still inside a multi-line string, False otherwise
        if self.currentBlockState() == in_state:
            return True
        else:
            return False

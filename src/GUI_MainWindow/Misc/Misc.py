# -*- coding: utf-8 -*-
"""
Created on Fri Sep  6 09:34:59 2013

@author: chico
"""

import logging

class QtStreamHandler(logging.Handler):
    def __init__(self,  parent,  main):
        logging.Handler.__init__(self)
        self.parent = parent
        self.main = main
       
        self.textWidget = parent
        self.formater =logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')

    def emit(self,record):
        
        item = self.formater.format(record)
        self.textWidget.append(self.formater.format(record))
       

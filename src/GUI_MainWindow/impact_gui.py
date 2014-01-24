 
import json
import logging
import sys
"""
Load the json configuration file. 
If it does not exist, it is created from impact_config.h.
This is needed to add the python path in the sys.path to load impact's python files.
"""
try:
    with open('./config.json'):
        config = json.loads(open('./config.json').read())
except IOError:
    try:
        config_dic = {}
        with open('./impact_config.h') as fileConfigH:
	    lines = [ line for line in fileConfigH.readlines() if( (len(line.split(' '))>2) and (line.split(' ')[0]=="#define") ) ]
	    for line in lines:
	        config_dic[line.split(' ')[1]] = line.split(' ')[2]
            json.dump(config_dic, open('./config.json', 'w'))
            config = json.loads(open('./config.json').read())
    except IOError:   
        raise Exception("Unable to open the file 'impact_config.h', it should be in the bin directory containing impact_gui.py. Did you 'make install' impact?")

sys.path.append( config["PYTHON_INSTALL_PREFIX"] )

from MainWindow import *

app = QtGui.QApplication(sys.argv)

#main = MainWindow()
globals()['main']=MainWindow()

main.showMaximized()

sys.exit(app.exec_())


from MainWindow import *

app = QtGui.QApplication(sys.argv)

#main = MainWindow()
globals()['main']=MainWindow()

main.showMaximized()

sys.exit(app.exec_())

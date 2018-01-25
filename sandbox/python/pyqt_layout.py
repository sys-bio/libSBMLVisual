#!/usr/bin/python

# pyqt_layout.py
print('hi')
import sbnw
# load the model
model = sbnw.loadsbml('/home/sagrada/myhome/code/sbnw-trunk/testcases/twocompsys-ex.xml')
print('loading model')
#model = sbnw.loadsbml(r'C:\Users\user\Documents\layout\twocompsys-ex.xml')
print('getting layout')
layout = model.layout
print('got layout')
# access api
network = layout.network
canvas = layout.canvas
#print(dir(network))
print('pre-randomize')
network.randomize(canvas)
print('randomized')
network.autolayout()

wndwidth, wndheight = 640,380
pad = 30
if pad > wndwidth or pad > wndheight:
    raise RuntimeError('Padding exceeds dimension')

layout.fitwindow(0,0,wndwidth - pad,wndheight - pad)

import sys
import random
from PyQt4 import QtCore, QtGui
print('Imported PyQt4')
def QPoint(p):
    return QtCore.QPoint(p[0], p[1])

class LayoutDemo(QtGui.QMainWindow):
    def __init__(self):
        QtGui.QMainWindow.__init__(self)

        self.setGeometry(300, 300, wndwidth, wndheight)
        self.setWindowTitle('LayoutDemo')
        self.mainframe = LayoutFrame(self)

        self.setCentralWidget(self.mainframe)

        #self.statusbar = self.statusBar()
        #self.connect(self.mainframe, QtCore.SIGNAL("messageToStatusbar(QString)"), 
            #self.statusbar, QtCore.SLOT("showMessage(QString)"))
        
        self.center((0,300))

    def center(self, offset=(0,0)):
        screen = QtGui.QDesktopWidget().screenGeometry()
        size =  self.geometry()
        self.move((screen.width()-size.width())/2 + offset[0], 
        (screen.height()-size.height())/2 + offset[1])
        
class LayoutFrame(QtGui.QFrame):
    def __init__(self, parent):
        QtGui.QFrame.__init__(self, parent)

        self.timer = QtCore.QBasicTimer()

        self.setFocusPolicy(QtCore.Qt.StrongFocus)
        
    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        path = QtGui.QPainterPath()
        rect = self.contentsRect()
        
        # draw curves
        for reaction in network.rxns:
            for curve in reaction.curves:
                self.drawCurve(path, painter, curve)
        
        # draw nodes
        for node in network.nodes:
            x, y = node.centroid
            #offset = (500,500)
            #factor = 0.125
            offset = (0,0)
            factor = 1.
            x = (x+offset[0])*factor
            y = (y+offset[1])*factor
            print('x,y = {},{}'.format(x,y))
            x += rect.left()
            self.drawRect(painter, x-20, y-10, x+20, y+10)
            
            
    def drawCurve(self, path, painter, curve):
        print(curve)
        path.moveTo(QPoint(curve[0]))
        path.cubicTo(QPoint(curve[1]), QPoint(curve[2]), QPoint(curve[3]))
        painter.strokePath(path, painter.pen())
        

    def drawRect(self, painter, x0, y0, x1, y1):
        colorTable = [0x000000, 0xCC6666, 0x66CC66, 0x6666CC,
                    0xCCCC66, 0xCC66CC, 0x66CCCC, 0xDAAA00]

        width, height = x1-x0, y1-y0
        
        color = QtGui.QColor(colorTable[2])
        painter.fillRect(x0 + 1, y0 + 1, width - 2, 
        height - 2, color)
        
        painter.setPen(color.light())
        painter.drawLine(x0, y0 + height - 1, x0, y0)
        painter.drawLine(x0, y0, x0 + width - 1, y0)
        
        painter.setPen(color.dark())
        painter.drawLine(x0 + 1, y0 + height - 1,
            x0 + width - 1, y0 + height - 1)
        painter.drawLine(x0 + width - 1, 
        y0 + height - 1, x0 + width - 1, y0 + 1)

    
app = QtGui.QApplication(sys.argv)
layoutdemo = LayoutDemo()
layoutdemo.show()
sys.exit(app.exec_())
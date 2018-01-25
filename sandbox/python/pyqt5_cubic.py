#!/usr/bin/python

# pyqt_layout.py
import sbnw

wndwidth, wndheight = 740,480
pad = 30
if pad > wndwidth or pad > wndheight:
    raise RuntimeError('Padding exceeds dimension')

import sys
import random
import math
import PyQt5
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import pyqtSignal
import platform

def QPoint(p):
    return QtCore.QPoint(int(p[0]), int(p[1]))

def frange(x, y, jump):
  while x < y:
    yield x
    x += jump

class Paramcubic(QtWidgets.QMainWindow):
    def __init__(self):
        QtWidgets.QMainWindow.__init__(self)
        
        # Menu actions
        
        self.exitAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/application-exit-5.png'), '&Exit', self)
        if platform.system() == 'Darwin':
          self.exitAct.setShortcuts(QtGui.QKeySequence.Quit)
        else:
          self.exitAct.setShortcuts(QtGui.QKeySequence('Ctrl+Q'))
        self.exitAct.triggered.connect(self.close)


        self.filemenu = self.menuBar().addMenu('&File')
        self.filemenu.addAction(self.exitAct)
        
        self.setGeometry(300, 300, wndwidth, wndheight)
        self.setWindowTitle('Paramcubic')
        self.mainframe = ParamcubicFrame(self)
        
        self.setCentralWidget(self.mainframe)
        
        #self.statusbar = self.statusBar()
        #self.connect(self.mainframe, QtCore.SIGNAL("messageToStatusbar(QString)"), 
            #self.statusbar, QtCore.SLOT("showMessage(QString)"))
        
        self.center((0,100))
        
        self.model = None
        
    
    def center(self, offset=(0,0)):
        screen = QtWidgets.QDesktopWidget().screenGeometry()
        size =  self.geometry()
        self.move((screen.width()-size.width())/2 + offset[0], 
        (screen.height()-size.height())/2 + offset[1])
    

class ParamcubicFrame(QtWidgets.QFrame):
    def __init__(self, parent):
        QtWidgets.QFrame.__init__(self, parent)

        self.setFocusPolicy(QtCore.Qt.StrongFocus)

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        path = QtGui.QPainterPath()
        rect = self.contentsRect()
        #painter.drawText(0, QtCore.Qt.AlignCenter, 'name')
        
        painter.setRenderHints(QtGui.QPainter.Antialiasing)
        painter.setRenderHints(QtGui.QPainter.HighQualityAntialiasing)
        painter.setRenderHints(QtGui.QPainter.SmoothPixmapTransform)
        
        # pens
        thickpen = QtGui.QPen(QtGui.QBrush(QtGui.QColor(0,0,0,255)), 2., QtCore.Qt.SolidLine)
        thinpen = QtGui.QPen(QtGui.QBrush(QtGui.QColor(0,0,0,255)), 1., QtCore.Qt.SolidLine)
        path = QtGui.QPainterPath()

        p0 = sbnw.point(100,150)
        p1 = sbnw.point(200,250)
        p2 = sbnw.point(250,50)
        p3 = sbnw.point(400,100)

        # draw cubic
        path.moveTo(QPoint(p0))
        path.cubicTo(QPoint(p1), QPoint(p2), QPoint(p3))

        painter.strokePath(path, thickpen)

        brush = QtGui.QBrush(QtGui.QColor(75,0,150,100))
        painter.setBrush(brush)

        for t in frange(0,1,0.1):
          p = sbnw.paramcubic(p0,p1,p2,p3,t)
          print('p {},{}'.format(p.x,p.y))
          painter.drawEllipse(p.x-2, p.y-2, 4, 4)

        # draw line
        l0 = sbnw.point(90,160)
        l1 = sbnw.point(450,75)

        path.moveTo(QPoint(l0))
        path.lineTo(QPoint(l1))
        painter.strokePath(path, thinpen)

        intersec = sbnw.cubicintersec()
        pts = intersec.getpoints(p0,p1,p2,p3,l0,l1)

        for p in pts:
          print('p {}'.format(p))
          #print('p {},{}'.format(p.x,p.y))
          painter.drawEllipse(p.x-5, p.y-5, 10, 10)
    
    
app = QtWidgets.QApplication(sys.argv)
pcapp = Paramcubic()
pcapp.show()

sys.exit(app.exec_())
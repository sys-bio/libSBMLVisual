#!/usr/bin/python

import sys
sys.path.insert(1,'/home/poltergeist/home/etc/install/pyqt-5.3.2-py2/site-packages')
sys.path.insert(1,'/home/poltergeist/home/etc/install/sip-4.16.4-py2/site-packages')
sys.path.insert(1,'/home/poltergeist/home/devel/install/sbnw-trunk/site-packages-py2')

import os
os.chdir('/home/poltergeist/home/devel/install/sbnw-trunk/bin')

# pyqt_layout.py
import sbnw

wndwidth, wndheight = 740,480
pad = 30
if pad > wndwidth or pad > wndheight:
    raise RuntimeError('Padding exceeds dimension')

import random
import math
import PyQt5
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import pyqtSignal
import platform


defaultfile = None

if __name__ == '__main__':
    from optparse import OptionParser
    optparser = OptionParser()
    optparser.add_option('-o', '--open', action='store', type='string', dest='openfile')
    optparser.parse_args()
    defaultfile = optparser.values.openfile

def intervalContains(a, b, x):
    if a <= x <= b:
        return True
    else:
        return False

class NodeData:
    def __init__(self):
        self.isBeingDragged = False

def QPoint(p):
    return QtCore.QPoint(int(p[0]), int(p[1]))
        
        
def fixNode(node):
    node.width = 50
    node.height = 26
        
def fixNodes(network):
    for node in network.nodes:
        fixNode(node)

class Autolayout(QtWidgets.QMainWindow):
    def __init__(self):
        QtWidgets.QMainWindow.__init__(self)
        
        # Menu actions
        
        # File
        self.newAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/document-new-8.png'), '&New', self)
        self.newAct.setShortcuts(QtGui.QKeySequence.New)
        self.newAct.triggered.connect(self.new)
        
        self.openAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/document-open-7.png'), '&Open', self)
        self.openAct.setShortcuts(QtGui.QKeySequence.Open)
        self.openAct.triggered.connect(self.open)
        
        self.saveAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/document-save-5.png'), '&Save', self)
        self.saveAct.setShortcuts(QtGui.QKeySequence.Save)
        self.saveAct.triggered.connect(self.save)
        
        self.saveasAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/document-save-as-5.png'), 'Save &As...', self)
        self.saveasAct.setShortcuts(QtGui.QKeySequence.SaveAs)
        self.saveasAct.triggered.connect(self.saveAs)
        
        self.exitAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/application-exit-5.png'), '&Exit', self)
        if platform.system() == 'Darwin':
          self.exitAct.setShortcuts(QtGui.QKeySequence.Quit)
        else:
          self.exitAct.setShortcuts(QtGui.QKeySequence('Ctrl+Q'))
        self.exitAct.triggered.connect(self.close)
        
        # Edit
        self.selectAllAct = QtWidgets.QAction('Select &All', self)
        self.selectNoneAct = QtWidgets.QAction('Select &None', self)
        
        self.layoutAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/view-refresh-3.png'), '&Layout Network...', self)
        self.layoutAct.setShortcuts(QtGui.QKeySequence('Ctrl+L'))
        self.layoutAct.triggered.connect(self.autolayoutEvent)
        
        # View
        self.showCompsAct = QtWidgets.QAction('Show &Compartments', self)
        self.showCompsAct.setCheckable(True)
        
        # Window
        self.closeAct = QtWidgets.QAction('Close &Window', self)
        self.closeAct.setShortcuts(QtGui.QKeySequence.Close)
        
        # Tools
        self.toolActionGroup = QtWidgets.QActionGroup(self)
        self.selectToolAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/edit-select.png'), '&Select Tool', self.toolActionGroup)
        self.selectToolAct.setCheckable(True)
        self.createNodeToolAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/draw-line-2.png'), '&Create Node Tool', self.toolActionGroup)
        self.createNodeToolAct.setCheckable(True)
        self.linkToolAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/insert-link-2.png'), '&Create Node Tool', self.toolActionGroup)
        self.linkToolAct.setCheckable(True)
        self.eraseToolAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/draw-eraser-2.png'), '&Erase Tool', self.toolActionGroup)
        self.eraseToolAct.setCheckable(True)
        self.lockToolAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/lock.png'), '&Lock Tool', self.toolActionGroup)
        self.lockToolAct.setCheckable(True)
        
        # Help
        
        self.homepageAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/homepage.png'), '&Online Documentation', self)
        self.homepageAct.triggered.connect(self.openhomepageEvent)
        
        self.aboutAct = QtWidgets.QAction(QtGui.QIcon('../icons/32x32/help-about-2.png'), '&About', self)
        
        # Menus
        
        self.filemenu = self.menuBar().addMenu('&File')
        self.filemenu.addAction(self.newAct)
        self.filemenu.addAction(self.openAct)
        self.filemenu.addAction(self.saveAct)
        self.filemenu.addAction(self.saveasAct)
        self.filemenu.addSeparator()
        self.filemenu.addAction(self.exitAct)
        
        self.filemenu = self.menuBar().addMenu('&Edit')
        self.filemenu.addAction(self.selectAllAct)
        self.filemenu.addAction(self.selectNoneAct)
        self.filemenu.addAction(self.layoutAct)
        
        self.menuBar().addSeparator()
        
        self.helpmenu = self.menuBar().addMenu('&Help')
        self.helpmenu.addAction(self.homepageAct)
        self.helpmenu.addSeparator()
        self.helpmenu.addAction(self.aboutAct)
        
        # Toolbars
        
        # File
        self.filetoolbar = self.addToolBar('File')
        self.filetoolbar.addAction(self.newAct)
        self.filetoolbar.addAction(self.openAct)
        self.filetoolbar.addAction(self.saveAct)
        
        # Edit
        self.edittoolbar = self.addToolBar('Edit')
        self.edittoolbar.addAction(self.layoutAct)
        
        # Tools
        self.toolbar = self.addToolBar('Tools')
        self.toolbar.addAction(self.selectToolAct)
        self.toolbar.addAction(self.createNodeToolAct)
        self.toolbar.addAction(self.eraseToolAct)
        self.toolbar.addAction(self.lockToolAct)
        
        
        # Emit default triggers
        # Make select tool default
        self.selectToolAct.activate(QtWidgets.QAction.Trigger)
        
        self.setGeometry(300, 300, wndwidth, wndheight)
        self.setWindowTitle('SBW PyQt Autolayout')
        self.mainframe = LayoutFrame(self)
        
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
    
    def new(self, event):
        print('New')
    
    def open(self, event):
        filename = QtWidgets.QFileDialog.getOpenFileName(self)[0]
        if(len(filename) > 0):
            self.openfile(filename)
    
    def save(self, event):
        self.savefile(self.openfilepath)
    
    def saveAs(self, event):
        filename = QtWidgets.QFileDialog.getSaveFileName(self)[0]
        qfile = QtCore.QFile(filename)
        self.savefile(filename)
    
    def autolayout(self):
        print('autolayout')
        self.network.randomize(self.canvas)
        self.network.autolayout(k=50.)
        self.fitLayoutToWindow()
    
    def autolayoutEvent(self, event):
        self.autolayout()
        
    def fitLayoutToWindow(self):
        # Fit to frame
        framewidth = self.mainframe.frameRect().width()
        frameheight = self.mainframe.frameRect().height()
        print('frame width, height = {}'.format((framewidth, frameheight)))
        self.layout.fitwindow(-framewidth/2 + pad,-frameheight/2 + pad,framewidth/2 - pad,frameheight/2 - pad)
        
        # Update translation in viewer
        self.mainframe.translateBase = QPoint((framewidth/2., frameheight/2.))
        self.mainframe.translate = self.mainframe.translateBase
        
        #fixNodes(self.network)
        
        self.update()
    
    def openfile(self, filepath):
        self.openfilepath = filepath
        try:
            self.model = sbnw.loadsbml(filepath)
        except:
            msg = QtWidgets.QMessageBox(QtWidgets.QMessageBox.Critical, 'Open File Dialog', 
            'Failed to open file ' + filepath + ' (check spelling)', QtWidgets.QMessageBox.Ok, self, 
            QtCore.Qt.Dialog or QtCore.Qt.MSWindowsFixedSizeDialogHint or QtCore.Qt.Sheet)
            #msg.exec()
            return
        self.layout = self.model.layout
        self.network = self.layout.network
        self.canvas = self.layout.canvas
        for node in self.network.nodes:
            node.custom = NodeData()
        if not self.network.haslayout():
            self.autolayout()
        self.fitLayoutToWindow()
        self.mainframe.network = self.network
        self.mainframe.resetTransform()
    
    def savefile(self, filepath):
        if self.model is None:
            return
        #print(self.model.gattr)
        self.openfilepath = filepath
        self.model.save(filepath)
        print('saved file ' + filepath)
        
    def openhomepageEvent(self, event):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl('http://code.google.com/p/sbnw/'))
        
class LayoutFrame(QtWidgets.QFrame):
    def __init__(self, parent):
        QtWidgets.QFrame.__init__(self, parent)
        
        # Blank: no open file
        self.network = None
        self.scale = 1.
        self.translateBase = QPoint((wndwidth/2., wndheight/2.))
        self.translate = self.translateBase
        self.panning = False
        self.dragging = False
        
        self.timer = QtCore.QBasicTimer()

        self.setFocusPolicy(QtCore.Qt.StrongFocus)
        
    def resetTransform(self):
      self. scale = 1.
      self.translate = self.translateBase = QPoint((wndwidth/2., wndheight/2.))
        
    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        path = QtGui.QPainterPath()
        rect = self.contentsRect()
        #painter.drawText(0, QtCore.Qt.AlignCenter, 'name')
        
        painter.setRenderHints(QtGui.QPainter.Antialiasing)
        painter.setRenderHints(QtGui.QPainter.HighQualityAntialiasing)
        painter.setRenderHints(QtGui.QPainter.SmoothPixmapTransform)
        painter.translate(self.translate)
        painter.scale(self.scale, self.scale)
        
        if self.network is None:
            return
        
        #print('Centroids:')
        # draw curves
        rxnpen = QtGui.QPen(QtGui.QBrush(QtGui.QColor(0,0,0,255)), 2., QtCore.Qt.SolidLine)
        modpen = QtGui.QPen(QtGui.QBrush(QtGui.QColor(50,50,100,255)), 1., QtCore.Qt.SolidLine)
        for reaction in self.network.rxns:
            for curve in reaction.curves:
                self.drawCurve(path, painter, curve, rxnpen, modpen)
            # mark
            #print('reaction centroid {}, {}'.format(reaction.centroid.x, reaction.centroid.y))
            self.drawRxnCentroid(path, painter, reaction.centroid)
            #painter.strokePath(path, rxnpen)


        
        # draw nodes
        for node in self.network.nodes:
            x, y = self.getNodeScreenSpaceCentroid(node)
            offset = (0,0)
            factor = 1.
            x = (x+offset[0])*factor
            y = (y+offset[1])*factor
            hemiwidth = node.width/2
            hemiheight = node.height/2
            self.drawRect(node, painter, x-hemiwidth, y-hemiheight, x+hemiwidth, y+hemiheight)
        
        glowtextpainter = QtGui.QPainter(self)
        glowtextpainter.setPen(QtGui.QPen(QtGui.QBrush(QtGui.QColor(255,255,255,15)), 4., QtCore.Qt.SolidLine))
        glowtextpainter.translate(self.translate)
        glowtextpainter.scale(self.scale, self.scale)
        glowtextpainter.setFont(QtGui.QFont('sans', 9))
        
        for node in self.network.nodes:
            x0, y0 = self.getNodeScreenSpaceCentroid(node)
            for x in range(int(x0)-2,int(x0)+2):
                for y in range(int(y0)-2,int(y0)+2):
                    glowtextpainter.drawText(QtCore.QRectF(x-node.width/2, y-node.height/2, node.width, node.height), 
                        QtCore.Qt.AlignCenter, node.name)
        
        textpainter = QtGui.QPainter(self)
        textpainter.translate(self.translate)
        textpainter.scale(self.scale, self.scale)
        textpainter.setFont(QtGui.QFont('sans', 9))
        
        for node in self.network.nodes:
            x, y = self.getNodeScreenSpaceCentroid(node)
            textpainter.drawText(QtCore.QRectF(x-node.width/2, y-node.height/2, node.width, node.height), 
                QtCore.Qt.AlignCenter, node.name)
            
    def drawRxnCentroid(self, path, painter, point):
        #brush = QtGui.QBrush(QtGui.QColor(255,0,0))
        #painter.setBrush(brush)
        #painter.drawEllipse(point.x, point.y, 10, 10)

        brush = QtGui.QBrush(QtGui.QColor(75,0,150,100))
        painter.setBrush(brush)
        painter.drawEllipse(point.x-10, point.y-10, 20, 20)
            
    def drawCurve(self, pathx, painter, curve, normalpen, modpen):
        path = QtGui.QPainterPath()
        print('  curve: begin {}, end {}'.format(curve[0],curve[3]))
        path.moveTo(QPoint(curve[0]))
        path.cubicTo(QPoint(curve[1]), QPoint(curve[2]), QPoint(curve[3]))
        painter.strokePath(path, normalpen)
    
    def makeQColorAlpha(self, rgb, alpha):
        return QtGui.QColor((rgb & 0xFF0000) >> 16, (rgb & 0x00FF00) >> 8, rgb & 0x0000FF, alpha)
    
    def drawRect(self, node, painter2, x0, y0, x1, y1):
        painter = QtGui.QPainter(self)
        painter.setRenderHints(QtGui.QPainter.Antialiasing)
        painter.setRenderHints(QtGui.QPainter.HighQualityAntialiasing)
        painter.setRenderHints(QtGui.QPainter.SmoothPixmapTransform)
        painter.translate(self.translate)
        painter.scale(self.scale, self.scale)
        
        width, height = x1-x0, y1-y0
        cornerrad = 4.
        
        if not node.islocked():
            #linearbright = 0x751a9b
            linearbright = 0xb343e1
            #radialbright = 0xa24dc6
            radialbright = 0xce59ff
            dark = 0x2e0041
            horizonlight = 0xae0df1
        else:
            linearbright = 0x888888
            radialbright = 0x888888
            dark = 0x111111
            horizonlight = 0xd5d5d5
        
        # shadow
        shadowoffset = QPoint((3.,3.))
        painter.setPen(QtGui.QPen(QtCore.Qt.NoPen))
        painter.setBrush(QtGui.QColor(0,0,0,100))
        painter.drawRoundedRect(QtCore.QRectF(x0 + shadowoffset.x(), y0 + shadowoffset.y(), width, 
            height), cornerrad, cornerrad)
        
        # main shape
        
        gradient = QtGui.QLinearGradient(QPoint((x0+width/2,y0)), QPoint((x0+width/2,y1)))
        gradient.setColorAt(0, QtGui.QColor(linearbright))
        gradient.setColorAt(1, QtGui.QColor(dark))
        
        brush = QtGui.QBrush(gradient)
        painter.setBrush(brush)
        
        painter.drawRoundedRect(QtCore.QRectF(x0, y0, width, 
            height), cornerrad, cornerrad)
        
        # radial pass
        
        # outline
        painter.setBrush(QtCore.Qt.NoBrush)
        outlinepen = QtGui.QPen(QtGui.QBrush(QtGui.QColor(00,0,00,255)), 1., QtCore.Qt.SolidLine)
        painter.setPen(outlinepen)
        
        radialgradient = QtGui.QRadialGradient(QPoint((x0+width/3,y0+height/2)), 30.*width/50, QPoint((x0+width/4,y0+height/4)))
        radialgradient.setColorAt(0, self.makeQColorAlpha(radialbright, 255))
        radialgradient.setColorAt(1, self.makeQColorAlpha(dark, 25))
        radialbrush = QtGui.QBrush(radialgradient)
        painter.setBrush(radialbrush)
        painter.setCompositionMode(QtGui.QPainter.CompositionMode_Screen)
        
        painter.drawRoundedRect(QtCore.QRectF(x0, y0, width, 
            height), cornerrad, cornerrad)
        
        # horizon pass
        
        horizongradient = QtGui.QLinearGradient(QPoint((x0+width/2,y0-100)), QPoint((x0+width/2,y0+height/2)))
        horizongradient.setSpread(QtGui.QGradient.RepeatSpread)
        horizongradient.setColorAt(0, QtGui.QColor(horizonlight))
        horizongradient.setColorAt(1, QtGui.QColor(0xffffff))
        
        brush = QtGui.QBrush(horizongradient)
        painter.setBrush(brush)
        painter.setCompositionMode(QtGui.QPainter.CompositionMode_Multiply)
        
        painter.drawRoundedRect(QtCore.QRectF(x0, y0, width, 
            height), cornerrad, cornerrad)
        
        if node.custom.isBeingDragged:
            painter.setBrush(QtGui.QColor(0,0,0,100))
            painter.drawRoundedRect(QtCore.QRectF(x0, y0, width, 
                height), cornerrad, cornerrad)
    
    def getNodeScreenSpaceCentroid(self, node):
        return node.centroid
    
    def setNodeScreenSpaceCentroid(self, node, coords):
        node.centroid = coords[0], coords[1]
    
    # x & y are screen space
    def addNode(self, x, y):
        newnode = self.network.newnode('NewNode')
        newnode.custom = NodeData()
        fixNode(newnode)
        newnode.centroid = x,y
    
    def removeNode(self, node):
        self.network.removenode(node)
    
    # Mouse wheel
    def wheelEvent(self, event):
        #print('Wheel event ' + repr(dir(event)))
        #print('Pixel delta ' + repr(event.pixelDelta().y()))
        #print('Angle delta ' + repr(event.angleDelta().y()))
        self.scale *= math.exp(event.angleDelta().y() / 600.)
        if self.scale < 0.1: self.scale = 0.1
        if self.scale > 4.: self.scale = 4.
        #print('scale: {}'.format(self.scale))
        #self.scale = 
        #self.sigScaleChg.emit()
        self.setScale(self.scale)
    
    # Mouse press
    def mousePressEvent(self, event):
        if event.button() == 1:
            mouse = QPoint(((event.x() - self.translate.x())/self.scale, 
                (event.y() - self.translate.y())/self.scale))
            
            if self.parent().createNodeToolAct.isChecked():
                self.addNode(mouse.x(), mouse.y())
            else:
                for node in self.network.nodes:
                    x, y = self.getNodeScreenSpaceCentroid(node)
                    hemiwidth = node.width/2
                    hemiheight = node.height/2
                    
                    if intervalContains(x - hemiwidth, x + hemiwidth, mouse.x()) and intervalContains(y - hemiheight, y + hemiheight, mouse.y()):
                        if self.parent().selectToolAct.isChecked():
                            node.custom.isBeingDragged = True
                            node.custom.centroidSource = QPoint(self.getNodeScreenSpaceCentroid(node))
                            self.dragging = True
                            self.dragSource = mouse
                        elif self.parent().lockToolAct.isChecked():
                            if not node.islocked():
                                node.lock()
                            else:
                                node.unlock()
                        elif self.parent().eraseToolAct.isChecked():
                            self.removeNode(node)
        elif event.button() == 4:
            mouse = QPoint((event.x(), event.y()))
            self.panning = True
            self.panstart = mouse
        self.update()
    
    # Mouse release
    def mouseReleaseEvent(self, event):
        if event.button() == 1:
            self.dragging = False
            for node in self.network.nodes:
                if node.custom.isBeingDragged:
                    node.custom.isBeingDragged = False
            self.update()
        if(event.button() == 4):
            self.panning = False
            self.applyTranslation()
            
    
    def applyTranslation(self):
        self.translateBase = self.translate
    
    def mouseMoveEvent(self, event):
        if self.dragging:
            mouse = QPoint(((event.x() - self.translate.x())/self.scale, 
                (event.y() - self.translate.y())/self.scale))
            delta = mouse - self.dragSource
            for node in self.network.nodes:
                if node.custom.isBeingDragged:
                    newcentroid = node.custom.centroidSource + delta
                    self.setNodeScreenSpaceCentroid(node, (newcentroid.x(), newcentroid.y()))
                    for reaction in self.network.rxns:
                        if reaction.has(node):
                            reaction.recenter()
            #self.network.recenterjunct()
            self.update()
        elif self.panning:
            mouse = QPoint((event.x(), event.y()))
            self.changeTranslate(mouse - self.panstart)
    
    def setScale(self, s):
        self.update()
    
    def changeTranslate(self, delta):
        self.translate = self.translateBase + delta
        self.update()

    
app = QtWidgets.QApplication(sys.argv)
layoutapp = Autolayout()
layoutapp.show()
if(defaultfile):
    layoutapp.openfile(defaultfile)
#layoutapp.autolayout()
#layoutapp.fitLayoutToWindow()

sys.exit(app.exec_())
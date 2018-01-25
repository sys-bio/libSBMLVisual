#!/usr/bin/python

import os
localpath = os.path.dirname(os.path.realpath(__file__))
newpath = os.path.abspath(os.path.join(localpath, '..', '..', 'bin'))

import sbnw

import sys
if sys.version_info[0] < 3:
  import __builtin__
  if hasattr(__builtin__, 'p_pyfab_spyder_env'):
    inspyder = True
  else:
    inspyder = False
else:
  import builtins
  if hasattr(builtins, 'p_pyfab_spyder_env'):
    inspyder = True
  else:
    inspyder = False

if inspyder:
  # disable keyboard shortcuts
  handsfree = True
else:
  handsfree = False

if not inspyder:
  enable_matplotlib2tikz = True
else:
  enable_matplotlib2tikz = False

try:
  import matplotlib2tikz
except ImportError:
  enable_matplotlib2tikz = False

enable_matplotlib2tikz = False

def is_pyqt4():
  return inspyder

def is_pyqt5():
  return not is_pyqt4()

def get_exec(x):
  if sys.version_info[0] < 3:
    return getattr(x, 'exec_')
  else:
    return getattr(x, 'exec')

def dict_iteritems(d):
  if sys.version_info[0] < 3:
    return d.iteritems()
  else:
    return iter(d.items())


wndwidth, wndheight = 740,480
pad = 30
if pad > wndwidth or pad > wndheight:
    raise RuntimeError('Padding exceeds dimension')

import random
import math
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
        self.customColor = None
        self.customWeight = None
        self.beacon = False

def QPoint(p):
    return QtCore.QPoint(int(p[0]), int(p[1]))
def QPointF(p):
    return QtCore.QPointF(float(p[0]), float(p[1]))
def QPoint2Tuple(p):
    return (p.x(), p.y())

def fixNode(node):
    node.width = 50
    node.height = 26

def fixNodes(network):
    for node in network.nodes:
        fixNode(node)

if is_pyqt5():
  import PyQt5
  from PyQt5 import QtCore, QtGui, QtWidgets
  from PyQt5.QtCore import pyqtSignal

  MainWindowBaseClass = QtWidgets.QMainWindow
  from PyQt5.QtGui import (QPainter,
    QPen, QColor, QLinearGradient,
    QBrush, QRadialGradient, QGradient, QPalette,
    QClipboard, QPixmap, QIcon, QImage)
  from PyQt5.QtWidgets import (QWidget, QDialog, QPushButton, QGroupBox,
                               QHBoxLayout, QVBoxLayout, QFrame,
                               QSizePolicy, QListWidget, QListWidgetItem,
                               QSlider, QMessageBox, QFileDialog, QDesktopWidget,
                               QActionGroup, QAction, QRadioButton, QColorDialog,
                               QLabel, QScrollArea, QGridLayout, QStackedWidget,
                               QToolButton, QWidgetAction, QMenu, QDoubleSpinBox,
                               QCheckBox, QSpinBox)
  from PyQt5.QtCore import (Qt, QRectF, QSize)
  from PyQt5.QtSvg import QSvgGenerator

  pyfab_getSaveFileName = QFileDialog.getSaveFileName
elif is_pyqt4():
  if inspyder:
    import spyderlib
    from spyderlib.qt import QtCore, QtGui
    QWidget = QtGui.QWidget
    from spyderlib.utils.qthelpers import qapplication
    from spyderlib.plugins import SpyderPluginWidget, PluginConfigPage

    def pyfab_getSaveFileName(parent, filter=None):
      if filter is not None:
        return spyderlib.qt.compat.getsavefilename(parent, filters=filter)
      else:
        return spyderlib.qt.compat.getsavefilename(parent)

  MainWindowBaseClass = SpyderPluginWidget
  QAction = QtGui.QAction
  QActionGroup = QtGui.QActionGroup
  QDesktopWidget = QtGui.QDesktopWidget
  QFileDialog = QtGui.QFileDialog
  QMessageBox = QtGui.QMessageBox
  QSlider = QtGui.QSlider
  QVBoxLayout = QtGui.QVBoxLayout
  QHBoxLayout = QtGui.QHBoxLayout
  QMenuBar = QtGui.QMenuBar
  QMenu = QtGui.QMenu
  QToolBar = QtGui.QToolBar
  QFrame = QtGui.QFrame
  QSplitter = QtGui.QSplitter
  QSizePolicy = QtGui.QSizePolicy
  from spyderlib.qt.QtGui import (QPainter,
    QPen, QColor, QLinearGradient, QPixmap,
    QBrush, QRadialGradient, QGradient, QPalette,
    QIcon, QImage)
  from spyderlib.qt.QtCore import (Qt, QRectF, QSize)
  from spyderlib.qt.QtSvg import QSvgGenerator

  from spyderlib.qt.QtGui import (QDialog, QGroupBox, QPushButton, QListWidget, QListWidgetItem,
                               QSlider, QMessageBox, QFileDialog, QDesktopWidget,
                               QActionGroup, QAction, QRadioButton, QColorDialog, QLabel,
                               QScrollArea, QGridLayout, QClipboard, QStackedWidget, QToolButton,
                               QWidgetAction, QDoubleSpinBox, QCheckBox, QSpinBox)

class PyfabRenderer:
  pass

if not inspyder:
  from pyfab_cfg import PyfabConfigStandalone as ConfigCls, get_default_options, make_color_rgba
else:
  from pyfab_cfg import get_default_options, make_color_rgba

import pyfab_qt_render
#if enable_matplotlib2tikz:
  #import pyfab_matplotlib_render

def tuple2QColor(t):
    return QColor(t[0]*255, t[1]*255, t[2]*255, t[3]*255)

def QColor2tuple(q):
    return (q.redF(), q.greenF(), q.blueF(), q.alphaF())

class Autolayout(MainWindowBaseClass):
    def __init__(self, parent=None):
        if not inspyder:
          MainWindowBaseClass.__init__(self)
        else:
          MainWindowBaseClass.__init__(self, parent)

        if not hasattr(self, 'configCls'):
          self.configCls = ConfigCls

        # Menu actions

        # File
        self.newAct = QAction(QtGui.QIcon(self.getIconPath('32x32/document-new-8.png')), '&New', self)
        if not handsfree:
          self.newAct.setShortcuts(QtGui.QKeySequence.New)
        self.newAct.triggered.connect(self.new)

        self.openAct = QAction(QtGui.QIcon(self.getIconPath('32x32/document-open-7.png')), '&Open', self)
        if not handsfree:
          self.openAct.setShortcuts(QtGui.QKeySequence.Open)
        self.openAct.triggered.connect(self.open)

        self.saveAct = QAction(QtGui.QIcon(self.getIconPath('32x32/document-save-5.png')), '&Save', self)
        if not handsfree:
          self.saveAct.setShortcuts(QtGui.QKeySequence.Save)
        self.saveAct.triggered.connect(self.save)

        self.saveasAct = QAction(QtGui.QIcon(self.getIconPath('32x32/document-save-as-5.png')), 'Save &As...', self)
        if not handsfree:
          self.saveasAct.setShortcuts(QtGui.QKeySequence.SaveAs)
        self.saveasAct.triggered.connect(self.saveAs)

        self.savePNGAct = QAction('Save PNG...', self)
        self.savePNGAct.triggered.connect(self.savePNG)

        self.saveSVGAct = QAction('Save SVG...', self)
        self.saveSVGAct.triggered.connect(self.saveSVG)

        self.saveTikZAct = QAction('Save TikZ...', self)
        self.saveTikZAct.triggered.connect(self.saveTikZ)

        self.saveTikZIntAct = QAction('Save TikZ (Internal)...', self)
        self.saveTikZIntAct.triggered.connect(self.saveTikZInt)

        self.prefsAct = QAction(QtGui.QIcon(self.getIconPath('32x32/system-settings.png')), '&Preferences', self)
        self.prefsAct.triggered.connect(self.openPrefDialog)

        self.getobjAct = QAction('Get obj', self)
        self.getobjAct.triggered.connect(self.getobj)

        self.exitAct = QAction(QtGui.QIcon(self.getIconPath('32x32/application-exit-5.png')), '&Exit', self)
        if not handsfree:
          if platform.system() == 'Darwin':
            self.exitAct.setShortcuts(QtGui.QKeySequence.Quit)
          else:
            self.exitAct.setShortcuts(QtGui.QKeySequence('Ctrl+Q'))
        self.exitAct.triggered.connect(self.close)

        # Edit
        self.selectAllAct = QAction('Select &All', self)
        self.selectNoneAct = QAction('Select &None', self)

        self.copySBMLCbAct = QAction('Copy SBML to Clipboard', self)
        self.copySBMLCbAct.triggered.connect(self.copySBMLCb)

        self.layoutAct = QAction(QtGui.QIcon(self.getIconPath('32x32/view-refresh-3.png')), '&Layout Network...', self)
        if not handsfree:
          self.layoutAct.setShortcuts(QtGui.QKeySequence('Ctrl+L'))
        self.layoutAct.triggered.connect(self.autolayoutEvent)

        # View
        self.showCompsAct = QAction('Show &Compartments', self)
        self.showCompsAct.setCheckable(True)

        # Window
        self.closeAct = QAction('Close &Window', self)
        if not handsfree:
          self.closeAct.setShortcuts(QtGui.QKeySequence.Close)

        # Tools
        self.toolActionGroup = QActionGroup(self)
        self.selectToolAct = QAction(QtGui.QIcon(self.getIconPath('32x32/select.png')), '&Select Tool', self.toolActionGroup)
        self.selectToolAct.setCheckable(True)
        self.createNodeToolAct = QAction(QtGui.QIcon(self.getIconPath('32x32/draw-freehand.png')), '&Create Node Tool', self.toolActionGroup)
        self.createNodeToolAct.setCheckable(True)
        self.linkToolAct = QAction(QtGui.QIcon(self.getIconPath('32x32/insert-link-2.png')), '&Create Node Tool', self.toolActionGroup)
        self.linkToolAct.setCheckable(True)
        self.eraseToolAct = QAction(QtGui.QIcon(self.getIconPath('32x32/draw-eraser-2.png')), '&Erase Tool', self.toolActionGroup)
        self.eraseToolAct.setCheckable(True)
        self.lockToolAct = QAction(QtGui.QIcon(self.getIconPath('32x32/lock.png')), '&Lock Tool', self.toolActionGroup)
        self.lockToolAct.setCheckable(True)
        self.aliasToolAct = QAction(QtGui.QIcon(self.getIconPath('32x32/gnome-iagno.png')), '&Alias Tool', self.toolActionGroup)
        self.aliasToolAct.setCheckable(True)

        # Help

        self.homepageAct = QAction(QtGui.QIcon(self.getIconPath('32x32/homepage.png')), '&Homepage', self)
        self.homepageAct.triggered.connect(self.openhomepageEvent)

        self.videosAct = QAction(QtGui.QIcon(self.getIconPath('32x32/applications-multimedia-3.png')), '&Video Tutorials', self)
        self.videosAct.triggered.connect(self.openvideosEvent)

        self.aboutAct = QAction(QtGui.QIcon(self.getIconPath('32x32/help-about-2.png')), '&About/License', self)
        self.aboutAct.triggered.connect(self.openAboutBoxEvent)

        # Emit default triggers
        # Make select tool default
        self.selectToolAct.activate(QAction.Trigger)

        self.setGeometry(300, 300, wndwidth, wndheight)
        self.setWindowTitle('Pyfab SBNW')
        self.mainframe = LayoutFrame(self)

        if not inspyder:
          self.setCentralWidget(self.mainframe)

          self.center((0,100))
        else:
          layout = QVBoxLayout()
          layout.setContentsMargins(0, 0, 0, 0)
          layout.addWidget(self.mainframe)
          self.setLayout(layout)

        # Menus
        if not inspyder:
          menu = self.menuBar()
        else:
          menubar = QMenuBar()
          layout.setMenuBar(menubar)
          menu = layout.menuBar()
          menu.setNativeMenuBar(True)

        self.filemenu = menu.addMenu('&File')
        self.filemenu.addAction(self.openAct)
        self.filemenu.addAction(self.saveAct)
        self.filemenu.addAction(self.saveasAct)
        self.filemenu.addSeparator()
        self.filemenu.addAction(self.savePNGAct)
        self.filemenu.addAction(self.saveSVGAct)
        if not inspyder and enable_matplotlib2tikz:
          self.filemenu.addAction(self.saveTikZAct)
        self.filemenu.addAction(self.saveTikZIntAct)

        # don't include exit option when in Spyder
        if not inspyder:
          self.filemenu.addSeparator()
          self.filemenu.addAction(self.exitAct)

        self.editmenu = menu.addMenu('&Edit')
        self.editmenu.addAction(self.copySBMLCbAct)
        self.editmenu.addAction(self.layoutAct)

        self.toolsmenu = menu.addMenu('&Tools')
        self.toolsmenu.addAction(self.prefsAct)

        menu.addSeparator()

        self.helpmenu = menu.addMenu('&Help')
        self.helpmenu.addAction(self.homepageAct)
        self.helpmenu.addAction(self.videosAct)
        self.helpmenu.addSeparator()
        self.helpmenu.addAction(self.aboutAct)


        # Toolbars

        if inspyder:
          toolbarlayout = QHBoxLayout()
          toolbarlayout.setContentsMargins(0, 0, 0, 0)
          layout.addLayout(toolbarlayout)

        def addToolBar(name):
          if not inspyder:
            return self.addToolBar(name)
          else:
            qtbar = QToolBar(name)
            toolbarlayout.addWidget(qtbar)
            return qtbar

        def addToolBarSeparator():
          if inspyder:
            sep = QFrame()
            sep.setFrameShape(QFrame.HLine)
            sep.setFrameShadow(QFrame.Sunken)
            toolbarlayout.addWidget(sep)
            sep.show()

        # File
        self.filetoolbar = addToolBar('File')
        self.filetoolbar.addAction(self.openAct)
        self.filetoolbar.addAction(self.saveAct)
        self.filetoolbar.addAction(self.saveasAct)
        self.filetoolbar.addAction(self.prefsAct)

        # Edit
        self.edittoolbar = addToolBar('Edit')
        self.edittoolbar.addAction(self.layoutAct)

        # Tools
        self.toolbar = addToolBar('Tools')
        self.toolbar.addAction(self.selectToolAct)
        self.toolbar.addAction(self.createNodeToolAct)
        self.toolbar.addAction(self.eraseToolAct)
        self.toolbar.addAction(self.lockToolAct)
        self.toolbar.addAction(self.aliasToolAct)

        self.sliderwidget = QSlider(QtCore.Qt.Horizontal)
        self.sliderwidget.setMaximum(100)
        self.sliderwidget.setMinimum(1)
        self.sliderwidget.setValue(self.openconfig().state.stiffness)
        self.sliderwidget.sliderReleased.connect(self.stiffness_changed_via_slider)

        self.slider_label = QLabel('Stiffness:')
        self.toolbar.addWidget(self.slider_label)

        self.toolbar.addWidget(self.sliderwidget)

        self.gravslider = QSlider(QtCore.Qt.Horizontal)
        self.gravslider.setMaximum(100)
        self.gravslider.setMinimum(1)
        self.gravslider.setValue(self.openconfig().state.gravity)
        self.gravslider.sliderReleased.connect(self.gravity_changed_via_slider)

        self.grav_label = QLabel('Gravity:')
        self.toolbar.addWidget(self.grav_label)

        self.toolbar.addWidget(self.gravslider)

        self.model = sbnw.sbmlmodel(3,1,600,600)
        self.layout = self.model.layout
        self.network = self.layout.network
        self.canvas = self.layout.canvas
        self.mainframe.network = self.network

    def stiffness_changed_via_slider(self):
        with self.openconfig() as config:
            config.state.stiffness = float(self.sliderwidget.value())

    def gravity_changed_via_slider(self):
        with self.openconfig() as config:
            config.state.gravity = float(self.gravslider.value())

    # marker for Spyder plugin detection
    def pyfabMarker():
        return 'pyfab'

    # proxy - this function is overridden by the Spyder plugin to rebind the asset path
    def getIconPath(self, p):
      result = str(os.path.join(localpath, '..', '..', 'icons', p))
      return result

    def center(self, offset=(0,0)):
        screen = QDesktopWidget().screenGeometry()
        size =  self.geometry()
        self.move((screen.width()-size.width())/2 + offset[0],
        (screen.height()-size.height())/2 + offset[1])

    def new(self, event):
        print('New')

    def launchOpenDialog(self):
        if is_pyqt5():
          # Qt5
          return QFileDialog.getOpenFileName(self, filter='SBML files (*.xml *.sbml);;All files (*)')[0]
        else:
          # Qt4
          return QFileDialog.getOpenFileName(self, filter='SBML files (*.xml *.sbml);;All files (*)')

    def open(self, event):
        filename = self.launchOpenDialog()
        if(len(filename) > 0):
            self.openfile(str(filename)) # Qt4: wrong type (not str)

    def save(self, event):
      if self.openfilepath:
        self.savefile(self.openfilepath)
      else:
        self.saveAs(event)

    def saveAs(self, event):
        filename = pyfab_getSaveFileName(self)[0]
        if filename:
          qfile = QtCore.QFile(filename)
          self.savefile(filename)

    def saveSVG(self, event):
        filt = 'Scalable Vector Graphics (*.svg);; All Files(*.*)'
        if not inspyder:
          filename = pyfab_getSaveFileName(self, filter=filt)[0]
        else:
          filename = pyfab_getSaveFileName(self, filter=filt)[0]
        if filename:
          self.mainframe.renderSVG(str(filename))

    def savePNG(self, event):
        filt = 'Portable Network Graphics (*.png);; All Files(*.*)'
        if not inspyder:
          filename = pyfab_getSaveFileName(self, filter=filt)[0]
        else:
          filename = pyfab_getSaveFileName(self, filter=filt)[0]
        if filename:
          self.mainframe.renderPNG(str(filename))

    def saveTikZ(self, event):
        filt = 'TikZ (*.tikz);; All Files(*.*)'
        filename = pyfab_getSaveFileName(self, filter=filt)[0]
        if filename:
          self.mainframe.renderTikZ(filename)

    def saveTikZInt(self, event):
      filt = 'TikZ (*.tikz);; All Files(*.*)'
      filename = pyfab_getSaveFileName(self, filter=filt)[0]
      if filename:
        self.savetikz(filename)

    def openPrefDialog(self, event):
      dlg = PrefDialog(self)
      dlg.setWindowTitle('Preferences')
      dlg.resize(680,450)
      dlg.show()

    def savetikz(self, filepath):
        if self.model is None:
          return
        self.model.savetikz(filepath)

    def getobj(self):
        print('getobj')

    def autolayout(self):
        #self.network.randomize(self.canvas)
        self.network.autolayout(k=self.sliderwidget.value(), grav=self.gravslider.value())
        self.fitLayoutToWindow()

    def copySBMLCb(self, event):
        print('copy SBML to clipboard')
        sbml = self.getsbml()
        cb = self.AppCls.clipboard()
        cb.setText(sbml)

    def autolayoutEvent(self, event):
        self.autolayout()

    def fitLayoutToWindow(self):
        # Fit to frame
        framewidth = self.mainframe.frameRect().width()
        frameheight = self.mainframe.frameRect().height()
        #self.layout.firstquad(0,0)
        self.mainframe.tf = self.layout.tf_fitwindow(pad,pad,framewidth - pad,frameheight - pad)
        self.mainframe.scale = self.mainframe.tf.scale.x
        self.mainframe.translateBase = QPoint(self.mainframe.tf.disp)
        self.mainframe.setScale(self.mainframe.scale)

        # Update translation in viewer
        self.mainframe.translate = self.mainframe.translateBase

        self.update()

    def openfile(self, filepath):
        self.openfilepath = filepath
        self.readsbml(str(filepath), str(filepath))

    def notify_config_changed(self):
        self.sliderwidget.setValue(self.openconfig().state.stiffness)
        self.gravslider.setValue(self.openconfig().state.gravity)
        self.update()

    def opensbml(self, sbml):
        self.openfilepath = None
        self.readsbml(str(sbml))

    def readsbml(self, sbml, filepath='<buffer>'):
        try:
            self.model = sbnw.loadsbml(sbml)
        except:
            title = 'Failed to Open File'
            errorstr = 'Failed to read SBML '

            if not filepath=='<buffer>':
                errorstr = 'Failed to read SBML file ' + filepath

            msg = QMessageBox(QMessageBox.Critical, title,
                errorstr, QMessageBox.Ok, self,
                QtCore.Qt.Dialog or QtCore.Qt.MSWindowsFixedSizeDialogHint or QtCore.Qt.Sheet)
            get_exec(msg)()
            return
        self.layout = self.model.layout
        self.network = self.layout.network
        self.canvas = self.layout.canvas
        for node in self.network.nodes:
            node.custom = NodeData()
        if not self.network.haslayout():
            self.autolayout()
        self.mainframe.resetTransform()
        self.fitLayoutToWindow()
        self.mainframe.network = self.network

    def savefile(self, filepath):
        if self.model is None:
            return
        self.openfilepath = filepath
        config = self.openconfig()
        if config.state.override_sbml_ns:
          self.model.level = config.state.sbml_level
          self.model.version = config.state.sbml_version
        self.model.save(filepath)

    def getsbml(self):
        if self.model is None:
          raise RuntimeError('No model')
        return self.model.getsbml()

    def openconfig(self):
      if not hasattr(self, 'config') or self.config is None:
        self.config = self.configCls()
        self.config.install_listener(self.notify_config_changed)
      config = self.configCls(self.config)
      return config

    def pushconfig(self, config):
      if not hasattr(self, 'configs') or self.configs is None:
        self.configs = []
      self.configs.append(self.config)
      self.config = config
      self.config.updateArrowStyles()
      assert( len(self.configs) > 0 )

    def popconfig(self):
      self.config = self.configs.pop()
      self.config.updateArrowStyles()

    def confirmconfig(self, config):
      '''
      Makes current config permanent
      '''
      config.finalize()
      self.config = self.configs.pop()
      self.config.updateArrowStyles()

    def openhomepageEvent(self, event):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl('https://github.com/0u812/sbnw'))

    def openvideosEvent(self, event):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl('http://0u812.github.io/sbnw/tutorials/'))

    def openAboutBoxEvent(self, event):
      box = QMessageBox.about(self, 'SBNW: SBML Network Viewer',
                              '<center><h1>SBNW</h1>\n\n<h2>Network Viewer for SBML</h2>\n<a href=\"https://github.com/0u812/sbnw\">Visit Homepage</a>\n</center>\n<br/>' +
'''<strong>Copyright (c) 2015, J. Kyle Medley, Herbert M. Sauro</strong> <br/>
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

<ul>
    <li>Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.</li>
    <li>Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.</li>
    <li>Neither the name of SBNW nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.</li>
</ul>

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
<br/><br/>
Uses icons from the <a href="http://sourceforge.net/projects/openiconlibrary/">Open Icon Library</a>. See the Open Icon Library for copyright/licenses for respective icon files.
''')

if not inspyder:
  FrameBaseClass = QtWidgets.QFrame
else:
  FrameBaseClass = QtGui.QFrame

class LayoutFrame(FrameBaseClass):
    def __init__(self, parent):
        FrameBaseClass.__init__(self, parent)

        # Blank: no open file
        self.network = None
        self.scale = 1.
        self.postScale = 1.

        self.translateBase = QPoint((0., 0.))
        self.translate = self.translateBase

        self.postTranslateBase = QPoint((0., 0.))
        self.postTranslate = self.postTranslateBase

        self.qtf = QtGui.QTransform()

        self.panning = False
        self.dragging = False
        self.connecting = False
        self.connecting_src_node = False

        self.connecting_node = None
        self.connecting_rxn = None

        self.droptarget = None

        self.timer = QtCore.QBasicTimer()

        self.setFocusPolicy(QtCore.Qt.StrongFocus)

        self.qtrender = pyfab_qt_render.PyQtRenderer()
        if enable_matplotlib2tikz:
          self.pypltrender = pyfab_matplotlib_render.PyPlotRenderer()

        self.plantNode = False
        self.plantRxn = False
        self.plantComp = False

    def resetTransform(self):
      self.scale = 1.
      self.translate = self.translateBase = QPoint((wndwidth/2., wndheight/2.))

    def paintEvent(self, event):
        self.render()

    def render(self, painter=None, horizonEnabled=True):
        def screentx(x, y):
          return QPoint2Tuple(self.qtf.map(QPoint((x,y))))

        if enable_matplotlib2tikz:
          self.pypltrender.begin(width=self.frameRect().width(), height=self.frameRect().height())

        if painter is None:
          painter = QtGui.QPainter(self)
        path = QtGui.QPainterPath()
        rect = self.contentsRect()

        painter.setRenderHints(QtGui.QPainter.Antialiasing)
        painter.setRenderHints(QtGui.QPainter.HighQualityAntialiasing)
        painter.setRenderHints(QtGui.QPainter.SmoothPixmapTransform)

        framewidth = self.frameRect().width()
        frameheight = self.frameRect().height()

        self.qtf.reset()
        self.qtf.translate(self.postTranslate.x(), self.postTranslate.y())
        self.qtf.translate(framewidth/2,frameheight/2)
        self.qtf.scale(self.postScale, self.postScale)
        self.qtf.translate(-framewidth/2,-frameheight/2)

        self.qtf.translate(self.translate.x(), self.translate.y())
        self.qtf.scale(self.scale, self.scale)

        config = self.parent().openconfig()

        # renderer needs backref to self
        self.qtrender.frame = self

        painter.setTransform(self.qtf)

        if self.network is None:
            return

        # draw compartments
        for comp in self.network.compartments:
            self.drawComp(comp, painter, config)

        for reaction in self.network.rxns:
            if hasattr(reaction, 'custom') and reaction.custom.beacon == True:
                painter.setPen(Qt.NoPen)
                painter.setBrush(QtGui.QBrush(tuple2QColor((0.35,0.25,0.8,0.5))))
                painter.drawEllipse(QtCore.QRectF(reaction.centroid.x-20, reaction.centroid.y-20, 40, 40))
            for curve in reaction.curves:
                self.drawCurve(path, painter, curve, config)
                if enable_matplotlib2tikz:
                    self.pypltrender.drawCurve(curve, screentx)
            # centroid mark
            self.qtrender.drawRxnCentroid(path, painter, reaction.centroid, config)


        # draw nodes
        for node in self.network.nodes:
            x, y = self.getNodeScreenSpaceCentroid(node)
            offset = (0,0)
            factor = 1.
            x = (x+offset[0])*factor
            y = (y+offset[1])*factor
            hemiwidth = node.width/2
            hemiheight = node.height/2
            # draw beacon if active
            if hasattr(node, 'custom') and node.custom.beacon == True:
                painter.setPen(Qt.NoPen)
                painter.setBrush(QtGui.QBrush(tuple2QColor((0.25,0.25,0.7,0.5))))
                painter.drawEllipse(QtCore.QRectF(x-hemiwidth-10, y-hemiwidth-10, node.width+20, node.width+20))
            self.qtrender.drawNode(node, x-hemiwidth, y-hemiheight, x+hemiwidth, y+hemiheight, config, painter, horizonEnabled=horizonEnabled)
            if enable_matplotlib2tikz:
              self.pypltrender.drawNode(node, x-hemiwidth, y-hemiheight, x+hemiwidth, y+hemiheight, screentx)

        if config.state.text_halo_enabled:
            glowtextpainter = painter
            text_halo_pen = QtGui.QPen(QtGui.QBrush(QtGui.QColor(255,255,255,15)), 4., QtCore.Qt.SolidLine)
            glowtextpainter.setPen(text_halo_pen)

            glowtextpainter.setFont(QtGui.QFont('sans', 9))

            for node in self.network.nodes:
                x0, y0 = self.getNodeScreenSpaceCentroid(node)
                for x in range(int(x0)-2,int(x0)+2):
                    for y in range(int(y0)-2,int(y0)+2):
                        glowtextpainter.drawText(QtCore.QRectF(x-node.width/2, y-node.height/2, node.width, node.height),
                          QtCore.Qt.AlignCenter, node.name)

        textpainter = painter
        textpen = QPen(QBrush(tuple2QColor(config.state.text_color)), 1., Qt.SolidLine)
        painter.setPen(textpen)

        textpainter.setFont(QtGui.QFont('sans', 9))

        for node in self.network.nodes:
            x, y = self.getNodeScreenSpaceCentroid(node)
            textpainter.drawText(QtCore.QRectF(x-node.width/2, y-node.height/2, node.width, node.height),
                QtCore.Qt.AlignCenter, node.name)

    def renderSVG(self, filename):
        print('renderSVG {}'.format(filename))
        generator = QSvgGenerator()
        generator.setFileName(filename)

        painter = QtGui.QPainter(generator)
        self.render(painter, horizonEnabled=False)

        painter.end()

    def renderPNG(self, filename):
        print('renderPNG {}'.format(filename))
        pixmap = QImage(self.size().width(), self.size().height(), QImage.Format_ARGB32)
        pixmap.fill(Qt.transparent)

        painter = QtGui.QPainter(pixmap)
        self.render(painter, horizonEnabled=True)

        painter.end()

        print('save pixmap {}'.format(filename))
        pixmap.save(filename)

    def renderTikZ(self, filename):
        self.render()
        if enable_matplotlib2tikz:
          self.pypltrender.save(filename)

    def drawCurve(self, pathx, painter, curve, config):
        path = QtGui.QPainterPath()
        path.moveTo(QPoint(curve[0]))
        path.cubicTo(QPoint(curve[1]), QPoint(curve[2]), QPoint(curve[3]))

        if curve[4] == 'SUBSTRATE':
          color = tuple2QColor(config.state.substrate_edge_color)
          width = config.state.substrate_edge_width
          dashed = config.state.substrate_edge_dashed
        elif curve[4] == 'PRODUCT':
          color = tuple2QColor(config.state.product_edge_color)
          width = config.state.product_edge_width
          dashed = config.state.product_edge_dashed
        elif curve[4] == 'ACTIVATOR':
          color = tuple2QColor(config.state.activator_edge_color)
          width = config.state.activator_edge_width
          dashed = config.state.activator_edge_dashed
        elif curve[4] == 'INHIBITOR':
          color = tuple2QColor(config.state.inhibitor_edge_color)
          width = config.state.inhibitor_edge_width
          dashed = config.state.activator_edge_dashed
        elif curve[4] == 'MODIFIER':
          color = tuple2QColor(config.state.modifier_edge_color)
          width = config.state.modifier_edge_width
          dashed = config.state.modifier_edge_dashed

        if dashed:
          pen = QPen(color, width, QtCore.Qt.DashLine)
        else:
          pen = QPen(color, width, QtCore.Qt.SolidLine)
        brush = QBrush(color)

        painter.strokePath(path, pen) # normalpen
        path = QtGui.QPainterPath()

        # arrowhead
        if len(curve) > 5 and len(curve[5]) > 0:
          arrowhead = curve[5]
          path.moveTo(QPoint(arrowhead[0]))
          for v in arrowhead[1:]:
            path.lineTo(QPoint(v))

        if sbnw.arrowpoly_filled(sbnw.get_arrow_style(curve[4])):
          painter.fillPath(path, brush)

        painter.strokePath(path, pen)

    def makeQColorAlpha(self, rgb, alpha):
        return QtGui.QColor((rgb & 0xFF0000) >> 16, (rgb & 0x00FF00) >> 8, rgb & 0x0000FF, alpha)

    def drawComp(self, comp, painter, config):
        if not config.state.compartment_enabled:
          return
        x, y = comp.min.x, comp.min.y
        width, height = comp.width, comp.height
        cornerrad = 10.

        compcolor = tuple2QColor(config.state.compartment_color)
        if self.droptarget is not None and self.droptarget is comp:
            compcolor = compcolor.darker()

        brush = QtGui.QBrush(compcolor)
        painter.setBrush(brush)

        if config.state.compartment_outline_enabled:
          outlinepen = QPen(tuple2QColor(config.state.compartment_outline_color), config.state.compartment_outline_width, Qt.SolidLine)
          painter.setPen(outlinepen)
        else:
          painter.setPen(Qt.NoPen)

        painter.drawRoundedRect(QtCore.QRectF(x, y, width,
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

    # x & y are screen space
    def addComp(self, x1, y1, x2, y2):
        newcomp = self.network.newcomp('NewComp')
        newcomp.min = (min(x1,x2),min(y1,y2))
        newcomp.max = (max(x1,x2),max(y1,y2))

    # x & y are screen space
    def addReaction(self, x, y):
        newrxn = self.network.newreaction('NewRxn')
        newrxn.centroid = x,y

    def removeNode(self, node):
        self.network.removenode(node)

    def aliasNode(self, node):
        self.network.aliasnode(node)

    def findNodeById(self, nodeid):
        '''
        Returns a node with the given id, throws RuntimeError if no such node exists
        '''
        for n in self.network.nodes:
            if n.id == nodeid:
                return n
        raise RuntimeError('No node with id {}'.format(nodeid))

    def findReactionById(self, rxnid):
        '''
        Returns a reaction with the given id, throws RuntimeError if no such reaction exists
        '''
        for r in self.network.reactions:
            if r.id == rxnid:
                return r
        raise RuntimeError('No node with id {}'.format(rxnid))

    def pickNode(self, pickx, picky):
        '''Pick a node on the canvas via e.g. a mouse click.

        Keyword arguments:
        pickx -- the global x coordinate
        picky -- the global y coordinate
        '''

        for node in reversed(self.network.nodes):
            x, y = self.getNodeScreenSpaceCentroid(node)
            hemiwidth = node.width/2
            hemiheight = node.height/2

            if intervalContains(x - hemiwidth, x + hemiwidth, pickx) and intervalContains(y - hemiheight, y + hemiheight, picky):
                return node

        return None

    def pickComp(self, pickx, picky):
        '''Pick a node on the canvas via e.g. a mouse click.

        Keyword arguments:
        pickx -- the global x coordinate
        picky -- the global y coordinate
        '''

        for comp in reversed(self.network.compartments):
            if intervalContains(comp.min.x, comp.max.x, pickx) and intervalContains(comp.min.y, comp.max.y, picky):
                return comp

        return None

    def pickReaction(self, pickx, picky):
        '''Pick a reaction on the canvas via e.g. a mouse click.

        Keyword arguments:
        pickx -- the global x coordinate
        picky -- the global y coordinate
        '''
        for rxn in reversed(self.network.rxns):
            x, y = rxn.centroid
            hemiwidth = 10
            hemiheight = 10

            if intervalContains(x - hemiwidth, x + hemiwidth, pickx) and intervalContains(y - hemiheight, y + hemiheight, picky):
                return rxn

        return None

    # Mouse wheel
    def wheelEvent(self, event):
        if is_pyqt5():
          self.postScale *= math.exp(event.angleDelta().y() / 600.)
        else:
          self.postScale *= math.exp(event.delta() / 600.)
        if self.postScale < 0.01: self.postScale = 0.01
        if self.postScale > 40.: self.postScale = 40.
        self.setScale(self.postScale)

    # Mouse press
    def mousePressEvent(self, event):
        if event.button() == 4:
            mouse = QPoint((event.x(), event.y()))
            self.panning = True
            self.panstart = mouse
        elif event.button() == 1:
            qtfi = self.qtf.inverted()[0]
            mouse = qtfi.map(QPoint((event.x(), event.y())))
            if self.parent().selectToolAct.isChecked():
                # try to drag a node
                node = self.pickNode(mouse.x(), mouse.y())
                if node is not None:
                    node.custom.isBeingDragged = True
                    node.custom.centroidSource = QPoint(self.getNodeScreenSpaceCentroid(node))
                    self.dragging = True
                    self.dragSource = mouse
            elif self.parent().lockToolAct.isChecked():
                node = self.pickNode(mouse.x(), mouse.y())
                if node is not None:
                    if not node.islocked():
                        node.lock()
                    else:
                        node.unlock()
            elif self.parent().eraseToolAct.isChecked():
                node = self.pickNode(mouse.x(), mouse.y())
                if node is not None:
                    self.removeNode(node)
            elif self.parent().aliasToolAct.isChecked():
                node = self.pickNode(mouse.x(), mouse.y())
                if node is not None:
                    self.aliasNode(node)
            elif self.parent().createNodeToolAct.isChecked():
                node = self.pickNode(mouse.x(), mouse.y())
                if node is not None:
                    node.custom.beacon = True
                    self.connecting = True
                    self.connecting_node = node
                    self.connecting_src_node = True
                else:
                    rxn = self.pickReaction(mouse.x(), mouse.y())
                    if rxn is not None:
                        if not hasattr(rxn, 'custom'):
                            rxn.custom = NodeData()
                        rxn.custom.beacon = True
                        self.connecting = True
                        self.connecting_rxn = rxn
                        self.connecting_src_node = False
                    else:
                        self.plantNode = True
                        self.dragSource = mouse
            else:
                # try to drag a reaction
                rxn = self.pickReaction(mouse.x(), mouse.y())
                if rxn is not None:
                    if self.parent().selectToolAct.isChecked():
                        if not hasattr(rxn, 'custom'):
                            rxn.custom = NodeData()
                        rxn.custom.isBeingDragged = True
                        rxn.custom.centroidSource = QPoint(self.getNodeScreenSpaceCentroid(rxn))
                        self.dragging = True
                        dragging_object = True
                        self.dragSource = mouse
                else:
                    # pan view
                    mouse = QPoint((event.x(), event.y()))
                    self.panning = True
                    self.panstart = mouse
        elif event.button() == 2:
            qtfi = self.qtf.inverted()[0]
            mouse = qtfi.map(QPoint((event.x(), event.y())))
            self.plantRxn = True
            self.dragSource = mouse
        self.update()

    # Mouse release
    def mouseReleaseEvent(self, event):
        if event.button() == 1:
            if self.connecting:
                self.connecting = False
                for node in self.network.nodes:
                    node.custom.beacon = False
                for rxn in self.network.rxns:
                    if hasattr(rxn, 'custom'):
                        rxn.custom.beacon = False
                if self.connecting_node is not None and self.connecting_rxn is not None:
                    #if not self.network.is_node_connected(self.connecting_node, self.connecting_rxn):
                        if self.connecting_src_node:
                            self.network.connectnode(self.connecting_node, self.connecting_rxn, 'SUBSTRATE')
                        else:
                            self.network.connectnode(self.connecting_node, self.connecting_rxn, 'PRODUCT')
                self.update()
            elif self.plantNode:
                self.plantNode = False
                qtfi = self.qtf.inverted()[0]
                mouse = qtfi.map(QPoint((event.x(), event.y())))
                self.addNode(mouse.x(), mouse.y())
                self.update()
            elif self.plantComp:
                self.plantComp = False
                qtfi = self.qtf.inverted()[0]
                mouse = qtfi.map(QPoint((event.x(), event.y())))
                self.addComp(self.dragSource.x(), self.dragSource.y(), mouse.x(), mouse.y())
                self.update()
            else:
                if self.panning:
                    self.panning = False
                    self.applyTranslation()
                else:
                    self.dragging = False
                    for node in self.network.nodes:
                        # Python: ahead of the curve in uselessness
                        if node.custom.isBeingDragged and self.droptarget is not None and not self.droptarget.__contains__(node):
                            self.droptarget.add(node)
                        node.custom.isBeingDragged = False
                        node.custom.beacon = False
                    for rxn in self.network.rxns:
                        if hasattr(rxn, 'custom') and rxn.custom.isBeingDragged:
                            rxn.custom.isBeingDragged = False
                    self.droptarget = None
                    self.update()
        elif(event.button() == 4):
            self.panning = False
            self.applyTranslation()
        elif(event.button() == 2):
            if self.plantRxn:
                self.plantRxn = False
                qtfi = self.qtf.inverted()[0]
                mouse = qtfi.map(QPoint((event.x(), event.y())))
                self.addReaction(mouse.x(), mouse.y())
                self.update()


    def applyTranslation(self):
        self.postTranslateBase = self.postTranslate

    def mouseMoveEvent(self, event):
        if self.dragging:
            self.droptarget = None
            qtfi = self.qtf.inverted()[0]
            mouse = qtfi.map(QPoint((event.x(), event.y())))

            delta = mouse - self.dragSource
            for node in self.network.nodes:
                if node.custom.isBeingDragged:
                    newcentroid = node.custom.centroidSource + delta
                    self.setNodeScreenSpaceCentroid(node, (newcentroid.x(), newcentroid.y()))
                    for reaction in self.network.rxns:
                        if reaction.has(node):
                            if self.parent().openconfig().state.auto_recenter_junctions:
                                reaction.recenter()
                            else:
                                reaction.recalccps()
                    comp = self.pickComp(mouse.x(), mouse.y())
                    if comp is not None:
                        self.droptarget = comp
            for rxn in self.network.rxns:
                if hasattr(rxn, 'custom') and rxn.custom.isBeingDragged:
                    newcentroid = rxn.custom.centroidSource + delta
                    self.setNodeScreenSpaceCentroid(rxn, (newcentroid.x(), newcentroid.y()))
                    if self.parent().openconfig().state.auto_recenter_junctions:
                        rxn.recenter()
                    else:
                        rxn.recalccps()
            self.update()
        elif self.panning:
            mouse = QPoint((event.x(), event.y()))
            self.changeTranslate(mouse - self.panstart)
        elif self.connecting:
            qtfi = self.qtf.inverted()[0]
            mouse = qtfi.map(QPoint((event.x(), event.y())))
            if self.connecting_src_node:
                rxn = self.pickReaction(mouse.x(), mouse.y())
                if rxn is not None:
                    if self.connecting_rxn is not None:
                        self.connecting_rxn.custom.beacon = False
                    if not hasattr(rxn, 'custom'):
                        rxn.custom = NodeData()
                    rxn.custom.beacon = True
                    self.connecting_rxn = rxn
            else:
                node = self.pickNode(mouse.x(), mouse.y())
                if node is not None:
                    if self.connecting_node is not None:
                        self.connecting_node.custom.beacon = False
                    node.custom.beacon = True
                    self.connecting_node = node
            self.update()
        elif self.plantNode:
            qtfi = self.qtf.inverted()[0]
            mouse = qtfi.map(QPoint((event.x(), event.y())))
            delta = mouse - self.dragSource
            if delta.x()*delta.x() + delta.y()*delta.y() > 500:
                self.plantNode = False
                self.plantComp = True

    def setScale(self, s):
        self.update()

    def changeTranslate(self, delta):
        self.postTranslate = self.postTranslateBase + delta
        self.update()

class OtherOptsCfgPage(QWidget):
    def __init__(self, parent, config):
      QWidget.__init__(self, parent)

      self.config = config

      self.mainlayout = QVBoxLayout()
      self.setLayout(self.mainlayout)

      # sbmlgroup
      self.sbmlgroup = QFrame(self)
      self.mainlayout.addWidget(self.sbmlgroup)

      self.other_opts_layout = QVBoxLayout()
      self.sbmlgroup.setLayout(self.other_opts_layout)

      self.auto_recenter_junctions = QCheckBox('Auto recenter junctions')
      self.auto_recenter_junctions.stateChanged.connect(self.set_auto_recenter_junctions)
      self.other_opts_layout.addWidget(self.auto_recenter_junctions)

      self.sync_widgets()

    def sync_widgets(self):
      if not self.config.state.auto_recenter_junctions:
        self.auto_recenter_junctions.setCheckState(Qt.Unchecked)
      else:
        self.auto_recenter_junctions.setCheckState(Qt.Checked)

    def set_auto_recenter_junctions(self, state):
      if self.auto_recenter_junctions.checkState() == Qt.Unchecked:
        self.config.state.auto_recenter_junctions = False
      else:
        self.config.state.auto_recenter_junctions = True
      self.sync_widgets()

class SBMLOptsCfgPage(QWidget):
    def __init__(self, parent, config):
      QWidget.__init__(self, parent)

      self.config = config

      self.mainlayout = QVBoxLayout()
      self.setLayout(self.mainlayout)

      # sbmlgroup
      self.sbmlgroup = QFrame(self)
      self.mainlayout.addWidget(self.sbmlgroup)

      self.sbml_opts_layout = QGridLayout()
      self.sbmlgroup.setLayout(self.sbml_opts_layout)

      self.override = QCheckBox('Override Defaults')
      self.override.stateChanged.connect(self.set_override)
      self.sbml_opts_layout.addWidget(self.override, 0, 0)

      self.sbml_ns_group = QGroupBox(self)
      self.sbml_ns_group.setTitle(' SBML Namespace ')
      self.sbml_opts_layout.addWidget(self.sbml_ns_group, 1, 0)

      self.sbml_ns_layout = QGridLayout()
      self.sbml_ns_group.setLayout(self.sbml_ns_layout)

      self.level_label = QLabel('Control the output SBML level/version')
      self.sbml_ns_layout.addWidget(self.level_label, 0, 0)

      self.level_label = QLabel('SBML Level')
      self.sbml_ns_layout.addWidget(self.level_label, 1, 0)
      self.level = QSpinBox(self)
      self.level.setMinimum(1)
      self.level.setMaximum(3)
      self.level.valueChanged.connect(self.set_sbml_level)
      self.sbml_ns_layout.addWidget(self.level, 1, 1)

      self.version_label = QLabel('SBML Version')
      self.sbml_ns_layout.addWidget(self.version_label, 2, 0)
      self.version = QSpinBox(self)
      self.version.valueChanged.connect(self.set_sbml_version)
      self.sbml_ns_layout.addWidget(self.version, 2, 1)

      self.sync_widgets()

    def sync_widgets(self):
      self.level.setValue(self.config.state.sbml_level)
      self.version.setValue(self.config.state.sbml_version)
      if not self.config.state.override_sbml_ns:
        self.override.setCheckState(Qt.Unchecked)
        self.sbml_ns_group.setEnabled(False)
      else:
        self.override.setCheckState(Qt.Checked)
        self.sbml_ns_group.setEnabled(True)
      if self.level.value() == 1:
        self.version.setMinimum(1)
        self.version.setMaximum(2)
      elif self.level.value() == 2:
        self.version.setMinimum(1)
        self.version.setMaximum(5)
      elif self.level.value() == 3:
        self.version.setMinimum(1)
        self.version.setMaximum(1)

    def set_override(self, state):
      if self.override.checkState() == Qt.Unchecked:
        self.config.state.override_sbml_ns = False
      else:
        self.config.state.override_sbml_ns = True
      self.sync_widgets()

    def set_sbml_level(self, val):
      self.config.state.sbml_level = val
      self.sync_widgets()

    def set_sbml_version(self, val):
      self.config.state.sbml_version = val
      self.sync_widgets()

def makeArrowImage(style):
  pixmap = QPixmap(256, 256)
  pixmap.fill(Qt.transparent)

  arrowhead = sbnw.arrowpoly(style)

  if len(arrowhead) > 0:
    tf = QtGui.QTransform()
    tf.translate(128,128)
    tf.scale(100,100)
    tf.rotate(90)

    painter = QPainter(pixmap)
    pen = QPen(Qt.black, 0.2)
    painter.setTransform(tf)

    path = QtGui.QPainterPath()
    path.moveTo(QPointF(arrowhead[0]))
    for v in arrowhead[1:]:
      path.lineTo(QPointF(v))

    painter.strokePath(path, pen)

    if sbnw.arrowpoly_filled(style):
      brush = QBrush(Qt.black)
      painter.fillPath(path, brush)

  return pixmap

def translateRoleToConfigArrowOpt(role):
  if role == 'SUBSTRATE':
    return 'substrate_arrowhead'
  elif role == 'PRODUCT':
    return 'product_arrowhead'
  elif role == 'ACTIVATOR':
    return 'activator_arrowhead'
  elif role == 'INHIBITOR':
    return 'inhibitor_arrowhead'
  elif role == 'MODIFIER':
    return 'modifier_arrowhead'
  else:
    raise RuntimeError('Unknown role')

class DrawStyleCfgPage(QWidget):
  def __init__(self, parent, config):
    QWidget.__init__(self, parent)

    self.config = config

    self.mainlayout = QVBoxLayout()
    self.setLayout(self.mainlayout)

    self.nstyles = sbnw.narrow_styles()

    # icons
    self.icon_width = self.icon_height = 25
    self.icons = []
    for k in range(self.nstyles):
      self.icons.append(QIcon(makeArrowImage(k)))

    # arrowheads
    self.arrowhead_group = QGroupBox(self)
    self.arrowhead_group.setTitle(' Arrowhead Styles ')
    self.mainlayout.addWidget(self.arrowhead_group)

    self.arrowhead_group_layout = QHBoxLayout()
    self.arrowhead_group.setLayout(self.arrowhead_group_layout)


    # arrowhead_selector
    self.arrowhead_selector = QScrollArea(self.arrowhead_group)
    self.arrowhead_selector_layout = QGridLayout()
    self.arrowhead_selector.setLayout(self.arrowhead_selector_layout)
    self.arrowhead_group_layout.addWidget(self.arrowhead_selector)

    # substrate_arrowhead
    self.substrate_arrow_label = QLabel('Substrate Arrowhead')
    self.arrowhead_selector_layout.addWidget(self.substrate_arrow_label, 0, 0)
    self.substrate_arrowhead = QToolButton()
    self.arrowhead_selector_layout.addWidget(self.substrate_arrowhead, 0, 1)
    self.substrate_arrowhead.setIcon(self.icons[self.config.state.substrate_arrowhead])
    self.substrate_arrowhead.setIconSize(QSize(self.icon_width,self.icon_height))

    self.substrate_menu = self.makeArrowMenu('SUBSTRATE', self.substrate_arrowhead)
    self.substrate_arrowhead.setMenu(self.substrate_menu)
    self.substrate_arrowhead.setPopupMode(QToolButton.InstantPopup)

    # product_arrowhead
    self.product_arrow_label = QLabel('Product Arrowhead')
    self.arrowhead_selector_layout.addWidget(self.product_arrow_label, 1, 0)
    self.product_arrowhead = QToolButton()
    self.arrowhead_selector_layout.addWidget(self.product_arrowhead, 1, 1)
    self.product_arrowhead.setIcon(self.icons[self.config.state.product_arrowhead])
    self.product_arrowhead.setIconSize(QSize(self.icon_width,self.icon_height))

    self.product_menu = self.makeArrowMenu('PRODUCT', self.product_arrowhead)
    self.product_arrowhead.setMenu(self.product_menu)
    self.product_arrowhead.setPopupMode(QToolButton.InstantPopup)

    # activator_arrowhead
    self.activator_arrow_label = QLabel('Activator Arrowhead')
    self.arrowhead_selector_layout.addWidget(self.activator_arrow_label, 2, 0)
    self.activator_arrowhead = QToolButton()
    self.arrowhead_selector_layout.addWidget(self.activator_arrowhead, 2, 1)
    self.activator_arrowhead.setIcon(self.icons[self.config.state.activator_arrowhead])
    self.activator_arrowhead.setIconSize(QSize(self.icon_width,self.icon_height))

    self.activator_menu = self.makeArrowMenu('ACTIVATOR', self.activator_arrowhead)
    self.activator_arrowhead.setMenu(self.activator_menu)
    self.activator_arrowhead.setPopupMode(QToolButton.InstantPopup)

    # inhibitor_arrowhead
    self.inhibitor_arrow_label = QLabel('Inhibitor Arrowhead')
    self.arrowhead_selector_layout.addWidget(self.inhibitor_arrow_label, 3, 0)
    self.inhibitor_arrowhead = QToolButton()
    self.arrowhead_selector_layout.addWidget(self.inhibitor_arrowhead, 3, 1)
    self.inhibitor_arrowhead.setIcon(self.icons[self.config.state.inhibitor_arrowhead])
    self.inhibitor_arrowhead.setIconSize(QSize(self.icon_width,self.icon_height))

    self.inhibitor_menu = self.makeArrowMenu('INHIBITOR', self.inhibitor_arrowhead)
    self.inhibitor_arrowhead.setMenu(self.inhibitor_menu)
    self.inhibitor_arrowhead.setPopupMode(QToolButton.InstantPopup)

    # modifier_arrowhead
    self.modifier_arrow_label = QLabel('Modifier Arrowhead')
    self.arrowhead_selector_layout.addWidget(self.modifier_arrow_label, 4, 0)
    self.modifier_arrowhead = QToolButton()
    self.arrowhead_selector_layout.addWidget(self.modifier_arrowhead, 4, 1)
    self.modifier_arrowhead.setIcon(self.icons[self.config.state.modifier_arrowhead])
    self.modifier_arrowhead.setIconSize(QSize(self.icon_width,self.icon_height))

    self.modifier_menu = self.makeArrowMenu('MODIFIER', self.modifier_arrowhead)
    self.modifier_arrowhead.setMenu(self.modifier_menu)
    self.modifier_arrowhead.setPopupMode(QToolButton.InstantPopup)

    self.config.install_listener(self.sync_widgets)

  def sync_widgets(self):
    buttons = [self.substrate_arrowhead, self.product_arrowhead, self.activator_arrowhead, self.inhibitor_arrowhead, self.modifier_arrowhead]
    styles = ['substrate_arrowhead', 'product_arrowhead', 'activator_arrowhead', 'inhibitor_arrowhead', 'modifier_arrowhead']

    for b,s in zip(buttons, styles):
      b.setIcon(self.icons[getattr(self.config.state, s)])

  def setStyle(self, role, style, button):
    def setc():
      print('set style for {} to {}'.format(role, style))
      button.setIcon(self.icons[style])
      setattr(self.config.state, translateRoleToConfigArrowOpt(role), style)
    return setc

  def makeArrowMenu(self, role, button):
    menu = QMenu(role)

    menu_buttons = []
    for k in range(self.nstyles):
      menu_buttons.append(QPushButton('', menu))
      menu_buttons[k].setIcon(self.icons[k])
      menu_buttons[k].setIconSize(QSize(self.icon_width,self.icon_height))
      menu_act = QWidgetAction(menu)
      menu_act.setDefaultWidget(menu_buttons[k])
      menu_buttons[k].clicked.connect(menu_act.triggered)
      menu_buttons[k].clicked.connect(menu.hide)
      menu_act.triggered.connect(self.setStyle(role, k, button))
      menu.addAction(menu_act)

    return menu

class ColorCfgPage(QWidget):
    def __init__(self, parent, config):
      QWidget.__init__(self, parent)

      self.config = config

      self.mainlayout = QVBoxLayout()
      self.setLayout(self.mainlayout)

      # colorgroup
      self.colorgroup = QGroupBox(self)
      self.colorgroup.setTitle(' Effect ')
      self.mainlayout.addWidget(self.colorgroup)
      self.colorgroup.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

      self.color_group_layout = QHBoxLayout()
      self.colorgroup.setLayout(self.color_group_layout)

      # color_effect_selector
      self.color_effect_selector = QFrame(self.colorgroup)
      self.color_effect_selector_layout = QVBoxLayout()
      self.color_effect_selector.setLayout(self.color_effect_selector_layout)
      self.color_group_layout.addWidget(self.color_effect_selector)

      # advanced
      self.advanced = QRadioButton()
      self.advanced.setText('Default')
      self.color_effect_selector_layout.addWidget(self.advanced)
      self.advanced.clicked.connect(self.set_effect_advanced)

      # basic
      self.basic = QRadioButton()
      self.basic.setText('Custom')
      self.color_effect_selector_layout.addWidget(self.basic)
      self.basic.clicked.connect(self.set_effect_icky)

      self.color_effect_selector_layout.addStretch()

      # color_selector
      self.color_selector = QScrollArea(self.colorgroup)
      self.color_group_layout.addWidget(self.color_selector)

      self.color_selector_inner = QWidget(self.color_selector)
      self.color_selector_layout = QGridLayout()
      self.color_selector_inner.setLayout(self.color_selector_layout)
      self.color_selector.setWidget(self.color_selector_inner)
      self.color_selector_inner.setMinimumSize(300,700)

      row = 0

      # preset
      self.preset_label = QLabel('Preset')
      self.color_selector_layout.addWidget(self.preset_label, row, 0)
      self.preset = QToolButton()
      self.preset.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
      self.preset.setPopupMode(QToolButton.InstantPopup)
      self.color_selector_layout.addWidget(self.preset, row, 1)

      self.preset_menu = QMenu('Preset')
      self.preset_default_act = QAction('Default', self.preset_menu)
      self.preset_default_act.triggered.connect(self.set_preset_default)
      self.preset_menu.addAction(self.preset_default_act)
      self.preset_jd_act = QAction('JDesigner', self.preset_menu)
      self.preset_jd_act.triggered.connect(self.set_preset_jd)
      self.preset_menu.addAction(self.preset_jd_act)

      self.preset.setMenu(self.preset_menu)
      row += 1

      # node_color1
      self.node_color1_label = QLabel('Node Color1')
      self.color_selector_layout.addWidget(self.node_color1_label, row, 0)
      self.node_color1 = QPushButton()
      self.color_selector_layout.addWidget(self.node_color1, row, 1)
      self.node_color1.clicked.connect(self.choose_node_color1)
      row += 1

      # node_color2
      self.node_color2_label = QLabel('Node Color2')
      self.color_selector_layout.addWidget(self.node_color2_label, row, 0)
      self.node_color2 = QPushButton()
      self.color_selector_layout.addWidget(self.node_color2, row, 1)
      self.node_color2.clicked.connect(self.choose_node_color2)
      row += 1

      # node_outline_color
      self.node_outline_color_label = QLabel('Outline Color')
      self.color_selector_layout.addWidget(self.node_outline_color_label, row, 0)
      self.node_outline_color = QPushButton()
      self.color_selector_layout.addWidget(self.node_outline_color, row, 1)
      self.node_outline_color.clicked.connect(self.choose_node_outline_color)
      row += 1

      # compartment_enabled
      self.compartment_enabled = QCheckBox('Compartment Enabled')
      self.color_selector_layout.addWidget(self.compartment_enabled, row, 0)
      self.compartment_enabled.stateChanged.connect(self.set_compartment_enabled)
      row += 1

      # compartment_outline_enabled
      self.compartment_outline_enabled = QCheckBox('Compartment Outline Enabled')
      self.color_selector_layout.addWidget(self.compartment_outline_enabled, row, 0)
      self.compartment_outline_enabled.stateChanged.connect(self.set_compartment_outline_enabled)
      row += 1

      # compartment_color
      self.compartment_color_label = QLabel('Compartment Color')
      self.color_selector_layout.addWidget(self.compartment_color_label, row, 0)
      self.compartment_color = QPushButton()
      self.color_selector_layout.addWidget(self.compartment_color, row, 1)
      self.compartment_color.clicked.connect(self.choose_compartment_color)
      row += 1

      # compartment_outline_color
      self.compartment_outline_color_label = QLabel('Compartment Outline Color')
      self.color_selector_layout.addWidget(self.compartment_outline_color_label, row, 0)
      self.compartment_outline_color = QPushButton()
      self.color_selector_layout.addWidget(self.compartment_outline_color, row, 1)
      self.compartment_outline_color.clicked.connect(self.choose_compartment_outline_color)
      row += 1

      # compartment_outline_width
      self.compartment_outline_width_label = QLabel('Compartment Outline Width')
      self.color_selector_layout.addWidget(self.compartment_outline_width_label, row, 0)
      self.compartment_outline_width = QDoubleSpinBox()
      self.compartment_outline_width.valueChanged.connect(self.set_compartment_outline_width)
      self.color_selector_layout.addWidget(self.compartment_outline_width, row, 1)
      row += 1

      # centroid_color
      self.centroid_color_label = QLabel('Centroid Color')
      self.color_selector_layout.addWidget(self.centroid_color_label, row, 0)
      self.centroid_color = QPushButton()
      self.color_selector_layout.addWidget(self.centroid_color, row, 1)
      self.centroid_color.clicked.connect(self.choose_centroid_color)
      row += 1

      # centroid_enabled
      self.centroid_enabled = QCheckBox('Centroid Enabled')
      self.color_selector_layout.addWidget(self.centroid_enabled, row, 0)
      self.centroid_enabled.stateChanged.connect(self.set_centroid_enabled)
      row += 1

      # centroid_outline_enabled
      self.centroid_outline_enabled = QCheckBox('Centroid Outline Enabled')
      self.color_selector_layout.addWidget(self.centroid_outline_enabled, row, 0)
      self.centroid_outline_enabled.stateChanged.connect(self.set_centroid_outline_enabled)
      row += 1

      # centroid_outline_color
      self.centroid_outline_color_label = QLabel('Centroid Outline Color')
      self.color_selector_layout.addWidget(self.centroid_outline_color_label, row, 0)
      self.centroid_outline_color = QPushButton()
      self.color_selector_layout.addWidget(self.centroid_outline_color, row, 1)
      self.centroid_outline_color.clicked.connect(self.choose_centroid_outline_color)
      row += 1

      # centroid_outline_width
      self.centroid_outline_width_label = QLabel('Centroid Outline Width')
      self.color_selector_layout.addWidget(self.centroid_outline_width_label, row, 0)
      self.centroid_outline_width = QDoubleSpinBox()
      self.centroid_outline_width.valueChanged.connect(self.set_centroid_outline_width)
      self.color_selector_layout.addWidget(self.centroid_outline_width, row, 1)
      row += 1

      # text_color
      self.text_color_label = QLabel('Text Color')
      self.color_selector_layout.addWidget(self.text_color_label, row, 0)
      self.text_color = QPushButton()
      self.color_selector_layout.addWidget(self.text_color, row, 1)
      self.text_color.clicked.connect(self.choose_text_color)
      row += 1

      # text_halo_enabled
      self.text_halo_enabled = QCheckBox('Text Halo')
      self.color_selector_layout.addWidget(self.text_halo_enabled, row, 0)
      self.text_halo_enabled.stateChanged.connect(self.set_text_halo_enabled)
      row += 1

      # substrate_edge_color
      self.substrate_edge_color_label = QLabel('Substrate Edge Color')
      self.color_selector_layout.addWidget(self.substrate_edge_color_label, row, 0)
      self.substrate_edge_color = QPushButton()
      self.color_selector_layout.addWidget(self.substrate_edge_color, row, 1)
      self.substrate_edge_color.clicked.connect(self.choose_substrate_edge_color)
      row += 1

      # product_edge_color
      self.product_edge_color_label = QLabel('Product Edge Color')
      self.color_selector_layout.addWidget(self.product_edge_color_label, row, 0)
      self.product_edge_color = QPushButton()
      self.color_selector_layout.addWidget(self.product_edge_color, row, 1)
      self.product_edge_color.clicked.connect(self.choose_product_edge_color)
      row += 1

      # activator_edge_color
      self.activator_edge_color_label = QLabel('Activator Edge Color')
      self.color_selector_layout.addWidget(self.activator_edge_color_label, row, 0)
      self.activator_edge_color = QPushButton()
      self.color_selector_layout.addWidget(self.activator_edge_color, row, 1)
      self.activator_edge_color.clicked.connect(self.choose_activator_edge_color)
      row += 1

      # inhibitor_edge_color
      self.inhibitor_edge_color_label = QLabel('Inhibitor Edge Color')
      self.color_selector_layout.addWidget(self.inhibitor_edge_color_label, row, 0)
      self.inhibitor_edge_color = QPushButton()
      self.color_selector_layout.addWidget(self.inhibitor_edge_color, row, 1)
      self.inhibitor_edge_color.clicked.connect(self.choose_inhibitor_edge_color)
      row += 1

      # modifier_edge_color
      self.modifier_edge_color_label = QLabel('Modifier Edge Color')
      self.color_selector_layout.addWidget(self.modifier_edge_color_label, row, 0)
      self.modifier_edge_color = QPushButton()
      self.color_selector_layout.addWidget(self.modifier_edge_color, row, 1)
      self.modifier_edge_color.clicked.connect(self.choose_modifier_edge_color)
      row += 1

      # substrate_edge_width
      self.substrate_edge_width_label = QLabel('Substrate Edge Width')
      self.color_selector_layout.addWidget(self.substrate_edge_width_label, row, 0)
      self.substrate_edge_width = QDoubleSpinBox()
      self.substrate_edge_width.valueChanged.connect(self.set_substrate_edge_width)
      self.color_selector_layout.addWidget(self.substrate_edge_width, row, 1)
      row += 1

      # product_edge_width
      self.product_edge_width_label = QLabel('Product Edge Width')
      self.color_selector_layout.addWidget(self.product_edge_width_label, row, 0)
      self.product_edge_width = QDoubleSpinBox()
      self.product_edge_width.valueChanged.connect(self.set_product_edge_width)
      self.color_selector_layout.addWidget(self.product_edge_width, row, 1)
      row += 1

      # activator_edge_width
      self.activator_edge_width_label = QLabel('Activator Edge Width')
      self.color_selector_layout.addWidget(self.activator_edge_width_label, row, 0)
      self.activator_edge_width = QDoubleSpinBox()
      self.activator_edge_width.valueChanged.connect(self.set_activator_edge_width)
      self.color_selector_layout.addWidget(self.activator_edge_width, row, 1)
      row += 1

      # inhibitor_edge_width
      self.inhibitor_edge_width_label = QLabel('Inhibitor Edge Width')
      self.color_selector_layout.addWidget(self.inhibitor_edge_width_label, row, 0)
      self.inhibitor_edge_width = QDoubleSpinBox()
      self.inhibitor_edge_width.valueChanged.connect(self.set_inhibitor_edge_width)
      self.color_selector_layout.addWidget(self.inhibitor_edge_width, row, 1)
      row += 1

      # modifier_edge_width
      self.modifier_edge_width_label = QLabel('Modifier Edge Width')
      self.color_selector_layout.addWidget(self.modifier_edge_width_label, row, 0)
      self.modifier_edge_width = QDoubleSpinBox()
      self.modifier_edge_width.valueChanged.connect(self.set_modifier_edge_width)
      self.color_selector_layout.addWidget(self.modifier_edge_width, row, 1)
      row += 1

      self.sync_widgets()
      self.config.install_listener(self.sync_widgets)

    def __del__(self):
      print('ColorCfgPage del')
      self.config.remove_listener(self.sync_widgets)

    def sync_widgets(self):
      if self.config.state.node_effect == 'advanced':
        self.advanced.setChecked(True)

      if self.config.state.node_effect == 'basic':
        self.basic.setChecked(True)

      if not self.basic.isChecked():
        self.color_selector.setVisible(False)

      self.node_color1.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.node_color1).name() + ';}')
      self.node_color2.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.node_color2).name() + ';}')
      self.node_outline_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.node_outline_color).name() + ';}')
      self.compartment_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.compartment_color).name() + ';}')
      self.compartment_outline_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.compartment_outline_color).name() + ';}')
      self.text_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.text_color).name() + ';}')

      self.compartment_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.compartment_color).name() + ';}')
      self.compartment_outline_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.compartment_outline_color).name() + ';}')
      self.compartment_outline_width.setValue(self.config.state.compartment_outline_width)

      # set compartment enabled state
      if self.config.state.compartment_enabled:
        self.compartment_enabled.setCheckState(Qt.Checked)
      else:
        self.compartment_enabled.setCheckState(Qt.Unchecked)

      # set compartment outline enabled state
      if self.config.state.compartment_outline_enabled:
        self.compartment_outline_enabled.setCheckState(Qt.Checked)
      else:
        self.compartment_outline_enabled.setCheckState(Qt.Unchecked)

      if self.compartment_outline_enabled.checkState() == Qt.Unchecked:
        self.compartment_outline_color_label.setEnabled(False)
        self.compartment_outline_color.setEnabled(False)
        self.compartment_outline_width_label.setEnabled(False)
        self.compartment_outline_width.setEnabled(False)
      else:
        self.compartment_outline_color_label.setEnabled(True)
        self.compartment_outline_color.setEnabled(True)
        self.compartment_outline_width_label.setEnabled(True)
        self.compartment_outline_width.setEnabled(True)

      if self.compartment_enabled.checkState() == Qt.Unchecked:
        self.compartment_color_label.setEnabled(False)
        self.compartment_color.setEnabled(False)
        self.compartment_outline_enabled.setEnabled(False)
        self.compartment_outline_color_label.setEnabled(False)
        self.compartment_outline_color.setEnabled(False)
        self.compartment_outline_width_label.setEnabled(False)
        self.compartment_outline_width.setEnabled(False)
      else:
        self.compartment_color_label.setEnabled(True)
        self.compartment_color.setEnabled(True)
        self.compartment_outline_enabled.setEnabled(True)

      self.centroid_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.centroid_color).name() + ';}')
      self.centroid_outline_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.centroid_outline_color).name() + ';}')
      self.centroid_outline_width.setValue(self.config.state.centroid_outline_width)

      # set centroid enabled state
      if self.config.state.centroid_enabled:
        self.centroid_enabled.setCheckState(Qt.Checked)
      else:
        self.centroid_enabled.setCheckState(Qt.Unchecked)

      # set centroid outline enabled state
      if self.config.state.centroid_outline_enabled:
        self.centroid_outline_enabled.setCheckState(Qt.Checked)
      else:
        self.centroid_outline_enabled.setCheckState(Qt.Unchecked)

      if self.centroid_outline_enabled.checkState() == Qt.Unchecked:
        self.centroid_outline_color_label.setEnabled(False)
        self.centroid_outline_color.setEnabled(False)
        self.centroid_outline_width_label.setEnabled(False)
        self.centroid_outline_width.setEnabled(False)
      else:
        self.centroid_outline_color_label.setEnabled(True)
        self.centroid_outline_color.setEnabled(True)
        self.centroid_outline_width_label.setEnabled(True)
        self.centroid_outline_width.setEnabled(True)

      if self.centroid_enabled.checkState() == Qt.Unchecked:
        self.centroid_outline_enabled.setEnabled(False)
        self.centroid_outline_color_label.setEnabled(False)
        self.centroid_outline_color.setEnabled(False)
        self.centroid_outline_width_label.setEnabled(False)
        self.centroid_outline_width.setEnabled(False)
      else:
        self.centroid_outline_enabled.setEnabled(True)

      if self.config.state.text_halo_enabled:
        self.text_halo_enabled.setCheckState(Qt.Checked)
      else:
        self.text_halo_enabled.setCheckState(Qt.Unchecked)
      self.centroid_outline_width.setValue(self.config.state.centroid_outline_width)
      self.substrate_edge_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.substrate_edge_color).name() + ';}')
      self.product_edge_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.product_edge_color).name() + ';}')
      self.activator_edge_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.activator_edge_color).name() + ';}')
      self.inhibitor_edge_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.inhibitor_edge_color).name() + ';}')
      self.modifier_edge_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.modifier_edge_color).name() + ';}')

      self.substrate_edge_width.setValue(self.config.state.substrate_edge_width)
      self.product_edge_width.setValue(self.config.state.product_edge_width)
      self.activator_edge_width.setValue(self.config.state.activator_edge_width)
      self.inhibitor_edge_width.setValue(self.config.state.inhibitor_edge_width)
      self.modifier_edge_width.setValue(self.config.state.modifier_edge_width)

    def set_effect_advanced(self):
      self.config.state.node_effect = 'advanced'
      self.color_selector.setVisible(False)
      #self.update_main_window()

    def set_effect_icky(self):
      self.config.state.node_effect = 'basic'
      self.color_selector.setVisible(True)

    def choose_node_color1(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.node_color1), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.node_color1 = QColor2tuple(result)
      self.node_color1.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.node_color1).name() + ';}')

    def choose_node_color2(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.node_color2), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.node_color2 = QColor2tuple(result)
      self.node_color2.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.node_color2).name() + ';}')

    def choose_centroid_color(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.centroid_color), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.centroid_color = QColor2tuple(result)
      self.centroid_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.centroid_color).name() + ';}')

    def choose_centroid_outline_color(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.centroid_outline_color), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.centroid_outline_color = QColor2tuple(result)
      self.centroid_outline_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.centroid_outline_color).name() + ';}')

    def choose_node_outline_color(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.node_outline_color), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.node_outline_color = QColor2tuple(result)
      self.node_outline_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.node_outline_color).name() + ';}')

    def choose_compartment_color(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.compartment_color), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.compartment_color = QColor2tuple(result)
      self.compartment_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.compartment_color).name() + ';}')

    def choose_compartment_outline_color(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.compartment_outline_color), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.compartment_outline_color = QColor2tuple(result)
      self.compartment_outline_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.compartment_outline_color).name() + ';}')

    def choose_text_color(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.text_color), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.text_color = QColor2tuple(result)
      self.text_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.text_color).name() + ';}')

    def choose_substrate_edge_color(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.substrate_edge_color), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.substrate_edge_color = QColor2tuple(result)
      self.substrate_edge_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.substrate_edge_color).name() + ';}')

    def choose_product_edge_color(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.product_edge_color), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.product_edge_color = QColor2tuple(result)
      self.product_edge_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.product_edge_color).name() + ';}')

    def choose_activator_edge_color(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.activator_edge_color), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.activator_edge_color = QColor2tuple(result)
      self.activator_edge_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.activator_edge_color).name() + ';}')

    def choose_inhibitor_edge_color(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.inhibitor_edge_color), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.inhibitor_edge_color = QColor2tuple(result)
      self.inhibitor_edge_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.inhibitor_edge_color).name() + ';}')

    def choose_modifier_edge_color(self):
      result = QColorDialog.getColor(tuple2QColor(self.config.state.modifier_edge_color), self, 'Select Color', QColorDialog.ShowAlphaChannel)
      if result.isValid():
        self.config.state.modifier_edge_color = QColor2tuple(result)
      self.modifier_edge_color.setStyleSheet('QPushButton {background-color: ' + tuple2QColor(self.config.state.modifier_edge_color).name() + ';}')

    def set_node_outline_width(self, val):
      self.config.state.node_outline_width = val

    def set_compartment_outline_width(self, val):
      self.config.state.compartment_outline_width = val

    def set_centroid_outline_width(self, val):
      self.config.state.centroid_outline_width = val

    def set_substrate_edge_width(self, val):
      self.config.state.substrate_edge_width = val

    def set_product_edge_width(self, val):
      self.config.state.product_edge_width = val

    def set_activator_edge_width(self, val):
      self.config.state.activator_edge_width = val

    def set_inhibitor_edge_width(self, val):
      self.config.state.inhibitor_edge_width = val

    def set_modifier_edge_width(self, val):
      self.config.state.modifier_edge_width = val

    def set_compartment_enabled(self, state):
      if self.compartment_enabled.checkState() == Qt.Unchecked:
        self.config.state.compartment_enabled = False

        self.compartment_color_label.setEnabled(False)
        self.compartment_color.setEnabled(False)

        self.compartment_outline_color_label.setEnabled(False)
        self.compartment_outline_color.setEnabled(False)

        self.compartment_outline_enabled.setEnabled(False)
        self.compartment_outline_width_label.setEnabled(False)
        self.compartment_outline_width.setEnabled(False)
      else:
        self.config.state.compartment_enabled = True

        self.compartment_color_label.setEnabled(True)
        self.compartment_color.setEnabled(True)

        if self.compartment_outline_enabled.checkState() != Qt.Unchecked:
          self.compartment_outline_color_label.setEnabled(True)
          self.compartment_outline_color.setEnabled(True)

          self.compartment_outline_enabled.setEnabled(True)
          self.compartment_outline_width_label.setEnabled(True)
          self.compartment_outline_width.setEnabled(True)
        else:
          self.compartment_outline_color_label.setEnabled(False)
          self.compartment_outline_color.setEnabled(False)

          self.compartment_outline_enabled.setEnabled(True)
          self.compartment_outline_width_label.setEnabled(False)
          self.compartment_outline_width.setEnabled(False)

    def set_compartment_outline_enabled(self, state):
      if self.compartment_outline_enabled.checkState() == Qt.Unchecked:
        self.config.state.compartment_outline_enabled = False

        self.compartment_outline_color_label.setEnabled(False)
        self.compartment_outline_color.setEnabled(False)

        self.compartment_outline_width_label.setEnabled(False)
        self.compartment_outline_width.setEnabled(False)
      else:
        self.config.state.compartment_outline_enabled = True

        self.compartment_outline_color_label.setEnabled(True)
        self.compartment_outline_color.setEnabled(True)

        self.compartment_outline_width_label.setEnabled(True)
        self.compartment_outline_width.setEnabled(True)
      #self.sync_widgets()

    def set_centroid_enabled(self, state):
      if self.centroid_enabled.checkState() == Qt.Unchecked:
        self.config.state.centroid_enabled = False

        self.centroid_outline_color_label.setEnabled(False)
        self.centroid_outline_color.setEnabled(False)

        self.centroid_outline_enabled.setEnabled(False)
        self.centroid_outline_width_label.setEnabled(False)
        self.centroid_outline_width.setEnabled(False)
      else:
        self.config.state.centroid_enabled = True
        if self.centroid_outline_enabled.checkState() != Qt.Unchecked:
          self.centroid_outline_color_label.setEnabled(True)
          self.centroid_outline_color.setEnabled(True)

          self.centroid_outline_enabled.setEnabled(True)
          self.centroid_outline_width_label.setEnabled(True)
          self.centroid_outline_width.setEnabled(True)
        else:
          self.centroid_outline_color_label.setEnabled(False)
          self.centroid_outline_color.setEnabled(False)

          self.centroid_outline_enabled.setEnabled(True)
          self.centroid_outline_width_label.setEnabled(False)
          self.centroid_outline_width.setEnabled(False)

    def set_centroid_outline_enabled(self, state):
      if self.centroid_outline_enabled.checkState() == Qt.Unchecked:
        self.config.state.centroid_outline_enabled = False

        self.centroid_outline_color_label.setEnabled(False)
        self.centroid_outline_color.setEnabled(False)

        self.centroid_outline_width_label.setEnabled(False)
        self.centroid_outline_width.setEnabled(False)
      else:
        self.config.state.centroid_outline_enabled = True

        self.centroid_outline_color_label.setEnabled(True)
        self.centroid_outline_color.setEnabled(True)

        self.centroid_outline_width_label.setEnabled(True)
        self.centroid_outline_width.setEnabled(True)
      #self.sync_widgets()

    def set_centroid_outline_width(self, val):
      self.config.state.centroid_outline_width = val
      #self.sync_widgets()

    def set_text_halo_enabled(self, state):
      if self.text_halo_enabled.checkState() == Qt.Unchecked:
        self.config.state.text_halo_enabled = False
      else:
        self.config.state.text_halo_enabled = True

    def set_preset_default(self):
      defaults = get_default_options()
      for k,v in dict_iteritems({
        'node_color1': make_color_rgba(0.1, 0.4, 0.9, 1.),
        'node_color2': make_color_rgba(0.8, 0.9, 0.95, 1.),
        'node_outline_color': make_color_rgba(0., 0., 0., 1.),
        'compartment_color': make_color_rgba(0.6, 0.3, 0.8, 0.3),
        'compartment_outline_color': make_color_rgba(0., 0., 0., 1.),
        'substrate_edge_color': make_color_rgba(0., 0., 0., 1.),
        'product_edge_color': make_color_rgba(0., 0., 0., 1.),
        'activator_edge_color': make_color_rgba(0.2, 0.2, 0.4, 1.),
        'inhibitor_edge_color': make_color_rgba(1., 0.5, 0.5, 1.),
        'modifier_edge_color': make_color_rgba(0.2, 0.2, 0.4, 1.),
        'text_color': make_color_rgba(0., 0., 0., 1.),
        'centroid_color': make_color_rgba(0.5, 0.9, 0.5, 0.5),
        'centroid_outline_color': make_color_rgba(0., 0., 0., 1.),
        'centroid_outline_enabled': True,
        'centroid_enabled': True,

        'node_outline_width': 1.,
        'compartment_outline_width': 1.,
        'centroid_outline_width': 1.,
        'substrate_edge_width': 1.,
        'product_edge_width': 1.,
        'activator_edge_width': 1.,
        'inhibitor_edge_width': 1.,

        'node_corner_radius': 4.,
        'compartment_enabled': True,
        'compartment_outline_enabled': True,
        }):
        setattr(self.config.state, k, v)

      self.sync_widgets()

    def set_preset_jd(self):
      defaults = get_default_options()
      for k,v in dict_iteritems({
        'node_color1': make_color_rgba(1., 1., 1., 1.),
        'node_color2': make_color_rgba(1., 209./255., 160./255., 1.),
        'node_outline_color': make_color_rgba(1., 101./255., 0., 1.),
        'compartment_color': make_color_rgba(1., 1., 1., 1.),
        'compartment_outline_color': make_color_rgba(0., 166./255., 1., 1.),
        'substrate_edge_color': make_color_rgba(50./255., 154./255., 100./255., 1.),
        'product_edge_color': make_color_rgba(50./255., 154./255., 100./255., 1.),
        'activator_edge_color': make_color_rgba(0.2, 0.2, 0.4, 1.),
        'inhibitor_edge_color': make_color_rgba(1., 0.5, 0.5, 1.),
        'modifier_edge_color': make_color_rgba(0.2, 0.2, 0.4, 1.),
        'text_color': make_color_rgba(0., 0., 0., 1.),
        'centroid_color': make_color_rgba(0.5, 0.9, 0.5, 0.5),
        'centroid_outline_color': make_color_rgba(0., 0., 0., 1.),
        'centroid_outline_enabled': False,
        'centroid_enabled': False,

        'node_outline_width': 2.,
        'compartment_outline_width': 4.,
        'centroid_outline_width': 1.,
        'substrate_edge_width': 2.,
        'product_edge_width': 2.,
        'activator_edge_width': 2.,
        'inhibitor_edge_width': 2.,

        'node_corner_radius': 12.,
        'compartment_enabled': True,
        'compartment_outline_enabled': True,
        }):
        setattr(self.config.state, k, v)

      self.sync_widgets()

class PrefDialog(QDialog):
    def __init__(self, parent):
      QDialog.__init__(self, parent)

      # http://doc.qt.io/qt-5/qtwidgets-dialogs-configdialog-configdialog-cpp.html

      self.mainlayout = QVBoxLayout()
      self.setLayout(self.mainlayout)

      self.cfgpage = QFrame()
      self.cfgpagelayout = QHBoxLayout()
      self.cfgpage.setLayout(self.cfgpagelayout)
      self.mainlayout.addWidget(self.cfgpage)
      self.cfgpage.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

      self.cfgbrowser = QListWidget()
      self.cfgpagelayout.addWidget(self.cfgbrowser)
      self.cfgbrowser.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Expanding)

      # List of config pages

      self.color_page_item = QListWidgetItem(self.cfgbrowser)
      self.color_page_item.setText('Rendering')

      self.draw_style_page_item = QListWidgetItem(self.cfgbrowser)
      self.draw_style_page_item.setText('Draw Style')

      self.sbml_page_item = QListWidgetItem(self.cfgbrowser)
      self.sbml_page_item.setText('SBML')

      self.other_page_item = QListWidgetItem(self.cfgbrowser)
      self.other_page_item.setText('Other')

      self.cfgbrowser.setMaximumWidth(self.cfgbrowser.sizeHintForColumn(0)*2)

      self.cfgbrowser.currentItemChanged.connect(self.page_changed)

      self.config = parent.openconfig()
      parent.pushconfig(self.config)
      self.mainwindow = parent
      self.config.install_listener(self.mainwindow.notify_config_changed)

      # pages

      self.pages = QStackedWidget(self)
      self.cfgpagelayout.addWidget(self.pages)
      self.pages.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

      # color config page

      self.color_cfg_page = ColorCfgPage(self, self.config)
      self.pages.addWidget(self.color_cfg_page)

      # draw style config page

      self.draw_style_page = DrawStyleCfgPage(self, self.config)
      self.pages.addWidget(self.draw_style_page)

      # SBML options config page

      self.sbml_page = SBMLOptsCfgPage(self, self.config)
      self.pages.addWidget(self.sbml_page)

      # Other options config page

      self.other_page = OtherOptsCfgPage(self, self.config)
      self.pages.addWidget(self.other_page)

      # active page
      if self.config.state.active_config_section == 'render_effect':
        self.cfgbrowser.setCurrentRow(0)
        self.pages.setCurrentIndex(0)
      elif self.config.state.active_config_section == 'draw_style':
        self.cfgbrowser.setCurrentRow(1)
        self.pages.setCurrentIndex(1)
      elif self.config.state.active_config_section == 'sbml_options':
        self.cfgbrowser.setCurrentRow(2)
        self.pages.setCurrentIndex(2)
      elif self.config.state.active_config_section == 'other_options':
        self.cfgbrowser.setCurrentRow(3)
        self.pages.setCurrentIndex(3)

      # buttons

      self.buttonframe = QFrame(self)
      self.mainlayout.addWidget(self.buttonframe)
      self.cghlayout = QHBoxLayout()
      self.buttonframe.setLayout(self.cghlayout)
      self.buttonframe.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum)

      self.reset_btn = QPushButton(self.buttonframe)
      self.reset_btn.setText('Reset')
      self.cghlayout.addWidget(self.reset_btn)
      self.reset_btn.clicked.connect(self.reset)

      self.cghlayout.addStretch()

      self.cancel = QPushButton(self.buttonframe)
      self.cancel.setText('Cancel')
      self.cghlayout.addWidget(self.cancel)
      self.cancel.clicked.connect(self.reject)

      self.confirm_btn = QPushButton(self.buttonframe)
      self.confirm_btn.setText('Ok')
      self.cghlayout.addWidget(self.confirm_btn)
      self.confirm_btn.clicked.connect(self.confirm)

    def page_changed(self, current, prev):
      self.pages.setCurrentIndex(self.cfgbrowser.row(current))
      if current is self.color_page_item:
        self.config.state.active_config_section = 'render_effect'
      elif current is self.draw_style_page_item:
        self.config.state.active_config_section = 'draw_style'
      elif current is self.sbml_page_item:
        self.config.state.active_config_section = 'sbml_options'
      elif current is self.other_page_item:
        self.config.state.active_config_section = 'other_options'

    def reset(self):
      self.config.reset_defaults()

    def apply_(self):
      self.parent().confirmconfig(self.config)
      self.parent().update()

    def confirm(self):
      self.apply_()
      self.accept()

    def reject(self):
      self.parent().popconfig()
      self.parent().update()
      super(PrefDialog, self).reject()

def start():
  if not inspyder:
    app = QtWidgets.QApplication([])
  else:
    app = qapplication()

  layoutapp = Autolayout()

  if not inspyder:
    layoutapp.AppCls = QtWidgets.QApplication
  else:
    layoutapp.AppCls = qapplication

  layoutapp.show()
  if(defaultfile):
    layoutapp.openfile(defaultfile)
  sys.exit(get_exec(app)())

if __name__ == '__main__':
  start()

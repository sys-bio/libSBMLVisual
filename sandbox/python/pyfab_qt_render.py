#!/usr/bin/python
# This file should not be run directly

import pyfab_app

class PyQtRenderer(pyfab_app.PyfabRenderer):
  def drawNode(self, node, x0, y0, x1, y1, config, painter=None, horizonEnabled=True):
    if painter is None:
      painter = pyfab_app.QPainter(self.frame)
    painter.save()
    painter.setRenderHints(pyfab_app.QPainter.Antialiasing)
    painter.setRenderHints(pyfab_app.QPainter.HighQualityAntialiasing)
    painter.setRenderHints(pyfab_app.QPainter.SmoothPixmapTransform)

    painter.setTransform(self.frame.qtf)

    width, height = x1-x0, y1-y0
    cornerrad = config.state.node_corner_radius

    if config.state.node_effect == 'advanced':

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
      shadowoffset = pyfab_app.QPoint((3.,3.))
      painter.setPen(pyfab_app.QPen(pyfab_app.Qt.NoPen))
      painter.setBrush(pyfab_app.QColor(0,0,0,100))
      painter.drawRoundedRect(pyfab_app.QRectF(x0 + shadowoffset.x(), y0 + shadowoffset.y(), width,
          height), cornerrad, cornerrad)

      # main shape

      gradient = pyfab_app.QLinearGradient(pyfab_app.QPoint((x0+width/2,y0)), pyfab_app.QPoint((x0+width/2,y1)))
      gradient.setColorAt(0, pyfab_app.QColor(linearbright))
      gradient.setColorAt(1, pyfab_app.QColor(dark))

      brush = pyfab_app.QBrush(gradient)
      painter.setBrush(brush)

      painter.drawRoundedRect(pyfab_app.QRectF(x0, y0, width,
          height), cornerrad, cornerrad)

      # radial pass

      # outline
      painter.setBrush(pyfab_app.Qt.NoBrush)
      outlinepen = pyfab_app.QPen(pyfab_app.QBrush(pyfab_app.QColor(00,0,00,255)), 1., pyfab_app.Qt.SolidLine)
      painter.setPen(outlinepen)

      radialgradient = pyfab_app.QRadialGradient(pyfab_app.QPoint((x0+width/3,y0+height/2)), 30.*width/50, pyfab_app.QPoint((x0+width/4,y0+height/4)))
      radialgradient.setColorAt(0, self.frame.makeQColorAlpha(radialbright, 255))
      radialgradient.setColorAt(1, self.frame.makeQColorAlpha(dark, 25))
      radialbrush = pyfab_app.QBrush(radialgradient)
      painter.setBrush(radialbrush)
      painter.setCompositionMode(pyfab_app.QPainter.CompositionMode_Screen)

      painter.drawRoundedRect(pyfab_app.QRectF(x0, y0, width,
          height), cornerrad, cornerrad)

      if not horizonEnabled:
        painter.restore()
        return

      # horizon pass

      horizongradient = pyfab_app.QLinearGradient(pyfab_app.QPoint((x0+width/2,y0-100)), pyfab_app.QPoint((x0+width/2,y0+height/2)))
      horizongradient.setSpread(pyfab_app.QGradient.RepeatSpread)
      horizongradient.setColorAt(0, pyfab_app.QColor(horizonlight))
      horizongradient.setColorAt(1, pyfab_app.QColor(0xffffff))

      brush = pyfab_app.QBrush(horizongradient)
      painter.setBrush(brush)
      painter.setCompositionMode(pyfab_app.QPainter.CompositionMode_Multiply)

      painter.drawRoundedRect(pyfab_app.QRectF(x0, y0, width,
          height), cornerrad, cornerrad)

      if hasattr(node, 'custom'):
        if node.custom.isBeingDragged:
          painter.setBrush(pyfab_app.QColor(0,0,0,100))
          painter.drawRoundedRect(pyfab_app.QRectF(x0, y0, width,
              height), cornerrad, cornerrad)
      else:
        node.custom = pyfab_app.NodeData()

      painter.restore()

    else:
      node_color1 = pyfab_app.tuple2QColor(config.state.node_color1)
      node_color2 = pyfab_app.tuple2QColor(config.state.node_color2)

      gradient = pyfab_app.QLinearGradient(pyfab_app.QPoint((x0+width/2,y0)), pyfab_app.QPoint((x0+width/2,y1)))
      gradient.setColorAt(0,node_color1)
      gradient.setColorAt(1, node_color2)

      brush = pyfab_app.QBrush(gradient)
      painter.setBrush(brush)

      #painter.setBrush(pyfab_app.Qt.NoBrush)
      outlinepen = pyfab_app.QPen(pyfab_app.QBrush(pyfab_app.tuple2QColor(config.state.node_outline_color)), config.state.node_outline_width, pyfab_app.Qt.SolidLine)
      painter.setPen(outlinepen)

      painter.drawRoundedRect(pyfab_app.QRectF(x0, y0, width,
          height), cornerrad, cornerrad)

      # darkened

      if hasattr(node, 'custom'):
        if node.custom.isBeingDragged:
          painter.setBrush(pyfab_app.QColor(0,0,0,100))
          painter.drawRoundedRect(pyfab_app.QRectF(x0, y0, width,
              height), cornerrad, cornerrad)
      else:
        node.custom = pyfab_app.NodeData()

      painter.restore()

  def drawRxnCentroid(self, path, painter, point, config):
    if config.state.node_effect == 'advanced':
      cent_color = pyfab_app.QColor(75,0,150,100)
    else:
      cent_color = pyfab_app.tuple2QColor(config.state.centroid_color)
    brush = pyfab_app.QBrush(cent_color)
    if config.state.centroid_outline_enabled:
      outlinepen = pyfab_app.QPen(pyfab_app.tuple2QColor(config.state.centroid_outline_color), config.state.centroid_outline_width, pyfab_app.Qt.SolidLine)
      painter.setPen(outlinepen)
    else:
      painter.setPen(pyfab_app.Qt.NoPen)

    if config.state.centroid_enabled:
      painter.setBrush(brush)
      painter.drawEllipse(point.x-10, point.y-10, 20, 20)
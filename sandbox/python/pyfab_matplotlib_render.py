#!/usr/bin/python
# This file should not be run directly

import pyfab_app
import matplotlib
import matplotlib.pyplot as plt
from matplotlib2tikz import save as tikz_save
from matplotlib.patches import FancyBboxPatch, PathPatch
from matplotlib.path import Path

class PyPlotRenderer(pyfab_app.PyfabRenderer):
  def begin(self, width=600, height=600):
    plt.clf()
    #plt.plot([10,20,30,40], [10,40,90,160], 'ro')
    self.width = width
    self.height = height
    self.cmscale = 50.
    plt.axis([0, self.width, 0, self.height])
    #plt.gca().invert_yaxis() # doesn't work in matplotlib2tikz

  @property
  def widthcm(self):
    return self.width/self.cmscale

  @property
  def heightcm(self):
    return self.height/self.cmscale

  def toScreenSpace(self, x, y, tx=None):
    if tx is not None:
      x, y = tx(x, y)
    return (x, self.height - y)

  def drawNode(self, node, x0_, y0_, x1_, y1_, tx=None):
    x0, y0 = self.toScreenSpace(x0_, y0_, tx)
    x1, y1 = self.toScreenSpace(x1_, y1_, tx)
    patch = FancyBboxPatch((x0, y0-abs(y1-y0)), abs(x1-x0), abs(y1-y0), boxstyle='round,pad=0.1,rounding_size=5', fc=(1., .8, 1.), ec=(1., 0.5, 1.))
    plt.gca().add_patch(patch)

    tx = (x0+x1)*0.5
    ty = (y0+y1)*0.5
    #ty = y0+10
    plt.text(tx, ty, node.name, verticalalignment=u'center', horizontalalignment=u'center', fontsize=9)

  def drawCurve(self, curve, tx=None):
    verts = [self.toScreenSpace(curve[x].x, curve[x].y, tx) for x in range(4)]
    codes = [Path.MOVETO, Path.CURVE4, Path.CURVE4, Path.CURVE4]
    path = Path(verts, codes)

    ls = 'solid'
    if(curve[4] == 'MODIFIER'):
      ls = 'dashed'
    patch = PathPatch(path, facecolor='none', lw=3, ls=ls)
    plt.gca().add_patch(patch)

  def save(self, filename):
    #matplotlib.use('SVG')
    plt.savefig('/tmp/pyfab_fig.svg')
    #tikz_save('/tmp/pyfab_fig.tikz', figurewidth='{}cm'.format(self.widthcm), figureheight='{}cm'.format(self.heightcm))
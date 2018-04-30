#!/usr/bin/python
# This file should not be run directly

import sys

#import pyfab_app
from os.path import expanduser
from os.path import join

import sbnw

if sys.version_info[0] < 3:
  from sets import Set as set
import json
#from copy import deepcopy

def dict_iteritems(d):
  if sys.version_info[0] < 3:
    return d.iteritems()
  else:
    return iter(d.items())

def make_unicode(x):
  if sys.version_info[0] < 3:
    return unicode(x)
  else:
    return str(x)

def make_color_rgba(r, g, b, a):
  return (r, g, b, a)

def get_default_options():
  # arrowheads: 0 = none, 1 = reg rxn, 2 = inh, etc.
  return {
      # dialog
      'active_config_section': 'render_effect', # values: render_effect, draw_style, sbml_options
      # global
      'centroid_enabled': True,
      'compartment_enabled': True,
      # colors
      'node_effect': 'advanced', # values: advanced, basic
      'node_color1': make_color_rgba(0.8, 0.9, 0.95, 1.),
      'node_color2': make_color_rgba(0.1, 0.4, 0.9, 1.),
      'node_outline_color': make_color_rgba(0., 0., 0., 1.),
      'compartment_color': make_color_rgba(0.6, 0.3, 0.8, 0.3),
      'compartment_outline_color': make_color_rgba(0., 0., 0., 1.),
      'edge_color': make_color_rgba(0., 0., 0., 1.), # deprecated
      'substrate_edge_color': make_color_rgba(0., 0., 0., 1.),
      'product_edge_color': make_color_rgba(0., 0., 0., 1.),
      'activator_edge_color': make_color_rgba(0.2, 0.2, 0.4, 1.),
      'inhibitor_edge_color': make_color_rgba(1., 0.5, 0.5, 1.),
      'modifier_edge_color': make_color_rgba(0.2, 0.2, 0.4, 1.),
      'text_color': make_color_rgba(0., 0., 0., 1.),
      'centroid_color': make_color_rgba(0.5, 0.9, 0.5, 0.5),
      'centroid_outline_color': make_color_rgba(0., 0., 0., 1.),
      # outlines
      'edge_width': 1., # deprecated
      'substrate_edge_width': 1.,
      'product_edge_width': 1.,
      'activator_edge_width': 1.,
      'inhibitor_edge_width': 1.,
      'modifier_edge_width': 1.,

      'node_outline_width': 1.,
      'compartment_outline_width': 1.,
      'compartment_outline_enabled': True,
      'centroid_outline_width': 1.,
      'centroid_outline_enabled': True,

      'substrate_edge_dashed': False,
      'product_edge_dashed':   False,
      'activator_edge_dashed': True,
      'inhibitor_edge_dashed': True,
      'modifier_edge_dashed':  True,
      # effects
      'node_corner_radius': 4.,
      # text
      'text_halo_enabled': True,
      # endcaps
      'substrate_arrowhead': 0,
      'product_arrowhead': 1,
      'activator_arrowhead': 4,
      'inhibitor_arrowhead': 5,
      'modifier_arrowhead': 7,
      # interaction
      'auto_recenter_junctions': False,
      'stiffness': 50.,
      # sbml
      'override_sbml_ns': False,
      'sbml_level': 3, # only if override_sbml_ns is True
      'sbml_version': 1 # only if override_sbml_ns is True
      }

def translateConfigArrowOptToRole(role):
  if role == 'substrate_arrowhead':
    return 'SUBSTRATE'
  elif role == 'product_arrowhead':
    return 'PRODUCT'
  elif role == 'activator_arrowhead':
    return 'ACTIVATOR'
  elif role == 'inhibitor_arrowhead':
    return 'INHIBITOR'
  elif role == 'modifier_arrowhead':
    return 'MODIFIER'
  else:
    raise RuntimeError('Unknown role')

class PyfabConfigState(object):
  def __init__(self, parent):
    self.table = {}
    self.parent = parent

  def __getattr__(self, name):
    #if name == u'table':
      #print(self)
      #print(self.__dict__)
      #print(dir(super(PyfabConfigState, self)))
      #return super(PyfabConfigState, self).__getattr__(name)
    assert(name != 'table')
    #assert(u'table' not in self.table)
    #print('getattr on config state: {}'.format(name))
    #print('  config state is: {}'.format(self.table))
    if not name in self.table:
      return get_default_options()[name]
    return self.table[make_unicode(name)]

  def __setattr__(self, name, value):
    if name == u'table' or name == u'parent':
      return super(PyfabConfigState, self).__setattr__(name, value)
    assert(u'table' not in self.table)
    #print('setattr on config state: {}'.format(name))
    #print('  config state is: {}'.format(self.table))
    self.table[name] = value
    assert(u'table' not in self.table)
    #print('PyfabConfigState __setattr__ {}'.format(name))
    self.parent.alert_cfg_changed()

  def get_table(self):
    return self.table

  def copy(self, parent):
    r = PyfabConfigState(parent)
    r.table = dict(self.get_table())
    return r


class PyfabConfig(object):
  def __init__(self, parent=None):
    self.parent = parent
    self.listeners = set()

    if not hasattr(self, 'ConfigStateCls'):
      self.ConfigStateCls = PyfabConfigState

    if self.parent:
      self.state = self.parent.state.copy(self)
      #print('inherited state: {}'.format(self.state.table))
    else:
      self.state = self.ConfigStateCls(self)

  def reset_defaults(self):
    self.set_options(get_default_options())
    self.alert_cfg_changed()

  def set_options(self, opts):
    """
    opts:dictionary of options and resp values
    """
    self.state.table.update(opts)
    self.updateArrowStyles()

  def serialize(self):
    #print('serialize: state = {}'.format(self.state.table))
    return json.dumps(self.state.table)

  def deserialize(self, s):
    self.set_options(json.loads(s))
    #print('deserialzed state: {}'.format(self.state.table))

  def propagate(self):
    """
    Propagate the changes to the parent
    """
    if self.parent is not None:
      self.parent.set_options(self.state.get_table())
      self.parent.alert_cfg_changed()

  def finalize(self):
    """
    Called whenever the config changes should be finalized / written to disk
    """
    #print('write cfg file')
    self.propagate()

  def install_listener(self, listener):
    """
    Add a listener to be called whenever the config updates
    """
    self.listeners.add(listener)

  def remove_listener(self):
    self.listeners.remove(listener)

  def alert_cfg_changed(self):
    #print('alert_cfg_changed')
    for listener in self.listeners:
      listener()
    self.updateArrowStyles()

  def __enter__(self):
    return self

  def __exit__(self, exception_type, exception_value, traceback):
    self.finalize()

  def updateArrowStyles(self):
    """
    Called to handle setting arrow styles
    """
    for k,v in dict_iteritems(self.state.get_table()):
      try:
        role = translateConfigArrowOptToRole(k)
        if sbnw.get_arrow_style(role) != v:
          sbnw.set_arrow_style(role, v)
      except RuntimeError:
        pass

class PyfabConfigStandalone(PyfabConfig):
  def __init__(self, parent=None):
    PyfabConfig.__init__(self, parent)
    #if parent is not None:
      #print('new PyfabConfigStandalone from {}'.format(parent))
      ##print('  parent.state {}'.format(parent.state.__dict__))
      #print('  * parent.state.node_effect: {}'.format(parent.state.node_effect))
      #print('  state: {}'.format(self.state.get_table()))
    if parent is None:
      self.cfgfilepath = join(expanduser('~'), '.pyfab')
      print(self.cfgfilepath)
      try:
        #print('reading config file {}'.format(self.cfgfilepath))
        self.read(self.cfgfilepath)
        print('read values from config file {}'.format(self.cfgfilepath))
        self.updateArrowStyles()
      except IOError:
        print('config file {} does not exist - using defaults'.format(self.cfgfilepath))
        self.reset_defaults() # file does not exist - use defaults

  def write(self, f):
    #print('writing config to {}'.format(f))
    try:
      with open(f, 'w') as f:
        f.write(self.serialize())
    except IOError:
      print('Unable to write to file {}'.format(f))

  def read(self, f):
    with open(f, 'r') as f:
      self.deserialize(f.read())

  def finalize(self):
    super(PyfabConfigStandalone, self).finalize()
    if self.parent is None:
      self.write(self.cfgfilepath)
    else:
      self.parent.finalize()
#import imp
import sys
import os

localpath = os.path.dirname(os.path.realpath(__file__))

if sys.version_info[0] < 3:
  import __builtin__
  __builtin__.p_pyfab_spyder_env = True
else:
  import builtin
  builtin.p_pyfab_spyder_env = True

#pyfab_app = imp.load_source('pyfab_app', '/home/poltergeist/home/devel/install/graphfab-trunk/python2/site-packages/pyfab_app.py')
#print('pyfab_app loaded source')

import pyfab_app
from pyfab_cfg import PyfabConfigState, PyfabConfigStandalone, PyfabConfig, get_default_options

import spyderlib
from spyderlib.qt.QtGui import (QWidget, QVBoxLayout)
from spyderlib.plugins import SpyderPluginMixin
from spyderlib.baseconfig import get_translation
from spyderlib.utils.qthelpers import get_icon, create_action, qapplication
from spyderlib.qt.QtCore import SIGNAL, Qt
from spyderlib.py3compat import configparser as cp

from spyderlib.plugins.externalconsole import ExternalConsole

_ = get_translation("p_pyfab", dirname="spyderplugins")

class PyfabSpyderConfigState(PyfabConfigState):
  def __init__(self, parent):
    super(PyfabSpyderConfigState, self).__init__(parent)
    self.parent = None
    #print('********** PyfabSpyderConfigState set self.parent')

  def __getattr__(self, name):
    assert(name != 'table')
    assert(name != 'parent')
      #return super(PyfabSpyderConfigState, self).__getattr__(name)
    assert(u'table' not in self.table)
    #print('PyfabSpyderConfigState getattr on config state: {}'.format(name))
    #print('  PyfabSpyderConfigState config state is: {}'.format(self.table))

    #return self.table[unicode(name)]
    try:
      return self.parent.get_option(name)
    except (cp.NoOptionError, cp.NoSectionError):
      return get_default_options()[name]

  def __setattr__(self, name, value):
    if name == u'parent':
      return object.__setattr__(self, name, value)
    if name == u'table' or name == u'parent':
      return super(PyfabSpyderConfigState, self).__setattr__(name, value)
    #assert(u'table' not in self.table)
    #print('PyfabSpyderConfigState setattr on config state: {}'.format(name))
    #print('  PyfabSpyderConfigState config state is: {}'.format(self.table))

    #self.table[name] = value
    self.parent.set_option(name, value)

    #assert(u'table' not in self.table)

  def get_table(self):
    r = {}
    for k,v in get_default_options().iteritems():
      r[k] = getattr(self, k)
    return r

  def copy(self, parent):
    return super(PyfabSpyderConfigState, self).copy(parent)

class PyfabConfigSpyder(PyfabConfig):
  def __init__(self, parent=None):

    # config state
    self.ConfigStateCls = PyfabSpyderConfigState
    PyfabConfig.__init__(self, parent)
    self.state.parent = self

    if parent is None:
      #print('PyfabConfigSpyder first init')
      self.updateArrowStyles()

  def reset_defaults(self):
    super(PyfabConfigSpyder, self).reset_defaults()
    #if self.parent is None:
      #self.set_option('cust_option', 123)
      #print('get_option cust_option: {}'.format(self.get_option('cust_option')))

  def set_options(self, opts):
    #super(PyfabConfigSpyder, self).set_options(opts)
    for o, v in opts.iteritems():
      #print('  set option {} {}'.format(o,v))
      self.set_option(o, v)
    self.updateArrowStyles()

  def finalize(self):
    super(PyfabConfigSpyder, self).finalize()
    if self.parent is None:
      pass
      #print('PyfabConfigSpyder finalize last')
      #self.set_options(self.state.get_table())
    else:
      self.parent.finalize()

class PyfabPlugin(pyfab_app.Autolayout, SpyderPluginMixin, PyfabConfigSpyder):
  CONF_SECTION = 'pyfab_plugin'
  #LOCATION = Qt.RightDockWidgetArea #useless
  #ALLOWED_AREAS = Qt::RightDockWidgetArea #useless
  def __init__(self, parent=None):

    self.configCls = PyfabConfigStandalone
    PyfabConfigSpyder.plugin = self
    self.config = self
    PyfabConfigSpyder.__init__(self)

    pyfab_app.Autolayout.__init__(self)
    self.AppCls = qapplication().__class__
    SpyderPluginMixin.__init__(self, parent)

    # Initialize plugin
    self.initialize_plugin()

    self.raise_() #useless

  def getIconPath(self, p):
    result = str(os.path.join(localpath, 'pyfab-assets', 'icons', p))
    #print('getIconPath: {} -> {}'.format(p, result))
    return result

  def get_plugin_title(self):
    return 'Network viewer'

  def get_plugin_icon(self):
    return None

  def get_focus_widget(self):
    return None

  def get_plugin_actions(self):
    return None

  def on_first_registration(self):
    pass

  def register_plugin(self):
    self.main.add_dockwidget(self)

    print('** Looking for ExternalPythonShell')
    #from spyderlib.widgets.externalshell import pythonshell
    #print(len(self.main.extconsole.shellwidgets))
    #for sw in self.main.extconsole.shellwidgets:
      #print('sw {}'.format(sw))
      #if isinstance(sw, pythonshell.ExternalPythonShell):
        #print('    Found ExternalPythonShell: {}'.format(sw))
    print(self.main.extconsole.start)
    self.main.extconsole.__class__ = SBNWExternalConsole

    if self.main.explorer is not None:
      self.connect(self.main.explorer, SIGNAL("open_interpreter(QString)"),
                   self.sigslot)

    if self.main.projectexplorer is not None:
      self.connect(self.main.projectexplorer, SIGNAL("open_interpreter(QString)"),
                   self.sigslot)

    if self.main.extconsole is not None:
      self.connect(self.main.extconsole, SIGNAL("open_interpreter(QString)"),
                   self.sigslot)

    #print(dir(self.main.extconsole))

    self.connect(self.main.console.shell, SIGNAL('refresh()'),
                 self.sigslot)
    self.connect(self.main, SIGNAL('open_external_file(QString)'),
                 self.sigslot)
    #self.open_interpreter()
    #self.connect(self, SIGNAL('doit()'),
                 #self.doit)
    #self.emit(SIGNAL('doit()'))

    #k_act = create_action(self, _("Network viewer"),
                                #triggered=self.launch)
    #k_act.setEnabled(True)
    #self.register_shortcut(k_act, context="network",
                            #name="launch network viewer", default="F7")

    #self.main.tools_menu_actions += [None, k_act]
    #self.main.editor.pythonfile_dependent_actions += [k_act]

  def refresh_plugin(self):
    pass

  def closing_plugin(self, cancelable=False):
    return True

  def apply_plugin_settings(self, options):
    pass

  def launch(self):
    self.show()

  def open_interpreter(self, arg=None):
    print('open_interpreter')

  def sigslot(self, *args, **kwargs):
    print('slot')

  #def doit(self, *args, **kwargs):
    #print('doit')

  def getobj(self):
      print('getobj derived')

  def opensbml(self, sbml):
    if self.dockwidget is not None:
      self.dockwidget.blockSignals(True)

    # raise window
    self.eventually_raise_network_viewer(False)

    if self.dockwidget is not None:
      self.dockwidget.blockSignals(False)

    # open sbml in the raised window
    pyfab_app.Autolayout.opensbml(self, sbml)

  # Based on Spyder's __eventually_raise_inspector in inspector.py
  def eventually_raise_network_viewer(self, force=False):
      if hasattr(self.main, 'tabifiedDockWidgets'):
          # 'QMainWindow.tabifiedDockWidgets' was introduced in PyQt 4.5
          if self.dockwidget and (force or self.dockwidget.isVisible()) \
              and not self.ismaximized:
              dockwidgets = self.main.tabifiedDockWidgets(self.dockwidget)
              if self.main.console.dockwidget not in dockwidgets and \
                  (hasattr(self.main, 'extconsole') and \
                  self.main.extconsole.dockwidget not in dockwidgets):
                  self.dockwidget.show()
                  self.dockwidget.raise_()


PLUGIN_CLASS = PyfabPlugin

from spyderlib.qt.QtCore import Signal as SpyderSignal

class SBNWExternalConsole(ExternalConsole):
  def start(self, *args, **kwds):
    from spyderlib.widgets.externalshell import pythonshell
    print('SBNWExternalConsole start')
    ExternalConsole.start(self, *args, **kwds)

    for sw in self.shellwidgets:
      #print('sw {}'.format(sw))
      if isinstance(sw, pythonshell.ExternalPythonShell):
        #print('    Found ExternalPythonShell: {}'.format(sw))
        #print('      NotificationThread: {}'.format(sw.notification_thread))
        #sw.notification_thread.__class__ = SBNWNotificationThread
        #print('      terminate NotificationThread')

        # stop the thread
        #sw.notification_thread.terminate()

        ## add the layout signal if it doesn't already exist
        #if not hasattr(sw.notification_thread, 'layout'):
          #sw.notification_thread.layout = SpyderSignal(str)

        ## connect the layout signal to the handlers
        #print(SpyderSignal)
        #print(type(sw.notification_thread.layout))
        #print(dir(sw.notification_thread.layout))
        sw.notification_thread.layout.connect(self.network_layout)
        sw.notification_thread.network_viewer_sbml_hook = self.get_network_viewer_sbml

        # start the thread again
        #sw.notification_thread.start()

  def network_layout(self, sbml):
    """Open the SBML model in the pyfab layout viewer"""
    print('network_layout, sbml {}'.format(sbml))
    if sbml != '~::empty::~':
      # find pyfab
      pyfab = None
      for plugin in self.main.thirdparty_plugins:
          if(hasattr(plugin, 'pyfabMarker')):
              pyfab = plugin
              break

      if pyfab is not None:
          pyfab.opensbml(sbml)
    else:
      raise RuntimeError('No SBML string')

  def get_network_viewer_sbml(self):
    print('get_network_viewer_sbml')
    # find pyfab
    pyfab = None
    for plugin in self.main.thirdparty_plugins:
        if(hasattr(plugin, 'pyfabMarker')):
            pyfab = plugin
            break

    if pyfab is not None:
        return pyfab.getsbml()

from spyderlib.widgets.externalshell.introspection import NotificationThread
from spyderlib.baseconfig import get_conf_path, DEBUG
from spyderlib.utils.bsdsocket import read_packet, write_packet
from spyderlib.utils.debug import log_last_error

LOG_FILENAME = get_conf_path('introspection.log')

class SBNWNotificationThread(NotificationThread):
    layout = SpyderSignal(str)

    def run(self):
        #print('run')
        """Start notification thread"""
        while True:
            #print('SBNWNotificationThread while True')

            # add layout signal if it doesn't already exist
            #if not hasattr(self, 'layout'):

            if self.notify_socket is None:
                continue
            output = None
            try:
                try:
                    cdict = read_packet(self.notify_socket)
                except:
                    # This except statement is intended to handle a struct.error
                    # (but when writing 'except struct.error', it doesn't work)
                    # Note: struct.error is raised when the communication has
                    # been interrupted and the received data is not a string
                    # of length 8 as required by struct.unpack (see read_packet)
                    break
                if cdict is None:
                    # Another notification thread has just terminated and
                    # then wrote 'None' in the notification socket
                    # (see the 'finally' statement below)
                    continue
                if not isinstance(cdict, dict):
                    raise TypeError("Invalid data type: %r" % cdict)
                command = cdict['command']
                data = cdict.get('data')
                if command == 'pdb_step':
                    fname, lineno = data
                    self.sig_pdb.emit(fname, lineno)
                    self.refresh_namespace_browser.emit()
                elif command == 'refresh':
                    self.refresh_namespace_browser.emit()
                elif command == 'remote_view':
                    self.sig_process_remote_view.emit(data)
                elif command == 'ipykernel':
                    self.new_ipython_kernel.emit(data)
                elif command == 'open_file':
                    fname, lineno = data
                    self.open_file.emit(fname, lineno)
                elif command == 'layout':
                    print('SBNWNotificationThread layout')
                    sbml = data
                    if sbml != '~::empty::~':
                      print('SBNWNotificationThread emit layout signal')
                      self.layout.emit(sbml)
                    else:
                      print('SBNWNotificationThread get sbml')
                      if hasattr(self, 'network_viewer_sbml_hook'):
                        output = self.network_viewer_sbml_hook()
                        print('output = {}'.format(output))
                      else:
                        print('no attr network_viewer_sbml_hook')
                else:
                    raise RuntimeError('Unsupported command: %r' % command)
                if DEBUG_INTROSPECTION:
                    logging.debug("received command: %r" % command)
            except:
                log_last_error(LOG_FILENAME, "notification thread")
            finally:
                try:
                    write_packet(self.notify_socket, output)
                except:
                    # The only reason why it should fail is that Spyder is
                    # closing while this thread is still alive
                    break

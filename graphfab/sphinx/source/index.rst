.. SBNW: a layout engine for SBML

libSBNW Documentation
====================================

Contents:

Example 1: Basic Usage

   Import the module

   >>> import sbnw

   Load an SBML model

   >>> model = sbnw.loadsbml('BorisEJB.xml') # included in the test cases

   Shrink or enlarge a network so that it fits within a window

   >>> model.network.fitwindow(0,0,300,300)

   Access the node coordinates

   >>> for node in model.network.nodes:
   >>>   print(node.centroid)

Example 2: Add layout information to an SBML model

   Import the module

   >>> import sbnw

   Load an SBML model

   >>> model = sbnw.loadsbml('ant_power_law_sbml.xml') # included in the test cases

   Randomize the initial configuration and run the FR algorithm

   >>> model.network.randomize(0,0,300,300)
   >>> model.network.autolayout()

   Shrink or enlarge a network so that it fits within a window

   >>> model.network.fitwindow(0,0,300,300)

   Override SBML level/version if desired

   >>> model.level = 3
   >>> model.version = 1

   Save the model to an SBML file

   >>> model.save('output.xml')

Example 3: Render network as a TikZ

   Import the module

   >>> import sbnw

   Load an SBML model

   >>> model = sbnw.loadsbml('GlycolysisOriginal.xml') # included in the test cases

   Render the model to a TikZ file

   >>> model.savetikz('GlycolysisOriginal_Render.tikz')

Example 4: Curves

   Import the module

   >>> import sbnw

   Load an SBML model

   >>> model = sbnw.loadsbml('BorisEJB.xml') # included in the test cases

   Access the curve control points:

   >>> for reaction in model.network.rxns:
   >>>   for curve in reaction.curves:
   >>>   print('Curve: Start {}, End {}, CP1 {}, CP2 {}'.format(curve[0], curve[3], curve[1], curve[2]))

   Access endcap polygon:

   >>>   if len(curve) > 5 and len(curve[5]) > 0:
   >>>     print('Endcap poly: {}'.format(curve[5]))

   Set endcap (a.k.a. arrowhead styles):

   >>> sbnw.get_arrow_style('SUBSTRATE') # Other values are PRODUCT, MODIFIER, ACTIVATOR, INHIBITOR
   >>> sbnw.set_arrow_style('SUBSTRATE', 3)

Example 5: Version

    Get the version information

    >>> import sbnw
    >>> print(sbnw.__version__)

API Documentation

.. currentmodule:: sbnw

.. autosummary::
   :toctree: generated/

.. rubric:: Functions
.. autosummary::
   :toctree: generated/

   loadsbml
   arrowpoly
   arrowpoly_filled
   narrow_styles
   get_arrow_style
   set_arrow_style

.. rubric:: Classes
.. autosummary::
   :toctree: generated/

   sbmlmodel
   point
   network
   node
   compartment
   reaction
   transform

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`


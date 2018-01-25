cmake_minimum_required (VERSION 2.8)
project (Graphfab)

# Examples from HMS etc.
install(FILES AntimonyPowerLaw.xml BorisEJB.xml ConservedCycles.xml
  EquilibRaw.xml GlycolysisOriginal.xml testbigmodel.xml DESTINATION testcases)

# Official examples are from http://otto.bioquant.uni-heidelberg.de/sbml/#examples
install(FILES official/example1.xml official/example2.xml official/example3.xml
  DESTINATION testcases)

# Used internally / expected failures
# install(FILES internal/bounding_box.xml
#   internal/EquilibRaw_wlayout-DrawNetwork-vs2010.xml internal/example2.xml
#   internal/twocompsys-ex.xml DESTINATION testcases/internal)
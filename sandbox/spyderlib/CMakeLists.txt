cmake_minimum_required (VERSION 2.8)
project (SagittariusSandbox)

# Based on Spyder 2.3.4

if(SBNW_SPYDER_DIST)
  install(FILES widgets/externalshell/sitecustomize.py widgets/externalshell/introspection.py DESTINATION python2/spyderlib/widgets/externalshell)
endif()

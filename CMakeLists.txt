cmake_minimum_required (VERSION 2.8)
project (SBNW)

set(SBNW_MAJOR_VER  1)
set(SBNW_MINOR_VER  3)
set(SBNW_PATCHLEVEL 3)

#User Configuration
if(${WIN32})
    include( "UserConfigWin.txt" )
else()
    include( "UserConfigLinux.txt" )
endif()

set(SBNW_SPYDER_DIST ON CACHE BOOL "Install Spyder plugin files")

set(BUILD_STATIC_LIB ON CACHE BOOL "Build the static version of the library")

# GTest
find_package(GTest)
if(GTEST_FOUND)
  set(WITH_GTEST ON CACHE BOOL "Use gtest framework")
else()
  set(WITH_GTEST OFF CACHE BOOL "Use gtest framework")
endif()

# Python 3
find_package(PythonLibs 3.2)
if(PYTHONLIBS_FOUND)
  set(ENABLE_PYTHON3_BINDINGS ON CACHE BOOL "Enable Python 3 bindings")
  set(PYTHON3_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}" CACHE PATHS "Python3 includes")
  set(PYTHON3_LIBRARY "${PYTHON_LIBRARY}" CACHE STRING "Python3 lib")
  unset(PYTHON_INCLUDE_DIR CACHE)
  unset(PYTHON_LIBRARY CACHE)
  unset(PYTHON_LIBRARY_DEBUG CACHE)
  unset(PYTHONLIBS_FOUND)
else()
  set(ENABLE_PYTHON3_BINDINGS OFF CACHE BOOL "Enable Python 3 bindings")
endif()

# Python 2
# find_package(PythonLibs 2.7 EXACT) # Maybe some day, CMake...
if(PYTHONLIBS_FOUND)
  set(ENABLE_PYTHON2_BINDINGS ON CACHE BOOL "Enable Python 2 bindings")
  # You need to set these manually
  set(PYTHON2_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}" CACHE PATHS "Python2 includes")
  set(PYTHON2_LIBRARY "${PYTHON_LIBRARY}" CACHE STRING "Python2 lib")
  unset(PYTHON_INCLUDE_DIR CACHE)
  unset(PYTHON_LIBRARY CACHE)
  unset(PYTHON_LIBRARY_DEBUG CACHE)
else()
  set(ENABLE_PYTHON2_BINDINGS OFF CACHE BOOL "Enable Python 2 bindings")
  set(PYTHON2_LIBRARY "${PYTHON_LIBRARY}" CACHE STRING "NOTFOUND")
endif()

#Dependencies
#SBML
if(LINK_WITH_LIBSBML)
    include_directories(${LIBSBML_INCLUDE})
endif()
#Image Magick
if(LINK_WITH_MAGICK)
    set(SBNW_USE_MAGICK 1)
    include_directories(${MAGICK_INCLUDE})
else()
    set(SBNW_USE_MAGICK 0)
endif()

if(ENABLE_FULL_RPATH) #http://www.cmake.org/Wiki/CMake_RPATH_handling
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
    
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    
    LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
    IF("${isSystemDir}" STREQUAL "-1")
        SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    ENDIF("${isSystemDir}" STREQUAL "-1")
endif()

#add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}) #we have to do this to get the generated header

add_subdirectory(graphfab)

add_subdirectory(sandbox)

add_subdirectory(icons)

add_subdirectory(testcases)

if(WITH_GTEST)
  add_subdirectory(test)
endif()

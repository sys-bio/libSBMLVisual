cmake_minimum_required (VERSION 2.8)
project (SBNW)

set(SBNW_MAJOR_VER  1)
set(SBNW_MINOR_VER  3)
set(SBNW_PATCHLEVEL 26)

set(SBNW_VER_STRING "${SBNW_MAJOR_VER}.${SBNW_MINOR_VER}.${SBNW_PATCHLEVEL}")

set(LINK_WITH_LIBSBML ON)
set(SBNW_LINK_TO_STATIC_LIBSBML ON)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(FPIC_FLAG "-fPIC")
else()
  set(FPIC_FLAG)
endif()

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

find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
  set(Python_ADDITIONAL_VERSIONS ${PYTHON_VERSION_STRING})
else()
  message(ERROR FATAL "Unable to find Python")
endif()

# Find Python libs
find_package(PythonLibs)
message(STATUS "Using Python include directory ${PYTHON_INCLUDE_DIR}")

#Dependencies

#SBML
find_library(LIBSBML_LIBRARY sbml libsbml HINTS ${LIBSBML_PREFIX}/lib)
find_library(LIBSBML_STATIC_LIBRARY sbml-static REQUIRED libsbml-static HINTS ${LIBSBML_PREFIX}/lib)
find_path(LIBSBML_INCLUDE_DIR sbml/SBMLTypes.h HINTS ${LIBSBML_PREFIX}/include)
include_directories(${LIBSBML_INCLUDE_DIR})

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

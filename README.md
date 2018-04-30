# libSBNW: A Network Viewer for SBML
[![GitHub version](https://badge.fury.io/gh/0u812%2Fsbnw.svg)](http://badge.fury.io/gh/0u812%2Fsbnw)

## HAS NOW MOVED TO https://github.com/sys-bio/sbnw

## Introduction
libSBNW is a network viewer which supports autolayout of reaction networks and uses libSBML for reading/writing models. The intent of this project is to provide user and developers of systems/synthetic biology software tools a convenient, portable library to enable automatic layout of SBML models and to record this information in the SBML layout extension. To facilitate this, the functionality of the library is provided as a custom plugin within the Spyder-based [Tellurium](http://tellurium.analogmachine.org/) modeling environment. For developers, pre-built binaries including bindings for Python 2 are provided via the [release](https://github.com/0u812/sbnw/releases) page on this site.

## Use Cases
The library is designed as a reusable implementation of a automatic layout for SBML. As such, it has many downstream uses. To name a few:

* Called by Python code via the included Python bindings
* Via the network viewer GUI plugin for Tellurium
* Directly via the C API in a pre-existing project
* As part of e.g. a Django web service

## Download

The network viewer plugin is part of Tellurium, which can be downloaded [here](http://sourceforge.net/projects/pytellurium/files/Tellurium-1.2/1.2.1/) for Windows and Mac OS X. Separate binaries for the libSBNW library are available from [SourceForge](https://sourceforge.net/projects/sbnw/files/).

## Tellurium Video Tutorials

Video tutorials are available [here](http://0u812.github.io/sbnw/tutorials) which demonstrate how to use the network viewer plugin in Tellurium.

## Test Cases

The SBML [test models](https://github.com/0u812/sbnw/releases/download/1.3.2/testcases.zip) used in development of this tool are available for download from the releases page as well. The network viewer is frequently tested with these files.

## Online Documentation

Online C API documentation (via Doxygen) can be found at http://0u812.github.io/sbnw/docs. Python documentation can be found at http://0u812.github.io/sbnw/sphinx.

## Using the Python bindings

* On Mac, extract the contents of http://sourceforge.net/projects/sbnw/files/1.2.7/sbnw-1.2.7-os-x-site-packages-py2.tar.bz2/download to your site-packages directory.
* On Windows, extract the contents of http://sourceforge.net/projects/sbnw/files/1.2.7/sbnw-1.2.7-site-packages-win32.zip/download to your site-packages directory.

## How to compile the library

 * Install the latest version of <a href="http://sourceforge.net/projects/sbml/files/libsbml/">libSBML</a> or build it from source (tested with 5.6, 5.8, 5.10, 5.11).
 * **NOTE**: If you install a pre-built binary of libSBML then you must compile SBNW with the same version of Visual Studio as used to build libSBML (2010 at the time of writing).
 * Clone the latest revision of the <a href="https://github.com/0u812/sbnw">master branch</a> to Documents\sbnw via git. In the Documents folder, run `git clone https://github.com/0u812/sbnw.git`
 * Download and instsall <a href="http://www.cmake.org/">CMake</a> (compatible with major version 2 or 3).
 * Open CMake and select Documents\sbnw as the source directory.
 * Select Documents\sbnw-build (or choose a directory at will) as the build directory.
 * Click configure & generate via CMake, choosing a generator that matches the required configuration (32-bit x86 is recommended on Windows; on Linux the default generator is sufficient).
 * Using CMake, set the `LIBSBML_DIR` variable to point to the directory where libSBML is installed/downloaded. Click configure & generate again.
 * **NOTE**: In order to statically link to libSBML, specify `SBNW_LINK_TO_STATIC_LIBSBML=ON`. Otherwise, the libSBML DLLs must be in the PATH to run any compiled code.
 * On Windows, open the generated .sln in Visual Studio, and change the configuration to "Release"; on Linux/Mac simply run make -j4 install from the build directory.
 * *(This step was previously used to instruct the user to set the MSVC runtime library. It is now set automatically through CMake. This placeholder serves as a reminder in case this solution breaks at some point.)*
 * **Windows Specific:** In Visual Studio, right click on the INSTALL target and select build. SBNW will be installed to the location stored in CMAKE_INSTALL_PREFIX (ensure your user has write access).

## Using the Tellurium Plugin

The network viewer is available as a plugin for a pre-release of [Tellurium](https://github.com/0u812/sbnw/releases/tag/1.2.5). Simply switch to the network viewer tab, open a model from the [test cases](https://github.com/0u812/sbnw/releases/download/1.2.4/testcases.zip) (e.g. BorisEJB.xml), and enter the following code:

```
# The nwed module is used to communicate with the plugin
import nwed
# Get the current model
sbmlstr = nwed.getsbml()
# Print the SBML
print(sbmlstr)
# Load the SBML back into the network viewer
nwed.setsbml(sbmlstr)
```

![Usage example](http://0u812.github.io/sbnw/hosted/img/usage_ex.png)

## Using the Library (C API)

The C API is exposed via graphfab/autolayoutc_api.h.  There is an example in sandbox/basic that demonstrates all the functionality needed to get the layout working:

```
        // type to store layout info
        gf_layoutInfo* l;

        // load the model
        gf_SBMLModel* mod = gf_loadSBMLbuf(buf);

        // options for layout algo
        fr_options opt;

        // read layout info from the model
        l = gf_processLayout(mod);

        // randomize node positions
        gf_randomizeLayout(l);

        // do layout algo
        opt.k = 20.;
        opt.boundary = 1;
        opt.mag = 0;
        opt.grav = 0.;
        opt.baryx = opt.baryy = 500.;
        opt.autobary = 1;
        gf_doLayoutAlgorithm(opt, l);

        // save layout information to new SBML file
        gf_writeSBMLwithLayout(outfile, mod, l);

        // run destructors on the model
        gf_freeSBMLModel(mod);

        // run destructors on the layout
        gf_freeLayoutInfo(l);
```

## License

This project is licensed under the BSD 3-clause license:
### Copyright (c) 2012-2015, J. Kyle Medley and Herbert M. Sauro
### All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* Neither the name of The University of Washington nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


## Acknowledgements

This work was funded by the generous support of the National Institute of General Medical Sciences of the National Institutes of Health under award R01-GM081070

Impact
======

The Interface for Multiscale Physics and ChemisTry intends to provide
a common friendly interface to interact with multiple C++ libraries.
The interface can be either used directly from C++, or from 
a Python-based integrated development environment ([see here](http://impact-one-for-all.googlecode.com/svn/website/index.html) ).

Currently Impact includes:

Atomism : A library for statistical analytic mechanics computations of molecular properties (thermo, reaction rate) from potential energy surfaces.

[Antioch](https://github.com/pbauman/antioch) :  High performance, thread safe, GPU accelerated, 
library for chemical kinetics.

[ Queso ](https://github.com/libqueso/queso) :   A library aimed for the solution of statistical inverse problems,
 and the prediction of quantities of interest along with the quantification of their uncertainties. 
It can run over uniprocessor or multiprocessor environments


Dependencies
============

Requirements
------------

The base version of Impact comes with Atomism, you need in this case:

1. [boost] (http://www.boost.org/), you should compile it with C++11 if you plan to work with Antioch.
To select a custom build directory to link with impact, set the BOOST_ROOT option to cmake (e.g. 
'cmake -DBOOST_ROOT=my/path CMakeLists.txt' ). 
2. [python2.7] (http://www.python.org/download/releases/2.7/). I did not try with other version.
3. [pyside] (http://qt-project.org/wiki/PySide). This library is required to launch the IDE.

To include [Antioch](https://github.com/pbauman/antioch) or [ Queso ](https://github.com/libqueso/queso)
packages, you need the associated libraries.

Optional Packages - C++
-----------------

1. [VTK](http://www.vtk.org/) - For volume rendering, a recent version (6.) is needed

Optional Packages - python
-----------------

1. [Matplotlib](http://matplotlib.org/) is used anytime a graphic is drawn, standart python packages
are needed.


Building Impact
================

Impact uses cmake build system

1. cmake CMakeLists.txt. Warning: The first time you run this command, do it twice (I don't know why...).
(for more options about build options do 'cmake -L')
2. make (note parallel builds are supported)
3. make install

Running the IDE
================

1. export PYTHONPATH=$PathOfInstallDir/python 
2. from the bin directory: python impact_gui.py


Adding Antioch
==============

Get and compile [Antioch](https://github.com/pbauman/antioch) code. You can set the
option ANTIOCH_INCLUDE_DIR to a custom path if cmake can not find Antioch by itself.

cmake -DANTIOCH=1 CMakeLists.txt ; make; make install.


Adding Queso
==============

Get and compile [ Queso ](https://github.com/libqueso/queso) code. You can set the
option ANTIOCH_INCLUDE_DIR to a custom path if cmake can not find Antioch by itself.

cmake -DQUESO=1 CMakeLists.txt ; make; make install.




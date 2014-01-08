Impact
======

The Interface for Multiscale Physics and ChemisTry intends to provide
a common friendly interface to interact and couple multiple C++ libraries.
The interface can be either used directly from C++, or from 
a Python-based integrated development environment ([see here](http://impact-one-for-all.googlecode.com/svn/website/index.html) ).

Currently Impact includes:

Atomism : Library to model motion of ponctual masses within a Lagrangian formalism (computation of
molecular thermodynamic data and reaction rates).

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

1. [boost] (http://www.boost.org/)
2. [python2.7] (); should work with older version of Python, but not sure.
3. [pyside] (http://qt-project.org/wiki/PySide)

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

1. cmake CMakeLists.txt (for more options, do 'cmake -L')
2. make (note parallel builds are supported)
3. make install

To launch the interface: 

1. export PYTHONPATH=$PathOfInstallDir/python 
2. from the bin directory: python impact_gui.py



namespace impact {
    
    /** \brief Module to compute thermodynamic data and reaction rates of molecular systems.
     *
     * @section overview Overview
     *
     * \i Atomism is a an open source library designed to study the motions of indivisible
     * elements on a Potential Energy Surfaces (PES). It is based on a Lagrangian formalism,
     * and emphasizes on statistical studies (computation of the density of states and partition
     * functions for thermodynamic and kinetic calculations). 
     * The library is entirely written in C++, it wrapped into python and part of the <em>Impact</em> project.
     *
     * *Atomism* is:
     *      - <b> Free and open source </b> :  All source codes is available under the GNU GPL
     *      - <b> Intuitive </b> : Built to work easily for students and advanced researchers;
     * using either C++, or python, or both. The API is clean, expressive and powerfull.
     *      - <b> Versatile </b> : It is built on Lagrangian formalism; in particular, the systems can be defined
     * using any type of coordinates. \n
     *      - <b> Extensible </b> : It is meant to be easily extended and adapted to specific case studies.
     * It provides several programming protocols and a high level of abstraction that increases considerably productivity.
     *      - <b> Reliable </b> : A large set of relevant test cases is beeing developed; a modern and safe object oriented style of
     * programing is used.
     *
     * The library is extended by a molecular package, designed to study atomic motions in polyatomic species.
     * Computation of thermodynamic properties (entropy, heat capacity, enthalpy,...)
     * or reaction rates is possible for molecules combining motions listed in the table below.
     *
     
     Type                                                    | Partition function | Density of states  | Formalism | Reference(s) on methods | Example(s) of application  |
     :----:                                                  |:----:              | :----:             | :----:    | :----:                  | :----:                     |
     Harmonic oscillators                                    | Yes                | Yes                | Quantum   |                         | Simple TD & Transition State Theorie (TST) computations |
     Anharmonic oscillators: perturbation at the 2nd order   | Yes                | Yes                | Quantum   | @ref NGuyen_JPCA2010    | Accurate TD & Semi-Classical TST with multi-dimensional tunneling |
     1D large amplitude motion                               | Yes                | Yes                | Quantum   | @ref Reinisch_JCP2010   | Very accurate model for 1D torsions, diatomic molecules |
     Multidimensional large amplitude motion                 | Yes                | Yes                | Classical | @ref Reinisch_JCTC2012  | Multi-dimensional torsions & Variational TST + Variable Reaction Coordinate |
     Asymmetric rigid rotor                                  | Yes                | No                 | Classical |                         | Simple TD & Transition State Theorie (TST) computation |
     Symmetric rigid rotor                                   | Yes                | Yes                | Quantum   |                         | RRKM computations |
     Linear rigid rotor                                      | Yes                | Yes                | Quantum   |                         | RRKM computations |
     *
     * @section doc Documentation
     *
     * The library is documented and illustrated by the different tutorials, they introduce step by step
     * the different features:
     *      - @ref python_demo1
     *      - @ref python_demo2
     *      - @ref python_demo3
     *      - @ref python_demo4
     *
     * @section    install Installation
     * @subsection getCode Getting the code
     *
     * To check out the source code:\n
     * <b> svn checkout https://atomism.googlecode.com/svn/trunk/ atomism </b>
     *
     * So far, the library is still under development, so only the trunk version is available,
     * a first release will be available soon.
     *
     * @subsection dependencies Dependencies
     *
     * Differents libraries are needed by atomism, the default installation required:
     *      - <a href="http://www.cmake.org/" > cmake </a> : required for compilation
     *      - <a href="http://www.boost.org/">  boost </a> : required about everywhere
     *      - <a href="http://eigen.tuxfamily.org/index.php?title=Main_Page"> Eigen </a> :
     * optional, it is required for linear algebra operations but if cmake doesn't find the library on your system
     * it will use the version supplied in src/Base/eigen3
     *      - <a href="http://www.python.org/getit/"> python </a> : optional, you need it
     * if you want to use atomism in python. If you don't have (or want) it, you need to turn off the python binding (see
     * cmake options below)
     *      - <a href="http://www.vtk.org/"> VTK </a> : optional, you need it
     * if you want to visualize the molecular structures. Otherwise, you need to turn off the VTK binding (see
     * cmake options below)
     *
     * @subsection compilation Compilation
     *
     * From the root directory execute the following commands for the default installation:
     *       - \b cmake \b CMakeLists.txt  : you can pass some options (see below), e.g. 'cmake -DOPTION=my_option CMakeLists.txt'
     *       - \b make  : compile
     *       - \b make \b install : install
     *       - \b make \b test : test
     *
     * Options you can pass to cmake:
     *       - -DCMAKE_INSTALL_PREFIX=/my/directory : set /my/directory as installation directory
     *       - -DPYTHON=1 : activate python binding (default), -DPYTHON=0 to desactivate
     *       - -DVTK=1 : link with the VTK part of the code for 3D visualization (default), -DVTK=0 to desactivate
     *       - -DCOVERAGE=0 : does not activate a compilation for line coverage studies during the tests (default),
     * if you activate it you need the library gcov (-DCOVERAGE=1 for activation)
     *       - -DBAYES=0 : does not activate a compilation for inverse problem and uncertaintie quantification (default)
     * if you want it you need to install the library <a href="https://red.ices.utexas.edu/projects/software/wiki/QUESO"> QUESO </a>
     * (-DBAYES=1 for activation)
     
     */
    namespace atomism {
    }
}

/*
 Copyright [2012] [REINISCH]
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 */
/*!
 * \file c_vpt2.h
 * \brief Definition of the classes  msOscillatorsAssembly and msOscillatorsAssemblyParams
 *
 * Definition of the classes:
 *	- msOscillatorsAssemblyParams
 *	- msAnhOscill
 *	- msReactAnhOscill
 *	- msOscillatorsAssembly
 *
 * \author G. Reinisch
 * \date 2012
 */

#ifndef msOscillatorsAssembly_H
#define msOscillatorsAssembly_H

#include <msOscillator.h>
#include <gsl/gsl_rng.h>/*
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
*/
namespace impact {
    
    namespace atomism {
        
        class msRAOp2;
        
        
        
        /*! \class msOscillatorsAssembly
         *
         * \brief Container/manager of coupled small amplitude anharmonic vibration.
         * Can contain a reaction motion to compute cumulative reaction probability
         *
         * This class model an assembly of coupled quantum anharmonic vibrations.
         * msOscillatorsAssembly derives from msMotion and msQMotion:
         * canonical and micro-canonical properties are computed using the quantum eigenvalues
         * of the assembly. The assembly is not associated to a specific type of
         * anharmonic oscillator, any class deriving from msAnhOscill can be included.
         *
         * A distinction of this class with respect to other msMotion derived classes is
         * that it can be linked to a reactive mode. In this case, the reactive mode allows
         * to compute the tunneling effect, in a multidimensional coupled
         * Cumulative Reaction Probability approache. This class compute the CRP at the
         * micro canonical level at the same time than the DOS is computed, the CRP
         * partition function is computed by the class msRAOp2 using a specific algorithm.
         *
         * \section PMA Physics, Mathematics, Algorithms
         *
         * The energy levels of an anharmonic oscillator are defined by a list of quantum
         * numbers (i,j,k...) (see msAnhOscill for the case of dunham expansion). The computation
         * of the partition function is based on the DOS.
         *
         * For small systems ( 3 or 4 atoms max) the DOS is easily computed by direct count
         * over the quantum number ( by nested 'for' kind loops). The method is exact, but
         * the computational time become quickly unmanagable when the size of the system
         * increases. \n
         * For bigger systems the Wang and Landau (ref) algorithm is used. The code is a
         * c++ traduction of the code adensum provided in Multiwell
         * (John R. Barker   and   Thanh Lam Nguyen )
         * The computation of the cumulative reaction probability is realized at the same
         * time (see \ref CRP_PMA )
         *  \xrefitem REF "Ref" "References" T. L. Nguyen and J. R. Baker in J. Phys. Chem. A,
         *  114, 3718 (2010): Sums and Densities of Fully Coupled Anharmonic Vibrational
         * States: A Comparison of Three Practical Methods
         *
         *  \xrefitem REF "Ref" "References" M. Basire, P. Parneix, and F. Calvo in J.
         * Chem. Phys. 129, 081101 (2008) : Quantum anharmonic densities of states using
         * the Wang–Landau method .
         *
         *  \xrefitem REF "Ref" "References" F. Wang and D. P. Landau in Phys. Rev. E., 64, 056101
         * (2001): Determining the Density of States for Classical Statistical Models: A Random Walk Algorithm
         * to Produce a Flat Histogram
         *
         */
        class msOscillatorsAssembly : public msMotion , public msQuantumInterface
        {
            
            
        protected:
            
            class msOscillatorsAssemblyParams : public msMotionParams
            {
                static msRegistrar Registrar;
                static msTreeMapper* createInstance() { return new msOscillatorsAssemblyParams; }
                
            public:
                
                void initialize() { msMotionParams::initialize();
                    addParameter( "DCLim" , "6" , "Max. number of atom for DOS direct count" , msUnit() );
                    addParameter( "nGaussHerm" ,  "32" , "No. of points to integrate along theta for Qcrp calculations" , msUnit("") );
                    addParameter( "nEigenLimit" , "10e3" , "Max. eigenvalues energy stored" , msUnit("") );
                    addParameter( "nMaxTrialsWL" , "100" , "Max. trials par energy grain for Wang and Landau algo." , msUnit("") );
                }
                
                static boost::shared_ptr<msOscillatorsAssemblyParams> New(){
                    
                    boost::shared_ptr<msOscillatorsAssemblyParams> T( new msOscillatorsAssemblyParams );
                    T->initialize(); T->update(); return T;
                }
                
                msOscillatorsAssemblyParams() : msMotionParams() {}
            };
            
            
        private:
            typedef vector< boost::weak_ptr<msOscillator> >::iterator iteratorOscillator;
            typedef vector< boost::weak_ptr<msOscillator> >::const_iterator const_iteratorOscillator;
            typedef msChildren< msOscillator >::iterator itAnhOscillator;
            typedef msChildren< msOscillator >::const_iterator const_itAnhOscillator;
            typedef msChildren< msHarmonicOscillator >::iterator itHarmOscillator;
            typedef msChildren< msHarmonicOscillator >::const_iterator const_itHarmOscillator;
            //! @Name from msRegistrer
            //@{
            static msRegistrar Registrar;
            static bool  isOscillatorsAssemblyRegisteredInPython ;
            static msRegister* createInstance() { return new msOscillatorsAssembly; }
            //@}
            
        protected:
            
            
            void registryInPython();
            
            
            void initialize(){
                
                msMotion::initialize();
                declareChildren<msHarmonicOscillator>(HarmOscillators,"HarmOscillators");
                declareChildren<msOscillator>(AnhOscillators,"AnhOscillators");
                DensOfStates_HO = msVectorFit1d::New();
                DensOfStates_AO = msVectorFit1d::New();
            }
            
            void update(){ msMotion::update();
                Oscillators.clear();
                for( itHarmOscillator it=HarmOscillators.begin();it!=HarmOscillators.end();++it)
                    Oscillators.push_back(*it);
                
                for( itAnhOscillator it=AnhOscillators.begin();it!=AnhOscillators.end();++it)
                    Oscillators.push_back(*it);
            }
            
        public:
            
            
            static boost::shared_ptr<msOscillatorsAssembly> New( ) {
                
                boost::shared_ptr<msOscillatorsAssembly> T( new msOscillatorsAssembly );
                T->initialize();
                T->setParameters( msOscillatorsAssemblyParams::New() );
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msOscillatorsAssembly> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            msOscillatorsAssembly() : msMotion() , msQuantumInterface() { constructVar("msOscillatorsAssembly","OscillatorsAssembly","oscillator assembly");
                /* const gsl_rng_type * T; gsl_rng_env_setup();
                 T = gsl_rng_default; rnd_gsl = gsl_rng_alloc (T);*/};
            
            boost::shared_ptr<msTreeMapper> addOscillator(boost::shared_ptr<msOscillator> oscill);
            
            double Qho(double T); 	//!< Total partition function in HO approximation
            
            double Q(double T);
            
            double ZPE() const ;
            double ZPE_AO() const;
            double ZPE_HO() const;
            bool isQuantum();
            
            void computeDOS();
            
            double DOS(double E){return -1;};
            
            ostream& print(ostream& out) const;
            
            
        protected:
            
            /** \brief Return the energy level for a particular (or current) state
             *
             * \param inds : the list of occupancies. If inds is null, the list Occups is used
             */
            double energyAO(const vector<int>& inds) const;
            
            /** \brief Check if the state under consideration is bounded
             *
             * \param nv occupancies, if not given Occups is used
             * \return 1 if bounded
             */
            bool isStateBounded(const vector<int>& nv) const ;
            
            /** \brief Generate random bounded state at energy inf. to emax
             *
             * The occupancy numbers corresponding to HO are set to 0.
             * \param emax maximum energy
             */
            void generateAnharmonicBoundedState(double emax);
            
            void TranslateForZPE(msVectorFit1d& dos,  double zpe );
            
            /** @name Density of states computation
             */
            //@{
            void computeDOS_HOpart();
            
            void computeDOS_AOpart();
            
            //!  compute the DOS by a direct count, used when System->nofOfElements() < getParameters()->DCLim
            void computeDOS_Direct();
            
            //!  compute the DOS using Wang and Landau algorithm.
            void computeDOS_WangLandau();
            
            //!  compute the CRP by a direct count.
            void computeCRP_Direct();
            
            void ConvolutionDos(); 	//!< Convolution of harmonic and anharmonic DOS
            
            /*! \brief compute the eigenstate
             *
             *  Definition of the virtual function from msQMotion. The counting is stopped at Emax or when
             * the number of eigenstate reach the limit (see the parameters)
             */
            void computeEigenStates(){};
            //@}
            
        private:
            
            msChildren< msOscillator > 		AnhOscillators; 	//!< Anharmonic oscillators: all
            
            msChildren< msHarmonicOscillator >      	HarmOscillators;	//!< Harmonic oscillators
            
            vector< boost::weak_ptr<msOscillator> > 	Oscillators; 		//!< All oscillators
            
            int DCLim, nGaussHerm, nEigenLimit, nMaxTrialsWL;  		//!< The parameters
            
        protected:
            
            /** if 1 EigenValues contain all the states contributing to the DOS,
             * eigenvalues should be use to compute the partition function.
             * Otherwise, use the density of states;
             */
            bool isDiscrete;
            
            boost::shared_ptr<msVectorFit1d> DensOfStates_HO;
            boost::shared_ptr<msVectorFit1d> DensOfStates_AO;
            
            std::vector<int>     Occups;   	//!< To store the occupancies, i.e quantum numbers
            
            void updateParameters();
            
            /** @name Random numbers generator
             * C++ rand() function seems not appropiated
             */
            //@{
            double Ran1();
            /*  int rndType;
             int idum; int iy;vector<int> iv;
             */
            gsl_rng * rnd_gsl;
            
            //@}
            
            
            /** @name direct count of DOS w/ DoF
             * These functions are the explicit count of the DOS w/ the number of DoF. Use nested 'for' loops.
             */
            //@{
            /** \brief Direct count of the DOS for 2 dofs
             *
             * \param Emax Maximum energy of the window
             * \param Emax Grain energy size
             * \param nLimitEV number under which the eigenvalues are stores 
             * (and use for the DOS)
             * \return Number of state counted. If == nLimitEV then the spectrum is
             * most likely truncated
             */
            double compDOSd_2();
            
            double compDOSd_3(); //!<  Direct count of the DOS for 3 dofs \copydetails compDOSd_2(double,int)
            
            double compDOSd_4(); //!<  Direct count of the DOS for 4 dofs \copydetails compDOSd_2(double,int)
            
            double compDOSd_5(); //!<  Direct count of the DOS for 5 dofs \copydetails compDOSd_2(double,int)
            
            double compDOSd_6(); //!<  Direct count of the DOS for 6 dofs \copydetails compDOSd_2(double,int)
            //@}
            
        };  
        
        ostream& operator<<(ostream& out,const msOscillatorsAssembly& oscil);
    }
}
#endif // msOscillatorsAssembly_H

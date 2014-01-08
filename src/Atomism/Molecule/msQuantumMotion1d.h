/*
 <one line to give the program's name and a brief idea of what it does.>
 Copyright (C) <year>  <name of author>
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */
/*!
 * \file c_1dquant.h
 * \brief One dimensional internal quantum motion
 * \author G.Reinisch
 */
#ifndef msQuantumMotion1d_H
#define msQuantumMotion1d_H


#include <msLagrangian.h>
#include <msQuantumInterface.h>

namespace impact {
    
    namespace atomism {
        
        /*! \class msQuantumMotion1d
         *
         * \brief 1D internal quantum motion. The Schrodinger equation is solved for any PES and kinetic function.
         *
         * This class is used to model a 1D internal quantum motion. This is a specialisation
         * of the class msLagrangian for 1D case, working with quantum statistics (and deriving
         * from msQuantumInterface).
         *
         * The resolution is based on an effective kinetic constant (ComputeKinEff(double))
         * used to compute an effective DOS w/ the temperature, from which partition function
         * is calculated using a direct count of the energy levels.
         *
         * \section quantumMotion1dPMA Physics, Mathematics, Algorithms
         *
         * In case of one dimensional motion the computation of the partition function
         * is done using quantum statistics. An effective temperature dependent kinetic
         * constant is introduced by the following relation
         * \f[
         K_{eff}(T)=\int K(q)P(q,T)dq
         \f]
         where
         \f[
         P(\theta,T)=\frac{\exp\left(-V(q)/RT\right)}{\int \exp\left(-V(q)/RT\right)dq}
         \f]
         * captures the influence of the temperature on the occupancy of different values of \f$ q \f$
         * and so on different values of $K(q)$, in close similarity with a Maxwell-Boltzmann
         * averaging.
         * The Fourier Grid Hamiltonian algorithm is employed to compute the eingenvalues
         * \f$ \varepsilon_i^{eff}(T) \f$ of the effective Hamiltonian defined by
         \f[
         H^{1D}_{eff}=-\frac{\hbar^2}{2K_{eff}}\frac{\partial^2}{\partial q^2}+V(q)
         \f]
         * The partition function is finally obtained through a direct counting of the eigenvalues:
         \f[
         Q^{1D}_{eff}(T)=\frac{1}{\sigma} \sum_i \text{exp}\left( -\varepsilon_i^{eff}(T)/k_BT \right)
         \f]
         *
         *
         * For more information, see reference @ref Reinisch_JCP2010 .
         */
        class msQuantumMotion1d : public  msLagrangian , public msQuantumInterface
        {
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool        isQuantumMotion1dRegisteredInPython;
            static msRegister* createInstance() { return new msQuantumMotion1d; }
            //@}
            
            class msKinFctObs : public msObservable {
	      
            public:
                msKinFctObs( msQuantumMotion1d* motion, const msUnitsManager* unit) : msObservable() { Motion=motion; Units=unit; };
                double operator()(){ return Motion->getKineticOperator()->kineticFunction( *Units ) ; };
                msQuantumMotion1d* Motion;
                const msUnitsManager* Units;
            };
            
            
        protected:
            
            void registryInPython();
            
            class msQM1dParams : public msLagrangianParams
            {
                static msRegistrar Registrar;
                static msRegister* createInstance() { return new msQM1dParams; }
                
            public:
                
                void initialize() {
                    
                    try{
                        msLagrangianParams::initialize();
                        addParameter( "BasisSize" , "201" , "Size of the basis set, MUST BE ODD" , msUnit() );
                        addParameter( "Tref"      , "500" , "Default temperature for DOS calculation" , msUnit("K") );
                    }
                    catch(msError& e){
                        
                        e.addContext("void msQM1dParams::initialize()");
                        throw e;
                    }
                }
                
                static boost::shared_ptr<msQM1dParams> New() { 
		    
		    boost::shared_ptr<msQM1dParams> T( new msQM1dParams );
                    T->initialize(); T->update();
                    return T;
                }
                
                msQM1dParams() : msLagrangianParams() {}
            };
                      
            void initialize()  {
                try{
                    msLagrangian::initialize();
                    KineticFunction          = msVectorFit1d::New();
                    EffectiveKineticFunction = msVectorFit1d::New();
                    EigenVectorCoordinate    = msScalarVariable::New();
                }
                catch(msError& e){
                    
                    e.addContext("void msQuantumMotion1d::initialize()");
                    throw e;
                }
            };
            
            void update()	 { msLagrangian::update();
                // updateParameters();
            }
            
            virtual void updateUnits(msUnitsManager& Old,msUnitsManager& New) {
	      
	        msLagrangian::updateUnits(Old,New);
	        for( size_t i=0;i <EigenValues.size(); ++i){
		  
		    EigenValues[i]*=New.convert(Old.getEnergy(),1);
	        }
	    }
	    
        public:
            
	    msQuantumMotion1d() : msLagrangian() , msQuantumInterface() {
                
                constructVar("msQuantumMotion1d","QuantumMotion1d","1D quantum motion");
                BasisSize=101;Tref=500;isKinFreezed=0;nDoF = 1;
            }
            
            static boost::shared_ptr<msQuantumMotion1d> New() {
	      
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msQuantumMotion1d> msQuantumMotion1d::New()","");
                LOGGER_WRITE(msLogger::DEBUG, "New msQuantumMotion1d object")
                
                boost::shared_ptr<msQuantumMotion1d> T( new msQuantumMotion1d );
                try{
                    T->initialize();
                    T->setParameters( msQM1dParams::New() );
                    T->update();
                }
                catch(msError& e){
                    
                    e.addContext("static boost::shared_ptr<msQuantumMotion1d> msQuantumMotion1d::New()");
                    throw e;
                }
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msQuantumMotion1d> msQuantumMotion1d::New()");
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msQuantumMotion1d> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            void freezeKinValue(double v){ 
	      
	        freezedKinValue = v;
                isKinFreezed=1;
            }
            void unfreezeKinValue(){ 
	      
	        freezedKinValue = -1;
                isKinFreezed = 0;
            }
            /** Solve the stationary Schrodinger equation, basis size is defined by the parameter BasisSize in the 'Parameters' attribut.
             */
            void solveSchrodinger(double kineff=-1);
            
            //! Compute the Kinetic effective constant at T
            double effectiveKineticFunction(double T) {return effectiveKineticFunction(T,*getUnits()); };
            
            //! Compute the Kinetic effective constant at T in a particular unit system
            double effectiveKineticFunction(double T,const msUnitsManager& unit);
            
            //! Zero Point Energy
            double ZPE() 		{ return msQuantumInterface::ZPE();
            };
            
            //! Compute the partition function by calling msQuantumInterface::Q(T)
            double Q(double T);
            
            //! Compute the partition function by calling msQuantumInterface::Q(T)
            double Q(double T,msVectorFit1d& kinEffFct);
            
            //!< Fill density of states and sum of states of the msMotion class
            void computeDOS()		{ computeEigenValues();
            };
            
            double DOS(double E)      { 
	      
	        if( EigenValues.size() ==0 ) computeDOS();
                return (*DensOfStates)(E);
            }
            
            
            bool isQuantum() {return 1;}
            
            //! Compute the classical partition function by calling msLagrangian::Q(T)
            double Qclassical(double T) { return msLagrangian::Q(T); };
            
            //! Compute the eigenstates, Definition of the virtual function from msQuantumInterface
            void computeEigenValues()   { solveSchrodinger(); };
            
            boost::shared_ptr<msVectorFit1d> computeKineticFunction();
            
            boost::shared_ptr<msVectorFit1d> computeEffectiveKineticFunction();
            
            boost::shared_ptr<msVectorFit1d> getEigenvector(size_t i);
            
            boost::shared_ptr<msVectorFit1d> getKineticFunction(){ return KineticFunction; };;
            
            boost::shared_ptr<msVectorFit1d> effectiveKineticFunction(){  
	      
	        if( EffectiveKineticFunction->size()==0 ) computeEffectiveKineticFunction();
                return EffectiveKineticFunction;
            };
           
	     void FillEvAndG( vector<double>& ev , vector<double>& g ) {
	      
	         for( size_t i=0; i<EigenValues.size();i++) {
		  
		     ev[i] = EigenValues[i]; g[i]=1;
		 }
	    };
        protected:
            
            double Tref;
            
            int BasisSize;
            
            void updateParameters();
           	    
        private:
            
            
            double freezedKinValue;
            bool isKinFreezed;
	    
            double previousKinUsed;
            int    previousBasisSize;
	    
            boost::shared_ptr<msScalarVariable>   EigenVectorCoordinate;
            
            boost::shared_ptr<msVectorFit1d> 	  KineticFunction;
            
            boost::shared_ptr<msVectorFit1d> 	  EffectiveKineticFunction;
            
            matrix_type Hamiltonian;
            matrix_type EigenStates;
            
        };
    }
}
#endif // msQuantumMotion1d_H


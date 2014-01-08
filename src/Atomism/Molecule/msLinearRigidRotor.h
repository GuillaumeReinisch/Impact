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
 * \file LinearRigidRotor
 * \brief degenerated 2D rigid rotor
 * \author G.Reinisch
 * \date 2011
 */

#ifndef msLinearRigidRotor_H
#define msLinearRigidRotor_H


#include <msRigidRotor.h>

namespace impact {
    
    namespace atomism {
        
        /*! \class msLinearRigidRotor
         * \brief A degenerated 2D rigid rotor model used for linear system (two moment of inertia equal, one equal to zero)
         *
         * The exact quantum spectrum is calculated, and used to compute the partition function and SOS and DOS.
         *
         * \section linearRotorPMA Physics, Mathematics, Algorithms
         * The energy levels for a degenerated 2D rotor are given by:
         \f[
         E_J=B\times J \times (J+1)
         \f]
         * where \f$ B \f$ is the rotational constant of the rotor, the energy levels are degenerated \f$ 2J+1 \f$ times.
         */
        class msLinearRigidRotor : public msRigidRotor
        {
            
            /*! \class msLinearRigidRotorParams
             * \brief Parameters for degenerated 2D rigid rotor computations
             *
	     * No more defined than msRigidRotorParams
	     */ 
            class msLinearRigidRotorParams : public msRigidRotorParams
            {
                static msRegistrar Registrar;
                static msRegister* createInstance() { return new msLinearRigidRotorParams; }
                        
            public:
                
                void initialize() { msRigidRotorParams::initialize();
                }
                
                static boost::shared_ptr<msLinearRigidRotorParams> New() {
		  
		    boost::shared_ptr<msLinearRigidRotorParams> T( new msLinearRigidRotorParams );
                    T->initialize(); T->update(); return T;
                }
                
                msLinearRigidRotorParams() : msRigidRotorParams() {
	        }
            };
            
            
        private:
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool        isLinearRigidRotorRegisteredInPython;
            static msRegister* createInstance() { return new msLinearRigidRotor; }
            //@}
            double* I2d;
            
        protected:
            
            void registryInPython();
            
        public:
            
            void initialize(){ msRigidRotor::initialize();
                declareAttribute( I3,"I2d");
            }
            
            void update(){ msRigidRotor::update();
	        I2d = &I3;
            }
            
            virtual void updateUnits(msUnitsManager& Old,msUnitsManager& New) {
	      
	        msRigidRotor::updateUnits(Old,New);
	        for( size_t i=0;i <EigenValues.size(); ++i){
		  
		    EigenValues[i]*=New.convert(Old.getEnergy(),1);
	        }
	    }
	    
            static boost::shared_ptr<msLinearRigidRotor> New() {
                
                boost::shared_ptr<msLinearRigidRotor> T( new msLinearRigidRotor );
                T->initialize(); 
                T->setParameters( msLinearRigidRotorParams::New() );
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
	      
                boost::shared_ptr<msLinearRigidRotor> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            msLinearRigidRotor() : msRigidRotor() { 
	      
	      constructVar("msLinearRigidRotor","LinearRigidRotor","linear rigid rotor");I2d=&I3; 
	    };
            
            void computeEigenValues();
            
            double Q(double T) { 
	      
	        LOGGER_ENTER_FUNCTION("double msLinearRigidRotor::Q(double T)",getFullId());
                computeEigenValues();
                double q = msQuantumInterface::Q( T , getUnits()->convert(msUnit::J_mol,csts::R) );
		LOGGER_EXIT_FUNCTION2("double msLinearRigidRotor::Q(double T)");
                return q * Mult / Sym; 
            }
            
            ostream& print(ostream& out) const;
	    
	    void FillEvAndG( vector<double>& ev , vector<double>& g_ ) {
	      
	         for( size_t i=0; i<EigenValues.size();i++) {
		  
		     ev[i] = eigenvalue(i); g_[i]=degenerency(i);
		 }
	    };
            
        };
        
        ostream& operator<<(ostream& out,const msLinearRigidRotor& rotor);
        
    }
}

#endif // msLinearRigidRotor_H

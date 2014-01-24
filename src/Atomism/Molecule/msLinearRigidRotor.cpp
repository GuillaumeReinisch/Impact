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

#include "msLinearRigidRotor.h"

namespace impact {
    
    namespace atomism {
        
        bool msLinearRigidRotor::isLinearRigidRotorRegisteredInPython=0;
        msRegistrar msLinearRigidRotor::Registrar("msLinearRigidRotor", msLinearRigidRotor::createInstance);
        
        
        ostream& operator<<(ostream& out,const msLinearRigidRotor& rotor) {
            
            rotor.print(out);
            return out;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msLinearRigidRotor::registryInPython() {
            
#if USE_PYTHON
            msRigidRotor::registryInPython();
            
            if( ! msLinearRigidRotor::isLinearRigidRotorRegisteredInPython )
            {
                using namespace boost::python;
                
                class_< msLinearRigidRotor, bases<msRigidRotor>, boost::shared_ptr<msLinearRigidRotor> >(
                                                                                                         "msLinearRigidRotor", "A rigid rotor motion of a linear system" ,
                                                                                                         no_init )
                .def( "New", &msLinearRigidRotor::New , "Create a new object.")
                .staticmethod("New")
                .def(self_ns::str(self_ns::self));
               		
		class_< msLinearRigidRotorParams, bases<msRigidRotorParams>, boost::shared_ptr<msLinearRigidRotorParams> >(
                        "msLinearRigidRotorParams", "A rigid rotor motion of a linear system" ,
                         no_init )
		.def( "New", &msLinearRigidRotorParams::New , "Create a new object.")
                .staticmethod("New");
		
		msTreeMapper::finalizeDeclarationForPython<msLinearRigidRotor>("msLinearRigidRotor");
		
		msLinearRigidRotor::isLinearRigidRotorRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msLinearRigidRotor::computeEigenValues() {
            
            LOGGER_ENTER_FUNCTION("void msLinearRigidRotor::computeEigenValues()",getFullId());
            LOGGER_WRITE(msLogger::DEBUG,"I2d = "+output::getString<double>(*I2d));
	    
	    if( *I2d == -1 ) {
	      
	       msError e("Can not compute eigenvalues: moment of inertia not initialized",
		         "void msLinearRigidRotor::computeEigenValues()",
			 getFullId());
	       BOOST_THROW_EXCEPTION(e);  
	    }
	    
            double convFactToSi =  SiUnits.convert( getUnits()->getMass(),1) * pow( SiUnits.convert(getUnits()->getLength(),1) , 2 );
            double B = getUnits()->convert( SiUnits.getEnergy() , pow(csts::hb,2) / ( 2 * (*I2d) * convFactToSi ) );
            size_t n = sqrt( msMotion::Emax / B ) + 1;
            EigenValues = vector_type::Zero(n);
            g.resize(n,1);
            
            LOGGER_WRITE(msLogger::DEBUG,"rotational constant = "+output::getString<double>(B)+getUnits()->getEnergyStr() );
            
            for(int j=0; j<n; ++j){ EigenValues[j]=B*j*(j+1);
                g[j] = 2*j + 1;
            }
            
            LOGGER_WRITE(msLogger::DEBUG,"Fill DOS and SOS" );
            fillDOS(*DensOfStates);
            fillSOSfromDOS();
            //checkCv(Tmax);
            
            LOGGER_EXIT_FUNCTION();
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        ostream& msLinearRigidRotor::print(ostream& out) const  {
            
            msRigidRotor::print(out);
            output::printHeader(out,"Linear Rigid Rotor");
            
            out<<"\n Degenerated axis with moment of inertia of "<<*I2d<<" "<<getUnits()->getMassStr()<<"."<<getUnits()->getLengthStr()<<"^2"<<endl;
            
            return out;
        }
    }
}

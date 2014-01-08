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

#include "msRigidRotor.h"

namespace impact {
    
    namespace atomism {
        
        bool msRigidRotor::isRigidRotordRegisteredInPython=0;
        
        msRegistrar msRigidRotor::Registrar("msRigidRotor", msRigidRotor::createInstance);
        
        
        ostream& operator<<(ostream& out,const msRigidRotor& rotor) {
            
            rotor.print(out);
            return out;
        }
        
        
        void msRigidRotor::registryInPython() {
            
#if USE_PYTHON
            msMotion::registryInPython();
            
            if( ! msRigidRotor::isRigidRotordRegisteredInPython )
            {
                using namespace boost::python;
                msQuantumInterface::registryInPython();
                
                class_< msRigidRotor, bases<msMotion,msQuantumInterface>, boost::shared_ptr<msRigidRotor> >(
                                                                                                            "msRigidRotor", "A rigid rotor motion" ,
                                                                                                            no_init )
                .def( "New", &msRigidRotor::New , "Create a new object.")
                .staticmethod("New")
                .def( "setMmtOfInertia" , &msRigidRotor::setMmtOfInertia,
                     "set the principal moment of inertia from the geometry of a system. arg2: system" )
                .def( "Qj", ( double (msRigidRotor::*)(size_t,double)) &msRigidRotor::Qj,
                     "partition function at a particular angular momentum. arg2: J quantum number, arg3: temperature" )
                .def( "DOSj", &msRigidRotor::DOSj,
                     "Density of states at a particular angular momentum. arg2: J quantum number, arg3: energy" )
                .def( "computeDOSj", &msRigidRotor::computeDOSj,
                     "Compute the DOS in [0:Emax] every dE_dos at a particular angular momentum. arg2: J quantum number" )
                .def( "getI1", &msRigidRotor::getI1, "Return the first principal moment of inertia.")
                .def( "getI2", &msRigidRotor::getI2, "Return the second principal moment of inertia.")
                .def( "getI3", &msRigidRotor::getI3, "Return the third principal moment of inertia.")
                .def(self_ns::str(self_ns::self));
                
		class_< msRigidRotorParams, bases<msMotionParams>, boost::shared_ptr<msRigidRotorParams> >(
                        "msLinearRigidRotorParams", "Parameters of a rigid rotor" ,
                         no_init )
		.def( "New", &msRigidRotorParams::New , "Create a new object.")
                .staticmethod("New");
		
		msTreeMapper::finalizeDeclarationForPython<msRigidRotor>("msRigidRotor");
		
                msRigidRotor::isRigidRotordRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msRigidRotor::setMmtOfInertia( boost::shared_ptr<msSystem> System) {
            
            if( System == boost::shared_ptr<msSystem>() )
                
                BOOST_THROW_EXCEPTION(msError( "The system is not defined, please use the function 'setSystem'",
                                              "void msRigidRotor::getEigenInertia(double& I1, double& I2, double& I3) const",getFullId()));
            
            vector_type3d eval;   matrix_type3d evect;
	    System->computeCartCoordinates();
            System->computeEigenInertia( evect , eval );
            
            double convFact =  getUnits()->convert( System->getUnits()->getMass(),1) * pow(  getUnits()->convert( System->getUnits()->getLength(),1) , 2 );
            I1=convFact*eval[0];  I2=convFact*eval[1];  I3=convFact*eval[2];
            update();
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msRigidRotor::updateParameters() {
            
            try{
                msMotion::updateParameters();
            }
            catch(msError& e)
            {
                e.addContext("can not get the parameters (void  msQuantumMotion1d::updateParameters())");
                throw e;
            }
            
            DensOfStates->set(0, msMotion::Emax, dE, getUnits()->getEnergyStr(), getUnits()->getEnergyStr()+"^-1" );
            SumOfStates->set(0, msMotion::Emax, dE, getUnits()->getEnergyStr(), "" );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msRigidRotor::Q(double T)  {
            
            LOGGER_ENTER_FUNCTION("double msRigidRotor::Q(double T)",getFullId());
            
            double convFact =  SiUnits.convert( getUnits()->getMass(),1) * pow( SiUnits.convert(getUnits()->getLength(),1) , 2 );
            
            LOGGER_WRITE(msLogger::INFO,"Compute partition using sym="+output::getString<int>(Sym));
            LOGGER_EXIT_FUNCTION();
            
            return  sqrt( csts::pi * I1 * I2 * I3 * pow(convFact,3))
            * pow( (8 *pow( csts::pi , 2) * csts::kb * T) / ( csts::h * csts::h ),1.5) / Sym;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        /*
         void msRigidRotor::getEigenInertia(double& I1, double& I2, double& I3) const
         {
         if( System.getSharedPtr() == boost::shared_ptr<msSystem>() )
         
         BOOST_THROW_EXCEPTION(msError( "The system is not defined, please use the function 'setSystem'",
         "void msRigidRotor::getEigenInertia(double& I1, double& I2, double& I3) const",getFullId()));
         
         vector_type3d eval;   matrix_type3d evect;
         System->computeEigenInertia( evect , eval );
         I1=eval[0];  I2=eval[1];  I3=eval[2];
         }*/
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        ostream& msRigidRotor::print(ostream& out) const  {
            
            msMotion::print(out);
            output::printHeader(out,"Rigid Rotor");
            
            // double I1, I2, I3;  getEigenInertia(I1,I2,I3);
            
            out<<"\nPrincipal moment of inertia of the rotor: "<<I1<<", "<<I2<<", "<<I3<<" "<<getUnits()->getMassStr()<<"."<<getUnits()->getLengthStr()<<"^2"<<endl;
            
            return out;
        }
    }
}



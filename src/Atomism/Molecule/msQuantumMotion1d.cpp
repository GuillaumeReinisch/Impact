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

#include "msQuantumMotion1d.h"


namespace impact {
    
    namespace atomism {
        
        bool msQuantumMotion1d::isQuantumMotion1dRegisteredInPython=0;
        
        msRegistrar msQuantumMotion1d::Registrar("msQuantumMotion1d", msQuantumMotion1d::createInstance);
        
        std::ostream& operator<<(std::ostream& out,const msQuantumMotion1d& motion)
        {
            motion.print(out);
            return out;
        }
        
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msQuantumMotion1d::registryInPython() {
            
#if USE_PYTHON
            msLagrangian::registryInPython();
            msQuantumInterface::registryInPython();
            
            if( ! msQuantumMotion1d::isQuantumMotion1dRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msQuantumMotion1d,bases<msLagrangian,msQuantumInterface>,boost::shared_ptr<msQuantumMotion1d> >
                ("msQuantumMotion1d",
                 "Study 1D motion using a quantum formalism.",no_init )
                .def( "New", &msQuantumMotion1d::New , "Create a new object. Arg1: msUnitsManager object")
                .staticmethod("New")
                .def( "effectiveKineticFunction",  		(double(msQuantumMotion1d::*)(double) )
                     &msQuantumMotion1d::effectiveKineticFunction
                     , "Compute the effective kinetic constant. arg2: temperature [double,K]")
                .def( "computeEffectiveKineticFunction" , 	&msQuantumMotion1d::computeEffectiveKineticFunction
                     , "compute the temperature dependant effective kinetic function")
                .def( "effectiveKineticFunction" , 		(boost::shared_ptr<msVectorFit1d>(msQuantumMotion1d::*)() )
                     &msQuantumMotion1d::effectiveKineticFunction
                     , "return the last effective kinetic function computed")
                .def( "computeKineticFunction" , 		&msQuantumMotion1d::computeKineticFunction
                     , "compute the kinetic function")
                .def( "getKineticFunction" , 		(boost::shared_ptr<msVectorFit1d>(msQuantumMotion1d::*)() )
                     &msQuantumMotion1d::getKineticFunction
                     , "return the last kinetic function computed")
                /* .def( "Q" , 			        (double (msQuantumMotion1d::*)(double T,msVectorFit1d& kinEffFct) )
                 &msQuantumMotion1d::Q
                 , "compute Q given the effective kinetic function") */
                .def( "getEigenvector" , 			&msQuantumMotion1d::getEigenvector
                     , "return an eigenvector. arg2: indice of the eigenvector")
                .def( "Qclassical", 			&msQuantumMotion1d::Qclassical
                     , "Compute the classical partition function (from class msLagrangian). Arg1: temperature [double,K]")
                .def( "freezeKinValue", &msQuantumMotion1d::freezeKinValue,
                     "freeze the kinetic value used for solving the schrodinger equation. arg2: value")
                .def( "unfreezeKinValue", &msQuantumMotion1d::unfreezeKinValue,
                     "compute the kinetic value each time the schrodinger equation is solved.")
                .def(self_ns::str(self_ns::self));
                
                msQuantumMotion1d::isQuantumMotion1dRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msQuantumMotion1d::updateParameters() {
            
            try{
                msLagrangian::updateParameters();
                getParameters()->queryIntValue("BasisSize", BasisSize );
                getParameters()->queryDoubleValue("Tref", Tref, *getUnits() );
            }
            catch(msError& e)
            {
                e.addContext("can not get the parameters (void  msQuantumMotion1d::updateParameters())");
                throw e;
            }
            
            DensOfStates->set(0, msMotion::Emax, dE, getUnits()->getEnergyStr(), getUnits()->getEnergyStr()+"^-1" );
            SumOfStates->set(0, msMotion::Emax, dE, getUnits()->getEnergyStr(), "" );
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        boost::shared_ptr<msVectorFit1d> msQuantumMotion1d::computeKineticFunction() {

            LOGGER_ENTER_FUNCTION( "boost::shared_ptr<msVectorFit1d> msQuantumMotion1d::computeKineticFunction()" , getFullId() );
           
            KineticFunction->set( *((*getGeneralizedCoordinates())[0]) ,
				  getUnits()->getMassStr()+"."+getUnits()->getLengthStr()+"^2"  );
            
            int i=0;
            double x0=(*getGeneralizedCoordinates())[0]->getValue();
            
            try{
                for( (*getGeneralizedCoordinates())[0]->begin();
                     (*getGeneralizedCoordinates())[0]->increment();i++)
                    
                    (*KineticFunction)[i] = getKineticOperator()->kineticFunction(* getUnits());
                
            }
            catch(msError& e) {
	        e.addContext("boost::shared_ptr<msVectorFit1d> msQuantumMotion1d::computeKineticFunction())");
                BOOST_THROW_EXCEPTION(e);
            }
            
            (*getGeneralizedCoordinates())[0]->setValue( x0 );
            
	    LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msVectorFit1d> msQuantumMotion1d::computeKineticFunction()" );
           
            return KineticFunction;
        }
        
        //-------------------------------------------------------------------------------
        
        boost::shared_ptr<msVectorFit1d> msQuantumMotion1d::computeEffectiveKineticFunction() {
            
            LOGGER_ENTER_FUNCTION( "msQuantumMotion1d::computeEffectiveKineticFunction()" , getFullId() );
            
            EffectiveKineticFunction->set(100, 2000, 300, getUnits()->getTempStr(),getUnits()->getMassStr()+"."+getUnits()->getLengthStr()+"^2"  );
            EffectiveKineticFunction->getVariable(0)->setId("T");
            
            int i=0;
            getParameters();
            try{
                for( EffectiveKineticFunction->getVariable(0)->begin();
                    EffectiveKineticFunction->getVariable(0)->increment();i++)
                    
                    (*EffectiveKineticFunction)[i] = effectiveKineticFunction( EffectiveKineticFunction->getVariable(0)->getValue(), *getUnits() );
                
            }
            catch(msError& e) {e.addContext("computation of the effective kinetic function failed (boost::shared_ptr<msVectorFit1d> msQuantumMotion1d::computeEffectiveKineticFunction())");
                LOGGER_EXIT_FUNCTION();
                throw e;
            }
            LOGGER_EXIT_FUNCTION();
            return EffectiveKineticFunction;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        double msQuantumMotion1d::Q(double T) {
            
            LOGGER_ENTER_FUNCTION( "msQuantumMotion1d::Q(double T)" , getFullId() );
            
            double T0=Tref; Tref=T;
            solveSchrodinger();
            LOGGER_WRITE( msLogger::INFO , "Compute Q using a direct count of the eigenvalue computed.");
            double Q = msQuantumInterface::Q( T , getUnits()->convert(msUnit::J_mol,csts::R) );
            
            Tref=T0;
            LOGGER_EXIT_FUNCTION();
            return Q;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        double msQuantumMotion1d::Q(double T,msVectorFit1d& kinEffFct) {
            
            LOGGER_ENTER_FUNCTION( "msQuantumMotion1d::Q(double T,msVectorFit& kinEffFct)" , getFullId() );
            
            double T0=Tref; Tref=T;
            solveSchrodinger( getUnits()->convert(kinEffFct.getYUnit(),kinEffFct(T)) );
            
            LOGGER_WRITE( msLogger::INFO , "Compute Q using a direct count of the eigenvalue computed.");
            double Q = msQuantumInterface::Q( T , getUnits()->convert(msUnit::J_mol,csts::R) );
            
            Tref=T0;
            LOGGER_EXIT_FUNCTION();
            return Q;
            
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        double msQuantumMotion1d::effectiveKineticFunction( double T, const msUnitsManager& unit )
        {
	    LOGGER_ENTER_FUNCTION( "double msQuantumMotion1d::effectiveKineticFunction( double T, const msUnitsManager& unit )" , getFullId() );
	    try{
                computeKineticFunction();
                msKinFctObs observable(this,&unit);
                double r = msLagrangian::averageByBoltzmann( observable , T);
                LOGGER_EXIT_FUNCTION2("double msQuantumMotion1d::effectiveKineticFunction( double T, const msUnitsManager& unit )");
		        return r;
	    }
	    catch(msError& e){
	      
	        e.addContext("double msQuantumMotion1d::effectiveKineticFunction( double T, const msUnitsManager& unit )"); 
		BOOST_THROW_EXCEPTION(e);
	    }
            return -1;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        boost::shared_ptr<msVectorFit1d> msQuantumMotion1d::getEigenvector(size_t i)
        {
            boost::shared_ptr<msVectorFit1d> states = msVectorFit1d::New();
            double min = getGeneralizedCoordinates()->getMin( getGeneralizedCoordinates()->begin(), *getUnits() );
            double max = getGeneralizedCoordinates()->getMax( getGeneralizedCoordinates()->begin(), *getUnits() );
            
            states->set(min,max, (max-min)/EigenValues.size(), (*getGeneralizedCoordinates())[0]->getUnitStr() , "" );
            
            double aera=0;
            try{
                for( int j=0; j<EigenValues.size(); ++j) aera += EigenStates(j,i)*EigenStates(j,i);
                
                for( size_t j=0; j<states->size(); ++j) (*states)[j]= EigenStates(j,i) * EigenStates(j,i) / aera;
            }
            catch(msError& e) {
	      
	        e.addContext("boost::shared_ptr<msVectorFit1d> msQuantumMotion1d::eigenvector(size_t i)");
                BOOST_THROW_EXCEPTION(e);
            }
            
            return states;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        void msQuantumMotion1d::solveSchrodinger(double kineff) {
            
            LOGGER_ENTER_FUNCTION( "void msQuantumMotion1d::solveSchrodinger()" , getFullId() );
            
            sanityCheck();
            
            int NX = BasisSize;double ZMU = freezedKinValue;
            
            msUnitsManager unit; unit.set("Bohr Radian amu hartree");
            
            if( !isKinFreezed )  ZMU = effectiveKineticFunction(Tref,unit);
            
            if( (EigenValues.size()!=0) && 
	        (ZMU==previousKinUsed)  &&
	        ( previousBasisSize == BasisSize) ) { 
	      
	        LOGGER_EXIT_FUNCTION2("void msQuantumMotion1d::solveSchrodinger()");
                return;
            }
            previousKinUsed = ZMU;
	    previousBasisSize = BasisSize;
	    
            EigenValues = vector_type::Zero(NX);
            g.resize(NX,1);
            Hamiltonian = matrix_type::Zero(NX,NX);
            
            double RMIN = getGeneralizedCoordinates()->getMin( getGeneralizedCoordinates()->begin(), unit);
            double RMAX = getGeneralizedCoordinates()->getMax( getGeneralizedCoordinates()->begin(), unit);
            
            //-----------------------------------------------------------------------------------------------------------------------------------
            // At this point RMIN and RMAX are either radian or Bohr, and ZMU either amu/Bohr2 or amu --> ok for calculations; now compute constant
            //-----------------------------------------------------------------------------------------------------------------------------------
            
            double convFactor = getUnits()->convert( unit.getEnergy() , 1 );
            msQuantumInterface::Emin = 10000000;
            msQuantumInterface::R    = getUnits()->convert( msUnit::J_mol , csts::R );
            ZMU       *= 1836.9822/1.0078; //masse en au;
            double DX  = (RMAX-RMIN)/NX;
            double n   = (NX-1)/2.;
            double TL0 = 2*csts::pi*csts::pi/( ZMU *( (RMAX-RMIN)*(RMAX-RMIN) ));
            double CONST3=0;
            for(int L=1;L<=n;L++)  CONST3=CONST3+TL0*L*L;
            CONST3=CONST3*2./double(NX);
            
            stringstream comment;
            comment<<"Effective kinetic constant computed at "<<output::getString<double>(Tref)
            <<" Compute H with NX="<<NX<<"; qmin= "<<RMIN<<" "<<"qmax[0]= "<<RMAX<<" in "<<"[radian or Bohr] (dX="<<DX<<")"
            <<" and ZMU="<<ZMU<<" in [ amu/Bohr2 or amu]...compute hamiltonian...";
            LOGGER_WRITE( msLogger::INFO , comment.str() );
            
            //--------------------------------
            //..Now compute Hamiltonian matrix
            //--------------------------------
            try{
                
                for(int I=0;I<NX;I++) {
                    for(int J=0;J<=I;J++) {
                        
                        int IJD=I-J;
                        if(I==J) Hamiltonian(I,J) = CONST3;
                        else{
                            double Vc=0;
                            for(int L=1;L<=n;L++) Vc=Vc+TL0*L*L*cos(L*2.*csts::pi*IJD/NX);
                            Vc=Vc*2./NX;
                            Hamiltonian(I,J) = Vc;
                        }
                    }
                    getGeneralizedCoordinates()->setValue( getGeneralizedCoordinates()->begin() , RMIN+I*(RMAX-RMIN)/NX, unit );
                    
                    Hamiltonian(I,I) += U() / convFactor;
                    msQuantumInterface::Emin = min( U() , msQuantumInterface::Emin );
                }
                for(int I=0;I<NX;I++) for(int J=0;J<=I;J++) Hamiltonian(J,I)=Hamiltonian(I,J);
            }
            catch(msError& e){e.addContext("computation of the hamiltonian matrix failed (void msQuantumMotion1d::solveSchrodinger())");}
            //-------------------------------- 
            //..Now compute eigenvalues:
            //--------------------------------     
            LOGGER_WRITE(msLogger::INFO,"computes eigenstates");
            Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(Hamiltonian);
            EigenStates =  eigensolver.eigenvectors() ;
            EigenValues =  eigensolver.eigenvalues()* convFactor;
            
            LOGGER_WRITE(msLogger::INFO,"ZPE=" + output::getString<double>(EigenValues[0]) + ";" + " first energy level:" + output::getString<double>(EigenValues[1])+getUnits()->getEnergyStr());
            
            msQuantumInterface::fillDOS( *DensOfStates ); 
            
            fillSOSfromDOS();
            LOGGER_EXIT_FUNCTION2( "void msQuantumMotion1d::solveSchrodinger()" );
        }
    }
    
}

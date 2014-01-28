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

#include "msMotion.h"

namespace impact {
    
    namespace atomism {
        
        bool msMotion::isMotionRegisteredInPython=0;
        msRegistrar msMotion::Registrar("msMotion", msMotion::createInstance);
        
        ostream& operator<<(ostream& out,const msMotion& motion)
        {
            motion.print(out);
            return out;
        }
        
        void msMotion::registryInPython()
        {
#if USE_PYTHON
            msTreeMapper::registryInPython();
            
            if( ! msMotion::isMotionRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msMotion,bases<msPhysicalInterface>,boost::shared_ptr<msMotion> >
                ("msMotion",
                 "Define a container of elements of type 'z-matrix'",no_init )
                .def( "New", &msMotion::New ,
                     "Create a new object. Arg1: msUnitsManager object")
                .staticmethod("New")
                .def("noOfDof",&msMotion::noOfDof,
                     "return the number of degrees of freedom described by the motion")
                .def("noOfSymmetry",&msMotion::noOfSymmetry,
                     "return the multiplicity of the motion")
                .def("noOfMultiplicity",&msMotion::noOfMultiplicity,
                     "return the number of degrees of freedom described by the motion")
                .def("Q", &msMotion::Q,
                     "return the partition function. Arg1: Temperature" )
                .def("getQ",&msMotion::getQ,
                     "return the partition function w/ temperature." )
                .def("getQfromDOS",&msMotion::getQfromDOS,
                     "return the partition function computed from the DOS w/ temperature." )
                .def("S", &msMotion::S,
                     "return the entropy. Arg1: Temperature" )
                .def("getS", &msMotion::getS,
                     "return the entropy w/ temperature." )
                .def("Cp", &msMotion::Cp,
                     "return the calorific capacity at constant pressure. Arg1: Temperature" )
                .def("getCp", &msMotion::getCp,
                     "return the Cp w/ temperature." )
                .def("Cv", &msMotion::Cv,
                     "return the calorific capacity at constant volume. Arg1: Temperature" )
                .def("Eav", &msMotion::Eav,
                     "return the thermal energy. Arg1: Temperature" )
                .def("computeDOS", &msMotion::computeDOS,
                     "compute the DOS in [0,Emax]." )
                .def("getSOS", &msMotion::getSOS,
                     "return the sum of states function."  )
                .def("getDOS",  &msMotion::getDOS,
                     "return the density of states function.")
                .def("SOS", (double (msMotion::*)(double) ) &msMotion::SOS,
                     "return the sum of states. Arg2: Energy"  )
                .def("DOS", ( double (msMotion::*)(double) ) &msMotion::DOS,
                     "return the density of states. Arg2: Energy")
                .def("QfromDOS",(double (msMotion::*)(double) )&msMotion::QfromDOS, "return the partition function computed from the DOS. Arg1: Temperature" )
                //  .def("ZPE", &msMotion::ZPE, "return the ZPE" )
                .def("setSymmetryNumber",  &msMotion::setSymmetryNumber, "Set the symetry number of the motion. arg2: symmetry number")
                .def_readwrite("Activated",&msMotion::Activated);
                
                class_< msMotionParams, bases<msParamsManager>, boost::shared_ptr<msMotionParams> >(
                                                                                                    "msMotionParams", "Parameters of a motion" ,
                                                                                                    no_init )
                .def( "New", &msMotionParams::New , "Create a new object.")
                .staticmethod("New");
                
                msTreeMapper::finalizeDeclarationForPython<msMotion>("msMotion");
                
                msMotion::isMotionRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msMotion::updateParameters() {
            
            LOGGER_ENTER_FUNCTION_DBG("void msMotion::updateParameters()", getFullId());
            
            try{
                initLocalParameters();
                stringstream out;
                out<<"dE="<<dE<<", Emax="<<Emax<<", dT="<<dT<<", De_dos="<<dE_dos<<", Tmin="<<Tmin<<", Tmax="<<Tmax<<", DeltaT="<<DeltaT;
                LOGGER_WRITE(msLogger::DEBUG, "parameters values: "+ out.str());
            }
            catch( msException& e)
            {
                e.addContext("void msMotion::updateParameters()");
                throw e;
            }
            LOGGER_EXIT_FUNCTION2("void msMotion::updateParameters()");
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotion::SOS( double E )
        {
            LOGGER_ENTER_FUNCTION("double msMotion::SOS( double E )",getFullId());
            double sos;
            try{ getParameters();
            }
            catch(...)
            {throw;
            }
            
            if( E > Emax ) LOGGER_WRITE(msLogger::WARNING,"E out of [0-Emax] bound");
            
            if( (SumOfStates->size()==0) || (E > Emax) )  sos=SOS(E);
            else sos = (*SumOfStates)[min( E/dE,Emax/dE - 1)];
            
            LOGGER_EXIT_FUNCTION2("double msMotion::SOS( double E )");
            return sos;
        };
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        /*
         double msMotion::DOS( double E )
         {
         LOGGER_ENTER_FUNCTION("double msMotion::DOS( double E )",getFullId());
         double dos;
         try{ getParameters();
         }
         catch(...)
         {throw;
         }
         
         if( E > Emax ) LOGGER_WRITE(msLogger::WARNING,"E out of [0-Emax] bound");
         
         if( (DensOfStates->size()==0) || (E > Emax) )  dos=DOS(E);
         else dos = (*DensOfStates)[min( E/dE,Emax/dE - 1)];
         
         LOGGER_EXIT_FUNCTION();
         return dos;
         }*/
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msMotion::fillSOSfromDOS(){
            
            (*SumOfStates)[0]=(*DensOfStates)[0];
            
            for(size_t i=1;i<DensOfStates->size(); i++)
                
                (*SumOfStates)[i]=(*SumOfStates)[i-1]+(*DensOfStates)[i]*dE;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotion::dlogQ_dT_Vcst(double T)  {
            
            LOGGER_ENTER_FUNCTION("double msMotion::dlogQ_dT_Vcst(double T)",getFullId());
            LOGGER_EXIT_FUNCTION2("double msMotion::dlogQ_dT_Vcst(double T)");
            return ( log( Q(T+dT) )-log( Q(T) ) )/(dT);
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msMotion::FillEvAndG(vector<double>& EigenValues,vector<double>& g) {
            
            LOGGER_ENTER_FUNCTION("void msMotion::FillEvAndG(vector<double>& EigenValues,vector<double>& g)",getFullId());
            int nEV=Emax/dE;
            EigenValues.resize(nEV,0); g.resize(nEV,0);
            
            if( DensOfStates->size()==0 ) computeDOS();
            
            for(int i=0;i<nEV;i++){ EigenValues[i]=i*dE; g[i]=(*DensOfStates)[i]*dE;
            }
            LOGGER_EXIT_FUNCTION2("void msMotion::FillEvAndG(vector<double>& EigenValues,vector<double>& g)");
        }
        
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msMotion::computeDOS()  {
            
            LOGGER_ENTER_FUNCTION("void msMotion::computeDOS()",getFullId());
            
            LOGGER_WRITE(msLogger::INFO, "Compute the density of states in [0-"+output::getString<double>(Emax)
                         +";delta="+output::getString<double>(dE_dos)+"] in "+getUnits()->getEnergyStr());
            
            int n=int(( Emax+dE)/dE_dos)+1;
            std::vector<double> e(n,0);
            std::vector<double> dos(n,0);
            LOGGER_HEADER2COLUMNS(msLogger::INFO,"E", getUnits()->getEnergyStr(), "DOS", getUnits()->getEnergyStr()+"^-1");
            
            for(int i=1; i<n ; i++) {
                
                dos[i]=DOS( e[i] = i*dE_dos );
                LOGGER_WRITE2COLUMNS(msLogger::INFO,e[i],dos[i]);
            }
            
            LOGGER_WRITE(msLogger::INFO, "fit the result using quadratic function");
            
            
            LOGGER_EXIT_FUNCTION2("void msMotion::computeDOS()");
        };
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotion::QfromDOS(double T) {
            
            LOGGER_ENTER_FUNCTION("void msMotion::QfromDOS(double T)",getFullId());
            
            if( DensOfStates->size() == 0 ) {  msMotion::computeDOS();
                fillSOSfromDOS();
            }
            double Q = QfromDOS( *DensOfStates ,  T );
            LOGGER_EXIT_FUNCTION2("void msMotion::QfromDOS(double T)");
            return Q;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotion::QfromDOS( msScalarFunction& DOS , double T ) {
            
            LOGGER_ENTER_FUNCTION("void msMotion::QfromDOS( msScalarFunction& DOS  , double T)",getFullId());
            
            int n    = Emax/dE;
            double Q = 0;
            double R = getUnits()->convert( msUnit::J_mol,  csts::R );
            double convDos = getUnits()->convert( DOS.getYUnit(), 1 );
            
            double dE = DOS.getVariable(0)->getDq(*getUnits());
            
            LOGGER_WRITE(msLogger::INFO, "Compute partition function from the density of state. [0-"+output::getString<double>(Emax)
                         +";delta="+output::getString<double>(dE)+"] in "+getUnits()->getEnergyStr());
            
            if( DOS.isDerivedFrom("msVectorFit1d") ){
                
                msVectorFit1d& dos = *( (msVectorFit1d*) &DOS);
                
                for( size_t i=0; i< dos.size() ; i++) Q += convDos * dos[i] * exp(- ( i*dE + dE/2. ) / (R * T) ) * dE;
                LOGGER_EXIT_FUNCTION2("void msMotion::QfromDOS( msScalarFunction& DOS  , double T)");
                return Q;
            }
            
            
            for( size_t i=0; i<n ; i++){
                
                double E = i*dE + dE/2.;
                if(  DOS(E) ==  DOS(E) ) Q += convDos * DOS(E) * exp(- E / (R * T) ) * dE;
            }
            
            LOGGER_WRITE(msLogger::INFO, " Contribution to Q at E=Emax:"
                         + output::getString<double>(DOS(n*dE)*exp(-(n-1)*dE/ (R * T) )));
            
            LOGGER_EXIT_FUNCTION2("void msMotion::QfromDOS( msScalarFunction& DOS  , double T)");
            return Q;
        }
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        /*
         void msMotion::FillSumFromDens()
         {
         double dE	= Parameters->getDoubleValue("dE",getUnits());
         
         SumOfStates.resize(DensOfStates.size(),0);
         
         (*SumOfStates)[0]=(*DensOfStates)[0]* dE;
         
         for( size_t i=1 ; i<DensOfStates.size(); i++) (*SumOfStates)[i] = (*SumOfStates)[i-1] + (*DensOfStates)[i] * dE ;
         
         }*/
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotion::Eav(double T)
        {
            double R  = getUnits()->convert( msUnit::J_mol,  csts::R );
            double e=R*T*T*dlogQ_dT_Vcst(T);
            return e;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotion::S(double T)
        {
            double R  = getUnits()->convert( msUnit::J_mol,  csts::R );
            double S  = R*log( Q(T) ) + Eav(T)/T;
            return S;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotion::Cv(double T)
        {
            LOGGER_ENTER_FUNCTION("void msMotion::Cv(double T)",getFullId());
            double Cv=(Eav(T+dT)-Eav(T))/dT;
            LOGGER_EXIT_FUNCTION2("void msMotion::Cv(double T)");
            return Cv;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msMotion::Cp(double T)
        {
            return( Cv(T) +  getUnits()->convert( msUnit::J_mol,  csts::R ) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& msMotion::print(std::ostream& out) const {
            
            msPhysicalInterface::print(out);
            output::printHeader(out,"Motion");
            
            out<<"Symetry: "<<Sym<<"; multiplicity: "<<Mult<<endl;
            return out;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
    }
    
}

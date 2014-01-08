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

#include "msIntegrandLagrangian.h"
#include "msLagrangian.h"

namespace impact {
    
    namespace atomism {
        
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        bool msQIntegrand::isQIntegrandRegisteredInPython=0;
        msRegistrar msQIntegrand::Registrar("QIntegrand", msQIntegrand::createInstance);
        
        void msQIntegrand::registryInPython()
        {
#if USE_PYTHON
            msScalarFunction::registryInPython();
            
            if( ! msQIntegrand::isQIntegrandRegisteredInPython )
            {
                using namespace boost::python;
                
                class_<msQIntegrand,bases<msScalarFunction>,boost::shared_ptr<msQIntegrand> >
                ("msQIntegrand",
                 "Defines a partition function integrand. ",no_init )
                .def( "New", &msQIntegrand::New , "Create a new object")
                .staticmethod("New");
                msQIntegrand::isQIntegrandRegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        msQIntegrand::msQIntegrand() : msScalarFunction() {
            
            constructVar("msQIntegrand","msQIntegrand","msQIntegrand");
            system = 0; T=0;
        }
        
        boost::shared_ptr<msTreeMapper> msQIntegrand::setSystemAndTemperature(const msLagrangian* sys,double temp)
        {
            system = sys;   T = temp;
            R     = system->getUnits()->convert( msUnit::J_mol , csts::R  );
            Econv = system->getUnits()->convert( system->getEpot()->getYUnit(), 1);
            
            fact   = pow( 2*csts::pi*double(csts::kb)*T , double(system->getGeneralizedCoordinates()->getIntegrationDim()/2.) )
            / pow( double(csts::h) ,system->getGeneralizedCoordinates()->getIntegrationDim() );
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        
        double msQIntegrand::evaluate()
        {
            double V   =  system->getEpot()->evaluate() * Econv;
            double Kin =  system->getKineticOperator()->kineticFunction( SiUnits );
            double r   =  fact * exp( - V  /( R * T )) * sqrt( Kin );
            return r;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        bool msQbyObservable::isQbyObservableRegisteredInPython=0;
        msRegistrar msQbyObservable::Registrar("QbyObservable", msQbyObservable::createInstance);
        
        void msQbyObservable::registryInPython()
        {
#if USE_PYTHON
            msQIntegrand::registryInPython();
            
            if( ! msQbyObservable::isQbyObservableRegisteredInPython )
            {
                using namespace boost::python;
                
                class_<msQbyObservable,bases<msQIntegrand>,boost::shared_ptr<msQbyObservable> >
                ("msQbyObservable",
                 "Defines a partition function integrand. ",no_init )
                .def( "New", &msQbyObservable::New , "Create a new object")
                .staticmethod("New");
                msQbyObservable::isQbyObservableRegisteredInPython=1;
            }
#endif
        }
        
        msQbyObservable::msQbyObservable() : msQIntegrand() {
            
            constructVar("msQbyObservable","msQbyObservable","msQbyObservable");
            Observable = 0; sumQ=0;
        }
        
        boost::shared_ptr<msTreeMapper> msQbyObservable::setObservable(msObservable* observable) {
            
            Observable = observable;
            return mySharedPtr();
        }
        
        double msQbyObservable::evaluate()
        {
            double q   = msQIntegrand::evaluate();
            sumQ 	   += q;
            double obs = (*Observable)();
            return q * obs;
        }
        
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        
        bool msDOSIntegrand::isDOSIntegrandRegisteredInPython=0;
        msRegistrar msDOSIntegrand::Registrar("DOSIntegrand", msDOSIntegrand::createInstance);
        
        void msDOSIntegrand::registryInPython()
        {
#if USE_PYTHON
            msScalarFunction::registryInPython();
            
            if( ! msDOSIntegrand::isDOSIntegrandRegisteredInPython )
            {
                using namespace boost::python;
                
                class_<msDOSIntegrand,bases<msScalarFunction>,boost::shared_ptr<msDOSIntegrand> >
                ("msDOSIntegrand",
                 "Defines a DOS integrand. ",no_init )
                .def( "New", &msDOSIntegrand::New , "Create a new object")
                .staticmethod("New");
                msDOSIntegrand::isDOSIntegrandRegisteredInPython=1;
            }
#endif
        }
        
        msDOSIntegrand::msDOSIntegrand()
        {
            constructVar("msDOSIntegrand","DOSIntegrand","DOSIntegrand");
            system = 0; E=0;
        }
        
        boost::shared_ptr<msTreeMapper> msDOSIntegrand::setSystemAndEnergy(const msLagrangian* sys,double ener)
        {
            system  = sys;
            Econv   = system->getUnits()->convert( system->getEpot()->getYUnit() , 1 );
            EconvSi = SiUnits.convert( system->getUnits()->getEnergy() , 1 );
            
            ndof = system->getGeneralizedCoordinates()->getIntegrationDim( );
            E    = ener;
            emin = system->getSampler()->sample(*(system->getEpot())).Min;
            fact = pow( 2*csts::pi / pow( double(csts::h) ,2 ), double(ndof/2.) )
            / Gamma(ndof/2.);
            
            return mySharedPtr();
        }
        //-------------------------------------------------------------------------------------------------
        
        double msDOSIntegrand::evaluate()
        {
            double Kin =  system->getKineticOperator()->kineticFunction( SiUnits );
            double V   =  system->getEpot()->evaluate() * Econv ; // in System->UserUnits
            
            if(V<emin) LOGGER_WRITE_ALONE(msLogger::WARNING,"e < emin","double msDOSIntegrand::Evaluate() ");
            
            double DOS=0;
            if( E - ( V - emin) > E/1000.)
                DOS = fact * sqrt(Kin) * pow( ( E - ( V - emin) ) * EconvSi, ndof/2.-1);
            
            return DOS;
        }
        
        
    }
    
}

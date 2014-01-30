/*
 <one line to give the library's name and an idea of what it does.>
 Copyright (C) 2012  Guillaume <email>
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "msReactionRate.h"

namespace impact {
    
    namespace antioch {
        
      
        bool msReactionRate::isReactionRateRegisteredInPython = 0;
        msRegistrar msReactionRate::Registrar("msReactionRate", msReactionRate::createInstance);
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msReactionRate::registryInPython() {
            
#if USE_PYTHON
            
            msPhysicalInterface::registryInPython();
            
            if( ! msReactionRate::isReactionRateRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msReactionRate,bases<msPhysicalInterface>,boost::shared_ptr<msReactionRate> >
                ("msReactionRate",
                 "Object describing a kinetic model for reaction",no_init
                 )
                .def( "New", &msReactionRate::New ,
                     "Create a new object.")
                .staticmethod("New")
                .def("getCoefficients",&msReactionRate::getCoefficients,
                     "return the coefficients of the law")
                .def("setCoefficient",&msReactionRate::setCoefficient,
                     "set a coefficient. arg2: name; arg3: value")
		.def("computeRate",&msReactionRate::computeRate,
                     "compute the rate")
		.def("setOrder",&msReactionRate::setOrder,
		     "set the order of the reaction rate. arg2: order")
		.def("getOrder",&msReactionRate::getOrder,
		     "return the order of the reaction rate.")
		.def("getUnit",&msReactionRate::getUnit,
		     "return the unit of the rate.");
                
		msTreeMapper::finalizeDeclarationForPython<msReactionRate>("msReactionRate");
                msReactionRate::isReactionRateRegisteredInPython  = 1;
                }
#endif
	}
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msReactionRateDerived<Antioch::ArrheniusRate<double> >::isReactionRateDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msReactionRateDerived<Antioch::ArrheniusRate<double> >::Registrar(
	    "msArrheniusRate", msReactionRateDerived<Antioch::ArrheniusRate<double> >::createInstance);
        template<>
        string msReactionRateDerived<Antioch::ArrheniusRate<double> >::doc = "rate of Arrhenius format";
        template<>
        string msReactionRateDerived<Antioch::ArrheniusRate<double> >::nameType = "msArrheniusRate";      
	
        template<>
        void msReactionRateDerived<Antioch::ArrheniusRate<double> >::initialize() { 
	      
	        msReactionRate::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::ArrheniusRate<double>() );
		
		declareRateCoefficient("Cf", & Antioch::ArrheniusRate<double>::set_Cf,
				             & Antioch::ArrheniusRate<double>::Cf, 
			                     getUnit() );
		declareRateCoefficient("Ea", & Antioch::ArrheniusRate<double>::set_Ea,
				             & Antioch::ArrheniusRate<double>::Ea,
			                     getUnits()->getUnit(msUnit::vEnergyByQuantity) );
            }
 
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msReactionRateDerived<Antioch::KooijRate<double> >::isReactionRateDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msReactionRateDerived<Antioch::KooijRate<double> >::Registrar(
	    "msKooijRate", msReactionRateDerived<Antioch::KooijRate<double> >::createInstance);
        template<>
        string msReactionRateDerived<Antioch::KooijRate<double> >::doc = "KoojRate";
        template<>
        string msReactionRateDerived<Antioch::KooijRate<double> >::nameType = "rate of Kooij format";      
	
        template<>
        void msReactionRateDerived<Antioch::KooijRate<double> >::initialize() { 
	      
	        msReactionRate::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::KooijRate<double>() );
		
		declareRateCoefficient("Cf", & Antioch::KooijRate<double>::set_Cf,
				             & Antioch::KooijRate<double>::Cf, 
			                     getUnit() );
		declareRateCoefficient("Ea", & Antioch::KooijRate<double>::set_Ea,
				             & Antioch::KooijRate<double>::Ea,
			                     getUnits()->getUnit(msUnit::vEnergyByQuantity) );
		declareRateCoefficient("eta",& Antioch::KooijRate<double>::set_eta,
				             & Antioch::KooijRate<double>::eta,
			                     getUnits()->getUnit(msUnit::vNone) );
            }
            
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msReactionRateDerived<Antioch::BerthelotRate<double> >::isReactionRateDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msReactionRateDerived<Antioch::BerthelotRate<double> >::Registrar(
	    "msBerthelotRate", 
	    msReactionRateDerived<Antioch::BerthelotRate<double> >::createInstance);
        template<>
        string msReactionRateDerived<Antioch::BerthelotRate<double> >::doc = "BerthelotRate";
        template<>
        string msReactionRateDerived<Antioch::BerthelotRate<double> >::nameType = "msBerthelotRate";      
	
	
        template<>
        void msReactionRateDerived<Antioch::BerthelotRate<double> >::initialize() { 
	      
	        msReactionRate::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::BerthelotRate<double>() );
		
		declareRateCoefficient("Cf", & Antioch::BerthelotRate<double>::set_Cf,
				             & Antioch::BerthelotRate<double>::Cf, 
			                     getUnit() );
		declareRateCoefficient("D",  & Antioch::BerthelotRate<double>::set_D,
				             & Antioch::BerthelotRate<double>::D,
			                     msUnit("K^-1"));
            }
            
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msReactionRateDerived<Antioch::HercourtEssenRate<double> >::isReactionRateDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msReactionRateDerived<Antioch::HercourtEssenRate<double> >::Registrar(
	    "msHercourtEssenRate", 
	    msReactionRateDerived<Antioch::HercourtEssenRate<double> >::createInstance);
        template<>
        string msReactionRateDerived<Antioch::HercourtEssenRate<double> >::doc = "HercourtEssenRate";
        template<>
        string msReactionRateDerived<Antioch::HercourtEssenRate<double> >::nameType = "msHercourtEssenRate";      
	
	
        template<>
        void msReactionRateDerived<Antioch::HercourtEssenRate<double> >::initialize() { 
	      
	        msReactionRate::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::HercourtEssenRate<double>() );
		
		declareRateCoefficient("Cf",   & Antioch::HercourtEssenRate<double>::set_Cf,
				               & Antioch::HercourtEssenRate<double>::Cf, 
			                       getUnit() );
		declareRateCoefficient("Tref", & Antioch::HercourtEssenRate<double>::set_Tref,
				               & Antioch::HercourtEssenRate<double>::Tref,
			                       msUnit("K"));
		declareRateCoefficient("eta",  & Antioch::HercourtEssenRate<double>::set_eta,
				               & Antioch::HercourtEssenRate<double>::eta,
			                       getUnits()->getUnit(msUnit::vNone) );
	}
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msReactionRateDerived<Antioch::BerthelotHercourtEssenRate<double> >::isReactionRateDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msReactionRateDerived<Antioch::BerthelotHercourtEssenRate<double> >::Registrar(
	    "msBerthelotHercourtEssenRate", 
	    msReactionRateDerived<Antioch::BerthelotHercourtEssenRate<double> >::createInstance);
        template<>
        string msReactionRateDerived<Antioch::BerthelotHercourtEssenRate<double> >::doc = "BerthelotHercourtEssenRate";
        template<>
        string msReactionRateDerived<Antioch::BerthelotHercourtEssenRate<double> >::nameType = "msBerthelotHercourtEssenRate";      
	
        template<>
        void msReactionRateDerived<Antioch::BerthelotHercourtEssenRate<double> >::initialize() { 
	      
	  
	        msReactionRate::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::BerthelotHercourtEssenRate<double>() );
		
		declareRateCoefficient("Cf",   & Antioch::BerthelotHercourtEssenRate<double>::set_Cf,
				               & Antioch::BerthelotHercourtEssenRate<double>::Cf, 
			                       getUnit() );
		declareRateCoefficient("Tref", & Antioch::BerthelotHercourtEssenRate<double>::set_Tref,
				               & Antioch::BerthelotHercourtEssenRate<double>::Tref,
			                       msUnit("K"));
		declareRateCoefficient("eta",  & Antioch::BerthelotHercourtEssenRate<double>::set_eta,
				               & Antioch::BerthelotHercourtEssenRate<double>::eta,
			                       getUnits()->getUnit(msUnit::vNone) );
		declareRateCoefficient("D",    & Antioch::BerthelotHercourtEssenRate<double>::set_D,
				               & Antioch::BerthelotHercourtEssenRate<double>::D,
			                       msUnit("K^-1"));
	}
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msReactionRateDerived<Antioch::VantHoffRate<double> >::isReactionRateDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msReactionRateDerived<Antioch::VantHoffRate<double> >::Registrar(
	    "msVantHoffRate", 
	    msReactionRateDerived<Antioch::VantHoffRate<double> >::createInstance);
        template<>
        string msReactionRateDerived<Antioch::VantHoffRate<double> >::doc = "VantHoffRate";
        template<>
        string msReactionRateDerived<Antioch::VantHoffRate<double> >::nameType = "msVantHoffRate";      
	
	
        template<>
        void msReactionRateDerived<Antioch::VantHoffRate<double> >::initialize() { 
	      
	  
	   	msReactionRate::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::VantHoffRate<double>() );
		
		declareRateCoefficient("Cf",   & Antioch::VantHoffRate<double>::set_Cf,
				               & Antioch::VantHoffRate<double>::Cf, 
			                       getUnit() );
		declareRateCoefficient("Tref", & Antioch::VantHoffRate<double>::set_Tref,
				               & Antioch::VantHoffRate<double>::Tref,
			                       msUnit("K"));
		declareRateCoefficient("eta",  & Antioch::VantHoffRate<double>::set_eta,
				               & Antioch::VantHoffRate<double>::eta,
			                       getUnits()->getUnit(msUnit::vNone) );
		declareRateCoefficient("D",    & Antioch::VantHoffRate<double>::set_D,
				               & Antioch::VantHoffRate<double>::D,
			                       msUnit("K^-1"));
		declareRateCoefficient("Ea",   & Antioch::VantHoffRate<double>::set_Ea,
				               & Antioch::VantHoffRate<double>::Ea,
			                       getUnits()->getUnit(msUnit::vEnergyByQuantity) );
	}
    }
}
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
	    "msArrheniusRate", 
	    msReactionRateDerived<Antioch::ArrheniusRate<double> >::createInstance);
        template<>
        string msReactionRateDerived<Antioch::ArrheniusRate<double> >::doc = "ArrheniusRate";
        template<>
        string msReactionRateDerived<Antioch::ArrheniusRate<double> >::nameType = "msArrheniusRate";      
	
	
        template<>
        void msReactionRateDerived<Antioch::ArrheniusRate<double> >::initialize() { 
	      
	        msReactionRate::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::ArrheniusRate<double>() );
		
		Coefficients["Cf"] =  0;
		Coefficients["Ea"] =  0;
		
		msPhysicalInterface::declarePhysicalVariable(getUnit(),&(Coefficients["Cf"]),"Cf");
		msPhysicalInterface::declarePhysicalVariable(getUnits()->getUnit(msUnit::vEnergyByQuantity),&(Coefficients["Ea"]),"Ea");
            }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msReactionRateDerived<Antioch::ArrheniusRate<double> >::setCoefficient(string name,double value) {
	  
	      if( name =="Cf" ) { getCastedCalculator<Antioch::ArrheniusRate<double> >()->set_Cf(getUnits()->convertTo(value,getUnitCalculator()));
	      }
	      else
	      if( name =="Ea" ) { getCastedCalculator<Antioch::ArrheniusRate<double> >()->set_Ea(getUnits()->convertTo(value,msUnit("J.kmol^-1")));
	      }     
	      else {
		msException e("Coefficient "+name+" not recognized",
			  " msReactionRateDerived<Antioch::ArrheniusRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		IMPACT_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
	}
	
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msReactionRateDerived<Antioch::KooijRate<double> >::isReactionRateDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msReactionRateDerived<Antioch::KooijRate<double> >::Registrar(
	    "msKooijRate", 
	    msReactionRateDerived<Antioch::KooijRate<double> >::createInstance);
        template<>
        string msReactionRateDerived<Antioch::KooijRate<double> >::doc = "KoojRate";
        template<>
        string msReactionRateDerived<Antioch::KooijRate<double> >::nameType = "msKoojRate";      
	
	
        template<>
        void msReactionRateDerived<Antioch::KooijRate<double> >::initialize() { 
	      
	        msReactionRate::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::KooijRate<double>() );
		
		Coefficients["Cf"] =  0;
		Coefficients["Ea"] =  0;
		Coefficients["eta"]=  0;
		
		msPhysicalInterface::declarePhysicalVariable(getUnit(),&(Coefficients["Cf"]),"Cf");
		msPhysicalInterface::declarePhysicalVariable(getUnits()->getUnit(msUnit::vEnergyByQuantity),&(Coefficients["Ea"]),"Ea");
            }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msReactionRateDerived<Antioch::KooijRate<double> >::setCoefficient(string name,double value) {
	  
	  
	      if( name =="Cf" ) { 
		getCastedCalculator<Antioch::KooijRate<double> >()->set_Cf(getUnits()->convertTo(value,getUnitCalculator()));
	      }
	      else
	      if( name =="Ea" ) { 
		double Ea = value / getUnits()->convert(msUnit("J.mol^-1.K^-1"),csts::R);
		getCastedCalculator<Antioch::KooijRate<double> >()->set_Ea(Ea);
	      }
	      else
	      if( name =="eta" ) { getCastedCalculator<Antioch::KooijRate<double> >()->set_eta(value);
	      }	      
	      else {
		msException e("Coefficient "+name+" not recognized",
			  " msReactionRateDerived<Antioch::KooijRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		IMPACT_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
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
		
		Coefficients["Cf"] =  0;
		Coefficients["D"]  =  0;
		
		msPhysicalInterface::declarePhysicalVariable(getUnit(),&(Coefficients["Cf"]),"Cf");
		msPhysicalInterface::declarePhysicalVariable(msUnit("K^-1"),&(Coefficients["D"]),"D");
            }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msReactionRateDerived<Antioch::BerthelotRate<double> >::setCoefficient(string name,double value) {
	  
	      if( name =="Cf" ) { getCastedCalculator<Antioch::BerthelotRate<double> >()->set_Cf(getUnits()->convertTo(value,getUnitCalculator()));
	      }
	      else
	      if( name =="D" ) { getCastedCalculator<Antioch::BerthelotRate<double> >()->set_D(getUnits()->convertTo(value,msUnit("K^-1")));
	      }    
	      else {
		msException e("Coefficient "+name+" not recognized",
			  " msReactionRateDerived<Antioch::BerthelotRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		IMPACT_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
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
		
		Coefficients["Cf"]    =  0;
		Coefficients["eta"]   =  0;
		Coefficients["Tref"]  =  0;
		
		msPhysicalInterface::declarePhysicalVariable(getUnit(),&(Coefficients["Cf"]),"Cf");
		msPhysicalInterface::declarePhysicalVariable(msUnit("K"),&(Coefficients["Tref"]),"Tref");
            }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msReactionRateDerived<Antioch::HercourtEssenRate<double> >::setCoefficient(string name,double value) {
	  
	      if( name =="Cf" ) { getCastedCalculator<Antioch::HercourtEssenRate<double> >()->set_Cf(getUnits()->convertTo(value,getUnitCalculator()));
	      }
	      else
	      if( name =="eta" ) { getCastedCalculator<Antioch::HercourtEssenRate<double> >()->set_eta(value);
	      }    
	      else
	      if( name =="Tref" ) { getCastedCalculator<Antioch::HercourtEssenRate<double> >()->set_Tref(getUnits()->convertTo(value,msUnit("K")));
	      }  
	      else {
		msException e("Coefficient "+name+" not recognized",
			  " msReactionRateDerived<Antioch::HercourtEssenRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		IMPACT_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
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
		
		Coefficients["Cf"]    =  0;
		Coefficients["eta"]   =  0;
		Coefficients["D"]  =  0;
		Coefficients["Tref"]  =  0;
		
		msPhysicalInterface::declarePhysicalVariable(getUnit(),&(Coefficients["Cf"]),"Cf");
		msPhysicalInterface::declarePhysicalVariable(msUnit("K"),&(Coefficients["Tref"]),"Tref");
		msPhysicalInterface::declarePhysicalVariable(msUnit("K^-1"),&(Coefficients["D"]),"D");
            }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msReactionRateDerived<Antioch::BerthelotHercourtEssenRate<double> >::setCoefficient(string name,double value) {
	  
	      if( name =="Cf" ) { getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->set_Cf(getUnits()->convertTo(value,getUnitCalculator()));
	      }
	      else
	      if( name =="eta" ) { getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->set_eta(value);
	      }    
	      else
	      if( name =="D" ) {  getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->set_D(getUnits()->convertTo(value,msUnit("K^-1")));
	      }  
	      	      else
	      if( name =="Tref" ) { getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->set_Tref(getUnits()->convertTo(value,msUnit("K")));
	      }  
	      else {
		msException e("Coefficient "+name+" not recognized",
			  " msReactionRateDerived<Antioch::BerthelotHercourtEssenRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		IMPACT_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
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
		
		Coefficients["Cf"]    =  0;
		Coefficients["eta"]   =  0;
		Coefficients["Ea"]   =  0;
		Coefficients["D"]  =  0;
		Coefficients["Tref"]  =  0;
		
		msPhysicalInterface::declarePhysicalVariable(getUnit(),&(Coefficients["Cf"]),"Cf");
		msPhysicalInterface::declarePhysicalVariable(msUnit("K"),&(Coefficients["Tref"]),"Tref");
		msPhysicalInterface::declarePhysicalVariable(msUnit("K^-1"),&(Coefficients["D"]),"D");
		msPhysicalInterface::declarePhysicalVariable(getUnits()->getUnit(msUnit::vEnergyByQuantity),&(Coefficients["Ea"]),"Ea");
        }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msReactionRateDerived<Antioch::VantHoffRate<double> >::setCoefficient(string name,double value) {
	  
	      if( name =="Cf" ) { getCastedCalculator<Antioch::VantHoffRate<double> >()->set_Cf(getUnits()->convertTo(value,getUnitCalculator()));
	      }
	      else
	      if( name =="eta" ) { getCastedCalculator<Antioch::VantHoffRate<double> >()->set_eta(value);
	      }    
	      else
	      if( name =="Ea" ) { getCastedCalculator<Antioch::VantHoffRate<double> >()->set_Ea(getUnits()->convertTo(value,msUnit("J.kmol^-1")));
	      }  
	      else
	      if( name =="D" ) { getCastedCalculator<Antioch::VantHoffRate<double> >()->set_D(getUnits()->convertTo(value,msUnit("K^-1")));
	      }  
	      	      else
	      if( name =="Tref" ) { getCastedCalculator<Antioch::VantHoffRate<double> >()->set_Tref(getUnits()->convertTo(value,msUnit("K")));
	      }  
	      else {
		msException e("Coefficient "+name+" not recognized",
			  " msReactionRateDerived<Antioch::VantHoffRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		IMPACT_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
	}
	
    }
}
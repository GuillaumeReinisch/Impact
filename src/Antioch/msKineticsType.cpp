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


#include "msKineticsType.h"

namespace impact {
    
    namespace antioch {
        
      
        bool msKineticsType::isKineticsTypeRegisteredInPython = 0;
        msRegistrar msKineticsType::Registrar("msKineticsType", msKineticsType::createInstance);
        
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msKineticsType::registryInPython() {
            
#if USE_PYTHON
            
            msPhysicalInterface::registryInPython();
            
            if( ! msKineticsType::isKineticsTypeRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msKineticsType,bases<msPhysicalInterface>,boost::shared_ptr<msKineticsType> >
                ("msKineticsType",
                 "Object describing a kinetic model for reaction",no_init
                 )
                .def( "New", &msKineticsType::New ,
                     "Create a new object.")
                .staticmethod("New")
                .def("getCoefficients",&msKineticsType::getCoefficients,
                     "return the coefficients of the law")
                .def("setCoefficient",&msKineticsType::setCoefficient,
                     "set a coefficient. arg2: name; arg3: value")
		.def("computeRate",&msKineticsType::computeRate,
                     "compute the rate");
                
                msKineticsType::isKineticsTypeRegisteredInPython  = 1;
                }
#endif
	}
	
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msKineticsTypeDerived<Antioch::ArrheniusRate<double> >::isKineticsTypeDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msKineticsTypeDerived<Antioch::ArrheniusRate<double> >::Registrar(
	    "msArrheniusRate", 
	    msKineticsTypeDerived<Antioch::ArrheniusRate<double> >::createInstance);
        template<>
        string msKineticsTypeDerived<Antioch::ArrheniusRate<double> >::doc = "ArrheniusRate";
        template<>
        string msKineticsTypeDerived<Antioch::ArrheniusRate<double> >::nameType = "msArrheniusRate";      
	
	
        template<>
        void msKineticsTypeDerived<Antioch::ArrheniusRate<double> >::initialize() { 
	      
	        msKineticsType::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::ArrheniusRate<double>() );
		msTreeMapper::declareAttribute(Order,"Order");
		Coefficients["Cf"] = getCastedCalculator<Antioch::ArrheniusRate<double> >()->Cf();
		Coefficients["Ea"] = getCastedCalculator<Antioch::ArrheniusRate<double> >()->Ea();
            }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msKineticsTypeDerived<Antioch::ArrheniusRate<double> >::setCoefficient(string name,double value) {
	  
	      if( name =="Cf" ) { getCastedCalculator<Antioch::ArrheniusRate<double> >()->set_Cf(value);
	      }
	      else
	      if( name =="Ea" ) { getCastedCalculator<Antioch::ArrheniusRate<double> >()->set_Ea(value);
	      }      
	      else {
		msError e("Coefficient "+name+" not recognized",
			  " msKineticsTypeDerived<Antioch::ArrheniusRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		BOOST_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
	}
	
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msKineticsTypeDerived<Antioch::KooijRate<double> >::isKineticsTypeDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msKineticsTypeDerived<Antioch::KooijRate<double> >::Registrar(
	    "msKooijRate", 
	    msKineticsTypeDerived<Antioch::KooijRate<double> >::createInstance);
        template<>
        string msKineticsTypeDerived<Antioch::KooijRate<double> >::doc = "KoojRate";
        template<>
        string msKineticsTypeDerived<Antioch::KooijRate<double> >::nameType = "msKoojRate";      
	
	
        template<>
        void msKineticsTypeDerived<Antioch::KooijRate<double> >::initialize() { 
	      
	        msKineticsType::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::KooijRate<double>() );
		msTreeMapper::declareAttribute(Order,"Order");
		Coefficients["Cf"] = getCastedCalculator<Antioch::KooijRate<double> >()->Cf();
		Coefficients["Ea"] = getCastedCalculator<Antioch::KooijRate<double> >()->Ea();
		Coefficients["eta"]= getCastedCalculator<Antioch::KooijRate<double> >()->eta();
            }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msKineticsTypeDerived<Antioch::KooijRate<double> >::setCoefficient(string name,double value) {
	  
	      if( name =="Cf" ) { getCastedCalculator<Antioch::KooijRate<double> >()->set_Cf(value);
	      }
	      else
	      if( name =="Ea" ) { getCastedCalculator<Antioch::KooijRate<double> >()->set_Ea(value);
	      }
	      else
	      if( name =="eta" ) { getCastedCalculator<Antioch::KooijRate<double> >()->set_eta(value);
	      }	      
	      else {
		msError e("Coefficient "+name+" not recognized",
			  " msKineticsTypeDerived<Antioch::KooijRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		BOOST_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
	}
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msKineticsTypeDerived<Antioch::BerthelotRate<double> >::isKineticsTypeDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msKineticsTypeDerived<Antioch::BerthelotRate<double> >::Registrar(
	    "msBerthelotRate", 
	    msKineticsTypeDerived<Antioch::BerthelotRate<double> >::createInstance);
        template<>
        string msKineticsTypeDerived<Antioch::BerthelotRate<double> >::doc = "BerthelotRate";
        template<>
        string msKineticsTypeDerived<Antioch::BerthelotRate<double> >::nameType = "msBerthelotRate";      
	
	
        template<>
        void msKineticsTypeDerived<Antioch::BerthelotRate<double> >::initialize() { 
	      
	        msKineticsType::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::BerthelotRate<double>() );
		msTreeMapper::declareAttribute(Order,"Order");
		Coefficients["Cf"] = getCastedCalculator<Antioch::BerthelotRate<double> >()->Cf();
		Coefficients["D"] = getCastedCalculator<Antioch::BerthelotRate<double> >()->D();
            }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msKineticsTypeDerived<Antioch::BerthelotRate<double> >::setCoefficient(string name,double value) {
	  
	      if( name =="Cf" ) { getCastedCalculator<Antioch::BerthelotRate<double> >()->set_Cf(value);
	      }
	      else
	      if( name =="D" ) { getCastedCalculator<Antioch::BerthelotRate<double> >()->set_D(value);
	      }    
	      else {
		msError e("Coefficient "+name+" not recognized",
			  " msKineticsTypeDerived<Antioch::BerthelotRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		BOOST_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
	}
		
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msKineticsTypeDerived<Antioch::HercourtEssenRate<double> >::isKineticsTypeDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msKineticsTypeDerived<Antioch::HercourtEssenRate<double> >::Registrar(
	    "msHercourtEssenRate", 
	    msKineticsTypeDerived<Antioch::HercourtEssenRate<double> >::createInstance);
        template<>
        string msKineticsTypeDerived<Antioch::HercourtEssenRate<double> >::doc = "HercourtEssenRate";
        template<>
        string msKineticsTypeDerived<Antioch::HercourtEssenRate<double> >::nameType = "msHercourtEssenRate";      
	
	
        template<>
        void msKineticsTypeDerived<Antioch::HercourtEssenRate<double> >::initialize() { 
	      
	        msKineticsType::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::HercourtEssenRate<double>() );
		msTreeMapper::declareAttribute(Order,"Order");
		Coefficients["Cf"] = getCastedCalculator<Antioch::HercourtEssenRate<double> >()->Cf();
		Coefficients["eta"] = getCastedCalculator<Antioch::HercourtEssenRate<double> >()->eta();
		Coefficients["Tref"] = getCastedCalculator<Antioch::HercourtEssenRate<double> >()->Tref();
            }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msKineticsTypeDerived<Antioch::HercourtEssenRate<double> >::setCoefficient(string name,double value) {
	  
	      if( name =="Cf" ) { getCastedCalculator<Antioch::HercourtEssenRate<double> >()->set_Cf(value);
	      }
	      else
	      if( name =="eta" ) { getCastedCalculator<Antioch::HercourtEssenRate<double> >()->set_eta(value);
	      }    
	      else
	      if( name =="Tref" ) { getCastedCalculator<Antioch::HercourtEssenRate<double> >()->set_Tref(value);
	      }  
	      else {
		msError e("Coefficient "+name+" not recognized",
			  " msKineticsTypeDerived<Antioch::HercourtEssenRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		BOOST_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
	}
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         
        template<>
        bool msKineticsTypeDerived<Antioch::BerthelotHercourtEssenRate<double> >::isKineticsTypeDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msKineticsTypeDerived<Antioch::BerthelotHercourtEssenRate<double> >::Registrar(
	    "msBerthelotHercourtEssenRate", 
	    msKineticsTypeDerived<Antioch::BerthelotHercourtEssenRate<double> >::createInstance);
        template<>
        string msKineticsTypeDerived<Antioch::BerthelotHercourtEssenRate<double> >::doc = "BerthelotHercourtEssenRate";
        template<>
        string msKineticsTypeDerived<Antioch::BerthelotHercourtEssenRate<double> >::nameType = "msBerthelotHercourtEssenRate";      
	
	
        template<>
        void msKineticsTypeDerived<Antioch::BerthelotHercourtEssenRate<double> >::initialize() { 
	      
	        msKineticsType::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::BerthelotHercourtEssenRate<double>() );
		msTreeMapper::declareAttribute(Order,"Order");
		Coefficients["Cf"] = getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->Cf();
		Coefficients["eta"] = getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->eta();
		Coefficients["D"] = getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->D();
		Coefficients["Tref"] = getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->Tref();
            }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msKineticsTypeDerived<Antioch::BerthelotHercourtEssenRate<double> >::setCoefficient(string name,double value) {
	  
	      if( name =="Cf" ) { getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->set_Cf(value);
	      }
	      else
	      if( name =="eta" ) { getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->set_eta(value);
	      }    
	      else
	      if( name =="D" ) { getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->set_D(value);
	      }  
	      	      else
	      if( name =="Tref" ) { getCastedCalculator<Antioch::BerthelotHercourtEssenRate<double> >()->set_Tref(value);
	      }  
	      else {
		msError e("Coefficient "+name+" not recognized",
			  " msKineticsTypeDerived<Antioch::BerthelotHercourtEssenRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		BOOST_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
	}
	
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         /*
        template<>
        bool msKineticsTypeDerived<Antioch::VantHoffRate<double> >::isKineticsTypeDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msKineticsTypeDerived<Antioch::VantHoffRate<double> >::Registrar(
	    "msVantHoffRate", 
	    msKineticsTypeDerived<Antioch::VantHoffRate<double> >::createInstance);
        template<>
        string msKineticsTypeDerived<Antioch::VantHoffRate<double> >::doc = "VantHoffRate";
        template<>
        string msKineticsTypeDerived<Antioch::VantHoffRate<double> >::nameType = "msVantHoffRate";      
	
	
        template<>
        void msKineticsTypeDerived<Antioch::VantHoffRate<double> >::initialize() { 
	      
	        msKineticsType::initialize();
		Calculator = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::VantHoffRate<double>() );
		msTreeMapper::declareAttribute(Order,"Order");
		Coefficients["Cf"] = getCastedCalculator<Antioch::VantHoffRate<double> >()->Cf();
		Coefficients["eta"] = getCastedCalculator<Antioch::VantHoffRate<double> >()->eta();
		Coefficients["Ea"] = getCastedCalculator<Antioch::VantHoffRate<double> >()->Ea();
		Coefficients["D"] = getCastedCalculator<Antioch::VantHoffRate<double> >()->D();
		Coefficients["Tref"] = getCastedCalculator<Antioch::VantHoffRate<double> >()->Tref();
            }
  
        template<>
        boost::shared_ptr<msTreeMapper>
        msKineticsTypeDerived<Antioch::VantHoffRate<double> >::setCoefficient(string name,double value) {
	  
	      if( name =="Cf" ) { getCastedCalculator<Antioch::VantHoffRate<double> >()->set_Cf(value);
	      }
	      else
	      if( name =="eta" ) { getCastedCalculator<Antioch::VantHoffRate<double> >()->set_eta(value);
	      }    
	      else
	      if( name =="Ea" ) { getCastedCalculator<Antioch::VantHoffRate<double> >()->set_Ea(value);
	      }  
	      else
	      if( name =="D" ) { getCastedCalculator<Antioch::VantHoffRate<double> >()->set_D(value);
	      }  
	      	      else
	      if( name =="Tref" ) { getCastedCalculator<Antioch::VantHoffRate<double> >()->set_Tref(value);
	      }  
	      else {
		msError e("Coefficient "+name+" not recognized",
			  " msKineticsTypeDerived<Antioch::VantHoffRate<double> >::setCoefficient(string name,double value)",
			  getFullId());
		
		BOOST_THROW_EXCEPTION(e);
	      }
	      Coefficients[name] = value;
	      return mySharedPtr();
	}
	*/
    }
}
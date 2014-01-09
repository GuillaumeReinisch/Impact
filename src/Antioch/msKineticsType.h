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


#ifndef PAReactions_H
#define PAReactions_H

#include <cmath>
#include <iomanip>
#include <limits>
#include <vector>

#include "antioch/vector_utils_decl.h"
#include "antioch/vector_utils.h"
#include "antioch/physical_constants.h"
#include "antioch/chemical_species.h"
#include "antioch/kinetics_type.h"
#include "antioch/kooij_rate.h"
#include "antioch/berthelot_rate.h"
#include "antioch/berthelothercourtessen_rate.h"
#include "antioch/arrhenius_rate.h"
#include "antioch/hercourtessen_rate.h"
#include "antioch/vanthoff_rate.h"
#include <msPhysicalInterface.h>

namespace impact {
    
    namespace antioch {
        
      class msKineticsType : public msPhysicalInterface
        {
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isKineticsTypeRegisteredInPython;
            static msRegister* createInstance() { return new msKineticsType; }
            //@}
            
        protected:
            
            void registryInPython();
            
        public:
            
            msKineticsType() : msPhysicalInterface() {
	      
	        constructVar("msKineticsType","KineticsType","kinetic model for reaction rate"); 
	    }
	    
            void initialize() { 
	      
	        msPhysicalInterface::initialize();
		msTreeMapper::declareAttribute(Coefficients,"Coefficients");
            }
            
            void update()     { 
	        
	        msPhysicalInterface::update();
            }
            
            static boost::shared_ptr<msKineticsType> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msKineticsType> msKineticsType::New()","");
                boost::shared_ptr<msKineticsType> T( new msKineticsType() );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msKineticsType> msKineticsType::New()");
                return T;
            }
            
            boost::shared_ptr<Antioch::KineticsType<double> > getCalculator() {
	      
	        return Calculator;
	    }
	    virtual boost::shared_ptr<msTreeMapper> setCoefficient(string name,double value) {
	      
	        throwNotImplemented("virtual boost::shared_ptr<msTreeMapper> msKineticsType::setCoefficient(string name,double value)");
	    } 
	    virtual double computeRate(double T) {
	      
	        throwNotImplemented("virtual double msKineticsType::computeRate()");
		return -1;
	    }
	    
	    template<class T>
	    boost::shared_ptr<T> getCastedCalculator() {
	      
	        return boost::static_pointer_cast<T >(Calculator);
	    }
	    
	    map<string,double> getCoefficients(){ 
	     
	        return Coefficients;
	    };
	    
	    virtual double getOrder() const{ 
	      throwNotImplemented("virtual double msKineticsType::getOrder() const"); 
	    };
	    
	    virtual boost::shared_ptr<msTreeMapper> setOrder(size_t value){ 
	      throwNotImplemented("virtual boost::shared_ptr<msTreeMapper> msKineticsType::setOrder(size_t value)"); 
	    };
	    /*
	    virtual boost::shared_ptr<Antioch::KineticsType<double> > getCalculator(){ 
	      throwNotImplemented("virtual boost::shared_ptr<Antioch::KineticsType<double> > msKineticsType::getCalculator()"); 
	    };
	    */
	protected:
	  
	    virtual boost::shared_ptr<msTreeMapper> setCalculator(boost::shared_ptr<Antioch::KineticsType<double> > calculator){ 
	      throwNotImplemented("virtual boost::shared_ptr<msTreeMapper> msKineticsType::setCalculator(boost::shared_ptr<Antioch::KineticsType<double> > calculator)"); 
	    };
	    
	    map<string, double> Coefficients;
	    
	    boost::shared_ptr<Antioch::KineticsType<double> > Calculator;
	    
	    void throwNotImplemented(string fctName) const {
	      
	        msError e("The class msReactionBase if virtual",fctName,getFullId());
		BOOST_THROW_EXCEPTION(e);
	    }
        };
	
	
	
	template<class AntiochCalculatorType>
        class msKineticsTypeDerived: public msKineticsType
        {
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isKineticsTypeDerivedRegisteredInPython;
            static msRegister* createInstance() {  new msKineticsTypeDerived<AntiochCalculatorType>; }
            
            static string nameType;
	    static string doc;
            //@}
            
        protected:
            
            void registryInPython() {
            
#if USE_PYTHON
                msKineticsType::registryInPython();
            
                if( ! msKineticsTypeDerived::isKineticsTypeDerivedRegisteredInPython ) {
                
                    using namespace boost::python;
                
                    class_< msKineticsTypeDerived<AntiochCalculatorType> ,
		           bases<msKineticsType>,
			   boost::shared_ptr<msKineticsTypeDerived<AntiochCalculatorType> > 
			   >
                    (msKineticsTypeDerived<AntiochCalculatorType >::nameType.c_str(),
                     msKineticsTypeDerived<AntiochCalculatorType >::doc.c_str(), no_init
                     ) 
                    .def( "New", &msKineticsTypeDerived<AntiochCalculatorType>::New ,
                          "Create a new object.")
                    .staticmethod("New");
                
                msKineticsTypeDerived::isKineticsTypeDerivedRegisteredInPython = 1;
            }
#endif
        }
            
        public:
            
            msKineticsTypeDerived() : msKineticsType() {
	      
	        constructVar(msKineticsTypeDerived<AntiochCalculatorType >::nameType,msKineticsTypeDerived<AntiochCalculatorType >::nameType,
			     msKineticsTypeDerived<AntiochCalculatorType >::nameType); 
		Order = 0;
	    }
	    
            void initialize();
            
            void update()     { 
	        
	        msKineticsType::update();
		setOrder(Order);
            }
            
            static boost::shared_ptr<msKineticsTypeDerived> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msKineticsTypeDerived> msKineticsTypeDerived::New()","");
                boost::shared_ptr<msKineticsTypeDerived> T( new msKineticsTypeDerived() );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msKineticsTypeDerived> msKineticsTypeDerived::New()");
                return T;
            }
            

	    boost::shared_ptr<msTreeMapper> setCoefficient(string name,double value);
	    
	    double computeRate(double T) { return getCastedCalculator<AntiochCalculatorType>()->rate(T); 
	    }
	    
	    double getOrder() const { return Order; }
	    
	    virtual boost::shared_ptr<msTreeMapper> setOrder(size_t value) {
	      
		Order = value;
	        return mySharedPtr();
	    }
	    /*
	    boost::shared_ptr<AntiochCalculatorType > getCalculator(){
	      
	        return Calculator;
	    }*/
	    
	protected:

	    boost::shared_ptr<msTreeMapper> setCalculator(boost::shared_ptr<AntiochCalculatorType > calculator){
	      
	        Calculator = calculator;
	        return mySharedPtr();
	    }
	    
	private:
	  
	   // boost::shared_ptr<AntiochCalculatorType > Calculator;
	    
	    size_t Order;
        };
 
    }
}
#endif // PA0DCSTSTP_H

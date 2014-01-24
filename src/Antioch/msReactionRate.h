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
        
      class msReactionRate : public msPhysicalInterface
        {
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isReactionRateRegisteredInPython;
            static msRegister* createInstance() { return new msReactionRate; }
            //@}
            
        protected:
            
            void registryInPython();
            
        public:
            
            msReactionRate() : msPhysicalInterface() {
	        
	        constructVar("msReactionRate","ReactionRate","kinetic model for reaction rate"); 
		Order = 1;
	    }
	    
            void initialize() { 
	      
	        msPhysicalInterface::initialize();
		msTreeMapper::declareAttribute(Coefficients,"Coefficients");
		msTreeMapper::declareAttribute(Order,"Order");
		//Calculator_BaseClass = boost::shared_ptr<Antioch::KineticsType<double> >(new Antioch::ArrheniusRate<double>() );
            }
            
            void update()     {  
	      
	        LOGGER_ENTER_FUNCTION_DBG("void msReactionRate::update()", getFullId());
	        msPhysicalInterface::update();
	        map<string,double>::iterator it=Coefficients.begin();
		for(;it!=Coefficients.end();++it) setCoefficient((*it).first,(*it).second);
		LOGGER_EXIT_FUNCTION2("void msReactionRate::update()");
	    }
            
            static boost::shared_ptr<msReactionRate> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msReactionRate> msReactionRate::New()","");
                boost::shared_ptr<msReactionRate> T( new msReactionRate() );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msReactionRate> msReactionRate::New()");
                return T;
            }
            
            boost::shared_ptr<Antioch::KineticsType<double> > getCalculator() { 
	      
		return Calculator; 
	    }
	    
	    double computeRate(double T) { 
	      
	        stringstream unit;
		unit<<"s^-1.";
		if(Order>0) unit<<"m^"<<3*(Order-1)<<".kmol^-"<<(Order-1);
		
		return getUnits()->convert(unit.str(),(*Calculator)(T));
	    }
	    
	    template<class T>
	    boost::shared_ptr<T> getCastedCalculator() { 
	        
	        return boost::static_pointer_cast<T>(Calculator); 
	    }
	    
	    map<string,double> getCoefficients() { 
	      
	        return Coefficients; 
	    };
	    
	    double getOrder() const { 
	      
	        return Order; 
	    }
	    
	    //! \brief return the unit of the rate constant
	    msUnit getUnit() const {
	      
		   stringstream unit;
		   unit<<"s^-1.";
		   if(Order>0)
	              unit<<getUnits()->getLengthStr()<<"^"<<3*(Order-1)
		       <<"."<<getUnits()->getQuantityStr()<<"^-"<<(Order-1);
		   msUnit u;
		   u.set(unit.str());
		   return u;	      
	    }
	    
	    boost::shared_ptr<msTreeMapper> setOrder(size_t value) {
	      
	        if(value<0) BOOST_THROW_EXCEPTION(msError("Order has to be superior or equal to 0",
						  "boost::shared_ptr<msTreeMapper> msReaction::setOrder(size_t value)",
						  getFullId()));
		Order = value;
		msPhysicalInterface::declarePhysicalVariable(getUnit(),&(Coefficients["Cf"]),"Cf");
	        return mySharedPtr();
	    }
	    
	    virtual boost::shared_ptr<msTreeMapper> setCoefficient(string name,double value) {
	      
	        throwNotImplemented("virtual boost::shared_ptr<msTreeMapper> msReactionRate::setCoefficient(string name,double value)");
	    } 
	   
	protected:
	    	
	    boost::shared_ptr<Antioch::KineticsType<double> > Calculator;
		    	  
	    map<string, double> Coefficients;
			
	    msUnit getUnitCalculator() {  stringstream unit; unit<<"s^-1.";
					  if(Order>0) unit<<"m^"<<3*(Order-1)<<".kmol^-"<<(Order-1);
					  return msUnit(unit.str());
	                                }
	    
	private:
	  	  	    
	    size_t Order;
	    
	    void throwNotImplemented(string fctName) const {
	      
	        msError e("The class msReactionBase if virtual",fctName,getFullId());
		BOOST_THROW_EXCEPTION(e);
	    }
	    
        };
	
	
	
	template<class AntiochCalculatorType>
        class msReactionRateDerived: public msReactionRate
        {
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isReactionRateDerivedRegisteredInPython;
            static msRegister* createInstance() {  return new msReactionRateDerived<AntiochCalculatorType>(); }
            
            static string nameType;
	    static string doc;
            //@}
            
        protected:
            
            void registryInPython() {
#if USE_PYTHON
	    
                msReactionRate::registryInPython();
            
                if( ! msReactionRateDerived::isReactionRateDerivedRegisteredInPython ) {
                
                    using namespace boost::python;
                
                    class_< msReactionRateDerived<AntiochCalculatorType> ,
		           bases<msReactionRate>,
			   boost::shared_ptr<msReactionRateDerived<AntiochCalculatorType> > 
			   >
                    (msReactionRateDerived<AntiochCalculatorType >::nameType.c_str(),
                     msReactionRateDerived<AntiochCalculatorType >::doc.c_str(), no_init
                     ) 
                    .def( "New", &msReactionRateDerived<AntiochCalculatorType>::New ,
                          "Create a new object.")
                    .staticmethod("New");
                
                msReactionRateDerived::isReactionRateDerivedRegisteredInPython = 1;
            }
#endif
        }
            
        public:
            
            msReactionRateDerived() : msReactionRate() {
	      
	        constructVar(msReactionRateDerived<AntiochCalculatorType >::nameType,msReactionRateDerived<AntiochCalculatorType >::nameType,
			     msReactionRateDerived<AntiochCalculatorType >::nameType); 
	    }
	    
            void initialize();
            
            void update()     { msReactionRate::update(); }
            
            static boost::shared_ptr<msReactionRateDerived> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msReactionRateDerived> msReactionRateDerived::New()","");
                boost::shared_ptr<msReactionRateDerived> T( new msReactionRateDerived() );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msReactionRateDerived> msReactionRateDerived::New()");
                return T;
            }
            
	    boost::shared_ptr<msTreeMapper> setCoefficient(string name,double value);
	    	
        };
 
    }
}
#endif // PA0DCSTSTP_H

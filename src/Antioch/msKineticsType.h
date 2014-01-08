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
/*
#include "antioch/vector_utils.h"
#include "antioch/physical_constants.h"
#include "antioch/chemical_species.h"*/
//#include "antioch/kinetics_type.h"

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
		Order = 0;
	    }
	    
            void initialize() { 
	      
	        msPhysicalInterface::initialize();
		msTreeMapper::declareAttribute(Order,"Order");
            }
            
            void update()     { 
	        
	        msPhysicalInterface::update();
		setOrder(Order);
            }
            
            static boost::shared_ptr<msKineticsType> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msKineticsType> msKineticsType::New()","");
                boost::shared_ptr<msKineticsType> T( new msKineticsType() );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msKineticsType> msKineticsType::New()");
                return T;
            }
            
	    virtual map<string,double> getCoefficients();
	    
	    virtual boost::shared_ptr<msTreeMapper> setCoefficient(string name,double value);
	    
	    
	    double getOrder() const { return Order; }
	    
	    virtual boost::shared_ptr<msTreeMapper> setOrder(size_t value) {
	      
		Order = value;
	        return mySharedPtr();
	    }
	    
	protected:
	  
            template<class T>
            boost::shared_ptr<T> getCalculator(){
	      
	        //return boost::static_pointer_cast<T>(Calculator);
	    }
/*
	    boost::shared_ptr<msTreeMapper> setCalculator(boost::shared_ptr<Antioch::KineticsType<double> > calculator){
	      
	        //Calculator = calculator;
	        return mySharedPtr();
	    }*/
	    
	private:
	  
	   // boost::shared_ptr<Antioch::KineticsType<double> > Calculator;
	     
	    
	    size_t Order;
        };
        
    }
}
#endif // PA0DCSTSTP_H

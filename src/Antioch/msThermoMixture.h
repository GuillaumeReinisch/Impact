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


#ifndef msTHERMOMIXTURE_H
#define msTHERMOMIXTURE_H

#include "antioch/chemical_mixture.h"
#include "antioch/cea_evaluator.h"
#include "antioch/cea_mixture.h"

#include <msThermoConstituent.h>
#include <msChemicalMixture.h>
#include <msMotionCEA.h>

namespace impact {
    
    namespace antioch {
        
        using namespace atomism;
        
        /** \brief A thermo mixture of components
         *
         * A mixture with a thermodynamic manager.
	 * 
	 * ~~~~~~~~~~~~~{.py}
         * ~~~~~~~~~~~~~
         */
        class msThermoMixture : public msChemicalMixture
        {
        public:
           
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isPhaseRegisteredInPython;
            static msRegister* createInstance() { return new msThermoMixture; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update() {
                
                LOGGER_ENTER_FUNCTION_DBG("void msThermoMixture::update()",getFullId());
                msChemicalMixture::update();
				
		Antioch::CEAThermoMixture cea_mixture( *(msChemicalMixture::getCalculator()) );
		 
		vector<string> names=getConstituentsNames();
		
		for(size_t i=0;i<noOfConstituents(); ++i) {
		  
		    if( ! getConstituentFromIndex(i)->isDerivedFrom("msThermoConstituent") ) {
		     
		        msError e("ThermoMixture's constituents need to be derived from msThermoCOnstituant "+
				  "(failed for species "+names[i]+")",
			  	  "void msThermoMixture::update()",getFullId());
			
		        BOOST_THROW_EXCEPTION(e);		     
		     }
		   
		     boost::shared_ptr<msThermoConstituent> speciesTD = 
		         getConstituentFromIndex(i)->impact_static_cast<msThermoConstituent>();
		   
		     if( !speciesTD->getMotion()->isDerivedFrom("msMotionCEA") ) {
		     
		        msError e("The motion of the constituent "+name+" is not of type CEAMotion. "+
			  	  "at this stage this is required.",
			  	  "void msThermoMixture::update()",getFullId());
			
		        BOOST_THROW_EXCEPTION(e);	
		     }
		   
		     boost::shared_ptr<msMotionCEA> motion = 
		         speciesTD->getMotion()->impact_static_cast<msMotionCEA>();
		   
		     for( size_t i = 0 ; i < 3; i++ ) {
		     
		        cea_mixture.add_curve_fit( names[i], motion->getCoeffsOfFit(i) );     
		     }
		}
		
		ThermoCalculator = boost::shared_ptr<Antioch::CEAEvaluator>( new Antioch::CEAEvaluator(cea_mixture) );
		
                LOGGER_EXIT_FUNCTION2("void msThermoMixture::update()");
            }
            
            
        public:
            
            msThermoMixture(): msChemicalMixture() {
                
                constructVar("msThermoMixture","ThermoMixture","mixture");
            }
            
            void initialize() {
                
                LOGGER_ENTER_FUNCTION_DBG("void msThermoMixture::initialize()","");
                
                msChemicalMixture::initialize();
		
                LOGGER_EXIT_FUNCTION2("void msThermoMixture::initialize()");
            }
                        
            virtual double getDensity() {
	      
	    }

	    vector<double> getH_RT_minus_SR( double T ) const  { 
	      
	      vector<double> result;
	      Antioch::CEAThermodynamics::Cache Temp(T,pow(T,2),pow(T,3),pow(T,4),ln(T));
	      
	      ThermoCalculator->h_RT_minus_s_R( Temp , result );
	    } 
	    	    
            std::ostream& print(std::ostream& out) const ;
            
	    
        protected:
            
            void testCalculator(std::string fct) const{
                
                //if( Calculator ) return;
                msError e("The calculator is not initialized, use the 'load' function",fct,getFullId());
                BOOST_THROW_EXCEPTION(e);
            }
            
        private:
                        
	    boost::shared_ptr<Antioch::CEAEvaluator> ThermoCalculator;
        };
    }
}

#endif // PA0DCSTSTP_H

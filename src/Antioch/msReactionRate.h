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
	      
	        IMPACT_LOGIN();
	        msPhysicalInterface::initialize();
		msTreeMapper::declareAttribute(Coefficients,"Coefficients");
		msTreeMapper::declareAttribute(Order,"Order");
		IMPACT_LOGOUT();
            }
            
            void update()     {  
	      
	        IMPACT_LOGIN();
	        msPhysicalInterface::update();
	        map<string,double>::iterator it=Coefficients.begin();
		for(;it!=Coefficients.end();++it) setCoefficient((*it).first,(*it).second);
		IMPACT_LOGOUT();
	    }
            
            static boost::shared_ptr<msReactionRate> New(){
                
                IMPACT_LOGIN_STATIC();
                boost::shared_ptr<msReactionRate> T( new msReactionRate() );
                T->initialize();
                T->update();
                IMPACT_LOGOUT();
                return T;
            }
            
            //! \brief return the calculator
            boost::shared_ptr<Antioch::KineticsType<double> > getCalculator() { 
	      
		return Calculator; 
	    }
	    
	    /** \brief return the rate at a particular temperature
	     * 
	     * \param T temperature
	     */ 
	    double computeRate(double T) { 
	      
	        IMPACT_LOGIN();
	        msUnit u = getUnitCalculator();
		double r = 0;
		IMPACT_TRY([&]() { r = getUnits()->convert( u, (*Calculator)(T) );} );
		IMPACT_LOGOUT();
		return r;
	    }
	    
	    //! \brief return the map 'name coefficient' -> 'value'
	    map<string,double> getCoefficients() { 
	      
	        return Coefficients; 
	    };
	    
	    //! \brief return the order of the reaction
	    double getOrder() const { 
	      
	        return Order; 
	    }
	    
	    //! \brief return the unit of the rate constant
	    msUnit getUnit() const {
	        
	        IMPACT_LOGIN();
	        IMPACT_EXCEPT_IF([&](){return Order<0 || Order>1; }, "Order has to be 1 or 2");
	        msUnit u;
		if( Order==0 ) u.set(msUnit::vReactionRateFirstOrder, *(getUnits()));
		if( Order==1 ) u.set(msUnit::vReactionRateSecondOrder,*(getUnits()));
		
		IMPACT_LOGOUT();
		return u;	      
	    }
	    
	     /** \brief set the order of the reaction
	     * 
	     * Note that the unit of the "Cf" coefficient is updated.
	     * \param value Order. Only order 1  or 2 are accepted. 
	     */ 
	    boost::shared_ptr<msTreeMapper> setOrder(size_t value) {
	      
	        IMPACT_LOGIN();
	        IMPACT_EXCEPT_IF([&](){return value<0; }, "Order has to be superior or equal to 0");
		Order = value;
		msPhysicalInterface::declarePhysicalVariable(getUnit(),&(Coefficients["Cf"]),"Cf");
		IMPACT_LOGOUT();
	        return mySharedPtr();
	    }
	    
	    /** \brief set a particular coefficient
	     * 
	     * Only coefficient declared by the msReactionRateDerived classes can be set.
	     * \param name name of the coefficient
	     * \param value value 
	     */ 
	    virtual boost::shared_ptr<msTreeMapper> setCoefficient(string name,double value) {
	      
	        IMPACT_THROW_NOT_IMPLEMENTED();
	    } 
	    
	    /** \brief set all the coefficients from a Antioch::KineticsType calculator
	     * 
	     * The coefficient are those declared by msReactionRateDerived classes.
	     * \param rate calculator of the type used by the derived msReactionRateDerived class
	     */  
	   virtual  boost::shared_ptr<msTreeMapper> setCoefficients(const Antioch::KineticsType<double>* rate){
	        
	        IMPACT_THROW_NOT_IMPLEMENTED();
	    }
	    
	    //! \brief return the calculator casted in a desired type
	    template<class T>
	    boost::shared_ptr<T> getCastedCalculator() { 
	        
	        return boost::static_pointer_cast<T>(Calculator); 
	    }
	    
	protected:
	    	
	    boost::shared_ptr<Antioch::KineticsType<double> > Calculator;
		    	  
	    map<string, double> Coefficients; //!< Attributs 
			    
	    msUnit getUnitCalculator() { 
	      
	        IMPACT_LOGIN();
	        IMPACT_EXCEPT_IF([&](){return Order<0 || Order>1; }, "Order has to be 1 or 2");
	        stringstream unit; unit<<"s^-1.";
	        if(Order>0) unit<<"m^"<<3*(Order-1)<<".kmol^-"<<(Order-1);
		IMPACT_LOGOUT();
		return msUnit(unit.str());
	    }
	    
	private:
	  	  	    
	    size_t Order;
        };
	
	
	template<class AntiochCalculatorType>
        class msReactionRateDerived: public msReactionRate
        {
            typedef void (AntiochCalculatorType::*setFunction)(const double);
	    typedef double (AntiochCalculatorType::*getFunction)() const; 
	    
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
		unitsCalculator = boost::shared_ptr<msUnitsManager>(new msUnitsManager);
		unitsCalculator->set("J kmol s m");
	    }
	    
            void initialize();
            
            void update()     { msReactionRate::update(); }
            
            static boost::shared_ptr<msReactionRateDerived> New(){
                
                IMPACT_LOGIN_STATIC();
                boost::shared_ptr<msReactionRateDerived> T( new msReactionRateDerived() );
                T->initialize();
                T->update();
                IMPACT_LOGOUT();
                return T;
            }
            
	    boost::shared_ptr<msTreeMapper> setCoefficient(string name,double value) {
	      
	        IMPACT_LOGIN();
		IMPACT_EXCEPT_IF( [&]() { return Coefficients.find(name)==Coefficients.end();},
		                  "Coefficient "+name+" not known" );
		
		Coefficients[name] = value;
		
		setFunction setFct = mapSetMethods[name].first;
		boost::shared_ptr<AntiochCalculatorType> calc =  getCastedCalculator<AntiochCalculatorType>();
		
		msUnit unitCoefForCalc = mapSetMethods[name].second;
		unitCoefForCalc.resetSymbols(*unitsCalculator);
		double f = getUnits()->convertTo(1,getUnitCalculator());
		
		if( name == "Ea" ) f = getUnits()->convert(msUnit("J.mol^-1.K^-1"),csts::R);
		(calc.get()->*setFct)( f*value );
		
		IMPACT_LOGOUT();
		return mySharedPtr();
	    }
	    
	    boost::shared_ptr<msTreeMapper> setCoefficients(const Antioch::KineticsType<double>* rate) {
	      
	        IMPACT_LOGIN();
		const AntiochCalculatorType* calc = static_cast<const AntiochCalculatorType*>(rate);
		
	        for_each(Coefficients.begin(),Coefficients.end(),[&](pair<const string, double>& elem) { 
		  
		    getFunction getFct =     mapGetMethods[elem.first].first;		    
		    msUnit unitCoefForCalc = mapGetMethods[elem.first].second;
		    unitCoefForCalc.resetSymbols(*unitsCalculator);
		    double f = getUnits()->convert(getUnitCalculator(),1);
		    
		    if( elem.first == "Ea" ) f = 1./getUnits()->convert(msUnit("J.mol^-1.K^-1"),csts::R);
			 
		    setCoefficient(elem.first, f * (calc->*getFct)());   
		});
		IMPACT_LOGOUT();
		return mySharedPtr();
	    }
	    	
	    
	private:
	  	   
	  boost::shared_ptr<msUnitsManager> unitsCalculator;
	  
	  void declareRateCoefficient(string id, setFunction setFct, getFunction getFct, msUnit unit) {
	    
	        Coefficients[id]  =  0; 
		mapSetMethods[id] =  pair<setFunction,msUnit>(setFct,unit);
		mapGetMethods[id] =  pair<getFunction,msUnit>(getFct,unit);
		if(! unit.isNoneUnit() )
		    msPhysicalInterface::declarePhysicalVariable(unit,&(Coefficients[id]),id);
	  }
	  
	  map<string, std::pair< setFunction, msUnit > > mapSetMethods;
	  map<string, std::pair< getFunction, msUnit > > mapGetMethods;
        };
 
    }
}
#endif // PA0DCSTSTP_H

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


#ifndef MSREACTION_H
#define MSREACTION_H


#include "antioch/reaction.h"
#include <msKineticsType.h>
#include <msChemicalMixture.h>
#include <boost/graph/graph_concepts.hpp>



namespace impact {
    
    namespace antioch {
        
        //using namespace impact::atomism;
        /** \brief Base class for a reaction
         *
         * A reaction is described by a set of coefficients and a name
         *
         */
        class msReaction : public msPhysicalInterface
        {
            
            friend class msKineticMechanism;
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isReactionRegisteredInPython;
            static msRegister* createInstance() { return new msReaction; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update() {
                
                LOGGER_ENTER_FUNCTION_DBG("void msReaction::update()",getFullId());
                msPhysicalInterface::update();
		/*
		Calculator = boost::shared_ptr<Antioch::Reaction>(new Antioch::Reaction(ChemicalMixture->noOfConstituents(),getId(),1) );
		
		Calculator->set_reversibility(isReversible);
				
		msChildren<msConstituent>::iterator it = Reactants.begin();
		for(int i=0;it!=Reactants.end();++it,++i) 
		    Calculator->add_reactant( (*it)->getId(), ChemicalMixture->getIndex( (*it)->getId()) , StoechiometryReactants[i] );
		
		it = Products.begin();
		for(int i=0;it!=Reactants.end();++it,++i) 
		    Calculator->add_product( (*it)->getId(), ChemicalMixture->getIndex( (*it)->getId()) , StoechiometryProducts[i] );
		
		msChildren<msRate>::iterator itRate = ForwardRates.begin();
		for(;it!=ForwardRates.end();++it) 
		    Calculator->add_forward_rate((*it)->getCalculator());
		*/	
                LOGGER_EXIT_FUNCTION2("void msReaction::update()");
            }
            
        public:
            
	    msReaction(): msPhysicalInterface() {
                
                constructVar("msReaction","Reaction","reaction");
            }
            
            void initialize() {
	      
                LOGGER_ENTER_FUNCTION_DBG("void msReaction::initialize()","");
                
                msPhysicalInterface::initialize();
		
		msTreeMapper::declareChildren(ForwardRates,"ForwardRates");
		
                LOGGER_EXIT_FUNCTION2("void msReaction::initialize()");
            }
            

            boost::shared_ptr<msTreeMapper> setChemicalMixture(boost::shared_ptr<msChemicalMixture> mixture){
                
	        msTreeMapper::update(ChemicalMixture,mixture);
		update();
            }     
      
            boost::shared_ptr<msTreeMapper> addReactant(boost::shared_ptr<msConstituent> reactant,size_t coef){
                
	        msTreeMapper::addElementToChildren(Reactants,reactant);
		StoechiometryReactants.push_back(coef);
	        update();
		return mySharedPtr();
            }
 
            boost::shared_ptr<msTreeMapper> addProduct(boost::shared_ptr<msConstituent> product,size_t coef){
                	        
	        msTreeMapper::addElementToChildren(Reactants,product);
	        StoechiometryProducts.push_back(coef);
	        update();
            }
            
            boost::shared_ptr<msTreeMapper> setReversibility(bool isreversible){
               
	       isReversible =  isreversible;
	       update();
	       return mySharedPtr();
            }
            
            boost::shared_ptr<msTreeMapper> addForwardRate(boost::shared_ptr<msKineticsType> rate) {
	      
	        msTreeMapper::addElementToChildren(ForwardRates,rate);
	        return mySharedPtr();
	    };
	    
	    boost::shared_ptr<msKineticsType> getForwardRate(size_t i) {
	      
	        boost::shared_ptr<msKineticsType> rate;/*
	        try{
	            rate= ForwardRates[i]->getSharedPtr();
		}
		catch(msError& e){
		    e.addContext("boost::shared_ptr<msRate> msReaction::getForwardRate(size_t i)");
		}
		*/
	        return rate;
	    };
	    
	    double  computeForwardRateCoefficient( const double& T) const {
	      
	       double k = Calculator->compute_forward_rate_coefficient( ChemicalMixture->getMolarDensities() , T );
	       stringstream unitCalc;
	       unitCalc<<"s^-1.m^"<<3*(Calculator->gamma()-1)<<".mol^-"<<Calculator->gamma()-1;
	       k *= getUnits()->Convert(unitCalc.str(),1);
	       
	       return k;
	    };

	    double  computeRateOfProgress( const double& T) const {
	      
	       if( isReversible && !ChemicalMixture->isDerivedFrom("msThermoMixture") ) {
		 
		   msError e("The computation of the reverse RoP from thermo data for reaction "+getId()+
		             "is required, but mixture is not derived from msThermoMixture ",
	                     "double msReaction::computeRateOfProgress( const double& T) const ");
		   BOOST_THROW_EXCEPTION(e); 
	       }
	       boost::shared_ptr<msThermoMixture> thermo = 
	           ChemicalMixture->impact_static_cast<msThermoMixture>(ChemicalMixture);
		   
	       double k = Calculator->compute_rate_of_progress( ChemicalMixture->getMolarDensities() ,
								T ,
								thermo->getP0_RT(), 
								thermo->getH_RT_minus_SR() );
	       stringstream unitCalc;
	       unitCalc<<"s^-1.m^"<<3*(Calculator->gamma()-1)<<".mol^-"<<Calculator->gamma()-1;
	       k *= getUnits()->Convert(unitCalc.str(),1);
	       
	       return k;
	    };
	    
	    /*
	    vector<double> equilibrium_constant( const StateType& P0_RT,
                                    const VectorStateType& h_RT_minus_s_R ) const;



	    
    //// in reaction set

    
    
    ////
            vector<double>  computeRateOfProgress(const double& T,
                                                  const StateType& P0_RT,  
                                                  const VectorStateType& h_RT_minus_s_R) const;

		
						*/
	    msUnit getUnitForwardRateCoefficient() const {
	      
		   stringstream unit;
	           unit<<"s^-1."<<getUnits()->getLengthStr()<<"^"<<3*(Calculator->gamma()-1)
		       <<"."<<getUnits()->getQuantityStr()<<"^-"<<Calculator->gamma()-1;
		   msUnit u;
		   u.set(unit.str());
		   return u;	      
	    }
            std::ostream& print(std::ostream& out) const ;


	    	   
	   boost::shared_ptr<Antioch::Reaction> getCalculator() {
	     
	        return Calculator;
	   }
	   
	protected:
	   	  
	   template<class T>
	   boost::shared_ptr<T> getCalculator() {
	     
	        return boost::static_pointer_cast<T>(Calculator);
	   }
	  	    
        private:
            
	    
	    msChild<msChemicalMixture> ChemicalMixture;
	    
	    msChildren<msKineticsType> ForwardRates;
	    msChildren<msConstituent>  Reactants;
	    msChildren<msConstituent>  Products;
	    
	    vector<double> StoechiometryReactants;
	    vector<double> StoechiometryProducts;
	    
	    bool isReversible;
	    
            boost::shared_ptr<Antioch::Reaction>     Calculator;         
        };
        
    }
}

#endif // PA0DCSTSTP_H

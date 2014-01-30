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

#include "msReactionRate.h"
#include "msThermoMixture.h"
#include "msEntity.h"

//#include "msThermoMixture.h"
#include "antioch/reaction.h"
#include "antioch/elementary_reaction.h"
#include "antioch/duplicate_reaction.h"
#include "antioch/threebody_reaction.h"
#include "antioch/lindemann_falloff.h"
#include "antioch/troe_falloff.h"
#include "antioch/lindemann_falloff.h"
#include "antioch/falloff_reaction.h"

namespace impact {
    
    namespace antioch {
        
         /** \brief Base class for reaction
         *
	 * Interface for the Antioch::Reaction calculator.
	 * 
	 * 
         * ~~~~~~~~~~~~~{.py}
          from libimpact import *
         
          
          H2 = msEntity.New().addElement(msHAtom.New()).addElement(msHAtom.New())
          H  = msEntity.New().addElement(msHAtom.New())
          
          mix = msChemicalMixture.New().addEntity(H2).addEntity(H)
          
          reaction = msElementaryReaction.New().setChemicalMixture(mix).addReactant(H2,1).addProduct(H,2)
          
          reaction.addForwardRate(msArrhenius.New().set('Cf',1e6).set('Ea',10000).setOrder(1) ) 
          
          #because mix is not of type msThermoMixture the reverse rate is not accounted
          print reaction.computeRateOfProgress(), reaction.getUnitForwardRateCoefficient().getStr()
         * ~~~~~~~~~~~~~
         */
        class msReaction : public msPhysicalInterface
        {     
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
	      
	        IMPACT_LOGIN();
	        msPhysicalInterface::update();
		
		if( isCalculatorLocked ) {
 		  IMPACT_LOGOUT();
		  return;
		}
		/*
		Calculator->set_reversibility(getReversibility());
		
		msChildren<msEntity>::iterator it = Reactants.begin();
		try{
		    for(int i=0;it!=Reactants.end();++it,++i) 
		        Calculator->add_reactant( (*it)->getId(), 
			    		          getChemicalMixture()->getIndex( (*it)->getId()) , 
					          getStoechiometryReactants()[i] );
		}
		catch(msException& e){
		    e.addContext("void msReaction::update() : set reactants");
		    IMPACT_THROW_EXCEPTION(e);
		}
		try{
		    it = Products.begin();
		    for(int i=0;it!=Products.end();++it,++i) 
		        Calculator->add_product( (*it)->getId(), 
					         getChemicalMixture()->getIndex( (*it)->getId()) , 
					         getStoechiometryProducts()[i] );
		}
		catch(msException& e){
		    e.addContext("void msReaction::update() : set products");
		    IMPACT_THROW_EXCEPTION(e);
		}
		try{
		    msChildren<msReactionRate>::iterator itRate = ForwardRates.begin();
		    for(;itRate!=ForwardRates.end();++itRate) 
			Calculator->add_forward_rate( (*itRate)->getCalculator().get());
		}
		catch(msException& e){
		    e.addContext("void msReaction::update() : set forward rate");
		    IMPACT_THROW_EXCEPTION(e);
		}
		try{
		    map<string,double>::iterator it = Efficiencies.begin();
		    for(;it!=Efficiencies.end();++it) 
			Calculator->set_efficiency("",getChemicalMixture()->getIndex((*it).first),(*it).second);
		}
		catch(msException& e){
		    e.addContext("void msReaction::update() : set forward rate");
		    IMPACT_THROW_EXCEPTION(e);
		}
		*/
	        IMPACT_LOGOUT();
	    }
            
        public:
            
	    msReaction(): msPhysicalInterface() {
                
                constructVar("msReaction","Reaction","reaction base");
		isReversible=1;isCalculatorLocked=0;
            }
            void initialize() {
	      
                IMPACT_LOGIN();
                
                msPhysicalInterface::initialize();
		
		msTreeMapper::declareChild(ChemicalMixture,msChemicalMixture::New(),"ChemicalMixture");
		
		msTreeMapper::declareChildren(ForwardRates,"ForwardRates");
		msTreeMapper::declareChildren(Reactants,"Reactants");
		msTreeMapper::declareChildren(Products,"Products");
		
		msTreeMapper::declareAttribute(StoechiometryReactants,"StoechiometryReactants");
		msTreeMapper::declareAttribute(StoechiometryProducts,"StoechiometryProducts");
		msTreeMapper::declareAttribute(Efficiencies,"Efficiencies");
		msTreeMapper::declareAttribute(isReversible,"isReversible");
		
                IMPACT_LOGOUT();
            }
	    
	    template<class T>
	    boost::shared_ptr<T> getCastedCalculator() const {
	         return boost::static_pointer_cast<T>(Calculator);
	    }
	   
	    boost::shared_ptr<Antioch::Reaction<double> > getCalculator() const {
	        return Calculator;
	    }
            //! \brief return the chemical mixture
            boost::shared_ptr<msChemicalMixture> getChemicalMixture() const {
	        return ChemicalMixture.getSharedPtr();
	    }
	    /** \brief set the chemical mixture the reaction is working with
	     * 
	     * \param mixture chemical mixture
	     */ 
            boost::shared_ptr<msTreeMapper> setChemicalMixture(boost::shared_ptr<msChemicalMixture> mixture){
                
	        IMPACT_LOGIN();
	        msTreeMapper::update(ChemicalMixture,mixture);
		update();
		IMPACT_LOGOUT();
		return mySharedPtr();
            }    
 
	    /** \brief add a reactant to the reaction definition
	     * 
	     * \param reactant reactant
	     * \param coef stoechiometric coefficient
	     */  
            boost::shared_ptr<msTreeMapper> addReactant(boost::shared_ptr<msEntity> reactant,size_t coef){
	      
                IMPACT_LOGIN();
	        msTreeMapper::addElementToChildren(Reactants,reactant);
		StoechiometryReactants.push_back(coef);
	        update();
		IMPACT_LOGOUT();
		return mySharedPtr();
            }  
 
	    /** \brief add a product to the reaction definition
	     * 
	     * \param product product
	     * \param coef stoechiometric coefficient
	     */  
            boost::shared_ptr<msTreeMapper> addProduct(boost::shared_ptr<msEntity> product,size_t coef){
                
	        IMPACT_LOGIN();
		msTreeMapper::addElementToChildren(Products,product);
	        StoechiometryProducts.push_back(coef);
	        update();
		IMPACT_LOGOUT();
		return mySharedPtr();
            }
            
	    /** \brief add a forward rate definition to the manager
	     * 
	     * \param rate reaction rate
	     */  
            boost::shared_ptr<msTreeMapper> addForwardRate(boost::shared_ptr<msReactionRate> rate){
                
	        IMPACT_LOGIN();
		msTreeMapper::addElementToChildren(ForwardRates,rate);
		update();
		IMPACT_LOGOUT();
		return mySharedPtr();
            }
            /** \brief set the reversibility
	     * 
	     * \param isreversible 1 if the reaction is reversible, 0 else
	     */
            boost::shared_ptr<msTreeMapper> setReversibility(bool isreversible){
               
	       IMPACT_LOGIN();
	       isReversible =  isreversible;
	       update();
	       IMPACT_LOGOUT();
	       return mySharedPtr();
            }

             /** \brief (re)set stoechiometric coefficient of a reactant
	     * 
	     * \param target reactant
	     * \param coef stoechiometric coefficient
	     */
            boost::shared_ptr<msTreeMapper> setReactantStoechiometry( boost::shared_ptr<msEntity> target, double coef ){
               
	       IMPACT_LOGIN();
	       IMPACT_EXCEPT_IF( [&] () { return (Reactants.find(target)==Reactants.end()); },
		       "Can not find the product of id "+target->getId()+" in the products list");
	       StoechiometryReactants[Reactants.find(target)-Reactants.begin()]=coef;
	       update();
	       IMPACT_LOGOUT();
	       return mySharedPtr();
            }
            
            /** \brief (re)set stoechiometric coefficient of a product
	     * 
	     * \param target product
	     * \param coef stoechiometric coefficient
	     */
            boost::shared_ptr<msTreeMapper> setProductStoechiometry( boost::shared_ptr<msEntity> target, double coef ){
               
	       IMPACT_LOGIN();
	       IMPACT_EXCEPT_IF( [&] () { return (Products.find(target)==Products.end()); },
		       "Can not find the product of id "+target->getId()+" in the products list");
	       
	       StoechiometryProducts[ Products.find(target) - Products.begin() ]=coef;
	       update();
	       IMPACT_LOGOUT();
	       return mySharedPtr();
            }
            
	    /*! \brief set the efficiency of a specie
	    *
	    * \param name name of the specie
	    * \param v efficiency
	    */ 
            boost::shared_ptr<msTreeMapper> setEfficiency(std::string name, double v){ 
	      
	      IMPACT_LOGIN();
	      IMPACT_EXCEPT_IF([&]() {return Efficiencies.find(name)==Efficiencies.end();},
		               "Id of the specie "+name+" not found.");
	      Efficiencies[name]=v;
	      IMPACT_TRY([&](){update();});
	      IMPACT_LOGOUT();
	      return mySharedPtr();
	    }
            
	    //! \brief return the reactants 
	    vector<boost::shared_ptr<msEntity> > getReactants()	const { return Reactants.getElements(); }
	    
	    //! \brief return the products 
	    vector<boost::shared_ptr<msEntity> > getProducts()	const { return Products.getElements();  }
	    
	    //! \brief return the reactants' stochiometric coefficients 
	    vector<double> getStoechiometryReactants() 		const { return StoechiometryReactants; }   
	    
	    //! \brief return the products' stochiometric coefficients 	    
	    vector<double> getStoechiometryProducts()  		const { return StoechiometryProducts; }
	    
	    //! \brief return 1 if the reaction is reversible, 0 else
	    bool getReversibility()                        	const { return isReversible; }
	    
	    //! \brief return the order of the reaction w/ reactants
	    double getReactantOrder() const { 
	      
	      vector<double> r(StoechiometryReactants.size());
	      std::partial_sum (StoechiometryReactants.begin(), StoechiometryReactants.end(), r.begin());
	      return r.back(); 
	    }
	    
	    //! \brief return the forward rate calculators	    
	    std::vector<boost::shared_ptr<msReactionRate> > getForwardRates() {
	      
	      return ForwardRates.getElements();
	    }

	    /** \brief return a forward rate calculator 
	     * 
	     * \param i index of the rate
	     */ 
	    boost::shared_ptr<msReactionRate> getForwardRate(size_t i) {
	      
	        IMPACT_LOGIN();
	        boost::shared_ptr<msReactionRate> rate = ForwardRates[i].getSharedPtr();
		IMPACT_LOGOUT();
	        return rate;
	    };

	    //! \brief return the unit of the forward rate coefficient    
	    msUnit getUnitForwardRateCoefficient() const {
	      
	        IMPACT_LOGIN();
		size_t o = getReactantOrder();
	        IMPACT_EXCEPT_IF( [&] () { return (o!=1) && (o!=2); },
			  "The order of the reaction should be one or two");

	        if (o==1)  return getUnits()->getUnit(msUnit::vReactionRateFirstOrder);
		if (o==2)  return getUnits()->getUnit(msUnit::vReactionRateSecondOrder);
		
		IMPACT_LOGOUT();
		return msUnit();
	    }
	    
	    //! \brief print in a stream
	    std::ostream& print(std::ostream& out) const {
	     
	        msPhysicalInterface::print(out);
                output::printHeader(out,"msReaction");
	        Calculator->print(out);
	        return out;
	    };
	   
	    
	    /**
             * @name Virtual methods
             */
            //@{
            //! \brief compute the forward rate coefficient for the mixture state
	    virtual double  computeForwardRateCoefficient() {  IMPACT_THROW_NOT_IMPLEMENTED(); };
	    
            //! \brief compute the rate of progress for the mixture state
	    virtual double  computeRateOfProgress() { IMPACT_THROW_NOT_IMPLEMENTED(); };
	    //@}
	    
	    bool sanityCheck();
	    	    
	protected:
	  
	    void setCalculator(boost::shared_ptr< Antioch::Reaction<double> > calc){ Calculator = calc;}
		
	    bool isCalculatorLocked;
	    
	private:
	    
	    //! @name Attributs & children
            //@{
	    bool isReversible;
	    	    
	    msChildren<msReactionRate> ForwardRates;
	    
	    msChild<msChemicalMixture> ChemicalMixture;
	    
	    msChildren<msEntity>  Reactants;
	    msChildren<msEntity>  Products;
	    
	    vector<double> StoechiometryReactants;
	    vector<double> StoechiometryProducts;
	   	    
	    map<string,double> Efficiencies;
	    //@}
	    
	    boost::shared_ptr< Antioch::Reaction<double> >     Calculator; 	    
	    
         };
      
	
        /** \brief Calculator-templated derived class for a reaction
         *
         */
	template<class AntiochCalculatorType>
        class msReactionDerived : public msReaction
        {            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isReactionDerivedRegisteredInPython;
            static msRegister* createInstance() { return new msReactionDerived<AntiochCalculatorType>; }
            
            static string nameType;
	    static string doc;
            //@}
            
        protected:
            
            void registryInPython() {
            
#if USE_PYTHON
                msReaction::registryInPython();
	        if( ! msReactionDerived::isReactionDerivedRegisteredInPython ) {
                
                    using namespace boost::python;
                
                    class_<msReactionDerived<AntiochCalculatorType>, bases<msReaction>,
		           boost::shared_ptr<msReactionDerived<AntiochCalculatorType> > >
			   (msReactionDerived<AntiochCalculatorType>::nameType.c_str(),
                            msReactionDerived<AntiochCalculatorType>::doc.c_str(),no_init
                            )
                    .def( "New", &msReactionDerived< AntiochCalculatorType >::New ,
                         "Create a new object.")
                    .staticmethod("New");
                
                    msReactionDerived::isReactionDerivedRegisteredInPython = 1;
                 }
                 
#endif
            };
            
            void update() {
                
                IMPACT_LOGIN();
		if( ! isCalculatorLocked ) 
 		    setCalculator( boost::shared_ptr<AntiochCalculatorType>(
		                   new AntiochCalculatorType(getChemicalMixture()->noOfEntities(),
				   getId(),1) )  
			          );
		    
		msReaction::update();
                IMPACT_LOGOUT();
            }
            
        public:
            
	    msReactionDerived(): msReaction() {
                
                constructVar(msReactionDerived<AntiochCalculatorType>::nameType,
			     msReactionDerived<AntiochCalculatorType>::nameType,
			     msReactionDerived<AntiochCalculatorType>::nameType);
            }
            
            static boost::shared_ptr<msReactionDerived> New(){
                
		//return IMPACT_NEW( [&]() {return new msReactionDerived();} );
		
                IMPACT_LOGIN_STATIC();
                boost::shared_ptr<msReactionDerived> T( new msReactionDerived() );
                T->initialize();
                T->update();
                IMPACT_LOGOUT();
                return T;
            }
            
            void initialize() {
	      
                IMPACT_LOGIN();
                msReaction::initialize();
		IMPACT_LOGOUT();
            }
            
            
	    double  computeForwardRateCoefficient() {
	      
	        IMPACT_LOGIN();
	        double k=0; 
	        boost::shared_ptr<AntiochCalculatorType> calc = getCastedCalculator<AntiochCalculatorType>();
	       
	        IMPACT_TRY([&](){ k = calc->compute_forward_rate_coefficient( 
	                             getChemicalMixture()->getMolarDensities() , 
		                     getChemicalMixture()->getTemperature() );
	        });
	       
	        k *= getUnits()->convert(getUnitForwardRateCoefficient().getStr(),1);
	        IMPACT_LOGOUT();
	        return k;
	    };

	    double  computeRateOfProgress( const double& T) {
	      
	        IMPACT_LOGIN();
	        IMPACT_EXCEPT_IF( [&]() { return isReversible && !ChemicalMixture->isDerivedFrom("msThermoMixture"); },
			     "The mixture is not derived from msThermoMixture and reverse rates are required");
	       
	        boost::shared_ptr<msThermoMixture> thermo = 
	            ChemicalMixture->impact_static_cast<msThermoMixture>();
	
	        double P0_RT = 1e5/(csts::R*T);
	        double k = getCastedCalculator<AntiochCalculatorType>()->compute_rate_of_progress(
		           ChemicalMixture->getMolarDensities() ,
		 	  T , P0_RT, thermo->getH_RT_minus_SR() );
	       
	        k *= getUnits()->convert(msUnit("kmol.s^-1"),1);
	        IMPACT_LOGOUT();
	        return k;
	    };
	 	    
	    std::ostream& print(std::ostream& out) const {
	     
	         msReaction::print(out);
                 output::printHeader(out,msReactionDerived<AntiochCalculatorType>::nameType);
	         getCastedCalculator<AntiochCalculatorType>()->print(out);
	         return out;
	    };    	   
        private:
            
  
        };
        
    }
}

#endif // PA0DCSTSTP_H

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
	      
	        LOGGER_ENTER_FUNCTION_DBG("void msReaction::update()",getFullId());
	        msPhysicalInterface::update();
		
		Calculator->set_reversibility(getReversibility());
		
		msChildren<msEntity>::iterator it = Reactants.begin();
		try{
		    for(int i=0;it!=Reactants.end();++it,++i) 
		        Calculator->add_reactant( (*it)->getId(), 
			    		          getChemicalMixture()->getIndex( (*it)->getId()) , 
					          getStoechiometryReactants()[i] );
		}
		catch(msError& e){
		    e.addContext("void msReaction::update() : set reactants");
		    BOOST_THROW_EXCEPTION(e);
		}
		try{
		    it = Products.begin();
		    for(int i=0;it!=Products.end();++it,++i) 
		        Calculator->add_product( (*it)->getId(), 
					         getChemicalMixture()->getIndex( (*it)->getId()) , 
					         getStoechiometryProducts()[i] );
		}
		catch(msError& e){
		    e.addContext("void msReaction::update() : set products");
		    BOOST_THROW_EXCEPTION(e);
		}
		try{
		    msChildren<msReactionRate>::iterator itRate = ForwardRates.begin();
		    for(;itRate!=ForwardRates.end();++itRate) 
			Calculator->add_forward_rate( (*itRate)->getCalculator().get());
		}
		catch(msError& e){
		    e.addContext("void msReaction::update() : set forward rate");
		    BOOST_THROW_EXCEPTION(e);
		}
		try{
		    map<string,double>::iterator it = Efficiencies.begin();
		    for(;it!=Efficiencies.end();++it) 
			Calculator->set_efficiency("",getChemicalMixture()->getIndex((*it).first),(*it).second);
		}
		catch(msError& e){
		    e.addContext("void msReaction::update() : set forward rate");
		    BOOST_THROW_EXCEPTION(e);
		}
	        LOGGER_EXIT_FUNCTION2("void msReaction::update()");
	    }
            
        public:
            
	    msReaction(): msPhysicalInterface() {
                
                constructVar("msReaction","Reaction","reaction base");
		isReversible=1;
            }
            void initialize() {
	      
                LOGGER_ENTER_FUNCTION_DBG("void msReaction::initialize()","");
                
                msPhysicalInterface::initialize();
		
		msTreeMapper::declareChild(ChemicalMixture,msChemicalMixture::New(),"ChemicalMixture");
		
		msTreeMapper::declareChildren(ForwardRates,"ForwardRates");
		msTreeMapper::declareChildren(Reactants,"Reactants");
		msTreeMapper::declareChildren(Products,"Products");
		
		msTreeMapper::declareAttribute(StoechiometryReactants,"StoechiometryReactants");
		msTreeMapper::declareAttribute(StoechiometryProducts,"StoechiometryProducts");
		msTreeMapper::declareAttribute(Efficiencies,"Efficiencies");
		msTreeMapper::declareAttribute(isReversible,"isReversible");
		
                LOGGER_EXIT_FUNCTION2("void msReaction::initialize()");
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
                
	        LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msReaction::setChemicalMixture()",getFullId());
	        msTreeMapper::update(ChemicalMixture,mixture);
		//update();
		LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msReaction::setChemicalMixture()");
		return mySharedPtr();
            }    
 
	    /** \brief add a reactant to the reaction definition
	     * 
	     * \param reactant reactant
	     * \param coef stoechiometric coefficient
	     */  
            boost::shared_ptr<msTreeMapper> addReactant(boost::shared_ptr<msEntity> reactant,size_t coef){
	      
                LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msReaction::addReactant(boost::shared_ptr<msEntity> reactant,size_t coef)",getFullId());
	        msTreeMapper::addElementToChildren(Reactants,reactant);
		StoechiometryReactants.push_back(coef);
	        //update();
		LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msReaction::addReactant(boost::shared_ptr<msEntity> reactant,size_t coef)");
		return mySharedPtr();
            }  
 
	    /** \brief add a product to the reaction definition
	     * 
	     * \param product product
	     * \param coef stoechiometric coefficient
	     */  
            boost::shared_ptr<msTreeMapper> addProduct(boost::shared_ptr<msEntity> product,size_t coef){
                
	        LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msReaction::addProduct(boost::shared_ptr<msEntity> product,size_t coef)",getFullId()); 
	        msTreeMapper::addElementToChildren(Products,product);
	        StoechiometryProducts.push_back(coef);
	        //update();
		LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msReaction::addProduct(boost::shared_ptr<msEntity> product,size_t coef)");
		return mySharedPtr();
            }
            
	    /** \brief add a forward rate definition to the manager
	     * 
	     * \param rate reaction rate
	     */  
            boost::shared_ptr<msTreeMapper> addForwardRate(boost::shared_ptr<msReactionRate> rate){
                
	        LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msReaction::addForwardRate(boost::shared_ptr<msReactionRate> rate)",getFullId()); 
	        msTreeMapper::addElementToChildren(ForwardRates,rate);
		if( !rate->hasParent() ) rate->setAffiliation(mySharedPtr());
	        //update();
		LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msReaction::addForwardRate(boost::shared_ptr<msReactionRate> rate)");
		return mySharedPtr();
            }
            /** \brief set the reversibility
	     * 
	     * \param isreversible 1 if the reaction is reversible, 0 else
	     */
            boost::shared_ptr<msTreeMapper> setReversibility(bool isreversible){
               
	       LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msReaction::setReversibility(bool isreversible)",getFullId());
	       isReversible =  isreversible;
	       //update();
	       LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msReaction::setReversibility(bool isreversible)");
	       return mySharedPtr();
            }

             /** \brief (re)set stoechiometric coefficient of a reactant
	     * 
	     * \param target reactant
	     * \param coef stoechiometric coefficient
	     */
            boost::shared_ptr<msTreeMapper> setReactantStoechiometry( boost::shared_ptr<msEntity> target, double coef ){
               
	       LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msReaction::setReactantStoechiometry( boost::shared_ptr<msEntity> target, double coef )",getFullId());
	       
	       msChildren<msEntity>::iterator it =Reactants.find(target); 
		
	       if(it==Reactants.end()) {
		  
		     msError e("Can not find the specie "+target->getId()+" in the Reactants list",
		               "boost::shared_ptr<msTreeMapper> msReaction::setReactantStoechiometry( boost::shared_ptr<msEntity> target, double coef )",
			       getFullId());
		    BOOST_THROW_EXCEPTION(e); 
	       }
		
	       StoechiometryReactants[(it-Reactants.begin())]=coef;
	       LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msReaction::setReactantStoechiometry( boost::shared_ptr<msEntity> target, double coef )");
	       return mySharedPtr();
            }
            
            /** \brief (re)set stoechiometric coefficient of a product
	     * 
	     * \param target product
	     * \param coef stoechiometric coefficient
	     */
            boost::shared_ptr<msTreeMapper> setProductStoechiometry( boost::shared_ptr<msEntity> target, double coef ){
               
	       LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> setProductStoechiometry( boost::shared_ptr<msEntity> target, double coef )",getFullId());
	       msChildren<msEntity>::iterator it =Products.find(target); 
		
	       if(it==Products.end()) {
		  
		     msError e("Can not find the specie "+target->getId()+" in the Products list",
		               "boost::shared_ptr<msTreeMapper> msReaction::setReactantStoechiometry( boost::shared_ptr<msEntity> target, double coef )",
			       getFullId());
		    BOOST_THROW_EXCEPTION(e); 
	       }
		
	       StoechiometryProducts[(it-Products.begin())]=coef;
	       LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> setProductStoechiometry( boost::shared_ptr<msEntity> target, double coef )");
	       return mySharedPtr();
            }
            
	    /*! \brief set the efficiency of a specie
	    *
	    * \param name name of the specie
	    * \param v efficiency
	    */ 
            boost::shared_ptr<msTreeMapper> setEfficiency(std::string name, double v){ Efficiencies[name]=v; }
            
	    //! \brief return the reactants 
	    vector<boost::shared_ptr<msEntity> > getReactants()       const { return Reactants.getElements(); }
	    
	    //! \brief return the products 
	    vector<boost::shared_ptr<msEntity> > getProducts()        const { return Products.getElements();  }
	    
	    //! \brief return the reactants' stochiometric coefficients 
	    vector<double> getStoechiometryReactants() const { return StoechiometryReactants; }   
	    
	    //! \brief return the products' stochiometric coefficients 	    
	    vector<double> getStoechiometryProducts()  const { return StoechiometryProducts; }
	    
	    //! \brief return 1 if the reaction is reversible, 0 else
	    bool getReversibility()                        const {return isReversible; }
	    
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
	      
	        boost::shared_ptr<msReactionRate> rate;
	        try{
	            rate= ForwardRates[i].getSharedPtr();
		}
		catch(msError& e){
		    e.addContext("boost::shared_ptr<msRate> msReactionDerived::getForwardRate(size_t i)");
		}
		
	        return rate;
	    };

	    //! \brief return the unit of the forward rate coefficient    
	    msUnit getUnitForwardRateCoefficient() const {
	      
	           if(getReactantOrder()<1){
		 
		       msError e("The order of the reaction is inferior to one",
		                 "msUnit msReaction::getUnitForwardRateCoefficient()",getFullId());
		       BOOST_THROW_EXCEPTION(e);
	           }
		   stringstream unit;
		   unit<<"s^-1.";
		   if(getReactantOrder()>0)
	              unit<<getUnits()->getLengthStr()<<"^"<<3*(getReactantOrder()-1)
		       <<"."<<getUnits()->getQuantityStr()<<"^-"<<(getReactantOrder()-1);
		   msUnit u;
		   u.set(unit.str());
		   return u;	      
	    }
	    
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
	    virtual double  computeForwardRateCoefficient() { 
	      
	      throwNotImplemented("double  computeForwardRateCoefficient( const double& T)"); 
	    };
	    
            //! \brief compute the rate of progress for the mixture state
	    virtual double  computeRateOfProgress() { 
	      
	      throwNotImplemented("double  computeForwardRateCoefficient( const double& T)"); 
	    };
	    
	    //@}
	    
	    bool sanityCheck();
	    	    
	protected:
	  
	    void setCalculator(boost::shared_ptr< Antioch::Reaction<double> > calc){
	        Calculator = calc;
		}
	private:
	    
	    bool isReversible;
	    	    
	    msChildren<msReactionRate> ForwardRates;
	    
	    msChild<msChemicalMixture> ChemicalMixture;
	    
	    msChildren<msEntity>  Reactants;
	    msChildren<msEntity>  Products;
	    
	    vector<double> StoechiometryReactants;
	    vector<double> StoechiometryProducts;
	   	    
	    map<string,double> Efficiencies;
	    
	    boost::shared_ptr< Antioch::Reaction<double> >     Calculator; 
	    
	    void throwNotImplemented(string fctName) const {
	      
	        msError e("The class msReaction if virtual",fctName,getFullId());
		BOOST_THROW_EXCEPTION(e);
	    }
		  
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
                
                LOGGER_ENTER_FUNCTION_DBG("void msReactionDerived::update()",getFullId());
		setCalculator( boost::shared_ptr<AntiochCalculatorType>(
		               new AntiochCalculatorType(getChemicalMixture()->noOfEntities(),
							 getId(),1) )  
			      );
		msReaction::update();
                LOGGER_EXIT_FUNCTION2("void msReactionDerived::update()");
            }
            
        public:
            
	    msReactionDerived(): msReaction() {
                
                constructVar(msReactionDerived<AntiochCalculatorType>::nameType,
			     msReactionDerived<AntiochCalculatorType>::nameType,
			     msReactionDerived<AntiochCalculatorType>::nameType);
            }
            
            static boost::shared_ptr<msReactionDerived> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msReactionDerived> msReactionDerived::New()","");
                boost::shared_ptr<msReactionDerived> T( new msReactionDerived() );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msReactionDerived> msReactionDerived::New()");
                return T;
            }
            
            void initialize() {
	      
                LOGGER_ENTER_FUNCTION_DBG("void msReactionDerived::initialize()","");
                msReaction::initialize();
		LOGGER_EXIT_FUNCTION2("void msReactionDerived::initialize()");
            }
            
            
	    double  computeForwardRateCoefficient() {
	      
	       double k=0;
	       
	       try{
	           k = getCastedCalculator<AntiochCalculatorType>()->compute_forward_rate_coefficient( 
	               getChemicalMixture()->getMolarDensities() , 
		       getChemicalMixture()->getTemperature() );
	       }
	       catch(msError& e){
		 
		   e.addContext("double msReactionDerived::computeForwardRateCoefficient()");
		   BOOST_THROW_EXCEPTION(e);
	       }
	       k *= getUnits()->convert(getUnitForwardRateCoefficient().getStr(),1);
	       
	       return k;
	    };

	    double  computeRateOfProgress( const double& T) {
	      
	       if( isReversible && !ChemicalMixture->isDerivedFrom("msThermoMixture") ) {
		 
		   msError e("The computation of the reverse RoP from thermo data for reaction "+getId()+
		             "is required, but mixture is not derived from msThermoMixture ",
	                     "double msReactionDerived::computeRateOfProgress( const double& T) const ",
			     getFullId() );
		   BOOST_THROW_EXCEPTION(e); 
	       }
	       boost::shared_ptr<msThermoMixture> thermo = 
	           ChemicalMixture->impact_static_cast<msThermoMixture>();
	
	       double P0_RT = 1e5/(csts::R*T);
	       double k = getCastedCalculator<AntiochCalculatorType>()->compute_rate_of_progress(
		          ChemicalMixture->getMolarDensities() ,
			  T , P0_RT, thermo->getH_RT_minus_SR() );
	
	       stringstream unitCalc;
	       unitCalc<<"kmol.s^-1";
	       k *= getUnits()->convert(unitCalc.str(),1);
	       
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

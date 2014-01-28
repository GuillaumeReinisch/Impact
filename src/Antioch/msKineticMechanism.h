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


#ifndef PAKineticMechanism_H
#define PAKineticMechanism_H

#include <msThermoMixture.h>
#include <msReaction.h>

#include "antioch/kinetics_evaluator.h"
#include "antioch/reaction_set.h"
#include "antioch/read_reaction_set_data_xml.h"
#include "antioch/kinetics_type.h"

namespace impact {
    
    namespace antioch {
        
        //using namespace atomism;
        /** \brief A kinetic mechanism to describe transformation in chemical mixtures
         *
         * The msKineticMechanism class represents the transformation in chemical mixture (msChemicalMixture)
	 * by a list of reactions that occur between the msChemicalMixture::Entities.
         * It is based on a Antioch::KineticsEvaluator calculator to evaluate the kinetic terms.
	 * 
         */
        class msKineticMechanism : public msPhysicalInterface
        {
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isKineticMechanismRegisteredInPython;
            static msRegister* createInstance() { return new msKineticMechanism; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update() {
                
                IMPACT_LOGIN();
                msPhysicalInterface::update();
		LOGGER_WRITE(msLogger::DEBUG,"create calculator");
		
		IMPACT_EXCEPT_IF( [&](){ return !ChemicalMixture->getCalculator();},
				 "calculator of chemical mixture not set" );
		
		IMPACT_TRY( [&](){ ReactionSet = boost::shared_ptr<Antioch::ReactionSet<double> >( 
		                   new Antioch::ReactionSet<double>( *(ChemicalMixture->getCalculator()) ) );
		});
		
		LOGGER_WRITE(msLogger::DEBUG,"create reactions");
		
		for_each(Reactions.begin(),Reactions.end(),
		        [&](boost::shared_ptr<msReaction> r) {
			  
			  IMPACT_EXCEPT_IF([&](){ return !r->getCalculator();}, "calculator of reaction not set" ); 
			  IMPACT_TRY( [&](){ ReactionSet->add_reaction( r->getCalculator().get() ); 
			  });
		 });	  
		
		LOGGER_WRITE(msLogger::DEBUG,"create calculator");
		IMPACT_TRY( [&](){ Calculator = boost::shared_ptr<Antioch::KineticsEvaluator<double> >(
			     new Antioch::KineticsEvaluator<double>(*ReactionSet,0) ); 
		}); 
				
                IMPACT_LOGOUT();
            }
                       
        public:
            
	    msKineticMechanism(): msPhysicalInterface() {
                
                constructVar("msKineticMechanism","KineticMechanism","KineticMechanism");
            }
            
            void initialize() {
                IMPACT_LOGIN();
                
                msPhysicalInterface::initialize();
		msTreeMapper::declareChild<msChemicalMixture>(ChemicalMixture,msChemicalMixture::New(),"ChemicalMixture");
                msTreeMapper::declareChildren<msReaction>(Reactions,"Reactions");
                
                IMPACT_LOGOUT();
            }
            
            static boost::shared_ptr<msKineticMechanism> New(){
                
                IMPACT_LOGIN_STATIC();
                boost::shared_ptr<msKineticMechanism> T( new msKineticMechanism );
                T->initialize();
                T->update();
                IMPACT_LOGOUT();
                return T;
            }
            
            boost::shared_ptr<msTreeMapper> parseFromXmlCantera2(std::string file);
	    
	    
            boost::shared_ptr<Antioch::KineticsEvaluator<double> > getCalculator(){
                
                return Calculator;
            }
 
            boost::shared_ptr<Antioch::ReactionSet<double> > getReactionSet(){
                
                return ReactionSet;
            }
            //! \brief return the chemical mixture
            boost::shared_ptr<msChemicalMixture> getChemicalMixture() const {
	      
	        return ChemicalMixture.getSharedPtr();
	    }
	    
            boost::shared_ptr<msTreeMapper> setChemicalMixture(boost::shared_ptr<msChemicalMixture> mix){
                
	        IMPACT_LOGIN();
		msTreeMapper::update(ChemicalMixture, mix );
		IMPACT_LOGOUT();
		return mySharedPtr();
            }
            
            boost::shared_ptr<msTreeMapper> addReaction(boost::shared_ptr<msReaction> reaction){
                
                IMPACT_LOGIN();
		msTreeMapper::addElementToChildren(Reactions, reaction );
		//update();
                IMPACT_LOGOUT();
		return mySharedPtr();
            }
            
            std::vector<std::string>  getReactionsNames() const;
            
            std::vector<std::string>  getSpeciesNames() const;
            
            size_t noOfReactions() const;
            
            size_t noOfSpecies() const;
            
            
            /**
             * @name Rate constant
             */
            //@{
            std::vector<double> computeForwardRateCoefficient();
            
          //  std::vector<double> getRevRateConstants();
            std::vector<double> computeRateOfProgress();
            //@}
            
            /**
             * @name Reaction Rates Of Progress
             */
            //@{
            /*
            std::vector<double> getFwdRatesOfProgress();
            
            std::vector<double> getRevRatesOfProgress();
            
            std::vector<double> getNetRatesOfProgress();*/
            //@}
            
            /**
             * @name species creation/destruction rates
             */
            //@{
            /*
            std::vector<double> getCreationRates();
            
            std::vector<double> getDestructionRates();
            
            std::vector<double> getNetProductionRates();
            */
            //@}
            
            std::ostream& print(std::ostream& out) const ;
            
            virtual bool sanityCheck();
	    
	    
        protected:
            
            
           // boost::shared_ptr<msTreeMapper> setCalculator(boost::shared_ptr<Antioch::KineticsEvaluator> calculator);
            
            
            void testCalculator(std::string fct) const{
                
                if( Calculator ) return;
                msException e("The calculators are not initialized, use the 'load' function",fct,getFullId());
                IMPACT_THROW_EXCEPTION(e);
            }
        private:
            
            //!\name attributs and children
            //@{
            
            msChildren<msReaction> Reactions;
	    
	    msChild<msChemicalMixture>  ChemicalMixture;
            //@}
            
	    boost::shared_ptr<Antioch::ReactionSet<double> >        ReactionSet;
            boost::shared_ptr<Antioch::KineticsEvaluator<double> >  Calculator;
                        
        };
    }
}
#endif // PA0DCSTSTP_H

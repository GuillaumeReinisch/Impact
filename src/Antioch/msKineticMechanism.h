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


#include "antioch/kinetics_evaluator.h"
#include "antioch/reaction_set.h"

#include <msChemicalMixture.h>
#include <msReaction.h>
#include <../../VTK6.0.0/ThirdParty/libproj4/vtk_libproj4.h>

namespace impact {
    
    namespace antioch {
        
        //using namespace atomism;
        /** \brief An ideal gas phase
         *
         * A simple wrapper for the class IdealGasMix from Cantera.
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
                
                LOGGER_ENTER_FUNCTION_DBG("void msKineticMechanism::update()",getFullId());
                msPhysicalInterface::update();
		
		ReactionSet = boost::shared_ptr<Antioch::ReactionSet<> >(*ChemicalMixture);
		
		msChildren<msReaction>::iterator it = Reactions.begin();
		for(;it!=Reactions.end();++it) 
		    ReactionSet->add_reaction( (*it)->getCalculator() );
				
		Calculator = boost::shared_ptr<Antioch::KineticsEvaluator>(new Antioch::KineticsEvaluator(*ReactionSet,0) );
		
				
                LOGGER_EXIT_FUNCTION2("void msKineticMechanism::update()");
            }
                       
        public:
            
	    msKineticMechanism(): msPhysicalInterface() {
                
                constructVar("msKineticMechanism","KineticMechanism","KineticMechanism");
            }
            
            void initialize() {
                LOGGER_ENTER_FUNCTION_DBG("void msKineticMechanism::initialize()","");
                
                msPhysicalInterface::initialize();
                msTreeMapper::declareChildren<msReaction>(Reactions,"Reactions");
                
                LOGGER_EXIT_FUNCTION2("void msKineticMechanism::initialize()");
            }
            
            static boost::shared_ptr<msKineticMechanism> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msKineticMechanism> msKineticMechanism::New()","");
                boost::shared_ptr<msKineticMechanism> T( new msKineticMechanism );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msKineticMechanism> msKineticMechanism::New()");
                return T;
            }
            
            
            bool isCalculatorReady() const {if( Calculator ) return 1; return 0;
            }
            
            template<class T>
            boost::shared_ptr<T> getCalculator(){
                
                return boost::static_pointer_cast<T>(Calculator);
            }
            
            
            boost::shared_ptr<msTreeMapper> addReaction(boost::shared_ptr<msReaction> reaction){
                
                LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msKineticMechanism> msKineticMechanism::addReaction(boost::shared_ptr<msReaction> reaction)","");
                msTreeMapper::addElementToChildren(Reactions, reaction );
		update();
                LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msKineticMechanism> msKineticMechanism::addReaction(boost::shared_ptr<msReaction> reaction)");
                
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
            
            //@}
            
            /**
             * @name Reaction Rates Of Progress
             */
            //@{
            
            std::vector<double> getFwdRatesOfProgress();
            
            std::vector<double> getRevRatesOfProgress();
            
            std::vector<double> getNetRatesOfProgress();
            //@}
            
            /**
             * @name species creation/destruction rates
             */
            //@{
            
            std::vector<double> getCreationRates();
            
            std::vector<double> getDestructionRates();
            
            std::vector<double> getNetProductionRates();
            
            //@}
            
            std::ostream& print(std::ostream& out) const ;
            
            //virtual void updateInCalculator(Cantera::Kinetics calculator);
	    
            virtual bool sanityCheck();
	    
	    
        protected:
            
            
           // boost::shared_ptr<msTreeMapper> setCalculator(boost::shared_ptr<Antioch::KineticsEvaluator> calculator);
            
            
            void testCalculator(std::string fct) const{
                
                if( Calculator ) return;
                msError e("The calculators are not initialized, use the 'load' function",fct,getFullId());
                BOOST_THROW_EXCEPTION(e);
            }
        private:
            
            //!\name attributs and children
            //@{
            
            msChildren<msReaction> Reactions;
	    
	    msChild<msChemicalMixture>  ChemicalMixture;
            //@}
            
	    boost::shared_ptr<Antioch::ReactionSet>        ReactionSet;
            boost::shared_ptr<Antioch::KineticsEvaluator>  Calculator;
                        
        };
    }
}
#endif // PA0DCSTSTP_H

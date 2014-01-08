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


#include "msKineticMechanism.h"

namespace impact {
    
    namespace antioch {
        
        bool msKineticMechanism::isKineticMechanismRegisteredInPython = 0;
        
        msRegistrar msKineticMechanism::Registrar("msKineticMechanism", msKineticMechanism::createInstance);
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msKineticMechanism::registryInPython() {
            
#if USE_PYTHON
            
            msPhysicalInterface::registryInPython();
            
            if( ! msKineticMechanism::isKineticMechanismRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msKineticMechanism,bases<msPhysicalInterface>,boost::shared_ptr<msKineticMechanism> >
                ("msKineticMechanism",
                 "Object describing a Polymer phase at the microscopic scale ",no_init
                 )
                .def( "New", &msKineticMechanism::New ,
                     "Create a new object.")
                .staticmethod("New")
                .def( "isCalculatorReady" , &msKineticMechanism::isCalculatorReady,
                     "return true if the calculator has been set.")
                .def( "noOfReactions" ,  &msKineticMechanism::noOfReactions,
                     "return the number of reactions.")
                .def( "getReactionsNames" ,  &msKineticMechanism::getReactionsNames,
                     "return the reaction's name.")
                .def( "noOfSpecies" ,  &msKineticMechanism::noOfSpecies,
                     "return the number of species.")
                .def( "getSpeciesNames" ,  &msKineticMechanism::getSpeciesNames,
                     "return the specie's name.")
                .def( "getCreationRates" ,  &msKineticMechanism::getCreationRates,
                     "return the creation rate of each species in a list.")
                .def( "getDestructionRates" ,  &msKineticMechanism::getDestructionRates,
                     "return the destruction rate of each species in a list.")
                .def( "getNetProductionRates" ,  &msKineticMechanism::getNetProductionRates,
                     "return the net production rate of each species in a list.")
                .def( "getFwdRatesOfProgress", &msKineticMechanism::getFwdRatesOfProgress,
                     "return the forward rate of progress of each reactions in a list.")
                .def( "getRevRatesOfProgress", &msKineticMechanism::getRevRatesOfProgress,
                     "return the reverse rate of progress of each reactions in a list.")
                .def( "getNetRatesOfProgress", &msKineticMechanism::getNetRatesOfProgress,
                     "return the net rate of progress of each reactions in a list.")
                .def( "computeForwardRateCoefficient", &msKineticMechanism::computeForwardRateCoefficient,
                     "return the forward rate constant of each reactions in a list.")
                .def( "getRevRateConstants", &msKineticMechanism::getRevRateConstants,
                     "return the reverse rate constant of each reactions in a list.");
                msKineticMechanism::isKineticMechanismRegisteredInPython = 1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msKineticMechanism::setCalculator(boost::shared_ptr<Cantera::Kinetics> calculator){
            
            Calculator = calculator;
            testCalculator("boost::shared_ptr<msTreeMapper> msKineticMechanism::setCalculator(boost::shared_ptr<Cantera::Kinetics> calculator)");
            return mySharedPtr();
            
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        size_t msKineticMechanism::noOfSpecies() const {
            
            testCalculator("virtual double msKineticMechanism::noOfSpecies() const");
            return Calculator->n_species();
        }
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<std::string> msKineticMechanism::getSpeciesNames() const {
            
            testCalculator("std::vector<std::string> msKineticMechanism::getSpeciesNames() const");
	    
            return ChemicalMixture->getConstituentsNames();
        }
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        size_t msKineticMechanism::noOfReactions() const {
            
            testCalculator("virtual double msKineticMechanism::noOfReactions() const");
            return Calculator->n_reactions();
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<std::string> msKineticMechanism::getReactionsNames() const {
            
            testCalculator("std::vector<std::string> msKineticMechanism::getReactionsNames() const");
            std::vector<std::string> reactions(noOfReactions());
            
            for(size_t i=0; i<noOfReactions(); i++)
                reactions[i] = ReactionSet->reaction(i).equation();
            
            return reactions;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<double> msKineticMechanism::computeForwardRateCoefficient(double T)  {
            
            testCalculator("std::vector<double> msKineticMechanism::getFwdRateConstants()");
            std::vector<double> reactions(noOfReactions(),0);
            
            if(noOfReactions()>0) {
	      
	        msChildren<msReaction>::iterator it = Reactions.begin();
	        for(int i=0;it!=Reactions.end();++it,++i) {
	     
	           reactions[i] = (*it)->computeForwardRateCoefficient(T);
		   reactions[i] *= getUnits()->convert( (*it)->getUnitForwardRateCoefficient(), 1 );
	        }
	    }   
            // Units ?!!!
            return reactions;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        /*
        std::vector<double> msKineticMechanism::getCreationRates()  {
            
            testCalculator("std::vector<double> msKineticMechanism::getCreationRates()");
            std::vector<double> species(noOfSpecies(),0);
            
            if(noOfReactions()==0) return species;
            Calculator->getCreationRates(species.data());
            std::vector<double>::iterator it = species.begin();
            
            double f = getUnits()->convert(msUnit("kmol.m^-3.s^-1"), 1 );
            
            for(;it!=species.end();++it)
                *it *= f;
            
            return species;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<double> msKineticMechanism::getDestructionRates() {
            
            testCalculator("std::vector<double> msKineticMechanism::getDestructionRates()");
            std::vector<double> species(noOfSpecies(),0);
            
            if(noOfReactions()==0) return species;
            Calculator->getDestructionRates(species.data());
            std::vector<double>::iterator it = species.begin();
            
            double f = getUnits()->convert(msUnit("kmol.m^-3.s^-1"), 1 );
            
            for(;it!=species.end();++it)
                *it *= f;
            
            return species;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<double> msKineticMechanism::getNetProductionRates() {
            
            testCalculator("std::vector<double> msKineticMechanism::getNetProductionRates()");
            std::vector<double> species(noOfSpecies(),0);
            
            if(noOfReactions()==0) return species;
            Calculator->getNetProductionRates(species.data());
            std::vector<double>::iterator it = species.begin();
            
            double f = getUnits()->convert(msUnit("kmol.m^-3.s^-1"), 1 );
            
            for(;it!=species.end();++it)
                *it *= f;
            
            return species;
        }
        */
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<double> msKineticMechanism::computeRateOfProgress() {
            
            testCalculator("virtual double msKineticMechanism::computeRateOfProgress()");
            std::vector<double> reactions(noOfReactions(),0);
            
            if(noOfReactions()==0) return reactions;
            
            Calculator->c();
            std::vector<double>::iterator it = reactions.begin();
            
            double f = getUnits()->convert(msUnit("kmol.m^-3.s^-1"), 1 );
            
            for(;it!=reactions.end();++it)
                *it *= f;
            
            return reactions;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<double> msKineticMechanism::getRevRatesOfProgress() {
            
            testCalculator("std::vector<double> msKineticMechanism::getRevRatesOfProgress()");
            std::vector<double> reactions(noOfReactions(),0);
            
            if(noOfReactions()==0) return reactions;
            
            Calculator->getRevRatesOfProgress(reactions.data());
            
            std::vector<double>::iterator it = reactions.begin();
            
            double f = getUnits()->convert(msUnit("kmol.m^-3.s^-1"), 1 );
            
            for(;it!=reactions.end();++it)
                *it *= f;
            
            return reactions;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<double> msKineticMechanism::getNetRatesOfProgress() {
            
            testCalculator("std::vector<double> msKineticMechanism::getNetRatesOfProgress()");
            std::vector<double> reactions(noOfReactions(),0);
            
            if(noOfReactions()==0) return reactions;
            
            Calculator->getNetRatesOfProgress(reactions.data());
            
            std::vector<double>::iterator it = reactions.begin();
            
            double f = getUnits()->convert(msUnit("kmol.m^-3.s^-1"), 1 );
            
            for(;it!=reactions.end();++it)
                *it *= f;
            
            return reactions;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::ostream& msKineticMechanism::print(std::ostream& out) const {
            
            msPhysicalInterface::print(out);
            output::printHeader(out,"Kinetic mechanism");
            
            out<<"a description to set up"<<endl;
            return out;
        }
        
        
        bool msKineticMechanism::sanityCheck() {
	  
	  
	   if( Calculator->n_species() != ChemicalMixture->noOfConstituents() ){
	     
	     msError e("Unconsistent number of species in Calculator and ChemicalMixture",
		"",getFullId());
	     
	     BOOST_THROW_EXCEPTION(e);
	   }
	   
	   vector<string> names = getReactionsNames();
	   msChildren<msReaction>::iterator it = Reactions.begin();
	   for(int i=0;it!=Reactions.end();++it,++i) {
	     
	       if( (*it)->getId() != names[i] ){
		 
		   msError e ("The name of the reaction number "+output::getString<int>(i)
		             +" is not consistent between children Reactions and calculator ReactionSet",
		             getFullId());
		  BOOST_THROW_EXCEPTION(e);
	      }
	  }
		
	   return 1;
	  }
	}
    }
}

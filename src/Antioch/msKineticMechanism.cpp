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
		.def( "parseFromXmlCantera2" , &msKineticMechanism::parseFromXmlCantera2,
                     "parse from a xml mechanism file like cantera. arg2: filename.")
                .def( "setChemicalMixture" , &msKineticMechanism::setChemicalMixture,
                     "set the chemical mixture. arg2: chemical mixture")
		.def( "addReaction" , &msKineticMechanism::addReaction,
                     "set the chemical mixture. arg2: reaction")
                .def( "noOfReactions" ,  &msKineticMechanism::noOfReactions,
                     "return the number of reactions.")
                .def( "getReactionsNames" ,  &msKineticMechanism::getReactionsNames,
                     "return the reaction's name.")
                .def( "noOfSpecies" ,  &msKineticMechanism::noOfSpecies,
                     "return the number of species.")
                .def( "getSpeciesNames" ,  &msKineticMechanism::getSpeciesNames,
                     "return the specie's name.")/*
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
                     "return the net rate of progress of each reactions in a list.")*/
                .def( "computeForwardRateCoefficient", &msKineticMechanism::computeForwardRateCoefficient,
                     "return the forward rate constant of each reactions in a list.");
                msKineticMechanism::isKineticMechanismRegisteredInPython = 1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<double> msKineticMechanism::computeForwardRateCoefficient()  {
            
	    IMPACT_LOGIN();
            IMPACT_EXCEPT_IF([&](){return !getCalculator();},"No calculator defined.");
	    std::vector<double> reactions(noOfReactions(),0);
	    
	    if (noOfReactions()==0) { IMPACT_LOGOUT(); return reactions;} 
	    
	    size_t i=-1;
            for_each(Reactions.begin(),Reactions.end(),[&](boost::shared_ptr<msReaction> r) {
	      
	        double f = getUnits()->convert( r->getUnitForwardRateCoefficient(), 1 ) ;
	        reactions[i++] = f * r->computeForwardRateCoefficient();
	    });
	    
            IMPACT_LOGOUT();
	    return reactions;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<double> msKineticMechanism::computeRateOfProgress() {
            
            IMPACT_LOGIN();
            IMPACT_EXCEPT_IF([&](){return !getCalculator();},"No calculator defined.");
            std::vector<double> sources(ChemicalMixture->noOfEntities(),0);
            
            if(ChemicalMixture->noOfEntities()==0) return sources;
	    
	    if(ChemicalMixture->isDerivedFrom("msThermoMixture"))	      
                Calculator->compute_mole_sources(ChemicalMixture->getTemperature(),
					         ChemicalMixture->getMolarDensities(),
						 ChemicalMixture->impact_static_cast<msThermoMixture>()->getH_RT_minus_SR(),
						 sources
						 );
	    else Calculator->compute_mole_sources(ChemicalMixture->getTemperature(),
					         ChemicalMixture->getMolarDensities(),
						 vector<double>(ChemicalMixture->noOfEntities(),0),
						 sources
						 );
            /*std::vector<double>::iterator it = reactions.begin();
            
            double f = getUnits()->convert(msUnit("kmol.m^-3.s^-1"), 1 );
            
            for(;it!=reactions.end();++it)
                *it *= f;*/
            IMPACT_LOGOUT();
	    return sources;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::ostream& msKineticMechanism::print(std::ostream& out) const {
            
            msPhysicalInterface::print(out);
            output::printHeader(out,"Kinetic mechanism");
            
            out<<"a description to set up"<<endl;
            return out;
        }
                
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        bool msKineticMechanism::sanityCheck() {
	  
	   IMPACT_EXCEPT_IF([&](){return Calculator->n_species() != ChemicalMixture->noOfEntities();},
			    "Unconsistent number of species in Calculator and ChemicalMixture");
	   
	   vector<string> names = getReactionsNames();
	   size_t i=-1;
	   for_each(Reactions.begin(),Reactions.end(), [&](boost::shared_ptr<msReaction> r){
	     
	      IMPACT_EXCEPT_IF([&](){ return r->getId() != names[i++];},
			       "Reaction: "+r->getId()+": unconsistent name with calculator");
	      
	      IMPACT_EXCEPT_IF([&](){ return getChemicalMixture() != r->getChemicalMixture();},
			       "Reaction: "+r->getId()+": chemical mixture linked not consistent.");
	   });
		
	   return 1;
        }
        
	//-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper>  msKineticMechanism::parseFromXmlCantera2(std::string filename){
	  
	    IMPACT_LOGIN();
            IMPACT_EXCEPT_IF([&](){return getChemicalMixture()->noOfEntities()==0;},
			     "No entities are defined in the chemical mixture" ); 
	    
	    LOGGER_WRITE(msLogger::DEBUG,"Load the mechanism of filename "+filename);
	    IMPACT_TRY([&](){Antioch::read_reaction_set_data_xml(filename,1, *getReactionSet() );});
	    
	    LOGGER_WRITE(msLogger::DEBUG,"Create the children structure");
	    
	    getUnits()->set("");
	    ReactionSet = boost::shared_ptr<Antioch::ReactionSet<double> >( 
	                  new Antioch::ReactionSet<double>(*(getChemicalMixture()->getCalculator())));
	    vector<string> names = getChemicalMixture()->getEntitiesNames();
	    
	    for(size_t i=0;i <ReactionSet->n_reactions();i++) {
	       
	         LOGGER_WRITE(msLogger::DEBUG,"Reaction " + ReactionSet->reaction(i).equation());
		 
		 boost::shared_ptr<msReaction> reaction;
	         const Antioch::Reaction<double>& reactionAntioch = ReactionSet->reaction(i);
		 Antioch::ReactionType::ReactionType typeReaction;
                 typeReaction = reactionAntioch.type();
		 string id = reactionAntioch.equation();
		 boost::replace_all(id,"[","<");boost::replace_all(id,"=","-");boost::replace_all(id,"]",">");
		 
	         IMPACT_EXCEPT_IF([&](){return typeReaction>4;} , "The reaction type is not known.");
		 switch(typeReaction){
		   
		     case Antioch::ReactionType::ELEMENTARY :
		          reaction = msReactionDerived<Antioch::ElementaryReaction<double> >::New(); 
			  break;
		     case Antioch::ReactionType::DUPLICATE :
		          reaction = msReactionDerived<Antioch::DuplicateReaction<double> >::New(); 
			  break;
		     case Antioch::ReactionType::THREE_BODY :
		          reaction = msReactionDerived<Antioch::ThreeBodyReaction<double> >::New();
			  break;
		     case Antioch::ReactionType::LINDEMANN_FALLOFF :
		          reaction = msReactionDerived<Antioch::FalloffReaction<double,Antioch::LindemannFalloff<double> > >::New(); 
			  break;
		     case Antioch::ReactionType::TROE_FALLOFF :
		          reaction = msReactionDerived<Antioch::FalloffReaction<double,Antioch::TroeFalloff<double> > >::New(); 
			  break;
		 }
		 reaction->setId(id);
		 reaction->setChemicalMixture(getChemicalMixture());
		 reaction->setUnits(getUnits());
		 reaction->setAffiliation(mySharedPtr());
		 reaction->setId(reactionAntioch.equation());
         	 addReaction(reaction);
		 
                 LOGGER_WRITE(msLogger::DEBUG,"Add reactants and products");
		 
                 IMPACT_TRY( [&](){
                     for( size_t r=0; r<reactionAntioch.n_reactants(); r++)		  
		         reaction->addReactant(ChemicalMixture->getEntityFromId(reactionAntioch.reactant_name(r)),
			  		       reactionAntioch.reactant_stoichiometric_coefficient(r));
		   
		     for( size_t r=0; r<reactionAntioch.n_products(); r++)		  
		         reaction->addProduct(ChemicalMixture->getEntityFromId(reactionAntioch.product_name(r)),
					       reactionAntioch.product_stoichiometric_coefficient(r));
		 });
		 
		 LOGGER_WRITE(msLogger::DEBUG,"Set efficiencies");
		 
		 for(size_t j=0;j<getChemicalMixture()->noOfEntities();j++)		  
		     if(reactionAntioch.efficiency(j)!=1) reaction->setEfficiency(names[j],1);
		
		 LOGGER_WRITE(msLogger::DEBUG,"Create rate constants");
		                  
		 for( size_t j=0 ; j<reactionAntioch.n_rate_constants(); j++) {
		   
		      LOGGER_WRITE(msLogger::DEBUG,"rate N°"+output::getString(i));
		      
		      boost::shared_ptr<msReactionRate> rate;
		      const Antioch::KineticsType<double>&  rateAntioch   = reactionAntioch.forward_rate(j);
		      Antioch::KineticsModel::KineticsModel kineticsModel = rateAntioch.type();
		      
		      LOGGER_WRITE(msLogger::DEBUG,"Create rate constants of type "+output::getString(kineticsModel));
		      IMPACT_EXCEPT_IF([&](){return kineticsModel>5;} , "The kinetics type is not known.");
		      
		      switch(kineticsModel){
		   
		         case Antioch::KineticsModel::ARRHENIUS :
		             rate =  msReactionRateDerived<Antioch::ArrheniusRate<double> >::New(); 
			     break;
		         case Antioch::KineticsModel::KOOIJ :
		             rate =  msReactionRateDerived<Antioch::KooijRate<double> >::New(); 
			     break;
		         case Antioch::KineticsModel::BERTHELOT :
		             rate = msReactionRateDerived<Antioch::BerthelotRate<double> >::New();
			     break;
		         case Antioch::KineticsModel::HERCOURT_ESSEN :
		             rate = msReactionRateDerived<Antioch::HercourtEssenRate<double> >::New(); 
			     break;
		         case Antioch::KineticsModel::BHE :
		             rate = msReactionRateDerived<Antioch::BerthelotHercourtEssenRate<double> >::New(); 
			     break;
		         case Antioch::KineticsModel::VANTHOFF :
		             rate = msReactionRateDerived<Antioch::VantHoffRate<double> >::New(); 
			     break;
		      }
		      rate->setId("rate "+id);
	              rate->setCoefficients(&rateAntioch);
		      rate->setUnits(getUnits());
		      rate->setAffiliation(reaction);
		      //rate->setOrder(reaction->getReactantOrder());
		      reaction->addForwardRate(rate);
		 }
	    } 
	IMPACT_LOGOUT();
        return mySharedPtr();   
	}
    }
}

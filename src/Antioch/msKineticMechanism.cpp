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
        
        size_t msKineticMechanism::noOfSpecies() const {
            
            testCalculator("virtual double msKineticMechanism::noOfSpecies() const");
            return Calculator->n_species();
        }
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::vector<std::string> msKineticMechanism::getSpeciesNames() const {
            
            testCalculator("std::vector<std::string> msKineticMechanism::getSpeciesNames() const");
	    
            return ChemicalMixture->getEntitiesNames();
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
        
        std::vector<double> msKineticMechanism::computeForwardRateCoefficient()  {
            
            testCalculator("std::vector<double> msKineticMechanism::getFwdRateConstants()");
            std::vector<double> reactions(noOfReactions(),0);
            double T = ChemicalMixture->getTemperature();

            if(noOfReactions()>0) {
	      
	        msChildren<msReaction>::iterator it = Reactions.begin();
	        for(int i=0;it!=Reactions.end();++it,++i) {
	     
	           reactions[i] = (*it)->computeForwardRateCoefficient();
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
            
            return sources;
        }
        
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        /*
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
        */
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        std::ostream& msKineticMechanism::print(std::ostream& out) const {
            
            msPhysicalInterface::print(out);
            output::printHeader(out,"Kinetic mechanism");
            
            out<<"a description to set up"<<endl;
            return out;
        }
        
        
        bool msKineticMechanism::sanityCheck() {
	  
	  
	   if( Calculator->n_species() != ChemicalMixture->noOfEntities() ){
	     
	     msException e("Unconsistent number of species in Calculator and ChemicalMixture",
		"",getFullId());
	     
	     IMPACT_THROW_EXCEPTION(e);
	   }
	   
	   vector<string> names = getReactionsNames();
	   msChildren<msReaction>::iterator it = Reactions.begin();
	   for(int i=0;it!=Reactions.end();++it,++i) {
	     
	      if( (*it)->getId() != names[i] ){
		 
		   msException e ("The name of the reaction number "+output::getString<int>(i)
		             +" is not consistent between children Reactions and calculator ReactionSet",
		             "bool msKineticMechanism::sanityCheck()",getFullId());
		  IMPACT_THROW_EXCEPTION(e);
	      }
	      if( getChemicalMixture() != (*it)->getChemicalMixture()){
		 
		   msException e ("The chemical mixture of the mechanism and of the reaction number "+output::getString<int>(i)
		             +" are not consistents.",
		             "bool msKineticMechanism::sanityCheck()",getFullId());
		  IMPACT_THROW_EXCEPTION(e);
	      }
	  }
		
	   return 1;
        }
        
	//-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper>  msKineticMechanism::parseFromXmlCantera2(std::string filename){
	  
	    IMPACT_LOGIN();
                
	    getUnits()->set("");
	    if( getChemicalMixture()->noOfEntities() == 0 ) {
	             
	         stringstream out;
	         out << "No entities are defined in the chemical mixture";
		 IMPACT_THROW_EXCEPTION(msException(out.str(),"boost::shared_ptr<msTreeMapper>  msKineticMechanism::parseFromXmlCantera2(std::string filename)",getFullId()));  
	    }
	    
	    ReactionSet = boost::shared_ptr<Antioch::ReactionSet<double> >( new Antioch::ReactionSet<double>(*(getChemicalMixture()->getCalculator())) );
	    
	    vector<string> names = getChemicalMixture()->getEntitiesNames();
	    
	    LOGGER_WRITE(msLogger::DEBUG,"Load the mechanism of filename "+filename);
	    
	    try{
	        Antioch::read_reaction_set_data_xml(filename,1, *getReactionSet() );
	    }
	    catch(std::exception& e0){
	      
	         stringstream out;
	         out << "Problem while parsing the file "<<filename<<" : "<<e0.what();
		 IMPACT_THROW_EXCEPTION(msException(out.str(),"boost::shared_ptr<msTreeMapper>  msKineticMechanism::parseFromXmlCantera2(std::string filename)",getFullId()));  
	    }
	    
	    LOGGER_WRITE(msLogger::DEBUG,"Create the children structure");
	    
	    for(size_t i=0;i <ReactionSet->n_reactions();i++) {
	       
	         LOGGER_WRITE(msLogger::DEBUG,"Reaction " + ReactionSet->reaction(i).equation());
		 
	         const Antioch::Reaction<double>& reactionAntioch = ReactionSet->reaction(i);
		 
		 Antioch::ReactionType::ReactionType typeReaction;
                 
		 typeReaction = reactionAntioch.type();
		 
		 boost::shared_ptr<msReaction> reaction;
	         
		 if (typeReaction == Antioch::ReactionType::ELEMENTARY )
		     reaction = msReactionDerived<Antioch::ElementaryReaction<double> >::New();
		 else 
		 if (typeReaction == Antioch::ReactionType::DUPLICATE )
		     reaction = msReactionDerived<Antioch::DuplicateReaction<double> >::New();
		 else 
		 if (typeReaction == Antioch::ReactionType::THREE_BODY )
		     reaction = msReactionDerived<Antioch::ThreeBodyReaction<double> >::New();
		 else 
		 if (typeReaction == Antioch::ReactionType::LINDEMANN_FALLOFF )
		     reaction = msReactionDerived<  Antioch::FalloffReaction<double,Antioch::LindemannFalloff<double> > >::New();
		 else 
		 if (typeReaction == Antioch::ReactionType::TROE_FALLOFF )
		     reaction = msReactionDerived< Antioch::FalloffReaction<double,Antioch::TroeFalloff<double> > >::New();
	         else {
		     stringstream out;
                     out << "The reaction type is not known.";
		     IMPACT_THROW_EXCEPTION(msException(out.str(),"boost::shared_ptr<msTreeMapper>  msKineticMechanism::parseFromXmlCantera2(std::string filename)",getFullId()));
                 }
                 
         	 addReaction(reaction);
		 reaction->setChemicalMixture(getChemicalMixture());
		 reaction->setUnits(getUnits());
		 reaction->setAffiliation(mySharedPtr());
		 reaction->setId(reactionAntioch.equation());
		 
                 LOGGER_WRITE(msLogger::DEBUG,"Add reactants and products");
		 
                 try{
                     for( size_t r=0; r<reactionAntioch.n_reactants(); r++)		  
		         reaction->addReactant(ChemicalMixture->getEntityFromId(reactionAntioch.reactant_name(r)),
			  		       reactionAntioch.reactant_stoichiometric_coefficient(r));
		   
		     for( size_t r=0; r<reactionAntioch.n_products(); r++)		  
		         reaction->addProduct(ChemicalMixture->getEntityFromId(reactionAntioch.product_name(r)),
					       reactionAntioch.product_stoichiometric_coefficient(r));
		 }
		 catch(msException& e){
		   
		     e.addContext("boost::shared_ptr<msTreeMapper>  msKineticMechanism::parseFromXmlCantera2(std::string filename) : problem while setting reactants or products.");
		     IMPACT_THROW_EXCEPTION(e);
		 }
		 LOGGER_WRITE(msLogger::DEBUG,"Set efficiencies");
		 
		 for(size_t i=0;i<getChemicalMixture()->noOfEntities();i++)		  
		     if(reactionAntioch.efficiency(i)!=1) reaction->setEfficiency(names[i],1);
		
		 double conversionEa = getUnits()->convert("J.mol^-1.K^-1",csts::R);
		 double conversionCf = getUnits()->convert(reaction->getUnitForwardRateCoefficient(),1);
		 
		 LOGGER_WRITE(msLogger::DEBUG,"Create rate constants");
		 for( size_t j=0 ; j<reactionAntioch.n_rate_constants(); j++) {
		   
		      stringstream out;
		      out<<"Create rate constants "<<j<<" on "<<reactionAntioch.n_rate_constants()<<endl;
		      LOGGER_WRITE(msLogger::DEBUG,out.str());
		       
		      const Antioch::KineticsType<double>& rateAntioch = reactionAntioch.forward_rate(j);
		      
		      Antioch::KineticsModel::KineticsModel kineticsModel = rateAntioch.type();
		      
		      boost::shared_ptr<msReactionRate> rate;
		      
		      if (kineticsModel == Antioch::KineticsModel::ARRHENIUS ){
			
			  LOGGER_WRITE(msLogger::DEBUG,"Create ARRHENIUS rate constants");
			  
			  rate = msReactionRateDerived<Antioch::ArrheniusRate<double> >::New();
			  /*rate->setId(reactionAntioch.equation());
			  const Antioch::ArrheniusRate<double>& rateAntiochCasted =
			        *( static_cast<const Antioch::ArrheniusRate<double>*>(&rateAntioch) );
		          rate->setCoefficient("Cf",rateAntiochCasted.Cf()*conversionCf);
			  rate->setCoefficient("Ea",rateAntiochCasted.Ea()*conversionEa);*/
		      }
		      else
		      if (kineticsModel == Antioch::KineticsModel::KOOIJ ){
			
			  LOGGER_WRITE(msLogger::DEBUG,"Create KOOIJ rate constants");
			  
		          rate = msReactionRateDerived<Antioch::KooijRate<double> >::New();
			  rate->setId(reactionAntioch.equation());
			  const Antioch::KooijRate<double>& rateAntiochCasted =
			        *( static_cast<const Antioch::KooijRate<double>*>(&rateAntioch) );
		          rate->setCoefficient("Cf",rateAntiochCasted.Cf()*conversionCf);
			  rate->setCoefficient("eta",rateAntiochCasted.eta());
			  rate->setCoefficient("Ea",rateAntiochCasted.Ea()*conversionEa);
		      }
		      else
		      if (kineticsModel == Antioch::KineticsModel::BERTHELOT ){
			
			  LOGGER_WRITE(msLogger::DEBUG,"Create BERTHELOT rate constants");
			  
			  rate = msReactionRateDerived<Antioch::BerthelotRate<double> >::New();
			  rate->setId(reactionAntioch.equation());
			  const Antioch::BerthelotRate<double>& rateAntiochCasted =
			        *( static_cast<const Antioch::BerthelotRate<double>*>(&rateAntioch) );
		          rate->setCoefficient("Cf",rateAntiochCasted.Cf()*conversionCf);
			  rate->setCoefficient("D",rateAntiochCasted.D());
		          rate = msReactionRateDerived<Antioch::BerthelotRate<double> >::New();
		      }
		      else
		      if (kineticsModel == Antioch::KineticsModel::HERCOURT_ESSEN ){
			
			  LOGGER_WRITE(msLogger::DEBUG,"Create HERCOURT_ESSEN rate constants");
			  
		          rate = msReactionRateDerived<Antioch::HercourtEssenRate<double> >::New();
			  rate->setId(reactionAntioch.equation());
			  const Antioch::HercourtEssenRate<double>& rateAntiochCasted =
			        *( static_cast<const Antioch::HercourtEssenRate<double>*>(&rateAntioch) );
		          rate->setCoefficient("Cf",rateAntiochCasted.Cf()*conversionCf);
			  rate->setCoefficient("eta",rateAntiochCasted.eta());
			  rate->setCoefficient("Tref",rateAntiochCasted.Tref());
		          rate = msReactionRateDerived<Antioch::BerthelotRate<double> >::New();
		      }
		      else
		      if (kineticsModel == Antioch::KineticsModel::BHE ) {
			
			  LOGGER_WRITE(msLogger::DEBUG,"Create BHE rate constants");
			  
		          rate = msReactionRateDerived<Antioch::BerthelotHercourtEssenRate<double> >::New();
			  rate->setId(reactionAntioch.equation());
			  const Antioch::BerthelotHercourtEssenRate<double>& rateAntiochCasted =
			        *( static_cast<const Antioch::BerthelotHercourtEssenRate<double>*>(&rateAntioch) );
		          rate->setCoefficient("Cf",rateAntiochCasted.Cf()*conversionCf);
			  rate->setCoefficient("eta",rateAntiochCasted.eta());
			  rate->setCoefficient("Tref",rateAntiochCasted.Tref());
			  rate->setCoefficient("D",rateAntiochCasted.D());
		          rate = msReactionRateDerived<Antioch::BerthelotRate<double> >::New();
		      }
		      else
		      if (kineticsModel == Antioch::KineticsModel::VANTHOFF ) {
			
			  LOGGER_WRITE(msLogger::DEBUG,"Create VANTHOFF rate constants");
			  
		          rate = msReactionRateDerived<Antioch::VantHoffRate<double> >::New();
			  rate->setId(reactionAntioch.equation());
			  const Antioch::VantHoffRate<double>& rateAntiochCasted =
			        *( static_cast<const Antioch::VantHoffRate<double>*>(&rateAntioch) );
		          rate->setCoefficient("Cf",rateAntiochCasted.Cf()*conversionCf);
			  rate->setCoefficient("eta",rateAntiochCasted.eta());
			  rate->setCoefficient("Tref",rateAntiochCasted.Tref());
			  rate->setCoefficient("D",rateAntiochCasted.D());
			  rate->setCoefficient("Ea",rateAntiochCasted.Ea()*conversionEa);
		          rate = msReactionRateDerived<Antioch::BerthelotRate<double> >::New();
		      }
	              else {
		          stringstream out;
                          out << "The kinetic type is not known.";
		          IMPACT_THROW_EXCEPTION(msException(out.str(),"boost::shared_ptr<msTreeMapper>  msKineticMechanism::parseFromXmlCantera2(std::string filename)",getFullId()));
                      }
                      rate->setUnits(getUnits());
		      rate->setAffiliation(reaction);
		      //rate->setOrder(reaction->getReactantOrder());
		      reaction->addForwardRate(rate);
		 }
		 
		 string id = reactionAntioch.equation();
		 for (int i = 0; i < id.length(); ++i) {
		     if (id[i] == '[') id[i] = '<';
		     if (id[i] == '=') id[i] = '-';
		     if (id[i] == ']') id[i] = '>';
                 }
                 reaction->setId(id);
	    } 
	IMPACT_LOGOUT();
        return mySharedPtr();   
	}
    }
}

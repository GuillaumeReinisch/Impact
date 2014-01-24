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


#include "msReaction.h"
#include <boost/graph/graph_concepts.hpp>

namespace impact {
    
    namespace antioch {
        
      
        bool msReaction::isReactionRegisteredInPython = 0;
        msRegistrar msReaction::Registrar("msReaction", msReaction::createInstance);
        
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msReaction::registryInPython() {
            
#if USE_PYTHON
            
            msPhysicalInterface::registryInPython();
            
            if( ! msReaction::isReactionRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msReaction,bases<msPhysicalInterface>,boost::shared_ptr<msReaction> >
                ("msReaction",
                 "Object describing a reaction",no_init
                 )
                .def( "New", &msReaction::New ,
                     "Create a new object.")
                .staticmethod("New")
                .def("computeForwardRateCoefficient",&msReaction::computeForwardRateCoefficient,
                     "return the forward rate coefficient")
                .def("computeRateOfProgress",&msReaction::computeRateOfProgress,
                     "return the rate of progress")
		.def("getChemicalMixture",&msReaction::getChemicalMixture,
		     "return the chemical mixture")
		.def("setChemicalMixture",&msReaction::setChemicalMixture,
		     "set the chemical mixture. arg2: chemical mixture")
		.def("addReactant",&msReaction::addReactant,
		     "add a reactant to the reacton. arg2: reactant; arg3: stoechiometry")
		.def("addProduct",&msReaction::addProduct,
		     "add a product to the reacton. arg2: product; arg3: stoechiometry")
		.def("getForwardRates",&msReaction::getForwardRates,
		     "return the forward rates.")
		.def("addForwardRate",&msReaction::addForwardRate,
		     "add a forward rate expression to the manager. arg2: rate")
		.def("setReversibility",&msReaction::setReversibility,
		     "set the reversibility of the reaction. arg2: reversibility")
		.def("setReactantStoechiometry",&msReaction::setReactantStoechiometry,
		     "set the the stoechiometry of a reactant. arg2: index of the reactant; arg3: coeff")
		.def("setProductStoechiometry",&msReaction::setProductStoechiometry,
		     "set the the stoechiometry of a product. arg2: index of the product; arg3: coeff")
		.def("getReactants",&msReaction::getReactants,
		     "return the reactants.")
		.def("getProducts",&msReaction::getProducts,
		     "return the products.")
		.def("getStoechiometryReactants",&msReaction::getStoechiometryReactants,
		     "return stoechiometry of the reactants.")
		.def("getStoechiometryProducts",&msReaction::getStoechiometryProducts,
		     "return stoechiometry of the products.")
		.def("getReversibility",&msReaction::getReversibility,
		     "return true if the reaction is reversible.")
		.def("getUnitForwardRateCoefficient",&msReaction::getUnitForwardRateCoefficient,
		     "return the unit of the forward rate coefficient");
                
		msTreeMapper::finalizeDeclarationForPython<msReaction>("msReaction");
                msReaction::isReactionRegisteredInPython = 1;
                }
#endif
	}
	
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         	
        bool msReaction::sanityCheck() {
	   
	    stringstream errors;
	    
	    if( Reactants.size() == 0 ) errors<<" - No reactants defined"<<endl;
	    
	    if( Products.size() == 0 )  errors<<" - No products defined"<<endl;	 
	    
	    if( find_if(StoechiometryReactants.begin(),StoechiometryReactants.end(),
	        [](double const& item) { return item<=0;}) != StoechiometryReactants.end() 
	      )
	        errors<<" - A reactant stoechiometry coefficient is null or negative"<<endl;
	    
	    if( find_if(StoechiometryProducts.begin(),StoechiometryProducts.end(), 
	        [](double const& item) { return item<=0;}) != StoechiometryProducts.end()  
	      )
	        errors<<" - A product stoechiometry coefficient is null or negative"<<endl;
	    
	    if( ForwardRates.size() == 0 ) errors<<" - No forward rates defined "<<endl;
	      
	    if( errors.str() != "") 
	        BOOST_THROW_EXCEPTION(msError(errors.str(),"bool msReaction::sanityCheck()",getFullId()));
	}

        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
         		    
        template<>
        bool msReactionDerived<Antioch::ElementaryReaction<double> >::isReactionDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msReactionDerived<Antioch::ElementaryReaction<double> >::Registrar(
	    "msElementaryReaction", 
	     msReactionDerived<Antioch::ElementaryReaction<double> >::createInstance);
	
        template<>
        string msReactionDerived<Antioch::ElementaryReaction<double> >::doc = "Elementary reaction";
        template<>
        string msReactionDerived<Antioch::ElementaryReaction<double> >::nameType = "msElementaryReaction";      
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
	
        template<>
        bool msReactionDerived<Antioch::DuplicateReaction<double> >::isReactionDerivedRegisteredInPython = 0;
	
	template<>
        msRegistrar msReactionDerived<Antioch::DuplicateReaction<double> >::Registrar(
	    "msDuplicateReaction", 
	     msReactionDerived<Antioch::DuplicateReaction<double> >::createInstance);
	
        template<>
        string msReactionDerived<Antioch::DuplicateReaction<double> >::doc = "DuplicateReaction";
        template<>
        string msReactionDerived<Antioch::DuplicateReaction<double> >::nameType = "msDuplicateReaction";      
              
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
	
	template<>
        bool msReactionDerived<Antioch::ThreeBodyReaction<double> >::isReactionDerivedRegisteredInPython = 0;
	
	template<>
        msRegistrar msReactionDerived<Antioch::ThreeBodyReaction<double> >::Registrar(
	    "msThreeBodyReaction", 
	     msReactionDerived<Antioch::ThreeBodyReaction<double> >::createInstance);
	
	template<>
        string msReactionDerived<Antioch::ThreeBodyReaction<double> >::doc = "ThreeBodyReaction";
        template<>
        string msReactionDerived<Antioch::ThreeBodyReaction<double> >::nameType = "msThreeBodyReaction";   
	
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
	
	template<>
        bool msReactionDerived< Antioch::FalloffReaction<double,Antioch::LindemannFalloff<double> > >::isReactionDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msReactionDerived<Antioch::FalloffReaction<double,Antioch::LindemannFalloff<double> > >::Registrar(
	    "msLindemannFalloff", 
	     msReactionDerived<Antioch::FalloffReaction<double,Antioch::LindemannFalloff<double> > >::createInstance);
	
	template<>
        string msReactionDerived<Antioch::FalloffReaction<double,Antioch::LindemannFalloff<double> > >::doc = "LindemannFalloff";
        template<>
        string msReactionDerived<Antioch::FalloffReaction<double,Antioch::LindemannFalloff<double> > >::nameType = "msLindemannFalloff";  
        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
	
	template<>
        bool msReactionDerived<Antioch::FalloffReaction<double,Antioch::TroeFalloff<double> > >::isReactionDerivedRegisteredInPython = 0;
	template<>
        msRegistrar msReactionDerived<Antioch::FalloffReaction<double,Antioch::TroeFalloff<double> > >::Registrar(
	    "msTroeFalloff", 
	     msReactionDerived<Antioch::FalloffReaction<double,Antioch::TroeFalloff<double> > >::createInstance);
	
	template<>
        string msReactionDerived<Antioch::FalloffReaction<double,Antioch::TroeFalloff<double> > >::doc = "TroeFalloff";
        template<>
        string msReactionDerived<Antioch::FalloffReaction<double,Antioch::TroeFalloff<double> > >::nameType = "msTroeFalloff";                 

        //-------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------
     
        
    }
}

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


#ifndef msTHERMOMIXTURE_H
#define msTHERMOMIXTURE_H

#include <msMolecule.h>
#include <msChemicalMixture.h>
#include <msMotionCEA.h>
#include <msAtom.h>

#include "antioch/chemical_mixture.h"
#include "antioch/cea_evaluator.h"
#include "antioch/cea_mixture.h"
#include "antioch/cea_thermo.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace impact {
    
    namespace antioch {
        
        using namespace atomism;
        
        /** \brief A mixture of components with thermodynamic manager
         *
         * The msThermoMixture class represents a msChemicalMixture that implements a thermodynamic manager to 
	 * compute entropies, enthaplies, calorific capacities. It is based on the calculator Antioch::CEAEvaluator.
	 * 
	 * Example of use:
	 * ~~~~~~~~~~~~~{.py}
         from libimpact import *
         
         mixture = msThermoMixture.New().parseFromCeaAscii("inputCEA.txt")
         
	 moleFractions = mixture.getMoleFractions()
	 names         = mixture.getEntitiesNames()
	 Ss            = mixture.getSs()
	 unitS         = mixture.getUnits().getStr(msUnit.vEntropyByQuantity)
	 
	 for (name,moleFrac,s) in itertools.izip(names,moleFractions,Ss):
	     print name,moleFrac,s,unitS
	     
         * ~~~~~~~~~~~~~
         */
        class msThermoMixture : public msChemicalMixture
        {
        public:
           
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isThermoMixtureRegisteredInPython;
            static msRegister* createInstance() { return new msThermoMixture; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update() {
                
                IMPACT_LOGIN();
                msChemicalMixture::update();
		
		IMPACT_TRY([&](){
		    CeaMixture = boost::shared_ptr<Antioch::CEAThermoMixture<double> >(new Antioch::CEAThermoMixture<double>(*(getCalculator())) );
		});
				
		vector<string> names=getEntitiesNames();
		
		for(size_t i=0;i<noOfEntities(); ++i) {
		  
		     IMPACT_EXCEPT_IF( [&](){ return ! getEntityFromIndex(i)->isDerivedFrom("msMolecule"); },
		                        "The entity is not derived from msMolecule");
		     
		     boost::shared_ptr<msMolecule> speciesTD = 
		         getEntityFromIndex(i)->impact_static_cast<msMolecule>();
			 
		     IMPACT_EXCEPT_IF( [&](){ return ! speciesTD->getMotion()->isDerivedFrom("msMotionCEA"); },
		                        "The motion needs to be derived from msMotionCEA");
		     
		     boost::shared_ptr<msMotionCEA> motion = 
		         speciesTD->getMotion()->impact_static_cast<msMotionCEA>();
			
		     IMPACT_TRY([&](){ CeaMixture->add_curve_fit( names[i], motion->getAllCoeffsOfFit() );});	 
		}
		IMPACT_TRY([&](){
		    ThermoCalculator = boost::shared_ptr<Antioch::CEAEvaluator<double> >( new Antioch::CEAEvaluator<double>(*CeaMixture) );
		});
		
                IMPACT_LOGOUT();
            }
            
            
        public:
            
            msThermoMixture(): msChemicalMixture() {
                
                constructVar("msThermoMixture","ThermoMixture","mixture"); 
            }
            
            static boost::shared_ptr<msThermoMixture> New(){
                
                IMPACT_LOGIN_STATIC();
                boost::shared_ptr<msThermoMixture> T( new msThermoMixture() );
                T->initialize();
                T->update();
                IMPACT_LOGOUT();
                return T;
            }
            
            void initialize() {
                
                IMPACT_LOGIN();
                msChemicalMixture::initialize();
		/*declarePhysicalVariable( msUnit("J.entity^-1.K^-1"), &R, "R");
		R = getUnits()->convert(msUnit("J.mol^-1.K^-1"),csts::R);*/
		declarePhysicalVariable( &R, csts::R, msUnit("J.mol^-1.K^-1"),  "R");
                IMPACT_LOGOUT();
            }
            
            boost::shared_ptr<msTreeMapper> parseFromCanteraMechanismXml(string path);
	    
            boost::shared_ptr<msTreeMapper> parseFromCeaAscii(string path);
	    
            boost::shared_ptr<msTreeMapper> addEntity( boost::shared_ptr<msEntity> entity ) {
                
	        IMPACT_LOGIN();
		IMPACT_EXCEPT_IF( [&](){ return entity->isDerivedFrom("msMolecule"); },
		                        "The entity is not derived from msMolecule");
	        msChemicalMixture::addEntity(entity);
		IMPACT_LOGOUT();
	      
                return mySharedPtr();
            }
            
           vector<double> getH_RT_minus_SR() const;
            
	   //! \brief Calorific capacity at constant pressure for all species
	   vector<double> Cps();

	   //! \brief Calorific capacity at constant volume for all species
	   vector<double> Cvs();
	   
	   //! \brief Enthalpy for all species	   
	   vector<double> Hs();
	   
	   //! \brief Entropy for all species
	   vector<double> Ss();
	   
	   /*! \brief Calorific capacity constant pressure of a specie
	    *
	    * \param i index of the specie
	    */	   
	   double Cp(size_t i);
	   
	   /*! \brief Calorific capacity constant volume of a specie
	    *
	    * \param i index of the specie
	    */	   
	   double Cv(size_t i);
	   
	   /*! \brief Enthalpy of a specie
	    *
	    * \param i index of the specie
	    */
	   double H(size_t i);
	   
	   /*! \brief Entropy of a specie
	    *
	    * \param i index of the specie
	    */
	   double S(size_t i);
	   
	   //! \brief Total calorific capacity constant pressure
	   double CpTot();
	   
	   //! \brief Total calorific capacity constant volume
	   double CvTot();   
	   
	   //! \brief Total enthalpy 
	   double HTot();
	   
	   //! \brief Total entropy 
	   double STot();
	   
           bool sanityCheck();
	   
	   std::ostream& print(std::ostream& out) const {
	     
	       msPhysicalInterface::print(out);
               output::printHeader(out,"Thermodynamic mixture");
	       return out;
	   };
	   
        private:
                        
	    boost::shared_ptr<Antioch::CEAEvaluator<double> > ThermoCalculator;
	    
	    boost::shared_ptr<Antioch::CEAThermoMixture<double> > CeaMixture;
	    
	    double R; //! physical variable, always in the this->getUnits() unit
        };
    }
}

#endif // PA0DCSTSTP_H

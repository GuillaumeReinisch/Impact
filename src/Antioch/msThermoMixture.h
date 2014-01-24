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
                
                LOGGER_ENTER_FUNCTION_DBG("void msThermoMixture::update()",getFullId());
                msChemicalMixture::update();
		
		CeaMixture = boost::shared_ptr<Antioch::CEAThermoMixture<double> >(new Antioch::CEAThermoMixture<double>(*(getCalculator())) );
				
		vector<string> names=getEntitiesNames();
		
		for(size_t i=0;i<noOfEntities(); ++i) {
		  
		     getEntityFromIndex(i)->exceptIfNotDerivedFrom("msMolecule","void msThermoMixture::update()");
		     
		     boost::shared_ptr<msMolecule> speciesTD = 
		         getEntityFromIndex(i)->impact_static_cast<msMolecule>();
			 
		     speciesTD->getMotion()->exceptIfNotDerivedFrom("msMotionCEA","void msThermoMixture::update()");
		     
		     boost::shared_ptr<msMotionCEA> motion = 
		         speciesTD->getMotion()->impact_static_cast<msMotionCEA>();
			 
		    try{
		        CeaMixture->add_curve_fit( names[i], motion->getAllCoeffsOfFit() );
		    }
		    catch(std::exception& e0){
		      
		        stringstream out;
			out<<names[i]<<": can not add a curve fit to the CeaMixture; coefs: "
			<<motion->getAllCoeffsOfFit()<<endl<<" Antioch error:"<<e0.what();
		        msError e(out.str(), "void msThermoMixture::update()",getFullId());
			BOOST_THROW_EXCEPTION(e);
		    }
		}
		
		ThermoCalculator = boost::shared_ptr<Antioch::CEAEvaluator<double> >( new Antioch::CEAEvaluator<double>(*CeaMixture) );
		
                LOGGER_EXIT_FUNCTION2("void msThermoMixture::update()");
            }
            
            
        public:
            
            msThermoMixture(): msChemicalMixture() {
                
                constructVar("msThermoMixture","ThermoMixture","mixture");
            }
            
            static boost::shared_ptr<msThermoMixture> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msThermoMixture> msThermoMixture::New()","");
                boost::shared_ptr<msThermoMixture> T( new msThermoMixture() );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msThermoMixture> msThermoMixture::New()");
                return T;
            }
            
            void initialize() {
                
                LOGGER_ENTER_FUNCTION_DBG("void msThermoMixture::initialize()","");
                
                msChemicalMixture::initialize();
		
                LOGGER_EXIT_FUNCTION2("void msThermoMixture::initialize()");
            }
            
            boost::shared_ptr<msTreeMapper> parseFromCanteraMechanismXml(string path);
	    
            boost::shared_ptr<msTreeMapper> parseFromCeaAscii(string path);
	    
            boost::shared_ptr<msTreeMapper> addEntity( boost::shared_ptr<msEntity> entity ) {
                
	        LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msThermoMixture::addEntity( boost::shared_ptr<msEntity> entity )",
					  getFullId());
	        entity->exceptIfNotDerivedFrom("msMolecule","boost::shared_ptr<msTreeMapper> msThermoMixture::addEntity( boost::shared_ptr<msEntity> entity )");
	        
		try{ msChemicalMixture::addEntity(entity); 
		}
		catch(msError& e){
		     e.addContext("boost::shared_ptr<msTreeMapper> msThermoMixture::addEntity( boost::shared_ptr<msEntity> entity )");
		     BOOST_THROW_EXCEPTION(e);
		}
		LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msThermoMixture::addEntity( boost::shared_ptr<msEntity> entity )");
	      
                return mySharedPtr();
            }
            
            vector<double> getH_RT_minus_SR() const { 
	      
	        double T = getTemperature();
	        vector<double> result(noOfEntities(),0);
	        Antioch::TempCache<double> Temp(T);
	      
	        for( size_t i=0; i<result.size(); i++) {
		
		    result[i] = ThermoCalculator->h_RT_minus_s_R( Temp ,i );
	        }
	        return result;
	    } 
	    	    
           std::ostream& print(std::ostream& out) const {
	     
	       msPhysicalInterface::print(out);
               output::printHeader(out,"Thermodynamic mixture");
	       return out;
	   };
            
	   //! \brief Calorific capacity at constant pressure for all species
	   vector<double> Cps() {
	      
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       vector<double> Cp(noOfEntities());
	       Antioch::TempCache<double> temp(T);
	       
	       for(size_t i=0;i<noOfEntities();i++)
		  Cp[i]=R*ThermoCalculator->cp_over_R(temp,i);
		
	       return Cp;
	   }

	   //! \brief Calorific capacity at constant volume for all species
	   vector<double> Cvs() {
	      
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       vector<double> Cp(noOfEntities());
	       Antioch::TempCache<double> temp(T);
	       
	       for(size_t i=0;i<noOfEntities();i++)
		  Cp[i]=R*ThermoCalculator->cp_over_R(temp,i) - R;
		
	       return Cp;
	   }
	   
	   //! \brief Enthalpy for all species	   
	   vector<double> Hs(){
	       
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       vector<double> H(noOfEntities());
	       Antioch::TempCache<double> temp(T);
	       for(size_t i=0;i<noOfEntities();i++) H[i]=R*T*ThermoCalculator->h_over_RT(temp,i);
	       return H;
	   }
	   
	   //! \brief Entropy for all species
	   vector<double> Ss(){
	      
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       vector<double> S(noOfEntities());
	       Antioch::TempCache<double> temp(T);
	       for(size_t i=0;i<noOfEntities();i++) S[i]=R*ThermoCalculator->s_over_R(temp,i);
	       return S;
	   }
	   
	   /*! \brief Calorific capacity constant pressure of a specie
	    *
	    * \param i index of the specie
	    */	   
	   double Cp(size_t i) {
	      
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       vector<double> Cp(noOfEntities());
	       Antioch::TempCache<double> temp(T);
	       return R*ThermoCalculator->cp_over_R(temp,i);
	   }
	   /*! \brief Calorific capacity constant volume of a specie
	    *
	    * \param i index of the specie
	    */	   
	   double Cv(size_t i) {
	      
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       vector<double> Cv(noOfEntities());
	       Antioch::TempCache<double> temp(T);
	       return R*ThermoCalculator->cp_over_R(temp,i)-R;
	   }	   
	   /*! \brief Enthalpy of a specie
	    *
	    * \param i index of the specie
	    */
	   double H(size_t i){
	       
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       vector<double> H(noOfEntities());
	       Antioch::TempCache<double> temp(T);
	       return R*T*ThermoCalculator->h_over_RT(temp,i);
	   }
	   
	   /*! \brief Entropy of a specie
	    *
	    * \param i index of the specie
	    */
	   double S(size_t i){
	      
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       vector<double> S(noOfEntities());
	       Antioch::TempCache<double> temp(T);
	       return R*ThermoCalculator->s_over_R(temp,i);
	   }
	   
	   //! \brief Total calorific capacity constant pressure
	   double CpTot() {
	      
	       vector<double> moles = getMoleFractions();
	       //return  getUnits()->convert( "J.mol^-1.K^-1",ThermoCalculator->cp(temp,moles));
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       double Cp=0;
	       Antioch::TempCache<double> temp(T);
	       for(size_t i=0;i<noOfEntities();i++) Cp += moles[i]*R*ThermoCalculator->cp_over_R(temp,i);
	       return Cp;
	   }
	   
	   //! \brief Total calorific capacity constant volume
	   double CvTot() {
	      
	       vector<double> moles = getMoleFractions();
	       //return  getUnits()->convert( "J.mol^-1.K^-1",ThermoCalculator->cp(temp,moles));
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       double Cp=0;
	       Antioch::TempCache<double> temp(T);
	       for(size_t i=0;i<noOfEntities();i++) Cp += moles[i]*(R*ThermoCalculator->cp_over_R(temp,i)-R);
	       return Cp;
	   }	   
	   
	   //! \brief Total enthalpy 
	   double HTot(){
	        
	       vector<double> moles = getMoleFractions();
	       //return getUnits()->convert( "J.mol^-1",ThermoCalculator->h(temp,moles ) );
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       double H=0;
	       Antioch::TempCache<double> temp(T);
	       for(size_t i=0;i<noOfEntities();i++) H += moles[i]*R*T*ThermoCalculator->h_over_RT(temp,i);
	       return H;
	   }
	   
	   //! \brief Total entropy 
	   double STot(){
	     
	       vector<double> moles = getMoleFractions();
	       //return getUnits()->convert( "J.mol^-1.K^-1",ThermoCalculator->s(temp,moles ));
	       double T = getTemperature();
	       double R = getUnits()->convert( "J.mol^-1.K^-1",  csts::R );  
	       double S=0;
	       Antioch::TempCache<double> temp(T);
	       for(size_t i=0;i<noOfEntities();i++) S += moles[i]*R*ThermoCalculator->s_over_R(temp,i);
	       return S;
	   }
	   
            
        private:
                        
	    boost::shared_ptr<Antioch::CEAEvaluator<double> > ThermoCalculator;
	    
	    boost::shared_ptr<Antioch::CEAThermoMixture<double> > CeaMixture;
        };
    }
}

#endif // PA0DCSTSTP_H

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


#ifndef msMIXTURE_H
#define msMIXTURE_H

#include "antioch/chemical_mixture.h"

#include <msConstituent.h>

namespace impact {
    
    namespace antioch {
        
        using namespace atomism;
        
        /** \brief A mixture of components
         *
         * Container that asociates constituents to mole fractions.
	 * 
	 * ~~~~~~~~~~~~~{.py}
         from libimpact import *
         import itertools
         
         mixture = msChemicalMixture::New()
         
         H2 = msConstituent::New()
         struct = H2.getAtomicStructure().addElement(NewAtom("H")).addElement(NewAtom("H"))
         mixture.addConstituent(H2)
         
         CH4 = msMolecule::New()
         struct = CH4.getAtomicStructure().addElement(NewAtom("C"))
         for i in range(0,'):
	     struct.addElement(NewAtom("H"))
	       
	 CH4->setMotion( msNasaPolynome.New().load("fileWithThermo")) );
	 
	 mixture.addConstituent(CH4)

	 moleFractions = mixture.getMoleFractions()
	 names         = mixture.getConstituentsNames()
	 
	 for (name,moleFrac) in itertools.izip(names,moleFractions):
	     print name,moleFrac
         * ~~~~~~~~~~~~~
         */
        class msChemicalMixture : public msPhysicalInterface
        {
        public:
           
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isPhaseRegisteredInPython;
            static msRegister* createInstance() { return new msChemicalMixture; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update() {
                
                LOGGER_ENTER_FUNCTION_DBG("void msChemicalMixture::update()",getFullId());
                msPhysicalInterface::update();
		
		msChildren<msConstituent>::iterator it = Constituents.begin();
		Calculator = boost::shared_ptr<Antioch::ChemicalMixture>( new Antioch::ChemicalMixture(vector<string>() ) );
		
		for(;it!=Constituents.end();++it)  addConstituent(*it);
		
                LOGGER_EXIT_FUNCTION2("void msChemicalMixture::update()");
            }
            
            
        public:
            
            msChemicalMixture(): msPhysicalInterface() {
                
                constructVar("msChemicalMixture","ChemicalMixture","mixture");
            }
            
            void initialize() {
                
                LOGGER_ENTER_FUNCTION_DBG("void msChemicalMixture::initialize()","");
                
                msPhysicalInterface::initialize();
                
                msTreeMapper::declareChildren<msConstituent>(Constituents,"Constituents");
                msTreeMapper::declareAttribute(MoleFractions,"MoleFractions");
		                
		//Calculator = boost::shared_ptr<Antioch::ChemicalChemicalMixture>(new Antioch::ChemicalChemicalMixture() );
		
                LOGGER_EXIT_FUNCTION2("void msChemicalMixture::initialize()");
            }
            
            static boost::shared_ptr<msChemicalMixture> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msChemicalMixture> msChemicalMixture::New()","");
                boost::shared_ptr<msChemicalMixture> T( new msChemicalMixture );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msChemicalMixture> msChemicalMixture::New()");
                return T;
            }
            
            
            virtual double getDensity() {
	      
	         msError e("getDenisty is not implemented in chemical mixture, you need to use derived class (like gas phase)",
		   "virtual double msChemicalMixture::getDensity()",getFullId());
		 
		 BOOST_THROW_EXCEPTION(e);
	    }
	    
            boost::shared_ptr<Antioch::ChemicalMixture> getCalculator(){
	      
	         return Calculator;
                
             //   return boost::static_pointer_cast<T>(Calculator);
            }
            
            size_t noOfConstituents() const {
                
            /*    if(Calculator->n_species() != Constituents.size()) {
                    
                    stringstream out;
                    out<<"Number of species in Cantera Calculator ("<<Calculator->nSpecies()<<")"
                    <<"are different than the number of constituents declared to the interface ("<<Constituents.size()<<").";
                    msError e(out.str(),"size_t msChemicalMixture::noOfConstituents() const",getFullId());
                    BOOST_THROW_EXCEPTION(e);
                }*/
                return Constituents.size();
            }
            
            boost::shared_ptr<msTreeMapper> addConstituent( boost::shared_ptr<msConstituent> entity ) {
                /*
	        try{
		  
		   Calculator->addSpecie(Constituents.size()-1, entity->getId(), entity->getMass(), 0, 3, 0);
		}
		catch(){
		  
		    msError e("","","");
		    BOOST_THROW_EXCEPTION(e);
		}*/
                addElementToChildren<msConstituent>(Constituents,entity);
		
                return mySharedPtr();
            }
            
            boost::shared_ptr<msConstituent> getConstituentFromIndex( size_t i) {
                
	        boost::shared_ptr<msConstituent> cons;
	        try {
	             cons = Constituents[i];
		}
		catch(msError& e){
		     
		     e.addContext("boost::shared_ptr<msConstituent> msChemicalMixture::getConstituentFromIndex( size_t i)");
		     BOOST_THROW_EXCEPTION(e);
		}
                return Constituents.getElementFromId(id);
            }
            
            boost::shared_ptr<msConstituent> getConstituentFromId( std::string id ) {
                
                return Constituents.getElementFromId(id);
            }
            
                        
            double totalMass() const {
	      
	        double mtot = 0;
		for(size_t i=0;i<noOfConstituents();i++){
		  
		    mtot += Constituents[i]->getMass();
		}
		return mtot;
	    }
	    
            std::vector<std::string> getConstituentsNames() const {
                
                testCalculator("void msChemicalMixture::getConstituentsNames() const");
		std::vector<string> vec;
		/*
		const std::map<std::string,Species>& namesMap = Calculator->species_name_map();
                const std::map<std::string,Species>::iterator it = namesMap.begin();
		
		
		for(;it!=namesMap.end();++it) vec.push_back( (*it).first);
		*/
                return vec;
            }
            
                        
            //! Get the species mole fractions. \see State::getMoleFraction
            std::vector<double> getMoleFractions() const {
                
                return MoleFractions;
            }
            
            //! Get the species mass fractions. \see State::getMassFractions
            std::vector<double> getMassFractions() const {
                
                std::vector<double> vec;
		double mtot = totalMass();
		for(size_t i=0;i<noOfConstituents();i++){
		  
		    vec.push_back( Constituents[i]->getMass() / mtot);
		}
		return vec;
            }
            
            /*! \brief Return a specie's mole fraction
             *
             * \param name name of the specie
             */
            double getMoleFractionByName(std::string name) const {
              
                testCalculator("double msChemicalMixture::getMoleFractionByName(std::string name) const");
                /*
		const std::map<std::string,unsigned int>& map = Calculator->active_species_name_map();
		
		if( map.find(name)== map.end() ){
		  
                    msError e("The specie of name "+name+" has not been found",
                              "double msMicroscopicPhase::getMoleFractionByName(std::string name) const",
                              getFullId());
                    BOOST_THROW_EXCEPTION(e);
                }
                */
                return 0;//map[name];
            }
 
            /*! \brief Return a specie's mole fraction
             *
             * \param name name of the specie
             */
            vector<double> getMolarDensities() const {
              
                testCalculator("vector<double> msChemicalMixture::getMolarDensities() const");
		
		vector<double> molarDensity;
		double density = getUnits()->convertTo("kg.m^-3",getDensity());
		Calculator->molar_densities( density, getMassFractions(), molarDensity);
		
		for( size_t i=0;i<density.size();i++) {
		  
		    density[i] *= getUnits()->convert("mol.m^-3",1);
		}
                return density;
            } 
            
            /*! \brief Return a specie's mole fraction
             *
             * \param name name of the specie
             */
            double getMassFractionByName(std::string name) const {
              
                testCalculator("double msChemicalMixture::getMoleFractionByName(std::string name) const");
                /*
		const std::map<std::string,unsigned int>& map = Calculator->active_species_name_map();
		
		if( map.find(name)== map.end() ){
		  
                    msError e("The specie of name "+name+" has not been found",
                              "double msMicroscopicPhase::getMoleFractionByName(std::string name) const",
                              getFullId());
                    BOOST_THROW_EXCEPTION(e);
                }
                */
                return 0;//Constituents[ map[name] ]->getMass() / totalMass();
            }
                        
            std::ostream& print(std::ostream& out) const ;
            
            //virtual void updateInCalculator(Cantera::Kinetics calculator);
            
        protected:
            
            void testCalculator(std::string fct) const{
                
                //if( Calculator ) return;
                msError e("The calculator is not initialized, use the 'load' function",fct,getFullId());
                BOOST_THROW_EXCEPTION(e);
            }
            
        private:
            
            //!\name attributs and children
            //@{
            
            msChildren<msConstituent>  	Constituents;
            
            vector<double> MoleFractions;
            //@}
            
	    boost::shared_ptr<Antioch::ChemicalMixture> Calculator;
	    
        };
    }
}

#endif // PA0DCSTSTP_H
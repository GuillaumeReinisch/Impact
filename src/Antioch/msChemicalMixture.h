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

#include "antioch/vector_utils_decl.h"
#include "antioch/chemical_mixture.h"

#include <msEntity.h>

namespace impact {
    
    namespace antioch {
        
        using namespace atomism;
        
        /** \brief A mixture of components
         *
         * A container of msEntity associated to mole fractions and defining a temperature.
	 * It is based on the calculator Antioch::ChemicalMixture.
	 * 
	 * Example of use:
	 * ~~~~~~~~~~~~~{.py}
         from libimpact import *
         import itertools
         
         mixture = msChemicalMixture::New()
         
         H2 = msEntity::New().addElement(NewAtom("H")).addElement(NewAtom("H"))
         
         CH4 = msMolecule::New().addElement(NewAtom("C"))
         for i in range(0,4):
	     CH4.addElement(NewAtom("H") 
	       
	 mixture.addEntity(H2).addEntity(CH4)

	 moleFractions = mixture.getMoleFractions()
	 names         = mixture.getEntitiesNames()
	 
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
                
                IMPACT_LOGIN();
                msPhysicalInterface::update();
		
		vector<string> strs = getEntitiesNames();size_t i=0;
		
		IMPACT_TRY( [&] () { Calculator = boost::shared_ptr<Antioch::ChemicalMixture<double> >( 
		                     new Antioch::ChemicalMixture<double>(strs) );  
		});
		
		for_each(Entities.begin(),Entities.end(), [&] (boost::shared_ptr<msEntity> e) {
			
		      IMPACT_TRY( [&] () { 
			  Calculator->add_species(i, e->getId(), 
			           e->getUnits()->convertTo( e->getMass(), msUnit("kg.mol^-1") ), 
				   0, min(size_t(3),e->noOfElements()), 0); i++;
		      });
		});
                IMPACT_LOGOUT();
            }
            
            
        public:
            
            msChemicalMixture(): msPhysicalInterface() {
                
                constructVar("msChemicalMixture","ChemicalMixture","mixture");
		Temperature = 298.; Density=0;
            }
            
            void initialize() {
                
                IMPACT_LOGIN();
                
                msPhysicalInterface::initialize();
                
                declareChildren<msEntity>(Entities,"Entities");
                declareAttribute(MoleFractions,"MoleFractions");
		declareAttribute(Temperature,"Temperature");
		declareAttribute(Density,"Density");
		
		declarePhysicalVariable( msUnit("kg.m^-3"), &Density, "Density");
                IMPACT_LOGOUT();
            }
            
            static boost::shared_ptr<msChemicalMixture> New(){
                
                IMPACT_LOGIN_STATIC();
                boost::shared_ptr<msChemicalMixture> T( new msChemicalMixture );
                T->initialize();
                T->update();
                IMPACT_LOGOUT();
                return T;
            }
            
            //! \brief return the density
            double getDensity() const { return Density;}
	    
	    /*! \brief set the density
	     * 
	     * \param d density
	     */ 
	    virtual boost::shared_ptr<msTreeMapper> setDensity(double d) {
	      
	         IMPACT_LOGIN();
	         IMPACT_EXCEPT_IF( [&] () { return d<=0; }, "The density is null or negative");
	         Density = d;
		 IMPACT_LOGOUT();
		 return mySharedPtr();
	    }
	    
            boost::shared_ptr<Antioch::ChemicalMixture<double> > getCalculator(){ return Calculator; }
            
            size_t noOfEntities() const { return Entities.size(); }
                        
            virtual boost::shared_ptr<msTreeMapper> addEntity( boost::shared_ptr<msEntity> entity ) {
                	       
	        IMPACT_LOGIN();
		addElementToChildren<msEntity>(Entities,entity);
		MoleFractions.push_back(double(Entities.size()==1));
		IMPACT_LOGOUT();
                return mySharedPtr();
            }
            
            size_t getIndex(string id) const {
	        
	        IMPACT_LOGIN();
	        vector<std::string> names = getEntitiesNames();
		vector<std::string>::iterator it =std::find(names.begin(), names.end(), id);
		
		IMPACT_EXCEPT_IF( [&] () { return it==names.end(); },
			         "Can not find the specie of id "+id);
		IMPACT_LOGOUT();
		return it-names.begin();
	    }
	    
            boost::shared_ptr<msEntity> getEntityFromIndex( size_t i) {
                
	        IMPACT_LOGIN();
	        boost::shared_ptr<msEntity> cons = Entities[i].getSharedPtr();
		IMPACT_LOGOUT();
                return cons;
            }
            
            boost::shared_ptr<msEntity> getEntityFromId( std::string id ) {
	      
                IMPACT_LOGIN();
	        boost::shared_ptr<msEntity> cons = Entities.getElementFromId(id);
		IMPACT_LOGOUT();
                return cons;
            }
            
	    vector<boost::shared_ptr<msEntity> > getEntities() const { return Entities.getElements(); }
	    
	    //! \brief return the total mass                        
            double totalMass() const {
	      
	        double r = 0;
	        for_each( Entities.begin(),Entities.end(), 
		    [&] (const boost::shared_ptr<msEntity> e) {r += e->getMass();} );
		  
	        return r;
	    }
	    
	    //! \brief return the temperature
	    double getTemperature() const {
	      
	        return Temperature;
	    }
	    
	    /*! \brief set the temperature
	     * 
	     * \param T temperature
	     */ 
	    boost::shared_ptr<msTreeMapper> setTemperature(double T) {
	      
	        Temperature = T;
	        return mySharedPtr();
	    }
	     
	    /*! \brief set the mole fractions
	     * 
	     * \param mapf map specie's name -> molar fraction, set 0 for species not included, and renormalize at the end 
	     */ 
	    boost::shared_ptr<msTreeMapper> setMoleFractions(std::map<std::string,double> mapf) {
	      
	        IMPACT_LOGIN();
		for_each( mapf.begin(),mapf.end(), 
		    [&] (pair<std::string,double> it) { MoleFractions[getIndex(it.first)] = it.second; } );
		
		IMPACT_LOGOUT();
	        return mySharedPtr();
	    }    
	    
	    
	    //! \brief return the constituents name in a list
            std::vector<std::string> getEntitiesNames() const {
                
                std::vector<string> vec;
		for_each( Entities.begin(),Entities.end(), 
		    [&] (const boost::shared_ptr<msEntity> e) { vec.push_back(e->getId());} );
				
                return vec;
            }
            
                        
            //! Get the species mole fractions.
            std::vector<double> getMoleFractions() const { return MoleFractions; }
            
            //! Get the species mass fractions. 
            std::vector<double> getMassFractions() const {
                
	        IMPACT_LOGIN();
                std::vector<double> vec;
		double mtot = totalMass();
		
		for_each( Entities.begin(),Entities.end(), [&] (const boost::shared_ptr<msEntity> e) {
		     vec.push_back(e->getMass() / mtot);
		});
			
                IMPACT_LOGOUT();		
		return vec;
            }
            /*! \brief Return a specie's mole fraction
             *
             * \param name index of the specie
             */
            double getMoleFraction(size_t i) const {
              
                IMPACT_LOGIN();
		IMPACT_EXCEPT_IF([&](){return i>noOfEntities();},"i > noOfEntities");
		IMPACT_LOGOUT();
		return MoleFractions[i];
            }
            
            /*! \brief Return a specie's mole fraction
             *
             * \param name name of the specie
             */
            double getMoleFractionByName(std::string name) const {
              
                IMPACT_LOGIN();
                double r =0;
		IMPACT_TRY( [&] () {r = getMoleFractions()[getIndex(name)]; });
		IMPACT_LOGOUT();
		return r;
            }
 
            /*! \brief Return the molar densities
             */
            vector<double> getMolarDensities() const {
              
                IMPACT_LOGIN();
		
		vector<double> molarDensity(noOfEntities());
		double density = getUnits()->convertTo(getDensity(),msUnit("kg.m^-3"));
		
		IMPACT_TRY( [&] () { Calculator->molar_densities( density, getMassFractions(), molarDensity); });
		
		for_each( molarDensity.begin(),molarDensity.end(), [&] ( double& e) { 
		     e *= getUnits()->convert("mol.m^-3",1) ;
		});
		
		IMPACT_LOGOUT();
                return molarDensity;
            } 
            
            /*! \brief Return a specie's mole fraction
             *
             * \param name name of the specie
             */
            double getMassFractionByName(std::string name) const {
              
 	        IMPACT_LOGIN();
                double r =0;
		IMPACT_TRY( [&] () { r = getMassFractions()[getIndex(name)]; });
		IMPACT_LOGOUT();
                return r;
            }
           
            bool sanityCheck();
	    
            std::ostream& print(std::ostream& out) const ;
        
        private:
            
            //!\name attributs and children
            //@{
            
            msChildren<msEntity>  	Entities;
            
            vector<double> MoleFractions;
	    
	    double Temperature;
	    
	    double Density;
            //@}
            
	    boost::shared_ptr<Antioch::ChemicalMixture<double> > Calculator;
	    
        };
    }
}

#endif // PA0DCSTSTP_H

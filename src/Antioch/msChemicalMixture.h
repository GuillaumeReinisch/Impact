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
                
                LOGGER_ENTER_FUNCTION_DBG("void msChemicalMixture::update()",getFullId());
                msPhysicalInterface::update();
		
		vector<string> strs = getEntitiesNames();
		
		try{
		    Calculator = boost::shared_ptr<Antioch::ChemicalMixture<double> >( 
		                 new Antioch::ChemicalMixture<double>(strs) );
		}
		catch(std::exception& e0) {
		  
		    msError e("Can not create the ChemicalMixture calculator: "+string(e0.what()),
			      "void msChemicalMixture::update()",getFullId());
		    BOOST_THROW_EXCEPTION(e);
		}
		msChildren<msEntity>::iterator it = Entities.begin();
		for(size_t i=0;it!=Entities.end();++it,++i) {
		  
		    try{ Calculator->add_species(i, (*it)->getId(),
					         (*it)->getUnits()->convertTo( (*it)->getMass(),msUnit("kg.mol^1") ), 
				                 0, 
					         min(size_t(3),(*it)->noOfElements()), 
					         0);
		    }
		    catch(std::exception& e0){
		  
		        msError e("An exception calling Antioch::ChemicalMixture<double>::add_species oocured: "+
			          string(e0.what()),
			          "void msChemicalMixture::update()",
			          getFullId());
		        BOOST_THROW_EXCEPTION(e);
		    }
		}
		
                LOGGER_EXIT_FUNCTION2("void msChemicalMixture::update()");
            }
            
            
        public:
            
            msChemicalMixture(): msPhysicalInterface() {
                
                constructVar("msChemicalMixture","ChemicalMixture","mixture");
		Temperature = 298.; Density=0;
            }
            
            void initialize() {
                
                LOGGER_ENTER_FUNCTION_DBG("void msChemicalMixture::initialize()","");
                
                msPhysicalInterface::initialize();
                
                msTreeMapper::declareChildren<msEntity>(Entities,"Entities");
                msTreeMapper::declareAttribute(MoleFractions,"MoleFractions");
		msTreeMapper::declareAttribute(Temperature,"Temperature");
		msTreeMapper::declareAttribute(Density,"Density");
		
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
            
            //! \brief return the density
            double getDensity() const {
	      
	         return Density;
	    }
	    
	    /*! \brief set the density
	     * 
	     * \param d density
	     */ 
	    virtual boost::shared_ptr<msTreeMapper> setDensity(double d) {
	      
	        if(d<=0){
		   
	             msError e("The density is null or negative","virtual boost::shared_ptr<msTreeMapper> msChemicalMixture::setDensity(double d)",getFullId());
		     BOOST_THROW_EXCEPTION(e);
		 }
	         Density = d;
		 return mySharedPtr();
	    }
	    
            boost::shared_ptr<Antioch::ChemicalMixture<double> > getCalculator(){
	      
	         return Calculator;
                
             //   return boost::static_pointer_cast<T>(Calculator);
            }
            
            size_t noOfEntities() const {
                
                if(Calculator->n_species() != Entities.size()) {
                    
                    stringstream out;
                    out<<"Number of species in Cantera Calculator ("<<Calculator->n_species()<<")"
                    <<"are different than the number of constituents declared to the interface ("<<Entities.size()<<").";
                    msError e(out.str(),"size_t msChemicalMixture::noOfEntities() const",getFullId());
                    BOOST_THROW_EXCEPTION(e);
                }
                return Calculator->n_species();
            }
            
            virtual boost::shared_ptr<msTreeMapper> addEntity( boost::shared_ptr<msEntity> entity ) {
                	       
	        LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msChemicalMixture::addEntity( boost::shared_ptr<msEntity> entity )",
					  getFullId());
	        
		addElementToChildren<msEntity>(Entities,entity);
		if(! entity->hasParent() ) entity->setAffiliation(mySharedPtr());
		
		if(Entities.size()==1) MoleFractions.push_back(1);
		else MoleFractions.push_back(0);
		
		LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msChemicalMixture::addEntity( boost::shared_ptr<msEntity> entity )");
                return mySharedPtr();
            }
            
            size_t getIndex(string id) const {
	        
	        vector<std::string> names = getEntitiesNames();
		vector<std::string>::iterator it =std::find(names.begin(), names.end(), id);
		
		if(it==names.end()) {
		     std::string speciesList;
		     for(size_t i=0;i<names.size();i++) speciesList += names[i]+" ";
		     msError e("Can not find the specie of name "+id+"; "+speciesList,
		       "size_t msChemicalMixture::getIndex(string id)",getFullId());
		    BOOST_THROW_EXCEPTION(e); 
		}
		return it-names.begin();
	    }
	    
            boost::shared_ptr<msEntity> getEntityFromIndex( size_t i) {
                
	        boost::shared_ptr<msEntity> cons;
	        try {
	             cons = Entities[i].getSharedPtr();
		}
		catch(msError& e){
		     
		     e.addContext("boost::shared_ptr<msEntity> msChemicalMixture::getEntityFromIndex( size_t i)");
		     BOOST_THROW_EXCEPTION(e);
		}
                return cons;
            }
            
            boost::shared_ptr<msEntity> getEntityFromId( std::string id ) {
                
                return Entities.getElementFromId(id);
            }
            
	    //! \brief return the total mass                        
            double totalMass() const {
	      
	        double mtot = 0;
		for(size_t i=0;i<noOfEntities();i++){
		  
		    mtot += Entities[i]->getMass();
		}
		return mtot;
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
	      
	        MoleFractions.resize(noOfEntities(),0);
		double tot=0;
		try{
		    for( std::map<std::string,double>::iterator it=mapf.begin();
			 it!=mapf.end();++it){
		      
		        MoleFractions[getIndex((*it).first)] = (*it).second;
			tot += (*it).second;
		    }
		    for(size_t i=0;i<noOfEntities();i++) MoleFractions[i] /= tot;
		}
		catch(msError& e){
		  
		    e.addContext("boost::shared_ptr<msTreeMapper> setMoleFractions(std::map<std::string,double>)");
		    BOOST_THROW_EXCEPTION(e);
		}
	        return mySharedPtr();
	    }    
	    
	    
	    //! \brief return the constituents name in a list
            std::vector<std::string> getEntitiesNames() const {
                
                std::vector<string> vec(Entities.size());
		msChildren<msEntity>::const_iterator it = Entities.begin();
		for(int i=0;it!=Entities.end();++it,i++) vec[i]=(*it)->getId();
		
                return vec;
            }
            
                        
            //! Get the species mole fractions.
            std::vector<double> getMoleFractions() const {
                
                return MoleFractions;
            }
            
            //! Get the species mass fractions. 
            std::vector<double> getMassFractions() const {
                
                std::vector<double> vec;
		double mtot = totalMass();
		for(size_t i=0;i<noOfEntities();i++){
		  
		    vec.push_back( Entities[i]->getMass() / mtot);
		}
		return vec;
            }
            
            /*! \brief Return a specie's mole fraction
             *
             * \param name name of the specie
             */
            double getMoleFractionByName(std::string name) const {
              
                testCalculator("double msChemicalMixture::getMoleFractionByName(std::string name) const");
                double r =0;
		
		try{ 
		    r = getMoleFractions()[getIndex(name)];
		}
		catch(msError& e) {
		  
		    e.addContext("double msChemicalMixture::getMoleFractionByName(std::string name) const");
		    BOOST_THROW_EXCEPTION(e);
		}
		
                return r;//map[name];
            }
 
            /*! \brief Return the molar densities
             */
            vector<double> getMolarDensities() const {
              
                testCalculator("vector<double> msChemicalMixture::getMolarDensities() const");
		
		vector<double> molarDensity(noOfEntities());
		double density = getUnits()->convertTo(getDensity(),msUnit("kg.m^-3"));
		Calculator->molar_densities( density, getMassFractions(), molarDensity);
		
		for( size_t i=0;i<molarDensity.size();i++) {
		  
		    molarDensity[i] *= getUnits()->convert("mol.m^-3",1);
		}
                return molarDensity;
            } 
            
            /*! \brief Return a specie's mole fraction
             *
             * \param name name of the specie
             */
            double getMassFractionByName(std::string name) const {
              
                testCalculator("double msChemicalMixture::getMassFractionByName(std::string name) const");
                double r =0;
		
		try{ 
		    r = getMassFractions()[getIndex(name)];
		}
		catch(msError& e) {
		  
		    e.addContext("double msChemicalMixture::getMassFractionByName(std::string name) const");
		    BOOST_THROW_EXCEPTION(e);
		}
		
                return r;
            }
           
            bool sanityCheck();
	    
            std::ostream& print(std::ostream& out) const ;
            
            
        protected:
            
            void testCalculator(std::string fct) const{
                
                if( Calculator ) return;
                msError e("The calculator is not initialized, use the 'load' function",fct,getFullId());
                BOOST_THROW_EXCEPTION(e);
            }
            
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

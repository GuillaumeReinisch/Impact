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


#ifndef MSTHERMOCONSTITUENT_H
#define MSTHERMOCONSTITUENT_H


#include <msEntity.h>
#include <msMotion.h>

namespace impact {
    
    namespace atomism {
        
        
        /** \brief An entity associated to a motion
         *
         * The msMolecule class represents an entity that is associated to a motion (msMotion).
	 * The motion is used to implement thermodynamic data computations (S,Cp,H,...).
         */
        class msMolecule : public msEntity
        {
           
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isMoleculeRegisteredInPython;
            static msRegister* createInstance() { return new msMolecule; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update() {
                
                LOGGER_ENTER_FUNCTION_DBG("void msMolecule::update()",getFullId());
                msEntity::update();
                LOGGER_EXIT_FUNCTION2("void msMolecule::update()");
            }
            
        public:
            
	    msMolecule(): msEntity() {
                
                constructVar("msMolecule","Molecule","constituent with thermo");
            }
            
            static boost::shared_ptr<msMolecule> New(){
                    
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msMolecule> msMolecule::New()","");
                boost::shared_ptr<msMolecule> T( new msMolecule );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msMolecule> msMolecule::New()");
                return T;
            }
                
            void initialize() {
	      
                LOGGER_ENTER_FUNCTION_DBG("void msMolecule::initialize()","");
                
                msEntity::initialize();
		msTreeMapper::declareChild(Motion,msMotion::New(),"Motion");
		
                LOGGER_EXIT_FUNCTION2("void msMolecule::initialize()");
            }
  	    
  	    //! \brief return the motion
  	    boost::shared_ptr<msMotion> getMotion() const {return Motion.getSharedPtr(); }
  	    
  	    /** \brief set the motion
	     * 
	     * \param motion motion
	     */ 
  	    boost::shared_ptr<msTreeMapper> setMotion( boost::shared_ptr<msMotion> motion) {
	      
	        msTreeMapper::update(Motion,motion);
	        return mySharedPtr();
	    }
  	    
        private:
            
	    msChild<msMotion> Motion;        
        };
        
    }
}

#endif // PA0DCSTSTP_H

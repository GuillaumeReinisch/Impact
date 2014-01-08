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


#ifndef PAReaction_H
#define PAReaction_H


#include <msRate.h>

#include "antioch/reaction.h"

#include <time.h>

namespace impact {
    
    namespace cantera {
        
        //using namespace impact::atomism;
        /** \brief Base class for a reaction
         *
         * A reaction is described by a set of coefficients and a name
         *
         */
        class msElementaryReaction : public msReaction
        {
            
            friend class msKineticMechanism;
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isReactionRegisteredInPython;
            static msRegister* createInstance() { return new msElementaryReaction; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update() {
                
                LOGGER_ENTER_FUNCTION_DBG("void msElementaryReaction::update()",getFullId());
                msReaction::update();
                LOGGER_EXIT_FUNCTION2("void msElementaryReaction::update()");
            }
            
        public:
            
	    msElementaryReaction(): msReaction() {
                
                constructVar("msElementaryReaction","Reaction","reaction");
                Name = "";
            }
            
            void initialize() {
                LOGGER_ENTER_FUNCTION_DBG("void msElementaryReaction::initialize()","");
                
                msReaction::initialize();
		
		msTreeMapper::declareChild(Rate,msRate::New(),"Rate");
                Rate->setAffiliation(mySharedPtr());
		Rate->setUnits(getUnits());
		
                LOGGER_EXIT_FUNCTION2("void msElementaryReaction::initialize()");
            }
            
            static boost::shared_ptr<msElementaryReaction> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msElementaryReaction> msElementaryReaction::New()","");
                boost::shared_ptr<msElementaryReaction> T( new msElementaryReaction );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msElementaryReaction> msElementaryReaction::New()");
                return T;
            }
            
            
            boost::shared_ptr<msTreeMapper> setRate(boost::shared_ptr<msRate> rate) {
	      
	        msTreeMapper::updateChild(Rate,rate);
	    };
	    
	    
            std::ostream& print(std::ostream& out) const ;
            
            boost::shared_ptr<msTreeMapper> setName(std::string name){
                
                Name=name;
                setId(Name);
                return mySharedPtr();
            }
            std::string getName(){ return Name;
            }
            
           
        
    }
}

#endif // PA0DCSTSTP_H

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


#ifndef MSREACTION_H
#define MSREACTION_H


#include <msConstituent.h>
#include <msMotion.h>

namespace impact {
    
    namespace antioch {
        
        
        /** \brief a constituent with a thermo model
         *
         * A themo model is anything derived from msMotion.
         */
        class msThermoConstituent : public msConstituent
        {
            
            friend class msKineticMechanism;
            
        private:
            
            //!\name from msRegister
            //@{
            static msRegistrar Registrar;
            static bool  isThermoConstituentRegisteredInPython;
            static msRegister* createInstance() { return new msThermoConstituent; }
            //@}
            
        protected:
            
            void registryInPython();
            
            void update() {
                
                LOGGER_ENTER_FUNCTION_DBG("void msThermoConstituent::update()",getFullId());
                msConstituent::update();
                LOGGER_EXIT_FUNCTION2("void msThermoConstituent::update()");
            }
            
        public:
            
	    msThermoConstituent(): msConstituent() {
                
                constructVar("msThermoConstituent","ThermoConstituent","constituent with thermo");
            }
            
            void initialize() {
	      
                LOGGER_ENTER_FUNCTION_DBG("void msThermoConstituent::initialize()","");
                
                msConstituent::initialize();
		msTreeMapper::declareChild(Motion,msMotion::New(),"Motion");
		
                LOGGER_EXIT_FUNCTION2("void msThermoConstituent::initialize()");
            }
  	    
  	    
  	    boost::shared_ptr<msMotion> getMotion() const {return Motion.getSharedPtr(); }
  	    
        private:
            
	    
	    msChild<msMotion> Motion;        
        };
        
    }
}

#endif // PA0DCSTSTP_H

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


#ifndef msCONSTITUENT_H
#define msCONSTITUENT_H

#include <msEntity.h>

namespace impact {
    
    namespace antioch {
        
        using namespace atomism;
        
         class msConstituent : public msPhysicalInterface
            {
                
            friend class msMixture;
                
            private:
                
                //!\name from msRegister
                //@{
                static msRegistrar Registrar;
                static bool  isConstituentRegisteredInPython;
                static msRegister* createInstance() { return new msConstituent; }
                //@}
                
            protected:
                
                void registryInPython();
                
                void update() {
                    
                    LOGGER_ENTER_FUNCTION_DBG("void msConstituent::update()",getFullId());
                    msPhysicalInterface::update();
                    LOGGER_EXIT_FUNCTION2("void msConstituent::update()");
                }
                
            public:
                
                msConstituent(): msPhysicalInterface() {
                    
                    constructVar("msConstituent","Constituent","constituent");
                }
                void initialize() {
                    
                    LOGGER_ENTER_FUNCTION_DBG("void msConstituent::initialize()","");
                    msPhysicalInterface::initialize();
                    msTreeMapper::declareChild<msEntity>(AtomicStructure,msEntity::New(),"AtomicStructure");
                    AtomicStructure->setAffiliation(mySharedPtr());
                    LOGGER_EXIT_FUNCTION2("void msConstituent::initialize()");
                }
                
                double getMass() const { 
		  
		  return getUnits()->convert( AtomicStructure->getUnits()->getMass() , AtomicStructure->getMass());
		}
                
                boost::shared_ptr<msEntity> getAtomicStructure() { return AtomicStructure.getSharedPtr();
                }
                
                boost::shared_ptr<msTreeMapper> setAtomicStructure(boost::shared_ptr<msEntity> structure) {
                    
                    if(!structure->hasParent()) structure->setAffiliation(mySharedPtr());
                    msTreeMapper::update(AtomicStructure,structure);
                    return mySharedPtr();
                }
                
                static boost::shared_ptr<msConstituent> New(){
                    
                    LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msConstituent> msConstituent::New()","");
                    boost::shared_ptr<msConstituent> T( new msConstituent );
                    T->initialize();
                    T->update();
                    LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msConstituent> msConstituent::New()");
                    return T;
                }

            private:
                
                //!\name attributs and children
                //@{
                msChild<msEntity>  AtomicStructure;
                //@}
                
            };
    }
}

#endif // PA0DCSTSTP_H

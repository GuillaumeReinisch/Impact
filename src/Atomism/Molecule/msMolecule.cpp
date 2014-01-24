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


#include "msMolecule.h"

namespace impact {
    
    namespace atomism {
        
        
        bool msMolecule::isMoleculeRegisteredInPython = 0;
	
        msRegistrar msMolecule::Registrar("msMolecule", msMolecule::createInstance);
        
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msMolecule::registryInPython() {
            
#if USE_PYTHON
            
            msEntity::registryInPython();
            
            if( ! msMolecule::isMoleculeRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msMolecule,bases<msEntity>,boost::shared_ptr<msMolecule> >
                ("msMolecule",
                 "Object describing a reaction",no_init
                 )
                .def( "New", &msMolecule::New ,
                     "Create a new object.")
                .staticmethod("New")
		.def("setMotion",&msMolecule::setMotion,
		     "set the motion")
		.def("getMotion",&msMolecule::getMotion,
		     "return the motion");
                
		msTreeMapper::finalizeDeclarationForPython<msMolecule>("msMolecule");
                msMolecule::isMoleculeRegisteredInPython = 1;
            }
#endif
        }
        
    }
}

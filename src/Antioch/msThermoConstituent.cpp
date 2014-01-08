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


#include "msThermoConstituent.h"

namespace impact {
    
    namespace antioch {
        
        
        bool msThermoConstituent::isThermoConstituentRegisteredInPython = 0;
	
        msRegistrar msThermoConstituent::Registrar("msThermoConstituent", msThermoConstituent::createInstance);
        
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msThermoConstituent::registryInPython() {
            
#if USE_PYTHON
            
            msConstituent::registryInPython();
            
            if( ! msThermoConstituent::isThermoConstituentRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msThermoConstituent,bases<msConstituent>,boost::shared_ptr<msThermoConstituent> >
                ("msThermoConstituent",
                 "Object describing a reaction",no_init
                 )
                .def( "New", &msThermoConstituent::New ,
                     "Create a new object.")
                .staticmethod("New");
                
                msThermoConstituent::isThermoConstituentRegisteredInPython = 1;
            }
#endif
        }
        
    }
}

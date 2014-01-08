/*
 <one line to give the library's name and an idea of what it does.>
 Copyright (C) 2013  Guillaume <email>
 
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


#include "msElement.h"

namespace impact {
    
    namespace atomism {
        
        bool msElement::isElementRegisteredInPython=0;
        
        msRegistrar msElement::Registrar("msElement", msElement::createInstance);
        
        void msElement::registryInPython()
        {
#if USE_PYTHON
            msPhysicalInterface::registryInPython();
            if( ! msElement::isElementRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msElement,bases<msPhysicalInterface>,boost::shared_ptr<msElement> >
                ("msElement",
                 "An indiviseable ponctual mass",no_init)
                .def("getMass" ,    (double(msElement::*)() const )&msElement::getMass,
                     "return the mass of the element"
                     )
                .def("getPosition" ,   &msElement::getPosition,
                     "return the cartesian coordinates of the element"
                     )
                .def("getX" ,   &msElement::getX,
                     "return the X cartesian coordinates of the element"
                     )
                .def("getY" ,   &msElement::getY,
                     "return the Y cartesian coordinates of the element"
                     )
                .def("getZ" ,   &msElement::getZ,
                     "return the Z cartesian coordinates of the element"
                     );
                msElement::isElementRegisteredInPython=1;
            }
#endif
        }
        
        
    }
}

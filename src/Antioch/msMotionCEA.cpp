/*
 <one line to give the program's name and a brief idea of what it does.>
 Copyright (C) <year>  <name of author>
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#include "msMotionCEA.h"

namespace impact {
    
    namespace antioch {
        
        bool msMotionCEA::isMotionCEARegisteredInPython=0;
        
        msRegistrar msMotionCEA::Registrar("msMotionCEA", msMotionCEA::createInstance);
        
        
        ostream& operator<<(ostream& out,const msMotionCEA& rotor) {
            
            rotor.print(out);
            return out;
        }
        
        
        void msMotionCEA::registryInPython() {
            
#if USE_PYTHON
            msMotion::registryInPython();
            
            if( ! msMotionCEA::isMotionCEARegisteredInPython )
            {
                using namespace boost::python;
                
                class_< msMotionCEA, bases<msMotion>, boost::shared_ptr<msMotionCEA> >(
                       "msMotionCEA", "A motion described by CEA polynoms" ,
                        no_init )
                .def( "New", &msMotionCEA::New , "Create a new object.")
                .staticmethod("New");
		
		msTreeMapper::finalizeDeclarationForPython<msMotionCEA>("msMotionCEA");
		
                msMotionCEA::isMotionCEARegisteredInPython=1;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        ostream& msMotionCEA::print(ostream& out) const  {
            
            msMotion::print(out);
            output::printHeader(out,"CEA Motion");
            
            return out;
        }
    }
}



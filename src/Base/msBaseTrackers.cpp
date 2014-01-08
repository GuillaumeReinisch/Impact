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

#include "msBaseTrackers.h"
//#include <msSampler.h>

namespace impact
{
    
    bool msTrackerScalarFunction::isTrackerScalarFunctionRegisteredInPython = 0;
    msRegistrar msTrackerScalarFunction::Registrar("msTrackerScalarFunction", msTrackerScalarFunction::createInstance);
    
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    
    void msTrackerScalarFunction::registryInPython() {
        
#if USE_PYTHON
        
        msTracker::registryInPython();
        
        if( ! msTrackerScalarFunction::isTrackerScalarFunctionRegisteredInPython ) {
            
            using namespace boost::python;
            
            class_<msTrackerScalarFunction,bases<msTracker>,boost::shared_ptr<msTrackerScalarFunction> >
            ("msTrackerScalarFunction",
             "Track the mass fraction propertie of a msConstituent object. ",no_init
             )
            .def( "New", &msTrackerScalarFunction::New ,
                 "Create a new object.")
            .staticmethod("New");
            
            msTrackerScalarFunction::isTrackerScalarFunctionRegisteredInPython = 1;
        }
#endif
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& msTrackerScalarFunction::print(std::ostream& out) const {
        
        msTracker::print(out);
        output::printHeader(out,"TrackerScalarFunction");
        
        return out<<endl;
    }
    
}

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

#include "msTracker.h"



namespace impact
{
    
    
    bool msTracker::isTrackerRegisteredInPython=0;
    
    msRegistrar msTracker::Registrar("msTracker", msTracker::createInstance);
    
    std::ostream& operator<<(std::ostream& out,const msTracker& fit)
    {
        fit.print(out);
        return out;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msTracker::registryInPython()
    {
#if USE_PYTHON
        msTreeMapper::registryInPython();
        
        if( ! msTracker::isTrackerRegisteredInPython ) {
            
            using namespace boost::python;
            
            class_<msTracker,bases<msTreeMapper>,boost::shared_ptr<msTracker> >
            ("msTracker",
             "Virtual base class for tracker: follow an object property with respect to a time indicator.",no_init )
            .def("setTrackedObject",&msTracker::setTrackedObject,
                 "set the object to be tracked. arg2: object")
	    .def("getProperty",&msTracker::getProperty,
                 "return the values.");
            
            msTracker::isTrackerRegisteredInPython=1;
            
            registerVector<msTracker>("VectorOfTracker","This object stores a vector of object deriving from msTracker");
            
        }
#endif
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& msTracker::print(std::ostream& out) const {
        
        msTreeMapper::print(out);
        
        output::printHeader(out,"Tracker");
        return out;
    }
    
    
}

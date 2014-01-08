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

#include "msVisualizer.h"



namespace impact
{
    
    
    bool msVisualizer::isVisualizerRegisteredInPython=0;
    
    msRegistrar msVisualizer::Registrar("msVisualizer", msVisualizer::createInstance);
    
    std::ostream& operator<<(std::ostream& out,const msVisualizer& fit)
    {
        fit.print(out);
        return out;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msVisualizer::registryInPython()
    {
#if USE_PYTHON
        msTreeMapper::registryInPython();
        
        if( ! msVisualizer::isVisualizerRegisteredInPython ) {
            
            using namespace boost::python;
            
            class_<msVisualizer,bases<msTreeMapper>,boost::shared_ptr<msVisualizer> >
            ("msVisualizer",
             "Virtual base class for visulalizers.",init<>() );
            
            msVisualizer::isVisualizerRegisteredInPython=1;
    
        }
#endif
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& msVisualizer::print(std::ostream& out) const {
        msTreeMapper::print(out); return out;
    }
    
    
}

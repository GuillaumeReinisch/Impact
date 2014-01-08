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

#include "msParameterSpace.h"

namespace impact {
    
    namespace queso {
        
        
        bool msModelParameter::isModelParameterRegisteredInPython=0;
        
        msRegistrar msModelParameter::Registrar( "msModelParameter" , msModelParameter::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& operator<<(std::ostream& out,const msModelParameter& bayes) {
            
            return( bayes.print(out) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msModelParameter::registryInPython(){
            
#if USE_PYTHON
            msScalarVariable::registryInPython();
            
            if(  ! msModelParameter::isModelParameterRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msModelParameter,bases<msScalarVariable>,boost::shared_ptr<msModelParameter> >
                ("msModelParameter",  "a model parameter",init<>()
                 )
                .def( "New",&msModelParameter::New ,"Create a new msModelParameter object" )
                .staticmethod("New");
                
                msModelParameter::isModelParameterRegisteredInPython = 1 ;
            }
#endif
        }

        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        bool msParameterSpace::isParameterSpaceRegisteredInPython=0;
        
        msRegistrar msParameterSpace::Registrar( "msParameterSpace" , msParameterSpace::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& operator<<(std::ostream& out,const msParameterSpace& bayes) {
            
            return( bayes.print(out) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msParameterSpace::registryInPython(){
            
#if USE_PYTHON
            msPhysicalInterface::registryInPython();
            
            if(  ! msParameterSpace::isParameterSpaceRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msParameterSpace,bases<msGeneralizedCoordinates>,boost::shared_ptr<msParameterSpace> >
                ("msParameterSpace",  "Container for model parameters",init<>()
                 )
                .def( "New",&msParameterSpace::New ,"Create a new msParameterSpace object" )
                .staticmethod("New");
                
                msParameterSpace::isParameterSpaceRegisteredInPython = 1 ;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        msParameterSpace::msParameterSpace() : msGeneralizedCoordinates() {
            
            constructVar("msParameterSpace","ParameterSpace","parameter space");
        }
    }
}
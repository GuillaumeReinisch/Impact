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


#include "msStepFunction.h"


namespace impact
{
    
    namespace math {
        
        
        bool msStepFunction::isStepFunctionRegisteredInPython = 0;
        
        msRegistrar msStepFunction::Registrar("msStepFunction", msStepFunction::createInstance);
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msStepFunction::registryInPython(){
            
#if USE_PYTHON
            if( ! msStepFunction::isStepFunctionRegisteredInPython ) {
                
                using namespace boost::python;
                msScalarFunction::registryInPython();
                
                class_< msStepFunction, bases<msScalarFunction>, boost::shared_ptr<msStepFunction> >(
                "msStepFunction", "step function" , init<>() )
                .def( "New", &msStepFunction::New , "Create a new object.")
                .staticmethod("New")
                .def("setCoordinateIndex",&msStepFunction::setCoordinateIndex , "set the coordinate index. arg2: Index of the x-coordinate in the generalized coordinates container.")
                .def("setSteps",&msStepFunction::setSteps , "set the steps. arg2: Index of the x-coordinate in the generalized coordinates, arg2: Minimum of the domains, arg3: Maximum of the domains; arg4: values Values in the domains");
                msStepFunction::isStepFunctionRegisteredInPython=1;
            }
#endif
        };
        
    }
}


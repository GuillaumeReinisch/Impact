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


#include "msLinearScalarFunction.h"


namespace impact
{
    
    namespace math {
        
        
        bool msLinearScalarFunction::isLinearScalarFunctionRegisteredInPython = 0;
        
        msRegistrar msLinearScalarFunction::Registrar("msLinearScalarFunction", msLinearScalarFunction::createInstance);
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        void msLinearScalarFunction::registryInPython(){
            
#if USE_PYTHON
            if( ! msLinearScalarFunction::isLinearScalarFunctionRegisteredInPython ) {
                
                using namespace boost::python;
                msScalarFunction::registryInPython();
                
                class_< msLinearScalarFunction, bases<msScalarFunction>, boost::shared_ptr<msLinearScalarFunction> >(
                                                                                                                     "msLinearScalarFunction", "a linear scalar function: y(q)= a.q[i] + b" ,
                                                                                                                     no_init )
                .def( "New", &msLinearScalarFunction::New , "Create a new object.")
                .staticmethod("New");
                msLinearScalarFunction::isLinearScalarFunctionRegisteredInPython=1;
            }
#endif
        };
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msLinearScalarFunction::setCoefsIndice(double a0, double a1,int i){
            
            a=a1; b=a0; Indice=i; return mySharedPtr();
        }
        
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------
        
        double msLinearScalarFunction::evaluate(){
            
            if(Indice>=getCoordinates()->noOfDim())
                IMPACT_THROW_EXCEPTION(msException("Indice of the coordinate out of bounds the coordinates container ",
                                              "double msLinearScalarFunction::evaluate()",getFullId())
                                      );
            return( a * (*getCoordinates())[Indice]->getValue() + b );
        }
    }
}


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

#include "msLikeliNoCorrel.h"

namespace impact {
    
    namespace queso {
        
        
        bool msLikeliNoCorrel::isLikeliNoCorrelRegisteredInPython=0;
        
        msRegistrar msLikeliNoCorrel::Registrar( "msLikeliNoCorrel" , msLikeliNoCorrel::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msLikeliNoCorrel::registryInPython(){
            
#if USE_PYTHON
            
            msLikelihood::registryInPython();
            
            if(  ! msLikeliNoCorrel::isLikeliNoCorrelRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msLikeliNoCorrel,bases<msLikelihood>,boost::shared_ptr<msLikeliNoCorrel> >
                ("msLikeliNoCorrel",  "likelihood based on an independant model error assumption.",init<>()
                 )
                .def( "New",&msLikeliNoCorrel::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
		.def("_likelihood_",&msLikeliNoCorrel::likelihood,
		     "return the likelihood between prediction and measure for uncorrelated model error. arg2:prediction; arg3: measure; arg4: QoI index.");
                
                msLikeliNoCorrel::isLikeliNoCorrelRegisteredInPython = 1 ;
            }
            
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        msLikeliNoCorrel::msLikeliNoCorrel() : msLikelihood() {
            
            constructVar("msLikeliNoCorrel","LikeliNoCorrel","likelihood no correlation");
        }
    }
}
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

#include "msLikeliNoCorrelAdd1Gaussian.h"

namespace impact {
    
    namespace queso {
        
        
        bool msLikeliNoCorrelAdd1Gaussian::isLikeliNoCorrelAdd1GaussianRegisteredInPython=0;
        
        msRegistrar msLikeliNoCorrelAdd1Gaussian::Registrar( "msLikeliNoCorrelAdd1Gaussian" , msLikeliNoCorrelAdd1Gaussian::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msLikeliNoCorrelAdd1Gaussian::registryInPython(){
            
#if USE_PYTHON
            
            msLikeliNoCorrel::registryInPython();
            
            if(  ! msLikeliNoCorrelAdd1Gaussian::isLikeliNoCorrelAdd1GaussianRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msLikeliNoCorrelAdd1Gaussian,bases<msLikeliNoCorrel>,boost::shared_ptr<msLikeliNoCorrelAdd1Gaussian> >
                ("msLikeliNoCorrelAdd1Gaussian",  "likelihood based on an independant model error assumption.",init<>()
                 )
                .def( "New",&msLikeliNoCorrelAdd1Gaussian::New ,"Return a shared pointer to a new object" )
                .staticmethod("New");
                
                msLikeliNoCorrelAdd1Gaussian::isLikeliNoCorrelAdd1GaussianRegisteredInPython = 1 ;
            }
            
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        msLikeliNoCorrelAdd1Gaussian::msLikeliNoCorrelAdd1Gaussian() : msLikeliNoCorrel() {
            
            constructVar("msLikeliNoCorrelAdd1Gaussian","LikeliNoCorrelAdd1Gaussian","likelihood");
        }
    }
}
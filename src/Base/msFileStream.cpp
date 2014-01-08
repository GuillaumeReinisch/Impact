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

#include "msFileStream.h"


namespace impact {
    
        
        boost::shared_ptr<msFileStream> FileStream;
        
        bool msFileStream::isFileStreamRegisteredInPython=0;
        
        msRegistrar msFileStream::Registrar( "msFileStream" , msFileStream::createInstance );
             
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msFileStream::registryInPython(){
            
#if USE_PYTHON
            msTreeMapper::registryInPython();
            
            if(  ! msFileStream::isFileStreamRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msFileStream,bases<msTreeMapper>,boost::shared_ptr<msFileStream> >
                ("msFileStream",  "Input output TreeMapper object in/from files"
                 )
                .def( "New",&msFileStream::New ,"Return a shared pointer to a new object" )
                .staticmethod("New");
                
                msFileStream::isFileStreamRegisteredInPython = 1 ;
		
            }
#endif
        }
        
       
}


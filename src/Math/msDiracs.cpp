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


#include "msDiracs.h"

namespace impact {
    
    
    namespace math{
        
        bool msDiracs::isDiracsRegisteredInPython=0;
        msRegistrar msDiracs::Registrar( "msDiracs" , msDiracs::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& operator<<(std::ostream& out,const msDiracs& pol) {
            
            return( pol.print(out) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msDiracs::registryInPython()
        {
#if USE_PYTHON
            msScalarFunction::registryInPython();
            
            if(  ! msDiracs::isDiracsRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msDiracs,bases<msScalarFunction>,boost::shared_ptr<msDiracs> >
                ("msDiracs",  "Dirac scalar function",no_init
                 )
                .def( "New",&msDiracs::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
                .def( "getSampler",&msDiracs::getSampler,"Return the sampler")
                .def( "load",&msDiracs::load,"Load the database from file. The first line is considered a comment. For the next lines: the first values are interpreted as coordinates, the value after the last coordinate is interpreted as the field value. arg2: filename")
                .def( "noOfValues",&msDiracs::noOfValues,"return the numbers of element int the database")
                .def( "addDirac",&msDiracs::addDirac,"add a dirac to the function. arg2: coordinates, arg3: value");
                msDiracs::isDiracsRegisteredInPython = 1 ;
                
                msTreeMapper::finalizeDeclarationForPython<msDiracs>("msDiracs");
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msDiracs::load(string filename) {
            
            ifstream input(filename.c_str());
            if(!input.is_open())
                IMPACT_THROW_EXCEPTION(msException("can not open file of name "+filename+". The file exists?","boost::shared_ptr<msTreeMapper> msDiracs::load(string filename)",getFullId()) );
            
            char line[500];  bool eof=0;
            input.getline(line,500);
            vector<double> coors(getCoordinates()->noOfDim(),0);
            
            while(!eof){
                
                double value;
                for( size_t i=0; (i<getCoordinates()->noOfDim()) && (!eof); i++) {
                    input>>coors[i];
                    eof=input.eof();
                } 
                input>>value;
                addDirac(coors,value);
                input.getline(line,500);
            }
            
            input.close();
            return mySharedPtr();
        }
        
    }
}

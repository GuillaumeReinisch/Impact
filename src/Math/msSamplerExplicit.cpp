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


#include "msSamplerExplicit.h"

namespace impact {
    
    
    namespace math{
        
        bool msSamplerExplicit::isSamplerExplicitRegisteredInPython=0;
        msRegistrar msSamplerExplicit::Registrar( "msSamplerExplicit" , msSamplerExplicit::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& operator<<(std::ostream& out,const msSamplerExplicit& sampler) {
            
            return( sampler.print(out) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msSamplerExplicit::registryInPython()
        {
#if USE_PYTHON
            msSampler::registryInPython();
            
            if(  ! msSamplerExplicit::isSamplerExplicitRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msSamplerExplicit,bases<msSampler>,boost::shared_ptr<msSamplerExplicit> >
                ("msSamplerExplicit",  "A sampler for which the sampling data are explicitely defined",no_init
                 )
                .def( "New",&msSamplerExplicit::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
                .def("addSample",(boost::shared_ptr<msTreeMapper>(msSamplerExplicit::*)(vector<double>) )&msSamplerExplicit::addSample,
                     "add a sample to the list. arg2: vector of values")
                .def("iterate", &msSamplerExplicit::iterate,
                     "iterate the current sample.")
                .def("noOfSamples", &msSamplerExplicit::noOfSamples,
                     "return the number of sample.")
                .def("noOfDim", &msSamplerExplicit::noOfDim,
                     "return the dimension of a sample.")
                .def("setSample", &msSamplerExplicit::setSample,
                     "set a generalized coordinate container to a particular sample of the chain. arg2: coordinates; arg3: index")
                .def("currentSampleIndex",&msSamplerExplicit::currentSampleIndex,
                     "return the index of the current sample.")
                .def("setSampleIndex",&msSamplerExplicit::setSampleIndex,
                     "set the current sample's index")
                .def("setCurrentSample", &msSamplerExplicit::setCurrentSample,
                     "set a generalized coordinate container to the sample of current index. arg2: coordinates.")
                ;
                msSamplerExplicit::isSamplerExplicitRegisteredInPython = 1 ;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
                  
       bool msSamplerExplicit::getNextPoint(boost::shared_ptr<msGeneralizedCoordinates> coor){
                
	  if(Samples.size()==0) { 
	     return 0; 
	  }
	  
          if( CurrentIndex>=Samples.size()-1 ){ 
	     CurrentIndex=0; 
	     return 0;	    
	  }
          
          if(coor->noOfDim() != Samples[CurrentIndex].size() )
               IMPACT_THROW_EXCEPTION(msException("coordinates (size="+output::getString<int>(coor->noOfDim())+") and data points loaded "+
                                             "(size="+output::getString<int>( Samples[CurrentIndex].size())+") are not coherent ("+
                                             "CurrentIndex="+output::getString<int>( CurrentIndex )+")",
                                             "bool getNextPoint(boost::shared_ptr<msGeneralizedCoordinates> coor)",
                                             getFullId())
                                     );
           coor->setCoordinates(Samples[CurrentIndex]);
           CurrentIndex++;
           nScanned++;
	   
           return 1;
        };
	
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
                    
        boost::shared_ptr<msTreeMapper> msSamplerExplicit::getRndPoint(boost::shared_ptr<msGeneralizedCoordinates> coor){
                
            CurrentIndex = Samples.size() * std::rand() / double(RAND_MAX);
            coor->setCoordinates(Samples[CurrentIndex]);
            return mySharedPtr();
        };
            
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        	    
        ostream& msSamplerExplicit::print(ostream& out) const{
            
            msSampler::print(out);
            output::printHeader(out,"msSamplerExplicit");
            
            out<<"Sampling point:"<<endl;
            //impact::operator<<(out,Samples)<<endl;
            return out;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
    }
    
}

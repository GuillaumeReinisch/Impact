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


#include "msSamplerRandom.h"

namespace impact {
    
    
    namespace math{
        
        bool msSamplerRandom::isSamplerRandomRegisteredInPython=0;
        msRegistrar msSamplerRandom::Registrar( "msSamplerRandom" , msSamplerRandom::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& operator<<(std::ostream& out,const msSamplerRandom& sampler) {
            
            return( sampler.print(out) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msSamplerRandom::registryInPython()
        {
#if USE_PYTHON
            msSampler::registryInPython();
            
            if(  ! msSamplerRandom::isSamplerRandomRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msSamplerRandom,bases<msSampler>,boost::shared_ptr<msSamplerRandom> >
                ("msSamplerRandom",  "A sampler for which the sampling data are explicitely defined",no_init
                 )
                .def( "New",&msSamplerRandom::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
                .def("getNoOfSamples", &msSamplerRandom::getNoOfSamples,
                     "return the number of sample to draw.")
                .def("setNoOfSamples", &msSamplerRandom::setNoOfSamples,
                     "set the number of sample to draw. arg2: number of samples");
		
		msTreeMapper::finalizeDeclarationForPython<msSamplerRandom>("msSamplerRandom");
		
                msSamplerRandom::isSamplerRandomRegisteredInPython = 1 ;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
                  
       bool msSamplerRandom::getNextPoint(boost::shared_ptr<msGeneralizedCoordinates> coor){
                
	  LOGGER_ENTER_FUNCTION_DBG("bool msSamplerRandom::getNextPoint(boost::shared_ptr<msGeneralizedCoordinates> coor)",
                                  getFullId());
	  LOGGER_WRITE(msLogger::DEBUG,"Number of sample: "+output::getString<int>(getNoOfSamples())+
		       " current: "+output::getString<int>(nScanned));
	  
	  if(nScanned>=getNoOfSamples()) {
	    
	      LOGGER_EXIT_FUNCTION2("bool msSamplerRandom::getNextPoint(boost::shared_ptr<msGeneralizedCoordinates> coor)");
	      nScanned=0;
	      return 0;
	  }	  
          coor->setRandom();
	  nScanned++;
	  LOGGER_EXIT_FUNCTION2("bool msSamplerRandom::getNextPoint(boost::shared_ptr<msGeneralizedCoordinates> coor)");
          return 1;
        };
	
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        	    
        ostream& msSamplerRandom::print(ostream& out) const{
            
            msSampler::print(out);
            output::printHeader(out,"msSamplerRandom");
            
            out<<"Number of samples to draw: "<<getNoOfSamples()<<endl;
            return out;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
    }
    
}

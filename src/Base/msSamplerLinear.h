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


#ifndef MSSAMPLERLINEAR_H
#define MSSAMPLERLINEAR_H

#include<msSampler.h>

namespace impact
{

class msSamplerLinear : public msSampler
{

protected:	
      //!\name from msRegister 
      //@{
      static msRegistrar Registrar;
      static bool  isSamplerLinearRegisteredInPython;
      static msRegister* createInstance() { return new msSamplerLinear; }
      void registryInPython();
      //@} 

      msSamplerLinear() : msSampler(){ constructVar("msSamplerLinear","SamplerLinear","linear sampler"); }  
      
public:
        
     static boost::shared_ptr<msSamplerLinear> New(){
             
               boost::shared_ptr<msSamplerLinear> T( new msSamplerLinear );
	       LOGGER_ENTER_FUNCTION_DBG("msSamplerLinear::New()",T->getFullId());
	       T->initialize(); T->update();
	       LOGGER_EXIT_FUNCTION();
	       return T;
	       }

     virtual boost::shared_ptr<msTreeMapper> clone() { 
            boost::shared_ptr<msSamplerLinear> clone = New(); 
	    clone->msTreeMapper::operator=(*this);					      
	    return clone;
            }    
     
      virtual bool getNextPoint(boost::shared_ptr<msGeneralizedCoordinates> coor) {
	
	  setCoordinates(coor); return getNextPoint(); 
	  };

      //msSampler::msSamplingData sample(msScalarFunction& fct);

      std::ostream& print(std::ostream&) const;

private:
	
      size_t 	noOfScanned(){return nScanned;}
	
//	size_t 	AffStep;       //!< Step to display the integration point when Integrate() is called
        	
	size_t 	FirstVar();

	size_t 	NextVar(int j0);
	
	size_t 	noOfVar();
	
        bool 	getNextPoint();

//	int 	nScanned;      //!< number of point already Scanned		

        
 };
 std::ostream& operator<<(std::ostream& out,const msSampler& sampler);
}

#endif // MSSAMPLER_H

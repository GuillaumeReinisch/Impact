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


#include "msSamplerLinear.h"


namespace impact
{

bool msSamplerLinear::isSamplerLinearRegisteredInPython=0;
msRegistrar msSamplerLinear::Registrar("msSamplerLinear", msSamplerLinear::createInstance);
 
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out,const msSamplerLinear& sampler)
{
 sampler.print(out);
 return out;
 }
 
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

void msSamplerLinear::registryInPython()
{
  #if USE_PYTHON
  msSampler::registryInPython();
  
  if( ! msSamplerLinear::isSamplerLinearRegisteredInPython ) {
    
    using namespace boost::python;
    
    class_<msSamplerLinear,bases<msSampler>,boost::shared_ptr<msSamplerLinear> >
         ("msSamplerLinear",
	   "Linear sampler.",no_init
	  )
       .def( "New", &msSamplerLinear::New , "Create a new object. Arg1: msUnitsManager object") 
       .staticmethod("New")
       .def(self_ns::str(self_ns::self));
      
    msSamplerLinear::isSamplerLinearRegisteredInPython=1;
   }
#endif
 }

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

size_t msSamplerLinear::FirstVar()
{
 int r=0;
 for(size_t j=0;j<getCoordinates()->noOfDim();j++)
    if( (*getCoordinates())[j]->getDq() != 0 ) { r = j; j = getCoordinates()->noOfDim(); 
				      }
    
 return r;
 }
 
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

size_t msSamplerLinear::NextVar(int j0)
{
 int r=0;
 for(size_t j=j0+1;j<getCoordinates()->noOfDim();j++)
     if( (*getCoordinates())[j]->getDq() != 0 ) { r = j; j = getCoordinates()->noOfDim(); }
     
 return r;
 }

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

size_t msSamplerLinear::noOfVar()
{ 
 int n=0;
 for(size_t j=0;j<getCoordinates()->noOfDim();j++)
    if( (*getCoordinates())[j]->getDq() != 0 )  n++; 
 return n;
 }

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

bool msSamplerLinear::getNextPoint()
{
 LOGGER_ENTER_FUNCTION_DBG("double msSamplerLinear::getNextPoint(boost::shared_ptr<msScalarFunction> fct)",
		        getFullId());
 if(nScanned==0) {
    for(size_t j=0;j<getCoordinates()->noOfDim();j++)
        if( (*getCoordinates())[j]->getDq() != 0 ) (*getCoordinates())[j]->setValue( (*getCoordinates())[j]->getMin() );
 nScanned++;
 LOGGER_EXIT_FUNCTION();
 return 1;
 }     

 (*getCoordinates())[FirstVar()]->setValue( (*getCoordinates())[FirstVar()]->getValue() + (*getCoordinates())[FirstVar()]->getDq() );
 for(size_t j=FirstVar();j<FirstVar()+noOfVar()-1;j++)
    {
     int next=NextVar(j);
     if( (*getCoordinates())[j]->getValue()  >= (*getCoordinates())[j]->getMax() - (*getCoordinates())[j]->getDq())
       {
	(*getCoordinates())[j]->setValue( (*getCoordinates())[j]->getMin() );
	(*getCoordinates())[next]->setValue( (*getCoordinates())[next]->getValue()  + (*getCoordinates())[next]->getDq() );
	}
     }
 if( (*getCoordinates())[FirstVar()+noOfVar()-1]->getValue() >= (*getCoordinates())[FirstVar()+noOfVar()-1]->getMax() ){
    LOGGER_EXIT_FUNCTION();
    return 0;
    }
 nScanned++;
 LOGGER_EXIT_FUNCTION();
 return 1;
 }

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

std::ostream& msSamplerLinear::print(std::ostream& out) const
{
 msSampler::print(out);
 output::printHeader(out,"SamplerLinear");
 return out;
 }
 
 
}

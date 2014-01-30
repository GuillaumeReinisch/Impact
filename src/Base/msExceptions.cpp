/*
 <one line to give the library's name and an idea of what it does.>
 Copyright (C) 2012  Guillaume <email>
 
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


#include "msExceptions.h"

#include "msLogger.h"

namespace impact
{

    msException::msException(std::string info, std::string function, std::string id) {
      
        Info=info;Id=id;Function=function;
	msLogElement* elem = msLogger::CurrentElement;
	//Function=elem->getFunctionName();
        while(elem->Parent != 0 ) {
	    addContext(elem->getFunctionName());
	    elem = elem->Parent;
	};
        }
        
    msException::msException(string info,string id){ 
  
        Info=info;Id=id;
        msLogElement* elem = msLogger::CurrentElement;
        Function=elem->getFunctionName();
        
        while(elem->Parent != 0 ) {
	    addContext(elem->getFunctionName());
	    elem = elem->Parent;
	}
   }
   
      
    void msException::registryInPython()
    {
#if USE_PYTHON > 0
        using namespace boost::python;
            
            class_< msException, boost::shared_ptr<msException> >(
	          "msException","base class for exception in impact",no_init);
            
#endif
    }
    
   char const*  msException::what() const throw() {
        
        message = "\nThrow in function "+Function+".\nThrow by the object "+Id+":\n\t "+output::printRed(Info)+".\n Contexts:\n";
        for(size_t i=0; i<Contexts.size();i++)  message+="-"+Contexts[i]+";\n";
        return  message.c_str();
    }
}
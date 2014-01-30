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


#ifndef MSEXCPETION_H
#define MSEXCPETION_H

#include <msUtilities.h>
#include <boost/exception/all.hpp>

#if USE_PYTHON > 0
#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#endif

#define IMPACT_THROW_EXCEPTION BOOST_THROW_EXCEPTION
#define IMPACT_THROW_NOT_IMPLEMENTED() throwNotImplemented( __PRETTY_FUNCTION__ )
#define IMPACT_EXCEPT_IF(FUNC,MESSAGE)  exceptIf( FUNC, MESSAGE, __PRETTY_FUNCTION__  , myConstSharedPtr()->getFullId() )
#define IMPACT_TRY(FUNC)  impact_try( FUNC, __PRETTY_FUNCTION__ )

class msLogger;

namespace impact
{
    
   
    class msException: public boost::exception, public std::exception
    {
       friend class msRegister;
       
    public:
      
        msException(std::string info, std::string function, std::string id);
        
        msException(std::string info, std::string id);
        
        void addContext(std::string str){ Contexts.push_back(str); }
        
        char const* what() const throw();
        	
        ~msException() throw () { }
     
    protected:
      
	std::string Info;
        
	std::string Function;
        
	std::string Id;
	
    private:
      
        std::vector<std::string> Contexts;
		
        static void registryInPython();
	
	mutable std::string message;
    };
    
    
    class msMethodNotDefined : public msException {
      
    public:
      
         msMethodNotDefined(std::string function) :
         msException("The method "+function+" is not defined",function,"")
        {  }      
    };
    
    class msExceptionInLogic : public msException {
      
    public:
      
         msExceptionInLogic(std::string message,std::string function,std::string id) :
         msException("exception in logic: "+message,function,id)
        {  }      
    };
    
    template<class Func>
    void exceptIf(Func func,std::string message,std::string method,std::string id) {
      
      if( func() ){  msExceptionInLogic e("The condition is not fullfilled: "+message,method,id);   
	             IMPACT_THROW_EXCEPTION(e);
	           }
    };
    
    template<class Func>
    void impact_try(Func func,std::string method) {
      
      try{ return func(); 
      }
      catch(msException& e0){
	
	  e0.addContext(method);
	  IMPACT_THROW_EXCEPTION(e0);
      }     
      catch(std::exception& e0){
	
	  msException e(e0.what(),method,"");
	  IMPACT_THROW_EXCEPTION(e);
      }  
    };
    
    void throwNotImplemented(std::string method) {
      msMethodNotDefined e(method);   
      IMPACT_THROW_EXCEPTION(e);
    };
    
}
#endif
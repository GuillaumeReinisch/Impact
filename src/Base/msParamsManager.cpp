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

#include "msParamsManager.h"
#include <msPhysicalInterface.h>

namespace impact{
    
    
    bool msParam::isParamRegisteredInPython=0;
    msRegistrar msParam::Registrar( "msParam" , msParam::createInstance );
    
    void msParam::registryInPython(){
        
        
#if USE_PYTHON
        msTreeMapper::registryInPython();
        
        if(  ! msParam::isParamRegisteredInPython ) {
            
            using namespace boost::python;
            
            class_<msParam,bases<msTreeMapper>,boost::shared_ptr<msParam> >
            ("msParam",  "Defines a parameter ",no_init
             )
            .def( "New", &msParam::New , "Create a new object.")
            .staticmethod("New")
            .def( "getValue" ,  &msParam::getValue,
                 "return the value of the parameter." )
            .def( "getUnit" , &msParam::getUnit ,
                 "return the unit of the parameter" )
            .def( "getAbstract" , &msParam::getAbstract ,
                 "return the abstract" );
            msParam::isParamRegisteredInPython= 1 ;
        }
#endif
        
    }
    
    
    
    bool msParamsManager::isParamsManagerRegisteredInPython=0;
    msRegistrar msParamsManager::Registrar( "msParamsManager" , msParamsManager::createInstance );
    
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    std::ostream& operator<<(std::ostream& out,const msParamsManager& params) {
        
        return( params.print(out) );
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    void msParamsManager::registryInPython()
    {
#if USE_PYTHON
        msTreeMapper::registryInPython();
        
        if(  ! msParamsManager::isParamsManagerRegisteredInPython ) {
            
            using namespace boost::python;
            
            class_<msParamsManager,bases<msTreeMapper>,boost::shared_ptr<msParamsManager> >
            ("msParamsManager",  "Defines a parameters container ",no_init
             )
            .def( "New", &msParamsManager::New , "Create a new object.")
            .staticmethod("New")
            .def( "getValue" ,  &msParamsManager::queryStringValue,
                 "return the value of a parameter as string" )
            .def( "getIntValue" , &msParamsManager::queryIntValue ,
                 "return the value of an integer parameter" )
            .def(self_ns::str(self_ns::self))
            .def( "getDoubleValue" ,
                 (double(msParamsManager::*)( std::string , const msUnitsManager&)) &msParamsManager::queryDoubleValue ,
                 "return the value of a double parameter in specific units system" )
            .def( "setParameter",
                 (boost::shared_ptr<msTreeMapper>  (msParamsManager::*)( std::string , std::string , const msUnitsManager&)) &msParamsManager::setParameter,
                 "set a value to a parameter. arg2: name of the parameter, arg3: value, arg4: units"  )
            .def( "listWhoUsingMe", &msParamsManager::listWhoUsingMe ,
                 "list the object using this as units system" )
            .def("getParameters", &msParamsManager::getParameters,
                 "return the parameters in a vector")
            ;
            
	    
            msParamsManager::isParamsManagerRegisteredInPython= 1 ;
        }
#endif
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    std::ostream& msParamsManager::print(std::ostream& out) const {
        
        
        if( Parameters.size()==0 ) return out;
        
        out<<setw(20)<<"Name"<<setw(20)<<"Value"<<setw(20)<<"Units"<<setw(50)<<"Abstract\n";
        out<<std::setprecision(3);
        msChildren<msParam>::const_iterator it;
        
        for( it = Parameters.begin(); it!= Parameters.end(); ++it)
        {
            out<<setw(20)<<(*it)->getId()<<setw(20)
            <<(*it)->Value<<std::setw(20)
            <<(*it)->Unit->getStr()<<std::setw(50)
            <<(*it)->Abstract<<endl;
        }
        out<<endl;
        return out;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    void msParamsManager::addParameter(string id,string val,string abstract,msUnit unit) {
      
        if ( Parameters.find(id) != Parameters.end() ) return;
	
        boost::shared_ptr<msParam> p=msParam::New();
        p->set(id,val,abstract,unit);
        p->setAffiliation( mySharedPtr() );
        addElementToChildren<msParam>(Parameters,p);
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msParamsManager::setParameter(string name , string value ,  const msUnitsManager& unit) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msParamsManager::setParameter(string name , string value ,  const msUnitsManager& unit)",getFullId());
        msChildren<msParam>::iterator it=Parameters.begin();
        it=Parameters.find(name);
        if(it==Parameters.end())
        {
            LOGGER_WRITE(msLogger::ERROR,"Parameter "+name+" not found");
            
            throw msError("Parameter "+name+" not found","void msParamsManager::setParameter(string name , double value ,  const msUnitsManager& unit)",getFullId());
            return mySharedPtr();
        }
        else
        {
            (*it)->Value=value; (*it)->Unit->reset(unit);
        }
        updateObjectsUsingMe();
        LOGGER_EXIT_FUNCTION();
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msParamsManager::setParameter(string name , double value ,  const msUnitsManager& unit) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msParamsManager::setParameter(string name , string value ,  const msUnitsManager& unit)",getFullId());
        msChildren<msParam>::iterator it=Parameters.begin();
        it=Parameters.find(name);
        if(it==Parameters.end())
        {
            LOGGER_WRITE(msLogger::ERROR,"Parameter "+name+" not found");
            
            throw msError("Parameter "+name+" not found","void msParamsManager::setParameter(string name , double value ,  const msUnitsManager& unit)",getFullId());
            return mySharedPtr();
        }
        
        (*it)->Value=output::getString<double>(value); (*it)->Unit->reset(unit);
        updateObjectsUsingMe();
        LOGGER_EXIT_FUNCTION();
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msParamsManager::setParameter(string name , double value ,  string unit) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msParamsManager::setParameter(string name , string value ,  string unit)",getFullId());
        msChildren<msParam>::iterator it=Parameters.begin();
        it=Parameters.find(name);
        
        if(it==Parameters.end())
            BOOST_THROW_EXCEPTION( msError("Parameter "+name+" not found","void msParamsManager::setParameter(string name , double value ,  const msUnitsManager& unit)",getFullId()) );
        
        if( !msUnit(unit).isConsistent( *( (*it)->Unit) ) )
            BOOST_THROW_EXCEPTION( msError("Parameter's unit not consistent with the original ( given: "+unit+", original:"+(*it)->Unit->getStr(),
                                           "void msParamsManager::setParameter(string name , double value ,  const msUnitsManager& unit)", getFullId()) );
        
        (*it)->Value=output::getString<double>(value);
        (*it)->Unit->set(unit);
        
        updateObjectsUsingMe();
        LOGGER_EXIT_FUNCTION();
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::vector< boost::shared_ptr<msParam> > msParamsManager::getParameters()  {
        
        msChildren<msParam>::iterator it = Parameters.begin();
        std::vector< boost::shared_ptr<msParam> > ChildrenForIteration;
        
        for( ; it != Parameters.end() ; ++it ){
            
            ChildrenForIteration.push_back((*it));
        }
        return ChildrenForIteration;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    vector< boost::shared_ptr<msParam> >::iterator msParamsManager::begin() {
        
        msChildren<msParam>::iterator it = Parameters.begin();
        ParametersForIteration.clear();
        
        for( ; it != Parameters.end() ; ++it ) ParametersForIteration.push_back( (*it) );
        return ParametersForIteration.begin();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    vector< boost::shared_ptr<msParam> >::iterator msParamsManager::end() {
        
        return ParametersForIteration.end();
    }
    
    msParamsManager::status_query msParamsManager::queryStringValue(string name,string& val)
    {
        msChildren<msParam>::iterator it = Parameters.begin();//find(name);
        
        if( it != Parameters.end() ) {
            val=(*it)->getValue(); return SUCESS;
        }
        msError e("The parameter queried ("+name+") does not exist",
                  "msParamsManager::status_query msParamsManager::queryDoubleValue(string name , double& val, const msUnitsManager& u )",
                  getFullId());
        BOOST_THROW_EXCEPTION(e);
        return NOT_FOUND;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    msParamsManager::status_query msParamsManager::queryIntValue(string name,int& val)
    {
        msChildren<msParam>::iterator it=Parameters.find(name);
        
        if( it != Parameters.end() ) {
            
            try { boost::lexical_cast <int> ( (*it)->getValue() );
            }
            catch(boost::bad_lexical_cast &e) {
                return WRONG_TYPE;
            }
            val = atoi( (*it)->getValue().c_str() );
            return SUCESS;
        }
        msError e("The parameter queried ("+name+") does not exist",
                  "msParamsManager::status_query msParamsManager::queryDoubleValue(string name , double& val, const msUnitsManager& u )",
                  getFullId());
        BOOST_THROW_EXCEPTION(e);
        return NOT_FOUND;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    msParamsManager::status_query msParamsManager::queryDoubleValue(string name , double& val, const msUnitsManager& u )
    {
        msChildren<msParam>::iterator it=Parameters.find(name);
        double v = 0;
	
        if( Parameters.find(name) != Parameters.end() ) {
            
            try { v = boost::lexical_cast <double> ( (*it)->getValue() ); 
	          val = v * u.convert( *((*it)->Unit) , 1 );
	          return SUCESS;
            }
            catch(boost::bad_lexical_cast &e) {
	      
	      msError e_("Can not convert the string '"+(*it)->getValue()+"' to double",
		        " msParamsManager::status_query msParamsManager::queryDoubleValue(string name , double& val, const msUnitsManager& u )",
			getFullId());
	     BOOST_THROW_EXCEPTION(e_);
	    }
        }
        msError e("The parameter queried ("+name+") does not exist",
                  "msParamsManager::status_query msParamsManager::queryDoubleValue(string name , double& val, const msUnitsManager& u )",
                  getFullId());
        BOOST_THROW_EXCEPTION(e);
        return NOT_FOUND;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    void msParamsManager::listWhoUsingMe()
    {
        std::vector< boost::weak_ptr<msPhysicalInterface> >::iterator it;
        
        for(it=LinkedObjects.begin(); it!= LinkedObjects.end() ; ++it){
            
            if ( boost::shared_ptr<msPhysicalInterface> ptr = (*it).lock() )
                std::cout<<ptr->getFullId()<<std::endl; 
        }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void msParamsManager::updateObjectsUsingMe() {
        
        std::vector< boost::weak_ptr<msPhysicalInterface> >::iterator it;
        
        for(it=LinkedObjects.begin(); it!= LinkedObjects.end() ; ++it){
            
            if ( boost::shared_ptr<msPhysicalInterface> ptr = (*it).lock() )
                ptr->updateParameters(); 
        }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void msParamsManager::addObjectUsingMe(boost::shared_ptr<msPhysicalInterface> obj) {
        
        std::vector< boost::weak_ptr<msPhysicalInterface> >::iterator it;
        
        for(it=LinkedObjects.begin(); it!= LinkedObjects.end() ; ++it){
            
            if ( boost::shared_ptr<msPhysicalInterface> ptr = (*it).lock() )
                if( ptr == obj ) return; 
        } 
        LinkedObjects.push_back( obj );	    
    } 
    
    
}

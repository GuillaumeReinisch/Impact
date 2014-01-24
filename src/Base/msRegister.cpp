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


#include "msRegister.h"

namespace impact
{
    char const*  msError::what() const throw() {
        
        message = "\nThrow in function "+Function+".\nThrow by the object "+Id+":\n\t "+output::printRed(Info)+".\n Contexts:\n";
        for(size_t i=0; i<Contexts.size();i++)  message+="-"+Contexts[i]+";\n";
        return  message.c_str();
    }
    
#if USE_PYTHON > 0
    
    BOOST_PYTHON_MODULE(libimpact)
    {
        
        using namespace boost::python;
        
        docstring_options local_docstring_options(true, true, false);
        std::cout<<"BOOST_PYTHON_MODULE(libimpact)"<<std::endl;
        std::map<std::string, msRegister::base_creator_fn>::iterator it;
        std::map<std::string, msRegister::base_creator_fn>& map = msRegister::registry();
        
        for( it = map.begin(); it!= map.end();++it) {
            
            std::cout<<"call registry in python of "<<it->first<<std::endl;
            it->second()->registryInPython();//L);
        };
        
    }
#endif
    
    
    bool impact::msRegister::isRegisterRegistredInPython = 0;
    
    //msRegistrar msRegister::Registrar("msRegister", msRegister::create);
    
    std::ostream& operator<<(std::ostream& out,msRegister& obj)
    {return obj.print(out);
    }
    
    std::vector<std::string> impact::msRegister::getClassesRegistrated() {
        
        std::vector<std::string> listClasses;
        std::map<std::string, msRegister::base_creator_fn>::iterator it;
        std::map<std::string, msRegister::base_creator_fn>& map = msRegister::registry();
        std::string str;
        
        for( it = map.begin(); it!= map.end();++it)
            listClasses.push_back(it->first);
        
        return listClasses;
    }
    
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    msRegistrar::msRegistrar(std::string name, msRegister::base_creator_fn func) {
        msRegister::registry()[name] = func;
    }
    
    //-------------------------------------------------------------------------------
    
    msRegister::RegistryMap& msRegister::registry() {
        
        static impact::msRegister::RegistryMap impl;
        return impl;
    }
    
#if USE_PYTHON > 0
    std::string msRegister::getRegistrated() {
        
        std::map<std::string, msRegister::base_creator_fn>::iterator it;
        std::map<std::string, msRegister::base_creator_fn>& map=msRegister::registry();
        std::string str;
        
        for( it = map.begin(); it!= map.end();++it)
            str += it->first + "\n";
        
        return str;
    }
#endif
    
    //-------------------------------------------------------------------------------
    
    void msRegister::registryInPython()
    {
#if USE_PYTHON > 0
        if( !msRegister::isRegisterRegistredInPython ) {
            
            using namespace boost::python;
            
           /* msVector< std::string >::registryInPython("msVector_string","a vector of string");
            msVector< double >::registryInPython("msVector_double","a vector of double");*/
            
            class_< std::vector<std::string>, boost::shared_ptr< std::vector<std::string> > >("msVector_string","a vector of string",init<>())
            .def(vector_indexing_suite<std::vector<std::string> >() );
            
            class_<std::vector<double>, boost::shared_ptr<std::vector<double> > >("msVector_double","a vector of double",init<>())
            .def(vector_indexing_suite<std::vector<double> >() );
            
            class_<std::vector<int>, boost::shared_ptr<std::vector<int> > >("msVector_int","a vector of integers",init<>())
            .def(vector_indexing_suite<std::vector<int> >() );
            
	    class_<std::map<std::string,double>, boost::shared_ptr< std::map<std::string,double> > >("msMap_string_double","a map string double")
            .def(map_indexing_suite< std::map<std::string,double> >() );
	    
            class_<msRegister, boost::shared_ptr<msRegister> >(
                    "msRegister",
                    "class handling registration in the tree database manager and in python" ,
                    init<>() )
            .def( "getClassesRegistrated",	&msRegister::getClassesRegistrated ,
                 "Return a list of the name of the classes registred")
            .staticmethod("getClassesRegistrated");
            msRegister::isRegisterRegistredInPython=1;
        }
#endif
    }
    
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    msRegister::msRegister(){ };
    
} //end namespace impact

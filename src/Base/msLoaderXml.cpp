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

#include "msLoaderXml.h"


namespace impact {
    
        
        boost::shared_ptr<msLoaderXml> LoaderXml;
        
        bool msLoaderXml::isLoaderXmlRegisteredInPython=0;
        
        msRegistrar msLoaderXml::Registrar( "msLoaderXml" , msLoaderXml::createInstance );
             
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msLoaderXml::registryInPython(){
            
#if USE_PYTHON
            msTreeMapper::registryInPython();
            
            if(  ! msLoaderXml::isLoaderXmlRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msLoaderXml,bases<msFileStream>,boost::shared_ptr<msLoaderXml> >
                ("msLoaderXml",  " Create TreeMapper objects from a xml file.",init<>()
                 )
                .def( "New",&msLoaderXml::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
		.def("parse",&msLoaderXml::parse ,"Parse a file. arg2: file path; arg3: object to fill" );
                
                msLoaderXml::isLoaderXmlRegisteredInPython = 1 ;
		
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
       
       boost::shared_ptr<msTreeMapper> msLoaderXml::parse( string filename, boost::shared_ptr<msTreeMapper> mapper) {
	    
           LOGGER_ENTER_FUNCTION("void msXmlLoader::open(std:string filename,boost::shared_ptr<msTreeMapper> mapper)",getFullId()); 

	   ifstream theFile(filename.c_str());
	   
	   if( ! theFile.is_open() ) {
	     
	       msError e("can not open the file "+filename,
			 "boost::shared_ptr<msTreeMapper> msLoaderXml::open( string filename, boost::shared_ptr<msTreeMapper> mapper)",
			 getFullId());
	       BOOST_THROW_EXCEPTION(e);
	   }
	   LOGGER_WRITE(msLogger::DEBUG,"file '"+filename+ "' open." );
	   
           vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
           buffer.push_back('\0');

	   rapidxml::xml_document<> Document;
           Document.parse<0>(&buffer[0]);
           RootNode = Document.first_node();
	   RootObject = mapper;
	   
           for( msTreeMapper::attributeIterator it = mapper->beginAttributes(); 
	        it!= mapper->endAttributes();
	        ++it) {
		   
	       rapidxml::xml_attribute<>* att = RootNode->first_attribute( (*it).first.c_str());
               mapper->setAttribute( (*it).first ,string( att->value() ) );
	   }
	   SymLinks.clear();
	   createObject( mapper, RootNode);
           mapper->update();
           
	   updateAll( mapper );
           replaceBySymbolicLinks();
           
           LOGGER_EXIT_FUNCTION2("void msXmlLoader::open(std:string filename,boost::shared_ptr<msTreeMapper> mapper)");
	   return mySharedPtr();
       };
       
       
       void msLoaderXml::updateAll(boost::shared_ptr<msTreeMapper> parent){
	 
	   LOGGER_ENTER_FUNCTION("void msLoaderXml::updateAll()", getFullId());  
	   
	   vector<boost::shared_ptr<msTreeMapper> > childs = getChildren();
	   
	   for(size_t i = 0 ; i<childs.size();i++){
	     
	       if( childs[i]->isAffiliatedTo(parent) ){
		
		   LOGGER_WRITE(msLogger::DEBUG,"child" + childs[i]->getVariableName(parent) + " is not symbolic link, update it" );
		   updateAll(childs[i]);
	       }
	   }
	   parent->update();  
	   LOGGER_EXIT_FUNCTION2("void msLoaderXml::updateAll()");  
       }
	
	
       void msLoaderXml::createObject( boost::shared_ptr<msTreeMapper> parent, rapidxml::xml_node<>* xmlnode ){
	 
	    LOGGER_ENTER_FUNCTION("void msLoaderXml::createObject( boost::shared_ptr<msTreeMapper> parent, rapidxml::xml_node<>* xmlnode )",
				  getFullId()); 
	    
	    if(xmlnode->first_node() == 0) {
	      
	        LOGGER_WRITE(msLogger::DEBUG,"no more children  " );
		LOGGER_EXIT_FUNCTION2("void msLoaderXml::createObject( boost::shared_ptr<msTreeMapper> parent, rapidxml::xml_node<>* xmlnode )");
		return;
	    }
	    LOGGER_WRITE(msLogger::DEBUG,"create the chidlren of xml node : "+string(xmlnode->name()) );
	    
	    for(rapidxml::xml_node<>* xmlChild = xmlnode->first_node();
		xmlChild!=xmlnode->last_node();
	        xmlChild = xmlChild->next_sibling()) {
	            
	        vector<string> strs;
	    
	        if( (xmlChild!=0) &&(xmlChild->name_size() >0 ) ) {
		  
		    LOGGER_WRITE(msLogger::DEBUG,"create object from xml node : " + string(xmlChild->name())  );fillVecFromString<string>(string(xmlChild->name()),strs,"-");
	            string vartype = strs[0];
	            string varname = strs[1];
		    			
		    boost::shared_ptr<msTreeMapper> child;
		
		    if( xmlChild->first_attribute("PointerTo") == 0 ){
		   
			 LOGGER_WRITE(msLogger::DEBUG,"Child not a symbolic link");
			 
		         if( ! parent->hasChildOfName(varname) ){
		       
		             LOGGER_WRITE(msLogger::DEBUG,"Object not included naturally, 'New' called");
			     child = newObject(xmlChild,parent);
			     parent->addChild( child , varname );   
		         }
		         else{
			     LOGGER_WRITE(msLogger::DEBUG,"Object included naturally");
		             child = parent->getChild(varname);
			     string childType = child->getLastType();
			     if(childType != vartype ) {
			  
			        LOGGER_WRITE(msLogger::DEBUG,"Type "+vartype+" requested, 'New' called");
			        child = newObject(xmlChild,parent);
			        parent->addChild( child , varname );  
			     }
		         }
		         LOGGER_WRITE(msLogger::DEBUG,"set attributes"); 
		     
		         for( msTreeMapper::attributeIterator it = child->beginAttributes(); 
			      it!= child->endAttributes();++it) {
			
			      rapidxml::xml_attribute<>* att = xmlChild->first_attribute( (*it).first.c_str() );
			      child->setAttribute( (*it).first , string( att->value() ) );
		         }
		         createObject( child, xmlChild );
		    }else{
		         LOGGER_WRITE(msLogger::DEBUG,"Add child as symbolic link");
		         SymLinks.push_back(msSymLink(parent, varname,string( xmlChild->first_attribute("PointerTo")->value() )));
		    }  
		}
	    }
	    
	    LOGGER_EXIT_FUNCTION2("void msLoaderXml::createObject( boost::shared_ptr<msTreeMapper> parent, rapidxml::xml_node<>* xmlnode )");
	}
              
       boost::shared_ptr<msTreeMapper> msLoaderXml::newObject( rapidxml::xml_node<>* xmlChild , boost::shared_ptr<msTreeMapper> parent) {
	 
	   LOGGER_ENTER_FUNCTION("void msLoaderXml::newObject( rapidxml::xml_node<> xmlchild , boost::shared_ptr<msTreeMapper> parent)",
				  getFullId());
	   vector<string> strs;
	   LOGGER_WRITE(msLogger::DEBUG,"new object from xml node " + string(xmlChild->name()) );
	   fillVecFromString<string>(xmlChild->name(),strs,"-");
	   boost::shared_ptr<msTreeMapper> child = boost::shared_ptr<msTreeMapper>( msRegister::New<msTreeMapper>(strs[0]) );
	   boost::shared_ptr<msTreeMapper> a = child->mySharedPtr();
	   child->initialize();
	   child->update();
	   LOGGER_EXIT_FUNCTION2("void msLoaderXml::newObject( rapidxml::xml_node<> xmlchild , boost::shared_ptr<msTreeMapper> parent)");  
           return child;
	}
    
  
       void msLoaderXml::replaceBySymbolicLinks(){
         
	   LOGGER_ENTER_FUNCTION("void msLoaderXml::replaceBySymbolicLinks()",
				  getFullId());
	   
           vector<msSymLink>::iterator it = SymLinks.begin();
	   
           for(; it!= SymLinks.end(); ++it) {
	     
               boost::shared_ptr<msTreeMapper> obj = getObject( (*it).path );
               boost::shared_ptr<msTreeMapper> parent = obj->getParent();
               (*it).parent->addChild( obj ,(*it).varname );
               (*it).parent->update();
               obj->setAffiliation(parent);
	   }
           LOGGER_EXIT_FUNCTION2("void msLoaderXml::replaceBySymbolicLinks()");
       }
       
             
        boost::shared_ptr<msTreeMapper> msLoaderXml::getObject(string path) {

            vector<string> paths;
	    fillVecFromString<string>(path,paths,"/");
	
	    boost::shared_ptr<msTreeMapper> node = RootObject; 
	
	    for(size_t i = 0 ;i< paths.size();++i){
	  
	        if( paths[i]==".." ) { 
	          node->getParent();
	        }
	        else if (paths[i]!="") {
	          node = node->getChild( paths[i] );	        
	        }
	    }
	return node;
	  
	}
       
}


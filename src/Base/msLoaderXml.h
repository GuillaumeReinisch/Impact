

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


#ifndef MSLOADERXML_H
#define MSLOADERXML_H


#include <msFileStream.h>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/property_tree/xml_parser.hpp>


namespace impact {
    
   using namespace boost::property_tree::detail;
   
        /*! \class msLoaderXml
         * \brief  Create TreeMapper objects from a xml file
         */
   class msLoaderXml : public msFileStream
        {
	
            class msSymLink{
	      
	         friend class msLoaderXml;
		
	         boost::shared_ptr<msTreeMapper> parent;
		 std::string varname;
	         std::string path;
		 
		 msSymLink(boost::shared_ptr<msTreeMapper> p, string a,string b){
		   
		     parent = p; varname=a; path=b;
		}
	    };
	    
	    static msRegistrar Registrar;
            static bool  isLoaderXmlRegisteredInPython;
            static msRegister* createInstance() { return new msLoaderXml; }
            
            void registryInPython();
	    
        public:
            
            msLoaderXml() : msFileStream() {};
            
            void update()  {constructVar("msLoaderXml","LoaderXml","xml loader");
                
                msFileStream::update();
            };
            
            void initialize() {
                
                msFileStream::initialize();
            };
            
            static boost::shared_ptr<msLoaderXml> New(){
                
                boost::shared_ptr<msLoaderXml> T( new msLoaderXml );
                T->initialize();
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
	      
                boost::shared_ptr<msLoaderXml> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            boost::shared_ptr<msTreeMapper> parse( string filename, boost::shared_ptr<msTreeMapper> mapper);
	    
	    void createObject( boost::shared_ptr<msTreeMapper> parent, rapidxml::xml_node<>* node);
	    
	    boost::shared_ptr<msTreeMapper> newObject( rapidxml::xml_node<>* xmlchild , boost::shared_ptr<msTreeMapper> parent);
	    
	    void updateAll( boost::shared_ptr<msTreeMapper> parent );
	    
	    vector<msSymLink> SymLinks;
	    
	    boost::shared_ptr<msTreeMapper> getObject( string path);
	    
	    void replaceBySymbolicLinks();
	    
            rapidxml::xml_node<>* RootNode;  
	    boost::shared_ptr<msTreeMapper> RootObject;
      };
}   

#endif
/*


    def findInClass(self, cls0,myobject):
      
	for cls in cls0.__subclasses__():
	    name = str(cls).split('.')[1].split('\'')[0]
	    print name,myobject
            if(name == myobject):
	        print "found"
                self.class0 =cls
                return self.class0
                    
	    self.findInClass(cls,myobject)
	
      
    def getObject( self,  parent, path):

        node = self.root
        for elem in path.split("/")[2:]:
            if( elem==".."):
                node = node.getParent()
            elif(elem!=""):
                node = node.getChild(str(elem))
        return node
        
    def __newObject(self, xmlchild,parent):
        msLogger.enterFunction("void msXmlLoader::__newObject(self, xmlchild,parent)","")
        child = 0
        try:
            child = globals()[ xmlchild.tagName.split('-')[0] ].New()
        except:

	    try:
	        self.findInClass(msTreeMapper, xmlchild.tagName.split('-')[0] )
	        child = self.class0.New()
	    except:
                raise Exception('Can not create the object of name '+ str(xmlchild.tagName.split('-')[0]))
	      
        msLogger.exitFunction("void msXmlLoader::__newObject(self, xmlchild,parent)")
    
        return child
*/
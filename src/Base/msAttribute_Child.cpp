
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

#include "msAttribute_Child.h"
#include "eigen3/Eigen/src/Core/products/GeneralBlockPanelKernel.h"


/*
 std::istream& operator>>( std::istream& in, double& obj){
 
 string str;in >> str;
 obj=atof(str.c_str());
 return in;
 }
 std::istream& operator>>( std::istream& in, int& obj){
 
 string str;in >> str;
 obj=atoi(str.c_str());
 return in;
 }
 std::istream& operator>>( std::istream& in, bool& obj){
 
 string str;in >> str;
 obj=atoi(str.c_str());
 return in;
 }
 std::istream& operator>>( std::istream& in, size_t& obj){
 
 string str;in >> str;
 obj=atoi(str.c_str());
 return in;
 }
 std::istream& operator>>( std::istream& in, string& obj){
 
 string str;in >> str;
 obj=str;
 return in;
 }*/
/*
 std::istream& operator>>( std::istream& in, std::map<std::string , double >& obj) {
 
 std::vector< std::string > strs;
 std::string str;
 getline(in,str);
 fillVecFromString( str , strs , " ");
 
 std::vector< std::string >::const_iterator it = strs.begin();
 
 obj.clear();
 
 for(;it!=strs.end();++it) {
 
 std::vector< std::string > strs2;
 fillVecFromString( *it , strs2 , ":");
 obj[strs2[0]] =  boost::lexical_cast <double> (strs2[1]);
 }
 return in;
 }
 */

namespace impact{
    
    
    void lexical_cast( std::string str, double& obj) { obj=boost::lexical_cast <double> (str);};

    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    void lexical_cast( std::string str, int& obj)    { obj=atoi(str.c_str()); };

    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    void lexical_cast( std::string str, bool& obj)   { obj=atoi(str.c_str()); };
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    void lexical_cast( std::string str, size_t& obj) { obj=atoi(str.c_str()); };
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    void lexical_cast( std::string str, string& obj) { obj=str; };
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    void lexical_cast(  std::string str, std::vector< double >& obj){
        if(str=="")
        return;
        fillVecFromString( str , obj , " ");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    void lexical_cast(  std::string str, std::vector< string >& obj){
        if(str=="")
            return;
        fillVecFromString( str , obj , " ");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    void lexical_cast(  std::string str, std::vector< int >& obj){	fillVecFromString( str , obj , " ");
    }

    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    // [1,1,1]:2. [1,2,0]:1.5
    void lexical_cast(  std::string str, std::map< vector<int> , double >& obj){
        
        vector<std::string> strs;
        fillVecFromString( str , strs , " ");
        std::vector< std::string >::const_iterator it = strs.begin();
        obj.clear();
        for(;it!=strs.end();++it) {
            
            std::vector< std::string > strs2;
            fillVecFromString( *it , strs2 , ":");
            
            if( strs2.size()!=2 ) return;
            vector<int> indices;
            fillVecFromString<int>(strs2[0].substr(1,strs2[0].length()-2) , indices, ",");
            obj[indices]=boost::lexical_cast<double>( strs2[1].c_str() );
        }
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    // a:[0,2.2,3.5] b:[1,2.6,0.8,7.6] ...
    void lexical_cast(  std::string str, std::map< string , vector<double> >& obj){
        
        vector<std::string> strs;
        fillVecFromString( str , strs , " ");
        std::vector< std::string >::const_iterator it = strs.begin();
        obj.clear();
        for(;it!=strs.end();++it) {
            
            std::vector< std::string > strs2;
            fillVecFromString( *it , strs2 , ":");
            
            if( strs2.size()!=2 ) return;
            vector<double> values;
            fillVecFromString<double>(strs2[1].substr(1,strs2[1].length()-2) , values, ",");
            
            obj[ strs2[0].c_str() ] = values;
        }
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    void lexical_cast(  std::string str, std::map<std::string , double >& obj){
        
        vector<std::string> strs;
        fillVecFromString( str , strs , " ");
        
        std::vector< std::string >::const_iterator it = strs.begin();
        
        obj.clear();
        
        for(;it!=strs.end();++it) {
            
            std::vector< std::string > strs2;
            fillVecFromString( *it , strs2 , ":");
            
            if( strs2.size()!=2 ) return;
            
            obj[strs2[0]]=boost::lexical_cast<double>(strs2[1]);
        }
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    // [1,1,1.5] [1,2.3,0,5.5]
    void lexical_cast(  std::string str, std::vector< std::vector<double> >& obj){
        
        vector<std::string> strs;
        fillVecFromString( str , strs , " ");
        std::vector< std::string >::const_iterator it = strs.begin();
        obj.clear();
        for(;it!=strs.end();++it) {
            
            if( (*it).length() > 2 ){
                
                vector<double> indices;
                
                fillVecFromString<double>( (*it).substr(1,(*it).length()-2) , indices, ",");
                
                obj.push_back(indices);
            }
        }
    }
    
    
    std::ostream& operator<<( std::ostream& out , const msAttributeBase& obj) { return out<<obj.get();
    }
 
#if USE_PYTHON > 0
    
    template<class T>
    void registerAttribute(string type_id){
        
        using namespace boost::python;
        
        class_<msAttributePython<T>,bases<msAttributeBase>,boost::shared_ptr<msAttributePython<T> > >
        (string("msAttribute_"+type_id).c_str(),
         string("virtual base class for "+type_id+" attribute.").c_str(),init<T>()
         )
        .def("setValue",&msAttributePython<T>::setValue,"set the value. arg2: value.")
        .def("getValue",&msAttributePython<T>::getValue,"return the value")
        .def("getRefOnValue",&msAttributePython<T>::getRefOnValue,"return the reference of the value");
    }
    
#endif

    
    void registerAttributeTypesInPython() {
        
#if USE_PYTHON > 0
        
        using namespace boost::python;

        class_<msAttributeBase,boost::shared_ptr<msAttributeBase> >
        ("msAttributeBase",
         "virtual base class for attributes.",no_init
         );
        
        registerAttribute<int>("int");
        registerAttribute<string>("string");
        registerAttribute<vector<int> >("vectorInt");
        registerAttribute<vector<double> >("vectorDouble");
        registerAttribute<vector<string> >("vectorString");
#endif
        
    }
    
}

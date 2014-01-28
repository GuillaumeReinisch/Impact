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


#include "msTreeMapper.h"
#include <boost/graph/graph_concepts.hpp>

namespace impact
{
    
    bool impact::msTreeMapper::isTreeMapperRegistredInPython = 0;
      
    msRegistrar msTreeMapper::Registrar("msTreeMapper", msTreeMapper::create);
    
    std::ostream& operator<<(std::ostream& out,const msTreeMapper& obj) { return( obj.print(out) ); }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msTreeMapper::registryInPython() {
#if USE_PYTHON > 0
        msRegister::registryInPython();
        if( !msTreeMapper::isTreeMapperRegistredInPython ) {
            
            using namespace boost::python;
            
	    cout<<"void msTreeMapper::registryInPython() 2"<<endl;
            class_<msTreeMapper, bases<msRegister>, boost::shared_ptr<msTreeMapper> >(
                "msTreeMapper",
                "class handling registration in the tree database manager and in python" ,
                init<>() )
	    .def( "New", &msTreeMapper::New , "Create a new object.")
	    .staticmethod("New")
            .def( "_initialize_" , 		&msTreeMapper::initialize ,
                 "initialize the object" )
            .def( "_update_" , 		(void (msTreeMapper::*)() ) &msTreeMapper::update ,
                 "update the object" )
            .def( "_constructVar_" , &msTreeMapper::constructVar,
                 "should be called at the object construction. arg2: type name , arg3: variable name , arg4: id ")
            .def( "_declareAttribute_" ,
		  (void (msTreeMapper::*)(boost::shared_ptr<msAttributeBase>,std::string) )
                 &msTreeMapper::declareAttribute,
                 "declare an attribute. arg2: attribute; arg3: name" )
            .def( "_declareChild_" , 
		  (void (msTreeMapper::*)(boost::shared_ptr<msChildBase>, boost::shared_ptr<msTreeMapper> ,string ) )
                 &msTreeMapper::declareChild,
                 "declare a child. arg2: child; arg3: initial value; arg4: name" )
            .def( "_declareChildren_" , (void (msTreeMapper::*)(boost::shared_ptr<msChildrenBase>,string ) )
                 &msTreeMapper::declareChildren,
                 "declare a children. arg2: children; arg3: name" )
            .def( "_updateChild_" ,&msTreeMapper::updateChild,
                 "update a child already declared. arg2: child declared; arg3: new value" )
            .def( "_addElementToChildren_" ,
		  (void (msTreeMapper::*)(boost::shared_ptr<msChildrenBase>,boost::  shared_ptr<msTreeMapper>) )
                 &msTreeMapper::addElementToChildren,
                 "add an element to a children. arg2: children; arg3: element" )
            .def( "getId" , 		&msTreeMapper::getId ,
                 "Return the id of the object" )
	    .def( "getComments" , 		&msTreeMapper::getComments ,
                 "Return the comments of the object" )
	    .def( "getFullId" , 		&msTreeMapper::getFullId ,
                 "Return the full id (from the root) of the object" )
            .def( "setId" , 		&msTreeMapper::setId ,
                 "Set the id of the object. Arg1: id (string)" )
	    .def( "setComments" , 		&msTreeMapper::setComments ,
                 "Set the comments of the object. Arg1: comments (string)" )
            .def( "getType" , 		&msTreeMapper::getType ,
                 "Return the full C++ type of the object")
            .def( "getVariableName" , 	(std::string (msTreeMapper::*)(boost::shared_ptr<msTreeMapper>) const )
                 &msTreeMapper::getVariableName ,
                 "Return the variable name")
            .def( "getVariableNames" , 	&msTreeMapper::getVariableNames ,
                 "Return the variable names in a list")
            .def( "getFullVariableName" , 	&msTreeMapper::getFullVariableName ,
                 "Return the variable name chain from root")
            .def( "getParent" , 	&msTreeMapper::getParent ,
                 "Return the object's parent")
            .def( "mySharedPtr" , 	&msTreeMapper::mySharedPtr ,
                 "Return the object's pointer")
            .def( "addChild",	&msTreeMapper::addChild,
                 "Add a child to the object" )
            .def( "replaceChild",	&msTreeMapper::replaceChild,
                 "Add a child to the object, replace the previous child of same id if it exists" )
            .def( "deepCopy",      &msTreeMapper::deepCopy,
                 "Realizes a deep copy of an object. arg2: object to clone" )
            .def( "getChild" , &msTreeMapper::getChild,
                 "return the child of a particular id, return this if none exists" )
            .def( "getAttributes" , &msTreeMapper::getAttributes,
                 "return a list of the attributes. Each item is in the format 'key = value'" )
            .def( "getAttributeInString" , &msTreeMapper::getAttributeInString,
                 "return a particular attribute casted in string. arg2: name of the attribute" )
            .def( "setAffiliation", (void(msTreeMapper::*)( boost::shared_ptr<msTreeMapper> ) ) &msTreeMapper::setAffiliation,
                 "set the parent of the object" )
            .def( "setAttribute",  &msTreeMapper::setAttribute,
                 "set a declared attribute. arg2: id, arg3: value" )
            .def( "getChild" , &msTreeMapper::getChild,
                 "return a child of particular id, return 'self' if none exists" )
            .def("getChildren", &msTreeMapper::getChildren,
                 "return all the children in a vector")
            .def("getChildrenDerivedFrom", &msTreeMapper::getChildrenDerivedFrom<msTreeMapper>,
                 "return all the children derived from a particular type in a vector. arg2: type")
            .def( "isDerivedFrom" , &msTreeMapper::isDerivedFrom,
                 "return 1 if the object is derived from a particular type. arg2: type's name." )
            .def( "isAffiliatedTo" , &msTreeMapper::isAffiliatedTo,
                 "return 1 if the object is directly affiliated to a given object. arg2: given object." )
            .def( "hasParent" , &msTreeMapper::hasParent,
                 "return 1 if the object has a parent.")
            .def( "hasChildOfName" , &msTreeMapper::hasChildOfName,
                 "return 1 if a child of particular variable name exists. arg2: variable name")
            .def( "getSymbolicLinkPath" , &msTreeMapper::getSymbolicLinkPath,
                 "return the path of the symbolic link object. arg2: symbolic link object." )
            .def( "isToBeSaved" , &msTreeMapper::isToBeSaved,
                 "return true if the object is to be saved." )
            .def( "setToBeSaved" , &msTreeMapper::setToBeSaved,
                 "set the saveable propertie of the object. arg2: true or false." )
            .def("sanityCheck",&msTreeMapper::sanityCheck,
                 "check the structure is correctly initialized")
            .def(self_ns::str(self_ns::self));
            class_<msChildBase ,boost::shared_ptr<msChildBase> >( "msChildBase",
                                                                  "Use to declare child of msTreeMapper structure" ,
                                                                  no_init );
            class_<msChildrenBase ,boost::shared_ptr<msChildrenBase> >( "msChildrenBase",
                                                                        "Use to declare children of msTreeMapper structure" ,
                                                                        no_init );
            registerVector<msTreeMapper>("Vector_msTreeMapper","Vector of object deriving from msTreeMapper");
            registerAttributeTypesInPython();
	    msTreeMapper::isTreeMapperRegistredInPython=1;
        }
#endif
    }
    
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    msTreeMapper::msTreeMapper() :msRegister(){
        
        constructVar("msTreeMapper","TreeMapper","tree mapper");
        CppType="msTreeMapper:";
        toBeSaved = 1;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::string msTreeMapper::getId() const { return VariableAttributs.Id; }
    std::string msTreeMapper::getComments() const { return VariableAttributs.Comments; }   
    //-------------------------------------------------------------------------------
    
    std::string msTreeMapper::getFullId() const {
        
        std::stringstream aff;
        aff<<"["<<getType()<<"] ";writeAffiliation(aff);
        return aff.str();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::string msTreeMapper::getLastType() const {
        
        vector<string> strs;
        fillVecFromString<string>(CppType,strs,":");
        return strs.back();
    }
    //-------------------------------------------------------------------------------
    
    std::string msTreeMapper::getVariableName( const boost::shared_ptr<const msTreeMapper> child) const {
        
        for( const_childIterator it=Children.begin();
            it!=Children.end();
            it++) {
            
            if( (*it).second->getBasePtr() == child.get() ){
                return it->first;
            }
        }
        
        return "";
    }
    std::string msTreeMapper::getVariableName( boost::shared_ptr<msTreeMapper> child) const {
        
        for( const_childIterator it=Children.begin();
            it!=Children.end();
            it++) {
            
            if( (*it).second->getBasePtr() == child.get() ){
                return it->first;
            }
        }
        
        return "";
    }
    vector<std::string> msTreeMapper::getVariableNames() const {
        
        vector<std::string> names;
        for( const_childIterator it=Children.begin();
            it!=Children.end();
            it++) {
            names.push_back(it->first);
        }
        
        return names;
    }
    //-------------------------------------------------------------------------------
    
    std::string msTreeMapper::getFullVariableName() const {
        
        std::string name;
        if( boost::shared_ptr<msTreeMapper> ptr = Parent.lock() ){
            
            name = ptr->getFullVariableName() + "-" + VariableAttributs.Name;
        }
        else{name = "root";
        }
        return name;
    }
    
    //-------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msTreeMapper::setId(std::string id)  {
        
        VariableAttributs.Id=id; return mySharedPtr();
    }
    
   boost::shared_ptr<msTreeMapper> msTreeMapper::setComments(std::string comm)  {
        
        VariableAttributs.Comments=comm; return mySharedPtr();
    }
    //-------------------------------------------------------------------------------
    
    std::string msTreeMapper::getType() const { return CppType; }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msTreeMapper::declareChild(boost::shared_ptr<msChildBase> refchild, boost::shared_ptr<msTreeMapper> value, std::string name) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msTreeMapper::declareChild(boost::shared_ptr<msChildBase> refchild, boost::shared_ptr<msTreeMapper> value, std::string name)",getFullId());
        LOGGER_WRITE(msLogger::DEBUG,"Declare a child of name '" + name +"' and type '"+getType()+"'");
        
        if( !value->isDerivedFrom("msTreeMapper") )
            
            IMPACT_THROW_EXCEPTION( msException("The child you want to register is not derived from msTreeMapper, only msTreeMapper derived object can be declared as child",
                                           "void msTreeMapper::declareChild(msChild<T>& refchild, boost::shared_ptr<T> value, std::string name)",
                                           getFullId()
                                           ));
        
        refchild->set( value );
        value->VariableAttributs.Name = name;
        try{
            mySharedPtr()    = shared_from_this();
            refchild->getBasePtr()->Parent = mySharedPtr();
            Children[name]   = refchild.get();
            //RegistredChildren[name]   = refchild.get();
            std::stringstream out;
            out<<"child passed: "<<static_cast<void*>(&refchild)<<" adress in children: "<<static_cast<void*>(&Children[name])
            <<" adress of the object:"<<refchild.get();
            LOGGER_WRITE(msLogger::DEBUG,out.str());
        }
        catch( boost::exception &e )
        {
            Children[name]   = refchild.get();
            //RegistredChildren[name]= refchild.get();
            LOGGER_EXIT_FUNCTION2("void msTreeMapper::declareChild(boost::shared_ptr<msChildBase> refchild, boost::shared_ptr<msTreeMapper> value, std::string name)");
            return;
        }
        
        LOGGER_EXIT_FUNCTION2("void msTreeMapper::declareChild(boost::shared_ptr<msChildBase> refchild, boost::shared_ptr<msTreeMapper> value, std::string name)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msTreeMapper::updateChild(boost::shared_ptr<msChildBase> lhschild,boost::shared_ptr<msTreeMapper> rhschild) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msTreeMapper::updateChild(boost::shared_ptr<msChildBase>& lhschild,boost::shared_ptr<msTreeMapper> rhschild)",getFullId());
        LOGGER_WRITE(msLogger::DEBUG,"Id of the updated child:"+lhschild->getBasePtr()->getFullId());
        
        if( lhschild->getSharedBasePtr() == boost::shared_ptr<msTreeMapper>() )
            IMPACT_THROW_EXCEPTION( msException("the original (lhschild) child is not initialized, did you declare it?",
                                           "void msTreeMapper::update(msChild<T>& lhschild,boost::shared_ptr<T> rhschild)",
                                           getFullId()
                                           ));
        
        childIterator it = Children.begin();
        std::string varname="";
        
        for(;it!= Children.end();++it)
            if( it->second->getBasePtr() == lhschild->getBasePtr() )  varname = it->first;
        
        if( varname=="")
            IMPACT_THROW_EXCEPTION( msException("ref. to lhschild no found",
                                           "void msTreeMapper::update(msChild<T>& lhschild,boost::shared_ptr<T> rhschild)",
                                           getFullId()
                                           ));
        
        //lhschild.operator=(rhschild);
        
        lhschild->set(rhschild);
        rhschild->VariableAttributs.Name = varname;
        Children[varname]->set(rhschild );
        
        LOGGER_EXIT_FUNCTION2("void msTreeMapper::updateChild(boost::shared_ptr<msChildBase>& lhschild,boost::shared_ptr<msTreeMapper> rhschild)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msTreeMapper::declareChildren( boost::shared_ptr<msChildrenBase> refvec,std::string name) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msTreeMapper::declareChildren( boost::shared_ptr<msChildrenBase> refvec,std::string name)",getFullId());
        LOGGER_WRITE(msLogger::DEBUG,"Name of the children: "+name);
        RegistredVectorsOfChildren[name]= refvec.get();
        LOGGER_EXIT_FUNCTION2("void msTreeMapper::declareChildren( boost::shared_ptr<msChildrenBase> refvec,std::string name)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msTreeMapper::addElementToChildren( boost::shared_ptr<msChildrenBase> vector,boost::shared_ptr<msTreeMapper> elem) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msTreeMapper::addElementToChildren(boost::shared_ptr<msChildrenBase> vector,boost::shared_ptr<T> elem)",getFullId());
        
        std::map< std::string , msChildrenBase * >::iterator  it;
        it=RegistredVectorsOfChildren.begin();
        std::string varname="";
        for(;it!= RegistredVectorsOfChildren.end();++it)
            if( it->second == vector.get() )  varname = it->first;
        
        LOGGER_WRITE(msLogger::DEBUG,"elem's id: "+elem->getId()+"; vector name: "+varname);
        
        if( varname=="")
            IMPACT_THROW_EXCEPTION( msException("reference to the children not found, did you declare it in the constructor ?"
                                           ,"msTreeMapper::addElementToChildren(msChildren<T>& vector,boost::shared_ptr<T> elem)"
                                           ,getFullId() ) );
        
        vector->push_back(elem);
        std::stringstream name;name<<varname<<"_"<<getNextIndiceInChildren(varname);
        elem->VariableAttributs.Name=name.str();
        LOGGER_WRITE(msLogger::DEBUG,"child's variable name: "+name.str());
        
        Children[name.str()] = new msChildBase();
        Children[name.str()]->set(elem);
        
        LOGGER_EXIT_FUNCTION2("void msTreeMapper::addElementToChildren(boost::shared_ptr<msChildrenBase> vector,boost::shared_ptr<T> elem)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    size_t msTreeMapper::getNextIndiceInChildren(std::string vecname){
        
        size_t max=0;
        
        for( childIterator it=Children.begin();
            it!=Children.end();
            it++) {
            std::vector<std::string> strs;
            boost::split(strs, it->first, boost::is_any_of("_"));
            if( (strs.size()>1) && (strs[0]==vecname) ) max++;
        }
        return max;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msTreeMapper::writeAffiliation(std::ostream& out) const
    {
        boost::shared_ptr<msTreeMapper> ptr;
        if( boost::shared_ptr<msTreeMapper> ptr = Parent.lock() ){
            ptr->writeAffiliation(out);out<<"-"<<getId();
        }
        else{out<<"root-"<<getId();}
    }
    
    void msTreeMapper::writeAffiliationVariable(std::ostream& out) const
    {
        boost::shared_ptr<msTreeMapper> ptr;
        if( boost::shared_ptr<msTreeMapper> ptr = Parent.lock() ){
            
            const boost::shared_ptr<msTreeMapper const> a = shared_from_this();
            
            ptr->writeAffiliationVariable(out);out<<"-"<<ptr->getVariableName( a );
        }
        else{out<<"root-"<<VariableAttributs.Name;}
    }
    
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    bool msTreeMapper::isDerivedFrom(std::string target) const {
        
        std::vector<std::string> strs0;
        boost::split(strs0,target, boost::is_any_of(":"));
        
        std::vector<std::string> strs;
        boost::split(strs,CppType, boost::is_any_of(":"));
        return( std::find( strs.begin(), strs.end(), strs0.back() ) != strs.end() );
    }
    
    void msTreeMapper::exceptIfNotDerivedFrom(std::string target,std::string method) const {
	
	if( !isDerivedFrom(target) ) {
		     
	    stringstream out;
	    out<<"The objet "<<getFullId()<<" should be of type "<<target<<" but is of type "<<getType();
	    msException e( out.str(),method,getFullId() );
			
	    IMPACT_THROW_EXCEPTION(e);
	}
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msTreeMapper::addChild( boost::shared_ptr<msTreeMapper> ptr, std::string varName) {
        
        LOGGER_ENTER_FUNCTION("void msTreeMapper::addChild( boost::shared_ptr<msTreeMapper> ptr, std::string varName)",
                              getFullId());
        
        //ptr->Parent=shared_from_this();
        
        if(!ptr->hasParent()) ptr->setAffiliation(mySharedPtr());
        
        ptr->VariableAttributs.Name=varName;
        std::vector<std::string> strs;
        boost::split(strs,varName, boost::is_any_of("_"));
        
        
        if( RegistredVectorsOfChildren.find(strs[0])!= RegistredVectorsOfChildren.end()){
            
            std::stringstream message;
            
            size_t index = atoi(strs[1].c_str());
            
            if( index >= RegistredVectorsOfChildren[strs[0]]->size() ) {
                message<<"Push back child '"<<ptr->VariableAttributs.Name<<"' to the children list '"<<strs[0]
                <<"' as element indexed "<<RegistredVectorsOfChildren[strs[0]]->size();
                LOGGER_WRITE(msLogger::DEBUG, message.str());
                RegistredVectorsOfChildren[strs[0]]->push_back(ptr);
            }
            else{
                message<<"insert child '"<<ptr->VariableAttributs.Name<<"' to the children list '"<<strs[0]
                <<"' before element indexed "<<index;
                LOGGER_WRITE(msLogger::DEBUG, message.str());
                RegistredVectorsOfChildren[strs[0]]->insert( index, ptr);
            }
            LOGGER_WRITE(msLogger::DEBUG, "new size of the children list: "+output::getString<int>(RegistredVectorsOfChildren[strs[0]]->size()));
            /*std::stringstream name;name<<strs[0]<<"_"<<getNextIndiceInChildren(strs[0]);
             ptr->VariableAttributs.Name=name.str();*/
        }
        
        
        if(Children.find(ptr->VariableAttributs.Name)!=Children.end()){
            
            //LOGGER_WRITE(msLogger::WARNING, "A child already exist under the same variable name, it is overriden");
            Children[ptr->VariableAttributs.Name]->set(ptr);
            LOGGER_EXIT_FUNCTION2("void msTreeMapper::addChild( boost::shared_ptr<msTreeMapper> ptr, std::string varName)");
            return;
        }
        
        
        
        LOGGER_WRITE(msLogger::DEBUG, "add element '"+ptr->VariableAttributs.Name+"' to the child list");
        
        try{
            Children[ptr->VariableAttributs.Name] = new msChild<msTreeMapper>(ptr);
        }
        catch(...){
            IMPACT_THROW_EXCEPTION( msException("Can not create the child"
                                           ,"void msTreeMapper::addChild( boost::shared_ptr<msTreeMapper> ptr, std::string varName) "
                                           ,getFullId() ) );
        }
        LOGGER_WRITE(msLogger::DEBUG, "Element added");
        LOGGER_EXIT_FUNCTION2("void msTreeMapper::addChild( boost::shared_ptr<msTreeMapper> ptr, std::string varName)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msTreeMapper::replaceChild( boost::shared_ptr<msTreeMapper> ptr, std::string varName){
        
        LOGGER_ENTER_FUNCTION("void msTreeMapper::replaceChild( boost::shared_ptr<msTreeMapper> ptr, std::string varName)"
                              ,getFullId());
        
        if(Children.find(varName)==Children.end())
            IMPACT_THROW_EXCEPTION( msException("can not find the child to replace with variable name "+varName
                                           ,"void msTreeMapper::replaceChild( boost::shared_ptr<msTreeMapper> ptr, std::string varName) "
                                           ,getFullId() ) );
        
        stringstream out;
        out<<"replace child of variable name "<<varName<<" and adress "<<static_cast<void*>(&Children[varName]);
        LOGGER_WRITE(msLogger::DEBUG, out.str());
        
        if(!ptr->hasParent()) ptr->setAffiliation(mySharedPtr());
        
        std::vector<std::string> strs;
        boost::split(strs,varName, boost::is_any_of("_"));
        
        if( RegistredVectorsOfChildren.find(strs[0])!=
           RegistredVectorsOfChildren.end()){
            
            LOGGER_WRITE(msLogger::DEBUG, "child included in a vector");
            Children[varName] = new msChild<msTreeMapper>(ptr);
            RegistredVectorsOfChildren[strs[0]]->set(atoi(strs[1].c_str()) , ptr);
        }else{
            
            //update(Children[varName],ptr);
            //Children[varName]->getBasePtr()->setId("update");
            Children[varName]->set(ptr);
            
        }
        
        LOGGER_EXIT_FUNCTION2("void msTreeMapper::replaceChild( boost::shared_ptr<msTreeMapper> ptr, std::string varName)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    /*
     msChild<msTreeMapper> msTreeMapper::getSymLink(msDbNodeReader* node)  {
     
     LOGGER_ENTER_FUNCTION("msChild<msTreeMapper> msTreeMapper::getSymLink(msDbNodeReader* node) ",getFullId());
     std::string path;
     if( node->queryStringAttribute( "Path" , path) != msDbNodeReader::SUCESS )
     LOGGER_WRITE(msLogger::ERROR, "a symbolic link node need a 'Path' attribut to find his path");
     
     std::vector<std::string> strs;
     boost::split(strs,path, boost::is_any_of("/"));
     std::vector<std::string>::iterator it;
     msChild<msTreeMapper> link;
     msTreeMapper* scan=this;
     
     for(it=strs.begin() ; it!=strs.end() ; it++) {
     if( *it =="..") scan=scan->Parent;
     else{
     if( scan->Children.find(*it) !=scan->Children.end()  )
     scan=scan->Children[*it].get();
     else
     LOGGER_WRITE(msLogger::ERROR, "problem when parsing the symbolic link "+path);
     
     }
     }
     return scan->Parent->Children[strs.back()];
     }*/
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msTreeMapper::deepCopy(){
        
        boost::shared_ptr<msTreeMapper> ptr = mySharedPtr();
        try{ ptr = clone();
        }
        catch(msException& e)
        {
            e.addContext("The deep copy has failed (boost::shared_ptr<msTreeMapper> msTreeMapper::deepCopy(boost::shared_ptr<msTreeMapper> original))");
            throw e;
        }
        return ptr;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    msTreeMapper& msTreeMapper::operator=(const msTreeMapper& rhs)
    {
        boost::shared_ptr<msTreeMapper> scan;
        
        if( !rhs.isDerivedFrom( getType() ) )
            IMPACT_THROW_EXCEPTION(msException("The copy operator work only for 'rhs' derived from this type",
                                          "msTreeMapper& msTreeMapper::operator=(const msTreeMapper& rhs)",getFullId()) );
        VariableAttributs = rhs.VariableAttributs;
        
        std::map< std::string , boost::shared_ptr<msAttributeBase> > ::const_iterator itatt;
        
        for(itatt = Attributes.begin(); itatt!=Attributes.end(); itatt++){
            
            if( rhs.Attributes.find(itatt->first) == rhs.Attributes.end() )
                
                IMPACT_THROW_EXCEPTION(msException("Can not find the corresponding attribut ("+itatt->first+") in rhs attributes list",
                                              "msTreeMapper& msTreeMapper::operator=(const msTreeMapper& rhs)",getFullId()) );
            
            try{ Attributes[itatt->first]->set(rhs.Attributes.find(itatt->first)->second->get() );
            }
            catch(msException& e)
            {
                e.addContext("The attribute "+itatt->first+" has not been duplicated from rhs."+itatt->first+" (="+rhs.Attributes.find(itatt->first)->second->get()
                             +") (msTreeMapper& msTreeMapper::operator=(const msTreeMapper& rhs))");
                throw e;
            }
        }
        
        std::map< std::string, msChildBase* >::const_iterator it;
        /*
         for(it=rhs.Children.begin();it!=rhs.Children.end();it++)
         {
         if ( Children.find(it->first) != Children.end() ) {
         
         Children.find(it->first)->second->operator=( *(it->second) );
         }
         if ( Children.find(it->first) == Children.end() ) {
         
         Children[it->first] = msChild<msTreeMapper>( it->second->clone() ); //operator=( *it->second );
         Children[it->first]->setAffiliation(mySharedPtr());
         }
         }
         */
        update();
        return *this;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msTreeMapper::constructVar(std::string type,std::string var,std::string id)
    {
        CppType+=type+":";
        VariableAttributs=VarDatabase(var,type,id);
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msTreeMapper::setAttribute(const std::string& id,const std::string& value)
    {
        LOGGER_ENTER_FUNCTION("boost::shared_ptr<msTreeMapper> msTreeMapper::setAttribute(const std::string& id,const std::string& value)"
                              ,getFullId());
        
        if (Attributes.find(id) == Attributes.end()) {
            IMPACT_THROW_EXCEPTION( msException("Attribut of id='"+id+"' has not been declared"
                                           ,"boost::shared_ptr<msTreeMapper> msTreeMapper::setAttributes(const std::string& id,const std::string& value)",  getFullId()) );
            
        }
        try{Attributes[id]->set(value);}
        catch(msException& e){
            
            e.addContext("Can not parse the string to the correct type (boost::shared_ptr<msTreeMapper> msTreeMapper::setAttribute(const std::string& id,const std::string& value))");
        }
        LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msTreeMapper::setAttribute(const std::string& id,const std::string& value)");
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    std::vector< std::string> msTreeMapper::getAttributes() const {
        
        std::vector< std::string> list;
        std::map< std::string , boost::shared_ptr<msAttributeBase> > ::const_iterator itatt;
        for(itatt=Attributes.begin(); itatt!=Attributes.end(); itatt++)
            list.push_back(itatt->first+" = "+itatt->second->get());
        
        return list;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::string msTreeMapper::getAttributeInString(std::string id )  {
        
        std::stringstream out;
        out<<Attributes[id]->get();
        return out.str();
    }
    
    boost::shared_ptr<msTreeMapper> msTreeMapper::getChild(const std::string& id) const {
        
        boost::shared_ptr<msTreeMapper> ptr;
        const_childIterator it = Children.begin();
        
        for( ; it != Children.end() ; ++it )
            
            if( ! getVariableName((*it).second->getSharedBasePtr()).compare( id ) ) return (*it).second->getSharedBasePtr();
        
        IMPACT_THROW_EXCEPTION( msException("Child of id='"+id+"' can not be found in the first layer of the tree"
                                       ,"boost::shared_ptr<msTreeMapper> msTreeMapper::getChild(std::string id)",  getFullId()) );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::vector< boost::shared_ptr<msTreeMapper> > msTreeMapper::getChildren() const {
        
        const_childIterator it = Children.begin();
        std::vector< boost::shared_ptr<msTreeMapper> > ChildrenForIteration;
        
        for( ; it != Children.end() ; ++it ){
            
            ChildrenForIteration.push_back((*it).second->getSharedBasePtr());
        }
        return ChildrenForIteration;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    /*
     std::vector< boost::shared_ptr<msTreeMapper> > msTreeMapper::getChildrenDerivedFrom(std::string type) const {
     
     boost::shared_ptr<msTreeMapper> scan;
     std::vector< boost::shared_ptr<msTreeMapper> > ChildrenForIteration;
     
     std::map< std::string, msChildBase* >::const_iterator it;
     for( it=Children.begin(); it!=Children.end(); it++)
     {
     if(it->second->getBasePtr()->isDerivedFrom(type)){
     
     ChildrenForIteration.push_back(it->second->getSharedBasePtr());
     }
     else {
     std::vector< boost::shared_ptr<msTreeMapper> > childs = it->second->getBasePtr()->getChildrenDerivedFrom(type);
     ChildrenForIteration.insert(ChildrenForIteration.end(),childs.begin(),childs.end());
     }
     }
     return ChildrenForIteration;
     }
     
     //-------------------------------------------------------------------------------
     //-------------------------------------------------------------------------------
     
     
     msTreeMapper::childIterator msTreeMapper::firstChildOfType(std::string type)
     {
     childIterator it;
     for(it=Children.begin();it!=Children.end();++it)
     if( it->second.get()->isDerivedFrom(type) )
     return it;
     return Children.end();
     }
     
     //-------------------------------------------------------------------------------
     //-------------------------------------------------------------------------------
     
     msTreeMapper::childIterator msTreeMapper::nextChildOfSameType(childIterator it)
     {
     for(++it;it!=Children.end();++it)
     if( it->second.get()->isDerivedFrom(it->second->getType()) )
     return it;
     return Children.end();
     }
     */
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& msTreeMapper::print(std::ostream& out)  const
    {
        out<<"\n"<<output::printSepLine()<<output::printSepLine()
        <<output::printGreen( "Object : "+getId()+" of type "+getType() )<<"\n"
        <<output::printSepLine()<<"\n";
        out<<output::printSepLine()<<output::printYellow("\tDatabase mapper\n")<<output::printSepLine()<<std::endl;
        size_t VisitorLevelInTree = 0;
        printTree(out,VisitorLevelInTree,"root");
        return out;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& msTreeMapper::printTree(std::ostream& out, size_t& VisitorLevelInTree, std::string varName)  const {
        boost::shared_ptr<msTreeMapper> scan;
        
        for(size_t i=0;i<VisitorLevelInTree;++i) out<<"    ";
        
        out<<VariableAttributs.Type<<"-"<<varName<<" [ ";
        
        std::map< std::string , boost::shared_ptr<msAttributeBase> > ::const_iterator itatt;
        for(itatt=Attributes.begin(); itatt!=Attributes.end(); itatt++)
            out<<" "<<itatt->first<<"=\""<<itatt->second->get()<<"\"";
        
        out<<" ] "/*object_pointer_adress:"<<static_cast<void*>(this)*/<<std::endl;
        
        std::map< std::string, msChildBase* >::const_iterator it;
        
        for( it=Children.begin(); it!=Children.end(); it++)
            
            if( ( !( scan=it->second->getBasePtr()->Parent.lock() )) || ( scan != shared_from_this() ) ) {
                
                for(size_t i=0;i<VisitorLevelInTree;i++)  out<<"    ";
                out<<"    "
                <<it->second->getBasePtr()->VariableAttributs.Type
                <<"-"<<it->first//it->second.get()->VariableAttributs.Name
                <<"\tSymLink="<<output::printYellow(getSymbolicLinkPath( it->second->getSharedBasePtr() ))<<std::endl;
            }
            else{
                VisitorLevelInTree++;
                it->second->getBasePtr()->printTree(out,VisitorLevelInTree,getVariableName(it->second->getSharedBasePtr()));
            }
        
        VisitorLevelInTree--;
        return out;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::string msTreeMapper::getSymbolicLinkPath(boost::shared_ptr<msTreeMapper> scan )const  {
        
        
        std::stringstream listSegment1; scan->writeAffiliationVariable( listSegment1);
        
        std::vector<std::string> strs1; std::string b=listSegment1.str();
        boost::split(strs1,b, boost::is_any_of("-"));
        std::stringstream path;
        for( size_t j=0; j<strs1.size() ; j++) path<<strs1[j]<<"/";
        
        return path.str();
    }
    
    
} //end namespace impact

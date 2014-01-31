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


#ifndef MSTREE_H
#define MSTREE_H

#include <msRegister.h>
#include <msLogger.h>
#include <msAttribute_Child.h>


namespace impact
{
    
    /*! \class msTreeMapper
     *
     *  \brief Interface to map the objects structure into a tree database
     *
     * \section presentation Presentation
     *
     * The primary purpose of msTreeMapper is to manage and organize data using
     * a tree representation. The tree representation is usually very close to the
     * class structure itself.
     * Loaders/savers can then be used to save/load them to/from a particular
     * format (see msXmlLoader/msXmlSaver, used to 'open' and 'save' projects in the GUI).
     * msTreeMapper always ensures the synchronisation between objects
     * and database structures.
     *
     * To derive a new object from msTreeMapper you need to declare all the Degrees Of Freedom (DoF)
     * of your objects (i.e. all the members defining completely the object's state).
     * There are three types of degrees of freedom you can use:
     *      - an attribute (msAttribute): this correspond to simple structure (int, double,
     * float, vector, map ... ). To declare one type as an attribute, a lexical cast of
     * this type needs to exist; see msAttribute.
     *      - a child (msChild) : this corresponds to members derived
     * from msTreeMapper
     *      - a vector of child (msChildren) : this correspond to a
     * vector that contains derived object from a particular msTreeMapper derived type.
     *
     * \section registration Registration of the degrees of freedom
     *
     * The registration of the childs and attributes is realized by overriding the
     * function **void initialize()**. In case other members of your class depends
     * on the DoFs, you need to override the function **void update()**:
     * its purpose is to update the non DoF members from the DoF members.
     * Also, the constructors of the derived class have to call the
     * constructVar method to declare the class. \n
     * The next example shows how it would work for a class containing as DoF:
     *    - a list of integer,
     *    - a msSampler
     *    - a vector of msScalarFunction
     
     ~~~~~~~~~~~~~{.cpp}
     #include <msScalarFunction.h>
     #include <msSampler.h>
     
     myClass : public msTreeMapper
     {
     public:
     
     // //--------- bellow are mandatory methods for msTreeMapper
     
     myClass() { constructVar("myClass","default_variable_name","default_id");
     }
     
     void  initialize() {
     
     msTreeMapper::initialize()
     declareAttribute(                   ListOfIntegers,                   "ListOfIntegers");
     declareChild<msSampler>(            TheSampler,     msSampler::New(), "TheSampler" );
     declareChildren<msScalarFunction>(  TheVectorOfScalarFct,             "TheVectorOfScalarFct");
     }
     
     void  update() {
     
     msTreeMapper::update()
     if( ListOfIntegers.size() > 0 ) FirstElementOfListOfIntegers = ListOfIntegers[0];
     else                            FirstElementOfListOfIntegers = -1;
     }
     
     // //--------- you should always define them, it is always the same
     
     static boost::shared_ptr<myClass> New() {
     
     boost::shared_ptr<myClass> T( new myClass );
     T->initialize();
     T->update();
     return T;
     }
     
     virtual boost::shared_ptr<msTreeMapper> clone() {
     
     boost::shared_ptr<msTreeMapper> clone = New();
     clone->msTreeMapper::operator=(*this);
     return clone;
     }
     
     // //--------- the myClass stuffs
     
     void addInteger(int a){
     
	 ListOfIntegers.push_back(a);
     }
     
     void setSampler(boost::shared_ptr<msSampler> newSampler) {
     
     msTreeMapper::update(TheSampler,newSampler)
     }
     
     void addFunctionToSample(boost::shared_ptr<msSampler> newFct){
     
     msTreeMapper::addElementToChildren(TheVectorOfScalarFct,newFct);
     }
     
     private:
     
     // //--------- Degrees of freedom
     
     vector<int> 				ListOfIntegers;
     msChild<msSampler>			TheSampler;            //  can be any type deriving from msSampler
     msChildren<msScalarFunction>	  	TheVectorOfScalarFct;  //  can store any type deriving from msScalarFunction
     
     // //--------- this is not a DoF, but say we need it for some purpose -> initialize it in the update function
     
     int FirstElementOfListOfIntegers;
     
     }
     ~~~~~~~~~~~~~
     *
     * **Some important comments:**
     *    - the instance of *myClass* is the parent of **ListOfIntegers**, **TheSampler**
     * and **TheVectorOfScalarFct**
     *    - the idea is to minize the construction tasks in the default constructor,
     * complex initialization should go in the *initialize* method
     *    - **initialize** and **update** methods should always call the corresponding
     * parent class methods
     *    - the static **New** function, and the **clone** are not mandatory at this point, however
     * for consistency and to avoid problem they should be added
     *    - child and children are mapped using the templated msChild and msChildren class respectively
     *    - msChild and msChildren work only with shared_ptr (this ensure a proper
     * managment of the memory )
     *    - the string argument providen in the *declare* family of methods has to be unique, you should
     * use the same name as the C++  variable to avoid confusion
     * /n
     * The database structure can be displayed when calling the print function of
     * msTreeMapper. Let's construct an object:
     *
     ~~~~~~~~~~~~~{.cpp}
     boost::shared_ptr<myClass> myobject = myClass::New();
     
     myobject->setSampler(msLinearSampler::New());
     
     myobject->addFunctionToSample(msParser::New());
     
     myobject->addInteger(5);
     
     myobject->print(cout);
     ~~~~~~~~~~~~~
     *
     * The output will be:
     ~~~~~~~~~~~~~{.xml}
     myClass-default_variable_name Id="default_id" ListOfIntegers="5"
     msLinearSampler-TheSampler       # here is the msLinearSampler description of attributes and childs #
     msParser-TheVectorOfScalarFct_0  # here is the msParser description of attributes and childs #
     ~~~~~~~~~~~~~
     
     
     
     
     * \section update How to use the degrees of freedom
     *
     * \subsection attr Attribute
     *
     * For the attribute type of DoF  (impact::msAttribute), there is no restriction: just use
     * it how you want to. \n
     *
     * \subsection child Child
     *
     * The child type of DoF (impact::msChild) is manipulated as a pointer on the templated type.
     * The only difference with usual pointers is when you want to change the adress where the child
     * is pointing to.
     * In this case your are not able to use the assignement operator (this will break at the
     * compile time), but you can do it using the function msTreeMapper::updateChild.
     *
     * \subsection children Children
     *
     * Similarly, *children* type of DoF (impact::msChildren) can be read as usual
     * pointer on vector, but modification on the elements can be done only using the functions
     * providen by msTreeMapper (ie. msTreeMapper::addElementToChildren, msTreeMapper::clearChildren ...).
     */
    class msTreeMapper : public msRegister , public boost::enable_shared_from_this<msTreeMapper>
    {
        
    public:
        
        //! @Name From msRegister
        //@{
        static msRegistrar Registrar;
        static msRegister*  create() {return new msTreeMapper;}
        static bool isTreeMapperRegistredInPython;
        virtual void registryInPython();
        //@}
        
        struct VarDatabase
        {
	    friend class msTreeMapper;
            
            VarDatabase(){};
            VarDatabase(std::string name, std::string type,std::string id) {
	      
                Name=name;Type=type;Id=id;
            }
	    std::string Name; 
	    std::string Id; 
	    std::string Comments;
	    
        private:
	  
            std::string Type;
        };
        
        typedef std::map<std::string , msChildBase* >::iterator childIterator;
        typedef std::map<std::string , msChildBase* >::const_iterator const_childIterator;
        
        typedef std::map<std::string , boost::shared_ptr<msAttributeBase> >::iterator attributeIterator;
        typedef std::map<std::string , boost::shared_ptr<msAttributeBase> >::const_iterator const_attributeIterator;
        
        static boost::shared_ptr<msTreeMapper> New() {
            
            boost::shared_ptr<msTreeMapper> T( new msTreeMapper );
            T->initialize();
            T->mySharedPtr()=T;
            return T;
        }
        
        //! return a deep copy of the structure
        virtual boost::shared_ptr<msTreeMapper> clone() {
            
            boost::shared_ptr<msTreeMapper> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        //! update the class members from the  attributes & childs of the tree structure. see @ref registration
        virtual void update() { IMPACT_LOGIN();IMPACT_LOGOUT(); }
        
    public:
        
        //! declare the attributes & childs of the tree structure. see @ref registration
        virtual void initialize() {
            
            IMPACT_LOGIN();
            declareAttribute(VariableAttributs.Id,"Id");
	    declareAttribute(VariableAttributs.Comments, "Comments");
	    IMPACT_LOGOUT();
        };
        
        msTreeMapper();
        
        msTreeMapper& operator=(const msTreeMapper& rhs);   //!< deep copy assignement
        
        boost::shared_ptr<msTreeMapper> deepCopy();  	//!< return a deep copy of the object
        
        //! @Name iterators of children / attributes
        //@{ 
	attributeIterator beginAttributes(){return Attributes.begin();};
        attributeIterator endAttributes(){return Attributes.end();};
        
        const_attributeIterator beginAttributes() const {return Attributes.begin();};
        const_attributeIterator endAttributes()const {return Attributes.end();};
        
        childIterator beginChildren(){return Children.begin();};
        childIterator endChildren(){return Children.end();};
        
        const_childIterator beginChildren() const {return Children.begin();};
        const_childIterator endChildren()const {return Children.end();};
	//@}
	
        //! @Name Function to operate on the children / attributes
        //@{
        //! \brief return the string cast of the attribute of 'id'
        std::string getAttributeInString(std::string id )  ;
              
        /** \brief return the first child matching an id
	 * 
	 * \param id target id
	 */
        boost::shared_ptr<msTreeMapper> getChild(const std::string& id) const;
        
        //! return all the children
        std::vector< boost::shared_ptr<msTreeMapper> > getChildren() const;
        
        /** \brief return all the children derived from a particular type, 
	 * 
	 * A recursive scheme is used to find the items
	 * \param type type wanted
	 */
        template<class T>
        std::vector< boost::shared_ptr<T> > getChildrenDerivedFrom(std::string type) const;
        
        /** \brief add a child 'child' to the tree structure 
	 * 
	 * \param child object to add to the object
	 * \param varName variable name to use
	 */
        void addChild( boost::shared_ptr<msTreeMapper> child, std::string varName);
                
        //! \brief return the attributes in a list. Each element is of type 'key : value'
        std::vector< std::string> getAttributes() const ;
        
        /** \brief set an attribute
	 * 
         * The string representation of the attribute w/ to their type are given in the file
         * msAttribute_Child.h
         * \param id key id
         * \param value string representation of the value
         */
        boost::shared_ptr<msTreeMapper> setAttribute(const std::string& id,const std::string& value);
        
        //@}
        
        
        //! \name  Type, id and affiliation
        //@{
        /** cast the object
         */
        template<class T>
        boost::shared_ptr<T> impact_static_cast() { return boost::static_pointer_cast<T>(mySharedPtr());
        }
        
        //! \brief return full type
        std::string getType() const; 		
        
        //! \brief return the last type
        std::string getLastType() const; 	
        
        //! \brief return the Id
        std::string getId()   const;		
        
        //! \brief return he comments
        std::string getComments()   const;	
        
        //! \brief return Affiliation+Id
        std::string getFullId()   const;	
        
        //! \brief return a vector of the object's variable name on the path to the root element.
        std::vector<std::string> getVariableNames() const;
        
        //! \brief  return the variable name of a particular child
        std::string getVariableName( const boost::shared_ptr<const msTreeMapper> child ) const;
        
        //! \brief  return the variable name of a particular child
        std::string getVariableName( boost::shared_ptr<msTreeMapper> child ) const;
        
	//! \brief  return variable name chain from root
        std::string getFullVariableName()   const;	
        
	//! \brief  return the parent
        boost::shared_ptr<msTreeMapper> getParent() const {
            
	    IMPACT_LOGIN();
            boost::shared_ptr<msTreeMapper> ptr;
	    IMPACT_EXCEPT_IF( [&] (){ return !(ptr=Parent.lock()); },
		       "Can not lock the Parent weak pointer");
	   
	    IMPACT_LOGOUT();
	    return ptr;
        }
        
        /** \brief set the parent (owner) object 
	 * 
         * \param parent parent object
         */
         void setAffiliation(boost::shared_ptr<msTreeMapper> parent){
	  
	    IMPACT_LOGIN()
            Parent.reset();
            boost::shared_ptr<msTreeMapper> ptr;
	   /* IMPACT_EXCEPT_IF( [&] () { return !(ptr=Parent.lock()); },
		       "Can not lock the Parent weak pointer");*/
	    Parent = boost::weak_ptr<msTreeMapper>(parent->mySharedPtr());
	    
	    IMPACT_LOGOUT()
        }
        
        /** \brief  set the id of the object 
	 * 
	 * \param id identification string
	 */ 
        boost::shared_ptr<msTreeMapper> setId(std::string id);			    

        /** \brief  set the comments of the object 
	 * 
	 * \param comment the comment
	 */ 
	boost::shared_ptr<msTreeMapper> setComments(std::string comment);		   
		
       
        
        //! \brief return 1 if the object is derived from the class 'target'
        bool isDerivedFrom(std::string target) const;
        		     
        //! \brief return 1 if the object has a parent
        bool hasParent() const { boost::shared_ptr<msTreeMapper> ptr;
            if( (ptr=Parent.lock()) ) return 1;
            return 0;
        }
        
        bool hasChildOfName(const string name ) { return Children.find(name)!=Children.end();
        }
        bool isAffiliatedTo(boost::shared_ptr<msTreeMapper> parent) { boost::shared_ptr<msTreeMapper> p = Parent.lock();
            return p == parent;
        }
        //@}
        
        bool isToBeSaved() { return toBeSaved;
        }
        
        boost::shared_ptr<msTreeMapper> setToBeSaved(bool ok) {
            
            toBeSaved = ok;
            return mySharedPtr();
        }
        
        ~msTreeMapper(){};
        
        
        virtual std::ostream& print(std::ostream&) const;   //!< print the object
        
        boost::shared_ptr<msTreeMapper> mySharedPtr() {
            
            boost::shared_ptr<msTreeMapper> me;
            try{ me = shared_from_this();
	        }
            catch(...){
                msException e("Can not get the shared pointer of the object, did you create it using the ::New operator?",
                          "boost::shared_ptr<msTreeMapper> msTreeMapper::mySharedPtr()",
                          getFullId());
                IMPACT_THROW_EXCEPTION(e);
            }
            return  me;
        };
  
	boost::shared_ptr<const msTreeMapper> myConstSharedPtr() const {
            
            boost::shared_ptr<const msTreeMapper> me;
            try{ me = shared_from_this();
	        }
            catch(...){
                msException e("Can not get the shared pointer of the object, did you create it using the ::New operator?",
                              "boost::shared_ptr<const msTreeMapper> msTreeMapper::mySharedPtr()",
                               getFullId());
                IMPACT_THROW_EXCEPTION(e);
            }
            return me;
        };
	
	/** \brief check if the object is correctly initialized/working
	 * 
	 * Exception is raised for each problem encountered
	 */
        virtual bool sanityCheck(){return 1;};
        
    protected:
        
        VarDatabase VariableAttributs;     	//!< Attributs of the variable: name, type, id
        
        //! Set the variable attributes, should be called from the constructors of the derived class
        void constructVar(std::string type,std::string var,std::string id);
        
        //boost::shared_ptr<msTreeMapper> getSharedPtr(){ return boost::shared_from_this(); };
        
        /** Finalize the declaration of a msTreeMapper derived class for python
         * (make accessible the child and children type of the template class, and
         * the msTreeMapper::declareChild and msTreeMapper::declareChildren method).
         */
        template<class T>
        void finalizeDeclarationForPython(string name);
        
        
        //! \name Functions to declare the Attribut/Objects(childs) in the database
        //@{
        
        
        /** declare an attribute, this version is meant to be use from Python, using c++
         * you should prefer the one using reference.
         * \param attribute attribute
         * \param key  variable name
         */
        void declareAttribute( boost::shared_ptr<msAttributeBase> attribute , std::string key ) { Attributes[key] = attribute;}
        
        /** declare an attribute.
         * \param elem attribute
         * \param key  variable name
         */
        template<class T>
        void declareAttribute( T& elem , std::string key );
        
        /** declare a Child.
         * \param refchild reference on the child
         * \param name  variable name you want for the child, should be the same as its name in the c++ object
         */
        template<class T>
        void declareChild(msChild<T>& refchild, boost::shared_ptr<T> value, std::string name);
        
        /** declare a Child.
         *
         * \param refchild reference on the child
         * \param name  variable name you want for the child, should be the same as its name in the c++ object
         */
        void declareChild(boost::shared_ptr<msChildBase> refchild, boost::shared_ptr<msTreeMapper> value, std::string name);
        
        
        /** declare a Children.
         * \param refchild reference on the children
         * \param name  variable name you want for the children, should be the same as its name in the c++ object
         */
        template<class T>
        void declareChildren( msChildren<T>& refvec,std::string name);
        
        /** \copydoc void declareChildren( msChildren<T>& refvec,std::string name)
         */
        void declareChildren( boost::shared_ptr<msChildrenBase> refvec,std::string name);
        //@}
        //! \name Functions to update the childs declared in the database from new objects
        //@{
        
        /** \brief update the adress pointed by a child declared to a new one
         *
         * \param lhschild reference on the child previsously declared
         * \param rhschild the adress of the new child
         */
        template<class T>
        void update(msChild<T>& lhschild,boost::shared_ptr<T> rhschild);
        
        
        /**  \copydoc void update(msChild<T>& vector,boost::shared_ptr<T> elem)
         */
        void updateChild(boost::shared_ptr<msChildBase> lhschild,boost::shared_ptr<msTreeMapper> rhschild);
        
        
        /** \brief add a child to a children vector, behave like std::vector::add
         *
         * \param vector reference on the children vector you want to complete
         * \param elem the adress of the new child to add
         */
        template<class T>
        void addElementToChildren(msChildren<T>& vector,boost::shared_ptr<T> elem);
        
        /** \brief reset an element of the Children
         *
         * \param vector reference on the children vector you want to modify
         * \param i index of the element you want to replace
         * \param elem the adress of the new child to add
         */
        template<class T>
        void replaceElementOfChildren(msChildren<T>& vector,size_t i,boost::shared_ptr<T> elem);
        
        
        /** \copydoc void addElementToChildren(msChildren<T>& vector,boost::shared_ptr<T> elem)
         */
        void addElementToChildren(boost::shared_ptr<msChildrenBase> vector,boost::shared_ptr<msTreeMapper> elem);
        
        /** clear a children vector, behave like std::vector::clear
         * \param vector reference on the children vector you want to complete
         */
        template<class T>
        void clearChildren(msChildren<T>& vector);
        
        /** resize a children vector, behave like std::vector::resize
         * \param vector reference on the children vector you want to complete
         * \param n the new size
         */
        template<class T>
        void resizeChildren(msChildren<T>& vector,size_t n){vector.resize(n);}
        //@}
        
        
        
        void writeAffiliation(std::ostream& out) const;
        
        void writeAffiliationVariable(std::ostream& out) const;
        
    private:
        
        bool toBeSaved;
        	
        boost::weak_ptr<msTreeMapper>  Parent;
        
        std::map< std::string , boost::shared_ptr<msAttributeBase> >	Attributes;
        
        std::map< std::string , msChildBase*  >             Children;
        std::map< std::string , msChildrenBase * >          RegistredVectorsOfChildren; //!< short cut for 'Vector values' child acess
        
        std::string getSymbolicLinkPath(boost::shared_ptr<msTreeMapper> scan ) const;
        
        std::ostream& printTree(std::ostream&,size_t& level,std::string varName) const;
        
        std::string CppType;		    	//!< full cpp type: Base:Derived1:Derived2 ...
        
        size_t getNextIndiceInChildren(std::string vecname); //!< indice of the next element of a container of id 'vecname'
        
    };
    
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T>
    std::vector< boost::shared_ptr<T> > msTreeMapper::getChildrenDerivedFrom(std::string type) const {
        
        boost::shared_ptr<msTreeMapper> scan;
        std::vector< boost::shared_ptr<T> > ChildrenForIteration;
        
        std::map< std::string, msChildBase* >::const_iterator it;
        for( it=Children.begin(); it!=Children.end(); it++)
        {
            if(it->second->getBasePtr()->isDerivedFrom(type)){
                
                ChildrenForIteration.push_back(boost::static_pointer_cast<T>(it->second->getSharedBasePtr()));
            }
            else {
                std::vector< boost::shared_ptr<T> > childs = it->second->getBasePtr()->getChildrenDerivedFrom<T>(type);
                ChildrenForIteration.insert(ChildrenForIteration.end(),childs.begin(),childs.end());
            }
        }
        return ChildrenForIteration;
    }

    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T>
    void msTreeMapper::declareAttribute( T& elem , std::string key ) {
        
        Attributes[key] = boost::shared_ptr<  msAttribute<T> >( new msAttribute<T> (elem) );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T>
    void msTreeMapper::declareChild(msChild<T>& refchild, boost::shared_ptr<T> value, std::string name) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msTreeMapper::declareChild(msChild<T>& refchild, T* value, std::string name)",getFullId());
        LOGGER_WRITE(msLogger::DEBUG,"Declare a child of name '" + name +"' and type '"+getType()+"'");
        
        if( !value->isDerivedFrom("msTreeMapper") )
            
            IMPACT_THROW_EXCEPTION( msException("The child you want to register is not derived from msTreeMapper, only msTreeMapper derived object can be declared as child",
                                           "void msTreeMapper::declareChild(msChild<T>& refchild, boost::shared_ptr<T> value, std::string name)",
                                           getFullId()
                                           ));
        
        refchild.set( value );
        value->VariableAttributs.Name = name;
        try{
            mySharedPtr()    = shared_from_this();
            refchild->Parent = mySharedPtr();
            Children[name]   = &refchild;
            //RegistredChildren[name]   = refchild.get();
            std::stringstream out;
            out<<"child passed: "<<static_cast<void*>(&refchild)<<" adress in children: "<<static_cast<void*>(&Children[name])
            <<" adress of the object:"<<refchild.get();
            LOGGER_WRITE(msLogger::DEBUG,out.str());
        }
        catch( boost::exception &e )
        {
            Children[name]   = &refchild;
            //RegistredChildren[name]= refchild.get();
            LOGGER_EXIT_FUNCTION2("void msTreeMapper::declareChild(msChild<T>& refchild, T* value, std::string name)");
            return;
        }
        
        LOGGER_EXIT_FUNCTION2("void msTreeMapper::declareChild(msChild<T>& refchild, T* value, std::string name)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T>
    void msTreeMapper::declareChildren( msChildren<T>& refvec,std::string name) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msTreeMapper::declareChildren( msChildren<T>& refvec,std::string name)",getFullId());
        LOGGER_WRITE(msLogger::DEBUG,"Name of the children: "+name);
        typedef typename msChildren<T>::iterator Iterator;
        Iterator it = refvec.Elements.begin();
        RegistredVectorsOfChildren[name]=&refvec;
        LOGGER_EXIT_FUNCTION2("void msTreeMapper::declareChildren( msChildren<T>& refvec,std::string name)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T>
    void msTreeMapper::update(msChild<T>& lhschild,boost::shared_ptr<T> rhschild) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msTreeMapper::update(msChild<T>& lhschild,boost::shared_ptr<T> rhschild)",getFullId());
        LOGGER_WRITE(msLogger::DEBUG,"Id of the updated child:"+lhschild->getFullId());
        
        if( lhschild.getSharedPtr() == boost::shared_ptr<T>() )
            IMPACT_THROW_EXCEPTION( msException("the original (lhschild) child is not initialized, did you declare it?",
                                           "void msTreeMapper::update(msChild<T>& lhschild,boost::shared_ptr<T> rhschild)",
                                           getFullId()
                                           ));
        
        childIterator it = Children.begin();
        std::string varname="";
        
        for(;it!= Children.end();++it)
            if( it->second->getBasePtr() == lhschild.getBasePtr() )  varname = it->first;
        
        if( varname=="")
            IMPACT_THROW_EXCEPTION( msException("ref. to lhschild no found",
                                           "void msTreeMapper::update(msChild<T>& lhschild,boost::shared_ptr<T> rhschild)",
                                           getFullId()
                                           ));
        
        //lhschild.operator=(rhschild);
        if(!rhschild->hasParent()) rhschild->setAffiliation(mySharedPtr());
        
        lhschild.set(rhschild);
        rhschild->VariableAttributs.Name = varname;
        Children[varname]->set( boost::static_pointer_cast<msTreeMapper>(rhschild) );
        
        LOGGER_EXIT_FUNCTION2("void msTreeMapper::update(msChild<T>& lhschild,boost::shared_ptr<T> rhschild)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T>
    void msTreeMapper::addElementToChildren(msChildren<T>& vector,boost::shared_ptr<T> elem) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msTreeMapper::addElementToChildren(msChildren<T>& vector,boost::shared_ptr<T> elem)",getFullId());
        
	if(!elem->hasParent()) elem->setAffiliation(mySharedPtr());
	
        std::map< std::string , msChildrenBase * >::iterator  it;
        it=RegistredVectorsOfChildren.begin();
        std::string varname="";
        for(;it!= RegistredVectorsOfChildren.end();++it)
            if( it->second == &vector )  varname = it->first;
        
        LOGGER_WRITE(msLogger::DEBUG,"elem's id: "+elem->getId()+"; vector name: "+varname);
        
        if( varname=="")
            IMPACT_THROW_EXCEPTION( msException("reference to the children not found, did you declare it in the constructor ?"
                                           ,"msTreeMapper::addElementToChildren(msChildren<T>& vector,boost::shared_ptr<T> elem)"
                                           ,getFullId() ) );
        
        vector.push_back(elem);
        std::stringstream name;name<<varname<<"_"<<getNextIndiceInChildren(varname);
        elem->VariableAttributs.Name=name.str();
        LOGGER_WRITE(msLogger::DEBUG,"child's variable name: "+name.str());
        
        Children[name.str()] = new msChild<T>();
        Children[name.str()]->set(elem);
        
        LOGGER_EXIT_FUNCTION2("void msTreeMapper::addElementToChildren(msChildren<T>& vector,boost::shared_ptr<T> elem)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T>
    void msTreeMapper::replaceElementOfChildren(msChildren<T>& vector,size_t i,boost::shared_ptr<T> elem) {
        
        LOGGER_ENTER_FUNCTION_DBG("void replaceElementOfChildren(msChildren<T>& vector,size_t i,boost::shared_ptr<T> elem)",getFullId());
        
        std::map< std::string , msChildrenBase * >::iterator  it;
        it=RegistredVectorsOfChildren.begin();
        std::string varname="";
        for(;it!= RegistredVectorsOfChildren.end();++it)
            if( it->second == &vector )  varname = it->first;
        
        LOGGER_WRITE(msLogger::DEBUG,"elem's id: "+elem->getId()+"; vector name: "+varname);
        
        if( varname=="")
            IMPACT_THROW_EXCEPTION( msException("reference to the children not found, did you declare it in the constructor ?"
                                           ,"void replaceElementOfChildren(msChildren<T>& vector,size_t i,boost::shared_ptr<T> elem)"
                                           ,getFullId() ) );
        
        vector.set(i,elem);
        LOGGER_EXIT_FUNCTION2("void replaceElementOfChildren(msChildren<T>& vector,size_t i,boost::shared_ptr<T> elem)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T>
    void msTreeMapper::clearChildren(msChildren<T>& vector) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msTreeMapper::clearChildren(msChildren<T>& vector)",getFullId());
        
        std::map< std::string , msChildrenBase * >::iterator  it;
        it=RegistredVectorsOfChildren.begin();
        std::string varname="";
        std::vector<std::string> strs;
        
        for(;it!= RegistredVectorsOfChildren.end();++it)
            if( it->second == &vector )  varname = it->first;
        
        if( varname=="")
            IMPACT_THROW_EXCEPTION( msException("reference to the children not found, did you declare it?"
                                           ,"void msTreeMapper::clearChildren(msChildren<T>& vector)"
                                           ,getFullId() ) );
        
        LOGGER_WRITE(msLogger::DEBUG,"Vector found, variable name of the vector: " + varname + "; size: "+
                     output::getString<int>(vector.size()) );
        size_t n0 = vector.size();
        vector.clear();
        LOGGER_WRITE(msLogger::DEBUG,"Vector cleared, number of element in vector: " + output::getString<int>(vector.size()) );
        LOGGER_WRITE(msLogger::DEBUG,"Erase the "+output::getString<int>(n0) +" references of vector's elements in the 'Children' list");
        
        for( size_t i=0; i< n0 ; i++) {
            std::stringstream name;
            name<<varname<<"_"<<i;
            LOGGER_WRITE(msLogger::DEBUG,"erase element " + name.str() );
            
            if(Children.find(name.str())==Children.end())
                IMPACT_THROW_EXCEPTION( msException("the child "+ name.str() + " does not exist in Children"
                                               ,"void msTreeMapper::clearChildren(msChildren<T>& vector)"
                                               ,getFullId() ) );
            Children.erase(name.str());
        }
        LOGGER_EXIT_FUNCTION2("void msTreeMapper::clearChildren(msChildren<T>& vector)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T>
    void msTreeMapper::finalizeDeclarationForPython(string name) {
        
        LOGGER_ENTER_FUNCTION_DBG("static void msTreeMapper::finalizeDeclarationForPython(string name)","");
        
        registerVector<T>("Vector_"+name,"Vector of object deriving from "+name);
        
        boost::python::class_<msChild<T>,
        boost::python::bases<msChildBase>,
        boost::shared_ptr<msChild<T> > >(
                                         string("msChild_"+name).c_str(),
                                         string("Child of type "+name).c_str() ,
                                         boost::python::init<>()
                                         )
        .def( "New", &msChild<T>::New , "Create a new object.")
        .staticmethod("New")
        .def("getSharedPtr",&msChild<T>::getSharedPtr,
             "return the shared pointer.");
        
        boost::python::class_<msChildren<T>,
        boost::python::bases<msChildrenBase>,
        boost::shared_ptr<msChildren<T> > >(
                                            string("msChildren_"+name).c_str(),
                                            string("Children (i.e. vector of child) of type "+name).c_str() ,
                                            boost::python::init<>()
                                            )
        .def( "New", &msChildren<T>::New , "Create a new object.")
        .staticmethod("New")
        .def("getElements",&msChildren<T>::getElements,"return the elements in a list.");
        
        
        LOGGER_EXIT_FUNCTION2("static void msTreeMapper::finalizeDeclarationForPython(string name)");
        
    }
} //end of impact


#endif // MSREGISTRATORS_H

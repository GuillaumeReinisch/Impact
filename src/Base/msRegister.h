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


#ifndef MSREGISTRATORS_H
#define MSREGISTRATORS_H

#include "config.h"

#if defined(__ICC) || defined(__INTEL_COMPILER)
#pragma warning disable 2536
#pragma warning disable 279
#pragma warning disable 161
#pragma warning disable 1125
#pragma warning disable 2196
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic ignored "-Wenum-compare"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wswitch"
// #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#if USE_PYTHON > 0
#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/suite/indexing/indexing_suite.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#endif

#include <msUtilities.h>

namespace impact
{
    
    typedef boost::error_info<struct tag_my_info,std::string> msErrorInfo;
    
    struct msError: public boost::exception, public std::exception
    {
        msError(std::string info, std::string function, std::string id)
        {
            Info=info;Id=id;Function=function;
        }
        
        
        mutable std::string message;
        
        std::string Info;
        std::string Function;
        std::string Id;
        
        void addContext(std::string str){ Contexts.push_back(str); }
        
        char const* what() const throw();
        
        std::vector<std::string> Contexts;
        ~msError() throw () { }
        
    };
    
#if USE_PYTHON
    inline boost::python::object pass_through(boost::python::object const& o) { return o; }
    
    /*
    template< class T>
    class msVector : public std::vector<T>
    {
    public:
        
        
        msVector() : std::vector<T>() {}
        
        msVector ( typename std::vector<T>::iterator first, typename std::vector<T>::iterator last ) :
        std::vector<T>(first,last){}
        
        static void registryInPython(std::string name, std::string description){
            
            using namespace boost::python;
            
            class_< msVector<T>, boost::shared_ptr<msVector<T> > >(
                                                                   name.c_str(),description.c_str(),init<>())
           // .def(vector_indexing_suite< msVector<T> >() )
            .def("push_back",&msVector<T>::push_back)
            .def("__iter__",boost::python::iterator< msVector<T> >() )
            .def("set", &msVector<T>::set )
            .def("get", &msVector<T>::set );
            
        }
        void set(size_t i, T value ) { (*this)[i]=value;
        }
        void get( size_t i ) { return (*this)[i];
        }
        
        bool operator==( msVector<T> rhs){return (*this).vectorT::operator==(rhs);
        }
    };*/
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
     
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class T> inline
    void registerVector(std::string name,std::string description){
       
        using namespace boost::python; 
       
        class_< std::vector<boost::shared_ptr<T> > >(name.c_str(),
                                                     description.c_str(),
                                                     init<>())
	.def( vector_indexing_suite<std::vector<boost::shared_ptr<T> > >() )
        .def("push_back",&std::vector<boost::shared_ptr<T> >::push_back)
        .def("__iter__",boost::python::iterator< std::vector<boost::shared_ptr<T> > >() );;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    template<class Klass, class KlassIter>
    struct iterator_wrappers
    {
        static Klass
        next(KlassIter& o)
        {
            Klass* result = o.next();
            if (!result) {
                PyErr_SetString(PyExc_StopIteration, "No more data.");
                boost::python::throw_error_already_set();
            }
            return *result;
        }
        
        static void
        wrap(const char* python_name)
        {
            using namespace boost::python;
            class_<KlassIter>(python_name, no_init)
            .def("next", next)
            .def("__iter__", pass_through)
            ;
        }
    };    //void translate( msError const& e); 
#endif
    
    
    class msRegistrar;
    class msTreeMapper;
    
    /*! \class msRegister
     *
     *  \brief All objects derived from this class are registered in a static array
     * and the function registryInPython is called for each objects at static
     * initialization.
     *
     * To registry a class named MyClass at static initialization time (just before
     * calling *main*)one have :
     *        - to derive MyClass from msRegister
     *        - to define the static createInstance method, e.g. :
     * static msRegister* MyClass::createInstance() { return new MyClass; }
     *        - and to define the static attribut:
     * static msRegistrar MyClass::Registrar("MyClass", MyClass::createInstance);
     *
     * To associate some methods/attributs in Python see registryInPython() function.
     *
     * Let's clarify with an example: a class that do addition.
     ~~~~~~~~~~~~~{.cpp}
     #include <msRegister.h>
     
     myClass : public msRegister
     {
     
     private:
     static msRegistrar Registrar;
     static msTreeMapper* createInstance() { return new myClass; }
     static bool ismyClassRegisteredInPython;
     
     protected:
     
     void registryInPython(){
     
     #if USE_PYTHON
     msRegister::registryInPython();
     
     if( ! myClass::ismyClassRegisteredInPython ) {
     
     using namespace boost::python;
     
     class_<myClass,bases<msRegister>,boost::shared_ptr<myClass> >
     ("myClass",
     "Do addition",no_init )
     .def("addition", &myClass::addition, "return a + b. arg1: a, arg2: b" );
     
     msMotion::ismyClassRegisteredInPython=1;
     }
     #endif
     }
     
     public:
     
     myClass() { }
     
     double addition(double a, double b){return a + b;}
     };
     
     bool myClass::ismyClassRegisteredInPython = 0;
     msRegistrar myClass::Registrar("myClass", myClass::createInstance);
     ~~~~~~~~~~~~~
     */
    class msRegister
    {
    public:
        
        typedef msRegister* (*base_creator_fn)();
        //! \brief this is used to store creators of dynamic childs
        typedef std::map< std::string , base_creator_fn> RegistryMap;
        
        static std::vector<std::string> getClassesRegistrated();
        
	template<class T>
	static T* New(std::string classname){
	 
	  if (msRegister::registry().find(classname) == msRegister::registry().end() ) {
	    
	      msError e("Can not find the class of name "+classname+". Is the class registrated? check by calling the method 'getClassesRegistrated'.",
		"msRegister::New","");
	      BOOST_THROW_EXCEPTION(e); 
	  }
	  return static_cast<T*>( msRegister::registry()[classname]() );
	};
	

        
        msRegister();
        
        /** \brief Register the class and its functions in Python
         *
         * This has to be protected as the derived classes will have to
         * call it.
         *
         * More information in http://www.boost.org/doc/libs/1_54_0/libs/python/doc/index.html
         *
         * A typical organisation for a class myClass:
         *~~~~~~~~~~~~~{.cpp}
         *   #if USE_PYTHON
         msParentClass::registryInPython();
         
         if( ! myClass::ismyClassRegisteredInPython ) {
         
         using namespace boost::python;
         
         class_<myClass,bases<msRegister>,boost::shared_ptr<myClass> >
         ("myClass",
         "Do addition",no_init )
         .def("method1", &myClass::method1, "documentation" )
	     .def("method2", &myClass::method2, "documentation" );
         
         msMotion::ismyClassRegisteredInPython = 1;
         }
         #endif
         *~~~~~~~~~~~~~
         */
        virtual void registryInPython();
        
        
        virtual std::ostream& print(std::ostream& out) const {return out;}
        
#if USE_PYTHON > 0
        std::string getRegistrated();
#endif
        
        //! \brief Return the dynamic registry map
        static RegistryMap& registry();
        
    private:
        
        static bool isRegisterRegistredInPython; //!< 1 if registryInPython have been called, 0 else
        
        static msRegistrar Registrar;
        
        //! \brief Create a new instance, used to registrate classes into database
        static msRegister*  create() { return new msRegister; }
        
    };
    
    struct msRegistrar
    {
        msRegistrar(std::string name, msRegister::base_creator_fn func);
    };
    
    std::ostream& operator<<(std::ostream& out,msRegister& obj);
    
} //end of impact 


#endif // MSREGISTRATORS_H

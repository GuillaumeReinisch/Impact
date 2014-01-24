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

#include "msAtom.h"

namespace impact {
    
    namespace atomism {
        
        double getCovalentLength(int order,const msAtom& a,const msAtom& b)
        {
            switch(order)
            {case 1: if( (a.CovalentRadius1>0) && (b.CovalentRadius1>0) )
            { return a.CovalentRadius1+b.CovalentRadius1;
            }
                case 2: if( (a.CovalentRadius2>0) && (b.CovalentRadius2>0) )
                { return a.CovalentRadius2+b.CovalentRadius2;
                }
                case 3: if( (a.CovalentRadius3>0) && (b.CovalentRadius3>0) )
                { return a.CovalentRadius3+b.CovalentRadius3;
                }
            }
            return -1;
        }
        
        std::string getAtomName(int atomicNumb)
        {
            switch(atomicNumb)
            {
                case 1: return "H";break;
                case 2: return "He";break;
                case 5: return "B";break;
                case 6: return "C";break;
                case 8: return "O";break;
                case 10: return "Ne";break;
                case 14: return "Si";break;
		case 15: return "P";break;
                case 17: return "Cl";break;
                case 7: return "N";break;
                case 9: return "F";break;
            }
            return "X";
        }
        
        
        boost::shared_ptr<msAtom> NewAtom(std::string type)
        {
            boost::shared_ptr<msAtom> at;
            
            if( type=="H"  )  	at = boost::static_pointer_cast<msAtom>( msHatom::New() );
	    if( type=="He"  )  	at = boost::static_pointer_cast<msAtom>( msHEatom::New() );
            if( type=="C"  )  	at = boost::static_pointer_cast<msAtom>( msCatom::New() );
            if( type=="B"  )  	at = boost::static_pointer_cast<msAtom>( msBatom::New() );
            if( type=="Cl" ) 	at = boost::static_pointer_cast<msAtom>( msCLatom::New() );
            if( type=="X"  )  	at = boost::static_pointer_cast<msAtom>( msXatom::New() );
            if( type=="O"  )  	at = boost::static_pointer_cast<msAtom>( msOatom::New() );
	    if( type=="Ne"  )  	at = boost::static_pointer_cast<msAtom>( msNEatom::New() );
            if( type=="Si" ) 	at = boost::static_pointer_cast<msAtom>( msSIatom::New() );
            if( type=="P" ) 	at = boost::static_pointer_cast<msAtom>( msPatom::New() );
            if( type=="N"  )  	at = boost::static_pointer_cast<msAtom>( msNatom::New() );	    
            if( type=="F"  )  	at = boost::static_pointer_cast<msAtom>( msFatom::New() );
            if( type=="Ar")  	at = msARatom::New();
            if( type=="AR")  	at = msARatom::New();
            
            if( at == boost::shared_ptr<msAtom>())
                
                BOOST_THROW_EXCEPTION( msError("Atom of type "+type+" not known. Did you add the class in msAtom.h? Did you updated the NewAtom function in msAtom.cpp?"
                                               ,"boost::shared_ptr<msAtom> NewAtom(std::string type)"
                                               ," no id " ));
            
            return at;
        }
        
        
        void getListAtomsFromName(string name,vector<boost::shared_ptr<msAtom> >& result){
	  
	     int i=0;
             while (name[i]) {
	       
                 char c = name[i];
                 if ( isupper(c) ) { //new atom
		   
		    string type = name.substr(i,1);
		    i++;
		    c = name[i];
		    while (islower(c)) {
		      
		        type += name.substr(i,1);
		        i++;
		        c = name[i];
		    } //end of name : c = Upper-> new atom or c=number ->get it
		    int n=1;
		    string count;
		    
		    if( isdigit(c) ) {
		        
		        count += c;
			i++;
		        c = name[i];
		        while (isdigit(c)) {
		      
		            count += c;
		            i++;
		            c = name[i];
		        }
		        n = atoi(count.c_str());
		    }
		    for( size_t j=0;j<n;j++) result.push_back( NewAtom(type) ); 
		 }
                 else{ i++; }
             }
	     
	}
	
	
        bool msAtom::isAtomRegisteredInPython=0;
        msRegistrar msAtom::Registrar("msAtom", msAtom::createInstance);
        
        
        void msAtom::registryInPython()    {
            
#if USE_PYTHON
            msElement::registryInPython();
            if( ! msAtom::isAtomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msAtom,bases<msElement>,boost::shared_ptr<msAtom> >
                ("msAtom",
                 "An atom",no_init)
                .def("getHf" ,   &msAtom::getHf,
                     "return the formation enthalpie at 273 C. arg2: energy unit desired"
                     )
                .def("getVdWRadius" ,   &msAtom::getVdWRadius,
                     "return the Vand der Walls radius. arg2: length unit desired"
                     )
                .def("getName" ,   &msAtom::getName,
                     "return the name of the atom"
                     );
                msAtom::isAtomRegisteredInPython=1;
            }
#endif
        }
        
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msARatom::isARatomRegisteredInPython = 0;
        msRegistrar msARatom::Registrar("msARatom", msARatom::createInstance);
        
        void msARatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msARatom::isARatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msARatom,bases<msAtom>,boost::shared_ptr<msARatom> >
                ("msARatom",
                 "argon atom",no_init)
                .def( "New", &msARatom::New , "Create a new object.")
                .staticmethod("New");
                msARatom::isARatomRegisteredInPython=1;
            }
#endif
            
        }
        
        
        
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msHatom::isHatomRegisteredInPython = 0;
        msRegistrar msHatom::Registrar("msHatom", msHatom::createInstance);
        
        void msHatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msHatom::isHatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msHatom,bases<msAtom>,boost::shared_ptr<msHatom> >
                ("msHatom",
                 "Hydrogen atom",no_init)
                .def( "New", &msHatom::New , "Create a new object.")
                .staticmethod("New");
                msHatom::isHatomRegisteredInPython=1;
            }
#endif
            
        }
        
        
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msOatom::isOatomRegisteredInPython = 0;
        msRegistrar msOatom::Registrar("msOatom", msOatom::createInstance);
        
        void msOatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msOatom::isOatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msOatom,bases<msAtom>,boost::shared_ptr<msOatom> >
                ("msOatom",
                 "Oxygen atom",no_init)
                .def( "New", &msOatom::New , "Create a new object.")
                .staticmethod("New");
                msOatom::isOatomRegisteredInPython=1;
            }
#endif
            
        }
        
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msBatom::isBatomRegisteredInPython = 0;
        msRegistrar msBatom::Registrar("msBatom", msBatom::createInstance);
        
        void msBatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msBatom::isBatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msBatom,bases<msAtom>,boost::shared_ptr<msBatom> >
                ("msBatom",
                 "Boron atom",no_init)
                .def( "New", &msBatom::New , "Create a new object.")
                .staticmethod("New");
                msBatom::isBatomRegisteredInPython=1;
            }
#endif
            
        }
        
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msCatom::isCatomRegisteredInPython = 0;
        msRegistrar msCatom::Registrar("msCatom", msCatom::createInstance);
        
        void msCatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msCatom::isCatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msCatom,bases<msAtom>,boost::shared_ptr<msCatom> >
                ("msCatom",
                 "Carbon atom",no_init)
                .def( "New", &msCatom::New , "Create a new object.")
                .staticmethod("New");
                msCatom::isCatomRegisteredInPython=1;
            }
#endif
            
        }
        
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msSIatom::isSIatomRegisteredInPython = 0;
        msRegistrar msSIatom::Registrar("msSIatom", msSIatom::createInstance);
        
        void msSIatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msSIatom::isSIatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msSIatom,bases<msAtom>,boost::shared_ptr<msSIatom> >
                ("msSIatom",
                 "Sicilium atom",no_init)
                .def( "New", &msSIatom::New , "Create a new object.")
                .staticmethod("New");
                msSIatom::isSIatomRegisteredInPython=1;
            }
#endif
            
        }
        
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msCLatom::isCLatomRegisteredInPython = 0;
        msRegistrar msCLatom::Registrar("msCLatom", msCLatom::createInstance);
        
        
        void msCLatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msCLatom::isCLatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msCLatom,bases<msAtom>,boost::shared_ptr<msCLatom> >
                ("msCLatom",
                 "Chlorine atom",no_init)
                .def( "New", &msCLatom::New , "Create a new object.")
                .staticmethod("New");
                msCLatom::isCLatomRegisteredInPython=1;
            }
#endif
            
        }
        
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msNatom::isNatomRegisteredInPython = 0;
        msRegistrar msNatom::Registrar("msNatom", msNatom::createInstance);
        
        void msNatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msNatom::isNatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msNatom,bases<msAtom>,boost::shared_ptr<msNatom> >
                ("msNatom",
                 "Nitrogen atom",no_init)
                .def( "New", &msNatom::New , "Create a new object.")
                .staticmethod("New");
                msNatom::isNatomRegisteredInPython=1;
            }
#endif
            
        }
        
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msFatom::isFatomRegisteredInPython = 0;
        msRegistrar msFatom::Registrar("msFatom", msFatom::createInstance);
        
        void msFatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msFatom::isFatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msFatom,bases<msAtom>,boost::shared_ptr<msFatom> >
                ("msFatom",
                 "fluor atom",no_init)
                .def( "New", &msFatom::New , "Create a new object.")
                .staticmethod("New");
                msFatom::isFatomRegisteredInPython=1;
            }
#endif
            
        }
        
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msPatom::isPatomRegisteredInPython = 0;
        msRegistrar msPatom::Registrar("msPatom", msPatom::createInstance);
        
        void msPatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msPatom::isPatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msPatom,bases<msAtom>,boost::shared_ptr<msPatom> >
                ("msPatom",
                 "helium atom",no_init)
                .def( "New", &msPatom::New , "Create a new object.")
                .staticmethod("New");
                msPatom::isPatomRegisteredInPython=1;
            }
#endif
            
        }
                
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msHEatom::isHEatomRegisteredInPython = 0;
        msRegistrar msHEatom::Registrar("msHEatom", msHEatom::createInstance);
        
        void msHEatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msHEatom::isHEatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msHEatom,bases<msAtom>,boost::shared_ptr<msHEatom> >
                ("msHEatom",
                 "helium atom",no_init)
                .def( "New", &msHEatom::New , "Create a new object.")
                .staticmethod("New");
                msHEatom::isHEatomRegisteredInPython=1;
            }
#endif
            
        }
                
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        
        bool msNEatom::isNEatomRegisteredInPython = 0;
        msRegistrar msNEatom::Registrar("msNEatom", msNEatom::createInstance);
        
        void msNEatom::registryInPython(){
            
#if USE_PYTHON
            msAtom::registryInPython();
            if( ! msNEatom::isNEatomRegisteredInPython ){
                
                using namespace boost::python;
                
                class_<msNEatom,bases<msAtom>,boost::shared_ptr<msNEatom> >
                ("msNEatom",
                 "helium atom",no_init)
                .def( "New", &msNEatom::New , "Create a new object.")
                .staticmethod("New");
                msNEatom::isNEatomRegisteredInPython=1;
            }
#endif
            
        }
    }
    
}

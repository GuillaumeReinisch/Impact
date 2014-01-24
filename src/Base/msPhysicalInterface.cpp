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

#include "msPhysicalInterface.h"

namespace impact{
    
    namespace csts
    {
        double h	=	6.6260755e-34;   	// J S
        double hb	=	1.054571628e-34;
        double kb	=	1.380658e-23;		// J/mol/K
        double pi	=	3.14159265;
        double R	=	8.3144621; 		// J/mol
        double Nav	=	6.0221413e23;
    }
    
    using namespace std;
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    bool msPhysicalInterface::isInterfaceRegisteredInPython=0;
    msRegistrar msPhysicalInterface::Registrar("msPhysicalInterface", msPhysicalInterface::createInstance);
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& operator<<(std::ostream& out,const msPhysicalInterface& inter)
    {
        return( inter.print(out) );
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msPhysicalInterface::registryInPython() {
        
#if USE_PYTHON
        msTreeMapper::registryInPython();
        if( ! msPhysicalInterface::isInterfaceRegisteredInPython ) {
            
            using namespace boost::python;
            class_<msPhysicalInterface,bases<msTreeMapper >
            ,boost::shared_ptr<msPhysicalInterface> >
            ("msPhysicalInterface",
             "Defines a physical object: provides the management of units + parameters + output ",
             no_init)
            .def( "getUnits", &msPhysicalInterface::getUnits ,
                 "return a shared pointer to the units manager." )
            .def( "getParameters",&msPhysicalInterface::getParameters ,
                 "return a shared pointer to the parameters manager." )
            .def( "setUnits", &msPhysicalInterface::setUnits  ,
                 "set the units manager." )
            .def( "setParameters", &msPhysicalInterface::setParameters  ,
                 "set the parameters manager. arg2: Parameters manager." )
            .def( "setParameter", &msPhysicalInterface::setParameter  ,
                 "set a parameter. arg2: key; arg3: value" )
            .def( "homogenizeUnits", &msPhysicalInterface::homogenizeUnits  ,
                 "set the units system of the physical objects children to the units system getUnits()." )
	    .def( "getPhysicalVariables", &msPhysicalInterface::getPhysicalVariables  ,
                 "return the physical variables." )
            .def(self_ns::str(self_ns::self));
            
            boost::python::scope().attr("pi") = csts::pi;
            boost::python::scope().attr("hb") = csts::hb;
            boost::python::scope().attr("kb") = csts::kb;
            boost::python::scope().attr("h") = csts::h;
            boost::python::scope().attr("R") = csts::R;
            boost::python::scope().attr("Nav") = csts::Nav;
	    
            msPhysicalInterface::msPhysicalVariable::registryInPython();
	    
            msPhysicalInterface::isInterfaceRegisteredInPython=1;
	    finalizeDeclarationForPython<msPhysicalInterface>("msPhysicalInterface");
        }
#endif
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msPhysicalInterface::setParameters(boost::shared_ptr<msParamsManager> param){
        
        LOGGER_ENTER_FUNCTION_DBG("void msPhysicalInterface::setParameters(boost::shared_ptr<msParamsManager> param)", getFullId());
        LOGGER_WRITE(msLogger::DEBUG, "set the parameters from type "+param->getType())
        
        try{
            msTreeMapper::update<msParamsManager>( Parameters , param);
            Parameters->addObjectUsingMe( boost::static_pointer_cast<msPhysicalInterface>( mySharedPtr() ) );
            Parameters->setAffiliation(mySharedPtr());
            updateParameters();
        }
        catch(msError& e){
            
            e.addContext("void msPhysicalInterface::setParameters(boost::shared_ptr<msParamsManager> param)");
	    throw e;
        }
        LOGGER_EXIT_FUNCTION2("void msPhysicalInterface::setParameters(boost::shared_ptr<msParamsManager> param)");
        
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msPhysicalInterface::setParameter(string key,string value){
        
        LOGGER_ENTER_FUNCTION_DBG("void msPhysicalInterface::setParameter(string key,string value)", getFullId());
        
        try{
            Parameters->setParameter(key,value,*Units);
            updateParameters();
        }
        catch(msError& e){
            
            e.addContext("void msPhysicalInterface::setParameter(string key,string value)");
        }
        LOGGER_EXIT_FUNCTION2("void msPhysicalInterface::setParameter(string key,string value)");
        
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msPhysicalInterface::setUnits(boost::shared_ptr<msUnitsManager> units){
        
        LOGGER_ENTER_FUNCTION_DBG("void msPhysicalInterface::setUnits(boost::shared_ptr<msUnitsManager> units)", getFullId());
        msUnitsManager old;
        old.set( Units->getExpression() );
        
        msTreeMapper::update<msUnitsManager>( Units , units);
        units->addObjectUsingMe( boost::static_pointer_cast<msPhysicalInterface>( mySharedPtr() ) );
	LOGGER_WRITE(msLogger::DEBUG, "Number of objects using the units: " + output::getString<size_t>(getUnits()->LinkedObjects.size()));
        updateUnits(old,*units);
	
	LOGGER_EXIT_FUNCTION2("void msPhysicalInterface::setUnits(boost::shared_ptr<msUnitsManager> units)");        
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msPhysicalInterface::updateUnits(msUnitsManager& Old,msUnitsManager& New) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msPhysicalInterface::updateUnits(msUnitsManager& Old,msUnitsManager& New)", getFullId());
        LOGGER_WRITE(msLogger::DEBUG, "update the value of the physical variables declared")
        /*
        map< double* , msUnit >::iterator it = PtrOnPhysicalVariables.begin();
        
        for(;it!=PtrOnPhysicalVariables.end();++it){
            
            *((*it).first) *= New.convert( (*it).second, 1 );
            (*it).second.reset(New);
        }*/
        map< string, boost::shared_ptr<msPhysicalVariable> >::iterator it = PhysicalVariables.begin();
        
        for(;it!=PhysicalVariables.end();++it)  (*it).second->reset( New );
        
        LOGGER_EXIT_FUNCTION2("void msPhysicalInterface::updateUnits(msUnitsManager& Old,msUnitsManager& New)");
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msPhysicalInterface::declarePhysicalVariable(const msUnit& unit,double* ptr,string id) {
        
        LOGGER_ENTER_FUNCTION_DBG("void msPhysicalInterface::declarePhysicalVariable(const msUnit& unit,double* ptr,string id)", getFullId());
	 
        if(id=="") id = "var_"+output::getString<size_t>(PhysicalVariables.size());
      
	cout<<getId()<<": declare physical variable "<<id<<" "<<*ptr<<" "<<unit.getStr()<<endl;
	
        PhysicalVariables[id] = boost::shared_ptr<msPhysicalVariable>(new msPhysicalVariable(id,ptr,unit));
	LOGGER_EXIT_FUNCTION2("void msPhysicalInterface::declarePhysicalVariable(const msUnit& unit,double* ptr,string id)");
       //PtrOnPhysicalVariables[ptr] =  unit;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
        
    vector<boost::shared_ptr<msPhysicalInterface::msPhysicalVariable> > msPhysicalInterface::getPhysicalVariables() const {
      
        vector<boost::shared_ptr<msPhysicalVariable> > vec;
	
	map< string, boost::shared_ptr<msPhysicalVariable> >::const_iterator it = PhysicalVariables.begin();
        
        for(;it!=PhysicalVariables.end();++it)  {
	  
	    vec.push_back((*it).second);
	    cout<<(*it).second->getName()<<" "<<(*it).second->getValue()<<endl;
	}
        return vec;
    }
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msPhysicalInterface::homogenizeUnits() {
        
        vector<boost::shared_ptr<msPhysicalInterface> > children =
            getChildrenDerivedFrom<msPhysicalInterface>("msPhysicalInterface");
        
        vector<boost::shared_ptr<msPhysicalInterface> >::iterator it;
        
        for( it = children.begin(); it != children.end();++it){
            (*it)->setUnits(getUnits());
        }
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    /*
    std::string getStringUnit(strunit::UnitType type, msPhysicalInterface* obj) {
        
        using namespace strunit;
        switch( type ) {
            case dKinEner:
                return obj->getUnits()->getMassStr() +"."+ obj->getUnits()->getLengthStr() + "^2"; break ;
            case dP:  
                return obj->getUnits()->getMassStr() +"."+ obj->getUnits()->getLengthStr(); break ;
            case dJ:  
                return obj->getUnits()->getMassStr() +"."+ obj->getUnits()->getLengthStr() + "^2"; break ;
        }
        return "";
    }*/
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& msPhysicalInterface::print(std::ostream& out) const {
        
        msTreeMapper::print(out);
        output::printHeader(out,"Physical interface");
        out<<"\nParameters: \n";
        Parameters->print(out);
        out<<"\nUnits: \n";
        Units->abstract(out);
        out<<"\n";
        return out;
    }
    
}

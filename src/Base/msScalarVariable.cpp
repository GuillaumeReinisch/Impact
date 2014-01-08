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


#include "msScalarVariable.h"

namespace impact
{
    
    
    bool msScalarVariable::isScalarVariableRegisteredInPython=0;
    //bool msLengthVariable::isLengthVariableRegisteredInPython=0;
    //bool msAngleVariable::isAngleVariableRegisteredInPython=0;
    
    msRegistrar msScalarVariable::Registrar("msScalarVariable", msScalarVariable::createInstance);
    //msRegistrar msLengthVariable::Registrar("msLengthVariable", msLengthVariable::createInstance);
    //msRegistrar msAngleVariable::Registrar("msAngleVariable", msAngleVariable::createInstance);
    
    
    std::ostream& operator<<(std::ostream& out,const msScalarVariable& var) {
        
        var.print(out);
        return out;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msScalarVariable::registryInPython() {
        
#if USE_PYTHON
        msTreeMapper::registryInPython();
        if( ! msScalarVariable::isScalarVariableRegisteredInPython ){
            
            using namespace boost::python;
            
            class_<msScalarVariable,bases<msTreeMapper>,boost::shared_ptr<msScalarVariable> >
            ("msScalarVariable","define a scalar variable",no_init)
            .def( "New", &msScalarVariable::New , "Create a new object. arg2: msUnitsManager object")
            .staticmethod("New")
            .def( "set", (boost::shared_ptr<msTreeMapper>(msScalarVariable::*)(double,double,double,double,double, string))&msScalarVariable::set
                 , "set the variable with specified unit. arg2: name, arg3-6: min,max,dq,Dq, arg7: unit")
            .def( "set", (boost::shared_ptr<msTreeMapper>(msScalarVariable::*)(double,double,double,double,double))&msScalarVariable::set
                 , "set the variable with previously defined unit. arg2: name, arg3-6: min,max,dq,Dq")
            .def( "getUnit" ,  &msScalarVariable::getUnit , "return the unit" )
            .def( "begin" ,  &msScalarVariable::begin ,"set the variable at its minimum" )
            .def( "increment" ,  &msScalarVariable::increment ,"iterate the variable by its integration step. return 0 if the maximum value is reached" )
            .def( "indice" , &msScalarVariable::indice , "return the int( (q0.getMax()-q0.getMin())/Dq +0.5 ).")
            .def( "setValue", (boost::shared_ptr<msTreeMapper>(msScalarVariable::*)(double)) &msScalarVariable::setValue,
                 "Set the value.")
            .def( "setMin", (boost::shared_ptr<msTreeMapper>(msScalarVariable::*)(double))&msScalarVariable::setMin,
                 "Set the minimum.")
            .def( "setMax", (boost::shared_ptr<msTreeMapper>(msScalarVariable::*)(double))&msScalarVariable::setMax,
                 "Set the maximum.")
            .def( "setdq", (boost::shared_ptr<msTreeMapper>(msScalarVariable::*)(double))&msScalarVariable::setdq,
                 "Set the derivation step.")
            .def( "setDq", (boost::shared_ptr<msTreeMapper>(msScalarVariable::*)(double))&msScalarVariable::setDq,
                 "Set the integration step.")
            .def( "getValue", (double(msScalarVariable::*)()) &msScalarVariable::getValue,
                 "return the value.")
            .def( "getMin",  (double(msScalarVariable::*)()) &msScalarVariable::getMin,
                 "return the minimum.")
            .def( "getMax",   (double(msScalarVariable::*)()) &msScalarVariable::getMax,
                 "return the maximum.")
            .def( "getdq",  (double(msScalarVariable::*)()) &msScalarVariable::getdq,
                 "return the derivation step.")
            .def( "getDq",  (double(msScalarVariable::*)()) &msScalarVariable::getDq,
                 "return the integration step.")
            .def("getUnitStr", &msScalarVariable::getUnitStr,
                 "return the unit of the variable")
            .def("setUnit",& msScalarVariable::setUnit,
                 "set the unit of the variable. arg2: unit.")
            .def(self_ns::str(self_ns::self));
	    
	    msTreeMapper::finalizeDeclarationForPython<msScalarVariable>("msScalarVariable");
		    
            msScalarVariable::isScalarVariableRegisteredInPython=1;
        }
#endif
    }
    
    
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& msScalarVariable::print(std::ostream& out) const {
        
        msTreeMapper::print(out);
        output::printHeader(out,"Scalar variable");
        
        abstract(out);
        return out;
    }
    
    std::ostream& msScalarVariable::abstract(std::ostream& out) const {
        
        out<<"variable of id "<<getId()<<": Value="<<Value<<" range:["<<Min<<","<<Max<<"], delta:["<<dq<<","<<Dq<<"], default: "<<Default;
        return out;
    }
    
    
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    /*
     void msAngleVariable::registryInPython() {
     
     msScalarVariable::registryInPython();
     
     if( ! msAngleVariable::isAngleVariableRegisteredInPython ){
     
     using namespace boost::python;
     
     class_<msAngleVariable,bases<msScalarVariable>,boost::shared_ptr<msAngleVariable> >
     ("msAngleVariable","define an angle scalar variable",no_init)
     .def( "New", &msAngleVariable::New , "Create a new angle variable")
     .staticmethod("New");
     
     msAngleVariable::isAngleVariableRegisteredInPython=1;
     }
     }
     
     void msLengthVariable::registryInPython() {
     
     msScalarVariable::registryInPython();
     
     if( ! msLengthVariable::isLengthVariableRegisteredInPython ){
     
     using namespace boost::python;
     
     class_<msLengthVariable,bases<msScalarVariable>,boost::shared_ptr<msLengthVariable> >
     ("msLengthVariable","define a length scalar variable",no_init)
     .def( "New", &msLengthVariable::New , "Create a new length variable")
     .staticmethod("New");
     
     msLengthVariable::isLengthVariableRegisteredInPython=1;
     }
     }*/
    
}


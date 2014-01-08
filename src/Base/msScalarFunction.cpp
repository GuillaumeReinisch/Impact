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

#include "msScalarFunction.h"
//#include <msSampler.h>

namespace impact
{
    bool msScalarFunction::isScalarFunctionRegisteredInPython=0;
    
    msRegistrar msScalarFunction::Registrar("msScalarFunction", msScalarFunction::createInstance);
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& operator<<(std::ostream& out,const msScalarFunction& fct)
    {
        fct.print(out);
        return out;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msScalarFunction::registryInPython()
    {
#if USE_PYTHON
        
        msTreeMapper::registryInPython();
        
        if( ! msScalarFunction::isScalarFunctionRegisteredInPython ) {
            
            using namespace boost::python;
            
            class_<msScalarFunction,bases<msTreeMapper>,boost::shared_ptr<msScalarFunction> >
            ("msScalarFunction",
             "virtual base class of scalar function",init<>()
             )
            .def( "New",&msScalarFunction::New ,"Return a shared pointer to a new object" )
            .staticmethod("New")
            .def("evaluate", (double(msScalarFunction::*)() ) &msScalarFunction::evaluate, "evaluate the function at the current value of the coordinates.")
            .def("evaluate", (double(msScalarFunction::*)(double)) &msScalarFunction::evaluate)
            .def("evaluate", (double(msScalarFunction::*)(double,double)) &msScalarFunction::evaluate)
            .def("evaluate", (double(msScalarFunction::*)(double,double,double)) &msScalarFunction::evaluate)
            .def("evaluate", (double(msScalarFunction::*)(const vector<double>&)) &msScalarFunction::evaluate)
            .def("partialDerivative", &msScalarFunction::partialDerivative ,
                 "return the partial derivative w/ particular coordinate at the current configuration. arg2: indice of the coordinate")
            .def("partialSecondDerivative", (double(msScalarFunction::*) (size_t))&msScalarFunction::partialSecondDerivative ,
                 "return the second partial derivative w/ particular coordinate at the current configuration. arg2: indice of the coordinate")
            .def("partialSecondDerivative", (double(msScalarFunction::*) (size_t,size_t))&msScalarFunction::partialSecondDerivative ,
                 "return the second partial derivative w/ particular coordinate at the current configuration. arg2: indice of the coordinate")
            .def("setConstant",  &msScalarFunction::setConstant,
                 "set a constante (scalar value) usable by the function. arg2: key, arg3:value")
            .def("getConstants",  &msScalarFunction::getConstants,
                 "return the constants defined in a vector of string. Each item is like: 'key : value'")
            .def("setYUnit",  &msScalarFunction::setYUnit,
                 "set the unit of the returned value. arg2: unit in a string representation")
            .def("getXSample1d",  &msScalarFunction::getXSample1d,
                 "return a regular sample (in python list) of a particular coordinate. Arg1: indice of the coordinate(int), Arg2: number of sample (int)." )
            .def("getYSample1d",  &msScalarFunction::getYSample1d,
                 "return a regular sample (in python list) of the function values against a particular coordinate. Arg1: indice of the coordinate(int), Arg2: number of sample (int)." )
            .def("getUnit", &msScalarFunction::getUnit,
                 "return the unit of the return value")
            .def("setCoordinates", &msScalarFunction::setCoordinates,
                 "set the coordinates. Arg1: coordinates")
            .def("getCoordinates",&msScalarFunction::getCoordinates,
                 "return the coordinates")
            .def(self_ns::str(self_ns::self));
            
            msTreeMapper::finalizeDeclarationForPython<msScalarFunction>("msScalarFunction");
            
            msScalarFunction::isScalarFunctionRegisteredInPython=1;
        }
#endif
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msScalarFunction::setCoordinates(boost::shared_ptr<msGeneralizedCoordinates> coor) {
        
        LOGGER_ENTER_FUNCTION_DBG("msScalarFunction::setCoordinates(...)",getFullId());
        if(!coor->hasParent()) coor->setAffiliation(mySharedPtr());
        msTreeMapper::update(Coordinates,coor);
        update();
        LOGGER_EXIT_FUNCTION();
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    double msScalarFunction::evaluate(const vector<double>& data) {
        
        for(size_t i=0;i<Coordinates->noOfDim();i++) (*Coordinates)[i]->setValue(data[i]);
        return( evaluate() );
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    double msScalarFunction::evaluate(double* data)
    {
        for(size_t i=0;i<Coordinates->noOfDim();i++) (*Coordinates)[i]->setValue(data[i]);
        return( evaluate() );
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    double msScalarFunction::evaluate(double data)  {
        
        LOGGER_ENTER_FUNCTION_DBG("double msScalarFunction::evaluate(double data) ",getFullId());
        
        if(noOfDim()<1) {
            
            BOOST_THROW_EXCEPTION(msError("the dimension of the coordinates has to be >0",
                                          "double msScalarFunction::evaluate(double data)", getFullId())
                                  );
        }
        LOGGER_EXIT_FUNCTION();
        return evaluate(&data);
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    double msScalarFunction::evaluate(double x,double y)
    {
        LOGGER_ENTER_FUNCTION_DBG("double msScalarFunction::evaluate(double x,y) ",getFullId());
        
        if(noOfDim()<2) {
            
            BOOST_THROW_EXCEPTION(msError("the dimension of the coordinates has to be >1",
                                          "double msScalarFunction::evaluate(double x,double y)", getFullId())
                                  );
        }
        double data[2];data[0]=x;data[1]=y;
        LOGGER_EXIT_FUNCTION();
        return evaluate(data);
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    double msScalarFunction::evaluate(double x,double y,double z)
    {
        LOGGER_ENTER_FUNCTION_DBG("double msScalarFunction::evaluate(double x,y,z) ",getFullId());
        
        if(noOfDim()<3) {
            
            BOOST_THROW_EXCEPTION(msError("the dimension of the coordinates has to be >2",
                                          "double msScalarFunction::evaluate(double x,double y,double z)", getFullId())
                                  );
        }
        double data[3];data[0]=x;data[1]=y;data[2]=z;
        LOGGER_EXIT_FUNCTION();
        return evaluate(data);
    }
    
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    double msScalarFunction::partialDerivative(size_t i) {
        
        if( i>= Coordinates->noOfDim() )
            BOOST_THROW_EXCEPTION(msError( "Indice i ("+output::getString<int>(i)+") out of range: number of coordinates = "+ output::getString<int>(Coordinates->noOfDim()),
                                          "double msScalarFunction::partialDerivative(size_t i)",
                                          getFullId()
                                          )
                                  );
        double dq=(*Coordinates)[i]->getdq();
        double v0=evaluate();
        (*Coordinates)[i]->setValue( (*Coordinates)[i]->getValue() + dq );
        double v1=evaluate();
        (*Coordinates)[i]->setValue( (*Coordinates)[i]->getValue() - dq );
        return ( (v1-v0)/dq);
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    double msScalarFunction::partialSecondDerivative(size_t i,size_t j) {
        
        if( i>= Coordinates->noOfDim() )
            BOOST_THROW_EXCEPTION(msError( "Indice i ("+output::getString<int>(i)+") out of range: number of coordinates = "+ output::getString<int>(Coordinates->noOfDim()),
                                          "double msScalarFunction::partialDerivative(size_t i)",
                                          getFullId()
                                          )
                                  );
        if( j>= Coordinates->noOfDim() )
            BOOST_THROW_EXCEPTION(msError( "Indice j ("+output::getString<int>(j)+") out of range: number of coordinates = "+ output::getString<int>(Coordinates->noOfDim()),
                                          "double msScalarFunction::partialDerivative(size_t i)",
                                          getFullId()
                                          )
                                  );
        double dv0=partialDerivative(i);
        double dq=(*Coordinates)[i]->getdq();
        (*Coordinates)[j]->setValue( (*Coordinates)[j]->getValue() + dq );
        double dv1=partialDerivative(j);
        (*Coordinates)[j]->setValue( (*Coordinates)[j]->getValue() - dq );
        return ( (dv1-dv0)/dq);
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    std::ostream& msScalarFunction::print(std::ostream& out) const
    {
        msTreeMapper::print(out);
        
        output::printHeader(out,"Scalar function");
        
        if(  Coordinates->noOfDim() != 0) {
            
            out<<"\nId of the coordinates:  "<<Coordinates->getId()<<"; type: "<<Coordinates->getType()<<endl<<"\t";
            Coordinates->getUnits()->abstract(out);
            out<<"\n\tList:\n";
            msGeneralizedCoordinates::iterator qi = Coordinates->begin();
            for(; qi!=Coordinates->end(); qi++) {
                out<<"\t\t"; (*qi)->abstract(out); out<<endl;}
            
        }
        else
        {
            out<<"no coordinates defined, you can set them using the setCoordinates function"<<endl;
        }
        return out;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    void msScalarFunction::save(size_t i,size_t n,string filename){
        
        ofstream file(filename.c_str());
        
        file<<"q"<<i<<"["<<(*Coordinates)[i]->getUnitStr()<<"]\t"<<"f(q"<<i<<")["<<getYUnit().getStr()<<"]"<<endl;
        
        for( size_t j=0;j<n;j++)
            file<<(*Coordinates)[i]->value(double(j)/n)<<"\t"<<evaluate( (*Coordinates)[i]->value(double(j)/n) )<<endl;
        file.close();    
    }
    
    /*
     msScalarFunction::msIntegrationData msScalarFunction::Integrate() 
     {
     LogLevel++;
     std::stringstream comment;
     double result=0;double v=0;
     msScalarFunction::msIntegrationData data;
     
     while( Sampler->getNextPoint() )
     {
     result += v = evaluate(); 
     
     if( v<data.Min ){ data.Min=v; }
     if( v>data.Max ){ data.Max=v; }
     
     if( (Sampler->noOfScanned()!=0) && (Sampler->noOfScanned() % Sampler->AffStep) == 0 )
     {   
     Coordinates.writePosAndField(comment,v);
     writeLog(comment);
     }    
     }
     data.Average=result/Sampler->noOfScanned();
     result*=Coordinates.getVolumeDq(); 
     
     comment<<getAffiliation()<<"Integrate() : result="<<result<<std::endl;
     writeLog(comment);
     LogLevel--;
     return data;
     }*/
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    /*
     msScalarFunction::msIntegrationData msScalarFunction::Integrate(boost::shared_ptr<msUnitsManager> units)
     {
     msScalarFunction::msIntegrationData data =  Integrate();
     double c=units->convert( *Unit, 1);
     data.Value = units->convert( *Unit, data.Value * Coordinates.getVolumeDq( *units)/Coordinates.getVolumeDq() );
     data.Min *= c; data.Max *= c; data.Average*= c;
     return data;
     }*/
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
}

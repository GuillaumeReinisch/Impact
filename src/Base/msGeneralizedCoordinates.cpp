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


#include "msGeneralizedCoordinates.h"

namespace impact
{
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    bool msGeneralizedCoordinates::isGeneralizedCoordinatesRegisteredInPython=0;
    
    msRegistrar msGeneralizedCoordinates::Registrar("msGeneralizedCoordinates", msGeneralizedCoordinates::createInstance);
    
    std::ostream& operator<<(std::ostream& out,const msGeneralizedCoordinates& gencoor) {
        
        gencoor.print(out);
        return out;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msGeneralizedCoordinates::registryInPython() {
        
#if USE_PYTHON
        
        msPhysicalInterface::registryInPython();
        if( ! msGeneralizedCoordinates::isGeneralizedCoordinatesRegisteredInPython ){
            
            using namespace boost::python;
            
            class_<msGeneralizedCoordinates,bases<msPhysicalInterface>,boost::shared_ptr<msGeneralizedCoordinates> >
            ("msGeneralizedCoordinates",
             "a container for generalized coordinates",no_init
             )
            .def( "New", &msGeneralizedCoordinates::New , "Create a new object. Arg1: msUnitsManager object")
            .staticmethod("New")
            .def( "addVar" , &msGeneralizedCoordinates::addVar , "Add a variable to the generalized coordinates")
            .def( "value" , &msGeneralizedCoordinates::value , " Set and return a coordinate value from a [0,1] range index. Arg1: indice of the variable, Arg2: range index (0=Min value, 1=Max value)")
            .def( "setCoordinate" , &msGeneralizedCoordinates::setCoordinate ,
                 "set a coordinate's value. arg2: indice, arg3: value")
            .def( "setCoordinates" , &msGeneralizedCoordinates::setCoordinates ,
                 "set a coordinates' values. arg1: values")
            .def( "setMinValues", &msGeneralizedCoordinates::setMinValues,
                 "set the value of unfreezed coordinates to their min value.")
            .def( "setRandom",  &msGeneralizedCoordinates::setRandom,
                 "set a random value to the coordinates")
            .def( "getVariable", (boost::shared_ptr<msScalarVariable>(msGeneralizedCoordinates::*)(size_t) const) &msGeneralizedCoordinates::getVariable,
                 "return a variable. arg2: indice of the variable")
            .def( "getVariables",  &msGeneralizedCoordinates::getVariables,
                 "return the variables in a vector.")
            .def( "noOfActive",  &msGeneralizedCoordinates::noOfActive,
                 "return the number of active variables.")	
	    .def( "getActives",  &msGeneralizedCoordinates::getActives,
                 "return the actives variables in a vector.")	
            .def( "__getitem__",(boost::shared_ptr<msScalarVariable>(msGeneralizedCoordinates::*)(size_t) const) &msGeneralizedCoordinates::getVariable,
		         boost::python::arg( "index" ))
            .def(self_ns::str(self_ns::self));
            
	    msTreeMapper::finalizeDeclarationForPython<msGeneralizedCoordinates>("msGeneralizedCoordinates");
            
            msGeneralizedCoordinates::isGeneralizedCoordinatesRegisteredInPython=1;
        }
#endif
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msGeneralizedCoordinates::addVar(boost::shared_ptr<msScalarVariable> var)
    {
        LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msGeneralizedCoordinates::addVar(boost::shared_ptr<msScalarVariable> var)",getFullId())
        msChildren<msScalarVariable>::iterator it=Variables.begin();
        
        for(;it!=Variables.end();++it)
            if( (*it)->getId()==var->getId())
                BOOST_THROW_EXCEPTION( msError("can not add the variable: variable of same Id already exists"
                                               ,"boost::shared_ptr<msTreeMapper> msGeneralizedCoordinates::addVar(boost::shared_ptr<msScalarVariable> var)"
                                               ,getFullId()) );
        
        if(!var->hasParent()) var->setAffiliation(mySharedPtr());
        msTreeMapper::addElementToChildren<msScalarVariable>(Variables,var);
        LOGGER_EXIT_FUNCTION();
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    std::vector< boost::shared_ptr<msScalarVariable> > msGeneralizedCoordinates::getVariables() const {
        
        msChildren<msScalarVariable>::const_iterator it=Variables.begin();
        std::vector< boost::shared_ptr<msScalarVariable> > ChildrenForIteration;
        
        for(;it!=Variables.end();++it) ChildrenForIteration.push_back((*it));
        
        return ChildrenForIteration;
    }
    
    vector<boost::shared_ptr<msScalarVariable> > msGeneralizedCoordinates::getActives() const {
      
        msChildren<msScalarVariable>::const_iterator it=Variables.begin();
        std::vector< boost::shared_ptr<msScalarVariable> > actives;
        
        for(;it!=Variables.end();++it) {
	  
	    if( (*it)->getdq()!=0) actives.push_back((*it));
	}
        
        return actives;
    }
	    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    size_t msGeneralizedCoordinates::noOfDim() const { return Variables.size(); };
    
    size_t msGeneralizedCoordinates::noOfActive() const {
        
        int d=noOfDim();
        msChildren<msScalarVariable>::const_iterator it=Variables.begin();
        for(;it!=Variables.end();++it) if( (*it)->dq==0) d--;
        return d;
    }
    
    size_t msGeneralizedCoordinates::getIntegrationDim() const {
        
        size_t d=noOfDim();
        msChildren<msScalarVariable>::const_iterator it=Variables.begin();
        for(;it!=Variables.end();++it) if( (*it)->Dq==0) d--;
        return d;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    boost::shared_ptr<msScalarVariable> msGeneralizedCoordinates::getVariable(std::string str) const {
        
        msChildren<msScalarVariable>::const_iterator it=Variables.begin();
        
        for(;it!=Variables.end();++it)
            if( (*it)->getId() == str ) return (*it);
        
        LOGGER_ENTER_FUNCTION("boost::shared_ptr<msScalarVariable> msGeneralizedCoordinates::getVariable(std::string str)",getFullId());
        LOGGER_WRITE(msLogger::ERROR,"variable "+str+" not found");
        LOGGER_EXIT_FUNCTION();
        return boost::shared_ptr<msScalarVariable>(new msScalarVariable());
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    boost::shared_ptr<msScalarVariable> msGeneralizedCoordinates::getVariable(size_t i) const {
        
        if( i<Variables.size() ) return Variables[i].getSharedPtr();
        
        std::stringstream out;
        out<<"Try to access variable out of range (i="<<i<<", ndof="<<Variables.size()<<")";
        BOOST_THROW_EXCEPTION( msError(out.str(),
                                       "boost::shared_ptr<msScalarVariable> msGeneralizedCoordinates::getVariable(size_t i)",getFullId()
                                       )
                              );
        return boost::shared_ptr<msScalarVariable>();
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    void msGeneralizedCoordinates::getUnfreezedValues(const msUnitsManager& units, vector_type& vec) const {
        
        msGeneralizedCoordinates::const_UnfreezIt qi = beginUnfreezed();
        vec.resize(noOfActive());
        
        for(int i=0; qi!=endUnfreezed(); ++qi,++i) {
            
            vec[i] = (*qi).getValue(units);
        }
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msGeneralizedCoordinates::setValue( iterator qi, double v, const msUnitsManager& units) {
        
        (*qi)->setValue(v , units);
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msGeneralizedCoordinates::setMinValues(){
        
        msGeneralizedCoordinates::UnfreezIt qi = beginUnfreezed();
        for(; qi!=endUnfreezed(); ++qi) (*qi).setValue( (*qi).getMin() );
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msGeneralizedCoordinates::setUnfreezedValues(const msUnitsManager& units, const vector_type& vec) {
        
        msGeneralizedCoordinates::UnfreezIt qi = beginUnfreezed();
        for(int i=0; qi!=endUnfreezed(); ++qi,++i) {
            
            (*qi).setValue(vec[i],units);
        }
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msGeneralizedCoordinates::setCoordinate(size_t i,double val)
    {
        if( i >= Variables.size() )
            BOOST_THROW_EXCEPTION( msError("indice 'i' out of bounds."
                                           ,"boost::shared_ptr<msTreeMapper> msGeneralizedCoordinates::setCoordinate(size_t i,double val)",getFullId()) );
        getVariable(i)->Value = val;
        return mySharedPtr();
    }
    
    
    boost::shared_ptr<msTreeMapper> msGeneralizedCoordinates::setCoordinates(const vector<double>& val){
        
        for(size_t i=0;i<min(Variables.size(),val.size());++i)
            getVariable(i)->Value = val[i];
        return mySharedPtr();
    }
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    void msGeneralizedCoordinates::setRandom() {
        
        msGeneralizedCoordinates::UnfreezIt qi = beginUnfreezed();
        
        for(int i=0; qi!=endUnfreezed(); ++qi,++i)
            
            (*qi).setValue( (*qi).getMin() + ((*qi).Max-(*qi).getMin())*( (double) rand() /(RAND_MAX)));
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    double msGeneralizedCoordinates::getdq(const_iterator q, const msUnitsManager& units) const  {
        
        return (*q)->getdq(units);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    double msGeneralizedCoordinates::getValue(const_iterator q, const msUnitsManager& units) const  {
        
        return (*q)->getValue(units);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    double msGeneralizedCoordinates::getMin(const_iterator q, const msUnitsManager& units) const  {
        
        return (*q)->getMin(units);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    double msGeneralizedCoordinates::getMax(const_iterator q, const msUnitsManager& units) const  {
        
        return (*q)->getMax(units);
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    double msGeneralizedCoordinates::getVolumeDq() const {
        
        double V=1;
        msChildren<msScalarVariable>::const_iterator it=Variables.begin();
        
        for(;it!=Variables.end();++it) if( (*it)->Dq != 0) V *= (*it)->dq;
        
        return V;
    }
    
    double msGeneralizedCoordinates::getVolumeDq(const msUnitsManager& units) const {
        
        double V=1;
        msChildren<msScalarVariable>::const_iterator qi=Variables.begin();
        
        for(;qi!=Variables.end();++qi) if( (*qi)->Dq != 0)  V *= (*qi)->getDq(units);
        return V;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    std::ostream& msGeneralizedCoordinates::writePosAndField(std::ostream& out,double value) {
        
        writePos(out); out<<value<<std::endl;
        return out;
    }
    std::ostream& msGeneralizedCoordinates::writePos(std::ostream& out) {
        
        msChildren<msScalarVariable>::iterator it=Variables.begin();
        for(;it!=Variables.end();++it) out<<(*it)->Value<<" ";
        return out;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    std::ostream& msGeneralizedCoordinates::print(std::ostream& out) const {
        
        msPhysicalInterface::print(out);
        output::printHeader(out,"Generalized coordinates");
        
        return abstract(out);
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    std::ostream& msGeneralizedCoordinates::abstract(std::ostream& out) const {
        
        if(  noOfDim() != 0) {
            
            out<<setw(15)<<"Name"<<setw(15)<<"Value"<<setw(25)<<"Range"<<setw(15)<<"dq(deriv.)"<<setw(15)<<"Dq(int.)"<<setprecision(8)<<endl;
            msGeneralizedCoordinates::const_iterator it = begin();
            for(;it!=end();++it) {
                
                stringstream range;
                range<<"["<<(*it)->Min<<":"<<(*it)->Max<<"]";
                out<<setw(15)<<(*it)->getId()<<setw(15)<<(*it)->Value<<setw(25)<<range.str()<<setw(15)<<(*it)->dq<<setw(15)<<(*it)->Dq<<setprecision(8)<<endl;
            }
        }
        else {
            out<<"no variables included, you can add them using the 'add' function"<<endl; 
        }
        return out;
    }
	
    
    /*
     msGeneralizedCoordinates::UnfreezedIterator msGeneralizedCoordinates::beginUnfreezed()
     {
     msGeneralizedCoordinates::UnfreezedIterator it(this);
     return it;
     }*/
    
}

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

#include "msVectorFit1d.h"



namespace impact
{
    
    
    bool msVectorFit1d::isVectorFit1dRegisteredInPython=0;
    
    msRegistrar msVectorFit1d::Registrar("msVectorFit1d", msVectorFit1d::createInstance);
    
    std::ostream& operator<<(std::ostream& out,const msVectorFit1d& fit)
    {
        fit.print(out);
        return out;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msVectorFit1d::registryInPython()
    {
#if USE_PYTHON
        msScalarFunction::registryInPython();
        
        if( ! msVectorFit1d::isVectorFit1dRegisteredInPython ) {
            
            using namespace boost::python;
            
            class_<msVectorFit1d,bases<msScalarFunction>,boost::shared_ptr<msVectorFit1d> >
            ("msVectorFit1d",
             "A vector with an interpolation method, also a scalar function",no_init )
            .def( "New", &msVectorFit1d::New , "Create a new object")
            .staticmethod("New")
            .def( "reset", &msVectorFit1d::reset, 
		  "Clear the vector and set all its values to the target. arg2: target" )
	    .def( "setVariable", &msVectorFit1d::setVariable,
		  "Set the x variable. arg2: variable; arg3: y unit.")
            .def( "setValue", &msVectorFit1d::setValue,
		  "Set a particular vector's value corresponding to the current coordinate position. arg2: value");
            msVectorFit1d::isVectorFit1dRegisteredInPython=1;
        }
#endif
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msVectorFit1d::set(double min,double max,double d, string xUnit, string Yunit) {
		
        if( d<0 ){
            
            stringstream out;
            out<<"min="<<min<<", max="<<max<<", d="<<d;
            BOOST_THROW_EXCEPTION( msError("the delta step is <0. "+out.str(),
                                           "void msVectorFit1d::set(double min,double max,double d, string xUnit, string Yunit)",getFullId())
                                  );
        }
        if( max<min ){
            stringstream out;
            out<<"min="<<min<<", max="<<max<<", d="<<d;
            BOOST_THROW_EXCEPTION( msError("the minimum > maximum. "+out.str(),
                                       "void msVectorFit1d::set(double min,double max,double d, string xUnit, string Yunit)",getFullId())
                              );
        }
        (*getCoordinates())[0]->set(min,min,max,d,d,xUnit);
        resize((max-min)/d,0);
        setYUnit(Yunit);
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------

    void msVectorFit1d::setVariable(boost::shared_ptr<msScalarVariable> v, string Yunit) {
        
        getCoordinates()->setVariable(0,v);
        resize((*getCoordinates())[0]->noOfBins(),0);
        setYUnit(Yunit);
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    void msVectorFit1d::set(msScalarVariable& v, string Yunit) {
        
        (*getCoordinates())[0]->set( v.getMin(), v.getMin(), v.getMax(), v.getDq(), v.getDq(), v.getUnitStr() );
        (*getCoordinates())[0]->setId(v.getId());
        resize((*getCoordinates())[0]->noOfBins(),0);
        setYUnit(Yunit);
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msVectorFit1d::setValues(vector<double>& y_) {
        
        if( size()==0 )
        BOOST_THROW_EXCEPTION( msError("You need to call a 'set' function before calling setValues"
                                       ,"boost::shared_ptr<msTreeMapper> msVectorFit1d::setValues(vector<double>& y_)",getFullId()) );
        
        for( size_t i=0; i<size();i++)  (*this)[i]=y_[i];
        
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    /*
     boost::shared_ptr<msTreeMapper> msVectorFit1d::setValues(string str) {
     
     vector<double> vec;
     fillVecFromString(str,vec," ");
     return setValues(vec);
     }
     */
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msVectorFit1d::setValue(double y){
        
        size_t indice =  ( (*getCoordinates())[0]->getValue() - (*getCoordinates())[0]->getMin() ) / (*getCoordinates())[0]->getDq();
        
        if( indice> size())
        BOOST_THROW_EXCEPTION( msError("the current value of the coordinate is out of bound for the vector, try to reset the vector or check the coordinate's value."
                                       ,"boost::shared_ptr<msTreeMapper> msVectorFit1d::setValue(double y)",getFullId()) );
        (*this)[indice]=y;
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    boost::shared_ptr<msTreeMapper> msVectorFit1d::reset( double v )  {
        
        clear();
        resize((*getCoordinates())[0]->noOfBins(),v);
        return mySharedPtr();
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    double msVectorFit1d::evaluate() {
        
        double r   = -1;
        double t   = (*getCoordinates())[0]->getValue();
        double dx  = (*getCoordinates())[0]->getDq();
        double min = (*getCoordinates())[0]->getMin();
        double x   = min;
        if( (*getCoordinates())[0]->isOutOfBounds() )
        
        BOOST_THROW_EXCEPTION( msError(" Coordinates out of range:  msVectorFit1d can not be used for extrapolation."
                                       ,"double msVectorFit1d::evaluate()",getFullId()) );
        
        for(size_t i=0;i<(*this).size();i++) {
            
            x = min + i*dx;
            if( (x>t)&&(i<(*this).size()-1) ){    r=Interpol(t,x-dx,(*this)[i-1],x, (*this)[i],x+dx, (*this)[i+1]);
                i=(*this).size();
            }
            if(    i==size()-1 )                  r=Interpol(t,x-2*dx,   (*this)[i-2],x-dx, (*this)[i-1],x,  (*this)[i]);
        }
        return r;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    double msVectorFit1d::max(){
        
        double max_=(*this)[0];
        for( size_t i=0; i<size();i++)  if( (*this)[i]>max_) max_=(*this)[i];
        return max_;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    double msVectorFit1d::Interpol(double t,double x0,double v0,double x1,double v1,double x2,double v2) {
        
        if( (x0==x1) || (x0==x2) || (x1==x2) )
        BOOST_THROW_EXCEPTION( msError("Two x-axis points are superposed: can not interpolate",
                                       "double msVectorFit1d::Interpol(double t,double x0,double v0,double x1,double v1,double x2,double v2)",
                                       getFullId()
                                       )
                              );
        double r=v0+(t-x0)*((v1-v0)/(x1-x0)+(t-x1)*(-(v1-v0)/(x1-x0)+(v2-v1)/(x2-x1))/(x2-x0));
        
        return r;
    }
    
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    
    std::ostream& msVectorFit1d::print(std::ostream& out) const { msScalarFunction::print(out); return out;
    }
    
    
}

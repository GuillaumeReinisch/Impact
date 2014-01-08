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
/*!
 * \file c_msVectorFit1d
 * \brief 1D scalar function from data fit object definition
 * \author G.Reinisch
 * \date 2011
 */
#ifndef msmsVectorFit1d_H
#define msmsVectorFit1d_H
#include <msScalarFunction.h>

namespace impact
{
    
    /*! \class msmsVectorFit1d
     *  \brief fit a 1D vector of data point
     *
     * This class interpolate {x,y} data to produce a 1D function between the minimum and maximum of the given x values.
     * A second order interpolation is used.
     *
     */
    class msVectorFit1d : public msScalarFunction , public vector<double>
    {
        
    private:
        
        //!\name from msRegister
        //@{
        static msRegistrar Registrar;
        static bool  isVectorFit1dRegisteredInPython;
        static msRegister* createInstance() { return new msVectorFit1d; }
        //@}
        
    protected:
        
        void registryInPython();
        
        void update(){ msScalarFunction::update(); }
        
    public:
        
        void initialize()  {
            msScalarFunction::initialize();
            declareAttribute(*((vector<double>*)this),"y");
            getCoordinates()->addVar(msScalarVariable::New());
        };
        
        static boost::shared_ptr<msVectorFit1d> New(){
            
            LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msVectorFit1d> msVectorFit1d::New(string unitexpr)","");
            LOGGER_WRITE(msLogger::DEBUG, "New msVectorFit1d object");
            boost::shared_ptr<msVectorFit1d> T( new msVectorFit1d );
            T->initialize();
            T->update();
            LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msVectorFit1d> msVectorFit1d::New(string unitexpr)");
            return T;
        }
        
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msVectorFit1d> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        msVectorFit1d () : msScalarFunction() ,vector<double>() { constructVar("msVectorFit1d","VectorFit1d","fit of 1D data");
        }
        
        void set(double min,double max,double d, string xUnit, string Yunit);
        
        void set(msScalarVariable& v, string Yunit);
	
        void setVariable(boost::shared_ptr<msScalarVariable> v, string Yunit);
        
        //  boost::shared_ptr<msTreeMapper> setValues(string str);
        
        boost::shared_ptr<msTreeMapper> setValues(vector<double>& y_);
        
        boost::shared_ptr<msTreeMapper> setValue(double y);
        
        boost::shared_ptr<msTreeMapper> reset( double v );
        
        double evaluate();
        
        double max();
        
        std::ostream& print(std::ostream& out) const;
        
    private:
        
        double Interpol(double t,double x0,double v0,double x1,double v1,double x2,double v2);
        
    };
    
    std::ostream& operator<<(std::ostream&,const msVectorFit1d&);
    
}
#endif // msFCT1DFROMDATA_H

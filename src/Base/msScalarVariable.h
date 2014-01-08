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


#ifndef MSSCALVARIABLE_H
#define MSSCALVARIABLE_H


#include <msTreeMapper.h>
#include <msUnitsManager.h>

namespace impact
{
    /*! \class msScalarVariable
     *
     *  \brief Represents a scalar variable
     *
     * A scalar variable define a value, a range, an integration step,
     * a derivation step, and a unit (msUnit). The integration step (Dq) 
     * is used when integration over the variable is computed, the derivation
     * step (dq) is used when derivation w/ variable is computed.
     *
     * You can access the value via the msScalarVariable::getValue methods, scan the variable
     * using the methods msScalarVariable::begin, msScalarVariable::increment,
     * msScalarVariable::decrement.
     * ~~~~~~~~~~~~~{.py}
     from libimpact import *
     import itertools
     
     
     var = msScalarVariable.New()
     var.set(0,-10,10,0.001,0.1)
     
     print var
     # scan from the current value (=0) to the max (=10) using the Dq (=0.1) value
     while var.increment():
         print var.getValue()
     * ~~~~~~~~~~~~~

     */
    class msScalarVariable : public msTreeMapper
    {
        friend class msGeneralizedCoordinates;
        
        /** @Name From msRegister
         *///@{
        static msRegistrar Registrar;
        static bool  isScalarVariableRegisteredInPython;
        static msRegister* createInstance() { return new msScalarVariable; }
        //@}
        
    protected:
        
        void registryInPython();
        
        void initialize() {
            
            msTreeMapper::initialize();
            declareAttribute(Min,"Min"); declareAttribute(Value,"Value");
            declareAttribute(Max,"Max"); declareAttribute(dq,"dq");
            declareAttribute(Dq,"Dq");
            msTreeMapper::declareChild<msUnit>(Unit,msUnit::New(),"Unit");
        };
        
    public:
        
        msScalarVariable() : msTreeMapper() {
            
            constructVar("msScalarVariable","ScalarVariable","q");
            Min=Max=dq=Dq=Value=0;
        }
        //   enum typeVar{ length, angle};
        
        static boost::shared_ptr<msScalarVariable> New()  {
            
            LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msScalarVariable> msScalarVariable::New()","");
            LOGGER_WRITE(msLogger::DEBUG, "New msScalarVariable object")
            boost::shared_ptr<msScalarVariable> T( new msScalarVariable );
            
            T->initialize();
            T->update();
            LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msScalarVariable> msScalarVariable::New()");
            return T;
        }
        
        virtual boost::shared_ptr<msTreeMapper> clone() {
            boost::shared_ptr<msScalarVariable> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        boost::shared_ptr<msTreeMapper> set(double v,double min,double max,double _dq,double _Dq)
        {
            return set(v, min, max, _dq, _Dq , Unit->getStr() );
        }
        
        boost::shared_ptr<msTreeMapper> set(double v,double min,double max,double _dq,double _Dq, string unit)
        {
            Min=min;Max=max;dq=_dq;Dq=_Dq;Value=v;Default=v;
            
            try{ Unit->set( unit );
            }
            catch(msError& e){
                e.addContext("can not set the unit of the variable (boost::shared_ptr<msTreeMapper> set(double v,double min,double max,double _dq,double _Dq, string unit))");
                throw e;
            }
            return mySharedPtr();
        }
        
        double begin(){return Value=Min;}
        
        bool increment(){ Value+=Dq; if(Value>Max) return 0; return 1; }
        
        bool decrement(){ Value-=Dq; if(Value<Min) return 0; return 1; }
        
        bool isOutOfBounds() {if( (Value>Max)||(Value<Min)) return 1;return 0;}
        
        double value(double x) {return Value=Min+x*(Max-Min); }
        
        double getValue(const msUnitsManager& units) const	{ return units.convert(*Unit,Value); };
        double getMin(const msUnitsManager& units)   const	{ return units.convert(*Unit,Min);   };
        double getMax(const msUnitsManager& units)   const	{ return units.convert(*Unit,Max);   };
        double getDq(const msUnitsManager& units)    const	{ return units.convert(*Unit,Dq);    };
        double getdq(const msUnitsManager& units)    const	{ return units.convert(*Unit,dq);    };
        
        //! should not be used ... but sometimes there is no choice
        double* getPtrOnValue()  { return &Value;
        }
        //! \brief return the value
        double getValue() 	{ return Value; };
        
        //! \brief return the minimum
        double getMin()   	{ return Min; };
        
        //! \brief return the maximum
        double getMax()   	{ return Max; };
        
        //! \brief return the integration step
        double getDq()    	{ return Dq; };
        
        //! \brief return the derivation step
        double getdq()    	{ return dq; };
        
        //! \brief set the value
        virtual boost::shared_ptr<msTreeMapper> setValue(double v) 	{ Value = v; return mySharedPtr(); };
        
        //! \brief set the minimum
        boost::shared_ptr<msTreeMapper> setMin(double v)   	{ Min   = v; return mySharedPtr();};
        
        //! \brief set the maximum
        boost::shared_ptr<msTreeMapper> setMax(double v)   	{ Max   = v; return mySharedPtr();};
        
        //! \brief set the integration step
        boost::shared_ptr<msTreeMapper> setDq(double v)    	{ Dq    = v; return mySharedPtr(); };
        
        //! \brief set the derivation step
        boost::shared_ptr<msTreeMapper> setdq(double v)    	{ dq    = v; return mySharedPtr();};
        
        
        boost::shared_ptr<msTreeMapper> setValue(double v, const msUnitsManager& units) 	{
            
            Value = v / units.convert(*Unit,1);
            return mySharedPtr();
        };
        
        boost::shared_ptr<msTreeMapper> setMin(double v, const msUnitsManager& units)   	{
            
            Min   = v / units.convert(*Unit,1);
            return mySharedPtr();
        };
        
        boost::shared_ptr<msTreeMapper> setMax(double v, const msUnitsManager& units)   	{
            
            Max   = v / units.convert(*Unit,1);
            return mySharedPtr();
        };
        
        boost::shared_ptr<msTreeMapper> setDq(double v, const msUnitsManager& units)    	{
            
            Dq    = v / units.convert(*Unit,1);
            return mySharedPtr();
        };
        
        boost::shared_ptr<msTreeMapper> setdq(double v, const msUnitsManager& units)    	{
            
            dq    = v / units.convert(*Unit,1);
            return mySharedPtr();
        };
        
        void setUnit(string unit){
            try{ Unit->set( unit );
            }
            catch(msError& e){
                e.addContext("can not set the unit of the variable (boost::shared_ptr<msTreeMapper> set(double v,double min,double max,double _dq,double _Dq, string unit))");
                throw e;
            }
        }
        int indice(){ return (Value-Min)/Dq + 0.5;
        };
        
        double operator()(){return Value;}
        
        std::string getUnitStr(){ return Unit->getStr(); }
        
        boost::shared_ptr<msUnit> getUnit() {return Unit.getSharedPtr();}
        
        size_t noOfBins(){ return (Max-Min)/Dq; }
        
        double Default;
        
        bool isHomogeneousWith(std::string u) { return Unit->isConsistent( msUnit(u) ); }
        
        std::ostream& print(std::ostream& out) const;
        
        std::ostream& abstract(std::ostream& out) const;
        
    protected:
        
        msChild<msUnit> Unit;
        
    private:
        
        double Value;  //!< Current value
        
        double Min;    //!< Minimum value
        
        double Max;    //!< Maximum value
        
        double dq;     //!< derivation step, if dq==0 the variable is called inactive
        
        double Dq;     //!< integration step, if Dq==0 the variable is called freezed
    };
    
    
    
}

#endif // MSSCALVARIABLE_H

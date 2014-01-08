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
 * \file c_scalfunction.h
 * \brief Defines the class msScalarFunction
 * \author G.Reinisch
 * \date 2011
 */
#ifndef msSCALFUNCTION_H
#define msSCALFUNCTION_H

#include <msGeneralizedCoordinates.h>

#include <boost/exception/all.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_array.hpp>
#include <exception>
#include <map>



namespace impact
{
    typedef boost::error_info<struct tag_errmsg, std::string> errmsg_info;
    
    class msSampler;
    
    /*! \class msScalarFunction
     * \brief Base class of scalar functions
     *
     * This virtual class implements the definition of a scalar function:
     * it associates to a generalized coordinates container a scalar field.
     * Typical use of msScalarFunction objects is by generating a configuration
     * to the generalized coordinates (for instance using msSampler derived classes),
     * and then evaluate the function using impact::msScalarFuncion::evaluate().
     * An unit (msUnit) is associated to the return type and may be check to ensure
     * units consistency.
     * A constants list can be filled (msScalarFunction::setConstant) and used by the derived classe when
     * msScalarFunction::evaluate() is called.
     *
     * msScalarFunction implements numerical definition of Jacobian and Hessians function,
     * the differentiation is computed using the msScalarVariable::dq member.
     * The integration of scalar functions is realized by msSampler derived objects.
     *
     * The functions msScalarFunction::getXSample1d  and msScalarFunction::getYSample1d return python lists of 1D sample
     * along specific coordinates to allow direct plotting.
     *
     * example:
     * ~~~~~~~~~~~~~{.py}
     from libimpact import *
     
     x = msScalarVariable.New().set(0,0,50,0.5,0.5)
     y = msScalarVariable.New().set(0,0,50,0.5,0.5)
     coors = msGeneralizedCoordinates.New().addVar(x).addVar(y)
     
     fct = msScalarFunction.New()
     fct.setCoordinates(coors)
     fct.setYUnit("kg")
     fct.setConstant("a0",3)
     
     fct.evaluate() # raise an exception as the method evaluate is virtual
     
     * ~~~~~~~~~~~~~
     
     * \section derivScalarFunction Derivation of new scalar function classes
     *
     * The derivation of new classes from msScalarFunction required the definition
     * of the virtual method evaluate().
     *
     * If analytic formulations of Jacobian or Hessians are possible you should
     * re-implement the associated functions:
     *    - partialDerivative(size_t i) : partial derivation w/ coordinate i
     *    - partialSecondDerivative(size_t i, size_t j) : partial second derivation w/
     * coordinates i and j
     *
     * The function getLabel() may also be redefined.
     */
    class msScalarFunction : public msTreeMapper
    {
    private:
        
        //!\name from msRegister
        //@{
        static msRegistrar Registrar;
        static bool  isScalarFunctionRegisteredInPython;
        static msRegister* createInstance() { return new msScalarFunction; }
        //@}
        msScalarFunction& operator=(msScalarFunction&){ return *this;};
        
    protected:
        
        void registryInPython();
        
    public:
        
        msScalarFunction() : msTreeMapper(){
            constructVar("msScalarFunction","ScalarFunction","scalar function");
        }
        
        static boost::shared_ptr<msScalarFunction> New(){
            
            LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msScalarFunction> msScalarFunction::New()","");
            LOGGER_WRITE(msLogger::DEBUG, "New msScalarFunction object")
            
            boost::shared_ptr<msScalarFunction> T( new msScalarFunction );
            T->initialize();
            T->update();
            LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msScalarFunction> msScalarFunction::New()");
            return T;
        }
        virtual boost::shared_ptr<msTreeMapper> clone() {
            
            boost::shared_ptr<msScalarFunction> clone = New();
            clone->msTreeMapper::operator=(*this);
            return clone;
        }
        
        void initialize()  {
            
            msTreeMapper::initialize();
            declareAttribute< std::map< std::string , double > >( Constants, "Constants" );
            declareChild<msGeneralizedCoordinates>(Coordinates, msGeneralizedCoordinates::New(),
                                                   "Coordinates");
            declareChild<msUnit>( Unit, msUnit::New(), "Unit" );
        };
        
        virtual void update()  {
            LOGGER_ENTER_FUNCTION_DBG("void msScalarFunction::update()",getFullId());
            msTreeMapper::update();LOGGER_EXIT_FUNCTION();
        };
        
        //! set the coordinates
        virtual boost::shared_ptr<msTreeMapper> setCoordinates(boost::shared_ptr<msGeneralizedCoordinates> coor);
        
        //! return the dimension of the coordinates
        size_t noOfDim() const { return Coordinates->noOfDim();}
        
        //! return the variable number 'i', throw an exception if out of bound
        boost::shared_ptr<msScalarVariable> getVariable(size_t i){
            
	     	if( i >= Coordinates->noOfDim() )
                BOOST_THROW_EXCEPTION( msError("Coordinates of the function not initialized (did you call msScalarFunction::initialize()?)"
                                               ,"boost::shared_ptr<msScalarVariable> getVariable",getFullId()) );
            return (*Coordinates)[i];
        }
        //! return the coordinates
        boost::shared_ptr<msGeneralizedCoordinates> getCoordinates() const {
            
            if(Coordinates == boost::shared_ptr<msGeneralizedCoordinates>()){
                BOOST_THROW_EXCEPTION( msError("Coordinates of the function not initialized (did you call msScalarFunction::initialize()?)"
                                               ,"const boost::shared_ptr<msGeneralizedCoordinates> msScalarFunction::getCoordinates() const",getFullId()) );
            }
            return Coordinates.getSharedPtr();
        }
        
        //! set the unit of returned value
        void setYUnit(string expr) {
            
            if( Unit.getSharedPtr() == boost::shared_ptr<msUnit>() )
                BOOST_THROW_EXCEPTION( msError("The Unit attribut has not been initialized (did you call msScalarFunction::initialize() before using it?)"
                                               ,"void msScalarFunction::setYUnit(string expr)",getFullId()) );
            
            Unit->set(expr);
        }
        
        //! return the unit of the return value
        msUnit& getYUnit() const { return *(Unit.get());
        }
        
        boost::shared_ptr<msUnit> getUnit() const { return Unit.getSharedPtr();
        }
        //! return a short descriptive label
        virtual string getLabel(){return ("Id: "+getId()+"; type:"+getType());}
        
        
        //! \name managment of the constants
        //@{
        //! add the definition of the constant, 'key': id of the constant, 'value': value
        virtual boost::shared_ptr<msTreeMapper> setConstant( std::string key , double value ) {
            Constants[key]=value; return mySharedPtr();
        };
        
        //! return 1 if the constant of id 'name' exist
        bool existConstant(string name) {
            
            return Constants.find(name)!=Constants.end();
        }
        
        //! return a reference to the constant value of id 'name', throw an exception if not defined
        double& getConstant(string name) {
            
            if(existConstant(name)) return Constants[name];
            BOOST_THROW_EXCEPTION(msError("Constants "+name+" does not exist",
                                          "msScalarFunction::getConstant",
                                          getFullId()));
        }
        
        //! return the constants defined in a vector of string. Each item is like: "key : value"
        std::vector<std::string> getConstants() const {
            
            std::vector<std::string> strs;
            for(std::map< std::string , double >::const_iterator it=Constants.begin();
                it!=Constants.end();++it) {
                
                stringstream item;
                item << it->first<<" : "<<it->second;
                strs.push_back(item.str());
            }
            return strs;
        }
        
        std::map< std::string , double >::iterator beginConstants() {
            return Constants.begin();
        }
        std::map< std::string , double >::iterator endConstants() {
            return Constants.end();
        }
        //@}
        std::ostream& print(std::ostream& out) const;
        
        virtual std::ostream&  abstract(std::ostream& out) const {out<<"no abstract providen.";return out;}
        
    private:
        
        msChild<msUnit> Unit;
        
        msChild<msGeneralizedCoordinates> Coordinates;
        
    protected:
        
        std::map< std::string , double >  Constants; //!< map defining the constants
        
    public:
        
        //! The virtual function to overide in derived classes: return the scalar field at the current coordinates
        virtual double evaluate()  {
            BOOST_THROW_EXCEPTION(msError( "This method is virtual, you need to overide it",
                                          "double msScalarFunction::evaluate()",
                                          getFullId()
                                          )
                                  );
        }
        
        //! \name evaluate methods, all set the coordinates and call evaluate()
        //@{
        //! from a vector
        double evaluate(const vector<double>& xs);
        
        /** \brief from an array pointer
         *
         * the array as to be at least as large as the number of coordinates, otherwise
         * an exception is thrown.
         */
        double evaluate(double* xs);
        
        /** \brief from a value
         *
         * Interpret x as the first coordinates.
         * The coordinates has to be of dim>0, otherwise an exception is thrown.
         */
        double evaluate(double x);
        
        /** \brief from two values
         *
         * Interpret x as the first coordinates, and y as the second.
         * The coordinates has to be of dim>1, otherwise an exception is thrown.
         */
        double evaluate(double x,double y);
        
        /** \brief from two values
         *
         * Interpret x as the first coordinates, y as the second, and z as the third.
         * The coordinates has to be of dim>2, otherwise an exception is thrown.
         */
        double evaluate(double x,double y,double z);
        
        //! \copydoc evaluate(double* xs);
        inline double operator()(double* x)			{ return evaluate(x);	 }
        
        //! \copydoc evaluate(const vector<double>& x);
        inline double operator()(const vector<double>& x)		{ return evaluate(x);	 }
        
        //! \copydoc evaluate(double x);
        inline double operator()(double x)			{ return evaluate(x);	 }
        
        //! \copydoc evaluate(double x, double y);
        inline double operator()(double x,double y)		{ return evaluate(x,y);	 }
        
        //! \copydoc evaluate(double x, double y, double z);
        inline double operator()(double x, double y, double z)	{ return evaluate(x,y,z);}
        
        //! \copydoc evaluate();
        inline double operator()()			 	{ return evaluate();	 }
        //@}
        
        //! \name jacobian and hessians methods
        //@{
        //! compute \f$ \frac{\partial f}{\partial dq_i} \f$, throw an exception if 'i' out of range
        virtual double partialDerivative(size_t i);
        
        //! compute \f$ \frac{\partial^2 f}{\partial dq_i\partial dq_j} \f$, throw an exception if 'i' or 'j' out of range
        virtual double partialSecondDerivative(size_t i,size_t j);
        
        //! compute \f$ \frac{\partial^2 f}{\partial dq_i\partial dq_i} \f$, throw an exception if 'i' out of range
        double partialSecondDerivative(size_t i){ return partialSecondDerivative(i,i); }
        //@}
        void save(size_t i,size_t n,string filename);
        
#if USE_PYTHON
        PyObject* getXSample1d(size_t i,size_t n) {
            
            vector<double> v;
            for( size_t j=0;j<n;j++) v.push_back( (*Coordinates)[i]->value(double(j)/n) );
            
            boost::python::list* l = new boost::python::list();
            for(size_t j = 0; j < v.size(); j++) (*l).append(v[j]);
            return l->ptr();
        }
        
        PyObject* getYSample1d(size_t i,size_t n) {
            
            vector<double> v;
            boost::shared_ptr<msScalarVariable> var = Coordinates->getVariable(i);
            for( size_t j=0;j<n;j++){
                
                var->setValue(var->getMin() + double(j)/double(n) * (var->getMax()-var->getMin()) );
                
                v.push_back( evaluate() );
            }
            
            boost::python::list* l = new boost::python::list();
            for(size_t j = 0; j < v.size(); j++) (*l).append(v[j]);
            return l->ptr();
        }
#endif
        double getEminByScan();
        
        
    };
    std::ostream& operator<<(std::ostream& out,const msScalarFunction& fct);
    
    
}
#endif // msSCALFUNCTION_H

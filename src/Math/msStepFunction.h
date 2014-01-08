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


#ifndef MSStepFunction_H
#define MSStepFunction_H


#include <msScalarFunction.h>

namespace impact {
    
    
    namespace math{
        
        /*!  \class msStepFunction
         *
         * A finite linear combination of indicator functions of intervals.
         * <a href="http://en.wikipedia.org/wiki/Step_function"> see Wikipedia </a>.
         * In addition to the definition of the coordinates, the construction required 
         * to provide the intervals and their values.
         *
         * example:
         * ~~~~~~~~~~~~~{.py}
         from libimpact import *
         
         mins = msVector_double()
         maxs = msVector_double()
         values = msVector_double()
         for i in range(0,10):
         mins.append(2*i)
         maxs.append(2*(i+1))
         values.append(i)
         
         x = msScalarVariable.New().set(0,0,50,0.5,0.5)
         coors = msGeneralizedCoordinates.New().addVar(x)
         fct = msStepFunction.New()
         
         fct.setCoordinates(coors)
         fct.setSteps(mins,maxs,values)
         
         while x.increment():
             print x.getValue(),fct.evaluate()

         * ~~~~~~~~~~~~~

         *
         */
        class msStepFunction : public msScalarFunction
        {
            
            static msRegistrar Registrar;
            static bool  isStepFunctionRegisteredInPython;
            static msRegister* createInstance() { return new msStepFunction; }
            
        protected:
            
            void registryInPython();
            
            void initialize()  {
                msScalarFunction::initialize();
                msTreeMapper::declareAttribute(Values,"Values");
                msTreeMapper::declareAttribute(Mins,"Mins");
                msTreeMapper::declareAttribute(Maxs,"Maxs");
                msTreeMapper::declareAttribute(IndexCoordinate,"IndexCoordinate");
            };
            
            void update(){  msScalarFunction::update(); }
            
        public:
            
            msStepFunction() : msScalarFunction(){
                
                constructVar("msStepFunction","msStepFunction","step function");
                IndexCoordinate=0;
            }
            
            static boost::shared_ptr<msStepFunction> New(){
                
                LOGGER_ENTER_FUNCTION_DBG("static boost::shared_ptr<msStepFunction> msStepFunction::New()","");
                LOGGER_WRITE(msLogger::DEBUG, "New msStepFunction object")
                
                boost::shared_ptr<msStepFunction> T( new msStepFunction );
                T->initialize();
                T->update();
                LOGGER_EXIT_FUNCTION2("static boost::shared_ptr<msStepFunction> msStepFunction::New()");
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msStepFunction> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            /*! \brief set the coordinate index.
             *
             * The index define the coordinate in the generalized coordinates used as
             * x variable.
             * \param index Index of the x-coordinate in the generalized coordinates
             */
            boost::shared_ptr<msTreeMapper> setCoordinateIndex(size_t index) {
                
                IndexCoordinate=index;
                return mySharedPtr();
            }
            
            /*!  \brief define the steps.
             *
             * \param mins Minimum of the domains
             * \param maxs Maximum of the domains
             * \param values Value in the domains
             */
            boost::shared_ptr<msTreeMapper> setSteps(vector<double> mins, vector<double> maxs,vector<double> values) {
                if( (mins.size()!=maxs.size())|| (values.size()!=maxs.size())){
                    
                    msError e("container does not have the same size.",
                              "boost::shared_ptr<msTreeMapper> msStepFunction::define(vector<double> mins, vector<double> maxs,vector<double> values)",
                              getFullId());
                    BOOST_THROW_EXCEPTION(e);
                }
                Values=values;Mins=mins;Maxs=maxs;
                return mySharedPtr();
            }
            
            double evaluate() {
                
                double r = 0;
                if( ! getCoordinates() )
                    BOOST_THROW_EXCEPTION(msError("Coordinates are not set, use the method setCoordinates.","double msStepFunction::evaluate()",getFullId()));
                
                if(IndexCoordinate>=getCoordinates()->noOfDim())
                    BOOST_THROW_EXCEPTION(msError("Index of the coordinate out of bounds the coordinates container ","double msStepFunction::evaluate()",getFullId()));
                
                double x = (*(getCoordinates()))[IndexCoordinate]->getValue(*(getCoordinates()->getUnits()));
                
                for(size_t i = 0; i<Mins.size(); i++ ) {
                    
                    if( (x>=Mins[i]) && (x<Maxs[i]) ){
                        r += Values[i];
                    }
                }
                return r;
            };
            
            ostream&  print(ostream& out) const {
                
                msScalarFunction::print(out);
                output::printHeader(out,"steps scalar function")<<"\n"<<endl;
                
                for(size_t i = 0; i<Mins.size(); i++ ) {
                    
                    out<<"["<<Mins[i]<<","<<Maxs[i]<<"[ :"<<Values[i]<<endl;
                }
                return out;
            }
            
        private:
            size_t IndexCoordinate;
            vector<double> Values;
            vector<double> Mins;
            vector<double> Maxs;
        };
        
    }
}

#endif
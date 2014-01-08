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


#ifndef MSBAYSIANScalarModel_H
#define MSBAYSIANScalarModel_H

#include <msPredictiveModel.h>
#include <msScalarFunction.h>

namespace impact {
    
    namespace queso {
        
        
        /*! \class msScalarModel 
         * \brief Predictive model that handle one quantity of interest
         *
         * This class implements a scalar predictive model: it predicts one 
         * (scalar) quantity of interest.
         * It is based on a scalar function object to predict the
         * QoI using the method msScalarFunction::evaluate().
         * The parameters of the model are identified to the constants defined
         * in the scalar function, the scenario variables to the generalized coordinates 
         * of the scalar function.
         *
         * To construct the scalar model you need to set the predictor,
         *  example:
         * ~~~~~~~~~~~~~{.py}
          from libimpact import *
         
          x = msScalarVariable.New().set(0,0,50,0.5,0.5).setId("q0")
          y = msScalarVariable.New().set(0,0,50,0.5,0.5).setId("q1")
          coors = msGeneralizedCoordinates.New().addVar(x).addVar(y)
         
          predictor = msParser.New()
          predictor.setCoordinates(coors)
          predictor.setConstant("a0",2)
          predictor.setExpression("a0*q0*q1")
         
          model = msScalarModel.New()
          model.setPredictor(predictor)
         
         * ~~~~~~~~~~~~~
         */
        class msScalarModel : public msPredictiveModel
        {
            
        protected:
            
            static msRegistrar Registrar;
            static bool  isScalarModelRegisteredInPython;
            static msRegister* createInstance() { return new msScalarModel; }
            
            void registryInPython();
            
        public:
            
            msScalarModel();
            
            void update()  {
                
                msPredictiveModel::update();
                
                map<string,double>::iterator it = Predictor->beginConstants();
                for( size_t i=0;  it!=Predictor->endConstants(); ++it ,++i) {
                    
                    
                    boost::shared_ptr<msModelParameter> ptr;
                    try {
                        ptr = boost::static_pointer_cast<msModelParameter>( getParameterSpace()->getVariable(i) );
                    } catch (...) {
                        
                        stringstream out;
                        out<<"can not cast the variable into a msModelParameter, "
                        <<"parameter of the model have to be of type msModelParameter, "
                        <<"type of the variable: "<< getParameterSpace()->getVariable(i)->getType();
                        msError e(out.str(),
                                  "void msScalarModel::update()",getFullId());
                        BOOST_THROW_EXCEPTION(e);
                    }
                    
                    ptr->addRefOnValue( Predictor->getConstant((*it).first) ) ;
                }
            };
            
            void initialize() {
                
                msPredictiveModel::initialize();
                declareChild<msScalarFunction>(Predictor,msScalarFunction::New(),"Predictor");
            };
            
            static boost::shared_ptr<msScalarModel> New(){
                
                boost::shared_ptr<msScalarModel> T( new msScalarModel );
                T->initialize();
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msScalarModel> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            /*! \brief predict the quantity of interest 
             * 
             * Compute the quantity of interest and put it in as first element of
             * the Qoi vector (obtained using msPredictiveModel::getQoi(0)).
             */
            boost::shared_ptr<msTreeMapper> predict() {
                
                 msPredictiveModel::setQoi("QoI",Predictor->evaluate());
                 return mySharedPtr();
            };
            
            /*! \brief set the predictor: a scalar function
             * 
             * Set the scalar function that is used to compute the quantity of interest, its constants defined
             * are used as parameters. You have to define the range of each parameters by yourself after 
             * having call this function (by default it is value/2 to value*2). The scenario variables
             * are set to the generalized coordinates of the scalar function.
             *
             * \param predictor the scalar function predictor
             */
            boost::shared_ptr<msTreeMapper> setPredictor(boost::shared_ptr<msScalarFunction> predictor);
            
            
        private:
            
            msChild<msScalarFunction> Predictor;
            
        };
    }
    
}
#endif // MSDIRACS_H

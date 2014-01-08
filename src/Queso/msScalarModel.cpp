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

#include "msScalarModel.h"

namespace impact {
    
    namespace queso {
        
        
        bool msScalarModel::isScalarModelRegisteredInPython=0;
        
        msRegistrar msScalarModel::Registrar( "msScalarModel" , msScalarModel::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& operator<<(std::ostream& out,const msScalarModel& bayes) {
            
            return( bayes.print(out) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msScalarModel::registryInPython(){
            
#if USE_PYTHON
            msPhysicalInterface::registryInPython();
            
            if(  ! msScalarModel::isScalarModelRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msScalarModel,bases<msPredictiveModel>,boost::shared_ptr<msScalarModel> >
                ("msScalarModel",  "model that predicts one scalar quantity of interest. A scalar function is used as predictor, its defined constants as parameters.",init<>()
                 )
                .def( "New",&msScalarModel::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
                .def( "setPredictor",&msScalarModel::setPredictor ,
                     "Set the predictor. arg2: scalar function predictor" );
                
                msScalarModel::isScalarModelRegisteredInPython = 1 ;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msScalarModel::setPredictor(  boost::shared_ptr<msScalarFunction> predictor ) {
            
            LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msScalarModel::setPredictor(  boost::shared_ptr<msScalarFunction> predictor )"
                                      ,getFullId());
            
            msTreeMapper::update(Predictor,predictor);
            
            if( !Predictor->hasParent()) Predictor->setAffiliation(mySharedPtr());
            
            setScenarioVariables(Predictor->getCoordinates());
            clearParameters();
            map<string,double>::iterator it;
            
            for( it=Predictor->beginConstants(); it!=Predictor->endConstants(); ++it ) {
                
                boost::shared_ptr<msModelParameter> var = msModelParameter::New();
                
                double min = Predictor->getConstant((*it).first)/2;
                double max = Predictor->getConstant((*it).first)*2;
                var->set( Predictor->getConstant((*it).first),  min, max,(max-min)/1000.,(max-min)/100.);
                var->setId( (*it).first );
                var->addRefOnValue( Predictor->getConstant((*it).first) );
                
                addParameter( var );
            }
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msScalarModel::setPredictor(  boost::shared_ptr<msScalarFunction> predictor )");
            
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        msScalarModel::msScalarModel() : msPredictiveModel() {
            
            constructVar("msScalarModel","ScalarModel","predictive model");
        }
    }
}
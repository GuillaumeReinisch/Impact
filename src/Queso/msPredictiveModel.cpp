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

#include "msPredictiveModel.h"

namespace impact {
    
    namespace queso {
        
        
        bool msPredictiveModel::isPredictiveModelRegisteredInPython=0;
        
        msRegistrar msPredictiveModel::Registrar( "msPredictiveModel" , msPredictiveModel::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& operator<<(std::ostream& out,const msPredictiveModel& bayes) {
            
            return( bayes.print(out) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msPredictiveModel::registryInPython(){
            
#if USE_PYTHON
            msPhysicalInterface::registryInPython();
            
            if(  ! msPredictiveModel::isPredictiveModelRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msPredictiveModel,bases<msPhysicalInterface>,boost::shared_ptr<msPredictiveModel> >
                ("msPredictiveModel",  "a predictive model",init<>()
                 )
                .def( "New",&msPredictiveModel::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
                .def( "setScenarioVariables",&msPredictiveModel::setScenarioVariables ,
                     "Set the scenario variables. arg2: coordinates container." )
                .def( "getScenarioVariables",&msPredictiveModel::getScenarioVariables ,
                     "return the scenario variables." )
		.def( "addParameter",&msPredictiveModel::addParameter,
		      "add a model parameter. arg2: parameter")
                .def( "getParameterSpace",&msPredictiveModel::getParameterSpace ,
                     "return the parameter space." )
                .def("predict",&msPredictiveModel::predict,
                     "compute the Qois.")
                .def("getQoi",(double (msPredictiveModel::*)(std::string) const ) 
		     &msPredictiveModel::getQoi,
                     "return the value of a Qoi computed. arg2: id of the Qoi.")
		.def("getQoiByIndex", &msPredictiveModel::getQoiByIndex,
                     "return the value of a Qoi computed. arg2: index of the Qoi.")
		.def("_setQoi_", &msPredictiveModel::setQoi,
                     "set a QoI computed by a derived class. arg2: key, arg3: QoI value.")
                .def("getQois",&msPredictiveModel::getQois,
                     "return the QoIs in a (pointer) vector.")
                .def("noOfQois",&msPredictiveModel::noOfQois,
                     "return the number if Qois.")
		.def("updateParametersModification",&msPredictiveModel::updateParametersModification,
		     "update object depending on the parameters value.");
                
                msPredictiveModel::isPredictiveModelRegisteredInPython = 1 ;
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msPredictiveModel::setScenarioVariables(  boost::shared_ptr<msGeneralizedCoordinates> coords ) {
            
            LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msPredictiveModel::setScenarioVariables(  boost::shared_ptr<msGeneralizedCoordinates> coords )"
                                      ,getFullId());
            msTreeMapper::update(ScenarioVariables,coords);
            if( !ScenarioVariables->hasParent()) ScenarioVariables->setAffiliation(mySharedPtr());
            LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msPredictiveModel::setScenarioVariables(  boost::shared_ptr<msGeneralizedCoordinates> coords )");
            return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        msPredictiveModel::msPredictiveModel() : msPhysicalInterface() {
            
            constructVar("msPredictiveModel","PredictiveModel","predictive model");
        }
    }
}
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

#include "msLikelihood.h"

namespace impact {
    
    namespace queso {
        
        
        bool msLikelihood::isLikelihoodRegisteredInPython=0;
        
        msRegistrar msLikelihood::Registrar( "msLikelihood" , msLikelihood::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msLikelihood::registryInPython(){
            
#if USE_PYTHON
            
            msScalarFunction::registryInPython();
            
            if(  ! msLikelihood::isLikelihoodRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msLikelihood,bases<msScalarFunction>,boost::shared_ptr<msLikelihood> >
                ("msLikelihood",  "model that predicts one scalar quantity of interest. A scalar function is used as predictor, its defined constants as parameters.",init<>()
                 )
                .def( "New",&msLikelihood::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
                .def("setMeasures",&msLikelihood::setMeasures,
                     "set the database of measures. arg2: measures")
                .def("getMeasures",&msLikelihood::getMeasures,
                     "return the database of measures ")
                .def("setPredictiveModel",&msLikelihood::setPredictiveModel,
                     "set the predictive model. arg2: predictive model")
                .def("getPredictiveModel",&msLikelihood::getPredictiveModel,
                     "return the predictive model")
                .def("getErrorModelParameters",&msLikelihood::getErrorModelParameters,
                     "return the model error parameters.")
                .def("checkSanity",&msLikelihood::checkSanity,
                     "check the sanity of the variables.")
		.def("computeQoiOverScenario",&msLikelihood::computeQoiOverScenario,
		     "return the predictions of a particular QoI on all the configuration for the current parameters' value. arg2: index of the QoI.")
		.def("getMeasureOverScenario",&msLikelihood::getMeasureOverScenario,
		     "return the values of a particular measure on all the configuration. arg2: index of the measure.");
				
                
                msLikelihood::isLikelihoodRegisteredInPython = 1 ;
            }
            
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        msLikelihood::msLikelihood() : msScalarFunction() {
            
	  boost::shared_ptr<msSampler> Sampler = msSampler::New();
            constructVar("msLikelihood","Likelihood","likelihood");
        }
             
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
         boost::shared_ptr<msVectorFit1d> msLikelihood::computeQoiOverScenario(size_t i) {
	   
	     boost::shared_ptr<msScalarVariable> x = msScalarVariable::New();	     
	     x->set(0,0,Measures->getSampler()->noOfSamples(),1,1);
	     
	     vector<double> values(Measures->getSampler()->noOfSamples(),0);
	     vector<string> names = PredictiveModel->getQoisName();
	     
	     boost::shared_ptr<msVectorFit1d> vec = msVectorFit1d::New();
	     vec->set(*x,"");
	     vec->setId(names[i]);
		 
	     Measures->getSampler()->setSample(0);
	     size_t j = 0;
		 
	     while( Measures->getSampler()->msSampler::getNextPoint() ) {
		   
		   PredictiveModel->predict();
		   values[j] = PredictiveModel->getQoiByIndex(i);
		   j++;
	     }
	     vec->setValues(values);
	     
	     return vec;
	  }
	
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        	     
	boost::shared_ptr<msTreeMapper> msLikelihood::computeQoisOverScenario(vector< vector<double> >& Qois) {
	  
	     LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msLikelihood::computeQoisOverScenario(vector< vector<double> >& Qois)",getFullId());
          
	     size_t noOfScenario = Measures->getSampler()->noOfSamples();
	     size_t noOfQois     = getPredictiveModel()->noOfQois();
	     
	     LOGGER_WRITE(msLogger::DEBUG,"Number of Qois: "+output::getString<int>(noOfQois) +
	                  "; number of scenario: "+output::getString<int>(noOfScenario) );
	     
	     Qois.resize(noOfQois);
	     for( size_t j =0 ; j < noOfQois; j++)  Qois[j].resize(noOfScenario);
	     
	     size_t k = 0;  
	     Measures->getSampler()->setSample(0);
	     LOGGER_WRITE(msLogger::DEBUG, "Start sampling the scenario database");
	     
	     while( Measures->getSampler()->msSampler::getNextPoint() ) {
		   
	           PredictiveModel->predict();
		   for( size_t j=0;j< noOfQois;j++) {
		     
		       Qois[j][k] =  PredictiveModel->getQoiByIndex(j);
		   }
		   k++;
	     }
	     LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msLikelihood::computeQoisOverScenario(vector< vector<double> >& Qois)");
	     return mySharedPtr();
	}
	
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
         boost::shared_ptr<msVectorFit1d> msLikelihood::getMeasureOverScenario(size_t i) {
	   
	     boost::shared_ptr<msScalarVariable> x = msScalarVariable::New();	     
	     x->set(0,0,Measures->getSampler()->noOfSamples(),1,1);
	     
	     vector<double> values(Measures->getSampler()->noOfSamples(),0);
	     
	     boost::shared_ptr<msVectorFit1d> vec = msVectorFit1d::New();
	     vec->set(*x,"");
	     vec->setId("measure indexed "+output::getString<size_t>(i) );
		 
	     Measures->getSampler()->setSample(0);
	     size_t j = 0;
		 
	     while( Measures->getSampler()->msSampler::getNextPoint() ) {
		   
		   values[j] = Measures->getValue(i);
		   j++;
	     }
	     vec->setValues(values);
	     
	     return vec;
	  }	   
    }
}
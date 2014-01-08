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


#ifndef MSLikelihood_H
#define MSLikelihood_H

#include <msScalarFunction.h>
#include <msPredictiveModel.h>
#include <msMultiDiracs.h>
#include <msParameterSpace.h>
#include <msSampler.h>
#include <msVectorFit1d.h>

namespace impact {
    
    namespace queso {
        
        class msInverseProblem;
        using namespace impact::math;
        
        /*! \class msLikelihood
         * \brief Virtual base class for likelihood function
         *
         * A likelihood object is a scalar funtion that returns how likeli is
         * a model to reproduce given data with respect to model's parameters:
         \f[
         P(\Theta,D)
         \f]
         * where Theta are the model parameters, and D the data. The likelihood
         * implicitely defines a model error and includes a
         * ErrorModelParameters container. It is then used to generate model predictions associated
         * to model error (to be implemented).\n
         * The generalized coordinates of the function are the parameters of the model,
         * they are set when the model is set. 
         *
         * The derived classes need to :
         *    - add some error model parameters using the addModelErroParameter
         *    - define the method msLikelihood::evaluate() using the model error parameters.
         *
         * To prepare the object you need to set the predictive model and the data.
         * @warning The predictive model scenario variables (msPredictiveModel::getScenarioVariables)
         * and the coordinates of the data (msMultiDiracs::getCoordinates) have to point to the same object.
         *
         * example:
         
         *
         */
        class msLikelihood : public msScalarFunction
        {
            friend class msInverseProblem;
            
        protected:
            
            static msRegistrar Registrar;
            static bool  isLikelihoodRegisteredInPython;
            static msRegister* createInstance() { return new msLikelihood; }
            
            void registryInPython();
            
        public:
            
            msLikelihood();
            
            void update()  {
                
                msScalarFunction::update();
            };
            
            void initialize() {
                
                msScalarFunction::initialize();
                declareChild<msPredictiveModel>(PredictiveModel,msPredictiveModel::New(),"PredictiveModel");
                declareChild<msMultiDiracs>(Measures,msMultiDiracs::New(),"Measures");
                declareChild<msParameterSpace>(ErrorModelParameters,msParameterSpace::New(),"ErrorModelParameters");
		
                getCoordinates()->setId("set when the model is set");
            };
            
            static boost::shared_ptr<msLikelihood> New(){
                
                boost::shared_ptr<msLikelihood> T( new msLikelihood );
                T->initialize();
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msLikelihood> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }

            
            /*! \brief set the predictive model
             *
             * The scenario of the model (msPredictiveModel::getScenarios()) has
             * to be the same (pointer) than the coordinates of the measures
             * (msMultiDiracs::getCoordinates).
             * \param model predictive model
             */
            boost::shared_ptr<msTreeMapper> setPredictiveModel(boost::shared_ptr<msPredictiveModel> model) {
 
		msTreeMapper::update(PredictiveModel,model);
                
                setCoordinates(model->getParameterSpace());
                
                return mySharedPtr();
            };
        
            /*! \brief return the model
             */
            boost::shared_ptr<msPredictiveModel> getPredictiveModel(){
                
                return PredictiveModel.getSharedPtr();
            };
            
            /*! \brief set the measurments database
             *
             * The scenario of the model (msPredictiveModel::getScenarios()) has
             * to be the same (pointer) than the coordinates of the measures
             * (msMultiDiracs::getCoordinates).
             * \param measures measures
             */
            boost::shared_ptr<msTreeMapper> setMeasures(boost::shared_ptr<msMultiDiracs> measures){
                
                msTreeMapper::update(Measures,measures);
                return mySharedPtr();
            };
            
            /*! \brief return the measurments database
             */
            boost::shared_ptr<msMultiDiracs> getMeasures() const {
                
                return Measures.getSharedPtr();
            };
            
            /*! \brief return the model parameters
             */
            boost::shared_ptr<msParameterSpace> getErrorModelParameters() const {
                
                return ErrorModelParameters.getSharedPtr();
            };
            
            boost::shared_ptr<msTreeMapper> setErrorModelParametersValues(vector<double>& values) {
	      
	        if(ErrorModelParameters->noOfDim()!=values.size()) {
                    
                    BOOST_THROW_EXCEPTION(msError( "values.size() != ErrorModelParameters->noOfDim() ",
                                                  " boost::shared_ptr<msTreeMapper> msLikelihood::setErrorModelParametersValues(vector<double>& values)",
                                                  getFullId()
                                                  )
                                          );
                }
                for(size_t i=0;i<ErrorModelParameters->noOfDim(); i++){
                    
                    ErrorModelParameters->getVariable(i)->setValue(values[i]);
                }
                return mySharedPtr();	      
	    }
	    
            /*! \brief compute the likelihood at the current parameters value
             *
             * This method needs to be defined by derived class. When the method is
             * entered, the current value of the parameters are stored in the
             * generalized coordinates container  (msScalarFunction::getCoordinates),
             * the measurments database is obtain via the msLikelihood::getMeasures method,
             * and the predictive model is obtained using msLikelihood::getPredictiveModel.
             *
             * The model and the database share the same scenario variables as coordinates.
             * (to ensure this please call msLikelihood::checkSanity at the beginning of the method).
             * Then, the sampler of the measures can be used to set the scenario sequentially;
             * a template of function look like this:
             *
             * ~~~~~~~~~~~~~{.py}
             from libimpact import *
             import itertools
             
             # we assume that the model and the measures have been correctly set
             
             def evaluate(self):
             
             checkSanity()
             
             Model    = getPredictiveModel()
             Measures = getMeasures()
             Sampler  = Measures.getSampler()
             ScenarioCoors = Model.getScenario() # same as Measures.getCoordinates()
             
             while Sampler.getNextPoint(ScenarioCoors):
             
             Model.predict()
             QoIs        = Model.getQois()
             Measures    = Measures.getValues()
             
             likelihood = 0
             
             for prediction,measure in itertools.izip(QoIs,Measures):
             
             likelihood = likelihood - function( prediction , measure)
             
             return likelihood
             * ~~~~~~~~~~~~~
             *
             */
            virtual double evaluate() {
                
                BOOST_THROW_EXCEPTION(msError( "This method is virtual, you need to overide it",
                                              "double msLikelihood::evaluate()",
                                              getFullId()
                                              )
                                      );
            };
            /*! \brief return the predictions of a particular QoI on all the configuration for the current parameters' value
	     * 
	     * return a scalar function (msVectorFit1d) with x as
	     * the index of the scenario and y the QoI.
	     * 
	     * \param i index of the QoI
	     */ 
	    boost::shared_ptr<msVectorFit1d> computeQoiOverScenario(size_t i); 

	    boost::shared_ptr<msTreeMapper> computeQoisOverScenario(vector< vector<double> >& Qois);
	    
            /*! \brief return the values of a particular measure on all the configuration
	     * 
	     * return a scalar function (msVectorFit1d) with x as
	     * the index of the scenario and y the measure.
	     * 
	     * \param i index of the measure
	     */ 
	    boost::shared_ptr<msVectorFit1d> getMeasureOverScenario(size_t i); 
	    
	    
	    
            //! \brief realize a sanity check of the variables
            virtual void checkSanity(){
                
                if( Measures->getCoordinates() != PredictiveModel->getScenarioVariables()){
                    
                    stringstream out;
                    out<<"The model's scenario and the measure's coordinates have to point to"
                    <<" the same object. This is not the case, please reorganize your data.";
                    
                    msError e(out.str(),
                              " boost::shared_ptr<msTreeMapper> msLikelihood::setMeasures(boost::shared_ptr<msMultiDiracs> measures)",getFullId());
                    BOOST_THROW_EXCEPTION(e);
                }
                if( getCoordinates() != PredictiveModel->getParameterSpace()){
                    
                    stringstream out;
                    out<<"The model's parameter space and the coordinates of the likelihood have to point to"
                    <<" the same object. This is not the case, please reorganize your data.";
                    
                    msError e(out.str(),
                              " boost::shared_ptr<msTreeMapper> msLikelihood::setMeasures(boost::shared_ptr<msMultiDiracs> measures)",getFullId());
                    BOOST_THROW_EXCEPTION(e);
                }
            }
            
        protected:
            
            /*! \brief return the model
             */
            boost::shared_ptr<msTreeMapper> addModelErrorParameter(boost::shared_ptr<msModelParameter> param){
                
                ErrorModelParameters->addVar(param);
                if( !param->hasParent()) {
                    
                    param->setAffiliation( ErrorModelParameters.getSharedPtr() );
                }
                return mySharedPtr();
            };
            
            
        private:
            
            msChild<msPredictiveModel> PredictiveModel;
            
            msChild<msMultiDiracs> Measures;
            
            msChild<msParameterSpace> ErrorModelParameters;
            
        };
    }
    
}
#endif // MSDIRACS_H

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


#ifndef MSINVPB_H
#define MSINVPB_H

#include <msSolver.h>
#include <msLikelihood.h>
#include <msUtilitiesBayes.h>

#include <queso/GslMatrix.h>
#include <queso/GslVector.h>
#include <queso/StatisticalInverseProblem.h>
#include <queso/StatisticalForwardProblem.h>
#include <queso/InfoTheory.h>
#include <queso/GenericScalarFunction.h>
#include <queso/UniformVectorRV.h>

namespace impact {
    
    namespace queso {
        
        using namespace impact::math;
        using namespace QUESO;
	
        /*! \class msInverseProblem
         * \brief Class that handles inverse problem
	 * 
	 * The inverse problem solver allows you to compute the probabilitie density function of 
	 * some model parameters of a predictive model (msPredictiveModel) with respect to some (experimental) 
	 * quantities of interest. The solver is based on a likelihood object (msLikelihood)
	 * that defines the probability that the QoI computed by the predictive model are the experimental ones.
	 * 
         */
        class msInverseProblem : public msSolver
        {
            
	    friend  double likelihoodRoutine_(const GslVector& paramValues,
					      const GslVector* paramDirection,
					      const void*  Data,
					      GslVector*  gradVector,
					      GslMatrix*  hessianMatrix,
					      GslVector* hessianEffect
					      );
	    
	    
	    /*! \class msInverseProblemParams
	     * 
	     *  Parameters used by msInverseProblem
	     */ 
            class msInverseProblemParams : public msParamsManager
            {
                static msRegistrar Registrar;
                static msRegister* createInstance() { return new msInverseProblemParams; }
                
            public:
	      
                void initialize() {
		  
                    msParamsManager::initialize();
		    
                    addParameter( "ChainLength" ,     
				  "1000" , 
				  "Default chain length" ,    
				  msUnit() );
                    addParameter( "LastChainLength" , 
				  "1000" , 
				  "Last level chain length" , 
				  msUnit() );
                    addParameter( "DisplayStep" ,     
				  "100" , 
				  "Period of log of the inference process" , 
				  msUnit() );
                }
                
                static boost::shared_ptr<msInverseProblemParams> New() {
		  
		    boost::shared_ptr<msInverseProblemParams> T( new msInverseProblemParams );
                    T->initialize(); T->update();
                    return T;
                }
                
                msInverseProblemParams() : msParamsManager() {};
            };
            
	    
        protected:
            
            static msRegistrar Registrar;
            static bool  isInverseProblemRegisteredInPython;
            static msRegister* createInstance() { return new msInverseProblem; }
            
            void registryInPython();
            
        public:
            
            msInverseProblem();
            
            void update()  {
                
                msSolver::update();
            };
            
            void initialize() {
                
                msSolver::initialize();
                
                declareChild<msLikelihood>(      Likelihood,msLikelihood::New(),
						 "Likelihood");
		declareChild<msSamplerExplicit>( LastChainSamples,msSamplerExplicit::New(),
						 "LastChainSamples");
		declareChild<msGeneralizedCoordinates>( AllParametersContainer,msGeneralizedCoordinates::New(),
							"AllParametersContainer");
            };
            
            static boost::shared_ptr<msInverseProblem> New(){
                
                boost::shared_ptr<msInverseProblem> T( new msInverseProblem );
                T->initialize();
                T->setParameters( msInverseProblemParams::New() );
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msInverseProblem> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            /*! \brief set the likelihood object
             *
             * \param likelihood likelihood
             */
            boost::shared_ptr<msTreeMapper> setLikelihood(boost::shared_ptr<msLikelihood> likelihood);
            
            /*! \brief return the likelihood object
             */
            boost::shared_ptr<msLikelihood> getLikelihood() {
                
                return Likelihood.getSharedPtr();
            }
                        
            /*! \brief solve the inverse problem
             */
            boost::shared_ptr<msTreeMapper> solve() ;
            
            /*! \brief compute the CI for all the QoI for all scenario 
	     * 
	     * The function are storred in the CIcontainer child, if 
	     * this child already exist, its content get updated.
             */	    
            boost::shared_ptr<msTreeMapper> computeCisOverScenario() ;
	     
            /*! \brief return the predictive model
             */
            boost::shared_ptr<msPredictiveModel> getPredictiveModel() {
                
                return Likelihood->getPredictiveModel();
            }

             /*! \brief return the container of all the parameters (from PredictiveModel + Likelihood )
             */
            boost::shared_ptr<msGeneralizedCoordinates> getAllParametersContainer() {
                
                return AllParametersContainer.getSharedPtr();
            }
            
            /*! \brief return the sample of the posterior PDF for all model's parameters
             */
            boost::shared_ptr<msSamplerExplicit> getLastChainSamples() {
                
                return LastChainSamples.getSharedPtr();
            }
            
        private:
            
            msChild<msLikelihood>             Likelihood;
            
	    msChild<msSamplerExplicit>        LastChainSamples;
	    
	    msChild<msGeneralizedCoordinates> AllParametersContainer;
	    
            double LikelihoodMax;
            
	    int DisplayStep;
	    	    
            size_t CurrentSolverIteration;
            
            void writeInputQueso();
	    
	   /*! \brief return the likelihood for given parameters
             *
             *\param params values of the parameters
             */
            double likelihood(const GslVector& params);
        };
    }
    
}
#endif


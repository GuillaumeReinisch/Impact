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

#include "msInverseProblem.h"


namespace impact {
    
    namespace queso {
        
        boost::shared_ptr<msInverseProblem> InverseProblem;
        
        bool msInverseProblem::isInverseProblemRegisteredInPython=0;
        
        msRegistrar msInverseProblem::Registrar( "msInverseProblem" , msInverseProblem::createInstance );
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        std::ostream& operator<<(std::ostream& out,const msInverseProblem& bayes) {
            
            return( bayes.print(out) );
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msInverseProblem::registryInPython(){
            
#if USE_PYTHON
            msSolver::registryInPython();
            
            if(  ! msInverseProblem::isInverseProblemRegisteredInPython ) {
                
                using namespace boost::python;
                
                class_<msInverseProblem,bases<msSolver>,boost::shared_ptr<msInverseProblem> >
                ("msInverseProblem",  "realize inverse problem on model.",init<>()
                 )
                .def( "New",&msInverseProblem::New ,"Return a shared pointer to a new object" )
                .staticmethod("New")
                .def("getPredictiveModel",&msInverseProblem::getPredictiveModel,
                     "return the predictive model.")
                .def("setLikelihood",&msInverseProblem::setLikelihood,
                     "set the likelihood object. arg2: likelihood.")
                .def("getLikelihood",&msInverseProblem::getLikelihood,
                     "return the likelihood object.")
                .def("solve",&msInverseProblem::solve,
                     "solve the inverse problem")
		.def("getAllParametersContainer",&msInverseProblem::getAllParametersContainer,
		     "return the container of all the parameters (from PredictiveModel + Likelihood )")
		.def("getLastChainSamples", &msInverseProblem::getLastChainSamples,
		      "return the sample of the posterior PDF for all model's parameters")
		.def("computeCisOverScenario", &msInverseProblem::computeCisOverScenario,
		      "compute the Qoi's confidance intervals over the scenario of the database.");
                
                msInverseProblem::isInverseProblemRegisteredInPython = 1 ;
		
            }
#endif
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        msInverseProblem::msInverseProblem() : msSolver() {
            
            constructVar("msInverseProblem","InverseProblem","predictive model");
        }
                
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msInverseProblem::setLikelihood(boost::shared_ptr<msLikelihood> likelihood) {
                
	        if(!likelihood->hasParent()) likelihood->setAffiliation(mySharedPtr());
		
                msTreeMapper::update(Likelihood,likelihood);
                
		for(int i=0;i<getPredictiveModel()->getParameterSpace()->noOfDim();i++ ) {
                
                    AllParametersContainer->addVar(getPredictiveModel()->getParameterSpace()->getVariable(i));
                }
                for(int i=0;i<getLikelihood()->getErrorModelParameters()->noOfDim();i++ ) {
                
                    AllParametersContainer->addVar(getLikelihood()->getErrorModelParameters()->getVariable(i));
                }
                
		LastChainSamples->setCoordinates(AllParametersContainer.getSharedPtr());
		
                return mySharedPtr();
            }
            
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double likelihoodRoutine_(const GslVector& paramValues,
                                  const GslVector* paramDirection,
                                  const void*  Data,
                                  GslVector*  gradVector,
                                  GslMatrix*  hessianMatrix,
                                  GslVector* hessianEffect) {
            
            double result = 0;
            
            try {
                result = InverseProblem->likelihood(paramValues);
                
            } catch (msException& e) {
                
                e.addContext("double likelihoodRoutine_(const GslVector& paramValues,const GslVector* paramDirection,const void*  Data,GslVector*  gradVector,GslMatrix*  hessianMatrix,GslVector* hessianEffect)");
                IMPACT_THROW_EXCEPTION(e);
            }
            
            return(result);
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        double msInverseProblem::likelihood( const GslVector& paramValues) {
            
	    size_t n = getPredictiveModel()->getParameterSpace()->noOfDim();
	    size_t nerror = getLikelihood()->getErrorModelParameters()->noOfDim();
	    
	    
	    for(int i=0;i<AllParametersContainer->noOfDim(); i++ ) {
	       
	        AllParametersContainer->getVariable(i)->setValue(paramValues[i]);
	    }
	    
            double l = getLikelihood()->evaluate();
	    	    
	    if( l > LikelihoodMax ) {
	      
	        LOGGER_WRITE(msLogger::INFO,"New maximum likelihood: "+output::getString<double>(l));  
		LikelihoodMax = l;
	    }
	    
	    if( (CurrentSolverIteration % DisplayStep) == 0 ) {
	      
	      stringstream out;
	      out<<"Current step: "<< CurrentSolverIteration <<"; current likelihood: "<<l;
	      LOGGER_WRITE(msLogger::INFO,out.str());
	    };
	    	    
	    if( ( (CurrentSolverIteration % DisplayStep) == 0 ) || (l==LikelihoodMax) ) {
	      
	        LOGGER_HEADER2COLUMNS(msLogger::INFO, "Param index","", "Value", "")
		
		for(int i=0; i<n + nerror; i++) {
                
                    LOGGER_WRITE2COLUMNS(msLogger::INFO, i ,paramValues[i]);
                }
	    }
	    CurrentSolverIteration ++;
            return l;
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msInverseProblem::solve() {
            
            int initialized;
            MPI_Initialized(&initialized);
            if (!initialized) MPI_Init(NULL, NULL);
            
            writeInputQueso();
            
            boost::shared_ptr<FullEnvironment> Environment =
            boost::shared_ptr<FullEnvironment> (new FullEnvironment(MPI_COMM_WORLD,"queso.inp","",NULL));
            
            LikelihoodMax =-1e30;
            CurrentSolverIteration = 0 ;
	    
            getParameters()->queryIntValue("DisplayStep",DisplayStep);

            size_t nParam      = getPredictiveModel()->getParameterSpace()->noOfDim();
            size_t nParamError = getLikelihood()->getErrorModelParameters()->noOfDim();
            
            LOGGER_WRITE(msLogger::INFO,"Step 1 of 5: Instantiate the parameter space of dim "+output::getString<int>(nParam+nParamError));
            
            VectorSpace<GslVector,GslMatrix>  paramSpace( *Environment, "param_", nParam + nParamError, NULL);
            
            
            LOGGER_WRITE(msLogger::INFO,"Step 2 of 5: Instantiate the parameter domain");
            
            GslVector paramMins(paramSpace.zeroVector());
            GslVector paramMaxs(paramSpace.zeroVector());
            
	    for(int i=0;i<AllParametersContainer->noOfDim(); i++ ) {
	       
	        paramMins[i]=AllParametersContainer->getVariable(i)->getMin();
                paramMaxs[i]=AllParametersContainer->getVariable(i)->getMax();
	    }
	    
            LOGGER_HEADER3COLUMNS(msLogger::INFO, "Param index","", "Min", "", "Max","" )
            
            for(int i=0;i<nParam + nParamError;i++) {
                
                LOGGER_WRITE3COLUMNS(msLogger::INFO, i ,paramMins[i] , paramMaxs[i] );
            }
            
            BoxSubset<GslVector,GslMatrix>
            paramDomain("param_",paramSpace,paramMins,paramMaxs);
            
            
            LOGGER_WRITE(msLogger::INFO,"Step 3 of 5: Instantiate the likelihood function object");
            
            GslVector meanVector(paramSpace.zeroVector());
            
            GenericScalarFunction<GslVector,GslMatrix>
            likelihoodFunctionObj("like_",
                                  paramDomain,
                                  likelihoodRoutine_,
                                  (void*) this,
                                  true
                                  ); // routine computes [-2.*ln(function)]
            
            
            LOGGER_WRITE(msLogger::INFO,"Step 4 of 5: Instantiate the inverse problem with uniform priors");
            
            UniformVectorRV<GslVector,GslMatrix> priorRv("prior_", paramDomain);
            GenericVectorRV<GslVector,GslMatrix>  postRv("post_", paramSpace);
            StatisticalInverseProblem<GslVector,GslMatrix>
            ip("", NULL, priorRv, likelihoodFunctionObj, postRv);
            
            
            LOGGER_WRITE(msLogger::INFO,"Step 5 of 5:  Solve the inverse problem");
            int world_size;
            MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  
	    LOGGER_WRITE(msLogger::INFO,"Number of process available: "+output::getString<int>(world_size));
	    LOGGER_WRITE(msLogger::INFO,"Does everithing seems ok ?");
            char ans[10]; std::cin>>ans;
            
            InverseProblem = boost::static_pointer_cast<msInverseProblem>(mySharedPtr());
            
            ip.solveWithBayesMLSampling();
	      
            LOGGER_WRITE(msLogger::INFO,"Inverse problem solved, prepare data");
	   
            GslVector tmpVec(postRv.imageSet().vectorSpace().zeroVector());
            vector<double> LastChain_i( nParam + nParamError , 0 );
	    
	    LastChainSamples->clear();
	    
            for (unsigned int i = 0; i < postRv.realizer().subPeriod(); ++i) {
                
	        postRv.realizer().realization(tmpVec);
             			
		for(int j = 0; j<nParam + nParamError; j++)  LastChain_i[j] = tmpVec[j];
		
                LastChainSamples->addSample(LastChain_i);
             }
	     
             return mySharedPtr();
        }
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        boost::shared_ptr<msTreeMapper> msInverseProblem::computeCisOverScenario(){
	  
	  LOGGER_ENTER_FUNCTION_DBG("boost::shared_ptr<msTreeMapper> msInverseProblem::computeCisOverScenario()",getFullId());
            
	  LOGGER_WRITE(msLogger::INFO, "Initialize data");
	  
	  vector< vector<vector<double> > > QoisForParamSample; // QoisForParamSample[i][j][k] = Param's sample i, Qoi number j, scenario number k
	  
	  boost::shared_ptr<msSamplerExplicit> scenarioSampler = getLikelihood()->getMeasures()->getSampler();
	  
	  boost::shared_ptr<msSamplerExplicit> paramsSampler   = getLastChainSamples();
	  
	  size_t noOfQois         = getLikelihood()->getPredictiveModel()->noOfQois();
	  size_t noOfParamSamples = getLastChainSamples()->noOfSamples() - 1;
	  size_t noOfScenario     = getLikelihood()->getMeasures()->getSampler()->noOfSamples();
	  
	  vector< boost::shared_ptr<msVectorFit1d> > CisMin(  noOfQois );
	  vector< boost::shared_ptr<msVectorFit1d> > CisMean( noOfQois );
	  vector< boost::shared_ptr<msVectorFit1d> > CisMax(  noOfQois );	  
	  
          vector<string> qoisName = getLikelihood()->getPredictiveModel()->getQoisName();
	  
	  for( size_t j =0; j < noOfQois ; ++j)  {
	    
	      CisMin[j]  = msVectorFit1d::New();
	      CisMean[j] = msVectorFit1d::New();
	      CisMax[j]  = msVectorFit1d::New();
	      
	      CisMin[j]->set(0,noOfScenario,1,"","");
	      CisMin[j]->setId("CI min of "+qoisName[j]);
	      
	      CisMean[j]->set(0,noOfScenario,1,"","");
	      CisMean[j]->setId("CI mean of "+qoisName[j]);
	      
	      CisMax[j]->set(0,noOfScenario,1,"","");
	      CisMax[j]->setId("CI max of "+qoisName[j]);
	  }
	  
	  LOGGER_WRITE(msLogger::INFO, "compute Qois over scenario at each parameter's samples");
	  paramsSampler->setSample(0);
	  
	  size_t i=0;
	  vector< vector<double> > qois;
	  
	  while( paramsSampler->msSampler::getNextPoint() ) {
	     
	      LOGGER_WRITE(msLogger::INFO, "set next parameter sample indexed "+output::getString<int>(i)+
	                   " on "+output::getString<int>(noOfParamSamples));

	      getLikelihood()->computeQoisOverScenario(qois);
	      
	      QoisForParamSample.push_back(qois);
	      i++;   
	  }
	  	  
	  vector< double> valuesAtScenarioAndQoi(noOfParamSamples);
	  
	  LOGGER_WRITE(msLogger::DEBUG, "compute CIs");
	  
	  for( size_t k=0;k< noOfScenario;k++) {
		
	        LOGGER_WRITE(msLogger::DEBUG, "compute CI for scenario "+output::getString<int>(k));
		
	        for( size_t j=0;j< noOfQois;j++) {
		  
		    for( size_t i =0;i<noOfParamSamples; i++) {
		        cout<<i<<" "<< QoisForParamSample[i][j][k]<<endl;
		        valuesAtScenarioAndQoi[i]= QoisForParamSample[i][j][k];
		    }
		 LOGGER_WRITE(msLogger::DEBUG, "QoI indexed "+output::getString<int>(j));
                 double CImin,CImax,CImean;
		 LOGGER_WRITE(msLogger::DEBUG, "compute CIs");
		 
		 AnalyseHisto(&valuesAtScenarioAndQoi,CImin,CImax,CImean);
		 
		 (*(CisMin[j]))[k]  = CImin;
		 (*(CisMean[j]))[k] = CImean;
		 (*(CisMax[j]))[k]  = CImax;
		}
	   }
	   LOGGER_WRITE(msLogger::DEBUG, "Construct CIsContainer");
	   
	   boost::shared_ptr<msTreeMapper> CIsContainer = msTreeMapper::New();
	   CIsContainer->setId("PredictionCIs");
	   for( size_t j=0;j< noOfQois;j++) {
	     
	       CIsContainer->addChild(CisMin[j],qoisName[j]  +"_min");
	       CIsContainer->addChild(CisMean[j],qoisName[j] +"_mean");
	       CIsContainer->addChild(CisMax[j],qoisName[j]  +"_max");
	   }
	   addChild(CIsContainer,"CIsContainer");
	   
	   LOGGER_EXIT_FUNCTION2("boost::shared_ptr<msTreeMapper> msInverseProblem::computeCisOverScenario()");
	   
	   return mySharedPtr();	   
	}
        
        //-------------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------------------------------
        
        void msInverseProblem::writeInputQueso() {
            
            int L,l;
            getParameters()->queryIntValue("ChainLength",l);
            getParameters()->queryIntValue("LastChainLength",L);
            cout<<"ChainLength="<<L<<" last"<<L<<endl;
            std::ofstream input("queso.inp");
            input<<"###############################################\n"
            <<"## UQ Environment\n"
            <<"################################################\n"
            <<"#env_help                = anything\n"
            <<"env_numSubEnvironments   = 1\n"
            <<"env_subDisplayFileName   = display\n"
            <<"env_subDisplayAllowAll   = 0\n"
            <<"env_subDisplayAllowedSet = 0\n"
            <<"env_displayVerbosity     = 0\n"
            <<"env_syncVerbosity        = 0\n"
            <<"env_seed                 = 0\n"
            <<"################################################\n"
            <<"## Statistical inverse problem (ip)\n"
            <<"################################################\n"
            <<"ip_help                 = anything\n"
            <<"ip_computeSolution      = 1\n"
            <<"ip_dataOutputFileName   = sipOutput\n"
            <<"ip_dataOutputAllowedSet = 0\n"
            <<"################################################\n"
            <<"## 'ip_': information for Multilevel algorithm\n"
            <<"################################################\n"
            <<"ip_ml_help                 = anything\n"
            <<"ip_ml_dataOutputFileName   = sipOutput_ml\n"
            <<"ip_ml_dataOutputAllowedSet = 0 1\n"
            <<"################################################\n"
            <<"## All levels, unless otherwise specified\n"
            <<"################################################\n"
            <<"ip_ml_default_help=xyz\n"
            <<"ip_ml_default_rawChain_size ="<<l<<"\n"
            <<"ip_ml_default_rawChain_dataOutputFileName = rawChain_ml\n"
            <<"ip_ml_0_rawChain_dataOutputFileName = rawChain_ml\n"
            <<"ip_ml_default_putOutOfBoundsInChain = 1\n"
            <<"ip_ml_default_scaleCovMatrix                      = 1\n"
            <<"ip_ml_default_dr_maxNumExtraStages                = 0\n"
            <<"################################################\n"
            <<"## Last level (level 4 for 'example_gsl' executable)\n"
            <<"################################################\n"
            <<"ip_ml_last_dataOutputFileName                  = sipOutput_ml\n"
            <<"ip_ml_last_dataOutputAllowedSet                = 0  1\n"
            <<"ip_ml_last_rawChain_size                       = "<<L<<"\n"
            <<"ip_ml_last_rawChain_computeStats               = 1\n"
            <<"ip_ml_last_rawChain_dataOutputFileName         = rawChain_ml\n"
            <<"ip_ml_last_rawChain_stats_kde_compute          = 1\n"
            //<<"ip_ml_last_rawChain_stats_kde_numEvalPositions = 1000\n"
            <<"ip_ml_last_rawChain_stats_covMatrix_compute    = 1\n"
            <<"ip_ml_last_rawChain_stats_corrMatrix_compute   = 1\n"
            <<"ip_ml_last_putOutOfBoundsInChain = 1\n"
            <<"ip_ml_last_scaleCovMatrix                      = 1\n"
            <<"ip_ml_last_dr_maxNumExtraStages                = 0\n";
            
            input.close();
        }
        
    }
    
}

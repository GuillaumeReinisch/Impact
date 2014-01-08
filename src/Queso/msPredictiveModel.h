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


#ifndef MSBAYSIANPredictiveModel_H
#define MSBAYSIANPredictiveModel_H

#include <msGeneralizedCoordinates.h>
#include <msParameterSpace.h>

namespace impact {
    
    namespace queso {
        
        /*! \brief Virtual base class for a predictive model
         *
         * The purpose of a predictive model is to return a vector of quantities of
         * interest (i.e. predictions) with respect to parameters values (model parameters) at
         *  scenario. Important use of predictive models is to realize inverse problem.
         *
         * Derived classes need to set the parameters (using msPredictiveModel::addParameter),
         * the scenario (using msPredictiveModel::setScenarioVariables), and define
         * the msPredictiveModel::predict() method. If the model paramaeters are not directly
         * used for prediction, the method msPredictiveModel::updateParametersModification should
         * also be redefined.
         *
         * Below a simple example of the definition of an harmonic model:
         * ~~~~~~~~~~~~~{.py}
         from libimpact import *
         
         class msHarmonicModel( msPredictiveModel ):
         """
         Class predicting a value for a harmonic model.
         """
         def __init__(self):
         super( msHarmonicModel , self).__init__()
         self._constructVar_("msHarmonicModel","harmonic model","simple model example");
         
         def _initialize_(self):
         super(msHarmonicModel, self)._initialize_()
         
         self.a0 = msScalarVariable.New().set("a0",0,-1,1,0.1,0.1)
         addParameter( self.a0 )
         
         self.a1 = msScalarVariable.New().set("a1",0,-1,1,0.1,0.1)
         addParameter( self.a1 )
         
         self.var = msScalarVariable.New().define("q",0,-10,10,0.1,0.1)
         self.scenario = msGeneralizedCoordinates.New().addVar(self.var)
         self.setScenarioVariables(scenario)
         
         def _update_(self):
         super(msHarmonicModel, self)._update_()
         
         @staticmethod
         def New(  ):
         Self = msHarmonicModel()
         Self._initialize_()
         Self._update_()
         return Self
         
         def predict():
         value = self.a0.getValue() + self.a1.getValue()*var.getValue()*var.getValue()
         if len(getQoIs()) == 0:
         getQoIs().append(value)
         else:
         getQoIs()[0] = value
         
         * ~~~~~~~~~~~~~
         
         */
        class msPredictiveModel : public msPhysicalInterface
        {
            
        protected:
            
            static msRegistrar Registrar;
            static bool  isPredictiveModelRegisteredInPython;
            static msRegister* createInstance() { return new msPredictiveModel; }
            
            void registryInPython();
            
        public:
            
            msPredictiveModel();
            
            void update()  {
                
                msPhysicalInterface::update();
            };
            
            void initialize() {
                
                msPhysicalInterface::initialize();
                declareChild<msGeneralizedCoordinates>(ScenarioVariables,msGeneralizedCoordinates::New(),"ScenarioVariables");
                declareChild<msParameterSpace>(ParameterSpace,msParameterSpace::New(),"ParameterSpace");
            };
            
            static boost::shared_ptr<msPredictiveModel> New(){
                
                boost::shared_ptr<msPredictiveModel> T( new msPredictiveModel );
                T->initialize();
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                boost::shared_ptr<msPredictiveModel> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            
            boost::shared_ptr<msTreeMapper> setModelParametersValues(vector<double>& values){
                
                if(ParameterSpace->noOfDim()!=values.size()) {
                    
                    BOOST_THROW_EXCEPTION(msError( "values.size() != ParameterSpace->noOfDim() ",
                                                  " boost::shared_ptr<msTreeMapper> setModelParametersValues(vector<double>& values)",
                                                  getFullId()
                                                  )
                                          );
                }
                for(size_t i=0;i<ParameterSpace->noOfDim(); i++){
                    
                    ParameterSpace->getVariable(i)->setValue(values[i]);
                }
                return mySharedPtr();
            };
            
            /*! \brief set the scenario space.
             *
             * \param scenario scenario space
             */
            boost::shared_ptr<msTreeMapper> setScenarioVariables(boost::shared_ptr<msGeneralizedCoordinates> scenario);
            
            
            /*! \brief This method is intended to change parameters in sub objects when a model parameter is changed
             *
             * This function is called after model parameters are changed.
             */
            virtual boost::shared_ptr<msTreeMapper> updateParametersModification() {
                
                BOOST_THROW_EXCEPTION(msError( "This method is virtual, you need to overide it",
                                              "boost::shared_ptr<vector<double> > msPredictiveModel::predict()",
                                              getFullId()
                                              )
                                      );
                return mySharedPtr();
            }
            
            /*! \brief virtual function that needs to be defined by derived classes.
             *
             * This function should compute the quantities of interest and fill the Qois vector
             * (obtained using the getQoIs() method from derived class).
             * The parameters are accessed using the method getParameterValue(size_t i).
             * Before calling this function, the parameters have to be already defined (using the addParameter(double* var)
             * method).
             */
            virtual boost::shared_ptr<msTreeMapper> predict() {
                
                BOOST_THROW_EXCEPTION(msError( "This method is virtual, you need to overide it",
                                              "boost::shared_ptr<vector<double> > msPredictiveModel::predict()",
                                              getFullId()
                                              )
                                      );
                return mySharedPtr();
            };
            
            //! return the number of QoIs
            size_t noOfQois() {
	      
                if(Qois.size()==0){
		  msError e("The number of Qois is 0, did you set the QOI in the 'update' method of your predictive model?",
			    "size_t msPredictiveModel::noOfQois()",getFullId());
		  BOOST_THROW_EXCEPTION(e);
		}
                return Qois.size();
            }
            
            //! \brief return the Qois vector
            map<string,double> getQois() const {
                
                return Qois;
            }
            
            vector<string> getQoisName() const {
	      
	      vector<string> names;
	      map<string,double>::const_iterator it = Qois.begin();
	      for(;it!=Qois.end();++it) names.push_back( (*it).first );
	      
	      return names;
	    }
            /*! \brief return the value of a Qoi
             *
             * \param id id of the Qoi
             */
            double getQoi(string id) const{
                
                map<string,double>::const_iterator it =  Qois.find(id);
                if( it == Qois.end() ) {
                    
                    stringstream out;
                    out<<"The Qoi of id '"<<id<<"' has not been found.";
                    msError e(out.str(),"double msPredictiveModel::getQoi(string id)",getFullId());
                    BOOST_THROW_EXCEPTION(e);
                }
                return (*it).second;
            }
            /*! \brief return the value of a Qoi
             *
             * \param i index of the Qoi
             */
            double getQoiByIndex(int i) const {
                
                if( i >  Qois.size() ) {
                    
                    stringstream out;
                    out<<"The Qoi index is out of range.";
                    msError e(out.str(),"double msPredictiveModel::getQoi(string id)",getFullId());
                    BOOST_THROW_EXCEPTION(e);
                }
                map<string,double>::const_iterator it = Qois.begin();
                std::advance(it,i);
                return (*it).second;
            }
            /*! \brief return the parameter space
             */
            boost::shared_ptr<msParameterSpace> getParameterSpace() const {
                
                return ParameterSpace.getSharedPtr();
            }
            
            /*! \brief return the scenario variables
             */
            boost::shared_ptr<msGeneralizedCoordinates> getScenarioVariables() const {
                
                return ScenarioVariables.getSharedPtr();
            }
            
        private:
            
            msChild<msGeneralizedCoordinates> ScenarioVariables;
            
            msChild<msParameterSpace> ParameterSpace;
            
            
        protected:
            
            map< string , double > Qois;
            
            boost::shared_ptr<msTreeMapper> setQoi(string key,double value){
                
                Qois[key]=value;
                return mySharedPtr();
            }
            /*! \brief add a parameter to the model
             *
             * \param param parameters
             */
            boost::shared_ptr<msTreeMapper> addParameter(boost::shared_ptr<msModelParameter> param )  {
                
                ParameterSpace->addVar(param);
                return mySharedPtr();
            }
            
            /*! \brief clear the model parameters list
             */
            boost::shared_ptr<msTreeMapper> clearParameters()  {
                
                ParameterSpace->clear();
                return mySharedPtr();
            }
            
        };
    }
    
}
#endif // MSDIRACS_H

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


#ifndef MSLikeliNoCorrel_H
#define MSLikeliNoCorrel_H

#include <msLikelihood.h>
#include <msPredictiveModel.h>
#include <msMultiDiracs.h>

namespace impact {
    
    namespace queso {
        
        using namespace impact::math;
        
        /*! \class msLikeliNoCorrel
         * \brief Virtual base class for likelihood that assumes additive independant model error.
         *
         * Within the assumption of independant model error the likelihood can be
         * evaluated directly using the predicted and measured values at each scenerio independantly.
         *
         * The method msLikelihood::evaluate() is reimplemented based on the virtual function
         * msLikeliNoCorrel::likelihood that needs to be implemented by
         * derived class.
         */
        class msLikeliNoCorrel : public msLikelihood
        {
            
            protected:
            
            static msRegistrar Registrar;
            static bool  isLikeliNoCorrelRegisteredInPython;
            static msRegister* createInstance() { return new msLikeliNoCorrel; }
            
            void registryInPython();
            
            public:
            
            msLikeliNoCorrel();
            
            void update()  {
                
                msLikelihood::update();
            };
            
            void initialize() {
                
                msLikelihood::initialize();
            };
            
            static boost::shared_ptr<msLikeliNoCorrel> New(){
                
                boost::shared_ptr<msLikeliNoCorrel> T( new msLikeliNoCorrel );
                T->initialize();
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msLikeliNoCorrel> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            /*! \brief return the likelihooh
             *
             * This method reimplements the msLikelihood::evaluate() using
             * the assumption of independant model error. The total likelihood
             * is the sum over the scenario and qoi of the individual likelihood
             * return by the method msLikeliNoCorrel::likelihood.
             */
            virtual double evaluate() {
                
	        LOGGER_ENTER_FUNCTION_DBG("virtual double msLikelihood::evaluate()",getFullId());
         
                checkSanity();
                
                boost::shared_ptr<msPredictiveModel>                Model = getPredictiveModel();
                boost::shared_ptr<msMultiDiracs>                 Measures = getMeasures();
                boost::shared_ptr<msSamplerExplicit>             Sampler  = Measures->getSampler();
                boost::shared_ptr<msGeneralizedCoordinates> ScenarioCoors = Measures->getCoordinates();
                
                double likeli = 0;
                
                while( Sampler->getNextPoint(ScenarioCoors) ){
                    
                    Model->predict();
                    map<string,double> Qois = Model->getQois();
		    map<string,double>::iterator it = Qois.begin();
		    
                    for(size_t i=0; it !=Qois.end(); ++it) {
                        
                        likeli -= likelihood( (*it).second , Measures->getValueById((*it).first) , i  );
                    }
                }
                LOGGER_EXIT_FUNCTION2("virtual double msLikelihood::evaluate()");
                return likeli;
            }
            
            /*! \brief used to define the msLikeliNoCorrel::evaluate function
             *
             * This method needs to be defined by derived class. 
             * The method has to return the likelihood when the model predict the value
             * 'prediction' when the measure is 'measure' for the QoI number 'i'. 
             */
            virtual double likelihood(double prediction, double measure, int i) {
                
                IMPACT_THROW_EXCEPTION(msException( "This method is virtual, you need to overide it",
                                              "double msLikeliNoCorrel::evaluate()",
                                              getFullId()
                                              )
                                      );
            };
            
            
            private:
           
        };
    }
    
}
#endif // MSDIRACS_H

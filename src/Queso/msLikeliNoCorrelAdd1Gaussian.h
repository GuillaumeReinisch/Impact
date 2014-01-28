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


#ifndef MSLikeliNoCorrelAdd1Gaussian_H
#define MSLikeliNoCorrelAdd1Gaussian_H

#include <msLikeliNoCorrel.h>

namespace impact {
    
    namespace queso {
        
        using namespace impact::math;
        
        /*! \class msLikeliNoCorrelAdd1Gaussian
         *
         * \brief Likelihood that assumes independant additive 1 parameter gaussian model error
         *
         * The contribution to the likelihood of a particular scenario using the predicted and measured value is defined
         * in msLikeliNoCorrelAdd1Gaussian::likelihood. Due to the assumption it is based one one model error parameter:
         * the width of the gaussian law.
         *
         * To use this class, one just has to specify the maximum of the model error parameter
         * considered, by default it is considered to be one.
         */
        class msLikeliNoCorrelAdd1Gaussian : public msLikeliNoCorrel
        {
            
        protected:
            
            static msRegistrar Registrar;
            static bool  isLikeliNoCorrelAdd1GaussianRegisteredInPython;
            static msRegister* createInstance() { return new msLikeliNoCorrelAdd1Gaussian; }
            
            void registryInPython();
            
        public:
            
            msLikeliNoCorrelAdd1Gaussian();
            
            void update()  {
                
                msLikeliNoCorrel::update();
            };
            
            void initialize() {
                
                msLikeliNoCorrel::initialize();
                boost::shared_ptr<msModelParameter> param = msModelParameter::New();
                param->set(1,0,1,0.01,0.1);
                param->setId("Sigma");
                addModelErrorParameter(param);
            };
            
            static boost::shared_ptr<msLikeliNoCorrelAdd1Gaussian> New(){
                
                boost::shared_ptr<msLikeliNoCorrelAdd1Gaussian> T( new msLikeliNoCorrelAdd1Gaussian );
                T->initialize();
                T->update();
                return T;
            }
            
            virtual boost::shared_ptr<msTreeMapper> clone() {
                
                boost::shared_ptr<msLikeliNoCorrelAdd1Gaussian> clone = New();
                clone->msTreeMapper::operator=(*this);
                return clone;
            }
            
            /*! \brief contibution to the likelihood for a particular predicted and measured QoI
             *
             * Due to the assumption of independant additive 1 parameter gaussian model error,
             * the contribution to the likelihood reads:
             * \f[
             - 0.5 log( 2.0 \pi \sigma^2) - \frac{(qoi-measure)^2}{2 \sigma^2};
             \f]
             * where \sigma is the model error parameter.
             * \param qoi predicted value
             * \param measure measure
             * \param qoi index (not used here)
             */
            virtual double likelihood(double qoi, double measure, int i) {
                
                double sigma2 = pow( getErrorModelParameters()->getVariable(0)->getValue(),2);
                if(sigma2==0){
                    
                    msException e("The model error is equal to zero",
                              "virtual double msLikeliNoCorrelAdd1Gaussian::likelihood(double qoi, double measure, int i)",
                              getFullId());
                    IMPACT_THROW_EXCEPTION(e);
                }
                double error2 = pow(qoi-measure,2);
                double pi = impact::csts::pi;
                
                double likeli = 0.5 * log(2 * pi * sigma2) + error2/(2*sigma2);
				
                return likeli;
            };
            
        };
    }
    
}
#endif // MSDIRACS_H
